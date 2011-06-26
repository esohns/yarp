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
#ifndef RPG_CLIENT_WINDOW_LEVEL_H
#define RPG_CLIENT_WINDOW_LEVEL_H

#include <rpg_graphics_common.h>
#include <rpg_graphics_SDL_window_base.h>

#include <rpg_map_common.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class RPG_Engine_Level;
class RPG_Client_Engine;

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_WindowLevel
 : public RPG_Graphics_SDLWindowBase
{
 public:
  RPG_Client_WindowLevel(// *** SDL window ***
                         const RPG_Graphics_SDLWindowBase&); // parent
  virtual ~RPG_Client_WindowLevel();

  // adjust viewport
  void setView(const RPG_Map_Position_t&); // view (map coordinates)
  void setView(const int&,
               const int&); // view (relative map coordinates)

  void toggleDoor(const RPG_Map_Position_t&); // door position
  void drawHighlight(const RPG_Graphics_Position_t&); // highlight position
  void storeHighlightBG(const RPG_Map_Position_t&,       // map position
                        const RPG_Graphics_Position_t&); // highlight position
  void restoreHighlightBG();

  // init level properties
  void init(RPG_Engine_Level*,               // state handle
            RPG_Client_Engine*,              // engine handle
            const RPG_Map_FloorPlan_t&,      // floor plan
            const RPG_Graphics_MapStyle_t&); // map style

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
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_WindowLevel());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_WindowLevel(const RPG_Client_WindowLevel&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_WindowLevel& operator=(const RPG_Client_WindowLevel&));

  // helper methods
  void clear();
  void setStyle(const RPG_Graphics_StyleUnion&);

  void initCeiling();
  void initWallBlend();

  RPG_Engine_Level*           myLevelState;
  RPG_Client_Engine*          myEngine;

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

  // center of displayed area
  RPG_Map_Position_t          myView;

  // cursor highlight
  RPG_Map_Position_t          myHighlightBGPosition;
  SDL_Surface*                myHighlightBG;
  SDL_Surface*                myHighlightTile;
};

#endif
