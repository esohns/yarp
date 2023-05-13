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

#include "rpg_engine.h"

#include <cmath>
#include <fstream>

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_error_tools.h"

#include "rpg_dice.h"
#include "rpg_dice_common.h"

#include "rpg_common.h"
#include "rpg_common_condition.h"
#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_character_race_common_tools.h"

#include "rpg_item_common_tools.h"
#include "rpg_item_common_XML_tools.h"
#include "rpg_item_dictionary.h"

#include "rpg_player.h"
#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_monster.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_net_common.h"
#include "rpg_net_defines.h"

#include "rpg_net_protocol_defines.h"

#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"
#include "rpg_engine_event_manager.h"
#include "rpg_engine_iclient.h"

// initialize statics
// *TODO* --> typedef
ACE_Atomic_Op<ACE_Thread_Mutex,
              RPG_Engine_EntityID_t> RPG_Engine::currentID = 1;

RPG_Engine::RPG_Engine ()
 : activePlayer_ (0)
 , client_ (NULL)
 , connector_ (NULL)
 , entities_ ()
 , heapAllocator_ ()
 , lock_ ()
 , allocatorConfiguration_ ()
 , messageAllocator_ (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES)
 , netConfiguration_ ()
 , queue_ (RPG_ENGINE_MAX_QUEUE_SLOTS)
 , seenPositions_ ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::RPG_Engine"));

  // use member message queue...
  inherited::msg_queue (&queue_);

  // set group ID for worker thread(s)
  inherited::grp_id (RPG_ENGINE_TASK_GROUP_ID);

  // initialize network connector
  allocatorConfiguration_.defaultBufferSize = RPG_NET_PROTOCOL_BUFFER_SIZE;
  ACE_OS::memset (&netConfiguration_, 0, sizeof (netConfiguration_));
  netConfiguration_.allocatorConfiguration = &allocatorConfiguration_;
  netConfiguration_.messageAllocator = &messageAllocator_;
  // ******************* socket configuration data ****************************
  //netConfiguration_.socketConfiguration.bufferSize =
  //  NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  ////netConfiguration_.socketConfiguration.peerAddress = INADDR_ANY;
  //netConfiguration_.socketConfiguration.useLoopbackDevice = false;

  if (RPG_ENGINE_USES_REACTOR)
    ACE_NEW_NORETURN (connector_,
                      RPG_Net_Protocol_Connector_t (true));
  else
    ACE_NEW_NORETURN (connector_,
                      RPG_Net_Protocol_AsynchConnector_t (true));
  if (!connector_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, returning\n")));
    return;
  } // end IF
  if (!connector_->initialize (netConfiguration_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize connector, returning\n")));
    return;
  } // end IF
}

RPG_Engine::~RPG_Engine ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::~RPG_Engine"));

  if (isRunning ())
    stop ();

  // clean up
  delete connector_;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);
    for (RPG_Engine_EntitiesIterator_t iterator = entities_.begin ();
         iterator != entities_.end ();
         iterator++)
      if (!(*iterator).second->character->isPlayerCharacter ())
      {
        // clean up NPC entities...
        delete (*iterator).second->character;
        delete (*iterator).second;
      } // end IF
  } // end lock scope
}

int
RPG_Engine::open (void* args_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::open"));

  ACE_UNUSED_ARG (args_in);

  // *IMPORTANT NOTE*: the first time around, the queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state, which
  // is expected.
  // Subsequently (i.e. after stop()ping/start()ing, the queue
  // will have been deactivate()d in the process, and getq() (see svc()) will
  // fail
  // (ESHUTDOWN) --> (re-)activate() the queue !
  // step1: (re-)activate() the queue
  if (inherited::msg_queue ()->activate () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Message_Queue::activate(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  try
  {
    start ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in start() method, aborting\n")));
    return -1;
  }

  return 0;
}

int
RPG_Engine::close (u_long arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::close"));

  // *IMPORTANT NOTE*: this method may be invoked
  // [- by an external thread closing down the active object (arg_in: 1)]
  // - by the worker thread which calls this after returning from svc() (arg_in: 0)
  //   --> in this case, this should be a NOP...
  switch (arg_in)
  {
    case 0:
    {
      // called from ACE_Task_Base on clean-up
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("(state engine) worker thread (ID: %t) leaving...\n")));

      return 0;
    }
    case 1:
    {
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument (was: %u), returning\n"),
                  arg_in));

      break;
    }
  } // end SWITCH

  ACE_ASSERT (false);
#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED (return -1;)
#endif
}

int
RPG_Engine::svc (void)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0A00) // _WIN32_WINNT_WIN10
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_TASK_THREAD_NAME),
                                     NULL);
#else
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_TASK_THREAD_NAME),
                                     0);
#endif // _WIN32_WINNT_WIN10
#endif // ACE_WIN32 || ACE_WIN64

  // sanity check(s)
  MESSAGE_QUEUE_T* message_queue_p = inherited::msg_queue ();
  ACE_ASSERT (message_queue_p);

  ACE_Message_Block* ace_mb = NULL;
  int result = -1;
  while (true)
  {
    // step1: wait for activity
    ace_mb = NULL;
    result = message_queue_p->peek_dequeue_head (ace_mb,
                                                 NULL); // block
    if (result == -1)
    {
      if (message_queue_p->deactivated ())
      {
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("message queue was deactivated, aborting\n")));
        break;
      } // end IF

      // queue has been pulsed --> proceed
    } // end IF
    else
    {
      // OK: message has arrived...
      ACE_ASSERT (result > 0);
      ace_mb = NULL;
      result =
        inherited::getq (ace_mb,
                         const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero)); // don't block
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Task::getq: \"%m\", aborting\n")));
        break;
      } // end IF
      ACE_ASSERT (ace_mb);

      switch (ace_mb->msg_type ())
      {
        // *NOTE*: currently, only use these...
        case ACE_Message_Block::MB_STOP:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("received MB_STOP...\n")));

          // clean up
          ace_mb->release ();

          return 0; // done
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("received an unknown/invalid control message (type: %d), continuing\n"),
                      ace_mb->msg_type ()));
          break;
        }
      } // end SWITCH

      // clean up
      ace_mb->release ();
    } // end ELSE

    // step2: process (one round of) entity actions
    handleEntities ();
  } // end WHILE

  return -1;
}

void
RPG_Engine::start ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::start"));

  // sanity check
  if (isRunning ())
    return;

  // OK: start worker thread
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
  char thread_name[BUFSIZ];
  ACE_OS::memset (thread_name, 0, sizeof (char[BUFSIZ]));
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_OS::strncpy (thread_name,
                   ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_TASK_THREAD_NAME),
                   std::min (static_cast<size_t> (BUFSIZ - 1), static_cast<size_t> (ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_TASK_THREAD_NAME)))));
#else
  ACE_ASSERT (COMMON_THREAD_PTHREAD_NAME_MAX_LENGTH <= BUFSIZ);
  ACE_OS::strncpy (thread_name,
                   ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_TASK_THREAD_NAME),
                   std::min (static_cast<size_t> (COMMON_THREAD_PTHREAD_NAME_MAX_LENGTH - 1), static_cast<size_t> (ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_TASK_THREAD_NAME)))));
#endif // ACE_WIN32 || ACE_WIN64
  const char* thread_names[1];
  thread_names[0] = thread_name;
  int result = inherited::activate ((THR_NEW_LWP |
                                     THR_JOINABLE |
                                     THR_INHERIT_SCHED),         // flags
                                    1,                           // number of threads
                                    0,                           // force spawning
                                    ACE_DEFAULT_THREAD_PRIORITY, // priority
                                    inherited::grp_id (),         // group id --> has been set (see above)
                                    NULL,                        // corresp. task --> use 'this'
                                    thread_handles,              // thread handle(s)
                                    NULL,                        // thread stack(s)
                                    NULL,                        // thread stack size(s)
                                    thread_ids,                  // thread id(s)
                                    thread_names);               // thread names(s)
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("(%s) started worker thread (group: %d, id: %u)...\n"),
                ACE_TEXT (RPG_ENGINE_TASK_THREAD_NAME),
                inherited::grp_id (),
                thread_ids[0]));

  // start AI...
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->start ();
  // ...auto-spawn (roaming) monsters ?
  if ((inherited2::myMetaData.max_num_spawned > 0) &&
      !inherited2::myMetaData.spawns.empty())
    for (RPG_Engine_SpawnsIterator_t iterator = inherited2::myMetaData.spawns.begin ();
         iterator != inherited2::myMetaData.spawns.end ();
         iterator++)
    {
      RPG_Engine_Event_t* spawn_event = NULL;
      ACE_NEW_NORETURN (spawn_event,
                        RPG_Engine_Event_t ());
      if (!spawn_event)
      {
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                    sizeof (RPG_Engine_Event_t)));

        return;
      } // end IF
      spawn_event->type = EVENT_ENTITY_SPAWN;
      spawn_event->entity_id = -1;
      spawn_event->timer_id = -1;

      ACE_ASSERT ((*iterator).timer_id == -1);
      // *NOTE*: fire&forget API for spawn_event
      ACE_Time_Value interval ((*iterator).spawn.interval.seconds,
                               static_cast<suseconds_t> ((*iterator).spawn.interval.u_seconds));
      (*iterator).timer_id =
        RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->schedule (spawn_event,
                                                                   interval,
                                                                   false);
      if ((*iterator).timer_id == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to schedule spawn event, continuing\n")));

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("scheduled spawn event (ID: %d)...\n"),
                  (*iterator).timer_id));
    } // end FOR
}

