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
#ifndef RPG_MAP_LEVEL_H
#define RPG_MAP_LEVEL_H

#include "rpg_map_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Map_Level
{
 public:
  RPG_Map_Level();
  virtual ~RPG_Map_Level();

  void init(const RPG_Map_FloorPlan_t&); // map

  const RPG_Map_Dimensions_t getDimensions() const;
  const RPG_Map_Element getElement(const RPG_Map_Position_t&) const;

  const RPG_Map_Door_t getDoor(const RPG_Map_Position_t&) const;
  void handleDoor(const RPG_Map_Position_t&,
                  const bool&);              // open ? : close

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Level(const RPG_Map_Level&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Level& operator=(const RPG_Map_Level&));

  // helper methods
  void initDoors();

  RPG_Map_FloorPlan_t myFloorPlan;
  RPG_Map_Doors_t     myDoors;
};

#endif
