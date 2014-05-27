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

#include <ace/Log_Msg.h>

#include "rpg_dice_common.h"
#include "rpg_dice.h"

#include "rpg_common_macros.h"
#include "rpg_common.h"
#include "rpg_common_tools.h"

#include "rpg_character_race_common_tools.h"

#include "rpg_item_dictionary.h"
#include "rpg_item_common_tools.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_net_defines.h"
#include "rpg_net_client_connector.h"
#include "rpg_net_client_asynchconnector.h"

#include "rpg_engine_defines.h"
#include "rpg_engine_event_manager.h"
#include "rpg_engine_common_tools.h"

// init statics
// *TODO* --> typedef
ACE_Atomic_Op<ACE_Thread_Mutex,
              RPG_Engine_EntityID_t> RPG_Engine::myCurrentID = 1;

RPG_Engine::RPG_Engine()
 : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
//    myEntities(),
   myActivePlayer(0),
   myClient(NULL),
   myConnector(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::RPG_Engine"));

  // use member message queue...
  inherited::msg_queue(&myQueue);

  // set group ID for worker thread(s)
  inherited::grp_id(RPG_ENGINE_TASK_GROUP_ID);

  // init network connector
  if (RPG_NET_USES_REACTOR)
    ACE_NEW_NORETURN(myConnector,
                     RPG_Net_Client_Connector());
  else
    ACE_NEW_NORETURN(myConnector,
                     RPG_Net_Client_AsynchConnector());
  if (!myConnector)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return;
  } // end IF
}

RPG_Engine::~RPG_Engine()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::~RPG_Engine"));

	if (isRunning())
		stop();

  // clean up
	delete myConnector;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
    for (RPG_Engine_EntitiesIterator_t iterator = myEntities.begin();
         iterator != myEntities.end();
         iterator++)
      if (!(*iterator).second->character->isPlayerCharacter())
      {
        // clean up NPC entities...
        delete (*iterator).second->character;
        delete (*iterator).second;
      } // end IF
  } // end lock scope
}

int
RPG_Engine::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::open"));

  ACE_UNUSED_ARG(args_in);

  // *IMPORTANT NOTE*: the first time around, the queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state, which
  // is expected.
  // Subsequently (i.e. after stop()ping/start()ing, the queue
  // will have been deactivate()d in the process, and getq() (see svc()) will
  // fail
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
RPG_Engine::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::close"));

  // *IMPORTANT NOTE*: this method may be invoked
  // [- by an external thread closing down the active object (arg_in: 1)]
  // - by the worker thread which calls this after returning from svc() (arg_in: 0)
  //   --> in this case, this should be a NOP...
  switch (arg_in)
  {
    case 0:
    {
      // called from ACE_Task_Base on clean-up
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("(state engine) worker thread (ID: %t) leaving...\n")));

      return 0;
    }
    case 1:
    {
      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument (was: %u), returning\n"),
                 arg_in));

      break;
    }
  } // end SWITCH

  ACE_ASSERT(false);
#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

int
RPG_Engine::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::svc"));

  ACE_Message_Block* ace_mb = NULL;
	int result = -1;
  while (true)
  {
		// step1: wait for activity
		ace_mb = NULL;
		result = inherited::msg_queue()->peek_dequeue_head(ace_mb,
			                                                 NULL); // block
		if (result == -1)
		{
			if (inherited::msg_queue()->deactivated())
			{
				ACE_DEBUG((LM_WARNING,
					         ACE_TEXT("message queue was deactivated, aborting\n")));

				break;
			} // end IF

			// queue has been pulsed --> proceed
		} // end IF
		else
		{
			// OK: message has arrived...
			ACE_ASSERT(result > 0);
			ace_mb = NULL;
			result =
					inherited::getq(ace_mb,
													const_cast<ACE_Time_Value*>(&ACE_Time_Value::zero)); // don't block
			if (result == -1)
			{
				ACE_DEBUG((LM_ERROR,
										ACE_TEXT("failed to ACE_Task::getq: \"%m\", aborting\n")));

				break;
			} // end IF
			ACE_ASSERT(ace_mb);

			switch (ace_mb->msg_type())
      {
        // *NOTE*: currently, only use these...
        case ACE_Message_Block::MB_STOP:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("received MB_STOP...\n")));

					// clean up
					ace_mb->release();

          return 0; // done
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("received an unknown/invalid control message (type: %d), continuing\n"),
                     ace_mb->msg_type()));

          break;
        }
      } // end SWITCH

			// clean up
			ace_mb->release();
		} // end ELSE

    // step2: process (one round of) entity actions
    handleEntities();
  } // end WHILE

	return -1;
}

void
RPG_Engine::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::start"));

  // sanity check
  if (isRunning())
    return;

  // OK: start worker thread
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
	const char* thread_names[1];
	thread_names[0] = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_TASK_THREAD_NAME);
	int ret = inherited::activate((THR_NEW_LWP  |
																 THR_JOINABLE |
																 THR_INHERIT_SCHED),         // flags
																1,                           // number of threads
																0,                           // force spawning
																ACE_DEFAULT_THREAD_PRIORITY, // priority
																inherited::grp_id(),         // group id --> has been set (see above)
																NULL,                        // corresp. task --> use 'this'
																thread_handles,              // thread handle(s)
																NULL,                        // thread stack(s)
																NULL,                        // thread stack size(s)
																thread_ids,                  // thread id(s)
																thread_names);               // thread names(s)
  if (ret == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));
  else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("(state engine) started worker thread (group: %d, id: %u)...\n"),
               inherited::grp_id(),
               thread_ids[0]));

  // start AI...
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->start();
  // ...spawn roaming monsters ?
  if (inherited2::myMetaData.spawn_interval != ACE_Time_Value::zero)
  {
		RPG_Engine_Event_t* spawn_event = NULL;
		ACE_NEW_NORETURN(spawn_event,
		                 RPG_Engine_Event_t());
		if (!spawn_event)
		{
		  ACE_DEBUG((LM_CRITICAL,
		             ACE_TEXT("failed to allocate memory, aborting\n")));

		  return;
		} // end IF
		spawn_event->type = EVENT_ENTITY_SPAWN;
		spawn_event->entity_id = -1;
		spawn_event->timer_id = -1;

		ACE_ASSERT(inherited2::myMetaData.spawn_timer == -1);
		// *NOTE*: fire&forget API for spawn_event
		inherited2::myMetaData.spawn_timer =
				RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->schedule(spawn_event,
																																 inherited2::myMetaData.spawn_interval,
																																 false);
    if (inherited2::myMetaData.spawn_timer == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule spawn event, continuing\n")));

		ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("scheduled spawn event (ID: %d)...\n"),
							 inherited2::myMetaData.spawn_timer));
  } // end IF
}

