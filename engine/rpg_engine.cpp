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

#include "rpg_engine_defines.h"
#include "rpg_engine_event_manager.h"
#include "rpg_engine_common_tools.h"

#include <rpg_map_common_tools.h>
#include <rpg_map_pathfinding_tools.h>

#include <rpg_item_dictionary.h>
#include <rpg_item_common_tools.h>

#include <rpg_character_race_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>

#include <rpg_dice_common.h>
#include <rpg_dice.h>

#include <ace/Log_Msg.h>

// init statics
ACE_Atomic_Op<ACE_Thread_Mutex, RPG_Engine_EntityID_t> RPG_Engine::myCurrentID = 1;

RPG_Engine::RPG_Engine()
 : myQueue(RPG_ENGINE_DEF_MAX_QUEUE_SLOTS),
//    myEntities(),
   myActivePlayer(0),
   myClient(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::RPG_Engine"));

  // use member message queue...
  inherited::msg_queue(&myQueue);

  // set group ID for worker thread(s)
  inherited::grp_id(RPG_ENGINE_DEF_TASK_GROUP_ID);
}

RPG_Engine::~RPG_Engine()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::~RPG_Engine"));

  // clean up
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
RPG_Engine::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::close"));

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
                 ACE_TEXT("(state engine) worker thread (ID: %t) leaving...\n")));

      // don't do anything...
      break;
    }
    case 1:
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("should never get here, returning\n")));

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
RPG_Engine::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::svc"));

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
                     ACE_TEXT("received an unknown control message (type: %d), continuing\n"),
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

    // process (one round of) entity actions
    handleEntities();
  } // end WHILE

  // SHOULD NEVER-EVER GET HERE !
  ACE_ASSERT(false);

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
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;

  // *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
  int ret = 0;
  ret = inherited::activate((THR_NEW_LWP |
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
                            thread_ids);                 // thread id(s)
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
  // ...spawn roaming monsters
  RPG_Engine_Event spawn_event;
  spawn_event.type = EVENT_ENTITY_SPAWN;
  ACE_ASSERT(inherited2::myLevelMeta.spawn_timer == -1);
  if (inherited2::myLevelMeta.spawn_interval != ACE_Time_Value::zero)
  {
    inherited2::myLevelMeta.spawn_timer = RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->schedule(spawn_event,
                                                                                                   inherited2::myLevelMeta.spawn_interval,
                                                                                                   false);
    ACE_ASSERT(inherited2::myLevelMeta.spawn_timer != -1);
    if (inherited2::myLevelMeta.spawn_timer == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule spawn timer, continuing\n")));
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
  ACE_ASSERT(inherited2::myLevelMeta.spawn_timer != -1);
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->remove(inherited2::myLevelMeta.spawn_timer);
  inherited2::myLevelMeta.spawn_timer = -1;
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->stop();

  // drop control message into the queue...
  ACE_Message_Block* stop_mb = NULL;
  stop_mb = new(std::nothrow) ACE_Message_Block(0,                                  // size
                                                ACE_Message_Block::MB_STOP,         // type
                                                NULL,                               // continuation
                                                NULL,                               // data
                                                NULL,                               // buffer allocator
                                                NULL,                               // locking strategy
                                                ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
                                                ACE_Time_Value::zero,               // execution time
                                                ACE_Time_Value::max_time,           // deadline time
                                                NULL,                               // data block allocator
                                                NULL);                              // message allocator
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

    // clean up, what else can we do ?
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
             ACE_TEXT("worker thread has joined...\n")));

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
  if (ACE_Thread_Manager::instance()->wait_grp(RPG_ENGINE_DEF_TASK_GROUP_ID) == -1)
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
RPG_Engine::init(RPG_Engine_IWindow* client_in,
                 const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::init"));

  myClient = client_in;
  inherited2::init(level_in);

  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->init(this);
}

RPG_Engine_EntityID_t
RPG_Engine::add(RPG_Engine_Entity* entity_in)
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
  float steps_per_round = static_cast<float>(entity_in->character->getSpeed(inherited2::myLevelMeta.environment.lighting)) /
                          static_cast<float>(RPG_ENGINE_FEET_PER_SQUARE);
  steps_per_round /= static_cast<float>(RPG_ENGINE_ROUND_INTERVAL);
  float fracional = ::modf(steps_per_round, &steps_per_round);
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->add(id,
                                                      ACE_Time_Value(static_cast<time_t>(steps_per_round),
                                                                     static_cast<suseconds_t>(fracional * 1000000)));

  // notify client / window
  RPG_Engine_Command command = COMMAND_E2C_ENTITY_ADD;
  RPG_Engine_ClientParameters_t parameters;
  parameters.push_back(&id);
  parameters.push_back(&entity_in->sprite);
  try
  {
    myClient->notify(command,
                     parameters);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
               RPG_Engine_CommandHelper::RPG_Engine_CommandToString(command).c_str()));
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
  } // end lock scope

  // notify client / window
  RPG_Engine_Command command = COMMAND_E2C_ENTITY_REMOVE;
  RPG_Engine_ClientParameters_t parameters;
  parameters.push_back(&const_cast<RPG_Engine_EntityID_t&>(id_in));
  try
  {
    myClient->notify(command,
                     parameters);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
               RPG_Engine_CommandHelper::RPG_Engine_CommandToString(command).c_str()));
  }
}

