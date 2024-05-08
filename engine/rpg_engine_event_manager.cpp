/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "stdafx.h"

#include "rpg_engine_event_manager.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common.h"
#include "common_timer_handler.h"
#include "common_timer_manager.h"

#include "rpg_dice.h"

#include "rpg_map_common_tools.h"

#include "rpg_monster_common.h"
#include "rpg_monster_dictionary.h"

#include "rpg_engine.h"
#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

RPG_Engine_Event_Manager::RPG_Engine_Event_Manager()
 : inherited ()
 , inherited2 (this,
               false)
 , myEngine (NULL)
 , myLock ()
 , myGameClockStart (ACE_Time_Value::zero)
 , myTimers ()
 , myEntityTimers ()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::RPG_Engine_Event_Manager"));

  // set group id for worker thread(s)
  inherited::grp_id (RPG_ENGINE_AI_TASK_GROUP_ID);
}

RPG_Engine_Event_Manager::~RPG_Engine_Event_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::~RPG_Engine_Event_Manager"));

  if (isRunning ())
    stop (true); // locked access ?

  cancel_all ();
}

int
RPG_Engine_Event_Manager::open (void* args_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::open"));

  ACE_UNUSED_ARG (args_in);

  // *IMPORTANT NOTE*: the first time around, the queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state, which
  // is expected.
  // Subsequently (i.e. after stop()ping/start()ing, the queue
  // will have been deactivate()d in the process, and getq() (see svc()) will
  // fail (ESHUTDOWN) --> (re-)activate() the queue !
  // step1: (re-)activate() the queue
  MESSAGE_QUEUE_EX* message_queue_p = inherited::msg_queue ();
  ACE_ASSERT (message_queue_p);
  if (message_queue_p->activate () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  try {
    start ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Engine_Event_Manager::start() method, aborting\n")));
    return -1;
  }

  return 0;
}

int
RPG_Engine_Event_Manager::close (u_long arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::close"));

  // *IMPORTANT NOTE*: this method may be invoked
  // [- by an external thread closing down the active object]
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(%s) worker thread (ID: %t) leaving...\n"),
                  ACE_TEXT (RPG_ENGINE_AI_TASK_THREAD_NAME)));

      // don't do anything...
      break;
    }
    case 1:
    {
      ACE_ASSERT (false);
      ACE_NOTREACHED (break;)
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument (was: %u), returning\n"),
                  arg_in));
      break;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Engine_Event_Manager::svc (void)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0A00) // _WIN32_WINNT_WIN10
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_AI_TASK_THREAD_NAME),
                                     NULL);
#else
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_AI_TASK_THREAD_NAME),
                                     0);
#endif // _WIN32_WINNT_WIN10
#endif // ACE_WIN32 || ACE_WIN64

  int result = -1;
  struct RPG_Engine_Event* event_p = NULL;
  while (true)
  {
    event_p = NULL;

    result = inherited::getq (event_p, NULL);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task_Ex::getq(): \"%m\", aborting\n")));
      return -1;
    } // end IF
    ACE_ASSERT (event_p);

    switch (event_p->type)
    {
      case EVENT_QUIT:
      {
        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("%t: received EVENT_QUIT, leaving...\n")));

        // clean up
        delete event_p;

        return 0;
      }
      default:
      {
        handleEvent (*event_p);

        break;
      }
    } // end SWITCH
  } // end WHILE

  return -1;
}

void
RPG_Engine_Event_Manager::add (RPG_Engine_EntityID_t id_in,
                               const ACE_Time_Value& activationInterval_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::add"));

  // sanity check
  ACE_ASSERT (id_in);
  ACE_ASSERT (activationInterval_in != ACE_Time_Value::zero);

  struct RPG_Engine_Event* event_p = NULL;
  ACE_NEW_NORETURN (event_p,
                    struct RPG_Engine_Event ());
  if (!event_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                sizeof (struct RPG_Engine_Event)));
    return;
  } // end IF
  event_p->type = EVENT_ENTITY_ACTIVATE;
  event_p->entity_id = id_in;
  event_p->timer_id = -1;

  // *NOTE*: fire&forget API for event_p
  long timer_id = schedule (event_p,
                            activationInterval_in,
                            false); // one-shot ?
  if (timer_id == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to schedule event, returning\n")));
    return;
  } // end IF

  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    ACE_ASSERT (myEntityTimers.find (id_in) == myEntityTimers.end ());
    myEntityTimers[id_in] = timer_id;
  } // end lock scope

  // *NOTE*: "%#T" doesn't work correctly 1.111111 --> " 01:00:01.111111"; that's OK...
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("scheduled activation event (ID: %d [%#T]) for entity (ID: %u)\n"),
              timer_id,
              &activationInterval_in,
              id_in));
}