void
RPG_Engine::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::stop"));

  // sanity check
  if (!isRunning())
    return;

  // stop AI (&& monster spawning)
  ACE_ASSERT(inherited2::myMetaData.spawn_timer != -1);
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->cancel(inherited2::myMetaData.spawn_timer);
	ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("cancelled spawn event (ID: %d)...\n"),
						inherited2::myMetaData.spawn_timer));
  inherited2::myMetaData.spawn_timer = -1;
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->stop();

  // drop control message into the queue...
  ACE_Message_Block* stop_mb = NULL;
  ACE_NEW_NORETURN(stop_mb,
                   ACE_Message_Block(0,                                  // size
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
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("unable to allocate memory, returning\n")));

    // *TODO*: what else can we do ?
    return;
  } // end IF

  // block, if necessary
  if (inherited::putq(stop_mb, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task::putq(): \"%m\", returning\n")));

    // clean up
    stop_mb->release();

    return;
  } // end IF

  // ... and wait for the worker thread to join
  if (inherited::wait() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", returning\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("joined (state engine) worker thread...\n")));

  clearEntityActions(0);
}

bool
RPG_Engine::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::isRunning"));

  return (inherited::thr_count() > 0);
}

void
RPG_Engine::wait_all()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::wait_all"));

  // ... wait for ALL worker(s) to join
  if (ACE_Thread_Manager::instance()->wait_grp(RPG_ENGINE_TASK_GROUP_ID) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Manager::wait_grp(): \"%m\", returning\n")));
}

void
RPG_Engine::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::dump_state"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("handling %u entities...\n"),
             myEntities.size()));

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
  {
    try
    {
      (*iterator).second->character->dump();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%@] caught exception in RPG_Character_Base::dump(), continuing\n"),
                 (*iterator).second->character));
    }
  } // end FOR
}

void
RPG_Engine::lock() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::lock"));

  myLock.acquire();
}

void
RPG_Engine::unlock() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::unlock"));

  myLock.release();
}

void
RPG_Engine::init(RPG_Engine_IClient* client_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::init"));

  // sanity check(s)
  ACE_ASSERT(client_in);

  myClient = client_in;

  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->init(this);
}

void
RPG_Engine::set(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::set"));

	// sanity check(s)
	ACE_ASSERT(myClient);

  inherited2::init(level_in);

  // notify client / window
  RPG_Engine_ClientNotificationParameters_t parameters;
  parameters.entity_id = 0;
	parameters.condition = RPG_COMMON_CONDITION_INVALID;
	parameters.position =
			std::make_pair(std::numeric_limits<unsigned int>::max(),
										 std::numeric_limits<unsigned int>::max());
	parameters.previous_position =
			std::make_pair(std::numeric_limits<unsigned int>::max(),
										 std::numeric_limits<unsigned int>::max());
	parameters.visible_radius = 0;
  parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  try
  {
    myClient->notify(COMMAND_E2C_INIT,
                     parameters);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
               ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_INIT).c_str())));
  }
}

RPG_Engine_EntityID_t
RPG_Engine::add(RPG_Engine_Entity_t* entity_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::add"));

  // sanity check
  ACE_ASSERT(entity_in);
  ACE_ASSERT(entity_in->character);
  ACE_ASSERT(myClient);

  RPG_Engine_EntityID_t id = myCurrentID++;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myEntities[id] = entity_in;
  } // end lock scope

  // notify AI
  unsigned char temp =
		entity_in->character->getSpeed(false,
                                   inherited2::myMetaData.environment.lighting);
  temp /= RPG_ENGINE_FEET_PER_SQUARE;
  temp *= RPG_ENGINE_ROUND_INTERVAL;
  float squares_per_round = temp;
  squares_per_round = (1.0F / squares_per_round);
  squares_per_round *= static_cast<float>(RPG_ENGINE_SPEED_MODIFIER);
  float fractional = std::modf(squares_per_round, &squares_per_round);
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->add(id,
                                                      ACE_Time_Value(static_cast<time_t>(squares_per_round),
                                                                     static_cast<suseconds_t>(fractional * 1000000.0F)));

  // notify client / window
  RPG_Engine_ClientNotificationParameters_t parameters;
  parameters.entity_id = id;
	parameters.condition = RPG_COMMON_CONDITION_INVALID;
	parameters.position =
			std::make_pair(std::numeric_limits<unsigned int>::max(),
										 std::numeric_limits<unsigned int>::max());
	parameters.previous_position =
			std::make_pair(std::numeric_limits<unsigned int>::max(),
										 std::numeric_limits<unsigned int>::max());
	parameters.visible_radius = 0;
  parameters.sprite = entity_in->sprite;
  try
  {
    myClient->notify(COMMAND_E2C_ENTITY_ADD,
                     parameters);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
               ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_ENTITY_ADD).c_str())));
  }

  return id;
}

