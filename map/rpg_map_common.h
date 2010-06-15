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
#include <functional>

enum RPG_Map_Element
{
  // *** basic ***
  MAPELEMENT_FLOOR = 0,
  MAPELEMENT_WALL,
  MAPELEMENT_DOOR,
//   // *** composite ***
//   MAPELEMENT_VAULT,
//   MAPELEMENT_ROOM,
//   MAPELEMENT_SUBROOM,
//   MAPELEMENT_CORRIDOR,
  //
  MAPELEMENT_MAX,
  MAPELEMENT_INVALID
};

typedef std::pair<unsigned long, unsigned long> RPG_Map_Position_t;
// *NOTE*: std::less<_Key> uses default operator< for std::pair<>, which sorts
// the cells left-to-right, top-to-bottom (0,0 is top-left)
// --> (0,0), (0,1), ..., (0,dimy-1), (1,0), ..., (1,dimy-1), ..., (dimx-1,dimy-1)
// This is not what we want; ensure that coordinates will be sorted
// top-to-bottom, left-to-right.
// --> (0,0), (1,0), ..., (dimx-1, 0), (0,1), ..., (dimx-1,1), ..., (dimx-1,dimy-1)
struct position_compare
 : public std::binary_function<RPG_Map_Position_t,
                               RPG_Map_Position_t,
                               bool>
{
  inline bool
  operator()(const RPG_Map_Position_t& __x,
             const RPG_Map_Position_t& __y) const
  {
    return ((__x.second < __y.second) ||
            (!(__y.second < __x.second) && (__x.first < __y.first)));
  }
};
typedef std::set<RPG_Map_Position_t, position_compare> RPG_Map_Positions_t;
typedef RPG_Map_Positions_t::const_iterator RPG_Map_PositionsConstIterator_t;
typedef RPG_Map_Positions_t::iterator RPG_Map_PositionsIterator_t;
typedef std::list<RPG_Map_Position_t> RPG_Map_PositionList_t;
typedef RPG_Map_PositionList_t::const_iterator RPG_Map_PositionListConstIterator_t;
typedef RPG_Map_PositionList_t::iterator RPG_Map_PositionListIterator_t;

struct RPG_Map_FloorPlan_t
{
  unsigned long size_x;
  unsigned long size_y;
  RPG_Map_Positions_t doors;
  RPG_Map_Positions_t walls;
};

enum RPG_Map_Direction
{
  UP = 0,
  RIGHT,
  DOWN,
  LEFT,
  INVALID
};
typedef std::set<RPG_Map_Direction> RPG_Map_Directions_t;
typedef RPG_Map_Directions_t::const_iterator RPG_Map_DirectionsConstIterator_t;
typedef std::pair<RPG_Map_Position_t, RPG_Map_Direction> RPG_Map_PathStep_t;
typedef std::list<RPG_Map_PathStep_t> RPG_Map_Path_t;
typedef RPG_Map_Path_t::const_iterator RPG_Map_PathConstIterator_t;
typedef std::list<RPG_Map_Path_t> RPG_Map_PathList_t;
typedef RPG_Map_PathList_t::const_iterator RPG_Map_PathListConstIterator_t;

#endif