void
RPG_Engine::stop (bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::stop"));

  // sanity check
  if (!isRunning ())
    return;

  // stop AI (&& monster spawning)
  for (RPG_Engine_SpawnsIterator_t iterator = inherited2::myMetaData.spawns.begin ();
       iterator != inherited2::myMetaData.spawns.end ();
       iterator++)
    if ((*iterator).timer_id != -1)
    {
      RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->cancel ((*iterator).timer_id);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("cancelled spawn event (ID: %d)...\n"),
                  (*iterator).timer_id));
      (*iterator).timer_id = -1;
    } // end IF
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->stop ();

  // drop control message into the queue...
  ACE_Message_Block* stop_mb = NULL;
  ACE_NEW_NORETURN (stop_mb,
                    ACE_Message_Block (0,                                  // size
                                       ACE_Message_Block::MB_STOP,         // type
                                       NULL,                               // continuation
                                       NULL,                               // data
                                       NULL,                               // buffer allocator
                                       NULL,                               // locking strategy
                                       ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
                                       ACE_Time_Value::zero,               // execution time
                                       ACE_Time_Value::max_time,           // deadline time
                                       NULL,                               // data block allocator
                                       NULL));                             // message allocator
  if (!stop_mb)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("unable to allocate memory, returning\n")));
    return;
  } // end IF

  // block, if necessary
  if (inherited::putq (stop_mb, NULL) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", returning\n")));

    // clean up
    stop_mb->release ();

    return;
  } // end IF

  // ... and wait for the worker thread to join
  if (inherited::wait () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", returning\n")));
    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%s) worker thread(s) have joined...\n"),
              ACE_TEXT (RPG_ENGINE_TASK_THREAD_NAME)));

  clearEntityActions (0,
                      lockedAccess_in);
}

void
RPG_Engine::wait_all ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::wait_all"));

  // ... wait for ALL worker(s) to join
  int result =
      ACE_Thread_Manager::instance ()->wait_grp (RPG_ENGINE_TASK_GROUP_ID);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(): \"%m\", returning\n")));
}

void
RPG_Engine::dump_state () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::dump_state"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("handling %u entities...\n"),
              entities_.size ()));

  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
  {
    try
    {
      (*iterator).second->character->dump ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%@] caught exception in RPG_Character_Base::dump(), continuing\n"),
                  (*iterator).second->character));
    }
  } // end FOR
}

void
RPG_Engine::initialize (RPG_Engine_IClient* client_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::initialize"));

  // sanity check(s)
  ACE_ASSERT (client_in);

  client_ = client_in;

  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->initialize (this);
}

void
RPG_Engine::set (const struct RPG_Engine_LevelData& level_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::set"));

  // sanity check(s)
  ACE_ASSERT (client_);

  // initialize state
  inherited2::init (level_in);
  // initialize any missing door state(s) (doors remain uninitialized when
  // loading level files)...
  for (RPG_Map_DoorsIterator_t iterator = inherited2::myMap.plan.doors.begin ();
       iterator != inherited2::myMap.plan.doors.end ();
       iterator++)
    if ((*iterator).state == RPG_MAP_DOORSTATE_INVALID)
      (*iterator).state = DOORSTATE_CLOSED;

  // notify client / window
  struct RPG_Engine_ClientNotificationParameters parameters;
  parameters.entity_id = 0;
  parameters.condition = RPG_COMMON_CONDITION_INVALID;
  parameters.positions.insert (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                               std::numeric_limits<unsigned int>::max ()));
  parameters.previous_position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  try
  {
    client_->notify (COMMAND_E2C_INIT,
                     parameters,
                     true);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (COMMAND_E2C_INIT).c_str ())));
  }
}

RPG_Engine_EntityID_t
RPG_Engine::add (struct RPG_Engine_Entity* entity_in,
                 bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::add"));

  // sanity check
  ACE_ASSERT (entity_in);
  ACE_ASSERT (entity_in->character);
  ACE_ASSERT (client_);

  RPG_Engine_EntityID_t id = RPG_Engine::currentID++;

  if (lockedAccess_in)
    lock_.acquire ();
  entities_[id] = entity_in;

  // step2: initialize vision cache
  RPG_Map_Positions_t visible_positions;
  getVisiblePositions (id,
                       visible_positions,
                       false);
  { //ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);
    ACE_ASSERT(seenPositions_.find (id) ==
               seenPositions_.end ());
    seenPositions_[id] = visible_positions;
  } // end lock scope
  if (lockedAccess_in)
    lock_.release ();

  // notify AI
  float temp =
    static_cast<float> (entity_in->character->getSpeed (false,
                                                        inherited2::myMetaData.environment.lighting,
                                                        inherited2::myMetaData.environment.terrain,
                                                        TRACK_NONE));
  temp /= RPG_ENGINE_FEET_PER_SQUARE;
  temp *= RPG_ENGINE_ROUND_INTERVAL;
  float squares_per_round = temp;
  squares_per_round = (1.0F / squares_per_round);
  squares_per_round *= static_cast<float> (RPG_ENGINE_SPEED_MODIFIER);
  float fractional = std::modf (squares_per_round, &squares_per_round);
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->add (id,
                                                       ACE_Time_Value (static_cast<time_t> (squares_per_round),
                                                                       static_cast<suseconds_t> (fractional * 1000000.0F)));

  // notify client / window
  struct RPG_Engine_ClientNotificationParameters parameters;
  parameters.entity_id = id;
  parameters.condition = RPG_COMMON_CONDITION_INVALID;
  parameters.positions.insert(entity_in->position);
  parameters.previous_position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  try {
    client_->notify (COMMAND_E2C_ENTITY_ADD,
                     parameters,
                     lockedAccess_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Engine_IWindow::notify(\"%s\"), returning\n"),
                ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (COMMAND_E2C_ENTITY_ADD).c_str ())));
    return id;
  }

  parameters.positions = visible_positions;
  parameters.visible_radius = getVisibleRadius (id,
                                                lockedAccess_in);
  try {
    client_->notify (COMMAND_E2C_ENTITY_VISION,
                     parameters,
                     lockedAccess_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (COMMAND_E2C_ENTITY_VISION).c_str ())));
  }

  return id;
}

void
RPG_Engine::remove (const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::remove"));

  // sanity check
  ACE_ASSERT (id_in);

  // notify AI
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->remove (id_in);

  struct RPG_Engine_ClientNotificationParameters parameters;
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    RPG_Engine_EntitiesIterator_t iterator = entities_.find (id_in);
    if (iterator == entities_.end ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid entity ID (was: %u), returning\n"),
                  id_in));
      return;
    } // end IF
    if (!(*iterator).second->character->isPlayerCharacter ())
    {
      // clean up NPC entities...
      delete (*iterator).second->character;
      delete (*iterator).second;
    } // end IF
    parameters.positions.insert ((*iterator).second->position);
    entities_.erase (iterator);

    // step2: update vision cache
//      { ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
    ACE_ASSERT (seenPositions_.find (id_in) != seenPositions_.end ());
    seenPositions_.erase (id_in);
//      } // end lock scope

    if (id_in == activePlayer_)
      setActive (0, false);
  } // end lock scope

  // notify client / window
  parameters.entity_id = id_in;
  parameters.condition = RPG_COMMON_CONDITION_INVALID;
  parameters.previous_position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  try {
    client_->notify (COMMAND_E2C_ENTITY_REMOVE,
                     parameters,
                     true);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (COMMAND_E2C_ENTITY_REMOVE).c_str ())));
  }
}

