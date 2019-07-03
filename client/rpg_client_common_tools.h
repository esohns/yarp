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

#include <string>

#include "ace/Global_Macros.h"

#include "rpg_map_common.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_style.h"

#include "rpg_engine.h"

#include "rpg_client_common.h"
//#include "rpg_client_exports.h"

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_Common_Tools
{
 public:
  static bool initializeSDLInput(const RPG_Client_SDL_InputConfiguration&); // SDL input configuration
  // *NOTE*: initializes cursor manager singleton/dictionaries
  static bool initialize (// *** input ***
                          const struct RPG_Client_SDL_InputConfiguration&, // SDL input configuration
                          // *** audio ***
                          const struct RPG_Sound_SDLConfiguration&,        // SDL sound configuration
                          const std::string&,                         // sound directory
                          bool,                                       // use CD ?
                          unsigned int,                               // cache size
                          bool,                                       // mute ?
                          const std::string&,                         // sound dictionary
                          // *** video ***
                          const std::string&,                         // graphics directory
                          unsigned int,                               // cache size
                          const std::string&,                         // graphics dictionary
                          bool = true);                               // init SDL ?
  static void finalize ();

  static void initFloorEdges(const RPG_Engine&,                      // state / engine
                             const struct RPG_Graphics_FloorEdgeTileSet&, // appropriate (style) tileset
                             RPG_Graphics_FloorEdgeTileMap_t&);      // return value: floor edge tiles / position
  static void updateFloorEdges(const struct RPG_Graphics_FloorEdgeTileSet&, // appropriate (style) tileset
                               RPG_Graphics_FloorEdgeTileMap_t&);      // input/output value: floor edge tiles / position
  static void initWalls(const RPG_Engine&,                 // state / engine
                        const struct RPG_Graphics_WallTileSet&, // appropriate (style) tileset
                        RPG_Graphics_WallTileMap_t&);      // return value: wall tiles / position
  static void updateWalls(const struct RPG_Graphics_WallTileSet&, // appropriate (style) tileset
                          RPG_Graphics_WallTileMap_t&);      // input/output value: wall tiles / position
  static void initDoors(const RPG_Engine&,                 // state / engine
                        const struct RPG_Graphics_DoorTileSet&, // appropriate (style) tileset
                        RPG_Graphics_DoorTileMap_t&);      // return value: door tiles / position
  static void updateDoors(const struct RPG_Graphics_DoorTileSet&, // appropriate (style) tileset
                          const RPG_Engine&,                 // state / engine
                          RPG_Graphics_DoorTileMap_t&);      // input/output value: door tiles / position

  static RPG_Graphics_Sprite classToSprite(const RPG_Character_Class&);
  static RPG_Graphics_Sprite monsterToSprite(const std::string&);

  static bool hasCeiling (const RPG_Map_Position_t&, // position
                          const RPG_Engine&,         // state / engine
                          const bool& = true);       // locked access ?
  static bool isVisible (const RPG_Graphics_Positions_t&, // positions (map coordinates !)
                         const RPG_Graphics_Size_t&,      // window size
                         const RPG_Graphics_Position_t&,  // viewport (map coordinates !)
                         const SDL_Rect&,                 // window area
                         const bool& = true);             // any ? : all
  static bool hasHighlight (const RPG_Map_Position_t&, // position
                            const RPG_Engine&,         // state / engine
                            const bool& = true);       // locked access ?
  static RPG_Graphics_Orientation getDoorOrientation(const RPG_Map_Position_t&, // door position
                                                     const RPG_Engine&,         // state / engine
                                                     const bool& = true);       // locked access ?

  static RPG_Graphics_Cursor getCursor(const RPG_Map_Position_t&,       // position
                                       const RPG_Engine_EntityID_t&,    // active player
                                       const bool&,                     // has player seen this position ?
                                       const RPG_Client_SelectionMode&, // current selection mode
                                       const RPG_Engine&,               // state / engine
                                       const bool& = true);             // locked access ?
  static RPG_Graphics_Style environmentToStyle(const RPG_Common_Environment&); // environment

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Client_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Common_Tools (const RPG_Client_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Common_Tools& operator= (const RPG_Client_Common_Tools&))

  // helper methods
  static void initializeUserProfiles();
};

#endif
