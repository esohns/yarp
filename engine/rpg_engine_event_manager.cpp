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

#include "rpg_engine_defines.h"
#include "rpg_engine_common.h"
#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"

#include <rpg_map_common_tools.h>

#include <rpg_monster_common.h>
#include <rpg_monster_dictionary.h>

#include <rpg_common_macros.h>
#include <rpg_common_timerhandler.h>
#include <rpg_common_timer_manager.h>

#include <rpg_dice.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

RPG_Engine_Event_Manager::RPG_Engine_Event_Manager()
 : myEngine(NULL) //,
//   myTimers(),
//   myEntityTimers()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::RPG_Engine_Event_Manager"));

  // set group ID for worker thread(s)
  inherited::grp_id(RPG_ENGINE_DEF_AI_TASK_GROUP_ID);
}

RPG_Engine_Event_Manager::~RPG_Engine_Event_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::~RPG_Engine_Event_Manager"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // *WARNING*: the timers are cleaned up by the timer manager (common.dll)...
  for (RPG_Engine_EventTimersConstIterator_t iterator = myTimers.begin();
       iterator != myTimers.end();
       iterator++)
  {
    //act = NULL;
    //if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel((*iterator).first, &act) <= 0)
    //  ACE_DEBUG((LM_DEBUG,
    //             ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
    //             (*iterator).first));
    //ACE_ASSERT(act == (*iterator).second);
    delete (*iterator).second;
  } // end FOR
}

int
RPG_Engine_Event_Manager::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::open"));

  ACE_UNUSED_ARG(args_in);

  // *IMPORTANT NOTE*: the first time around, the queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state, which
  // is expected.
  // Subsequently (i.e. after stop()ping/start()ing, the queue
  // will have been deactivate()d in the process, and getq() (see svc()) will fail
  // (ESHUTDOWN) --> (re-)activate() the queue !
  // step1: (re-)activate() the queue
  if (inherited::msg_queue()->activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));

    return -1;
  } // end IF

  try
  {
    start();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in start() method, aborting\n")));

    // what else can we do here ?
    return -1;
  }

  return 0;
}

int
RPG_Engine_Event_Manager::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::close"));

  // *IMPORTANT NOTE*: this method may be invoked
  // [- by an external thread closing down the active object]
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("(AI engine) worker thread (ID: %t) leaving...\n")));

      // don't do anything...
      break;
    }
    case 1:
    {
      ACE_NOTREACHED(ACE_TEXT("should never get here\n"));
      ACE_ASSERT(false);

      // what else can we do ?
      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument (was: %u), returning\n"),
                 arg_in));

      // what else can we do ?
      break;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Engine_Event_Manager::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::svc"));

  ACE_Message_Block* ace_mb          = NULL;
  ACE_Time_Value     peek_delay(0, (RPG_ENGINE_DEF_EVENT_PEEK_INTERVAL * 1000));
  ACE_Time_Value     delay;
  bool               stop_processing = false;

  while (!stop_processing)
  {
    delay = (ACE_OS::gettimeofday() + peek_delay);
    if (inherited::getq(ace_mb, &delay) != -1)
    {
      switch (ace_mb->msg_type())
      {
        // currently, we only use these...
        case ACE_Message_Block::MB_STOP:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("received MB_STOP...\n")));

          stop_processing = true;

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("[%t]: received an unknown control message (type: %d), continuing\n"),
                     ace_mb->msg_type()));

          break;
        }
      } // end SWITCH

      // clean up
      ace_mb->release();
      ace_mb = NULL;

      if (stop_processing)
        return 0;
    } // end IF

    //// trigger (one round of) entity actions (where appropriate)
    //handleEntities();
  } // end WHILE

  ACE_NOTREACHED(ACE_TEXT("not reached"));
  ACE_ASSERT(false);

  return -1;
}

