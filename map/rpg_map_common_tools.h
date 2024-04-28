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

#include <list>
#include <set>
#include <string>

#include "ace/Global_Macros.h"

#include "rpg_map_common.h"
#include "rpg_map_defines.h"

class RPG_Map_Common_Tools
{
 public:
  static void initializeStringConversionTables ();

  static unsigned int distance (const RPG_Map_Position_t&,  // position 1
                                const RPG_Map_Position_t&); // position 2
  static unsigned int distanceMax (const RPG_Map_Position_t&,  // position 1
                                   const RPG_Map_Position_t&); // position 2
  static bool isAdjacent (const RPG_Map_Position_t&,  // position 1
                          const RPG_Map_Position_t&); // position 2
  static std::string toString (enum RPG_Map_Orientation);
  static std::string toString (const struct RPG_Map&);
  static std::string toString (enum RPG_Map_Element);

  // *TODO*: relies on the size of the room in relation to its' surrounding space
  // --> results may be wrong for small enclosures
  static enum RPG_Map_Direction doorToExitDirection (const RPG_Map_Position_t&,     // door-
                                                     const struct RPG_Map_FloorPlan&);
  static bool isFloor (const RPG_Map_Position_t&,
                       const struct RPG_Map_FloorPlan&);

  // *NOTE*: relies on a consistent notion of "outside" set for all doors !
  static bool isInsideRoom (const RPG_Map_Position_t&,
                            const struct RPG_Map_FloorPlan&);
  static bool roomsAreSquare (const struct RPG_Map&); // map

  static bool hasLineOfSight(const RPG_Map_Position_t&,  // start position
                             const RPG_Map_Position_t&,  // end position
                             const RPG_Map_Positions_t&, // obstacles
                             bool = false);              // target can be an obstacle ?

  static void createFloorPlan (unsigned int,          // map dimension x
                               unsigned int,          // map dimension y
                               unsigned int,          // #areas (==> "rooms")
                               bool,                  // rooms to be square ?
                               bool,                  // maximize rooms ?
                               unsigned int,          // min. room area (0: don't care)
                               bool,                  // want doors (==> "rooms") ?
                               bool,                  // want corridors (between rooms) ?
                               bool,                  // doors to fill positions ?
                               unsigned int,          // max. #doors/room
                               RPG_Map_Positions_t&,  // return value: seed positions
                               struct RPG_Map_FloorPlan&); // return value: floor plan
  static void buildCorridor (const RPG_Map_Path_t&, // path
                             RPG_Map_Positions_t&); // return value: corridor
  static void buildSquare (const RPG_Map_Position_t&, // center
                           const RPG_Map_Size_t&,     // map size
                           unsigned int,              // radius
                           bool,                      // fill area ?
                          RPG_Map_Positions_t&);     // return value: area
  static void buildCircle (const RPG_Map_Position_t&, // center
                           const RPG_Map_Size_t&,     // map size
                           unsigned int,              // radius
                           bool,                      // fill area ?
                           RPG_Map_Positions_t&);     // return value: area

