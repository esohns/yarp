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

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Map_Level
{
 public:
  RPG_Map_Level();
  RPG_Map_Level(const RPG_Map_t&); // map
  virtual ~RPG_Map_Level();

  void init(const RPG_Map_t&); // map

  const RPG_Map_Position_t getStartPosition() const;
  const RPG_Map_Positions_t getSeedPoints() const;
  const RPG_Map_FloorPlan_t getFloorPlan() const;

  const RPG_Map_Dimensions_t getDimensions() const;

 protected:
  RPG_Map_t myMap;
};

#endif