void
RPG_Engine_Event_Manager::remove (RPG_Engine_EntityID_t id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::remove"));

  // sanity check
  ACE_ASSERT (id_in);

  long timer_id = -1;
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    RPG_Engine_EntityTimersConstIterator_t iterator = myEntityTimers.find (id_in);
    ACE_ASSERT (iterator != myEntityTimers.end ());
    timer_id = (*iterator).second;
    myEntityTimers.erase (iterator);
  } // end lock scope

  // cancel corresponding activation timer
  ACE_ASSERT (timer_id > 0);
  cancel (timer_id);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("cancelled activation timer (ID: %d) for entity %u\n"),
              timer_id,
              id_in));
}

void
RPG_Engine_Event_Manager::reschedule (RPG_Engine_EntityID_t id_in,
                                      const ACE_Time_Value& interval_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::reschedule"));

  // sanity check
  ACE_ASSERT (id_in);

  long timer_id = -1;
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    RPG_Engine_EntityTimersConstIterator_t iterator = myEntityTimers.find (id_in);
    ACE_ASSERT (iterator != myEntityTimers.end ());
    //if (iterator == myEntityTimers.end ())
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("invalid entity ID (was: %u), returning\n"),
    //              id_in));
    //  return;
    //} // end IF
    timer_id = (*iterator).second;
  } // end lock scope

  ACE_ASSERT (timer_id != -1);
  COMMON_TIMERMANAGER_SINGLETON::instance ()->reset_timer_interval (timer_id,
                                                                    interval_in);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("reset activation timer interval (ID: %d) for entity %u to \"%#T\"\n"),
              timer_id,
              id_in,
              &interval_in));
}

void
RPG_Engine_Event_Manager::start ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::start"));

  // sanity check
  if (isRunning ())
    return;

  // initialize game clock
  myGameClockStart = COMMON_TIME_POLICY ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started game clock: \"%#D\"\n"),
              &myGameClockStart));

  // OK: start worker thread
  ACE_hthread_t thread_handles[RPG_ENGINE_AI_DEF_NUM_THREADS];
  ACE_OS::memset(thread_handles, 0, sizeof(thread_handles));
  ACE_thread_t thread_ids[RPG_ENGINE_AI_DEF_NUM_THREADS];
  ACE_OS::memset(thread_ids, 0, sizeof(thread_ids));
  const char* thread_names[RPG_ENGINE_AI_DEF_NUM_THREADS];
  char* thread_name = NULL;
  std::string buffer;
  std::ostringstream converter;
  for (unsigned int i = 0;
       i < RPG_ENGINE_AI_DEF_NUM_THREADS;
       i++)
  {
    thread_name = NULL;
    ACE_NEW_NORETURN (thread_name,
                      char[BUFSIZ]);
    if (!thread_name)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, returning\n")));

      // clean up
      for (unsigned int j = 0; j < i; j++)
        delete [] thread_names[j];

      return;
    } // end IF
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (i + 1);
    buffer = ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_AI_TASK_THREAD_NAME);
    buffer += ACE_TEXT_ALWAYS_CHAR (" #");
    buffer += converter.str ();
    ACE_OS::memset (thread_name, 0, sizeof (char[BUFSIZ]));
    ACE_OS::strcpy (thread_name,
                    buffer.c_str ());
    thread_names[i] = thread_name;
  } // end FOR
  // *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
  int result = 0;
  result = inherited::activate ((THR_NEW_LWP       |
                                 THR_JOINABLE      |
                                 THR_INHERIT_SCHED),            // flags
                                 RPG_ENGINE_AI_DEF_NUM_THREADS, // number of threads
                                 0,                             // force spawning
                                 ACE_DEFAULT_THREAD_PRIORITY,   // priority
                                 inherited::grp_id (),          // group id --> has been set (see above)
                                 NULL,                          // corresp. task --> use 'this'
                                 thread_handles,                // thread handle(s)
                                 NULL,                          // thread stack(s)
                                 NULL,                          // thread stack size(s)
                                 thread_ids,                    // thread id(s)
                                 thread_names);                 // thread names(s)
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::activate(): \"%m\", returning\n")));

    // clean up
    for (unsigned int i = 0; i < RPG_ENGINE_AI_DEF_NUM_THREADS; i++)
      delete [] thread_names[i];

    return;
  }

  std::string thread_ids_string;
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  for (unsigned int i = 0;
        i < RPG_ENGINE_AI_DEF_NUM_THREADS;
        i++)
  {
    // clean up
    delete [] thread_names[i];

    converter << ACE_TEXT_ALWAYS_CHAR ("#") << (i + 1)
              << ACE_TEXT_ALWAYS_CHAR (" ")
              << thread_ids[i]
              << ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end FOR
  thread_ids_string = converter.str ();
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) started %u worker thread(s) (group: %d):\n%s"),
              ACE_TEXT (RPG_ENGINE_AI_TASK_THREAD_NAME),
              RPG_ENGINE_AI_DEF_NUM_THREADS,
              inherited::grp_id (),
              ACE_TEXT (thread_ids_string.c_str ())));
}