bool
RPG_Engine::exists (const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::exists"));

  // sanity check
  ACE_ASSERT (id_in);

  ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, lock_, false);

  return (entities_.find (id_in) != entities_.end ());
}

void
RPG_Engine::action (const RPG_Engine_EntityID_t& id_in,
                    const struct RPG_Engine_Action& action_in,
                    bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::action"));

  // sanity check
  ACE_ASSERT (id_in);

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesIterator_t iterator = entities_.find (id_in);
  if (iterator == entities_.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));
    if (lockedAccess_in) lock_.release ();
    return;
  } // end IF

  switch (action_in.command)
  {
    case COMMAND_ATTACK:
    {
      // if the entity is currently attacking, change the previous target
      if ((*iterator).second->modes.find (ENTITYMODE_FIGHTING) != (*iterator).second->modes.end ())
      {
        if ((*iterator).second->modes.find (ENTITYMODE_TRAVELLING) != (*iterator).second->modes.end ())
        {
          // stop moving
          while ((*iterator).second->actions.front ().command == COMMAND_STEP)
            (*iterator).second->actions.pop_front ();

          ACE_ASSERT ((*iterator).second->actions.front ().command == COMMAND_TRAVEL);
          (*iterator).second->actions.pop_front ();

          (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);
        } // end IF
        else
        {
          // stop attacking
          while (((*iterator).second->actions.front ().command == COMMAND_ATTACK_FULL)    ||
                 ((*iterator).second->actions.front ().command == COMMAND_ATTACK_STANDARD))
            (*iterator).second->actions.pop_front ();
        } // end ELSE
        ACE_ASSERT ((*iterator).second->actions.front ().command == COMMAND_ATTACK);
        (*iterator).second->actions.pop_front ();
        (*iterator).second->actions.push_front (action_in);

        if (lockedAccess_in) lock_.release ();

        // done
        return;
      } // end IF

      break;
    }
    case COMMAND_STOP:
    {
      (*iterator).second->actions.clear ();

      if (lockedAccess_in) lock_.release ();

      // done
      return;
    }
    case COMMAND_TRAVEL:
    {
      // if the entity is currently travelling, change the previous destination
      if ((*iterator).second->modes.find (ENTITYMODE_TRAVELLING) != (*iterator).second->modes.end ())
      { ACE_ASSERT (!(*iterator).second->actions.empty ()); // *TODO*
        while ((*iterator).second->actions.front ().command == COMMAND_STEP)
          (*iterator).second->actions.pop_front ();

        ACE_ASSERT ((*iterator).second->actions.front ().command == COMMAND_TRAVEL);
        (*iterator).second->actions.pop_front ();
        (*iterator).second->actions.push_front (action_in);

        if (lockedAccess_in) lock_.release ();

        // done
        return;
      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH

  (*iterator).second->actions.push_back (action_in);

  if (lockedAccess_in) lock_.release ();

  // wake up the engine
  inherited::msg_queue ()->pulse ();
}

bool
RPG_Engine::load (const std::string& filename_in,
                  const std::string& schemaRepository_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::load"));

  // sanity check(s)
  if (!Common_File_Tools::isReadable (filename_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  bool restart = isRunning ();
  stop (false);

  // step0: clean up
  for (RPG_Engine_EntitiesIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
    if (!(*iterator).second->character->isPlayerCharacter ())
    {
      // clean up NPC entities...
      delete (*iterator).second->character;
      delete (*iterator).second;
    } // end IF
  entities_.clear ();

  // step1: load level state model
  std::ifstream ifs;
  ifs.exceptions (std::ifstream::badbit | std::ifstream::failbit);
  try {
    ifs.open (filename_in.c_str (),
              std::ios_base::in);
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::open(\"%s\"): caught exception: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (exception.what ())));
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::open(\"%s\"): caught exception, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  }

  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  ::xml_schema::properties props;
  std::string path;
  ACE_ASSERT (!schemaRepository_in.empty ());
  if (Common_Error_Tools::inDebugSession ())
  {
    path = schemaRepository_in;
    path += ACE_DIRECTORY_SEPARATOR_STR;
    path += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING);
    path += ACE_DIRECTORY_SEPARATOR_STR;
    path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  } // end IF
  else
   path = schemaRepository_in;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SCHEMA_FILE);
  // sanity check(s)
  if (!Common_File_Tools::isReadable (path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (path.c_str ())));

    // clean up
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
      return false;
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
      return false;
    }

    return false;
  } // end IF
  // *NOTE*: support paths with spaces
  path = RPG_Common_Tools::sanitizeURI (path);
  path.insert (0, ACE_TEXT_ALWAYS_CHAR ("file:///"));
  props.schema_location (ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_TARGET_NAMESPACE),
                         path);
//   props.no_namespace_schema_location(RPG_ENGINE_SCHEMA_FILE);
//   props.schema_location("http://www.w3.org/XML/1998/namespace", "xml.xsd");
  std::auto_ptr<RPG_Engine_State_XMLTree_Type> engine_state_p;
  try {
    engine_state_p = ::engine_state_t (ifs,
                                       RPG_XSDErrorHandler,
                                       flags,
                                       props);
  } catch (::xml_schema::parsing const& exception) {
    std::ostringstream converter;
    converter << exception;
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to engine_state_t(\"%s\"): exception occurred: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (converter.str ().c_str ())));

    // clean up
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
      return false;
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
      return false;
    }

    return false;
  } catch (::xml_schema::exception const& exception) {
    std::ostringstream converter;
    converter << exception;
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to engine_state_t(\"%s\"): exception occurred: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (converter.str ().c_str ())));

    // clean up
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
      return false;
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
      return false;
    }

    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to engine_state_t(\"%s\"): exception occurred, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
      return false;
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
      return false;
    }

    return false;
  }

  try {
    ifs.close ();
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (exception.what ())));
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  }

  // initialize level
  struct RPG_Engine_LevelData level;
  RPG_Engine_State_XMLTree_Type::entities_sequence& entities =
    engine_state_p->entities ();
  path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  std::string filename;
  RPG_Magic_Spells_t spells;
  RPG_Character_Conditions_t condition;
  struct RPG_Engine_Entity* entity = NULL;
  for (RPG_Engine_State_XMLTree_Type::entities_const_iterator iterator = entities.begin ();
       iterator != entities.end ();
       iterator++)
  {
    entity = NULL;
    ACE_NEW_NORETURN (entity,
                      struct RPG_Engine_Entity ());
    if (!entity)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                  sizeof (struct RPG_Engine_Entity)));
      return false;
    } // end IF

    if ((*iterator).player ().present ())
    {
      const RPG_Player_State_XMLTree_Type& player_state =
        (*iterator).player ().get ();
      filename = path;
      filename += ACE_DIRECTORY_SEPARATOR_CHAR;
      filename += player_state.file ();
      const RPG_Player_Conditions_XMLTree_Type::condition_sequence& condition_xml =
        player_state.conditions ().condition ();
      for (RPG_Player_Conditions_XMLTree_Type::condition_const_iterator iterator2 = condition_xml.begin ();
           iterator2 != condition_xml.end ();
           iterator2++)
        condition.insert (RPG_Common_ConditionHelper::stringToRPG_Common_Condition (*iterator2));
      spells.clear ();
//      if (player_state.spells().present())
//        spells =
//            RPG_Player_Common_Tools::spellsXMLTreeToSpells(player_state.spells().get());
      condition.clear ();
      entity->character = RPG_Player::load (filename,
                                            schemaRepository_in,
                                            condition,
                                            player_state.HP (),
                                            spells);
      if (!entity->character)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Player::load(\"%s\"), aborting\n"),
                    ACE_TEXT (filename.c_str ())));
        delete entity;
        return false;
      } // end IF
      entity->is_spawned = false;
    } // end IF
    else
    {
      const RPG_Monster_State_XMLTree_Type& monster_state =
        (*iterator).monster ().get ();
      RPG_Item_List_t items;
      if (monster_state.inventory ().present ())
        items = RPG_Item_Common_XML_Tools::instantiate (monster_state.inventory ().get ());
      condition.clear ();
      const RPG_Player_Conditions_XMLTree_Type::condition_sequence& condition_xml =
        monster_state.conditions ().condition ();
      for (RPG_Player_Conditions_XMLTree_Type::condition_const_iterator iterator2 = condition_xml.begin ();
           iterator2 != condition_xml.end ();
           iterator2++)
        condition.insert (RPG_Common_ConditionHelper::stringToRPG_Common_Condition (*iterator2));
      spells.clear ();
//      if (monster_state.spells().present())
//        spells =
//            RPG_Player_Common_Tools::spellsXMLTreeToSpells(monster_state.spells().get());
      *entity = RPG_Engine_Common_Tools::createEntity (monster_state.type (),
                                                       monster_state.maxHP (),
                                                       monster_state.gold (),
                                                       items,
                                                       condition,
                                                       monster_state.HP (),
                                                       spells);
      if (!entity->character)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Engine_Common_Tools::createEntity(\"%s\"), aborting\n"),
                    ACE_TEXT (monster_state.type ().c_str ())));
        delete entity;
        return false;
      } // end IF
    } // end ELSE
    const RPG_Engine_EntityState_XMLTree_Type::mode_sequence& modes =
      (*iterator).mode ();
    for (RPG_Engine_EntityState_XMLTree_Type::mode_const_iterator iterator2 = modes.begin ();
         iterator2 != modes.end ();
         iterator2++)
      entity->modes.insert (RPG_Engine_EntityModeHelper::stringToRPG_Engine_EntityMode (*iterator2));
    const RPG_Map_Position_XMLTree_Type& position = (*iterator).position ();
    entity->position = std::make_pair (position.x (),
                                       position.y ());

    add (entity,
         false);
  } // end FOR
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded %u entities...\n"),
              ACE_TEXT (entities.size ())));

