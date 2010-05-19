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
                                 const unsigned long&,   // # rooms
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
  typedef RPG_Map_Positions_t RPG_Map_Area_t;
  typedef std::list<RPG_Map_Area_t> RPG_Map_Partition_t;
  typedef RPG_Map_Partition_t::const_iterator RPG_Map_PartitionIterator_t;
  static void makePartition(const unsigned long&,  // dimension x
                            const unsigned long&,  // dimension y
                            const unsigned long&,  // # partitions
                            RPG_Map_Partition_t&); // return value: partition

  typedef std::list<RPG_Map_Area_t> RPG_Map_AreaList_t;
  typedef RPG_Map_AreaList_t::const_iterator RPG_Map_AreaListIterator_t;
  static void makeRooms(const RPG_Map_Partition_t&, // partition
                        RPG_Map_AreaList_t&);       // return value: room(s)

  static void connectRooms(const RPG_Map_AreaList_t&, // room(s)
                           RPG_Map_DungeonLevel&);    // return value: doors & walls
};

#endif
