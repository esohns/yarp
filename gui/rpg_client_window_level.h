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
#ifndef RPG_CLIENT_WINDOWMAIN_H
#define RPG_CLIENT_WINDOWMAIN_H

#include "rpg_client_common.h"

#include <rpg_graphics_common.h>
#include <rpg_graphics_SDLwindow.h>
#include <rpg_graphics_type.h>
#include <rpg_graphics_floorstyle.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_WindowLevel
 : public RPG_Graphics_SDLWindow
{
 public:
  RPG_Client_WindowLevel(const RPG_Graphics_SDLWindow&,         // parent window
                         const RPG_Graphics_InterfaceWindow_t&, // type
                         const RPG_Graphics_Type&);             // style
  virtual ~RPG_Client_WindowLevel();

  // set view coordinates
  void setView(const RPG_Graphics_Position_t&); // view coordinates

  // set map properties
  void setMap(const RPG_Client_DungeonLevel&); // map properties

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface*,                    // target surface (screen !)
                    const RPG_Graphics_Position_t&); // offset

 private:
  typedef RPG_Graphics_SDLWindow inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_WindowLevel());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_WindowLevel(const RPG_Client_WindowLevel&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_WindowLevel& operator=(const RPG_Client_WindowLevel&));

  // helper methods
  void setStyle(const RPG_Graphics_StyleUnion&);
  static void loadTileset(const RPG_Graphics_Type&, // index type (== offset)
                          const unsigned long&,     // #tiles in set
                          RPG_Graphics_Tiles_t&);   // return value: tileset

  RPG_Graphics_TileSet_t  myCurrentFloorSet;
  RPG_Graphics_TileSet_t  myCurrentWallSet;

  // center of displayed map area (map coordinate system)
  RPG_Graphics_Position_t myView;

  RPG_Client_DungeonLevel myMap;
  bool                    myInitialized;
};

#endif