void
RPG_Engine::action(const RPG_Engine_EntityID_t& id_in,
                   const RPG_Engine_Action& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::action"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    return;
  } // end IF

  switch (action_in.command)
  {
    case COMMAND_ATTACK:
    {
      // if the entity is currently attacking, change the previous target
      if ((*iterator).second->modes.find(ENTITYMODE_FIGHTING) != (*iterator).second->modes.end())
      {
        if ((*iterator).second->modes.find(ENTITYMODE_TRAVELLING) != (*iterator).second->modes.end())
        {
          // stop moving
          while ((*iterator).second->actions.front().command == COMMAND_STEP)
            (*iterator).second->actions.pop_front();

          ACE_ASSERT((*iterator).second->actions.front().command == COMMAND_TRAVEL);
          (*iterator).second->actions.pop_front();

          (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);
        } // end IF
        
        ACE_ASSERT((*iterator).second->actions.front().command == COMMAND_ATTACK);
        (*iterator).second->actions.pop_front();
        (*iterator).second->actions.push_front(action_in);

        // done
        return;
      } // end IF

      break;
    }
    case COMMAND_STOP:
    {
      (*iterator).second->actions.clear();

      // done
      return;
    }
    case COMMAND_TRAVEL:
    {
      // if the entity is currently travelling, change the previous destination
      if ((*iterator).second->modes.find(ENTITYMODE_TRAVELLING) != (*iterator).second->modes.end())
      {
        while ((*iterator).second->actions.front().command == COMMAND_STEP)
          (*iterator).second->actions.pop_front();

        ACE_ASSERT((*iterator).second->actions.front().command == COMMAND_TRAVEL);
        (*iterator).second->actions.pop_front();
        (*iterator).second->actions.push_front(action_in);

        // done
        return;
      } // end IF

      break;
    }
    default:
      break;
  } // end SWITCH

  (*iterator).second->actions.push_back(action_in);
}

void
RPG_Engine::setActive(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::setActive"));

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myActivePlayer = id_in;
  } // end lock scope

  // notify client window
  unsigned char visible_radius = getVisibleRadius(id_in);
  RPG_Engine_ClientParameters_t parameters;
  parameters.push_back(&visible_radius);
  try
  {
    myClient->notify(COMMAND_E2C_ENTITY_VISION_UPDATE, parameters);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
               RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_ENTITY_VISION_UPDATE).c_str()));
  }
}

RPG_Engine_EntityID_t
RPG_Engine::getActive() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getActive"));

  RPG_Engine_EntityID_t result = 0;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    result = myActivePlayer;
  } // end lock scope

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

  return ((*iterator).second->modes.find(mode_in) != (*iterator).second->modes.end());
}

