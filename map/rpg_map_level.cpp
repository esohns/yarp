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

#include "rpg_map_level.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include <rpg_common_macros.h>

RPG_Map_Level::RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

  //myMap.name.clear();
  myMap.start = std::make_pair(0, 0);
  //myMap.seeds.clear();
  myMap.plan.size_x = 0;
  myMap.plan.size_y = 0;
  //myMap.plan.unmapped.clear();
  //myMap.plan.walls.clear();
  //myMap.plan.doors.clear();
  myMap.plan.rooms_are_square = false;
}

RPG_Map_Level::RPG_Map_Level(const RPG_Map_t& map_in)
 : myMap(map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

}

RPG_Map_Level::~RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::~RPG_Map_Level"));

}

void
RPG_Map_Level::init(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::init"));

  myMap = map_in;
}

void
RPG_Map_Level::save(const std::string& filename_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::save"));

  if (!RPG_Map_Common_Tools::save(filename_in,
                                  myMap))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), continuing\n"),
               filename_in.c_str()));
}

const std::string&
RPG_Map_Level::getName() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getName"));

  return myMap.name;
}

const RPG_Map_Position_t&
RPG_Map_Level::getStartPosition() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getStartPosition"));

  return myMap.start;
}

const RPG_Map_Positions_t&
RPG_Map_Level::getSeedPoints() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getSeedPoints"));

  return myMap.seeds;
}

const RPG_Map_FloorPlan_t&
RPG_Map_Level::getFloorPlan() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getFloorPlan"));

  return myMap.plan;
}

RPG_Map_Size_t
RPG_Map_Level::getSize() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getSize"));

  return std::make_pair(myMap.plan.size_x, myMap.plan.size_y);
}

bool
RPG_Map_Level::isValid(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::isValid"));

  RPG_Map_Element element = getElement(position_in);
  switch (element)
  {
    case MAPELEMENT_FLOOR:
      return true;
    case MAPELEMENT_DOOR:
    {
      RPG_Map_Door_t current_door = getDoor(position_in);
      if (current_door.is_open)
        return true;

      break;
    }
    default:
      break;
  } // end SWITCH

  return false;
}

bool
RPG_Map_Level::isCorner(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::isCorner"));

  RPG_Map_Position_t east, west, south, north;
  east = position_in;
  east.first++;
  west = position_in;
  west.first--;
  north = position_in;
  north.second--;
  south = position_in;
  south.second++;

  return ((((getElement(west) == MAPELEMENT_FLOOR) ||
            (getElement(west) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_FLOOR) ||
            (getElement(south) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_UNMAPPED) ||
            (getElement(north) == MAPELEMENT_WALL)) &&
           ((getElement(east) == MAPELEMENT_UNMAPPED) ||
            (getElement(east) == MAPELEMENT_WALL))) || // SW
          (((getElement(east) == MAPELEMENT_FLOOR) ||
            (getElement(east) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_FLOOR) ||
            (getElement(south) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_UNMAPPED) ||
            (getElement(north) == MAPELEMENT_WALL)) &&
           ((getElement(west) == MAPELEMENT_UNMAPPED) ||
            (getElement(west) == MAPELEMENT_WALL))) || // SE
          (((getElement(west) == MAPELEMENT_FLOOR) ||
            (getElement(west) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_FLOOR) ||
            (getElement(north) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_UNMAPPED) ||
            (getElement(south) == MAPELEMENT_WALL)) &&
           ((getElement(east) == MAPELEMENT_UNMAPPED) ||
            (getElement(east) == MAPELEMENT_WALL))) || // NW
          (((getElement(east) == MAPELEMENT_FLOOR) ||
            (getElement(east) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_FLOOR) ||
            (getElement(north) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_UNMAPPED) ||
            (getElement(south) == MAPELEMENT_WALL)) &&
           ((getElement(west) == MAPELEMENT_UNMAPPED) ||
            (getElement(west) == MAPELEMENT_WALL)))); // NE
}

RPG_Map_Element
RPG_Map_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getElement"));

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

const RPG_Map_Door_t&
RPG_Map_Level::getDoor(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Map_Level"));

  ACE_ASSERT(!myMap.plan.doors.empty());

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  position_door.outside = DIRECTION_INVALID;
  position_door.is_open = false;
  position_door.is_locked = false;
  position_door.is_broken = false;

  RPG_Map_DoorsConstIterator_t iterator = myMap.plan.doors.find(position_door);
  // sanity check
  ACE_ASSERT(iterator != myMap.plan.doors.end());

  return *iterator;
}

bool
RPG_Map_Level::findPath(const RPG_Map_Position_t& start_in,
                        const RPG_Map_Position_t& end_in,
                        RPG_Map_Path_t& path_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::findPath"));

  // init result
  path_out.clear();

  // sanity check
  if (start_in == end_in)
    return true;

  // add obstacles:
  // - walls
  // - (closed) doors
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

  return ((path_out.size() >= 2) &&
          (path_out.front().first == start_in) &&
          (path_out.back().first == end_in));
}

void
RPG_Map_Level::findValid(const RPG_Map_Position_t& center_in,
                         const unsigned int& radius_in,
                         RPG_Map_Positions_t& area_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::findValid"));

  // init return value(s)
  area_out.clear();

  // step1: compute square perimeter
  //RPG_Map_Positions_t square;
  RPG_Map_Common_Tools::buildSquare(center_in,
                                    getSize(),
                                    radius_in,
                                    true,
                                    area_out);

  // step2: remove invalid (== blocked) positions
  //RPG_Map_Positions_t invalid;
  //for (RPG_Map_PositionsConstIterator_t iterator = square.begin();
  //     iterator != square.end();
  //     iterator++)
  //  if (!isValid(*iterator))
  //    invalid.insert(*iterator);
  //std::vector<RPG_Map_Position_t> difference(area_out.size());
  //std::vector<RPG_Map_Position_t>::iterator difference_end;
  //difference_end = std::set_difference(area_out.begin(), area_out.end(),
  //                                     invalid.begin(), invalid.end(),
  //                                     difference.begin());
  //area_out.insert(difference.begin(), difference_end);
  // *WARNING*: this works for associative containers ONLY
  for (RPG_Map_PositionsIterator_t iterator = area_out.begin();
       iterator != area_out.end();
       iterator++)
    if (!isValid(*iterator))
      area_out.erase(iterator++);
}
