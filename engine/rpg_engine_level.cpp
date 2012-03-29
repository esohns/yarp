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

#include "rpg_engine_level.h"

#include "rpg_engine_defines.h"
#include "rpg_engine_common_tools.h"

#include <rpg_map_common_tools.h>
#include <rpg_map_pathfinding_tools.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

// init statics
ACE_Atomic_Op<ACE_Thread_Mutex, RPG_Engine_EntityID_t> RPG_Engine_Level::myCurrentID = 1;

RPG_Engine_Level::RPG_Engine_Level()
 : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
   myCondition(myLock),
//    myEntities(),
   myActivePlayer(0),
   myClient(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

  // use member message queue...
  inherited2::msg_queue(&myQueue);

  // set group ID for worker thread(s)
  inherited2::grp_id(RPG_ENGINE_DEF_TASK_GROUP_ID);

  inherited::myMap.plan.size_x = 0;
  inherited::myMap.plan.size_y = 0;
}

RPG_Engine_Level::RPG_Engine_Level(RPG_Engine_IWindow* client_in,
                                   const RPG_Map_t& map_in)
 : inherited(map_in),
   myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
   myCondition(myLock),
   myClient(client_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

  // sanity check
  ACE_ASSERT(client_in);

  // use member message queue...
  inherited2::msg_queue(&myQueue);

  // set group ID for worker thread(s)
  inherited2::grp_id(RPG_ENGINE_DEF_TASK_GROUP_ID);
}

RPG_Engine_Level::~RPG_Engine_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::~RPG_Engine_Level"));

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
RPG_Engine_Level::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::open"));

  ACE_UNUSED_ARG(args_in);

  // *IMPORTANT NOTE*: the first time around, the queue will have been open()ed
  // from within the default ctor; this sets it into an ACTIVATED state, which
  // is expected.
  // Subsequently (i.e. after stop()ping/start()ing, the queue
  // will have been deactivate()d in the process, and getq() (see svc()) will fail
  // (ESHUTDOWN) --> (re-)activate() the queue !
  // step1: (re-)activate() the queue
  if (inherited2::msg_queue()->activate() == -1)
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
RPG_Engine_Level::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::close"));

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
RPG_Engine_Level::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::svc"));

  ACE_Message_Block* ace_mb          = NULL;
  ACE_Time_Value     peek_delay(0, (RPG_ENGINE_EVENT_PEEK_INTERVAL * 1000));
  ACE_Time_Value     delay;
  bool               stop_processing = false;

  while (!stop_processing)
  {
    delay = (ACE_OS::gettimeofday() + peek_delay);
    if (inherited2::getq(ace_mb, &delay) != -1)
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
RPG_Engine_Level::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::start"));

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
  ret = inherited2::activate((THR_NEW_LWP |
                              THR_JOINABLE |
                              THR_INHERIT_SCHED),         // flags
                             1,                           // number of threads
                             0,                           // force spawning
                             ACE_DEFAULT_THREAD_PRIORITY, // priority
                             inherited2::grp_id(),        // group id --> has been set (see above)
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
               inherited2::grp_id(),
               thread_ids[0]));

  // start AI
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::instance()->start();
}

void
RPG_Engine_Level::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::stop"));

  // sanity check
  if (!isRunning())
    return;

  // stop AI
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
  if (inherited2::putq(stop_mb, NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task::putq(): \"%m\", returning\n")));

    // clean up, what else can we do ?
    stop_mb->release();

    return;
  } // end IF

  // ... and wait for the worker thread to join
  if (inherited2::wait() == -1)
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
RPG_Engine_Level::isRunning()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::isRunning"));

  return (inherited2::thr_count() > 0);
}

void
RPG_Engine_Level::wait_all()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::wait_all"));

  // ... wait for ALL worker(s) to join
  if (ACE_Thread_Manager::instance()->wait_grp(RPG_ENGINE_DEF_TASK_GROUP_ID) == -1)
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to ACE_Thread_Manager::wait_grp(): \"%m\", returning\n")));
}

void
RPG_Engine_Level::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::dump_state"));

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
RPG_Engine_Level::init(RPG_Engine_IWindow* client_in,
                       const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::init"));

  myClient = client_in;
  inherited::init(map_in);
}

void
RPG_Engine_Level::save(const std::string& filename_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::save"));

  if (!RPG_Map_Common_Tools::save(filename_in,
                                  inherited::myMap))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), continuing\n"),
               filename_in.c_str()));
}

RPG_Engine_EntityID_t
RPG_Engine_Level::add(RPG_Engine_Entity* entity_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::add"));

  // sanity check
  ACE_ASSERT(entity_in);
  ACE_ASSERT(myClient);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntityID_t id = myCurrentID++;
  myEntities[id] = entity_in;

  // notify client / window
  try
  {
    myClient->addEntity(id, entity_in->graphic);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::addEntity(%u), continuing\n"),
               id));
  }

  return id;
}