//  level.metadata.name.clear();
  level.metadata.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  level.metadata.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  level.metadata.environment.outdoors = false;
  level.metadata.environment.plane = RPG_COMMON_PLANE_INVALID;
  level.metadata.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  level.metadata.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;

//  level.metadata.spawns.clear();
  level.metadata.max_num_spawned = 0;

  level.map.start =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  level.map.seeds.clear ();
  level.map.plan.size_x = 0;
  level.map.plan.size_y = 0;
  level.map.plan.unmapped.clear ();
  level.map.plan.walls.clear ();
  level.map.plan.doors.clear ();
  level.map.plan.rooms_are_square = false;
  if (!RPG_Engine_Level::load (filename_in,
                               schemaRepository_in,
                               level))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Engine_Level::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF

  if (restart)
    start ();

  return true;
}

bool
RPG_Engine::save (const std::string& descriptor_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::save"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  // step1: save active player profile (if any)
  RPG_Engine_EntitiesIterator_t iterator = entities_.find (activePlayer_);
  if (activePlayer_)
  {
    // sanity check(s)
    ACE_ASSERT (iterator != entities_.end ());

    RPG_Player* player_p = NULL;
    try {
      player_p = dynamic_cast<RPG_Player*> ((*iterator).second->character);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dynamic_cast<RPG_Player*>(%@), aborting\n"),
                  (*iterator).second->character));
      return false;
    }
    if (!player_p->save (std::string ()))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Player::save(\"%s\"), aborting\n"),
                  ACE_TEXT ((*iterator).second->character->getName ().c_str ())));
      return false;
    } // end IF
  } // end IF

  // step2: save state
  if (inherited2::myMetaData.name.empty ())
    return true; // nothing to do...

  std::string filename = RPG_Engine_Common_Tools::getEngineStateDirectory ();
  filename += myMetaData.name;
  if (iterator != entities_.end ())
  {
    filename += ACE_TEXT_ALWAYS_CHAR ("_");
    filename += (*iterator).second->character->getName ();
  } // end IF
  if (!descriptor_in.empty ())
  {
    filename += ACE_TEXT_ALWAYS_CHAR ("_");
    filename += descriptor_in;
  } // end IF
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_STATE_EXT);
  if (Common_File_Tools::isReadable (filename))
  {
    // *TODO*: warn user ?
//     if (!Common_File_Tools::deleteFile (filename))
//     {
//       ACE_DEBUG ((LM_ERROR,
//                   ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
//                   ACE_TEXT (filename.c_str ())));
//       return false;
//     } // end IF
  } // end IF

  std::ofstream ofs;
  ofs.exceptions (std::ofstream::badbit | std::ofstream::failbit);
  try {
    ofs.open (filename.c_str (),
              (std::ios_base::out | std::ios_base::trunc));
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::open(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename.c_str ()),
                ACE_TEXT (exception.what ())));
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::open(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return false;
  }

  RPG_Engine_State_XMLTree_Type level_state_model (RPG_Common_Tools::sanitize (inherited2::myMetaData.name));

  RPG_Map_State_XMLTree_Type::door_sequence& doors = level_state_model.door ();
  for (RPG_Map_DoorsConstIterator_t iterator = inherited2::myMap.plan.doors.begin ();
       iterator != inherited2::myMap.plan.doors.end ();
       iterator++)
  {
    RPG_Map_Door_State_XMLTree_Type door (RPG_Map_Position_XMLTree_Type ((*iterator).position.first,
                                                                         (*iterator).position.first),
                                                                         RPG_Map_DoorState_XMLTree_Type (static_cast<RPG_Map_DoorState_XMLTree_Type::value>((*iterator).state)));
    doors.push_back (door);
  } // end FOR
  level_state_model.door (doors);

  RPG_Engine_State_XMLTree_Type::entities_sequence& entities =
    level_state_model.entities ();
  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
  {
    RPG_Engine_EntityState_XMLTree_Type entity_state (RPG_Map_Position_XMLTree_Type ((*iterator).second->position.first,
                                                                                     (*iterator).second->position.second));

    const RPG_Character_Conditions_t& condition =
      (*iterator).second->character->getCondition ();
    RPG_Player_Conditions_XMLTree_Type condition_xml;
    RPG_Player_Conditions_XMLTree_Type::condition_sequence& conditions =
      condition_xml.condition ();
    for (RPG_Character_ConditionsIterator_t iterator2 = condition.begin ();
         iterator2 != condition.end ();
         iterator2++)
      conditions.push_back (RPG_Common_Condition_XMLTree_Type (static_cast<RPG_Common_Condition_XMLTree_Type::value> (*iterator2)));
    if ((*iterator).second->character->isPlayerCharacter ())
    {
      std::string filename =
        RPG_Common_Tools::sanitize ((*iterator).second->character->getName ());
      filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
      RPG_Player_State_XMLTree_Type player_state (condition_xml,
                                                  (*iterator).second->character->getNumHitPoints (),
                                                  filename);

      entity_state.player (player_state);
    } // end IF
    else
    {
      RPG_Monster_State_XMLTree_Type monster_state (condition_xml,
                                                    (*iterator).second->character->getNumHitPoints (),
                                                    (*iterator).second->character->getWealth (),
                                                    (*iterator).second->character->getNumTotalHitPoints (),
                                                    (*iterator).second->character->getName ());

      entity_state.monster (monster_state);
    } // end ELSE

    RPG_Engine_EntityState_XMLTree_Type::mode_sequence& modes =
      entity_state.mode ();
    for (RPG_Engine_EntityModeConstIterator_t iterator2 = (*iterator).second->modes.begin ();
         iterator2 != (*iterator).second->modes.end ();
         iterator2++)
      modes.push_back (RPG_Engine_EntityMode_XMLTree_Type (static_cast<RPG_Engine_EntityMode_XMLTree_Type::value>(*iterator2)));
    entity_state.mode (modes);

    entities.push_back (entity_state);
  } // end FOR
  level_state_model.entities (entities);

  ::xml_schema::namespace_infomap map;
  map[""].name = ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_TARGET_NAMESPACE);
  map[""].schema = ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SCHEMA_FILE);
  std::string character_set (ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_SCHEMA_CHARSET));
  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  try {
    ::engine_state_t (ofs,
                      level_state_model,
                      map,
                      character_set,
                      flags);
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::engine_state_t(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename.c_str ()),
                ACE_TEXT (exception.what ())));

    // clean up
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (filename.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(\"%s\"), aborting\n"),
                  ACE_TEXT (filename.c_str ())));
    }

    return false;
  } catch (::xml_schema::serialization& exception) {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::engine_state_t(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename.c_str ()),
                ACE_TEXT (text.c_str ())));

    // clean up
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (filename.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(\"%s\"), aborting\n"),
                  ACE_TEXT (filename.c_str ())));
    }

    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::engine_state_t(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));

    // clean up
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (filename.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(\"%s\"), aborting\n"),
                  ACE_TEXT (filename.c_str ())));
    }

    return false;
  }

  try {
    ofs.close ();
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename.c_str ()),
                ACE_TEXT (exception.what ())));
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::close(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
  }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("saved state to file: \"%s\"\n"),
              ACE_TEXT (filename.c_str ())));

  return true;
}

void
RPG_Engine::setActive (const RPG_Engine_EntityID_t& id_in,
                       bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::setActive"));

  if (lockedAccess_in) lock_.acquire ();
  activePlayer_ = id_in;
  if (lockedAccess_in) lock_.release ();
}