void
RPG_Engine::remove(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::remove"));

  // sanity check
  ACE_ASSERT(id_in);

  // notify AI
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->remove(id_in);

  bool was_active = false;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Engine_EntitiesIterator_t iterator = myEntities.find(id_in);
    if (iterator == myEntities.end())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
                 id_in));

      return;
    } // end IF
    if (!(*iterator).second->character->isPlayerCharacter())
    {
      // clean up NPC entities...
      delete (*iterator).second->character;
      delete (*iterator).second;
    } // end IF
    myEntities.erase(iterator);

    if (id_in == myActivePlayer)
      was_active = true;
  } // end lock scope

  // notify client / window
  RPG_Engine_ClientNotificationParameters_t parameters;
  parameters.entity_id = id_in;
	parameters.condition = RPG_COMMON_CONDITION_INVALID;
	parameters.position =
			std::make_pair(std::numeric_limits<unsigned int>::max(),
										 std::numeric_limits<unsigned int>::max());
	parameters.previous_position =
			std::make_pair(std::numeric_limits<unsigned int>::max(),
										 std::numeric_limits<unsigned int>::max());
	parameters.visible_radius = 0;
	parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  try
  {
    myClient->notify(COMMAND_E2C_ENTITY_REMOVE,
                     parameters);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
               ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_ENTITY_REMOVE).c_str())));
  }

  // was active player ?
  if (was_active)
    setActive(0);
}

bool
RPG_Engine::exists(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::exists"));

  // sanity check
  ACE_ASSERT(id_in);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  return (myEntities.find(id_in) != myEntities.end());
}

void
RPG_Engine::action(const RPG_Engine_EntityID_t& id_in,
                   const RPG_Engine_Action_t& action_in,
                   const bool& lockedAccess_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::action"));

  // sanity check
  ACE_ASSERT(id_in);

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    if (lockedAccess_in)
      myLock.release();

    return;
  } // end IF

  switch (action_in.command)
  {
    case COMMAND_ATTACK:
    {
      // if the entity is currently attacking, change the previous target
      if ((*iterator).second->modes.find(ENTITYMODE_FIGHTING) !=
          (*iterator).second->modes.end())
      {
        if ((*iterator).second->modes.find(ENTITYMODE_TRAVELLING) !=
            (*iterator).second->modes.end())
        {
          // stop moving
          while ((*iterator).second->actions.front().command == COMMAND_STEP)
            (*iterator).second->actions.pop_front();

          ACE_ASSERT((*iterator).second->actions.front().command ==
                     COMMAND_TRAVEL);
          (*iterator).second->actions.pop_front();

          (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);
        } // end IF
        else
        {
          // stop attacking
          while (((*iterator).second->actions.front().command ==
                  COMMAND_ATTACK_FULL)                           ||
                 ((*iterator).second->actions.front().command ==
                  COMMAND_ATTACK_STANDARD))
            (*iterator).second->actions.pop_front();
        } // end ELSE

        ACE_ASSERT((*iterator).second->actions.front().command ==
                   COMMAND_ATTACK);
        (*iterator).second->actions.pop_front();
        (*iterator).second->actions.push_front(action_in);

        if (lockedAccess_in)
          myLock.release();

        // done
        return;
      } // end IF

      break;
    }
    case COMMAND_STOP:
    {
      (*iterator).second->actions.clear();

      if (lockedAccess_in)
        myLock.release();

      // done
      return;
    }
    case COMMAND_TRAVEL:
    {
      // if the entity is currently travelling, change the previous destination
      if ((*iterator).second->modes.find(ENTITYMODE_TRAVELLING) !=
          (*iterator).second->modes.end())
      {
        while ((*iterator).second->actions.front().command == COMMAND_STEP)
          (*iterator).second->actions.pop_front();

        ACE_ASSERT((*iterator).second->actions.front().command ==
                   COMMAND_TRAVEL);
        (*iterator).second->actions.pop_front();
        (*iterator).second->actions.push_front(action_in);

        if (lockedAccess_in)
          myLock.release();

        // done
        return;
      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH

  (*iterator).second->actions.push_back(action_in);

  if (lockedAccess_in)
    myLock.release();

	// wake up the engine
	inherited::msg_queue()->pulse();
}

void
RPG_Engine::setActive(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::setActive"));

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myActivePlayer = id_in;
  } // end lock scope

  // notify client ?
  if (id_in)
  {
    unsigned char visible_radius = getVisibleRadius(id_in);
    RPG_Engine_ClientNotificationParameters_t parameters;
    parameters.entity_id = id_in;
		parameters.condition = RPG_COMMON_CONDITION_INVALID;
		parameters.position =
				std::make_pair(std::numeric_limits<unsigned int>::max(),
											 std::numeric_limits<unsigned int>::max());
		parameters.previous_position =
				std::make_pair(std::numeric_limits<unsigned int>::max(),
											 std::numeric_limits<unsigned int>::max());
		parameters.visible_radius = visible_radius;
		parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
    try
    {
      myClient->notify(COMMAND_E2C_ENTITY_VISION, parameters);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                 ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_ENTITY_VISION).c_str())));
    }
  } // end IF
}

RPG_Engine_EntityID_t
RPG_Engine::getActive(const bool& lockedAcces_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getActive"));

  RPG_Engine_EntityID_t result = 0;

  if (lockedAcces_in)
    myLock.acquire();

  result = myActivePlayer;

  if (lockedAcces_in)
    myLock.release();

  return result;
}