void
RPG_Engine_Event_Manager::stop (bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::stop"));

  ACE_UNUSED_ARG (lockedAccess_in);

  // sanity check
  if (!isRunning ())
    return;

  // cancel all events
  cancel_all ();

  // stop/fini game clock
  ACE_Time_Value elapsed = COMMON_TIME_POLICY () - myGameClockStart;
  // *TODO*: "%#T" doesn't work correctly 1.111111 --> " 01:00:01.111111"
  // --> that's OK for now...
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stopped game clock: \"%#T\"\n"),
              &elapsed));

  // drop control message(s) into the queue...
  struct RPG_Engine_Event* event_p = NULL;
  for (unsigned int i = 0;
       i < RPG_ENGINE_AI_DEF_NUM_THREADS;
       i++)
  {
    event_p = NULL;
    ACE_NEW_NORETURN (event_p,
                      struct RPG_Engine_Event ());
    if (!event_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("unable to allocate memory, returning\n")));
      return;
    } // end IF
    event_p->type = EVENT_QUIT;
    if (inherited::putq (event_p, NULL) == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", returning\n")));

      // clean up
      delete event_p;

      return;
    } // end IF
  } // end IF

  // ... and wait for the worker thread(s) to join
  if (inherited::wait () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", returning\n")));
    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) worker thread(s) has/have joined...\n"),
              ACE_TEXT (RPG_ENGINE_AI_TASK_THREAD_NAME)));

  // clean up
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    myEntityTimers.clear ();

    for (RPG_Engine_EventTimersConstIterator_t iterator = myTimers.begin ();
         iterator != myTimers.end ();
         iterator++)
      delete (*iterator).second;
    myTimers.clear ();
  } // end lock scope
}

void
RPG_Engine_Event_Manager::dump_state () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::dump_state"));

  // *TODO*
  ACE_ASSERT (false);
}

void
RPG_Engine_Event_Manager::initialize (RPG_Engine* engine_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::initialize"));

  // sanity check(s)
  ACE_ASSERT (engine_in);

  myEngine = engine_in;
}

long
RPG_Engine_Event_Manager::schedule (struct RPG_Engine_Event*& event_inout,
                                    const ACE_Time_Value& interval_in,
                                    bool isOneShot_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::schedule"));

  // sanity check(s)
  ACE_ASSERT (event_inout);
  ACE_ASSERT (interval_in != ACE_Time_Value::zero);

  long timer_id = -1;
  // *IMPORTANT NOTE*: grab the lock BEFORE the event can fire
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, -1); // *TODO*: failure case leaks event_inout
    timer_id =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule_timer (this,                                                 // event handler handle
                                                                  event_inout,                                          // ACT
                                                                  COMMON_TIME_POLICY () + interval_in,                  // first wakeup time
                                                                  (isOneShot_in ? ACE_Time_Value::zero : interval_in)); // interval
    if (timer_id == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to schedule timer, aborting\n")));
      delete event_inout; event_inout = NULL;
      return -1;
    } // end IF
    event_inout->timer_id = timer_id;

    ACE_ASSERT (myTimers.find (timer_id) == myTimers.end ());
    myTimers[timer_id] = event_inout;
  } // end lock scope
  event_inout = NULL;

  return timer_id;
}

