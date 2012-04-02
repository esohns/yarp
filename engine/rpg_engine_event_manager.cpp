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
#include "rpg_engine_level.h"
#include "rpg_engine_common_tools.h"

#include <rpg_map_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_timerhandler.h>
#include <rpg_common_timer_manager.h>

#include <rpg_dice.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

RPG_Engine_Event_Manager::RPG_Engine_Event_Manager()
 : myEngine(NULL),
   myMaxNumSpawnedEntities(RPG_ENGINE_DEF_MAX_NUM_SPAWNED)//,
//   myTimers()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::RPG_Engine_Event_Manager"));

  // set group ID for worker thread(s)
  inherited::grp_id(RPG_ENGINE_DEF_AI_TASK_GROUP_ID);
}

RPG_Engine_Event_Manager::~RPG_Engine_Event_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::~RPG_Engine_Event_Manager"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EventTimerIDsConstIterator_t iterator = myTimers.begin();
       iterator != myTimers.end();
       iterator++)
  {
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer((*iterator).first);
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
  ACE_Time_Value     peek_delay(0, (RPG_ENGINE_EVENT_PEEK_INTERVAL * 1000));
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

    // trigger (one round of) entity actions (where appropriate)
    handleEntities();
  } // end WHILE

  ACE_NOTREACHED(ACE_TEXT("should never get here\n"));
  ACE_ASSERT(false);

  return -1;
}

void
RPG_Engine_Event_Manager::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::start"));

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
               ACE_TEXT("(AI engine) started worker thread (group: %d, id: %u)...\n"),
               inherited::grp_id(),
               thread_ids[0]));
}

void
RPG_Engine_Event_Manager::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::stop"));

  // sanity check
  if (!isRunning())
    return;

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
}

bool
RPG_Engine_Event_Manager::isRunning()
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
RPG_Engine_Event_Manager::init(const unsigned int& maxNumSpawnedEntities_in,
                               RPG_Engine_Level* engine_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::init"));

  ACE_ASSERT(engine_in);

  myEngine = engine_in;
  myMaxNumSpawnedEntities = maxNumSpawnedEntities_in;
}

int
RPG_Engine_Event_Manager::schedule(const RPG_Engine_Event& event_in,
                                   const ACE_Time_Value& interval_in,
                                   const bool& isOneShot_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::schedule"));

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

  int timer_id = -1;
  // *NOTE*: this is a fire-and-forget API (assumes resp. for timer_handler)...
  if (!RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->scheduleTimer(timer_handler,
                                                                    event_handle,
                                                                    interval_in,
                                                                    isOneShot_in,
                                                                    timer_id))
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
RPG_Engine_Event_Manager::remove(const int& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::remove"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EventTimerIDsConstIterator_t iterator = myTimers.find(id_in);
  if (iterator == myTimers.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid id (was: %d), aborting\n"),
               id_in));

    return;
  } // end IF

  RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancelTimer(id_in);
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
    case EVENT_SPAWN_MONSTER:
    {
      if ((myEngine->numSpawned() >= myMaxNumSpawnedEntities) ||
          !RPG_Dice::probability(event_handle->probability))
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
      *entity = RPG_Engine_Common_Tools::createEntity(event_handle->monster);
      entity->is_spawned = true;
      ACE_ASSERT(entity->character && entity->graphic);
      if (!entity->character || !entity->graphic)
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
      RPG_Dice_RollResult_t random_number;
      RPG_Dice::generateRandomNumbers(seed_points.size(),
                                      1,
                                      random_number);
      RPG_Map_PositionsConstIterator_t iterator = seed_points.begin();
      std::advance(iterator, random_number.front() - 1);
      entity->position = *iterator;

      RPG_Engine_EntityID_t id = myEngine->add(entity);

      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("spawned a %s [id: %u]...\n"),
                 entity->character->getName().c_str(),
                 id));

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

void
RPG_Engine_Event_Manager::handleEntities()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Event_Manager::handleEntities"));

  ACE_ASSERT(myEngine);

  RPG_Engine_Action next_action;
  next_action.command = RPG_ENGINE_COMMAND_INVALID;
  next_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                        std::numeric_limits<unsigned int>::max());
  next_action.path.clear();
  next_action.target = 0;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myEngine->myLock);

    for (RPG_Engine_EntitiesConstIterator_t iterator = myEngine->myEntities.begin();
         iterator != myEngine->myEntities.end();
         iterator++)
    {
      // do not control player characters...
      if ((*iterator).second->character->isPlayerCharacter())
        continue;

      // idle ? --> do something !
      if ((*iterator).second->actions.empty())
      {
        // find closest target (if any)
        // *TODO*: implement strategy here (strongest/weakest target, ...)
        unsigned int distance, closest_distance = std::numeric_limits<unsigned int>::max();
        RPG_Engine_EntitiesConstIterator_t closest_target = myEngine->myEntities.end();
        for (RPG_Engine_EntitiesConstIterator_t iterator2 = myEngine->myEntities.begin();
             iterator2 != myEngine->myEntities.end();
             iterator2++)
        {
          if (!(*iterator2).second->character->isPlayerCharacter())
            continue;

          distance = RPG_Map_Common_Tools::distance((*iterator).second->position,
                                                    (*iterator2).second->position);
          if (distance < closest_distance)
          {
            closest_distance = distance;
            closest_target = iterator2;
          } // end IF
        } // end FOR
        ACE_ASSERT(closest_target != myEngine->myEntities.end());
        if (closest_target == myEngine->myEntities.end())
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("no target, continuing\n")));

          break;
        } // end IF
        next_action.target = (*closest_target).first;
        // *TODO*: check reach instead
        if (RPG_Map_Common_Tools::isAdjacent((*iterator).second->position,
                                             (*closest_target).second->position))
        {
          // start attacking right away...
          next_action.command = COMMAND_ATTACK;
        } // end IF
        else
        {
          // start pursuing...
          next_action.command = COMMAND_TRAVEL;
        } // end ELSE

        (*iterator).second->actions.push_back(next_action);
      } // end IF
    } // end FOR
  } // end lock scope
}
