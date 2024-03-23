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

#ifndef RPG_GRAPHICS_HOTSPOT_H
#define RPG_GRAPHICS_HOTSPOT_H

#include "ace/Global_Macros.h"

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_SDL_window_base.h"

class RPG_Graphics_HotSpot
 : public RPG_Graphics_SDLWindowBase
{
  typedef RPG_Graphics_SDLWindowBase inherited;

 public:
  RPG_Graphics_HotSpot (const RPG_Graphics_SDLWindowBase&, // parent
                        const RPG_Graphics_Size_t&,        // size
                        // *NOTE*: offset doesn't include any border(s) !
                        const RPG_Graphics_Offset_t&,      // offset
                        enum RPG_Graphics_Cursor,          // (hover) cursor graphic
                        bool = false);                     // debug ?
  inline virtual ~RPG_Graphics_HotSpot () {}

  RPG_Graphics_Cursor getCursorType () const;

  // implement (part of) RPG_Graphics_IWindowBase
  // *IMPORTANT NOTE*: dummy stub --> DO NOT CALL
  virtual RPG_Graphics_Position_t getView () const; // return value: view (map coordinates !)

  virtual void draw (SDL_Surface* = NULL, // target surface (default: screen)
                     unsigned int = 0,    // offset x (top-left = [0,0])
                     unsigned int = 0);   // offset y (top-left = [0,0])
  virtual void handleEvent (const union SDL_Event&,    // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            struct SDL_Rect&);         // return value: "dirty" region

  static bool initialize (const RPG_Graphics_SDLWindowBase&, // parent
                          const RPG_Graphics_Size_t&,        // size
                          // *NOTE*: offset doesn't include any border(s) !
                          const RPG_Graphics_Offset_t&,      // offset
                          enum RPG_Graphics_Cursor,          // (hover) cursor graphic
                          bool = false);                     // debug ?

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_HotSpot ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_HotSpot (const RPG_Graphics_HotSpot&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_HotSpot& operator= (const RPG_Graphics_HotSpot&))

  enum RPG_Graphics_Cursor myCursorType;
  bool                     myCursorHasBeenSet;
  bool                     myDebug;
};

#endif
