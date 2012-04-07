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

#ifndef RPG_MAP_COMMON_TOOLS_H
#define RPG_MAP_COMMON_TOOLS_H

#include "rpg_map_exports.h"
#include "rpg_map_defines.h"
#include "rpg_map_common.h"

#include <ace/Global_Macros.h>

#include <list>
#include <set>
#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Map_Export RPG_Map_Common_Tools
{
 public:
  static void create(const std::string&,                // name
                     const RPG_Map_FloorPlan_Config_t&, // floor plan config
                     RPG_Map_t&);                       // return value: map
  static bool load(const std::string&, // FQ filename
                   RPG_Map_t&,         // return value: map
                   const bool& = RPG_MAP_DEF_TRACE_SCANNING, // trace scanning ?
                   const bool& = RPG_MAP_DEF_TRACE_PARSING); // trace parsing ?
  static bool save(const std::string&, // FQ filename
                   const RPG_Map_t&);  // map
  static void print(const RPG_Map_t&); // map
  static std::string info(const RPG_Map_t&); // map

  static unsigned int distance(const RPG_Map_Position_t&,  // position 1
                               const RPG_Map_Position_t&); // position 2
  static bool isAdjacent(const RPG_Map_Position_t&,  // position 1
                         const RPG_Map_Position_t&); // position 2
  static std::string direction2String(const RPG_Map_Direction&);
  static std::string orientation2String(const RPG_Map_Orientation&);

  // *TODO*: relies on the size of the room in relation to its' surrounding space
  // --> results may be wrong for small enclosures
  static RPG_Map_Direction door2exitDirection(const RPG_Map_Position_t&,   // door
                                              const RPG_Map_FloorPlan_t&);
  static bool isFloor(const RPG_Map_Position_t&,
                      const RPG_Map_FloorPlan_t&);
  // *NOTE*: relies on a consistent notion of "outside" set for all doors !
  static bool isInsideRoom(const RPG_Map_Position_t&,
                           const RPG_Map_FloorPlan_t&);

  static bool hasLineOfSight(const RPG_Map_Position_t&,   // start position
                             const RPG_Map_Position_t&,   // end position
                             const RPG_Map_Positions_t&); // obstacles

  static void buildCorridor(const RPG_Map_Path_t&, // path
                            RPG_Map_Positions_t&); // return value: corridor

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Map_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Common_Tools(const RPG_Map_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Common_Tools& operator=(const RPG_Map_Common_Tools&));

  // helper types/methods
  // *WARNING*: make sure the set of positions forms one (connected) area !
  // (cannot enforce this without a (more complicated) algebraic definition...)

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
  typedef std::set<RPG_Map_Position_t, position_compare> RPG_Map_Zone_t;
  typedef RPG_Map_Zone_t::const_iterator RPG_Map_ZoneConstIterator_t;
  typedef RPG_Map_Zone_t::iterator RPG_Map_ZoneIterator_t;
  static void dump(const RPG_Map_Zone_t&); // zone
  typedef std::list<RPG_Map_Zone_t> RPG_Map_Partition_t;
  typedef RPG_Map_Partition_t::const_iterator RPG_Map_PartitionConstIterator_t;
  typedef RPG_Map_Partition_t::iterator RPG_Map_PartitionIterator_t;
  static void makePartition(const unsigned int&,   // dimension x
                            const unsigned int&,   // dimension y
                            const unsigned int&,   // #partitions
                            const bool&,           // resolve conflicts ?
                            RPG_Map_Positions_t&,  // return value: conflicts
                            RPG_Map_Positions_t&,  // return value: seed points
                            RPG_Map_Partition_t&); // return value: partition
  static void displayPartition(const unsigned int&,         // dimension x
                               const unsigned int&,         // dimension y
                               const RPG_Map_Positions_t&,  // conflicts
                               const RPG_Map_Positions_t&,  // seed points
                               const RPG_Map_Partition_t&); // partition

  typedef std::list<RPG_Map_Zone_t> RPG_Map_ZoneList_t;
  typedef RPG_Map_ZoneList_t::const_iterator RPG_Map_ZoneListConstIterator_t;
  typedef RPG_Map_ZoneList_t::iterator RPG_Map_ZoneListIterator_t;
  typedef std::list<RPG_Map_Positions_t> RPG_Map_PositionsList_t;
  typedef RPG_Map_PositionsList_t::iterator RPG_Map_PositionsListIterator_t;
  struct RPG_Map_Square_t
  {
    RPG_Map_Position_t ul;
    RPG_Map_Position_t lr;
  };
  typedef std::list<RPG_Map_Square_t> RPG_Map_SquareList_t;
  typedef RPG_Map_SquareList_t::const_iterator RPG_Map_SquareListConstIterator_t;
  static unsigned int area2Positions(const RPG_Map_Position_t&,  // position 1
                                     const RPG_Map_Position_t&); // position 2
  static bool positionInSquare(const RPG_Map_Position_t&, // position
                               const RPG_Map_Square_t&);  // square
  static void findMaxSquare(const RPG_Map_Zone_t&, // room
                            RPG_Map_Square_t&);    // return value: max square
  static void makeRooms(const unsigned int&,        // dimension x
                        const unsigned int&,        // dimension y
                        const RPG_Map_Partition_t&, // partition
                        const bool&,                // rooms to be square ?
                        const bool&,                // want room separation ?
                        const bool&,                // crop areas ?
                        const bool&,                // maximize rooms ?
                        const unsigned int&,        // min. room area (0: don't care)
                        RPG_Map_ZoneList_t&,        // return value: room(s)
                        RPG_Map_ZoneList_t&);       // return value: boundary(s)
  static void displayRooms(const unsigned int&,        // dimension x
                           const unsigned int&,        // dimension y
                           const RPG_Map_ZoneList_t&); // room(s)
  static void displayRoom(const unsigned int&,    // dimension x
                          const unsigned int&,    // dimension y
                          const RPG_Map_Zone_t&); // room

  typedef RPG_Map_Direction ORIGIN;
  static bool turn(const RPG_Map_Zone_t&,     // map
                   const RPG_Map_Position_t&, // position
                   const ORIGIN&,             // origin
                   const bool&,               // turn clockwise ?
                   bool&,                     // return value: is corner ? (else intersection)
                   RPG_Map_Direction&);       // return value: next direction
  static void findDoorPositions(const RPG_Map_Zone_t&,    // room (shell !)
                                const bool&,              // doors to fill positions ?
                                RPG_Map_PositionList_t&); // suitable position(s)
  static void makeDoors(const unsigned int&,       // dimension x
                        const unsigned int&,       // dimension y
                        const RPG_Map_ZoneList_t&, // room boundary(s)
                        const bool&,               // doors to fill positions ?
                        const unsigned int&,       // max. #doors/room
                        RPG_Map_ZoneList_t&);      // return value: doors

  static bool intersect(const RPG_Map_Zone_t&,     // map
                        const RPG_Map_Position_t&, // position
                        const ORIGIN&,             // origin
                        RPG_Map_Directions_t&,     // possible direction(s)
                        RPG_Map_Direction&);       // next direction, if NOT an intersection
  // *NOTE*: proceeds in clockwise direction
  static void crawlToPosition(const RPG_Map_Zone_t&,     // map
                              const RPG_Map_Position_t&, // origin
                              const RPG_Map_Position_t&, // target
                              const ORIGIN&,             // starting origin
                              RPG_Map_PositionList_t&);  // return value: trail
  // *NOTE*: this uses alternate sorting left-to-right, top-to-bottom (0,0 is top-left)
  // --> (0,0), (0,1), ..., (0,dimy-1), (1,0), ..., (1,dimy-1), ..., (dimx-1,dimy-1)
  typedef std::set<RPG_Map_Position_t> RPG_Map_AltPositions_t;
  typedef RPG_Map_AltPositions_t::const_iterator RPG_Map_AltPositionsConstIterator_t;
  static void crop(RPG_Map_Zone_t&); // room
//   static void cropSquareBoundary(RPG_Map_Zone_t&); // room
  static RPG_Map_Direction door2exitDirection(const RPG_Map_Zone_t&,      // room
                                              const RPG_Map_Position_t&); // door
  static void connectRooms(const unsigned int&,       // dimension x
                           const unsigned int&,       // dimension y
                           const RPG_Map_ZoneList_t&, // boundary(s)
                           const RPG_Map_ZoneList_t&, // doors
                           const RPG_Map_ZoneList_t&, // room(s) // *TODO*: faster in/out tests ?
                           RPG_Map_FloorPlan_t&);     // return value: doors & walls
  static void displayCorridors(const unsigned int&,        // dimension x
                               const unsigned int&,        // dimension y
                               const RPG_Map_ZoneList_t&,  // rooms
                               const RPG_Map_ZoneList_t&,  // doors
                               const RPG_Map_ZoneList_t&); // corridors

  static void floodFill(const RPG_Map_Position_t&,  // position
                        const RPG_Map_FloorPlan_t&, // floor plan
                        RPG_Map_Positions_t&);      // return value: (filled) area

  static void createFloorPlan(const unsigned int&,   // map dimension x
                              const unsigned int&,   // map dimension y
                              const unsigned int&,   // #areas (==> "rooms")
                              const bool&,           // rooms to be square ?
                              const bool&,           // maximize rooms ?
                              const unsigned int&,   // min. room area (0: don't care)
                              const bool&,           // want doors (==> "rooms") ?
                              const bool&,           // want corridors (between rooms) ?
                              const bool&,           // doors to fill positions ?
                              const unsigned int&,   // max. #doors/room
                              RPG_Map_Positions_t&,  // return value: seed positions
                              RPG_Map_FloorPlan_t&); // return value: floor plan

  static bool isSquare(const RPG_Map_Positions_t&); // area
  static void perimeter(const RPG_Map_Zone_t&, // room
                        RPG_Map_Zone_t&);      // return value: perimeter
  static unsigned int countAdjacentDoors(const RPG_Map_Positions_t&,  // area
                                         const RPG_Map_FloorPlan_t&); // floor plan
};

#endif
