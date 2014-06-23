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

#ifndef RPG_CLIENT_WINDOW_MAIN_H
#define RPG_CLIENT_WINDOW_MAIN_H

#include "rpg_client_exports.h"
#include "rpg_client_iwindow.h"

#include "rpg_graphics_cursor.h"
#include "rpg_graphics_font.h"
#include "rpg_graphics_image.h"
#include "rpg_graphics_tilegraphic.h"
#include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"
#include "rpg_graphics_toplevel.h"

#include <SDL.h>

#include <ace/Global_Macros.h>

#include <string>

// forward declaration(s)
class RPG_Client_Engine;
class RPG_Engine;

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_Export RPG_Client_Window_Main
 : public RPG_Graphics_TopLevel,
   public RPG_Client_IWindow
{
 public:
  RPG_Client_Window_Main(const RPG_Graphics_Size_t&,                  // size
                         const RPG_Graphics_GraphicTypeUnion&,        // (element) type
                         const std::string&,                          // title
                         const RPG_Graphics_Font& = FONT_MAIN_LARGE); // title font
  virtual ~RPG_Client_Window_Main();

  // initialize different hotspots
  // *WARNING*: call this AFTER setScreen() !
  void init(RPG_Client_Engine*,   // engine handle
            const bool&,          // do automatic edge scroll ?
            RPG_Engine*,          // (level) state handle
            const bool& = false); // debug ?

  // implement RPG_Client_IWindow
  virtual void drawBorder(SDL_Surface* = NULL,      // target surface (default: screen)
                          const unsigned int& = 0,  // offset x (top-left = [0,0])
                          const unsigned int& = 0); // offset y (top-left = [0,0])

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw(SDL_Surface* = NULL,      // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,          // event
                           RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&);                // return value: "dirty" region
  virtual void notify(const RPG_Graphics_Cursor&) const;

 private:
  typedef RPG_Graphics_TopLevel inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_Main());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_Main(const RPG_Client_Window_Main&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_Main& operator=(const RPG_Client_Window_Main&));

  // helper methods
  void initScrollSpots(const bool& = false); // debug ?
  void initMap(RPG_Client_Engine*,   // engine handle
               RPG_Engine*,          // (level) state handle
               const bool& = false); // debug ?

  RPG_Client_Engine* myEngine;

  // counter
  unsigned int       myScreenshotIndex;

  unsigned int       myLastHoverTime;
  bool               myAutoEdgeScroll;
  RPG_Graphics_Font  myTitleFont;
};

#endif
