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

#ifndef SDL_GUI_LEVELWINDOW_3D_H
#define SDL_GUI_LEVELWINDOW_3D_H

#include <string>

#include "ace/Global_Macros.h"

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "common_ilock.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_style.h"
#include "rpg_graphics_styleunion.h"
#include "rpg_graphics_SDL_window_base.h"

#include "rpg_engine_common.h"
#include "rpg_engine_iclient.h"

#include "rpg_client_iwindow_level.h"

#include "SDL_gui_common.h"

// forward declarations
class RPG_Engine;

class SDL_GUI_LevelWindow_3D
 : public RPG_Graphics_SDLWindowBase,
   public RPG_Client_IWindowLevel,
   public RPG_Engine_IClient
{
  typedef RPG_Graphics_SDLWindowBase inherited;

 public:
  SDL_GUI_LevelWindow_3D (// *** SDL window ***
                          const RPG_Graphics_SDLWindowBase&, // parent
                          // *** level properties ***
                          RPG_Engine*);                      // (level) state handle
  virtual ~SDL_GUI_LevelWindow_3D ();

  void initialize (state_t*,           // state handle
                   Common_ILock*); // screen lock interface handle

  // implement (part of) RPG_Client_IWindowLevel
  virtual void initialize (const RPG_Graphics_Style&); // style
  virtual void setView (int,          // offset x (map coordinates !)
                        int,          // offset x (map coordinates !)
                        bool = true); // locked access ?
  virtual void setView (const RPG_Map_Position_t&); // position
  virtual RPG_Graphics_Position_t getView () const; // return value: view (map coordinates !)

  void center ();

  // (re-)init / set level properties
  // implement (part of) RPG_Client_IWindowLevel
  virtual void toggleDoor (const RPG_Map_Position_t&); // door position
  virtual void setBlendRadius (unsigned char); // radius
  virtual void updateMinimap ();
  virtual void updateMessageWindow (const std::string&); // message

  bool setStyle (const struct RPG_Graphics_StyleUnion&);

  // implement RPG_Client_IWindow
  virtual void drawBorder (SDL_Surface* = NULL, // target surface (default: screen)
                           unsigned int = 0,    // offset x (top-left = [0,0])
                           unsigned int = 0);   // offset y (top-left = [0,0])
  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw (SDL_Surface* = NULL, // target surface (default: screen)
                     unsigned int = 0,    // offset x (top-left = [0,0])
                     unsigned int = 0);   // offset y (top-left = [0,0])
  virtual void update (SDL_Surface* = NULL); // target surface (default: screen)
  virtual void handleEvent (const SDL_Event&,          // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            struct SDL_Rect&);         // return value: "dirty" region

  // implement RPG_Engine_IClient
  //virtual void redraw();
  //// *NOTE*: this triggers a complete redraw !
  //virtual void updateEntity(const RPG_Engine_EntityID_t&);
  virtual void notify (enum RPG_Engine_Command,
                       const struct RPG_Engine_ClientNotificationParameters&,
                       bool = true);

 private:
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow_3D())
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow_3D(const SDL_GUI_LevelWindow_3D&))
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_LevelWindow_3D& operator=(const SDL_GUI_LevelWindow_3D&))

  // helper types
  typedef std::pair<int, int> RPG_Position_t;

  // helper methods
  void clear ();

  void initCeiling ();
  void initWallBlend (bool); // half-height walls ?
  void initMiniMap (RPG_Engine*); // level state handle
  void initTiles ();

  state_t*                        myState;
  RPG_Engine*                     myEngine;

  RPG_Graphics_Style              myCurrentMapStyle;
  struct RPG_Graphics_FloorTileSet     myCurrentFloorSet;
  struct RPG_Graphics_FloorEdgeTileSet myCurrentFloorEdgeSet;
  struct RPG_Graphics_WallTileSet      myCurrentWallSet;
  SDL_Surface*                    myCurrentCeilingTile;
  struct RPG_Graphics_DoorTileSet      myCurrentDoorSet;
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
