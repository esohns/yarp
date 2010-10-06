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
#include "rpg_map_level.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Map_Level::RPG_Map_Level()
//  : myFloorPlan(),
//    myDoors()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

}

RPG_Map_Level::~RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::~RPG_Map_Level"));

}

void
RPG_Map_Level::init(const RPG_Map_FloorPlan_t& floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::init"));

  myFloorPlan = floorPlan_in;
}

const RPG_Map_Dimensions_t
RPG_Map_Level::getDimensions() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getDimensions"));

  RPG_Map_Dimensions_t result = std::make_pair(myFloorPlan.size_x, myFloorPlan.size_y);

  return result;
}

const RPG_Map_Element
RPG_Map_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getElement"));

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

const RPG_Map_Door_t
RPG_Map_Level::getDoor(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getDoor"));

  RPG_Map_Door_t dummy;
  dummy.position = position_in;
  dummy.outside = INVALID;
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
RPG_Map_Level::handleDoor(const RPG_Map_Position_t& position_in,
                          const bool& open_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::handleDoor"));

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
  ACE_const_cast(RPG_Map_Door_t&, *iterator).is_open = open_in;
}