//const SDL_Surface*
//RPG_Engine::getGraphics(const RPG_Engine_EntityID_t& id_in) const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Engine::getGraphics"));
//
//  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
//
//  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
//  if (iterator == myEntities.end())
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
//               id_in));
//
//    return NULL;
//  } // end IF
//
//  return (*iterator).second->graphic;
//}
//
//RPG_Engine_EntityGraphics_t
//RPG_Engine::getGraphics() const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Engine::getGraphics"));
//
//  RPG_Engine_EntityGraphics_t graphics;
//
//  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
//
//  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
//       iterator != myEntities.end();
//       iterator++)
//    graphics.insert(std::make_pair((*iterator).second->position, (*iterator).second->graphic));
//
//  return graphics;
//}

RPG_Map_Position_t
RPG_Engine::getPosition(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getPosition"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    return std::make_pair(std::numeric_limits<unsigned int>::max(),
                          std::numeric_limits<unsigned int>::max());
  } // end IF

  return (*iterator).second->position;
}

RPG_Map_Position_t
RPG_Engine::findValid(const RPG_Map_Position_t& center_in,
                      const unsigned int& radius_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::findValid"));

  // init return value(s)
  RPG_Map_Position_t result = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                             std::numeric_limits<unsigned int>::max());

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Map_Positions_t valid, occupied, possible;
  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    occupied.insert((*iterator).second->position);
  std::vector<RPG_Map_Position_t> difference;
  std::vector<RPG_Map_Position_t>::iterator difference_end;
  for (unsigned int i = 0;
       i < radius_in;
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
    ACE_DEBUG((LM_ERROR,
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
RPG_Engine::isBlocked(const RPG_Map_Position_t& position_in)
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
RPG_Engine::hasEntity(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::hasEntity"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    if ((*iterator).second->position == position_in)
      return (*iterator).first;

  return 0;
}

bool
RPG_Engine::isMonster(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::isMonster"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());

  return !(*iterator).second->character->isPlayerCharacter();
}

std::string
RPG_Engine::getName(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getName"));

  std::string result;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
    ACE_ASSERT(iterator != myEntities.end());

    result = (*iterator).second->character->getName();
  } // end lock scope

  return result;
}

unsigned char
RPG_Engine::getVisibleRadius(const RPG_Engine_EntityID_t& id_in) const
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
  RPG_Item_CommodityLight equipped_light_source = RPG_ITEM_COMMODITYLIGHT_INVALID;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
    ACE_ASSERT(iterator != myEntities.end());

    if ((*iterator).second->character->isPlayerCharacter())
    {
      RPG_Player_Player_Base* player_base = NULL;
      player_base = dynamic_cast<RPG_Player_Player_Base*>((*iterator).second->character);
      ACE_ASSERT(player_base);

      race = player_base->getRace();
      equipped_light_source = player_base->getEquipment().getLightSource();
    } // end IF
    else
    {
      RPG_Monster* monster = NULL;
      monster = dynamic_cast<RPG_Monster*>((*iterator).second->character);
      ACE_ASSERT(monster);

      equipped_light_source = monster->getEquipment().getLightSource();
    } // end ELSE
  } // end lock scope

  // step2: consider environment / ambient lighting
  unsigned char environment_radius = RPG_Common_Tools::environment2Radius(inherited2::myLevelMeta.environment);

  // step3: consider equipment (ambient lighting conditions)
  unsigned char lit_radius = 0;
  if (equipped_light_source != RPG_ITEM_COMMODITYLIGHT_INVALID)
    lit_radius = RPG_Item_Common_Tools::lightingItem2Radius(equipped_light_source,
                                                            (inherited2::myLevelMeta.environment.lighting == AMBIENCE_BRIGHT));

  // step4: consider low-light vision
  if (RPG_Character_Race_Common_Tools::hasRace(race, RACE_ELF) ||
      RPG_Character_Race_Common_Tools::hasRace(race, RACE_GNOME))
    lit_radius *= 2;

  result = ((environment_radius > lit_radius) ? environment_radius
                                              : lit_radius);

  // step5: consider darkvision
  if ((RPG_Character_Race_Common_Tools::hasRace(race, RACE_DWARF) ||
       RPG_Character_Race_Common_Tools::hasRace(race, RACE_ORC)) &&
      (result < 60))
    result = 60;

  result /= RPG_ENGINE_FEET_PER_SQUARE;

  return result;
}