RPG_Engine_EntityID_t
RPG_Engine::getActive (bool lockedAcces_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getActive"));

  RPG_Engine_EntityID_t result = 0;

  if (lockedAcces_in) lock_.acquire ();
  result = activePlayer_;
  if (lockedAcces_in) lock_.release ();

  return result;
}

void
RPG_Engine::mode (const RPG_Engine_EntityMode& mode_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::mode"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  // sanity check
  if (activePlayer_ == 0)
    return; // nothing to do...

  RPG_Engine_EntitiesIterator_t iterator = entities_.find (activePlayer_);
  ACE_ASSERT (iterator != entities_.end ());
  //RPG_Engine_PlayerModeConstIterator_t iterator2 = (*iterator)->modes.find (mode_in);
  //if (iterator2 != (*iterator)->modes.end ())
  //  return; // nothing to do...

  (*iterator).second->modes.insert (mode_in);
}

void
RPG_Engine::clear (const RPG_Engine_EntityMode& mode_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::clear"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  // sanity check
  if (activePlayer_ == 0)
    return; // nothing to do...

  RPG_Engine_EntitiesIterator_t iterator = entities_.find (activePlayer_);
  ACE_ASSERT (iterator != entities_.end ());
  //RPG_Engine_PlayerModeConstIterator_t iterator2 = (*iterator)->modes.find (mode_in);
  //if (iterator2 == (*iterator)->modes.end ())
  //  return; // nothing to do...

  (*iterator).second->modes.erase (mode_in);
}

bool
RPG_Engine::hasMode (const RPG_Engine_EntityMode& mode_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::hasMode"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  // sanity check
  if (activePlayer_ == 0)
    return false;

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (activePlayer_);
  ACE_ASSERT (iterator != entities_.end ());

  return ((*iterator).second->modes.find (mode_in) !=
          (*iterator).second->modes.end ());
}

RPG_Map_Position_t
RPG_Engine::getPosition (const RPG_Engine_EntityID_t& id_in,
                         bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getPosition"));

  RPG_Map_Position_t result =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (id_in);
  ACE_ASSERT (iterator != entities_.end ());
  if (iterator == entities_.end ())
  {
    if (lockedAccess_in) lock_.release ();

    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));

    return result;
  } // end IF

  result = (*iterator).second->position;

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Position_t
RPG_Engine::findValid (const RPG_Map_Position_t& center_in,
                       unsigned int radius_in,
                       bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::findValid"));

  // init return value(s)
  RPG_Map_Position_t result =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());

  if (lockedAccess_in) lock_.acquire ();

  RPG_Map_Positions_t valid, occupied, possible;
  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
    occupied.insert ((*iterator).second->position);
  std::vector<RPG_Map_Position_t> difference;
  std::vector<RPG_Map_Position_t>::iterator difference_end;
  unsigned int max_radius =
      ((myMap.plan.size_x > myMap.plan.size_x) ? myMap.plan.size_x
                                               : myMap.plan.size_y);
  for (unsigned int i = 0;
       i < (radius_in ? radius_in : max_radius);
       i++)
  {
    valid.clear ();

    // step1: retrieve valid positions in a specific radius
    inherited2::findValid (center_in,
                           i,
                           valid);

    // step2: remove any occupied positions
    possible.clear ();
    difference.resize (valid.size ());
    difference_end = std::set_difference (valid.begin (), valid.end (),
                                          occupied.begin (), occupied.end (),
                                          difference.begin ());
    possible.insert (difference.begin (), difference_end);

    if (!possible.empty ())
      break;
  } // end FOR

  if (lockedAccess_in) lock_.release ();

  if (possible.empty ())
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("could not find a valid position around [%u,%u] (radius: %u), aborting\n"),
                center_in.first, center_in.second,
                radius_in));

    return result; // failed
  } // end IF
  else if (possible.size () == 1)
    return *possible.begin ();

  // step3: choose a random position
  RPG_Map_PositionsConstIterator_t iterator = possible.begin ();
  RPG_Dice_RollResult_t roll_result;
  RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (possible.size ()),
                                   1,
                                   roll_result);
  std::advance (iterator, roll_result.front () - 1);
  result = *iterator;

  return result;
}

bool
RPG_Engine::isBlocked (const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::isBlocked"));

  // blocked by walls, closed doors ... ?
  if (!inherited2::isValid (position_in))
    return false;

  // occupied ... ?
  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
    if ((*iterator).second->position == position_in)
      return true;

  return false;
}

RPG_Engine_EntityID_t
RPG_Engine::hasEntity (const RPG_Map_Position_t& position_in,
                       bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::hasEntity"));

  RPG_Engine_EntityID_t result = 0;

  if (lockedAccess_in) lock_.acquire ();

  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
    if ((*iterator).second->position == position_in)
    {
      result = (*iterator).first;

      if (lockedAccess_in) lock_.release ();

      return result;
    } // end IF

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Engine_EntityList_t
RPG_Engine::entities (const RPG_Map_Position_t& position_in,
                      bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::entities"));

  RPG_Engine_EntityList_t result;

  if (lockedAccess_in) lock_.acquire ();

  // step1: retrieve entities
  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
    result.push_back ((*iterator).first);

  if (lockedAccess_in) lock_.release ();

  // step2: sort entity list
  distance_sort_t distance_sort = {this, false, position_in};
  result.sort (distance_sort);

  return result;
}

bool
RPG_Engine::isMonster (const RPG_Engine_EntityID_t& id_in,
                       bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::isMonster"));

  bool result = false;

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (id_in);
  ACE_ASSERT (iterator != entities_.end ());
  if (iterator == entities_.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));

    if (lockedAccess_in) lock_.release ();

    // *CONSIDER*: --> false negative ?
    return result;
  } // end IF

  result = !(*iterator).second->character->isPlayerCharacter ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

std::string
RPG_Engine::getName (const RPG_Engine_EntityID_t& id_in,
                     bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getName"));

  std::string result;

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (id_in);
  ACE_ASSERT(iterator != entities_.end ());
  if (iterator == entities_.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));

    if (lockedAccess_in) lock_.release ();

    // *CONSIDER*: --> false negative ?
    return result;
  } // end IF

  result = (*iterator).second->character->getName ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Character_Class
RPG_Engine::getClass (const RPG_Engine_EntityID_t& id_in,
                      bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getClass"));

  RPG_Character_Class result;
  result.metaClass = RPG_CHARACTER_METACLASS_INVALID;
//  result.subClasses.clear();

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (id_in);
  ACE_ASSERT (iterator != entities_.end ());
  if (iterator == entities_.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));

    if (lockedAccess_in) lock_.release ();

    // *CONSIDER*: --> false negative ?
    return result;
  } // end IF

  RPG_Player_Player_Base* player_base = NULL;
  try {
    player_base =
        dynamic_cast<RPG_Player_Player_Base*> ((*iterator).second->character);
  } catch (...) {
    player_base = NULL;
  }
  if (!player_base)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), returning\n"),
                (*iterator).second->character));
    if (lockedAccess_in) lock_.release ();
    return result;
  } // end IF

  result = player_base->getClass ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

