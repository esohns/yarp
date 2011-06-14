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

RPG_Map_Level::RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

  myStartPosition = std::make_pair(0, 0);
  mySeedPoints.clear();
  myFloorPlan.size_x = 0;
  myFloorPlan.size_y = 0;
  myFloorPlan.unmapped.clear();
  myFloorPlan.walls.clear();
  myFloorPlan.doors.clear();
}

RPG_Map_Level::RPG_Map_Level(const RPG_Map_Position_t& startingPosition_in,
                             const RPG_Map_Positions_t& seedPoints_in,
                             const RPG_Map_FloorPlan_t& floorPlan_in)
 : myStartPosition(startingPosition_in),
   mySeedPoints(seedPoints_in),
   myFloorPlan(floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

}

RPG_Map_Level::~RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::~RPG_Map_Level"));

}

void
RPG_Map_Level::init(const RPG_Map_Position_t& startingPosition_in,
                    const RPG_Map_Positions_t& seedPoints_in,
                    const RPG_Map_FloorPlan_t& floorPlan_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::init"));

  myStartPosition = startingPosition_in;
  mySeedPoints = seedPoints_in;
  myFloorPlan = floorPlan_in;
}

const RPG_Map_Position_t
RPG_Map_Level::getStartPosition() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getStartPosition"));

  return myStartPosition;
}

const RPG_Map_Positions_t
RPG_Map_Level::getSeedPoints() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getSeedPoints"));

  return mySeedPoints;
}

const RPG_Map_FloorPlan_t
RPG_Map_Level::getFloorPlan() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getFloorPlan"));

  return myFloorPlan;
}
