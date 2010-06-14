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

}

RPG_Map_Level::~RPG_Map_Level()
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::~RPG_Map_Level"));

}

const RPG_Map_Element
RPG_Map_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Map_Level::getElement"));

  if (myFloorPlan.walls.find(position_in) != myFloorPlan.walls.end())
    return MAPELEMENT_WALL;
  else if (myFloorPlan.doors.find(position_in) != myFloorPlan.doors.end())
    return MAPELEMENT_DOOR;

  return MAPELEMENT_FLOOR;
}