void
RPG_Engine::mode(const RPG_Engine_EntityMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::mode"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // sanity check
  if (myActivePlayer == 0)
    return; // nothing to do...

  RPG_Engine_EntitiesIterator_t iterator = myEntities.find(myActivePlayer);
  ACE_ASSERT(iterator != myEntities.end());
  //RPG_Engine_PlayerModeConstIterator_t iterator2 = (*iterator)->modes.find(mode_in);
  //if (iterator2 != (*iterator)->modes.end())
  //  return; // nothing to do...

  (*iterator).second->modes.insert(mode_in);
}

void
RPG_Engine::clear(const RPG_Engine_EntityMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::clear"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // sanity check
  if (myActivePlayer == 0)
    return; // nothing to do...

  RPG_Engine_EntitiesIterator_t iterator = myEntities.find(myActivePlayer);
  ACE_ASSERT(iterator != myEntities.end());
  //RPG_Engine_PlayerModeConstIterator_t iterator2 = (*iterator)->modes.find(mode_in);
  //if (iterator2 == (*iterator)->modes.end())
  //  return; // nothing to do...

  (*iterator).second->modes.erase(mode_in);
}

bool
RPG_Engine::hasMode(const RPG_Engine_EntityMode& mode_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::hasMode"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // sanity check
  if (myActivePlayer == 0)
    return false;

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(myActivePlayer);
  ACE_ASSERT(iterator != myEntities.end());

  return ((*iterator).second->modes.find(mode_in) !=
          (*iterator).second->modes.end());
}

RPG_Map_Position_t
RPG_Engine::getPosition(const RPG_Engine_EntityID_t& id_in,
                        const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getPosition"));

  RPG_Map_Position_t result =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());
  if (iterator == myEntities.end())
  {
    if (lockedAccess_in)
      myLock.release();

    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    return result;
  } // end IF

  result = (*iterator).second->position;

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Position_t
RPG_Engine::findValid(const RPG_Map_Position_t& center_in,
                      const unsigned int& radius_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::findValid"));

  // init return value(s)
  RPG_Map_Position_t result =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Map_Positions_t valid, occupied, possible;
  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    occupied.insert((*iterator).second->position);
  std::vector<RPG_Map_Position_t> difference;
  std::vector<RPG_Map_Position_t>::iterator difference_end;
  unsigned int max_radius =
      ((myMap.plan.size_x > myMap.plan.size_x) ? myMap.plan.size_x
                                               : myMap.plan.size_y);
  for (unsigned int i = 0;
		   i < (radius_in ? radius_in : max_radius);
       i++)
  {
    valid.clear();

    // step1: retrieve valid positions in a specific radius
    inherited2::findValid(center_in,
                          i,
                          valid);

    // step2: remove any occupied positions
    possible.clear();
    difference.resize(valid.size());
    difference_end = std::set_difference(valid.begin(), valid.end(),
                                         occupied.begin(), occupied.end(),
                                         difference.begin());
    possible.insert(difference.begin(), difference_end);

    if (!possible.empty())
      break;
  } // end FOR

  if (possible.empty())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("could not find a valid position around [%u,%u] (radius: %u), aborting\n"),
               center_in.first, center_in.second,
               radius_in));

    return result; // failed
  } // end IF
  else if (possible.size() == 1)
  {
    ACE_ASSERT(*possible.begin() == center_in);
    return center_in; // exact
  } // end IF

  // step3: choose a random position
  RPG_Map_PositionsConstIterator_t iterator = possible.begin();
  RPG_Dice_RollResult_t roll_result;
  RPG_Dice::generateRandomNumbers(possible.size(),
                                  1,
                                  roll_result);
  std::advance(iterator, roll_result.front() - 1);
  result = *iterator;

  return result;
}

bool
RPG_Engine::isBlocked(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::isBlocked"));

  // blocked by walls, closed doors ... ?
  if (!inherited2::isValid(position_in))
    return false;

  // occupied ... ?
  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    if ((*iterator).second->position == position_in)
      return true;

  return false;
}

RPG_Engine_EntityID_t
RPG_Engine::hasEntity(const RPG_Map_Position_t& position_in,
                      const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::hasEntity"));

  RPG_Engine_EntityID_t result = 0;

  if (lockedAccess_in)
    myLock.acquire();

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    if ((*iterator).second->position == position_in)
    {
      result = (*iterator).first;

      if (lockedAccess_in)
        myLock.release();

      return result;
    } // end IF

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Engine_EntityList_t
RPG_Engine::entities(const RPG_Map_Position_t& position_in,
                     const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::entities"));

  RPG_Engine_EntityList_t result;

  if (lockedAccess_in)
    myLock.acquire();
  
  // step1: retrieve entities
  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
        iterator != myEntities.end();
        iterator++)
    result.push_back((*iterator).first);

  if (lockedAccess_in)
    myLock.release();
    
  // step2: sort entity list
  distance_sort_t distance_sort = {this, false, position_in};
  result.sort(distance_sort);

  return result;
}

bool
RPG_Engine::isMonster(const RPG_Engine_EntityID_t& id_in,
                      const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::isMonster"));

  bool result = false;

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    if (lockedAccess_in)
      myLock.release();

    // *CONSIDER*: --> false negative ?
    return result;
  } // end IF

  result = !(*iterator).second->character->isPlayerCharacter();

  if (lockedAccess_in)
    myLock.release();

  return result;
}

std::string
RPG_Engine::getName(const RPG_Engine_EntityID_t& id_in,
                    const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getName"));

  std::string result;

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    if (lockedAccess_in)
      myLock.release();

    // *CONSIDER*: --> false negative ?
    return result;
  } // end IF

  result = (*iterator).second->character->getName();

  if (lockedAccess_in)
    myLock.release();

  return result;
}