void
RPG_Engine_Event_Manager::cancel (long id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::cancel"));

  const void* act_p = NULL;
  if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (id_in,
                                                          &act_p) <= 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                id_in));
  else
  { ACE_ASSERT (act_p);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("cancelled timer (ID: %d)...\n"),
                id_in));
  } // end ELSE

  // clean up
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    RPG_Engine_EventTimersConstIterator_t iterator = myTimers.find (id_in);
    ACE_ASSERT (iterator != myTimers.end ());
    ACE_ASSERT (act_p == (*iterator).second);
    // *WARNING*: cannot free event here (it may be in use)
    (*iterator).second->free = true;
  } // end lock scope
}

void
RPG_Engine_Event_Manager::cancel_all ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::cancel_all"));

  const void* act_p = NULL;

  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    for (RPG_Engine_EventTimersConstIterator_t iterator = myTimers.begin ();
         iterator != myTimers.end ();
         iterator++)
    { // sanity check(s)
      if ((*iterator).second->free) // already cancelled ?
        continue;

      act_p = NULL;
      if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel ((*iterator).first,
                                                              &act_p) <= 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    (*iterator).first));
      else
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("cancelled timer (ID: %d)...\n"),
                    (*iterator).first));
      ACE_ASSERT (act_p == (*iterator).second);
      // *WARNING*: cannot free event here (it may be in use)
      (*iterator).second->free = true;
    } // end IF
  } // end lock scope
}

