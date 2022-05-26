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

#ifndef SDL_GUI_MAINWINDOW_H
#define SDL_GUI_MAINWINDOW_H

#include <string>

#define _SDL_main_h
#include "SDL.h"

#include "ace/Global_Macros.h"
//#include "ace/Synch.h"

#include "common_ilock.h"

#include "rpg_map_common.h"

#include "rpg_graphics_cursor.h"
#include "rpg_graphics_font.h"
#include "rpg_graphics_image.h"
#include "rpg_graphics_sprite.h"
#include "rpg_graphics_tilegraphic.h"
#include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"
#include "rpg_graphics_toplevel.h"

#include "rpg_client_graphicsmode.h"

#include "SDL_gui_common.h"
#include "SDL_gui_defines.h"

// forward declarations
class RPG_Engine;

/**
  @author Erik Sohns <erik.sohns@web.de>
*/
class SDL_GUI_MainWindow
 : public RPG_Graphics_TopLevel,
   public Common_ILock
{
  typedef RPG_Graphics_TopLevel inherited;

 public:
  SDL_GUI_MainWindow(const RPG_Graphics_Size_t&,                  // size
                     const RPG_Graphics_GraphicTypeUnion&,        // (element) type
                     const std::string&,                          // title
                     const RPG_Graphics_Font& = FONT_MAIN_LARGE); // title font
  virtual ~SDL_GUI_MainWindow();

  // initialize different hotspots/sub-windows
  // *WARNING*: call this AFTER setScreen() !
  void init(state_t*,                                                    // state
//            RPG_Client_Engine*,                                          // (graphics) engine handle
            RPG_Engine*,                                                 // (level) state handle
            const RPG_Client_GraphicsMode& = SDL_GUI_DEF_GRAPHICS_MODE); // graphics mode

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw(SDL_Surface* = NULL, // target surface (default: screen)
                    unsigned int = 0,    // offset x (top-left = [0,0])
                    unsigned int = 0);   // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&);            // return value: "dirty" region
  // implement (part of) RPG_Graphics_IWindow
  virtual void notify(const RPG_Graphics_Cursor&) const;

  // implement RPG_Common_ILock
  virtual bool lock (bool = true); // block ?
  virtual int unlock (bool = false); // unlock completely ?

 private:
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow())
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow(const SDL_GUI_MainWindow&))
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow& operator=(const SDL_GUI_MainWindow&))

  // helper methods
  void initScrollSpots();
  bool initMap(state_t*,                                                    // state
               RPG_Engine*,                                                 // level engine handle
               const RPG_Client_GraphicsMode& = SDL_GUI_DEF_GRAPHICS_MODE); // graphics mode
  void drawBorder(SDL_Surface* = NULL,      // target surface (default: screen)
                  const unsigned int& = 0,  // offset x (top-left = [0,0])
                  const unsigned int& = 0); // offset y (top-left = [0,0])
  void drawTitle(const RPG_Graphics_Font&,  // font
                 const std::string&,        // text
                 SDL_Surface* = NULL);      // target surface (default: screen)

//  RPG_Client_Engine* myEngine;

  // counter
  unsigned int       myScreenshotIndex;

  unsigned int       myLastHoverTime;
  bool               myHaveMouseFocus;

  RPG_Graphics_Font  myTitleFont;
  mode_t             myMode;

  ACE_Thread_Mutex   myScreenLock;
};

#endif