void
RPG_Engine::getVisiblePositions(const RPG_Engine_EntityID_t& id_in,
                                RPG_Map_Positions_t& positions_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::getVisiblePositions"));
  
  // init return value(s)
  positions_out.clear();

  // sanity check
  if (id_in == 0)
    return; // done

  // step1: find "lit" positions
  RPG_Map_Position_t current_position = getPosition(id_in);
  RPG_Map_Common_Tools::buildCircle(current_position,
                                    getSize(),
                                    getVisibleRadius(id_in),
                                    true,
                                    positions_out);

  //// step2: remove any blocked (== unreachable) positions
  //// --> cannot see through walls / (closed) doors...
  //RPG_Map_Positions_t obstacles;
  //{
  //  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  //  obstacles = myMap.plan.walls;
  //  for (RPG_Map_DoorsConstIterator_t door_iterator = inherited2::myMap.plan.doors.begin();
  //       door_iterator != myMap.plan.doors.end();
  //       door_iterator++)
  //    if (!(*door_iterator).is_open)
  //      obstacles.insert((*door_iterator).position);
  //} // end lock scope
  //RPG_Map_Path_t path;
  //// *WARNING*: this works for associative containers ONLY
  //for (RPG_Map_PositionsConstIterator_t iterator = positions_out.begin();
  //     iterator != positions_out.end();
  //     iterator++)
  //  if (!inherited2::findPath(current_position,
  //                            *iterator,
  //                            obstacles,
  //                            path))
  //    positions_out.erase(iterator++);
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
                     RPG_Map_Path_t& path_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::findPath"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // obstacles:
  // - walls
  // - (closed, locked) doors
  RPG_Map_Positions_t obstacles = myMap.plan.walls;
  for (RPG_Map_DoorsConstIterator_t door_iterator = inherited2::myMap.plan.doors.begin();
       door_iterator != myMap.plan.doors.end();
       door_iterator++)
    if (!(*door_iterator).is_open)
      obstacles.insert((*door_iterator).position);
  // - entities
  for (RPG_Engine_EntitiesConstIterator_t entity_iterator = myEntities.begin();
       entity_iterator != myEntities.end();
       entity_iterator++)
    obstacles.insert((*entity_iterator).second->position);
  // - start, end are NEVER obstacles...
  obstacles.erase(start_in);
  obstacles.erase(end_in);

  return inherited2::findPath(start_in,
                              end_in,
                              obstacles,
                              path_out);
}