void
RPG_Engine_Event_Manager::handleEvent (const struct RPG_Engine_Event& event_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::handleEvent"));

  switch (event_in.type)
  {
    case EVENT_ENTITY_ACTIVATE:
    { ACE_ASSERT (event_in.entity_id);

      struct RPG_Engine_Action next_action;
      next_action.command = RPG_ENGINE_COMMAND_INVALID;
      next_action.position =
          std::make_pair (std::numeric_limits<unsigned int>::max (),
                          std::numeric_limits<unsigned int>::max ());
      next_action.target = 0;

      myEngine->lock ();
      // step1: check pending action (if any)
      RPG_Engine_EntitiesIterator_t iterator =
        myEngine->entities_.find (event_in.entity_id);
      if (iterator == myEngine->entities_.end ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid entity ID (was: %u), returning\n"),
                    event_in.entity_id));
        myEngine->unlock ();
        break;
      } // end IF

      // idle monster ? --> choose strategy
      if (!(*iterator).second->character->isPlayerCharacter ())
      {
        bool is_idle =
          ((*iterator).second->actions.empty () ||
           ((*iterator).second->actions.front ().command == COMMAND_IDLE));
        if (is_idle)
        {
          // step1: sort targets by distance
          RPG_Engine_EntityList_t possible_targets =
              myEngine->entities ((*iterator).second->position,
                                  false);

          // step2: remove self
          RPG_Engine_EntityListIterator_t iterator_2 =
            std::find (possible_targets.begin (), possible_targets.end (), event_in.entity_id);
          ACE_ASSERT (iterator_2 != possible_targets.end ());
          possible_targets.erase (iterator_2);

          // step3: remove fellow monsters
          struct entity_remove monster_remove_s = {myEngine, // engine handle
                                                   false,    // locked access ?
                                                   true};    // --> remove monsters
          possible_targets.remove_if (monster_remove_s);

          // step4: remove invisible (== cannot (currently) see) targets
          struct invisible_remove invisible_remove_s =
            {myEngine,            // engine handle
             false,               // locked access ?
             event_in.entity_id}; // entity id
          possible_targets.remove_if (invisible_remove_s);

          if (possible_targets.empty ())
          {
            //ACE_DEBUG((LM_DEBUG,
            //           ACE_TEXT("no target, continuing\n")));

            // --> no active player(s), amble about...
            next_action.command = COMMAND_IDLE;
          } // end IF
          else
          {
            // start attacking/pursuing...
            next_action.command = COMMAND_ATTACK;
            // *TODO*: implement more strategy here (strongest/weakest target,
            // ...)
            next_action.target = possible_targets.front ();
          } // end ELSE

          // transition idle --> attack ?
          if (!(*iterator).second->actions.empty () &&
              (next_action.command == COMMAND_ATTACK))
            (*iterator).second->actions.pop_front (); // pop idle action

          if ((*iterator).second->actions.empty () ||
              (next_action.command != COMMAND_IDLE))
            (*iterator).second->actions.push_back (next_action);
        } // end IF
      } // end IF

      // idle ? --> nothing to do this round then...
      if ((*iterator).second->actions.empty ())
      {
        myEngine->unlock ();
        break;
      } // end IF

      // fighting / travelling ?
      struct RPG_Engine_Action& current_action = (*iterator).second->actions.front ();
      bool done_current_action = false;
      bool do_next_action = false;
      switch (current_action.command)
      {
        case COMMAND_ATTACK:
        { ACE_ASSERT (current_action.target);
          RPG_Engine_EntitiesConstIterator_t target =
              myEngine->entities_.find (current_action.target);
          if ((target == myEngine->entities_.end ()) ||
              RPG_Engine_Common_Tools::isCharacterDisabled ((*target).second->character)) // target disabled ?
          {
            (*iterator).second->modes.erase (ENTITYMODE_FIGHTING);
            done_current_action = true;

            break; // nothing to do...
          } // end IF

          (*iterator).second->modes.insert (ENTITYMODE_FIGHTING);

          // adjacent ? --> start attack !
          // *TODO*: compute standard/fullround
          do_next_action = true;
          if (myEngine->canReach (event_in.entity_id,
                                  (*target).second->position,
                                  false))
          {
            // (try to) attack the opponent
            next_action.command = COMMAND_ATTACK_FULL;
            next_action.target = current_action.target;
          } // end IF
          else
          {
            // (try to) travel there first...
            next_action.command = COMMAND_TRAVEL;
            next_action.target = current_action.target;
          } // end ELSE

          break;
        }
        case COMMAND_IDLE:
        {
          // amble a little bit ?
          bool do_amble = false;
          if (!(*iterator).second->character->isPlayerCharacter ())
          { // *NOTE*: currently, only spawned monsters amble about...
            struct RPG_Engine_LevelMetaData level_metadata =
                myEngine->getMetaData (false);
            for (RPG_Engine_SpawnsConstIterator_t iterator2 = level_metadata.spawns.begin ();
                 iterator2 != level_metadata.spawns.end ();
                 iterator2++)
              if ((*iterator2).spawn.type == (*iterator).second->character->getName ())
              {
                do_amble =
                  RPG_Dice::probability ((*iterator2).spawn.amble_probability);
                break;
              } // end IF
          } // end IF
          if (!do_amble)
            break; // no(t this time)...

          // OK: amble about !
          do_next_action = true;
          next_action.command = COMMAND_STEP;
          // choose random direction
          enum RPG_Map_Direction direction;
          RPG_Dice_RollResult_t roll_result;
          do
          {
            direction = RPG_MAP_DIRECTION_INVALID;
            roll_result.clear ();
            RPG_Dice::generateRandomNumbers (RPG_MAP_DIRECTION_MAX,
                                             1,
                                             roll_result);
            direction = static_cast<enum RPG_Map_Direction> (roll_result.front () - 1);
            ACE_ASSERT (direction < RPG_MAP_DIRECTION_MAX);

            next_action.position = myEngine->getPosition (event_in.entity_id,
                                                          false); // lock access ?
            switch (direction)
            {
              case DIRECTION_UP:
                next_action.position.second--; break;
              case DIRECTION_RIGHT:
                next_action.position.first++; break;
              case DIRECTION_DOWN:
                next_action.position.second++; break;
              case DIRECTION_LEFT:
                next_action.position.first--; break;
              default:
              {
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("invalid direction (was: \"%s\"), continuing\n"),
                            ACE_TEXT (RPG_Map_DirectionHelper::RPG_Map_DirectionToString (direction).c_str())));
                break;
              }
            } // end SWITCH
          } while (!myEngine->isValid (next_action.position,
                                       false)); // lock access ?

          break;
        }
        case COMMAND_TRAVEL:
        {
          // --> (try to) take the first/next step along a path...

          // sanity check
          if (current_action.position == (*iterator).second->position)
          {
            (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
            done_current_action = true;
            break; // nothing (more) to do...
          } // end IF
          ACE_ASSERT (current_action.position != (*iterator).second->position);

          // step0: chasing a target ?
          if (current_action.target)
          {
            // determine target position
            RPG_Engine_EntitiesConstIterator_t target =
              myEngine->entities_.find (current_action.target);
            if (target == myEngine->entities_.end ())
            {
              // *NOTE*: --> target has gone...
              (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
              done_current_action = true;
              break; // nothing (more) to do...
            } // end IF

            current_action.position = (*target).second->position;

            if (RPG_Map_Common_Tools::isAdjacent ((*iterator).second->position,
                                                  current_action.position))
            {
              // *NOTE*: --> reached target...
              (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
              done_current_action = true;
              break; // nothing (more) to do...
            } // end IF
            else if (!myEngine->canSee (event_in.entity_id,
                                        current_action.target,
                                        false)) // lock access ?
            {
              // *NOTE*: --> lost sight of target; stop chasing...
              (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
              done_current_action = true;
              break; // nothing (more) to do...
            } // end ELSE IF
          } // end IF

          // step1: compute path first/again ?
          if (current_action.path.empty () ||
              (current_action.path.back ().first != current_action.position)) // pursuit mode...
          {
            current_action.path.clear ();
            RPG_Map_Positions_t obstacles = myEngine->getObstacles (true,   // include entities
                                                                    false); // don't lock
            if (!myEngine->findPath ((*iterator).second->position,
                                     current_action.position,
                                     obstacles,
                                     current_action.path))
            {
              // *NOTE*: --> no/invalid path, cannot proceed...
              (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
              done_current_action = true;
              break; // stop & cancel path...
            } // end IF
            ACE_ASSERT (!current_action.path.empty ());
            ACE_ASSERT ((current_action.path.front ().first == (*iterator).second->position) && (current_action.path.back ().first == current_action.position));
            current_action.path.pop_front ();

            (*iterator).second->modes.insert (ENTITYMODE_TRAVELLING);
          } // end IF
          ACE_ASSERT (!current_action.path.empty ());

          // step2: (try to) step to the next adjacent position
          next_action.command = COMMAND_STEP;
          next_action.position = current_action.path.front ().first;
          ACE_ASSERT (RPG_Map_Common_Tools::isAdjacent ((*iterator).second->position, next_action.position));
          do_next_action = true;

          // step3: check: done ?
          current_action.path.pop_front ();
          done_current_action = current_action.path.empty ();
          if (done_current_action)
          { ACE_ASSERT (next_action.position == current_action.position);
            // *NOTE*: --> reaching target...
            (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
          } // end IF

          break;
        }
        default:
        {
          // the engine executes these...
          myEngine->msg_queue ()->pulse ();

          break;
        }
      } // end SWITCH

      if (done_current_action)
        (*iterator).second->actions.pop_front ();
      if (do_next_action)
        (*iterator).second->actions.push_front (next_action);

      myEngine->unlock ();

      break;
    }
    case EVENT_ENTITY_SPAWN:
    {
      myEngine->lock ();
      struct RPG_Engine_LevelMetaData level_metadata =
          myEngine->getMetaData (false);
      RPG_Engine_SpawnsConstIterator_t iterator =
          level_metadata.spawns.begin ();
      for (;
           iterator != level_metadata.spawns.end ();
           iterator++)
        if (event_in.timer_id == (*iterator).timer_id)
          break;
      // spawn an instance ?
      if ((iterator == level_metadata.spawns.end ())                          || // no spawns on this level(/invalid spawn timer id) ?
          (myEngine->numSpawned (ACE_TEXT_ALWAYS_CHAR (""),
                                 false) >= level_metadata.max_num_spawned)    || // too many spawns/level ?
          (myEngine->numSpawned ((*iterator).spawn.type,
                                 false) >= (*iterator).spawn.max_num_spawned) || // too many spawns/type ?
          !RPG_Dice::probability ((*iterator).spawn.probability))                // spawn an instance ?
      {
        myEngine->unlock ();
        break; // no(t this time)...
      } // end IF

      // OK: spawn an instance
      struct RPG_Engine_Entity* entity = NULL;
      ACE_NEW_NORETURN (entity,
                        struct RPG_Engine_Entity ());
      if (!entity)
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("unable to allocate memory(%u), continuing\n"),
                    sizeof (struct RPG_Engine_Entity)));
        myEngine->unlock ();
        break;
      } // end IF
      // *TODO*: define max HP, treasure (gold, items, ...), spells
      ACE_UINT16 max_hitpoints = 0;
      ACE_UINT64 gold = 0;
      RPG_Item_List_t items;
      RPG_Character_Conditions_t condition;
      condition.insert (CONDITION_NORMAL);
      ACE_INT16 hitpoints = std::numeric_limits<ACE_INT16>::max (); // spawns start healthy
      RPG_Magic_Spells_t spells;
      *entity = RPG_Engine_Common_Tools::createEntity ((*iterator).spawn.type,
                                                       max_hitpoints,
                                                       gold,
                                                       items,
                                                       condition,
                                                       hitpoints,
                                                       spells);
      if (!entity->character)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Engine_Common_Tools::createEntity(\"%s\"), continuing\n"),
                    ACE_TEXT ((*iterator).spawn.type.c_str ())));
        delete entity;
        myEngine->unlock ();
        break;
      } // end IF
      entity->is_spawned = true;

      // choose random entry point
      RPG_Map_Positions_t seed_points = myEngine->getSeedPoints (false);
      if (seed_points.empty ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("current map has no seed points, continuing\n")));
        delete entity->character; delete entity;
        myEngine->unlock ();
        break;
      } // end IF
      RPG_Map_PositionsConstIterator_t iterator2 = seed_points.begin ();
      RPG_Dice_RollResult_t roll_result;
      RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (seed_points.size ()),
                                       1,
                                       roll_result);
      std::advance (iterator2, roll_result.front () - 1);

      // find empty position around seed point, if necessary
      entity->position = myEngine->findValid (*iterator2,
                                              0,
                                              false); // lock access ?
      if (entity->position == std::make_pair (std::numeric_limits<unsigned int>::max (),
                                              std::numeric_limits<unsigned int>::max ()))
      {
        // *NOTE*: --> level map must be full ?
        delete entity->character; delete entity;
        myEngine->unlock ();
        break;
      } // end IF

      RPG_Engine_EntityID_t id = myEngine->add (entity,
                                                false); // locked access ?
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("spawned \"%s\" [id: %u] @ [%u,%u]...\n"),
                  ACE_TEXT ((*iterator).spawn.type.c_str ()),
                  id,
                  entity->position.first, entity->position.second));
      myEngine->unlock ();
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                  event_in.type));
      break;
    }
  } // end SWITCH

  // clean up ?
  if (RPG_Engine_Common_Tools::isOneShotEvent (event_in.type))
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
    RPG_Engine_EventTimersConstIterator_t iterator = myTimers.find (event_in.timer_id);
    ACE_ASSERT (iterator != myTimers.end ());
    delete (*iterator).second; // free one-shot events here
    myTimers.erase ((*iterator).first);
  } // end IF | lock scope
}