unsigned char
RPG_Engine::getVisibleRadius(const RPG_Engine_EntityID_t& id_in,
                             const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getVisibleRadius"));

  // init return value(s)
  unsigned char result = 0;

  // sanity check
  if (id_in == 0)
    return result;

  // step1: retrieve:
  // - race (if any)
  // - center position
  // - equiped light source (if any)
  RPG_Character_Race_t race(0);
  RPG_Item_CommodityLight equipped_light_source =
		RPG_ITEM_COMMODITYLIGHT_INVALID;

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    if (lockedAccess_in)
      myLock.release();

    return result;
  } // end IF

  if ((*iterator).second->character->isPlayerCharacter())
  {
    RPG_Player_Player_Base* player_base = NULL;
    try
    {
      player_base =
          dynamic_cast<RPG_Player_Player_Base*>((*iterator).second->character);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in dynamic_cast<RPG_Player_Player_Base*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      return result;
    }
    if (!player_base)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in dynamic_cast<RPG_Player_Player_Base*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      return result;
    } // end IF

    race = player_base->getRace();
    equipped_light_source = player_base->getEquipment().getLightSource();
  } // end IF
  else
  {
    RPG_Monster* monster = NULL;
    monster = dynamic_cast<RPG_Monster*>((*iterator).second->character);
    try
    {
      monster =
          dynamic_cast<RPG_Monster*>((*iterator).second->character);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in dynamic_cast<RPG_Monster*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      return result;
    }
    if (!monster)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in dynamic_cast<RPG_Monster*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      return result;
    } // end IF

    equipped_light_source = monster->getEquipment().getLightSource();
  } // end ELSE

  // step2: consider environment / ambient lighting
  unsigned short environment_radius =
		RPG_Common_Tools::environment2Radius(inherited2::myMetaData.environment);

  // step3: consider equipment (ambient lighting conditions)
  unsigned short lit_radius = 0;
  if (equipped_light_source != RPG_ITEM_COMMODITYLIGHT_INVALID)
    lit_radius =
        RPG_Item_Common_Tools::lightingItem2Radius(equipped_light_source,
                                                   (inherited2::myMetaData.environment.lighting == AMBIENCE_BRIGHT));

  if (lockedAccess_in)
    myLock.release();

  // step4: consider low-light vision
  // *TODO*: consider monsters as well...
  if (RPG_Character_Race_Common_Tools::hasRace(race, RACE_ELF)   ||
      RPG_Character_Race_Common_Tools::hasRace(race, RACE_GNOME))
    lit_radius *= 2;

  result =
		((environment_radius > lit_radius) ? static_cast<unsigned char>(environment_radius)
                                       : static_cast<unsigned char>(lit_radius));

  // step5: consider darkvision
  // *TODO*: consider monsters as well...
  if ((RPG_Character_Race_Common_Tools::hasRace(race, RACE_DWARF) ||
       RPG_Character_Race_Common_Tools::hasRace(race, RACE_ORC)) &&
      (result < 60))
    result = 60;

  result /= RPG_ENGINE_FEET_PER_SQUARE;

  return result;
}

void
RPG_Engine::getVisiblePositions(const RPG_Engine_EntityID_t& id_in,
                                RPG_Map_Positions_t& positions_out,
                                const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getVisiblePositions"));
  
  // init return value(s)
  positions_out.clear();

  // sanity check
  if (id_in == 0)
    return; // done
 
  if (lockedAccess_in)
    myLock.acquire();

  // step1: find "lit" positions
  RPG_Map_Position_t current_position = getPosition(id_in, false);
  RPG_Map_Common_Tools::buildCircle(current_position,
                                    inherited2::getSize(),
                                    getVisibleRadius(id_in, false),
                                    true,
                                    positions_out);

  // step2: remove any blocked (== unreachable) positions
  // --> cannot see through walls / (closed) doors...
  RPG_Map_Positions_t obstacles = inherited2::getObstacles();

  if (lockedAccess_in)
    myLock.release();

  // *WARNING*: this works for associative containers ONLY
  for (RPG_Map_PositionsConstIterator_t iterator = positions_out.begin();
       iterator != positions_out.end();
       )
    if (RPG_Map_Common_Tools::hasLineOfSight(current_position,
                                             *iterator,
                                             obstacles,
                                             true))
      iterator++;
    else
      positions_out.erase(iterator++);
}

bool
RPG_Engine::canSee(const RPG_Engine_EntityID_t& id_in,
                   const RPG_Map_Position_t& position_in,
                   const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::canSee"));

  // sanity check
  if (id_in == 0)
    return false; // *CONSIDER*: false negative ?
 
  if (lockedAccess_in)
    myLock.acquire();

  RPG_Map_Positions_t visible_positions;
  getVisiblePositions(id_in,
                      visible_positions,
                      false);

  if (lockedAccess_in)
    myLock.release();

  return (visible_positions.find(position_in) != visible_positions.end());
}

