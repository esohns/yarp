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

#include "SDL_gui_common.h"

#include "rpg_client_iwindow_level.h"

#include "rpg_engine_common.h"
#include "rpg_engine_iclient.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_style.h"
#include "rpg_graphics_styleunion.h"
#include "rpg_graphics_SDL_window_base.h"
#include "rpg_graphics_cursor_manager.h"

#include "SDL.h"

#include "ace/Global_Macros.h"

#include <string>

// forward declarations
class RPG_Engine;
class RPG_Common_ILock;

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class SDL_GUI_LevelWindow_Isometric
 : public RPG_Graphics_SDLWindowBase,
   public RPG_Client_IWindowLevel,
   public RPG_Engine_IClient
{
 public:
  SDL_GUI_LevelWindow_Isometric(// *** SDL window ***
                                const RPG_Graphics_SDLWindowBase&, // parent
                                // *** level properties ***
                                RPG_Engine*);                      // (level) state handle
  virtual ~SDL_GUI_LevelWindow_Isometric();

  void init(state_t*,           // state handle
            RPG_Common_ILock*); // screen lock interface handle

  // implement (part of) RPG_Client_IWindowLevel
  virtual void init(const RPG_Graphics_Style&); // style

  // adjust viewport
  // implement (part of) RPG_Client_IWindowLevel
  virtual void setView(const int&,  // offset x (map coordinates !)
                       const int&); // offset y (map coordinates !)
  virtual void setView(const RPG_Map_Position_t&); // offset
  virtual RPG_Graphics_Position_t getView() const; // return value: view (map coordinates !)
	void center();

  // (re-)init / set level properties
  // implement (part of) RPG_Client_IWindowLevel
  virtual void toggleDoor(const RPG_Map_Position_t&); // door position
  virtual void setBlendRadius(const unsigned char&); // radius
  virtual void updateMinimap();
  virtual void updateMessageWindow(const std::string&); // message

  bool setStyle(const RPG_Graphics_StyleUnion&);

  // implement RPG_Client_IWindow
  virtual void drawBorder(SDL_Surface* = NULL,      // target surface (default: screen)
                          const unsigned int& = 0,  // offset x (top-left = [0,0])
                          const unsigned int& = 0); // offset y (top-left = [0,0])
  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw(SDL_Surface* = NULL,       // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,          // event
                           RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&);                // return value: "dirty" region

  // implement RPG_Engine_IClient
  //virtual void redraw();
  //// *NOTE*: this triggers a complete redraw !
  //virtual void updateEntity(const RPG_Engine_EntityID_t&);
  virtual void notify(const RPG_Engine_Command&,
                      const RPG_Engine_ClientNotificationParameters_t&);

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow_Isometric());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow_Isometric(const SDL_GUI_LevelWindow_Isometric&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow_Isometric& operator=(const SDL_GUI_LevelWindow_Isometric&));

  // helper types
  typedef std::pair<int, int> RPG_Position_t;

  // helper methods
	bool hasSeen(const RPG_Engine_EntityID_t&,
							 const RPG_Map_Position_t&) const;
	void redrawCursor(const RPG_Graphics_Position_t& =
		RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position(false), // screen position
		                const bool& = true);                                // restore BG ?

  void initCeiling();
  void initWallBlend(const bool&); // half-height walls ?
  void initMiniMap(RPG_Engine*); // level state handle
//  void restoreBG();
	void initTiles();

  state_t*                        myState;
  RPG_Engine*                     myEngine;

  RPG_Graphics_FloorTileSet_t     myCurrentFloorSet;
  RPG_Graphics_FloorEdgeTileSet_t myCurrentFloorEdgeSet;
  RPG_Graphics_WallTileSet_t      myCurrentWallSet;
  SDL_Surface*                    myCurrentCeilingTile;
  RPG_Graphics_DoorTileSet_t      myCurrentDoorSet;
  SDL_Surface*                    myCurrentOffMapTile;

  // floor edge tiles / position
  RPG_Graphics_FloorEdgeTileMap_t myFloorEdgeTiles;

  // wall tiles / position
  RPG_Graphics_WallTileMap_t      myWallTiles;

  bool                            myHideFloor;
  bool                            myHideWalls;

  SDL_Surface*                    myWallBlend;

  // door tiles / position
  RPG_Graphics_DoorTileMap_t      myDoorTiles;

  // center of displayed map area (map coordinates)
  RPG_Graphics_Position_t         myView;

  bool                            myMinimapIsOn;
};

#endif