void
RPG_Engine_Event_Manager::handle (const void* act_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::handle"));

  // sanity check(s)
  struct RPG_Engine_Event* event_p =
    reinterpret_cast<struct RPG_Engine_Event*> (const_cast<void*> (act_in));
  ACE_ASSERT (event_p);

  if (inherited::putq (event_p, NULL) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", returning\n")));

    // clean up ?
    if (RPG_Engine_Common_Tools::isOneShotEvent (event_p->type))
    { ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);
      RPG_Engine_EventTimersConstIterator_t iterator = myTimers.find (event_p->timer_id);
      if (iterator == myTimers.end ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid timer id (was: %d), returning\n"),
                    event_p->timer_id));
        return;
      } // end IF

      delete (*iterator).second;
      myTimers.erase ((*iterator).first);
    } // end IF && lock scope

    return;
  } // end IF
}

//////////////////////////////////////////

bool
RPG_Engine_Event_Manager::entity_remove::operator() (const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::entity_remove::operator()"));

  // sanity check(s)
  ACE_ASSERT (engine);

  bool is_monster = engine->isMonster (id_in,
                                       locked_access);

  return (remove_monsters ? is_monster : !is_monster);
}

bool
RPG_Engine_Event_Manager::invisible_remove::operator() (const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine_Event_Manager::invisible_remove::operator()"));

  // sanity check(s)
  ACE_ASSERT (engine);
  ACE_ASSERT (entity_id);

  return !engine->canSee (entity_id,
                          id_in,
                          locked_access);
}