void
RPG_Engine_Event_Manager::add(const RPG_Engine_EntityID_t& id_in,
                              const ACE_Time_Value& activationInterval_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::add"));

  // sanity check
  ACE_ASSERT(id_in);
  ACE_ASSERT(activationInterval_in != ACE_Time_Value::zero);

  RPG_Engine_Event activate_event;
  activate_event.type = EVENT_ENTITY_ACTIVATE;
  activate_event.entity_id = id_in;
  long timer_id = -1;
  timer_id = schedule(activate_event,
                      activationInterval_in,
                      false);
  ACE_ASSERT(timer_id != -1);
  if (timer_id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to schedule activation timer, aborting\n")));

    return;
  } // end IF

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    ACE_ASSERT(myEntityTimers.find(id_in) == myEntityTimers.end());
    myEntityTimers[id_in] = timer_id;
  } // end lock scope

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("scheduled activation timer (ID: %d [%#T]) for entity %u\n"),
             timer_id,
             &activationInterval_in,
             id_in));
}

void
RPG_Engine_Event_Manager::remove(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::remove"));

  // sanity check
  ACE_ASSERT(id_in);

  long timer_id = -1;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Engine_EntityTimersConstIterator_t iterator = myEntityTimers.find(id_in);
    ACE_ASSERT(iterator != myEntityTimers.end());
    if (iterator == myEntityTimers.end())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
                 id_in));

      return;
    } // end IF
    timer_id = (*iterator).second;
    ACE_ASSERT(timer_id != -1);

    myEntityTimers.erase(iterator);
  } // end lock scope

  // cancel corresponding activation timer
  remove(timer_id);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("cancelled activation timer (ID: %d) for entity %u\n"),
             timer_id,
             id_in));
}

void
RPG_Engine_Event_Manager::reschedule(const RPG_Engine_EntityID_t& id_in,
                                     const ACE_Time_Value& activationInterval_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::reschedule"));

  // sanity check
  ACE_ASSERT(id_in);

  long timer_id = -1;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Engine_EntityTimersConstIterator_t iterator = myEntityTimers.find(id_in);
    ACE_ASSERT(iterator != myEntityTimers.end());
    if (iterator == myEntityTimers.end())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
                 id_in));

      return;
    } // end IF
    timer_id = (*iterator).second;
    ACE_ASSERT(timer_id != -1);

    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->resetInterval(timer_id, activationInterval_in);
  } // end lock scope

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("reset timer interval (ID: %d) for entity %u\n"),
  //           timer_id,
  //           id_in));
}

void
RPG_Engine_Event_Manager::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::start"));

  // init game clock
  myGameClockStart = ACE_OS::gettimeofday();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started game clock: \"%#D\"\n"),
             &myGameClockStart));

  //// sanity check
  //if (isRunning())
  //  return;

  //// OK: start worker thread
  //ACE_thread_t thread_ids[1];
  //thread_ids[0] = 0;
  //ACE_hthread_t thread_handles[1];
  //thread_handles[0] = 0;

  //// *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
  //int ret = 0;
  //ret = inherited::activate((THR_NEW_LWP |
  //                           THR_JOINABLE |
  //                           THR_INHERIT_SCHED),         // flags
  //                          1,                           // number of threads
  //                          0,                           // force spawning
  //                          ACE_DEFAULT_THREAD_PRIORITY, // priority
  //                          inherited::grp_id(),         // group id --> has been set (see above)
  //                          NULL,                        // corresp. task --> use 'this'
  //                          thread_handles,              // thread handle(s)
  //                          NULL,                        // thread stack(s)
  //                          NULL,                        // thread stack size(s)
  //                          thread_ids);                 // thread id(s)
  //if (ret == -1)
  //  ACE_DEBUG((LM_ERROR,
  //             ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));
  //else
  //  ACE_DEBUG((LM_DEBUG,
  //             ACE_TEXT("(AI engine) started worker thread (group: %d, id: %u)...\n"),
  //             inherited::grp_id(),
  //             thread_ids[0]));
}