bool
RPG_Engine::canSee(const RPG_Engine_EntityID_t& id_in,
                   const RPG_Engine_EntityID_t& target_in,
                   const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::canSee"));

  // sanity check
  if ((id_in == 0) || (target_in == 0))
    return false; // *CONSIDER*: false negative ?
 
  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(target_in);
  ACE_ASSERT(iterator != myEntities.end());
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               target_in));

    if (lockedAccess_in)
      myLock.release();

    // *CONSIDER*: false negative ?
    return false;
  } // end IF

  // target using a light source ?
  // *TODO*: consider magic light, spells, ...
  bool target_equipped_a_light = false;
  if ((*iterator).second->character->isPlayerCharacter())
  {
    RPG_Player_Player_Base* player_base = NULL;
    try
    {
      player_base =
          dynamic_cast<RPG_Player_Player_Base*>((*iterator).second->character);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      // *CONSIDER*: false negative ?
      return false;
    }
    if (!player_base)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      // *CONSIDER*: false negative ?
      return false;
    } // end IF

    target_equipped_a_light =
			(player_base->getEquipment().getLightSource() != RPG_ITEM_COMMODITYLIGHT_INVALID);
  } // end IF
  else
  {
    RPG_Monster* monster = NULL;
    try
    {
      monster = dynamic_cast<RPG_Monster*>((*iterator).second->character);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to dynamic_cast<RPG_Monster*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      // *CONSIDER*: false negative ?
      return false;
    }
    if (!monster)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to dynamic_cast<RPG_Monster*>(%@), aborting\n"),
                 (*iterator).second->character));

      if (lockedAccess_in)
        myLock.release();

      // *CONSIDER*: false negative ?
      return false;
    } // end IF

    target_equipped_a_light =
			(monster->getEquipment().getLightSource() != RPG_ITEM_COMMODITYLIGHT_INVALID);
  } // end ELSE
  RPG_Map_Position_t target_position = (*iterator).second->position;
  if (!target_equipped_a_light)
  {
    if (lockedAccess_in)
      myLock.release();

    // can the source see the target position ?
    return canSee(id_in,
                  target_position,
                  lockedAccess_in);
  } // end IF

  RPG_Map_Position_t source_position = getPosition(id_in, false);
  RPG_Map_Positions_t obstacles = inherited2::getObstacles();

  if (lockedAccess_in)
    myLock.release();

  return RPG_Map_Common_Tools::hasLineOfSight(source_position,
                                              target_position,
                                              obstacles,
                                              false);
}

unsigned int
RPG_Engine::numSpawned() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::numSpawned"));

  unsigned int result = 0;

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    if ((*iterator).second->is_spawned)
      result++;

  return result;
}

bool
RPG_Engine::findPath(const RPG_Map_Position_t& start_in,
                     const RPG_Map_Position_t& end_in,
                     RPG_Map_Path_t& path_out,
                     const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::findPath"));

  if (lockedAccess_in)
    myLock.acquire();

  // obstacles:
  // - walls
  // - (closed, locked) doors
  RPG_Map_Positions_t obstacles = inherited2::getObstacles();
  // - entities
  for (RPG_Engine_EntitiesConstIterator_t entity_iterator = myEntities.begin();
       entity_iterator != myEntities.end();
       entity_iterator++)
    obstacles.insert((*entity_iterator).second->position);
  // - start, end are NEVER obstacles...
  obstacles.erase(start_in);
  obstacles.erase(end_in);

  if (lockedAccess_in)
    myLock.release();

  return inherited2::findPath(start_in,
                              end_in,
                              obstacles,
                              path_out);
}

bool
RPG_Engine::canReach(const RPG_Engine_EntityID_t& id_in,
                     const RPG_Map_Position_t& position_in,
                     const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::canReach"));

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    if (lockedAccess_in)
      myLock.release();

    // *CONSIDER*: false negative ?
    return false;
  } // end IF

  // step1: compute distance
  unsigned int range =
    RPG_Engine_Common_Tools::range((*iterator).second->position, position_in);

  // step2: compute entity reach
  unsigned short base_reach = 0;
  bool absolute_reach = false;
  unsigned int max_reach =
    (*iterator).second->character->getReach(base_reach, absolute_reach) / RPG_ENGINE_FEET_PER_SQUARE;

  if (lockedAccess_in)
    myLock.release();

  return (absolute_reach ? (range == max_reach)
                         : (range <= max_reach));
}

