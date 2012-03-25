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

#include <rpg_common_macros.h>

RPG_Map_Level::RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

  myMap.plan.size_x = 0;
  myMap.plan.size_y = 0;
  myMap.plan.unmapped.clear();
  myMap.plan.walls.clear();
  myMap.plan.doors.clear();
  myMap.start = std::make_pair(0, 0);
  myMap.seeds.clear();
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