	static std::string getMapsDirectory ();

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Map_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_Common_Tools (const RPG_Map_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Map_Common_Tools& operator= (const RPG_Map_Common_Tools&))

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
    inline bool operator() (const RPG_Map_Position_t& position1,
                            const RPG_Map_Position_t& position2) const
    {
      return (  (position1.second < position2.second) ||
              (!(position2.second < position1.second) && (position1.first < position2.first)));
    }
  };
  typedef std::set<RPG_Map_Position_t, position_compare> RPG_Map_Area_t;
  typedef RPG_Map_Area_t::const_iterator RPG_Map_AreaConstIterator_t;
  typedef RPG_Map_Area_t::iterator RPG_Map_AreaIterator_t;
  struct RPG_Map_Zone_t
  {
    RPG_Map_Area_t area;
    RPG_Map_Position_t seed;
  };
  static void dump (const RPG_Map_Area_t&); // zone
  typedef std::list<RPG_Map_Zone_t> RPG_Map_Partition_t;
  typedef RPG_Map_Partition_t::const_iterator RPG_Map_PartitionConstIterator_t;
  typedef RPG_Map_Partition_t::iterator RPG_Map_PartitionIterator_t;
  static void makePartition (unsigned int,          // dimension x
                             unsigned int,          // dimension y
                             unsigned int,          // #partitions
                             bool,                  // resolve conflicts ?
                             RPG_Map_Positions_t&,  // return value: conflicts
                             RPG_Map_Partition_t&); // return value: partition
  static void displayPartition (unsigned int,                // dimension x
                                unsigned int,                // dimension y
                                const RPG_Map_Positions_t&,  // conflicts
                                const RPG_Map_Positions_t&,  // seed points
                                const RPG_Map_Partition_t&); // partition

  typedef std::list<RPG_Map_Area_t> RPG_Map_AreaList_t;
  typedef RPG_Map_AreaList_t::const_iterator RPG_Map_AreaListConstIterator_t;
  typedef RPG_Map_AreaList_t::iterator RPG_Map_AreaListIterator_t;
  typedef std::list<RPG_Map_Positions_t> RPG_Map_PositionsList_t;
  typedef RPG_Map_PositionsList_t::iterator RPG_Map_PositionsListIterator_t;
  typedef RPG_Map_PositionsList_t::const_iterator RPG_Map_PositionsListConstIterator_t;
  struct RPG_Map_Square_t
  {
    RPG_Map_Position_t ul;
    RPG_Map_Position_t lr;
  };
  typedef std::list<RPG_Map_Square_t> RPG_Map_SquareList_t;
  typedef RPG_Map_SquareList_t::const_iterator RPG_Map_SquareListConstIterator_t;
  static unsigned int areaToPositions (const RPG_Map_Position_t&,  // position 1
                                       const RPG_Map_Position_t&); // position 2
  static bool positionInSquare (const RPG_Map_Position_t&, // position
                                const RPG_Map_Square_t&);  // square
  static void findMaxSquare (const RPG_Map_Area_t&, // room
                             RPG_Map_Square_t&);    // return value: max square
  static void makeRooms (unsigned int,               // dimension x
                         unsigned int,               // dimension y
                         const RPG_Map_Partition_t&, // partition
                         bool,                       // rooms to be square ?
                         bool,                       // want room separation ?
                         bool,                       // crop areas ?
                         bool,                       // maximize rooms ?
                         unsigned int,               // min. room area (0: don't care)
                         RPG_Map_AreaList_t&,        // return value: room(s)
                         RPG_Map_AreaList_t&);       // return value: boundary(s)
  static void displayRooms (unsigned int,               // dimension x
                            unsigned int,               // dimension y
                            const RPG_Map_AreaList_t&); // room(s)
  static void displayRoom (unsigned int,           // dimension x
                           unsigned int,           // dimension y
                           const RPG_Map_Area_t&); // room

  typedef RPG_Map_Direction ORIGIN;
  static bool turn (const RPG_Map_Area_t&,     // map
                    const RPG_Map_Position_t&, // position
                    const ORIGIN&,             // origin
                    bool,                      // turn clockwise ?
                    bool&,                     // return value: is corner ? (else intersection)
                    enum RPG_Map_Direction&);  // return value: next direction
  static void findDoorPositions (const RPG_Map_Area_t&,    // room (shell !)
                                 bool,                     // doors to fill positions ?
                                 RPG_Map_PositionList_t&); // suitable position(s)
  static void makeDoors (unsigned int,              // dimension x
                         unsigned int,              // dimension y
                         const RPG_Map_AreaList_t&, // room boundary(s)
                         bool,                      // doors to fill positions ?
                         unsigned int,              // max. #doors/room
                         RPG_Map_AreaList_t&);      // return value: doors

  static bool intersect (const RPG_Map_Area_t&,     // map
                         const RPG_Map_Position_t&, // position
                         const ORIGIN&,             // origin
                         RPG_Map_Directions_t&,     // return value: possible direction(s)
                         enum RPG_Map_Direction&);  // return value: next direction, if NOT an intersection
  // *NOTE*: proceeds in clockwise direction
  static void crawlToPosition (const RPG_Map_Area_t&,     // map
                               const RPG_Map_Position_t&, // origin
                               const RPG_Map_Position_t&, // target
                               const ORIGIN&,             // starting origin
                               RPG_Map_PositionList_t&);  // return value: trail
  // *NOTE*: this uses alternate sorting left-to-right, top-to-bottom (0,0 is top-left)
  // --> (0,0), (0,1), ..., (0,dimy-1), (1,0), ..., (1,dimy-1), ..., (dimx-1,dimy-1)
  typedef std::set<RPG_Map_Position_t> RPG_Map_AltPositions_t;
  typedef RPG_Map_AltPositions_t::const_iterator RPG_Map_AltPositionsConstIterator_t;
  static void crop(RPG_Map_Area_t&); // room
//   static void cropSquareBoundary(RPG_Map_Zone_t&); // room
  static enum RPG_Map_Direction doorToExitDirection (const RPG_Map_Area_t&,      // room
                                                     const RPG_Map_Position_t&); // door
  static void connectRooms (unsigned int,              // dimension x
                            unsigned int,              // dimension y
                            const RPG_Map_AreaList_t&, // boundary(s)
                            const RPG_Map_AreaList_t&, // doors
                            const RPG_Map_AreaList_t&, // room(s) // *TODO*: faster in/out tests ?
                            struct RPG_Map_FloorPlan&);     // return value: doors & walls
  static void displayCorridors (unsigned int,                    // dimension x
                                unsigned int,                    // dimension y
                                const RPG_Map_AreaList_t&,       // rooms
                                const RPG_Map_AreaList_t&,       // doors
                                const RPG_Map_PositionsList_t&); // corridors

  static void floodFill (const RPG_Map_Position_t&,  // position
                         const struct RPG_Map_FloorPlan&, // floor plan
                         RPG_Map_Positions_t&);      // return value: (filled) area

  static bool isSquare (const RPG_Map_Positions_t&); // area
  static void perimeter (const RPG_Map_Area_t&, // room
                         RPG_Map_Positions_t&); // return value: perimeter
  static unsigned int countAdjacentDoors (const RPG_Map_Positions_t&,  // area
                                          const struct RPG_Map_FloorPlan&); // floor plan
};

#endif