RPG_Engine_LevelMetaData_t
RPG_Engine::getMetaData(const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getMetaData"));

  RPG_Engine_LevelMetaData_t result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::getMetaData();

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Position_t
RPG_Engine::getStartPosition(const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getStartPosition"));

  RPG_Map_Position_t result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::getStartPosition();

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Size_t
RPG_Engine::getSize(const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getSize"));

  RPG_Map_Size_t result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::getSize();

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_DoorState
RPG_Engine::state(const RPG_Map_Position_t& position_in,
                  const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::state"));

  RPG_Map_DoorState result = RPG_MAP_DOORSTATE_INVALID;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::state(position_in);

  if (lockedAccess_in)
    myLock.release();

  return result;
}

bool
RPG_Engine::isValid(const RPG_Map_Position_t& position_in,
                    const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::isValid"));

  bool result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::isValid(position_in);

  if (lockedAccess_in)
    myLock.release();

  return result;
}

bool
RPG_Engine::isCorner(const RPG_Map_Position_t& position_in,
                     const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::isCorner"));

  bool result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::isCorner(position_in);

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Element
RPG_Engine::getElement(const RPG_Map_Position_t& position_in,
                       const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getElement"));

  RPG_Map_Element result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::getElement(position_in);

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getObstacles(const bool& includeEntities_in,
                         const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getObstacles"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::getObstacles();

  if (includeEntities_in)
    for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
         iterator != myEntities.end();
         iterator++)
      result.insert((*iterator).second->position);

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getWalls(const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getWalls"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in)
    myLock.acquire();

  result = inherited2::myMap.plan.walls;

  if (lockedAccess_in)
    myLock.release();

  return result;
}

RPG_Map_Positions_t
RPG_Engine::getDoors(const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getDoors"));

  RPG_Map_Positions_t result;

  if (lockedAccess_in)
    myLock.acquire();

  for (RPG_Map_DoorsConstIterator_t iterator = inherited2::myMap.plan.doors.begin();
       iterator != inherited2::myMap.plan.doors.end();
       iterator++)
    result.insert((*iterator).position);

  if (lockedAccess_in)
    myLock.release();

  return result;
}

void
RPG_Engine::clearEntityActions(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::clearEntityActions"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesIterator_t iterator;
  if (id_in)
  {
    iterator = myEntities.find(id_in);
    ACE_ASSERT(iterator != myEntities.end());
    (*iterator).second->actions.clear();
  } // end IF
  else
    for (iterator = myEntities.begin();
         iterator != myEntities.end();
         iterator++)
      (*iterator).second->actions.clear();
}

void
RPG_Engine::handleEntities()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::handleEntities"));

  bool action_complete = true;
  RPG_Engine_EntityID_t remove_id = 0;
  RPG_Engine_ClientNotifications_t notifications;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    for (RPG_Engine_EntitiesIterator_t iterator = myEntities.begin();
         iterator != myEntities.end();
         iterator++)
    {
      // *TODO*: check for actions/round limit
      if ((*iterator).second->actions.empty())
        continue;

      action_complete = true;
      RPG_Engine_Action_t& current_action = (*iterator).second->actions.front();
      switch (current_action.command)
      {
        case COMMAND_ATTACK:
        {
          action_complete = false; // *NOTE*: handled by the AI

          break;
        }
        case COMMAND_ATTACK_FULL:
        case COMMAND_ATTACK_STANDARD:
        {
          ACE_ASSERT(current_action.target);
          RPG_Engine_EntitiesConstIterator_t target =
						myEntities.find(current_action.target);
          if ((target == myEntities.end()) ||
              !canReach((*iterator).first,
                        (*target).second->position,
                        false))
            break; // nothing to do...

          // notify client
          RPG_Engine_ClientNotificationParameters_t parameters;
          parameters.entity_id = (*target).first;
					parameters.condition = RPG_COMMON_CONDITION_INVALID;
					parameters.position =
						std::make_pair(std::numeric_limits<unsigned int>::max(),
                           std::numeric_limits<unsigned int>::max());
					parameters.previous_position =
						std::make_pair(std::numeric_limits<unsigned int>::max(),
						               std::numeric_limits<unsigned int>::max());
					parameters.visible_radius = 0;
					parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
          // *TODO*: implement combat situations, in-turn-movement, ...
          bool hit =
						RPG_Engine_Common_Tools::attack((*iterator).second->character,
                                            (*target).second->character,
																						ATTACK_NORMAL,
																						DEFENSE_NORMAL,
																						(current_action.command == COMMAND_ATTACK_FULL),
																						(RPG_Engine_Common_Tools::range((*iterator).second->position, (*target).second->position) * RPG_ENGINE_FEET_PER_SQUARE));
          notifications.push_back(std::make_pair((hit ? COMMAND_E2C_ENTITY_HIT
                                                      : COMMAND_E2C_ENTITY_MISS),
                                                 parameters));

          // target disabled ? --> remove entity (see below)
          if (RPG_Engine_Common_Tools::isCharacterDisabled((*target).second->character))
          {
            // notify client
						parameters.condition = CONDITION_DISABLED;
            parameters.message = (*target).second->character->getName();
            parameters.message += ACE_TEXT_ALWAYS_CHAR(" has been disabled");
            notifications.push_back(std::make_pair(COMMAND_E2C_ENTITY_CONDITION,
                                                   parameters));
						parameters.condition = RPG_COMMON_CONDITION_INVALID;

            // award XP
            if (!(*target).second->character->isPlayerCharacter())
            {
              RPG_Player_Player_Base* player_base = NULL;
              try
              {
                player_base =
                    dynamic_cast<RPG_Player_Player_Base*>((*iterator).second->character);
              }
              catch (...)
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), continuing\n"),
                           (*iterator).second->character));

                break;
              }
              if (!player_base)
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), continuing\n"),
                           (*iterator).second->character));

								break;
							} // end IF
							unsigned int xp =
								RPG_Engine_Common_Tools::combat2XP((*target).second->character->getName(),
								                                   player_base->getLevel(),
																									 1,
																									 1);
							bool level_up = player_base->gainExperience(xp);

							parameters.entity_id = (*iterator).first;
              // append some more info to the message
              std::ostringstream converter;
              parameters.message += ACE_TEXT_ALWAYS_CHAR(", ");
              parameters.message += (*iterator).second->character->getName();
              parameters.message += ACE_TEXT_ALWAYS_CHAR(" received ");
              converter << xp;
              parameters.message += converter.str();
              parameters.message += ACE_TEXT_ALWAYS_CHAR(" XP");
	            notifications.push_back(std::make_pair(COMMAND_E2C_MESSAGE,
                                                     parameters));
							parameters.message.clear();

							// level up ?
							if (level_up)
								notifications.push_back(std::make_pair(COMMAND_E2C_ENTITY_LEVEL_UP,
                                                       parameters));
            } // end IF

            remove_id = (*target).first;
          } // end IF

          break;
        }
        case COMMAND_DOOR_CLOSE:
        case COMMAND_DOOR_OPEN:
        {
          // notify client window ?
          if (handleDoor(current_action.position,
                         (current_action.command == COMMAND_DOOR_OPEN)))
          {
            RPG_Engine_ClientNotificationParameters_t parameters;
            parameters.entity_id = (*iterator).first;
						parameters.condition = RPG_COMMON_CONDITION_INVALID;
            parameters.position = current_action.position;
   					parameters.previous_position =
							std::make_pair(std::numeric_limits<unsigned int>::max(),
							               std::numeric_limits<unsigned int>::max());
						parameters.visible_radius = 0;
						parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
						notifications.push_back(std::make_pair(current_action.command,
																									 parameters));
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
          // toggle mode
          bool is_running =
              ((*iterator).second->modes.find(ENTITYMODE_RUNNING) != (*iterator).second->modes.end());
          if (is_running)
          {
            // stop running

            // notify AI
            unsigned char temp =
                (*iterator).second->character->getSpeed(false, inherited2::myMetaData.environment.lighting);
            temp /= RPG_ENGINE_FEET_PER_SQUARE;
            temp *= RPG_ENGINE_ROUND_INTERVAL;
            float squares_per_round = temp;
            squares_per_round = (1.0F / squares_per_round);
            squares_per_round *= static_cast<float>(RPG_ENGINE_SPEED_MODIFIER);
            float fractional = std::modf(squares_per_round, &squares_per_round);
            RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->reschedule((*iterator).first,
                                                                       ACE_Time_Value(static_cast<time_t>(squares_per_round),
                                                                                      static_cast<suseconds_t>(fractional * 1000000.0F)));

            (*iterator).second->modes.erase(ENTITYMODE_RUNNING);
          } // end IF
          else
          {
            // start running

            // notify AI
            unsigned char temp =
							(*iterator).second->character->getSpeed(true,
							                                        inherited2::myMetaData.environment.lighting);
            temp /= RPG_ENGINE_FEET_PER_SQUARE;
            temp *= RPG_ENGINE_ROUND_INTERVAL;
            float squares_per_round = temp;
            squares_per_round = 1.0F / squares_per_round;
            squares_per_round /= static_cast<float>(RPG_ENGINE_SPEED_MODIFIER);
            float fractional = std::modf(squares_per_round, &squares_per_round);
            RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->reschedule((*iterator).first,
                                                                       ACE_Time_Value(static_cast<time_t>(squares_per_round),
                                                                                      static_cast<suseconds_t>(fractional * 1000000.0F)));

            (*iterator).second->modes.insert(ENTITYMODE_RUNNING);
          } // end ELSE

          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("\"%s\" %s running...\n"),
                     ACE_TEXT((*iterator).second->character->getName().c_str()),
                     (is_running ? ACE_TEXT("stopped") : ACE_TEXT("started"))));

          break;
        }
        case COMMAND_SEARCH:
        {
          // toggle mode
          bool is_searching =
              ((*iterator).second->modes.find(ENTITYMODE_SEARCHING) != (*iterator).second->modes.end());
          if (is_searching)
          {
            // stop searching
            (*iterator).second->modes.erase(ENTITYMODE_SEARCHING);
          } // end IF
          else
          {
            // start searching
            (*iterator).second->modes.insert(ENTITYMODE_SEARCHING);
          } // end ELSE

          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("\"%s\" %s searching...\n"),
                     ACE_TEXT((*iterator).second->character->getName().c_str()),
                     (is_searching ? ACE_TEXT("stopped") : ACE_TEXT("started"))));

          break;
        }
        case COMMAND_STEP:
        {
          // position blocked ?
          if (isBlocked(current_action.position))
          {
            if ((*iterator).second->modes.find(ENTITYMODE_TRAVELLING) !=
                (*iterator).second->modes.end())
            {
              // *NOTE*: --> no/invalid path, cannot proceed...
              (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);

              // remove all queued steps + travel action
              while ((*iterator).second->actions.front().command ==
                     COMMAND_STEP)
                (*iterator).second->actions.pop_front();
              ACE_ASSERT((*iterator).second->actions.front().command ==
                         COMMAND_TRAVEL);
            } // end IF

            action_complete = true;

            break;
          } // end IF

          // notify client window
					RPG_Engine_ClientNotificationParameters_t parameters;
          parameters.entity_id = (*iterator).first;
					parameters.condition = RPG_COMMON_CONDITION_INVALID;
          parameters.position = current_action.position;
					parameters.previous_position = (*iterator).second->position;
					parameters.visible_radius = 0;
					parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
          notifications.push_back(std::make_pair(COMMAND_E2C_ENTITY_POSITION,
						                                     parameters));

          // OK: take a step...
          (*iterator).second->position = current_action.position;

          break;
        }
        case COMMAND_STOP:
        {
          (*iterator).second->modes.clear();

          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("\"%s\" stopped...\n"),
                     ACE_TEXT((*iterator).second->character->getName().c_str())));

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
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid action (ID: %u, was: \"%s\"), continuing\n"),
                     (*iterator).first,
                     ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(current_action.command).c_str())));

          break;
        }
      } // end SWITCH

      if (action_complete)
        (*iterator).second->actions.pop_front();
    } // end FOR
  } // end lock scope

  // notify client / window
  for (RPG_Engine_ClientNotificationsConstIterator_t iterator = notifications.begin();
       iterator != notifications.end();
       iterator++)
  {
    try
    {
      myClient->notify((*iterator).first, (*iterator).second);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                 ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString((*iterator).first).c_str())));
    }
  } // end FOR

  // remove entity ?
  if (remove_id)
  {
    RPG_Engine_EntityID_t active_entity_id = getActive(true);
    remove(remove_id);

    // has active entity left the game ?
    if (remove_id == active_entity_id)
    {
      // notify client
      RPG_Engine_ClientNotificationParameters_t parameters;
      parameters.entity_id = 0;
      parameters.condition = RPG_COMMON_CONDITION_INVALID;
      parameters.position =
          std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max());
      parameters.previous_position =
          std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max());
      parameters.visible_radius = 0;
      parameters.sprite = RPG_GRAPHICS_SPRITE_INVALID;
      try
      {
        myClient->notify(COMMAND_E2C_QUIT, parameters);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                   ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_QUIT).c_str())));
      }
    } // end IF
  } // end IF
}

bool
RPG_Engine::distance_sort_t::operator()(const RPG_Engine_EntityID_t& id1_in,
                                        const RPG_Engine_EntityID_t& id2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::distance_sort_t::operator()"));

  // sanity check(s)
  ACE_ASSERT(engine);

  RPG_Map_Position_t position_1, position_2;
  position_1 = engine->getPosition(id1_in, locked_access);
  position_2 = engine->getPosition(id2_in, locked_access);

  unsigned int distance_1, distance_2;
  distance_1 = RPG_Map_Common_Tools::distance(position_1, reference_position);
  distance_2 = RPG_Map_Common_Tools::distance(position_2, reference_position);

  return (distance_1 < distance_2);
}
