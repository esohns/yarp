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

#include "rpg_map_exports.h"
#include "rpg_map_defines.h"
#include "rpg_map_common.h"

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Map_Export RPG_Map_Level
{
 public:
  RPG_Map_Level(const RPG_Map_t&); // map
  virtual ~RPG_Map_Level();

  // static functionality
  static void create(const RPG_Map_FloorPlan_Config_t&, // floor plan config
                     RPG_Map_t&);                       // return value: map
  static bool load(const std::string&, // FQ filename
                   RPG_Map_t&,         // return value: map
                   const bool& = RPG_MAP_DEF_TRACE_SCANNING, // trace scanning ?
                   const bool& = RPG_MAP_DEF_TRACE_PARSING); // trace parsing ?
  static void print(const RPG_Map_t&); // map
  static std::string info(const RPG_Map_t&); // map

  void init(const RPG_Map_t&); // map
  void save(const std::string&) const; // FQ filename

  const RPG_Map_Position_t& getStartPosition() const;
  const RPG_Map_Positions_t& getSeedPoints() const;
  const RPG_Map_FloorPlan_t& getFloorPlan() const;

  RPG_Map_Size_t getSize() const;

  // either floor or an open (!) door ?
  bool isValid(const RPG_Map_Position_t&) const;
  bool isCorner(const RPG_Map_Position_t&)const;
  RPG_Map_Element getElement(const RPG_Map_Position_t&) const;
  const RPG_Map_Door_t& getDoor(const RPG_Map_Position_t&) const;
  bool findPath(const RPG_Map_Position_t&, // start position
                const RPG_Map_Position_t&, // end position
                RPG_Map_Path_t&) const;    // return value: (partial) path A --> B
  void findValid(const RPG_Map_Position_t&,   // center
                 const unsigned int&,         // max (square !) radius
                 RPG_Map_Positions_t&) const; // return value: area

 protected:
  RPG_Map_Level();

  RPG_Map_t myMap;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Level(const RPG_Map_Level&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Map_Level& operator=(const RPG_Map_Level&));
};

#endif