void
RPG_Engine_Level::remove(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::remove"));

  // sanity check
  ACE_ASSERT(id_in);

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

  // notify client / window
  try
  {
    myClient->removeEntity(id_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Engine_IWindow::removeEntity(%u), continuing\n"),
               id_in));
  }
}

void
RPG_Engine_Level::action(const RPG_Engine_EntityID_t& id_in,
                         const RPG_Engine_Action& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::action"));

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
    case COMMAND_STOP:
    {
      (*iterator).second->actions.clear();

      return;
    }
    default:
    {
      (*iterator).second->actions.push_back(action_in);

      break;
    }
  } // end SWITCH
}

void
RPG_Engine_Level::setActive(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::setActive"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myActivePlayer = id_in;
}

RPG_Engine_EntityID_t
RPG_Engine_Level::getActive() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getActive"));

  RPG_Engine_EntityID_t result = 0;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    result = myActivePlayer;
  } // end lock scope

  return result;
}

void
RPG_Engine_Level::mode(const RPG_Engine_EntityMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::mode"));

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
RPG_Engine_Level::clear(const RPG_Engine_EntityMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::clear"));

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
RPG_Engine_Level::hasMode(const RPG_Engine_EntityMode& mode_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::hasMode"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // sanity check
  if (myActivePlayer == 0)
    return false;

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(myActivePlayer);
  ACE_ASSERT(iterator != myEntities.end());

  return ((*iterator).second->modes.find(mode_in) != (*iterator).second->modes.end());
}

RPG_Map_Element
RPG_Engine_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getElement"));

  // sanity check
  if ((position_in.first  == std::numeric_limits<unsigned int>::max()) ||
      (position_in.second == std::numeric_limits<unsigned int>::max()))
    return MAPELEMENT_INVALID;

  // unmapped/off-map ?
  if ((position_in.first  >= myMap.plan.size_x) ||
      (position_in.second >= myMap.plan.size_y) ||
      (myMap.plan.unmapped.find(position_in) != myMap.plan.unmapped.end()))
    return MAPELEMENT_UNMAPPED;

  if (myMap.plan.walls.find(position_in) != myMap.plan.walls.end())
    return MAPELEMENT_WALL;

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  if (myMap.plan.doors.find(position_door) != myMap.plan.doors.end())
    return MAPELEMENT_DOOR;

  return MAPELEMENT_FLOOR;
}

const SDL_Surface*
RPG_Engine_Level::getGraphics(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getGraphics"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), aborting\n"),
               id_in));

    return NULL;
  } // end IF

  return (*iterator).second->graphic;
}

RPG_Engine_EntityGraphics_t
RPG_Engine_Level::getGraphics() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getGraphics"));

  RPG_Engine_EntityGraphics_t graphics;

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    graphics.insert(std::make_pair((*iterator).second->position, (*iterator).second->graphic));

  return graphics;
}

RPG_Map_Position_t
RPG_Engine_Level::getPosition(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getPosition"));

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

RPG_Map_Door_t
RPG_Engine_Level::getDoor(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getDoor"));

  RPG_Map_Door_t dummy;
  dummy.position = position_in;
  dummy.outside = DIRECTION_INVALID;
  dummy.is_open = false;
  dummy.is_locked = false;
  dummy.is_broken = false;

  // sanity check
  dummy.position = position_in;
  RPG_Map_DoorsConstIterator_t iterator = myMap.plan.doors.find(dummy);
  if (iterator == myMap.plan.doors.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid door position (was: [%u,%u]), aborting\n"),
               position_in.first,
               position_in.second));

    // what else can we do ?
    dummy.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                    std::numeric_limits<unsigned int>::max());

    return dummy;
  } // end IF

  return *iterator;
}

bool
RPG_Engine_Level::findPath(const RPG_Map_Position_t& start_in,
                           const RPG_Map_Position_t& end_in,
                           RPG_Map_Path_t& path_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::findPath"));

  // init result
  path_out.clear();

  // sanity check
  if (start_in == end_in)
    return true;

  RPG_Map_Positions_t obstacles = myMap.plan.walls;
  for (RPG_Map_DoorsConstIterator_t door_iterator = myMap.plan.doors.begin();
       door_iterator != myMap.plan.doors.end();
       door_iterator++)
    if (!(*door_iterator).is_open)
      obstacles.insert((*door_iterator).position);
  RPG_Map_Pathfinding_Tools::findPath(getSize(),
                                      obstacles, // walls & closed doors
                                      start_in,
                                      DIRECTION_INVALID,
                                      end_in,
                                      path_out);

  return (!path_out.empty() &&
          (path_out.front().first == start_in) &&
          (path_out.back().first == end_in));
}

RPG_Engine_EntityID_t
RPG_Engine_Level::hasEntity(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::hasEntity"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    if ((*iterator).second->position == position_in)
      return (*iterator).first;

  return 0;
}

bool
RPG_Engine_Level::isMonster(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::isMonster"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  ACE_ASSERT(iterator != myEntities.end());

  return !(*iterator).second->character->isPlayerCharacter();
}