unsigned char
RPG_Engine::getVisibleRadius (const RPG_Engine_EntityID_t& id_in,
                              bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getVisibleRadius"));

  // init return value(s)
  unsigned char result = 0;

  // sanity check
  if (id_in == 0)
    return result;

  // step1: retrieve:
  // - race (if any)
  // - center position
  // - equiped light source (if any)
  RPG_Character_Race_t race (0);
  RPG_Item_CommodityLight equipped_light_source =
      RPG_ITEM_COMMODITYLIGHT_INVALID;

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (id_in);
  ACE_ASSERT (iterator != entities_.end ());
  if (iterator == entities_.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));
    if (lockedAccess_in) lock_.release ();
    return result;
  } // end IF

  if ((*iterator).second->character->isPlayerCharacter ())
  {
    RPG_Player_Player_Base* player_base = NULL;
    try {
      player_base =
          dynamic_cast<RPG_Player_Player_Base*> ((*iterator).second->character);
    } catch (...) {
      player_base = NULL;
    }
    if (!player_base)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in dynamic_cast<RPG_Player_Player_Base*>(%@), aborting\n"),
                  (*iterator).second->character));
      if (lockedAccess_in) lock_.release ();
      return result;
    } // end IF

    race = player_base->getRace ();
    equipped_light_source = player_base->getEquipment ().getLightSource ();
  } // end IF
  else
  {
    RPG_Monster* monster = NULL;
    monster = dynamic_cast<RPG_Monster*> ((*iterator).second->character);
    try {
      monster =
          dynamic_cast<RPG_Monster*> ((*iterator).second->character);
    } catch (...) {
      monster = NULL;
    }
    if (!monster)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in dynamic_cast<RPG_Monster*>(%@), aborting\n"),
                  (*iterator).second->character));
      if (lockedAccess_in) lock_.release ();
      return result;
    } // end IF

    equipped_light_source = monster->getEquipment ().getLightSource ();
  } // end ELSE

  // step2: consider environment / ambient lighting
  unsigned short environment_radius =
    RPG_Common_Tools::environmentToRadius (inherited2::myMetaData.environment);

  // step3: consider equipment (ambient lighting conditions)
  unsigned short lit_radius = 0;
  if (equipped_light_source != RPG_ITEM_COMMODITYLIGHT_INVALID)
    lit_radius =
        RPG_Item_Common_Tools::lightingItemToRadius (equipped_light_source,
                                                     (inherited2::myMetaData.environment.lighting == AMBIENCE_BRIGHT));

  if (lockedAccess_in) lock_.release ();

  // step4: consider low-light vision
  // *TODO*: consider monsters as well...
  if (RPG_Character_Race_Common_Tools::hasRace (race, RACE_ELF)   ||
      RPG_Character_Race_Common_Tools::hasRace (race, RACE_GNOME))
    lit_radius *= 2;

  result =
    ((environment_radius > lit_radius) ? static_cast<unsigned char> (environment_radius)
                                       : static_cast<unsigned char> (lit_radius));

  // step5: consider darkvision
  // *TODO*: consider monsters as well...
  if ((RPG_Character_Race_Common_Tools::hasRace (race, RACE_DWARF) ||
       RPG_Character_Race_Common_Tools::hasRace (race, RACE_ORC)) &&
      (result < 60))
    result = 60;

  result /= RPG_ENGINE_FEET_PER_SQUARE;

  return result;
}

void
RPG_Engine::getVisiblePositions (const RPG_Engine_EntityID_t& id_in,
                                 RPG_Map_Positions_t& positions_out,
                                 bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getVisiblePositions"));

  // init return value(s)
  positions_out.clear ();

  // sanity check
  ACE_ASSERT (id_in != 0);

  if (lockedAccess_in) lock_.acquire ();

  // step1: find "lit" positions
  RPG_Map_Position_t current_position = getPosition (id_in, false);
  RPG_Map_Common_Tools::buildCircle (current_position,
                                     inherited2::getSize (),
                                     getVisibleRadius (id_in, false),
                                     true,
                                     positions_out);

  // step2: remove any blocked (== unreachable) positions
  // --> cannot see through walls / (closed) doors...
  RPG_Map_Positions_t obstacles = inherited2::getObstacles ();

  if (lockedAccess_in) lock_.release ();

  // *WARNING*: this works for associative containers ONLY
  for (RPG_Map_PositionsConstIterator_t iterator = positions_out.begin ();
       iterator != positions_out.end ();
       )
    if (RPG_Map_Common_Tools::hasLineOfSight (current_position,
                                              *iterator,
                                              obstacles,
                                              true))
      iterator++;
    else
      positions_out.erase (iterator++);
}

bool
RPG_Engine::canSee (const RPG_Engine_EntityID_t& id_in,
                    const RPG_Map_Position_t& position_in,
                    bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::canSee"));

  // sanity check
  if (id_in == 0)
    return false; // *CONSIDER*: false negative ?

  if (lockedAccess_in) lock_.acquire ();

  RPG_Map_Positions_t visible_positions;
  getVisiblePositions (id_in,
                       visible_positions,
                       false);

  if (lockedAccess_in) lock_.release ();

  return (visible_positions.find (position_in) != visible_positions.end ());
}

bool
RPG_Engine::canSee (const RPG_Engine_EntityID_t& id_in,
                    const RPG_Engine_EntityID_t& target_in,
                    bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::canSee"));

  // sanity check(s)
  ACE_ASSERT (id_in && target_in);

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (target_in);
  ACE_ASSERT (iterator != entities_.end ());

  // target using a light source ?
  // *TODO*: consider magic light, spells, ...
  bool target_equipped_a_light = false;
  if ((*iterator).second->character->isPlayerCharacter ())
  {
    RPG_Player_Player_Base* player_base_p =
          dynamic_cast<RPG_Player_Player_Base*> ((*iterator).second->character);
    ACE_ASSERT (player_base_p);
    target_equipped_a_light =
      (player_base_p->getEquipment ().getLightSource () != RPG_ITEM_COMMODITYLIGHT_INVALID);
  } // end IF
  else
  {
    RPG_Monster* monster_p =
      dynamic_cast<RPG_Monster*> ((*iterator).second->character);
    ACE_ASSERT (monster_p);
    target_equipped_a_light =
      (monster_p->getEquipment ().getLightSource () != RPG_ITEM_COMMODITYLIGHT_INVALID);
  } // end ELSE
  RPG_Map_Position_t target_position = (*iterator).second->position;
  if (!target_equipped_a_light)
  {
    if (lockedAccess_in) lock_.release ();

    // can the source see the target position ?
    return canSee (id_in,
                   target_position,
                   lockedAccess_in);
  } // end IF

  RPG_Map_Position_t source_position = getPosition (id_in, false);
  RPG_Map_Positions_t obstacles = inherited2::getObstacles ();

  if (lockedAccess_in) lock_.release ();

  return RPG_Map_Common_Tools::hasLineOfSight (source_position,
                                               target_position,
                                               obstacles,
                                               false);
}

bool
RPG_Engine::hasSeen (const RPG_Engine_EntityID_t& id_in,
                     const RPG_Map_Position_t& position_in,
                     bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::hasSeen"));

  bool result = false;
  RPG_Map_PositionsConstIterator_t iterator2;

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_SeenPositionsConstIterator_t iterator =
    seenPositions_.find (id_in);
  if (iterator == seenPositions_.end ())
    goto error; // unknown entity id
  iterator2 = (*iterator).second.find (position_in);
  result = (iterator2 != (*iterator).second.end ());

error:
  if (lockedAccess_in) lock_.release ();

  return result;
}

unsigned int
RPG_Engine::numSpawned (const std::string& type_in,
                        bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::numSpawned"));

  unsigned int result = 0;

  if (lockedAccess_in) lock_.acquire ();

  for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
       iterator != entities_.end ();
       iterator++)
    if ((*iterator).second->is_spawned &&
        (type_in.empty () ||
         (type_in == (*iterator).second->character->getName ())))
      result++;

  if (lockedAccess_in) lock_.release ();

  return result;
}

bool
RPG_Engine::findPath (const RPG_Map_Position_t& start_in,
                      const RPG_Map_Position_t& end_in,
                      RPG_Map_Path_t& path_out,
                      bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::findPath"));

  if (lockedAccess_in) lock_.acquire ();

  // obstacles:
  // - walls
  // - (closed, locked) doors
  RPG_Map_Positions_t obstacles = inherited2::getObstacles ();
  // - entities
  for (RPG_Engine_EntitiesConstIterator_t entity_iterator = entities_.begin ();
       entity_iterator != entities_.end ();
       entity_iterator++)
    obstacles.insert ((*entity_iterator).second->position);
  // - start, end are NEVER obstacles...
  obstacles.erase (start_in);
  obstacles.erase (end_in);

  if (lockedAccess_in) lock_.release ();

  return inherited2::findPath (start_in,
                               end_in,
                               obstacles,
                               path_out);
}

bool
RPG_Engine::canReach (const RPG_Engine_EntityID_t& id_in,
                      const RPG_Map_Position_t& position_in,
                      bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::canReach"));

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesConstIterator_t iterator = entities_.find (id_in);
  ACE_ASSERT(iterator != entities_.end ());
  if (iterator == entities_.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), aborting\n"),
                id_in));
    if (lockedAccess_in) lock_.release ();
    return false; // *TODO*: false negative ?
  } // end IF

  // step1: compute distance
  unsigned int range =
    RPG_Engine_Common_Tools::range ((*iterator).second->position, position_in);

  // step2: compute entity reach
  unsigned short base_reach = 0;
  bool absolute_reach = false;
  unsigned int max_reach =
    (*iterator).second->character->getReach (base_reach, absolute_reach) / RPG_ENGINE_FEET_PER_SQUARE;

  if (lockedAccess_in) lock_.release ();

  return (absolute_reach ? (range == max_reach)
                         : (range <= max_reach));
}

