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

#include <vector>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "rpg_map_common.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_style.h"
#include "rpg_graphics_SDL_window_base.h"

#include "rpg_client_common.h"
#include "rpg_client_iwindow_level.h"

// forward declaration(s)
class RPG_Client_Engine;
class RPG_Engine;

class RPG_Client_Window_Level
 : public RPG_Graphics_SDLWindowBase
 , public RPG_Client_IWindowLevel
{
  typedef RPG_Graphics_SDLWindowBase inherited;

 public:
  RPG_Client_Window_Level (// *** SDL window ***
                           const RPG_Graphics_SDLWindowBase&); // parent
  virtual ~RPG_Client_Window_Level ();

  void toggleMiniMap ();
  bool showMiniMap () const;
  void toggleMessages ();
  bool showMessages () const;

  // implement RPG_Client_IWindowLevel
  inline virtual void drawBorder (SDL_Surface* = NULL,                      // target surface (default: screen)
                                  unsigned int = 0,                         // offset x (top-left = [0,0])
                                  unsigned int = 0) { ACE_ASSERT (false); } // offset y (top-left = [0,0])
  virtual bool initialize (RPG_Client_Engine*, // engine handle
                           RPG_Engine*,        // (level) state handle
                           bool = false);      // debug ?
  virtual void initialize (const RPG_Graphics_Style&); // style
  virtual void setView (int,
                        int,          // view (relative map coordinates)
                        bool = true); // locked access ?
  virtual void setView (const RPG_Map_Position_t&); // view (map coordinates)
  virtual RPG_Graphics_Position_t getView () const; // return value: view (map coordinates !)
  virtual void toggleDoor (const RPG_Map_Position_t&); // door position
  // (re)set lighting blend cache
  virtual void setBlendRadius (ACE_UINT8); // radius
  virtual void updateMinimap ();
  virtual void updateMessageWindow (const std::string&); // message

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw (SDL_Surface* = NULL, // target surface (default: screen)
                     unsigned int = 0,    // offset x (top-left = [0,0])
                     unsigned int = 0);   // offset y (top-left = [0,0])
  virtual void handleEvent (const union SDL_Event&,    // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            struct SDL_Rect&);         // return value: "dirty" region

  // debug
#if defined (_DEBUG)
  inline void toggleShowCoordinates () { myShowCoordinates = !myShowCoordinates; }
#endif // _DEBUG

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_Level ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_Level (const RPG_Client_Window_Level&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_Level& operator= (const RPG_Client_Window_Level&))

  // helper methods
  bool setStyle (const struct RPG_Graphics_StyleUnion&); // style

  void initCeiling ();
  void initWallBlend (bool); // half-height walls ?
  bool initMiniMap (bool = false); // debug ?
  bool initMessageWindow ();

  void drawChild (enum RPG_Graphics_WindowType, // (child) type
                  SDL_Surface* = NULL,          // target surface (default: screen)
                  unsigned int = 0,             // offset x (top-left = [0,0])
                  unsigned int = 0,             // offset y (top-left = [0,0])
                  bool = true);                 // refresh ?

#if defined (_DEBUG)
  bool                                 myDebug;
#endif // _DEBUG
  RPG_Engine*                          myEngine;
  RPG_Client_Engine*                   myClient;
  struct RPG_Client_Action             myClientAction;
  bool                                 myDrawMinimap;
#if defined (_DEBUG)
  bool                                 myShowCoordinates;
#endif // _DEBUG
  bool                                 myShowMessages;

  struct RPG_Graphics_FloorTileSet     myCurrentFloorSet;
  struct RPG_Graphics_FloorEdgeTileSet myCurrentFloorEdgeSet;
  struct RPG_Graphics_WallTileSet      myCurrentWallSet;
  SDL_Surface*                         myCeilingTile;
  struct RPG_Graphics_DoorTileSet      myCurrentDoorSet;
  SDL_Surface*                         myOffMapTile;
  SDL_Surface*                         myInvisibleTile;
  SDL_Surface*                         myVisionBlendTile;
  SDL_Surface*                         myVisionTempTile;

  // tiles / position
  RPG_Graphics_FloorEdgeTileMap_t      myFloorEdgeTiles;
  RPG_Graphics_WallTileMap_t           myWallTiles;
  RPG_Graphics_DoorTileMap_t           myDoorTiles;

  SDL_Surface*                         myWallBlend;
  RPG_Client_BlendingMaskCache_t       myLightingCache;

  mutable ACE_Thread_Mutex             myLock;
  // center of displayed area
  RPG_Map_Position_t                   myView;
};

#endif
