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
#include "rpg_engine_level.h"

#include <rpg_map_common_tools.h>
#include <rpg_map_pathfinding_tools.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

// init statics
ACE_Atomic_Op<ACE_Thread_Mutex, RPG_Engine_EntityID_t> RPG_Engine_Level::myCurrentID = 1;

RPG_Engine_Level::RPG_Engine_Level()
 : myCondition(myLock)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

  myFloorPlan.size_x = 0;
  myFloorPlan.size_y = 0;
}

RPG_Engine_Level::RPG_Engine_Level(const RPG_Map_FloorPlan_t& floorPlan_in)
 : myCondition(myLock),
   myFloorPlan(floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

}

RPG_Engine_Level::~RPG_Engine_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::~RPG_Engine_Level"));

}

void
RPG_Engine_Level::init(const RPG_Map_FloorPlan_t& floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::init"));

  myFloorPlan = floorPlan_in;
}

const RPG_Engine_EntityID_t
RPG_Engine_Level::add(RPG_Engine_Entity& entity_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::add"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntityID_t id = myCurrentID++;

  myEntities.insert(std::make_pair(id, entity_in));

  return id;
}

void
RPG_Engine_Level::remove(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::remove"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity (ID was %u), aborting\n"),
               id_in));

    return;
  } // end IF

  myEntities.erase(iterator);
}

void
RPG_Engine_Level::action(const RPG_Engine_EntityID_t& id_in,
                         const RPG_Engine_Action& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::action"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity (ID was %u), aborting\n"),
               id_in));

    return;
  } // end IF

  switch (action_in.command)
  {
    case COMMAND_STEP:
    {
      // compute target position
      RPG_Map_Position_t targetPosition = (*iterator).second.position;
      switch (action_in.direction)
      {
        case DIRECTION_UP:
          targetPosition.second--; break;
        case DIRECTION_DOWN:
          targetPosition.second++; break;
        case DIRECTION_LEFT:
          targetPosition.first--; break;
        case DIRECTION_RIGHT:
          targetPosition.first++; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid direction (was: \"%s\"), aborting\n"),
                     RPG_Map_Common_Tools::direction2String(action_in.direction).c_str()));

          break;
        }
      } // end SWITCH

      // position valid ?
      RPG_Map_Element element = getElement(targetPosition);
      if ((element == MAPELEMENT_FLOOR) ||
          (element == MAPELEMENT_DOOR))
      {
        if (element == MAPELEMENT_DOOR)
        {
          RPG_Map_Door_t door = getDoor(targetPosition);
          if (!door.is_open)
            return;
        } // end IF

        (*iterator).second.position = targetPosition;
      } // end IF

      break;
    }
    case COMMAND_STOP:
    {
      (*iterator).second.actions.clear();

      return;
    }
    default:
    {
      (*iterator).second.actions.push_back(action_in);

      break;
    }
  } // end SWITCH
}

const RPG_Map_Dimensions_t
RPG_Engine_Level::getDimensions() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getDimensions"));

  RPG_Map_Dimensions_t result = std::make_pair(myFloorPlan.size_x, myFloorPlan.size_y);

  return result;
}

const RPG_Map_Element
RPG_Engine_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getElement"));

  // sanity check
  if ((position_in.first > (myFloorPlan.size_x - 1)) ||
      (position_in.second > (myFloorPlan.size_y - 1)))
    return MAPELEMENT_INVALID;

  if (myFloorPlan.unmapped.find(position_in) != myFloorPlan.unmapped.end())
    return MAPELEMENT_UNMAPPED;

  if (myFloorPlan.walls.find(position_in) != myFloorPlan.walls.end())
    return MAPELEMENT_WALL;

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  if (myFloorPlan.doors.find(position_door) != myFloorPlan.doors.end())
    return MAPELEMENT_DOOR;

  return MAPELEMENT_FLOOR;
}

const RPG_Engine_EntityGraphics_t
RPG_Engine_Level::getGraphics() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getGraphics"));

  RPG_Engine_EntityGraphics_t graphics;

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EntitiesConstIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
    graphics.insert(std::make_pair((*iterator).second.position, (*iterator).second.graphic));

  return graphics;
}

const RPG_Map_Position_t
RPG_Engine_Level::getPosition(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getPosition"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  RPG_Engine_EntitiesConstIterator_t iterator = myEntities.find(id_in);
  if (iterator == myEntities.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity (ID was %u), aborting\n"),
               id_in));

    return std::make_pair(0, 0);
  } // end IF

  return (*iterator).second.position;
}

const RPG_Map_Door_t
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
  RPG_Map_DoorsConstIterator_t iterator = myFloorPlan.doors.find(dummy);
  if (iterator == myFloorPlan.doors.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid door position (was [%u,%u]), aborting\n"),
               position_in.first,
               position_in.second));

    // what else can we do ?
    dummy.position = std::make_pair(0, 0);

    return dummy;
  } // end IF

  return *iterator;
}

void
RPG_Engine_Level::handleDoor(const RPG_Map_Position_t& position_in,
                             const bool& open_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleDoor"));

  RPG_Map_Door_t position_door;
  position_door.position = position_in;

  RPG_Map_DoorsIterator_t iterator = myFloorPlan.doors.find(position_door);
  if (iterator == myFloorPlan.doors.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid door position (was [%u,%u]), aborting\n"),
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
}

const RPG_Map_Positions_t
RPG_Engine_Level::getWalls() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getWalls"));

  return myFloorPlan.walls;
}

void
RPG_Engine_Level::handleEntities()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleEntities"));

  bool action_complete;

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  for (RPG_Engine_EntitiesIterator_t iterator = myEntities.begin();
       iterator != myEntities.end();
       iterator++)
  {
    if ((*iterator).second.actions.empty())
      continue;

    action_complete = true;
    switch ((*iterator).second.actions.front().command)
    {
      case COMMAND_ATTACK:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("[%u] attacking...\n"),
                   (*iterator).first));

        action_complete = false;

        break;
      } // end CASE
      case COMMAND_TRAVEL:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("[%u] travelling...\n"),
                   (*iterator).first));

        // compute path
        RPG_Map_Path_t path;
        if (!RPG_Map_Pathfinding_Tools::findPath(myFloorPlan.size_x,
                                                 myFloorPlan.size_y,
                                                 myFloorPlan.walls,
                                                 (*iterator).second.position,
                                                 RPG_Map_Pathfinding_Tools::getDirection((*iterator).second.position,
                                                                                         (*iterator).second.actions.front().position),
                                                 (*iterator).second.actions.front().position,
                                                 path))
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("could not find a path [%u,%u] --> [%u,%u], continuing\n"),
                     (*iterator).second.position.first,
                     (*iterator).second.position.second,
                     (*iterator).second.actions.front().position.first,
                     (*iterator).second.actions.front().position.second));
        } // end IF

        if (path.empty())
          break;

        // sanity check
        ACE_ASSERT(path.front().first == (*iterator).second.position);
        path.pop_front();

        if (!path.empty())
        {
          (*iterator).second.position = path.front().first;

          // reached destination ?
          if ((*iterator).second.position != (*iterator).second.actions.front().position)
            action_complete = false;
        } // end IF

        break;
      } // end CASE
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid action (ID: %u, was: \"%s\"), continuing\n"),
                   (*iterator).first,
                   RPG_Engine_CommandHelper::RPG_Engine_CommandToString((*iterator).second.actions.front().command).c_str()));

        break;
      } // end CASE
    } // end SWITCH

    if (action_complete)
      (*iterator).second.actions.pop_front();
  } // end FOR
}
