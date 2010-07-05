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

#include <ace/Log_Msg.h>

RPG_Map_Level::RPG_Map_Level(const RPG_Map_FloorPlan_t& floorPlan_in)
 : myFloorPlan(floorPlan_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

  // init doors
  initDoors();
}

RPG_Map_Level::~RPG_Map_Level()
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::~RPG_Map_Level"));

}

void
RPG_Map_Level::init(const RPG_Map_FloorPlan_t& floorPlan_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::init"));

  myFloorPlan = floorPlan_in;
}

const RPG_Map_Dimensions_t
RPG_Map_Level::getDimensions() const
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::getDimensions"));

  RPG_Map_Dimensions_t result = std::make_pair(myFloorPlan.size_x, myFloorPlan.size_y);

  return result;
}

const RPG_Map_Element
RPG_Map_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::getElement"));

  if (myFloorPlan.unmapped.find(position_in) != myFloorPlan.unmapped.end())
    return MAPELEMENT_UNMAPPED;

  if (myFloorPlan.walls.find(position_in) != myFloorPlan.walls.end())
    return MAPELEMENT_WALL;

  if (myFloorPlan.doors.find(position_in) != myFloorPlan.doors.end())
    return MAPELEMENT_DOOR;

  return MAPELEMENT_FLOOR;
}

const RPG_Map_Door_t
RPG_Map_Level::getDoor(const RPG_Map_Position_t& position_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::getDoor"));

  RPG_Map_Door_t dummy;
  dummy.position = std::make_pair(0, 0);
  dummy.is_open = false;
  dummy.is_locked = false;
  dummy.is_broken = false;

  // sanity check
  ACE_ASSERT(myFloorPlan.doors.find(position_in) != myFloorPlan.doors.end());
  if (myFloorPlan.doors.find(position_in) == myFloorPlan.doors.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid door position (was [%u,%u]), aborting\n"),
               position_in.first,
               position_in.second));

    // what else can we do ?
    ACE_ASSERT(false);

    return dummy;
  } // end IF

  for (RPG_Map_DoorsConstIterator_t iterator = myDoors.begin();
       iterator != myDoors.end();
       iterator++)
  {
    if ((*iterator).position == position_in)
      return *iterator;
  } // end FOR

  return dummy;
}

void
RPG_Map_Level::handleDoor(const RPG_Map_Position_t& position_in,
                          const bool& open_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::handleDoor"));

  for (RPG_Map_DoorsIterator_t iterator = myDoors.begin();
       iterator != myDoors.end();
       iterator++)
  {
    if ((*iterator).position == position_in)
    {
      if (open_in)
      {
        // sanity check
        if ((*iterator).is_open)
          return;

        // cannot simply open locked doors
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
          return;
      } // end ELSE

      (*iterator).is_open = open_in;

      // done
      return;
    } // end IF
  } // end FOR
}

void
RPG_Map_Level::initDoors()
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::initDoors"));

  RPG_Map_Door_t current;
  for (RPG_Map_PositionsConstIterator_t iterator = myFloorPlan.doors.begin();
       iterator != myFloorPlan.doors.end();
       iterator++)
  {
    current.position = *iterator;
    current.is_open = false;
    current.is_locked = false;
    current.is_broken = false;

    myDoors.push_back(current);
  } // end FOR
}