void
RPG_Engine_Event_Manager::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::stop"));

  // stop/fini game clock
  ACE_Time_Value elapsed = ACE_OS::gettimeofday() - myGameClockStart;
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("stopping game clock: \"%#T\"\n"),
             &elapsed));

  //// sanity check
  //if (!isRunning())
  //  return;

  //// drop control message into the queue...
  //ACE_Message_Block* stop_mb = NULL;
  //stop_mb = new(std::nothrow) ACE_Message_Block(0,                                  // size
  //                                              ACE_Message_Block::MB_STOP,         // type
  //                                              NULL,                               // continuation
  //                                              NULL,                               // data
  //                                              NULL,                               // buffer allocator
  //                                              NULL,                               // locking strategy
  //                                              ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
  //                                              ACE_Time_Value::zero,               // execution time
  //                                              ACE_Time_Value::max_time,           // deadline time
  //                                              NULL,                               // data block allocator
  //                                              NULL);                              // message allocator
  //if (!stop_mb)
  //{
  //  ACE_DEBUG((LM_CRITICAL,
  //             ACE_TEXT("unable to allocate memory, returning\n")));

  //  // *TODO*: what else can we do ?
  //  return;
  //} // end IF

  //// block, if necessary
  //if (inherited::putq(stop_mb, NULL) == -1)
  //{
  //  ACE_DEBUG((LM_ERROR,
  //             ACE_TEXT("failed to ACE_Task::putq(): \"%m\", returning\n")));

  //  // clean up, what else can we do ?
  //  stop_mb->release();

  //  return;
  //} // end IF

  //// ... and wait for the worker thread to join
  //if (inherited::wait() == -1)
  //{
  //  ACE_DEBUG((LM_ERROR,
  //             ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", returning\n")));

  //  return;
  //} // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("worker thread has joined...\n")));
}

bool
RPG_Engine_Event_Manager::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::isRunning"));

  return (inherited::thr_count() > 0);
}

void
RPG_Engine_Event_Manager::wait_all()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::wait_all"));

  // ... wait for ALL worker(s) to join
  if (ACE_Thread_Manager::instance()->wait_grp(RPG_ENGINE_DEF_AI_TASK_GROUP_ID) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Manager::wait_grp(): \"%m\", returning\n")));
}

void
RPG_Engine_Event_Manager::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::dump_state"));

  ACE_ASSERT(false);
}

void
RPG_Engine_Event_Manager::init(RPG_Engine* engine_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::init"));

  ACE_ASSERT(engine_in);

  myEngine = engine_in;
}

long
RPG_Engine_Event_Manager::schedule(const RPG_Engine_Event& event_in,
                                   const ACE_Time_Value& interval_in,
                                   const bool& isOneShot_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::schedule"));

  // sanity check
  ACE_ASSERT(interval_in != ACE_Time_Value::zero);
  if (interval_in == ACE_Time_Value::zero)
    return -1; // nothing to do...

  RPG_Common_TimerHandler* timer_handler = NULL;
  timer_handler = new(std::nothrow) RPG_Common_TimerHandler(this);
  if (!timer_handler)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate RPG_Common_TimerHandler, aborting\n")));

    return -1;
  } // end IF

  RPG_Engine_Event* event_handle = NULL;
  event_handle = new(std::nothrow) RPG_Engine_Event;
  if (!event_handle)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate RPG_Common_TimerHandler, aborting\n")));

    // clean up
    delete timer_handler;

    return -1;
  } // end IF
  *event_handle = event_in;

  long timer_id = -1;
  // *NOTE*: this is a fire-and-forget API (assumes resp. for timer_handler)...
  timer_id = RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(timer_handler,
                                                                     event_handle,
                                                                     ACE_OS::gettimeofday () + interval_in,
                                                                     (isOneShot_in ? ACE_Time_Value::zero
                                                                                   : interval_in));
  if (timer_id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to schedule timer, aborting\n")));

    // clean up
    delete timer_handler;
    delete event_handle;

    return -1;
  } // end IF

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myTimers[timer_id] = event_handle;

  return timer_id;
}

