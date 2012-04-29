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

#ifndef RPG_CLIENT_COMMON_TOOLS_H
#define RPG_CLIENT_COMMON_TOOLS_H

#include "rpg_client_exports.h"

#include <rpg_engine.h>

#include <rpg_graphics_common.h>

#include <rpg_map_common.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_Export RPG_Client_Common_Tools
{
 public:
  // *NOTE*: initializes cursor manager singleton/dictionaries
  static void init(const std::string&,  // sound dictionary
                   const std::string&,  // sound directory
                   const std::string&,  // graphics dictionary
                   const std::string&,  // graphics directory
                   const bool& = true); // init SDL ?

  static void initFloorEdges(const RPG_Engine&,                      // state / engine
                             const RPG_Graphics_FloorEdgeTileSet_t&, // appropriate (style) tileset
                             RPG_Graphics_FloorEdgeTileMap_t&);      // return value: floor edge tiles / position
  static void updateFloorEdges(const RPG_Graphics_FloorEdgeTileSet_t&, // appropriate (style) tileset
                               RPG_Graphics_FloorEdgeTileMap_t&);      // input/output value: floor edge tiles / position
  static void initWalls(const RPG_Engine&,                 // state / engine
                        const RPG_Graphics_WallTileSet_t&, // appropriate (style) tileset
                        RPG_Graphics_WallTileMap_t&);      // return value: wall tiles / position
  static void updateWalls(const RPG_Graphics_WallTileSet_t&, // appropriate (style) tileset
                          RPG_Graphics_WallTileMap_t&);      // input/output value: wall tiles / position
  static void initDoors(const RPG_Engine&,                 // state / engine
                        const RPG_Graphics_DoorTileSet_t&, // appropriate (style) tileset
                        RPG_Graphics_DoorTileMap_t&);      // return value: door tiles / position
  static void updateDoors(const RPG_Graphics_DoorTileSet_t&, // appropriate (style) tileset
                          const RPG_Engine&,                 // state / engine
                          RPG_Graphics_DoorTileMap_t&);      // input/output value: door tiles / position

  static RPG_Graphics_Sprite class2Sprite(const RPG_Character_Class&);
  static RPG_Graphics_Sprite monster2Sprite(const std::string&);

  static bool hasCeiling(const RPG_Map_Position_t&, // position
                         const RPG_Engine&,         // state / engine
                         const bool& = true);       // locked access ?
  static RPG_Graphics_Orientation getDoorOrientation(const RPG_Engine&,          // state / engine
                                                     const RPG_Map_Position_t&); // door
  static RPG_Graphics_Cursor getCursor(const RPG_Map_Position_t&, // position
                                       const RPG_Engine&);        // state / engine

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Client_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Common_Tools(const RPG_Client_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Common_Tools& operator=(const RPG_Client_Common_Tools&));
};

#endif
