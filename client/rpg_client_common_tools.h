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

#include <rpg_engine_level.h>

#include <rpg_graphics_common.h>

#include <rpg_map_common.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_Export RPG_Client_Common_Tools
{
 public:
  // *NOTE*: initializes cursor manager singleton
  static void init();

  static void initFloorEdges(const RPG_Map_FloorPlan_t&,             // floor plan
                             const RPG_Graphics_FloorEdgeTileSet_t&, // appropriate (style) tileset
                             RPG_Graphics_FloorEdgeTileMap_t&);      // return value: floor edge tiles / position
  static void updateFloorEdges(const RPG_Graphics_FloorEdgeTileSet_t&, // appropriate (style) tileset
                               RPG_Graphics_FloorEdgeTileMap_t&);      // input/output value: floor edge tiles / position
  static void initWalls(const RPG_Map_FloorPlan_t&,        // floor plan
                        const RPG_Graphics_WallTileSet_t&, // appropriate (style) tileset
                        RPG_Graphics_WallTileMap_t&);      // return value: wall tiles / position
  static void updateWalls(const RPG_Graphics_WallTileSet_t&, // appropriate (style) tileset
                          RPG_Graphics_WallTileMap_t&);      // input/output value: wall tiles / position
  static void initDoors(const RPG_Map_FloorPlan_t&,        // floor plan
                        const RPG_Engine_Level&,           // state
                        const RPG_Graphics_DoorTileSet_t&, // appropriate (style) tileset
                        RPG_Graphics_DoorTileMap_t&);      // return value: door tiles / position
  static void updateDoors(const RPG_Graphics_DoorTileSet_t&, // appropriate (style) tileset
                          const RPG_Engine_Level&,           // state
                          RPG_Graphics_DoorTileMap_t&);      // input/output value: door tiles / position

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Client_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Common_Tools(const RPG_Client_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Common_Tools& operator=(const RPG_Client_Common_Tools&));
};

#endif
