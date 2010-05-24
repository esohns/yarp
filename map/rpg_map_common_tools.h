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
                                 RPG_Map_DungeonLevel&); // return value: doors & walls

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
  static void makeRooms(const RPG_Map_Partition_t&, // partition
                        const bool&,                // rooms to be square ?
                        const bool&,                // maximize area ?
                        const unsigned long&,       // min. area (0: don't care)
                        RPG_Map_ZoneList_t&);       // return value: room(s)
  static void displayRooms(const unsigned long&,       // dimension x
                           const unsigned long&,       // dimension y
                           const RPG_Map_ZoneList_t&); // room(s)

  static void connectRooms(const RPG_Map_ZoneList_t&, // room(s)
                           RPG_Map_DungeonLevel&);    // return value: doors & walls
};

#endif