struct RPG_Engine_LevelMetaData
RPG_Engine::getMetaData (bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getMetaData"));

  struct RPG_Engine_LevelMetaData result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::getMetaData ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Position_t
RPG_Engine::getStartPosition (bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getStartPosition"));

  RPG_Map_Position_t result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::getStartPosition ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getSeedPoints (bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getSeedPoints"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::getSeedPoints ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Size_t
RPG_Engine::getSize (bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getSize"));

  RPG_Map_Size_t result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::getSize ();

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_DoorState
RPG_Engine::state (const RPG_Map_Position_t& position_in,
                   bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::state"));

  RPG_Map_DoorState result = RPG_MAP_DOORSTATE_INVALID;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::state (position_in);

  if (lockedAccess_in) lock_.release ();

  return result;
}

bool
RPG_Engine::isValid (const RPG_Map_Position_t& position_in,
                     bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::isValid"));

  bool result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::isValid (position_in);

  if (lockedAccess_in) lock_.release ();

  return result;
}

bool
RPG_Engine::isCorner (const RPG_Map_Position_t& position_in,
                      bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::isCorner"));

  bool result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::isCorner (position_in);

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Element
RPG_Engine::getElement (const RPG_Map_Position_t& position_in,
                        bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getElement"));

  RPG_Map_Element result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::getElement (position_in);

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getObstacles (const bool& includeEntities_in,
                          bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getObstacles"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::getObstacles ();

  if (includeEntities_in)
    for (RPG_Engine_EntitiesConstIterator_t iterator = entities_.begin ();
         iterator != entities_.end ();
         iterator++)
      result.insert ((*iterator).second->position);

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getWalls (bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getWalls"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in) lock_.acquire ();

  result = inherited2::myMap.plan.walls;

  if (lockedAccess_in) lock_.release ();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getDoors (bool lockedAccess_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::getDoors"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in) lock_.acquire ();

  for (RPG_Map_DoorsConstIterator_t iterator = inherited2::myMap.plan.doors.begin ();
       iterator != inherited2::myMap.plan.doors.end ();
       iterator++)
    result.insert ((*iterator).position);

  if (lockedAccess_in) lock_.release ();

  return result;
}

void
RPG_Engine::clearEntityActions (const RPG_Engine_EntityID_t& id_in,
                                bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::clearEntityActions"));

  if (lockedAccess_in) lock_.acquire ();

  RPG_Engine_EntitiesIterator_t iterator;
  if (id_in)
  {
    iterator = entities_.find (id_in);
    ACE_ASSERT (iterator != entities_.end ());
    (*iterator).second->actions.clear ();
  } // end IF
  else
    for (iterator = entities_.begin ();
         iterator != entities_.end ();
         iterator++)
      (*iterator).second->actions.clear ();

  if (lockedAccess_in) lock_.release ();
}

void
RPG_Engine::handleEntities ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::handleEntities"));

  bool action_complete = true;
  RPG_Engine_EntityID_t remove_id = 0;
  RPG_Engine_ClientNotifications_t notifications;

  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    for (RPG_Engine_EntitiesIterator_t iterator = entities_.begin();
         iterator != entities_.end();
         iterator++)
    {
      // *TODO*: check for actions/round limit
      if ((*iterator).second->actions.empty ())
        continue;

      action_complete = true;
      struct RPG_Engine_Action& current_action = (*iterator).second->actions.front ();
      switch (current_action.command)
      {
        case COMMAND_ATTACK:
        {
          action_complete = false; // *NOTE*: timed --> handled by the AI
          break; // nothing to do...
        }
        case COMMAND_ATTACK_FULL:
        case COMMAND_ATTACK_STANDARD:
        { ACE_ASSERT (current_action.target);
          RPG_Engine_EntitiesConstIterator_t target =
            entities_.find (current_action.target);
          if ((target == entities_.end ()) ||
              !canReach ((*iterator).first,
                         (*target).second->position,
                         false))
            break; // nothing to do...

          // notify client
          struct RPG_Engine_ClientNotificationParameters parameters;
          parameters.entity_id = (*iterator).first;
          parameters.positions.insert (std::make_pair(std::numeric_limits<unsigned int>::max (),
                                                      std::numeric_limits<unsigned int>::max ()));
          parameters.previous_position =
            std::make_pair (std::numeric_limits<unsigned int>::max (),
                            std::numeric_limits<unsigned int>::max ());
          // *TODO*: implement combat situations, in-turn-movement, ...
          bool hit =
            RPG_Engine_Common_Tools::attack ((*iterator).second->character,
                                             (*target).second->character,
                                             ATTACK_NORMAL,
                                             DEFENSE_NORMAL,
                                             (current_action.command == COMMAND_ATTACK_FULL),
                                             RPG_Engine_Common_Tools::range ((*iterator).second->position,
                                                                             (*target).second->position) * RPG_ENGINE_FEET_PER_SQUARE);
          notifications.push_back (std::make_pair ((hit ? COMMAND_E2C_ENTITY_HIT
                                                        : COMMAND_E2C_ENTITY_MISS),
                                                   parameters));
          parameters.entity_id = 0;

          // target disabled ? --> remove entity (see below)
          std::ostringstream converter;
          if (RPG_Engine_Common_Tools::isCharacterDisabled ((*target).second->character))
          {
            // notify client
            parameters.condition = CONDITION_DISABLED;
            parameters.entity_id = (*target).first;
            parameters.message = (*target).second->character->getName ();
            parameters.message += ACE_TEXT_ALWAYS_CHAR (" has been disabled");
            notifications.push_back (std::make_pair (COMMAND_E2C_ENTITY_CONDITION,
                                                     parameters));
            parameters.condition = RPG_COMMON_CONDITION_INVALID;
            parameters.entity_id = 0;

            // award XP
            if (!(*target).second->character->isPlayerCharacter ())
            {
              RPG_Player_Player_Base* player_base_p =
                    dynamic_cast<RPG_Player_Player_Base*> ((*iterator).second->character);
              ACE_ASSERT (player_base_p);
              unsigned int xp =
                RPG_Engine_Common_Tools::combatToXP ((*target).second->character->getName (),
                                                     player_base_p->getLevel (),
                                                     1,
                                                     1);
              enum RPG_Common_SubClass subclass_e =
                  player_base_p->gainExperience (xp);
              bool level_up = (subclass_e != RPG_COMMON_SUBCLASS_INVALID);

              parameters.entity_id = (*iterator).first;
              // append some more info to the message
              parameters.message += ACE_TEXT_ALWAYS_CHAR (", ");
              parameters.message += (*iterator).second->character->getName ();
              parameters.message += ACE_TEXT_ALWAYS_CHAR (" received ");
              converter << xp;
              parameters.message += converter.str ();
              parameters.message += ACE_TEXT_ALWAYS_CHAR (" XP");
              notifications.push_back (std::make_pair (COMMAND_E2C_MESSAGE,
                                                       parameters));
              parameters.entity_id = 0;
              parameters.message.clear ();

              // level up ?
              if (level_up)
              {
                parameters.entity_id = (*iterator).first;
                parameters.subclass = subclass_e;
                notifications.push_back (std::make_pair (COMMAND_E2C_ENTITY_LEVEL_UP,
                                                         parameters));
                parameters.entity_id = 0;
                parameters.subclass = RPG_COMMON_SUBCLASS_INVALID;

                parameters.entity_id = (*iterator).first;
                parameters.message += (*iterator).second->character->getName ();
                parameters.message += ACE_TEXT_ALWAYS_CHAR (" gained a level ");
                notifications.push_back (std::make_pair (COMMAND_E2C_MESSAGE,
                                                         parameters));
                parameters.entity_id = 0;
                parameters.message.clear ();
              } // end IF
            } // end IF
            else
            {
              notifications.push_back (std::make_pair (COMMAND_E2C_MESSAGE,
                                                       parameters));
              parameters.message.clear ();
            } // end ELSE

            remove_id = (*target).first;
          } // end IF

          break;
        }
        case COMMAND_DOOR_CLOSE:
        case COMMAND_DOOR_OPEN:
        {
          // notify client window ?
          if (handleDoor (current_action.position,
                          (current_action.command == COMMAND_DOOR_OPEN)))
          {
            struct RPG_Engine_ClientNotificationParameters parameters;
            parameters.entity_id = (*iterator).first;
            parameters.condition = RPG_COMMON_CONDITION_INVALID;
            parameters.positions.insert(current_action.position);
            parameters.previous_position =
              std::make_pair (std::numeric_limits<unsigned int>::max(),
                              std::numeric_limits<unsigned int>::max());
            notifications.push_back (std::make_pair (current_action.command,
                                                     parameters));
          } // end IF

          // update seen positions ?
          if (current_action.command == COMMAND_DOOR_OPEN)
          {
            RPG_Map_Positions_t visible_positions;
            RPG_Engine_SeenPositionsIterator_t iterator_2 =
              seenPositions_.find ((*iterator).first);
            ACE_ASSERT (iterator_2 != seenPositions_.end ());
            getVisiblePositions ((*iterator).first,
                                 visible_positions,
                                 false);
            (*iterator_2).second.insert (visible_positions.begin (),
                                         visible_positions.end ());
          } // end IF

          break;
        }
        case COMMAND_IDLE:
        {
          action_complete = false; // *NOTE*: handled by the AI

          break;
        }
        case COMMAND_RUN:
        {
          struct RPG_Engine_ClientNotificationParameters parameters;
          parameters.entity_id = (*iterator).first;

          // toggle mode
          bool is_running =
              ((*iterator).second->modes.find (ENTITYMODE_RUNNING) != (*iterator).second->modes.end ());
          if (is_running)
          {
            // stop running

            // notify AI
            unsigned char temp =
                (*iterator).second->character->getSpeed (false,
                                                         inherited2::myMetaData.environment.lighting);
            temp /= RPG_ENGINE_FEET_PER_SQUARE;
            temp *= RPG_ENGINE_ROUND_INTERVAL;
            float squares_per_round = temp;
            squares_per_round = (1.0F / squares_per_round);
            squares_per_round *= static_cast<float> (RPG_ENGINE_SPEED_MODIFIER);
            float fractional = std::modf (squares_per_round,
                                          &squares_per_round);
            RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->reschedule ((*iterator).first,
                                                                         ACE_Time_Value (static_cast<time_t> (squares_per_round),
                                                                                         static_cast<suseconds_t> (fractional * 1000000.0F)));

            (*iterator).second->modes.erase (ENTITYMODE_RUNNING);

            // notify client
            parameters.message += (*iterator).second->character->getName ();
            parameters.message += ACE_TEXT_ALWAYS_CHAR (" stopped running");
          } // end IF
          else
          {
            // start running

            // notify AI
            unsigned char temp =
              (*iterator).second->character->getSpeed (true,
                                                       inherited2::myMetaData.environment.lighting);
            temp /= RPG_ENGINE_FEET_PER_SQUARE;
            temp *= RPG_ENGINE_ROUND_INTERVAL;
            float squares_per_round = temp;
            squares_per_round = 1.0F / squares_per_round;
            squares_per_round /= static_cast<float> (RPG_ENGINE_SPEED_MODIFIER);
            float fractional = std::modf (squares_per_round,
                                          &squares_per_round);
            RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance ()->reschedule ((*iterator).first,
                                                                         ACE_Time_Value (static_cast<time_t> (squares_per_round),
                                                                                         static_cast<suseconds_t> (fractional * 1000000.0F)));

            (*iterator).second->modes.insert (ENTITYMODE_RUNNING);

            // notify client
            parameters.message += (*iterator).second->character->getName ();
            parameters.message += ACE_TEXT_ALWAYS_CHAR (" started running");
          } // end ELSE
          notifications.push_back (std::make_pair (COMMAND_E2C_MESSAGE,
                                                   parameters));
          parameters.entity_id = 0;
          parameters.message.clear ();

          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("\"%s\" %s running...\n"),
                      ACE_TEXT ((*iterator).second->character->getName ().c_str ()),
                      (is_running ? ACE_TEXT ("stopped") : ACE_TEXT ("started"))));

          break;
        }
        case COMMAND_SEARCH:
        {
          // toggle mode
          bool is_searching =
              ((*iterator).second->modes.find (ENTITYMODE_SEARCHING) != (*iterator).second->modes.end ());
          if (is_searching)
          {
            // stop searching
            (*iterator).second->modes.erase (ENTITYMODE_SEARCHING);
          } // end IF
          else
          {
            // start searching
            (*iterator).second->modes.insert (ENTITYMODE_SEARCHING);
          } // end ELSE

          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("\"%s\" %s searching...\n"),
                      ACE_TEXT ((*iterator).second->character->getName ().c_str ()),
                      (is_searching ? ACE_TEXT ("stopped") : ACE_TEXT ("started"))));

          break;
        }
        case COMMAND_STEP:
        {
          // position blocked ?
          if (isBlocked (current_action.position))
          {
            if ((*iterator).second->modes.find (ENTITYMODE_TRAVELLING) !=
                (*iterator).second->modes.end ())
            {
              // *NOTE*: --> no/invalid path, cannot proceed...
              (*iterator).second->modes.erase (ENTITYMODE_TRAVELLING);

              // remove all queued steps + travel action
              while ((*iterator).second->actions.front ().command == COMMAND_STEP)
                (*iterator).second->actions.pop_front ();
              ACE_ASSERT ((*iterator).second->actions.front ().command == COMMAND_TRAVEL);
            } // end IF

            action_complete = true;

            break;
          } // end IF

          // OK: take a step...
          (*iterator).second->position = current_action.position;

          // step1: entity has moved, update seen positions
          RPG_Map_Positions_t positions;
          getVisiblePositions ((*iterator).first,
                               positions,
                               false);
//          { ACE_Guard<ACE_Thread_Mutex> aGuard (myLock);
            RPG_Engine_SeenPositionsIterator_t iterator_2 =
              seenPositions_.find ((*iterator).first);
            ACE_ASSERT (iterator_2 != seenPositions_.end ());
            (*iterator_2).second.insert (positions.begin (),
                                         positions.end ());
//          } // end lock scope

          // notify client window
          struct RPG_Engine_ClientNotificationParameters parameters;
          parameters.entity_id = (*iterator).first;
          parameters.condition = RPG_COMMON_CONDITION_INVALID;
          parameters.positions.insert (current_action.position);
          parameters.previous_position = (*iterator).second->position;
          notifications.push_back (std::make_pair (COMMAND_E2C_ENTITY_POSITION,
                                                   parameters));

          break;
        }
        case COMMAND_STOP:
        {
          (*iterator).second->modes.clear ();

          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("\"%s\" stopped...\n"),
                      ACE_TEXT ((*iterator).second->character->getName ().c_str ())));

          // *TODO*: stop ALL activities !

          break;
        }
        case COMMAND_TRAVEL:
        {
          action_complete = false; // *NOTE*: handled by the AI

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid action (ID: %u, was: \"%s\"), continuing\n"),
                      (*iterator).first,
                      ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (current_action.command).c_str ())));

          break;
        }
      } // end SWITCH

      if (action_complete)
        (*iterator).second->actions.pop_front ();
    } // end FOR
  } // end lock scope

  // notify client / window
  for (RPG_Engine_ClientNotificationsConstIterator_t iterator = notifications.begin ();
       iterator != notifications.end ();
       iterator++)
  {
    try {
      client_->notify ((*iterator).first,
                       (*iterator).second,
                       true);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                  ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString ((*iterator).first).c_str ())));
    }
  } // end FOR

  // remove entity ?
  if (remove_id)
  {
    remove (remove_id);

    // has active entity left the game ?
    RPG_Engine_EntityID_t active_entity_id = getActive (true);
    if (remove_id == active_entity_id)
    {
      // notify client
      struct RPG_Engine_ClientNotificationParameters parameters;
      parameters.entity_id = 0;
      parameters.condition = RPG_COMMON_CONDITION_INVALID;
      parameters.positions.insert (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                   std::numeric_limits<unsigned int>::max ()));
      parameters.previous_position =
          std::make_pair (std::numeric_limits<unsigned int>::max (),
                          std::numeric_limits<unsigned int>::max ());
      try {
        client_->notify (COMMAND_E2C_QUIT,
                         parameters,
                         true);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                    ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (COMMAND_E2C_QUIT).c_str ())));
      }
    } // end IF
  } // end IF
}

//////////////////////////////////////////

bool
RPG_Engine::distance_sort_t::operator() (const RPG_Engine_EntityID_t& id1_in,
                                         const RPG_Engine_EntityID_t& id2_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Engine::distance_sort_t::operator()"));

  // sanity check(s)
  ACE_ASSERT (engine);

  RPG_Map_Position_t position_1, position_2;
  position_1 = engine->getPosition (id1_in, locked_access);
  position_2 = engine->getPosition (id2_in, locked_access);

  unsigned int distance_1, distance_2;
  distance_1 = RPG_Map_Common_Tools::distance (position_1, reference_position);
  distance_2 = RPG_Map_Common_Tools::distance (position_2, reference_position);

  return (distance_1 < distance_2);
}
