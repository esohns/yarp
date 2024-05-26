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

#ifndef RPG_CLIENT_WINDOW_MAIN_STUB_H
#define RPG_CLIENT_WINDOW_MAIN_STUB_H

#include <string>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"

#include "rpg_graphics_SDL_window_base.h"

#include "rpg_client_iwindow.h"

// forward declaration(s)
class RPG_Client_Engine;
class RPG_Engine;

class RPG_Client_Window_Main_Stub
 : public RPG_Graphics_SDLWindowBase
 , public RPG_Client_IWindow
{
  typedef RPG_Graphics_SDLWindowBase inherited;

 public:
  RPG_Client_Window_Main_Stub (const RPG_Graphics_Size_t&,                  // size
                               const std::string&,                          // title
                               enum RPG_Graphics_Font = FONT_MAIN_LARGE);   // title font
  inline virtual ~RPG_Client_Window_Main_Stub () {}

  // initialize different hotspots
  // *WARNING*: call this AFTER setScreen() !
  bool initialize (RPG_Client_Engine*, // engine handle
                   bool,               // do automatic edge scroll ?
                   RPG_Engine*,        // (level) state handle
                   bool = false);      // debug ?

  // implement RPG_Client_IWindow
  virtual void drawBorder (SDL_Surface* = NULL, // target surface (default: screen)
                           unsigned int = 0,    // offset x (top-left = [0,0])
                           unsigned int = 0);   // offset y (top-left = [0,0])

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw (SDL_Surface* = NULL, // target surface (default: screen)
                     unsigned int = 0,    // offset x (top-left = [0,0])
                     unsigned int = 0);   // offset y (top-left = [0,0])
  virtual void handleEvent (const SDL_Event&,          // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            struct SDL_Rect&);         // return value: "dirty" region
  virtual void notify (enum RPG_Graphics_Cursor) const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_Main_Stub ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_Main_Stub (const RPG_Client_Window_Main_Stub&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_Main_Stub& operator= (const RPG_Client_Window_Main_Stub&))

  // helper methods
  void initScrollSpots (bool = false); // debug ?
  bool initMap (RPG_Client_Engine*, // engine handle
                RPG_Engine*,        // (level) state handle
                bool = false);      // debug ?

  RPG_Client_Engine*     myEngine;
  unsigned int           myLastHoverTime;
  bool                   myAutoEdgeScroll;
  enum RPG_Graphics_Font myTitleFont;
};

#endif