void
RPG_Engine_Level::handleDoor(const RPG_Map_Position_t& position_in,
                             const bool& open_in,
                             bool& toggled_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleDoor"));

  // init return value(s)
  toggled_out = false;

  RPG_Map_Door_t position_door;
  position_door.position = position_in;

  RPG_Map_DoorsIterator_t iterator = myMap.plan.doors.find(position_door);
  if (iterator == myMap.plan.doors.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid door position (was: [%u,%u]), aborting\n"),
               position_in.first,
               position_in.second));

    return;
  } // end IF

  if (open_in)
  {
    // sanity check
    if ((*iterator).is_open)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already open...\n"),
//                  position_in.first,
//                  position_in.second));

      return;
    } // end IF

    // cannot simply open locked doors...
    if ((*iterator).is_locked)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("door [%u,%u] is locked...\n"),
                 position_in.first,
                 position_in.second));

      return;
    } // end IF
  } // end IF
  else
  {
    // sanity check
    if (!(*iterator).is_open)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already closed...\n"),
//                  position_in.first,
//                  position_in.second));

      return;
    } // end IF
  } // end ELSE

  // *WARNING*: set iterators are CONST for a good reason !
  // --> (but we know what we're doing)...
  (const_cast<RPG_Map_Door_t&>(*iterator)).is_open = open_in;

  toggled_out = true;
}

void
RPG_Engine_Level::clearEntityActions(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::clearEntityActions"));

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
RPG_Engine_Level::handleEntities()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleEntities"));

  bool action_complete = true;

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
        ACE_ASSERT(current_action.target);
        RPG_Engine_EntitiesConstIterator_t target = myEntities.find(current_action.target);
        if ((target == myEntities.end()) ||
            !RPG_Map_Common_Tools::isAdjacent((*iterator).second->position,
                                              (*target).second->position))
          break; // nothing to do...

        action_complete = false;
        (*iterator).second->modes.insert(ENTITYMODE_FIGHTING);

        // debug info
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\" attacks \"%s\"...\n"),
                   (*iterator).second->character->getName().c_str(),
                   (*target).second->character->getName().c_str()));

        break;
      }
      case COMMAND_DOOR_CLOSE:
      case COMMAND_DOOR_OPEN:
      {
        bool toggled = false;
        handleDoor(current_action.position,
                   (current_action.command == COMMAND_DOOR_OPEN),
                   toggled);

        // notify client window
        if (toggled)
        {
          try
          {
            myClient->toggleDoor(current_action.position);
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Engine_IWindow::toggleDoor([%u,%u]), continuing\n"),
                       current_action.position.first,
                       current_action.position.second));
          }
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
        // sanity check
        if ((current_action.target == 0) && // no target...
            (current_action.position == (*iterator).second->position))
        {
          (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);

          break; // nothing to do...
        } // end IF

        // compute path first ?
        if (current_action.target || // pursuit mode...
            current_action.path.empty())
        {
          RPG_Map_Position_t target_position = current_action.position;
          if (current_action.target)
          {
            // retrieve current target position
            RPG_Engine_EntitiesConstIterator_t target = myEntities.find(current_action.target);
            if ((target == myEntities.end()) ||
                RPG_Map_Common_Tools::isAdjacent((*iterator).second->position,
                                                 (*target).second->position))
            {
              (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);

              break; // nothing (more) to do...
            } // end IF

            target_position = (*target).second->position;
          } // end IF
          if (current_action.path.empty() ||
              (current_action.path.back().first != target_position)) // target has moved...
            findPath((*iterator).second->position,
                     target_position,
                     current_action.path);
          if (current_action.path.size() < 2)
          {
            // *NOTE*: --> no/invalid path, cannot proceed...

            (*iterator).second->modes.erase(ENTITYMODE_TRAVELLING);

            break; // stop & cancel path...
          } // end IF
          ACE_ASSERT(current_action.path.front().first == (*iterator).second->position);
          //ACE_ASSERT(current_action.path.back().first == current_action.position);

          current_action.path.pop_front();
        } // end IF
        ACE_ASSERT(!current_action.path.empty());
        
        (*iterator).second->modes.insert(ENTITYMODE_TRAVELLING);

        // move to next adjacent position (if still (!) possible)
        RPG_Map_Element element = getElement(current_action.path.front().first);
        ACE_ASSERT((element == MAPELEMENT_FLOOR) || (element == MAPELEMENT_DOOR));
        if (element == MAPELEMENT_DOOR)
        {
          RPG_Map_Door_t door = getDoor(current_action.path.front().first);
          if (!door.is_open)
            break; // stop & cancel path...
        } // end IF

        (*iterator).second->position = current_action.path.front().first;
        current_action.path.pop_front();
        action_complete = false;

        // notify client window
        try
        {
          myClient->updateEntity((*iterator).first);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in RPG_Engine_IWindow::updateEntity(%u), continuing\n"),
                     (*iterator).first));
        }

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
}
