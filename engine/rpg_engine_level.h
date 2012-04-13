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

#ifndef RPG_ENGINE_LEVEL_H
#define RPG_ENGINE_LEVEL_H

#include "rpg_engine_exports.h"
#include "rpg_engine_common.h"
#include "rpg_engine_XML_tree.h"

#include <rpg_map_common.h>
#include <rpg_map_level.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Engine_Export RPG_Engine_Level
 : public RPG_Map_Level
{
 public:
  RPG_Engine_Level();
  virtual ~RPG_Engine_Level();

  // static functionality
  static void create(const RPG_Map_FloorPlan_Config_t&, // floor plan config
                     RPG_Engine_Level_t&);              // return value: level
  static RPG_Engine_Level_t load(const std::string&,  // FQ filename
                                 const std::string&); // schema repository (directory)
  static void print(const RPG_Engine_Level_t&); // level

  void init(const RPG_Engine_Level_t&); // level
  void save(const std::string&) const; // FQ filename

  const RPG_Engine_LevelMeta_t& getMeta() const;

 protected:
  void handleDoor(const RPG_Map_Position_t&, // position
                  const bool&,               // open ? : close
                  bool&);                    // return value: toggled ?
  bool findPath(const RPG_Map_Position_t&,  // start position
                const RPG_Map_Position_t&,  // end position
                const RPG_Map_Positions_t&, // obstacles
                RPG_Map_Path_t&) const;     // return value: (partial) path A --> B

  RPG_Engine_LevelMeta_t myLevelMeta;

 private:
  typedef RPG_Map_Level inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Level(const RPG_Engine_Level&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Level& operator=(const RPG_Engine_Level&));

  // helper methods
  // *WARNING*: result needs to be delete()d !
  RPG_Engine_Level_XMLTree_Type* toLevelXML() const;
  static RPG_Engine_Level_t levelXMLToLevel(const RPG_Engine_Level_XMLTree_Type&);
};
#endif