void
RPG_Engine_Event_Manager::remove(const long& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::remove"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EventTimersConstIterator_t iterator = myTimers.find(id_in);
  ACE_ASSERT(iterator != myTimers.end());
  if (iterator == myTimers.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid id (was: %d), aborting\n"),
               id_in));

    return;
  } // end IF

  const void* act = NULL;
  if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(id_in, &act) <= 0)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
               id_in));
  ACE_ASSERT(act == (*iterator).second);
  delete (*iterator).second;
  myTimers.erase(iterator);
}

void
RPG_Engine_Event_Manager::handleTimeout(const void* act_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::handleTimeout"));

  ACE_ASSERT(myEngine);
  ACE_ASSERT(act_in);
  const RPG_Engine_Event* event_handle = reinterpret_cast<const RPG_Engine_Event*>(act_in);

  switch (event_handle->type)
  {
    case EVENT_ENTITY_ACTIVATE:
    {
      ACE_ASSERT(event_handle->entity_id);

      RPG_Engine_Action next_action;
      next_action.command = RPG_ENGINE_COMMAND_INVALID;
      next_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                            std::numeric_limits<unsigned int>::max());
      next_action.path.clear();
      next_action.target = 0;

      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myEngine->myLock);

        // step1: check pending action (if any)
        RPG_Engine_EntitiesIterator_t iterator = myEngine->myEntities.find(event_handle->entity_id);
        ACE_ASSERT(iterator != myEngine->myEntities.end());
        if (iterator == myEngine->myEntities.end())
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
                     event_handle->entity_id));

           return;
        } // end IF

        // idle monster ? --> choose strategy
        if (!(*iterator).second->character->isPlayerCharacter())
        {
          bool is_idle = (*iterator).second->actions.empty();
          if (!is_idle)
            is_idle = ((*iterator).second->actions.front().command == COMMAND_IDLE);
          if (is_idle)
          {
            // step1: sort targets by distance
            RPG_Engine_EntityList_t possible_targets = myEngine->entities((*iterator).second->position);
            ACE_ASSERT(!possible_targets.empty() &&
                       (possible_targets.front() == (*iterator).first));
            possible_targets.erase(possible_targets.begin());

            // step2: remove fellow monsters
            monster_remove_t monster_remove = {myEngine, false, true};
            possible_targets.remove_if(monster_remove);

            // step3: remove invisible (== cannot (currently) see) targets
            invisible_remove_t invisible_remove = {myEngine, false, (*iterator).first};
            possible_targets.remove_if(invisible_remove);

            if (possible_targets.empty())
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
              // *TODO*: implement more strategy here (strongest/weakest target, ...)
              next_action.target = possible_targets.front();
            } // end ELSE

            // transition idle --> attack ?
            if (!(*iterator).second->actions.empty() &&
                (next_action.command == COMMAND_ATTACK))
              (*iterator).second->actions.pop_front();

            if ((*iterator).second->actions.empty() ||
                (next_action.command != COMMAND_IDLE))
              (*iterator).second->actions.push_back(next_action);
          } // end IF
        } // end IF

        // idle ? --> nothing to do this round then...
        if ((*iterator).second->actions.empty())
          break;

        // fighting / travelling ?
        RPG_Engine_Action& current_action = (*iterator).second->actions.front();
        bool done_current_action = false;
        bool do_next_action = false;
        switch (current_action.command)
        {
          case COMMAND_ATTACK:
          {
            ACE_ASSERT(current_action.target);
            RPG_Engine_EntitiesConstIterator_t target = myEngine->myEntities.find(current_action.target);
            if ((target == myEngine->myEntities.end()) ||
                RPG_Engine_Common_Tools::isCharacterDisabled((*target).second->character)) // target disabled ?
            {
              (*iterator).second->modes.erase(ENTITYMODE_FIGHTING);
              done_current_action = true;

              break; // nothing to do...
            } // end IF

            (*iterator).second->modes.insert(ENTITYMODE_FIGHTING);

            // adjacent ? --> start attack !
            // *TODO*: compute standard/fullround
            do_next_action = true;
            if (myEngine->canReach(event_handle->entity_id,
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
            // amble around a little bit...
            const RPG_Engine_LevelMeta_t& level_meta = myEngine->getMeta(false);
            if (!RPG_Dice::probability(level_meta.amble_probability))
              break; // not this time...
            do_next_action = true;

            next_action.command = COMMAND_STEP;
            // choose random direction
            RPG_Map_Direction direction;
            RPG_Dice_RollResult_t roll_result;
            do
            {
              direction = DIRECTION_INVALID;
              roll_result.clear();
              RPG_Dice::generateRandomNumbers(DIRECTION_MAX,
                                              1,
                                              roll_result);
              direction = static_cast<RPG_Map_Direction>(roll_result.front() - 1);
              ACE_ASSERT(direction < DIRECTION_MAX);

              next_action.position = myEngine->getPosition((*iterator).first, false);
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
                  ACE_DEBUG((LM_ERROR,
                             ACE_TEXT("invalid direction (was: %u), aborting\n"),
                             direction));

                  break;
                }
              } // end SWITCH
            } while (!myEngine->isValid(next_action.position, false));

            break;
          }
          case COMMAND_TRAVEL:
          {
            // --> (try to) take the first/next step along a path...

            // sanity check
            if (current_action.position == (*iterator).second->position)
            {
              (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);
              done_current_action = true;
              break; // nothing (more) to do...
            } // end IF

            // step0: chasing a target ?
            if (current_action.target)
            {
              // determine target position
              RPG_Engine_EntitiesConstIterator_t target = myEngine->myEntities.end();
              target = myEngine->myEntities.find(current_action.target);
              if (target == myEngine->myEntities.end())
              {
                // *NOTE*: --> target has gone...
                (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);
                done_current_action = true;
                break; // nothing (more) to do...
              } // end IF

              current_action.position = (*target).second->position;

              if (RPG_Map_Common_Tools::isAdjacent((*iterator).second->position,
                                                   current_action.position))
              {
                // *NOTE*: --> reached target...
                (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);
                done_current_action = true;
                break; // nothing (more) to do...
              } // end IF
            } // end IF
            ACE_ASSERT(current_action.position != (*iterator).second->position);

            // step1: compute path first/again ?
            if (current_action.path.empty() ||
                (current_action.path.back().first != current_action.position)) // pursuit mode...
            {
              current_action.path.clear();

              // obstacles:
              // - walls
              // - (closed, locked) doors
              // - entities
              RPG_Map_Positions_t obstacles = myEngine->getObstacles(true);
              // - start, end positions never are obstacles...
              obstacles.erase((*iterator).second->position);
              obstacles.erase(current_action.position);

              if (!myEngine->findPath((*iterator).second->position,
                                      current_action.position,
                                      obstacles,
                                      current_action.path))
              {
                // *NOTE*: --> no/invalid path, cannot proceed...
                (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);
                done_current_action = true;
                break; // stop & cancel path...
              } // end IF
              ACE_ASSERT(!current_action.path.empty());
              ACE_ASSERT((current_action.path.front().first == (*iterator).second->position) &&
                         (current_action.path.back().first == current_action.position));
              current_action.path.pop_front();

              (*iterator).second->modes.insert(ENTITYMODE_TRAVELLING);
            } // end IF
            ACE_ASSERT(!current_action.path.empty());

            // step2: (try to) step to the next adjacent position
            next_action.command = COMMAND_STEP;
            next_action.position = current_action.path.front().first;
            do_next_action = true;

            current_action.path.pop_front();

            break;
          }
          default:
            break; // the engine executes these...
        } // end SWITCH

        if (done_current_action)
          (*iterator).second->actions.pop_front();
        if (do_next_action)
          (*iterator).second->actions.push_front(next_action);
      } // end lock scope

      break;
    }
    case EVENT_ENTITY_SPAWN:
    {
      const RPG_Engine_LevelMeta_t& level_meta = myEngine->getMeta(true);

      if (level_meta.roaming_monsters.empty()                  ||
          (myEngine->numSpawned() >= level_meta.max_spawned)   ||
          !RPG_Dice::probability(level_meta.spawn_probability))
        break; // not this time...

      // OK: spawn an instance
      RPG_Engine_Entity* entity = NULL;
      entity = new(std::nothrow) RPG_Engine_Entity;
      if (!entity)
      {
        ACE_DEBUG((LM_CRITICAL,
                   ACE_TEXT("unable to allocate memory, aborting\n")));

        return;
      } // end IF

      RPG_Dice_RollResult_t roll_result;
      std::string monster_type;
      if (level_meta.roaming_monsters.size() == 1)
        monster_type = level_meta.roaming_monsters.front();
      else
      {
        // choose a random type
        RPG_Monster_ListConstIterator_t iterator = level_meta.roaming_monsters.begin();
        RPG_Dice::generateRandomNumbers(level_meta.roaming_monsters.size(),
                                        1,
                                        roll_result);
        std::advance(iterator, roll_result.front() - 1);
        monster_type = *iterator;
      } // end ELSE

      *entity = RPG_Engine_Common_Tools::createEntity(monster_type);
      entity->is_spawned = true;
      ACE_ASSERT(entity->character);
      if (!entity->character)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Engine_Common_Tools::createEntity, aborting\n")));

        // clean up
        delete entity;

        return;
      } // end IF

      // choose random entry point
      const RPG_Map_Positions_t& seed_points = myEngine->getSeedPoints();
      ACE_ASSERT(!seed_points.empty());
      RPG_Map_PositionsConstIterator_t iterator = seed_points.begin();
      roll_result.clear();
      RPG_Dice::generateRandomNumbers(seed_points.size(),
                                      1,
                                      roll_result);
      std::advance(iterator, roll_result.front() - 1);
      entity->position = myEngine->findValid(*iterator,
                                             RPG_ENGINE_DEF_MAX_RAD_SPAWNED);
      ACE_ASSERT(entity->position != std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                    std::numeric_limits<unsigned int>::max()));
      if (entity->position == std::make_pair(std::numeric_limits<unsigned int>::max(),
                                             std::numeric_limits<unsigned int>::max()))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Engine::findValid([%u,%u], %u), aborting\n"),
                   (*iterator).first, (*iterator).second,
                   RPG_ENGINE_DEF_MAX_RAD_SPAWNED));

        // clean up
        delete entity;

        return;
      } // end IF

      RPG_Engine_EntityID_t id = myEngine->add(entity);

      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("spawned \"%s\" [id: %u] @ [%u,%u]...\n"),
                 monster_type.c_str(),
                 id,
                 entity->position.first, entity->position.second));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid event type (was: %d), aborting\n"),
                 event_handle->type));

      break;
    }
  } // end SWITCH
}

bool
RPG_Engine_Event_Manager::monster_remove_t::operator()(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::monster_remove_t::operator()"));

  // sanity check(s)
  ACE_ASSERT(engine);

  bool is_monster = engine->isMonster(id_in, locked_access);

  return (remove_monsters ? is_monster : !is_monster);
}

bool
RPG_Engine_Event_Manager::invisible_remove_t::operator()(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::invisible_remove_t::operator()"));

  // sanity check(s)
  ACE_ASSERT(engine);
  ACE_ASSERT(entity_id);

  RPG_Map_Position_t position = engine->getPosition(id_in, locked_access);

  return !engine->canSee(entity_id,
                         id_in,
                         locked_access);
}
