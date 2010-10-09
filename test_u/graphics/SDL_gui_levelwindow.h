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
#include <rpg_graphics_SDL_window_base.h>

#include <rpg_map_common.h>
#include <rpg_map_level.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class SDL_GUI_LevelWindow
 : public RPG_Graphics_SDLWindowBase
{
 public:
  SDL_GUI_LevelWindow(// *** SDL window ***
                      const RPG_Graphics_SDLWindowBase&, // parent
                      // *** level properties ***
                      const RPG_Graphics_MapStyle_t&,    // map style
                      const RPG_Map_FloorPlan_t&);       // (initial) map
  virtual ~SDL_GUI_LevelWindow();

  // adjust viewport
  void setView(const RPG_Graphics_Position_t&); // view (map coordinates)
  void setView(const int&,
               const int&); // view (relative map coordinates)
  void centerView();

  // set level properties
  void init(const RPG_Graphics_MapStyle_t&, // map style
            const RPG_Map_FloorPlan_t&);    // map
  void setStyle(const RPG_Graphics_StyleUnion&);

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface* = NULL,       // target surface (default: screen)
                    const unsigned long& = 0,  // offset x (top-left = [0,0])
                    const unsigned long& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&);                // return value: redraw ?

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow(const SDL_GUI_LevelWindow&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow& operator=(const SDL_GUI_LevelWindow&));

  // helper types
  typedef std::pair<int, int> RPG_Position_t;

  // helper methods
  void clear();

  void initCeiling();
  void initWalls(const RPG_Map_FloorPlan_t&,        // level map
                 const RPG_Graphics_WallTileSet_t&, // appropriate (style) tileset
                 RPG_Graphics_WallTileMap_t&);      // return value: wall tiles / position
  void initWallBlend();
  void initDoors(const RPG_Map_FloorPlan_t&,        // level map
                 const RPG_Map_Level&,              // state
                 const RPG_Graphics_DoorTileSet_t&, // appropriate (style) tileset
                 RPG_Graphics_DoorTileMap_t&);      // return value: door tiles / position

  void restoreBG();

  RPG_Map_Level               myMap;

  RPG_Graphics_MapStyle_t     myCurrentMapStyle;
  RPG_Graphics_FloorTileSet_t myCurrentFloorSet;
  RPG_Graphics_WallTileSet_t  myCurrentWallSet;
  SDL_Surface*                myCurrentCeilingTile;
  RPG_Graphics_DoorTileSet_t  myCurrentDoorSet;
  SDL_Surface*                myCurrentOffMapTile;

  // wall tiles / position
  RPG_Graphics_WallTileMap_t  myWallTiles;

  SDL_Surface*                myWallBlend;

  // door tiles / position
  RPG_Graphics_DoorTileMap_t  myDoorTiles;

  // center of displayed map area (map coordinates)
  RPG_Graphics_Position_t     myView;

  // cursor highlight
  RPG_Graphics_Position_t     myHighlightBGPosition; // map coordinates
  SDL_Surface*                myHighlightBG;
  SDL_Surface*                myHighlightTile;
};

#endif
