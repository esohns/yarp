/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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
#ifndef RPG_MAP_COMMON_H
#define RPG_MAP_COMMON_H

#include <map>
#include <set>
#include <list>

enum RPG_Map_Element
{
  // *** basic ***
  MAPITEM_WALL = 0,
  MAPITEM_DOOR,
  // *** composite ***
  MAPITEM_ROOM,
  MAPITEM_SUBROOM,
  MAPITEM_CORRIDOR
};

typedef std::pair<unsigned long, unsigned long> RPG_Map_Position_t;
typedef std::set<RPG_Map_Position_t> RPG_Map_Positions_t;
typedef std::list<RPG_Map_Position_t> RPG_Map_PositionList_t;
typedef RPG_Map_PositionList_t::const_iterator RPG_Map_PositionListIterator_t;

struct RPG_Map_DungeonLevel
{
  RPG_Map_Positions_t doors;
  RPG_Map_Positions_t walls;
};

#endif
