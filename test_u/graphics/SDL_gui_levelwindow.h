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
#ifndef SDL_GUI_LEVELWINDOW_H
#define SDL_GUI_LEVELWINDOW_H

#include <rpg_graphics_common.h>
#include <rpg_graphics_SDLwindow.h>
#include <rpg_graphics_floorstyle.h>
#include <rpg_graphics_wallstyle.h>
#include <rpg_graphics_doorstyle.h>

#include <rpg_map_common.h>
#include <rpg_map_level.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class SDL_GUI_LevelWindow
 : public RPG_Graphics_SDLWindow
{
 public:
  SDL_GUI_LevelWindow(// *** SDL window ***
                      const RPG_Graphics_SDLWindow&,         // parent
                      const RPG_Graphics_InterfaceWindow_t&, // type
                      // *** level properties ***
                      const RPG_Graphics_FloorStyle&,        // floor style
                      const RPG_Graphics_WallStyle&,         // wall style
                      const RPG_Graphics_DoorStyle&,         // door style
                      const RPG_Map_FloorPlan_t&);           // (initial) map
  virtual ~SDL_GUI_LevelWindow();

  // transformation
  const RPG_Graphics_Position_t screen2Map(const RPG_Graphics_Position_t&); // position (absolute)
  const RPG_Graphics_Position_t map2Screen(const RPG_Graphics_Position_t&); // position (map)

  // set view coordinates
  void setView(const RPG_Graphics_Position_t&); // view coordinates

  // set level properties
  void init(const RPG_Graphics_FloorStyle&, // floor style
            const RPG_Graphics_WallStyle&,  // wall style
            const RPG_Graphics_DoorStyle&,  // door style
            const RPG_Map_FloorPlan_t&);    // map

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface*,                    // target surface (screen !)
                    const RPG_Graphics_Position_t&); // offset

 private:
  typedef RPG_Graphics_SDLWindow inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow(const SDL_GUI_LevelWindow&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow& operator=(const SDL_GUI_LevelWindow&));

  // helper methods
  void setStyle(const RPG_Graphics_StyleUnion&);
  void initWalls(const RPG_Map_FloorPlan_t&,        // level map
                 const RPG_Graphics_WallTileSet_t&, // appropriate (style) tileset
                 RPG_Graphics_WallTileMap_t&);      // return value: wall tiles / position
  void initDoors(const RPG_Map_FloorPlan_t&,        // level map
                 const RPG_Map_Level&,              // level state
                 const RPG_Graphics_DoorTileSet_t&, // appropriate (style) tileset
                 RPG_Graphics_DoorTileMap_t&);      // return value: door tiles / position
  static const RPG_Graphics_Orientation getDoorOrientation(const RPG_Map_Level&,       // level
                                                           const RPG_Map_Position_t&); // door position

  RPG_Map_Level               myMap;

  RPG_Graphics_FloorStyle     myCurrentFloorStyle;
  RPG_Graphics_FloorTileSet_t myCurrentFloorSet;
  RPG_Graphics_WallStyle      myCurrentWallStyle;
  RPG_Graphics_WallTileSet_t  myCurrentWallSet;
  RPG_Graphics_DoorStyle      myCurrentDoorStyle;
  RPG_Graphics_DoorTileSet_t  myCurrentDoorSet;
  SDL_Surface*                myCurrentOffMapTile;

  // wall tiles / position
  RPG_Graphics_WallTileMap_t  myWallTiles;

  // door tiles / position
  RPG_Graphics_DoorTileMap_t  myDoorTiles;

  // center of displayed map area (map coordinate system)
  RPG_Graphics_Position_t     myView;
};

#endif
