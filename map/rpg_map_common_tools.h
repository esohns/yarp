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

#include "rpg_map_common.h"

#include <ace/Global_Macros.h>

#include <list>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Map_Common_Tools
{
 public:
  static void createDungeonLevel(const unsigned long&,   // map dimension x
                                 const unsigned long&,   // map dimension y
                                 const unsigned long&,   // #rooms
                                 const bool&,            // rooms to be square ?
                                 const bool&,            // maximize area ?
                                 const unsigned long&,   // min. area (0: don't care)
                                 const bool&,            // doors to fill positions ?
                                 const unsigned long&,   // max. #doors/room
                                 RPG_Map_DungeonLevel&); // return value: doors & walls
  static void displayDungeonLevel(const unsigned long&,         // map dimension x
                                  const unsigned long&,         // map dimension y
                                  const RPG_Map_DungeonLevel&); // doors & walls

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Map_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Common_Tools(const RPG_Map_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Common_Tools& operator=(const RPG_Map_Common_Tools&));

  // helper types/methods
  // *WARNING*: make sure the set of positions forms one (connected) area !
  // (cannot enforce this without a (more complicated) algebraic definition...)
  typedef RPG_Map_Positions_t RPG_Map_Zone_t;
  typedef RPG_Map_Zone_t::const_iterator RPG_Map_ZoneConstIterator_t;
  typedef RPG_Map_Zone_t::iterator RPG_Map_ZoneIterator_t;
  static void dump(const RPG_Map_Zone_t&); // zone
  typedef std::list<RPG_Map_Zone_t> RPG_Map_Partition_t;
  typedef RPG_Map_Partition_t::const_iterator RPG_Map_PartitionConstIterator_t;
  typedef RPG_Map_Partition_t::iterator RPG_Map_PartitionIterator_t;
  static const unsigned long dist2Positions(const RPG_Map_Position_t&,  // position 1
                                            const RPG_Map_Position_t&); // position 2
  static void makePartition(const unsigned long&,  // dimension x
                            const unsigned long&,  // dimension y
                            const unsigned long&,  // #partitions
                            RPG_Map_Partition_t&); // return value: partition
  static void displayPartition(const unsigned long&,        // dimension x
                               const unsigned long&,        // dimension y
                               const RPG_Map_Positions_t&,  // seed points
                               const RPG_Map_Positions_t&,  // conflicts
                               const RPG_Map_Partition_t&); // partition

  typedef std::list<RPG_Map_Zone_t> RPG_Map_ZoneList_t;
  typedef RPG_Map_ZoneList_t::const_iterator RPG_Map_ZoneListConstIterator_t;
  typedef RPG_Map_ZoneList_t::iterator RPG_Map_ZoneListIterator_t;
  struct RPG_Map_Square_t
  {
    RPG_Map_Position_t ul;
    RPG_Map_Position_t lr;
  };
  typedef std::list<RPG_Map_Square_t> RPG_Map_SquareList_t;
  typedef RPG_Map_SquareList_t::const_iterator RPG_Map_SquareListConstIterator_t;
  static const unsigned long area2Positions(const RPG_Map_Position_t&,  // position 1
                                            const RPG_Map_Position_t&); // position 2
  static const bool positionInSquare(const RPG_Map_Position_t&, // position
                                     const RPG_Map_Square_t&);  // square
  static void findMaxSquare(const RPG_Map_Zone_t&, // room
                            RPG_Map_Square_t&);    // return value: max square
  static void makeRooms(const unsigned long&,       // dimension x
                        const unsigned long&,       // dimension y
                        const RPG_Map_Partition_t&, // partition
                        const bool&,                // rooms to be square ?
                        const bool&,                // crop areas ?
                        const bool&,                // maximize area ?
                        const unsigned long&,       // min. area (0: don't care)
                        RPG_Map_ZoneList_t&);       // return value: room(s)
  static void displayRooms(const unsigned long&,       // dimension x
                           const unsigned long&,       // dimension y
                           const RPG_Map_ZoneList_t&); // room(s)

  typedef RPG_Map_Direction_t ORIGIN;
  static const bool intersect(const RPG_Map_Zone_t&,     // map
                              const RPG_Map_Position_t&, // position
                              const ORIGIN&,             // origin
                              RPG_Map_Directions_t&,     // possible direction(s)
                              RPG_Map_Direction_t&);     // next direction, if NOT an intersection
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
  static const bool turn(const RPG_Map_Zone_t&,     // map
                         const RPG_Map_Position_t&, // position
                         const ORIGIN&,             // origin
                         const bool&,               // turn clockwise ?
                         bool&,                     // return value: is corner ? (else intersection)
                         RPG_Map_Direction_t&);     // return value: next direction
  static void findDoorPositions(const RPG_Map_Zone_t&,    // room (shell !)
                                const bool&,              // doors to fill positions ?
                                RPG_Map_PositionList_t&); // suitable position(s)
  static void connectRooms(const unsigned long&,      // dimension x
                           const unsigned long&,      // dimension y
                           const RPG_Map_ZoneList_t&, // room(s)
                           const bool&,               // doors to fill positions ?
                           const unsigned long&,      // max. #doors/room
                           RPG_Map_DungeonLevel&);    // return value: doors & walls
};

#endif