bool
RPG_Engine::canReach(const RPG_Engine_EntityID_t& id_in,
                     const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine::canReach"));

  // sanity check(s)
  ACE_ASSERT(id_in);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesConstIterator_t entity_iterator = myEntities.end();
  entity_iterator = myEntities.find(id_in);
  ACE_ASSERT(entity_iterator != myEntities.end());

  // step1: compute distance
  unsigned int range = RPG_Engine_Common_Tools::range((*entity_iterator).second->position,
                                                      position_in);

  // step2: compute entity reach
  unsigned char reach = 0;
  bool absolute_reach = false;

  if ((*entity_iterator).second->character->isPlayerCharacter())
  {
    RPG_Player_Player_Base* player_base = NULL;
    player_base = dynamic_cast<RPG_Player_Player_Base*>((*entity_iterator).second->character);
    ACE_ASSERT(player_base);

    reach = RPG_Common_Tools::sizeToReach(player_base->getSize());

    RPG_Item_WeaponType weapon_type = player_base->getEquipment().getPrimaryWeapon(player_base->getOffHand());
    const RPG_Item_WeaponProperties& properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_type);
    if (RPG_Item_Common_Tools::isMeleeWeapon(weapon_type))
    {
      if (properties.isReachWeapon)
      {
        reach *= 2;
        absolute_reach = RPG_Item_Common_Tools::hasAbsoluteReach(weapon_type);
      } // end IF
    } // end IF
    else
      reach = properties.rangeIncrement; // ranged weapon

    // compute max reach for ranged weapons
    if (RPG_Item_Common_Tools::isThrownWeapon(weapon_type))
      reach *= 5;
    else if (RPG_Item_Common_Tools::isProjectileWeapon(weapon_type))
      reach *= 10;
  } // end IF
  else
  {
    RPG_Monster* monster = NULL;
    monster = dynamic_cast<RPG_Monster*>((*entity_iterator).second->character);
    ACE_ASSERT(monster);

    const RPG_Monster_Size& monster_size = monster->getSize();
    reach = RPG_Common_Tools::sizeToReach(monster_size.size,
                                          monster_size.isTall);
  } // end IF

  return (absolute_reach ? (range == reach)
                         : (range <= reach));
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
  bool do_remove = false;
  RPG_Engine_EntityID_t entity_id = 0;
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
      RPG_Engine_Action& current_action = (*iterator).second->actions.front();
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
          RPG_Engine_EntitiesConstIterator_t target = myEntities.find(current_action.target);
          if ((target == myEntities.end()) ||
              !RPG_Map_Common_Tools::isAdjacent((*iterator).second->position,
                                                (*target).second->position))
          {
            (*iterator).second->modes.erase(ENTITYMODE_FIGHTING);

            break; // nothing to do...
          } // end IF

          action_complete = false;
          (*iterator).second->modes.insert(ENTITYMODE_FIGHTING);

          // *TODO*: implement combat situations, in-turn-movement, reach
          RPG_Engine_Common_Tools::attack((*iterator).second->character,
                                          (*target).second->character,
                                          ATTACK_NORMAL,
                                          DEFENSE_NORMAL,
                                          (current_action.command == COMMAND_ATTACK_FULL),
                                          RPG_COMBAT_DEF_ADJACENT_DISTANCE);

          // target helpless ?
          if (RPG_Engine_Common_Tools::isCharacterHelpless((*target).second->character))
            action_complete = true; // --> done fighting

          // target disabled ?
          if (RPG_Engine_Common_Tools::isCharacterDisabled((*target).second->character))
          {
            // remove entity
            do_remove = true;
            entity_id = (*target).first;

            if ((*target).first == myActivePlayer)
            {
              myActivePlayer = 0;

              // notify client window
              RPG_Engine_ClientParameters_t* parameters = NULL;
              parameters = new(std::nothrow) RPG_Engine_ClientParameters_t;
              if (!parameters)
              {
                ACE_DEBUG((LM_CRITICAL,
                           ACE_TEXT("unable to allocate memory, aborting\n")));

                return;
              } // end IF
              notifications.push_back(std::make_pair(COMMAND_E2C_QUIT, parameters));
            } // end ELSEIF
          } // end IF

          break;
        }
        case COMMAND_DOOR_CLOSE:
        case COMMAND_DOOR_OPEN:
        {
          bool toggled = false;
          handleDoor(current_action.position,
                     (current_action.command == COMMAND_DOOR_OPEN),
                     toggled);

          // notify client window ?
          if (toggled)
          {
            RPG_Engine_ClientParameters_t* parameters = NULL;
            parameters = new(std::nothrow) RPG_Engine_ClientParameters_t;
            if (!parameters)
            {
              ACE_DEBUG((LM_CRITICAL,
                         ACE_TEXT("unable to allocate memory, aborting\n")));

              return;
            } // end IF
            RPG_Map_Position_t* position = NULL;
            position = new(std::nothrow) RPG_Map_Position_t;
            if (!position)
            {
              ACE_DEBUG((LM_CRITICAL,
                         ACE_TEXT("unable to allocate memory, aborting\n")));

              // clean up
              delete parameters;

              return;
            } // end IF
            *position = current_action.position;
            parameters->push_back(position);
            notifications.push_back(std::make_pair(current_action.command, parameters));
          } // end IF

          break;
        }
        case COMMAND_SEARCH:
        {
          (*iterator).second->modes.insert(ENTITYMODE_SEARCHING);

          // debug info
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("\"%s\" is searching...\n"),
                     (*iterator).second->character->getName().c_str()));

          break;
        }
        case COMMAND_STEP:
        {
          // position blocked ?
          if (isBlocked(current_action.position))
          {
            // *NOTE*: --> no/invalid path, cannot proceed...
            (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);

            // remove all queued steps + travel action
            while ((*iterator).second->actions.front().command == COMMAND_STEP)
              (*iterator).second->actions.pop_front();
            ACE_ASSERT((*iterator).second->actions.front().command == COMMAND_TRAVEL);
            (*iterator).second->actions.pop_front();
            action_complete = false;

            break;
          } // end IF

          // OK: take a step...
          (*iterator).second->position = current_action.position;

          // notify client window
          RPG_Engine_ClientParameters_t* parameters = NULL;
          parameters = new(std::nothrow) RPG_Engine_ClientParameters_t;
          if (!parameters)
          {
            ACE_DEBUG((LM_CRITICAL,
                       ACE_TEXT("unable to allocate memory, aborting\n")));

            return;
          } // end IF
          RPG_Engine_EntityID_t* entity_id = NULL;
          entity_id = new(std::nothrow) RPG_Engine_EntityID_t;
          if (!entity_id)
          {
            ACE_DEBUG((LM_CRITICAL,
                       ACE_TEXT("unable to allocate memory, aborting\n")));

            // clean up
            delete parameters;

            return;
          } // end IF
          *entity_id = (*iterator).first;
          parameters->push_back(entity_id);
          RPG_Map_Position_t* position = NULL;
          position = new(std::nothrow) RPG_Map_Position_t;
          if (!position)
          {
            ACE_DEBUG((LM_CRITICAL,
                       ACE_TEXT("unable to allocate memory, aborting\n")));

            // clean up
            delete entity_id;
            delete parameters;

            return;
          } // end IF
          *position = (*iterator).second->position;
          parameters->push_back(position);
          notifications.push_back(std::make_pair(COMMAND_E2C_ENTITY_POSITION_UPDATE, parameters));

          break;
        }
        case COMMAND_STOP:
        {
          (*iterator).second->modes.clear();

          // debug info
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("\"%s\" stopped...\n"),
                     (*iterator).second->character->getName().c_str()));

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
                     RPG_Engine_CommandHelper::RPG_Engine_CommandToString(current_action.command).c_str()));

          break;
        }
      } // end SWITCH

      if (action_complete)
        (*iterator).second->actions.pop_front();
    } // end FOR
  } // end lock scope

  // remove entity ?
  if (do_remove)
  {
    ACE_ASSERT(entity_id);
    remove(entity_id);
  } // end IF

  // notify client / window
  for (RPG_Engine_ClientNotificationsConstIterator_t iterator = notifications.begin();
       iterator != notifications.end();
       iterator++)
  {
    try
    {
      myClient->notify((*iterator).first, *(*iterator).second);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                 RPG_Engine_CommandHelper::RPG_Engine_CommandToString((*iterator).first).c_str()));
    }
  } // end FOR
  // clean up
  for (RPG_Engine_ClientNotificationsConstIterator_t iterator = notifications.begin();
       iterator != notifications.end();
       iterator++)
  {
    for (RPG_Engine_ClientParametersConstIterator_t iterator2 = (*iterator).second->begin();
         iterator2 != (*iterator).second->end();
         iterator2++)
      delete (*iterator2);

    ACE_ASSERT((*iterator).second);
    delete (*iterator).second;
  } // end FOR
}
