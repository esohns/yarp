/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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

#ifndef RPG_GRAPHICS_IWINDOWBASE_H
#define RPG_GRAPHICS_IWINDOWBASE_H

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "common_ilock.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_windowtype.h"

class RPG_Graphics_IWindowBase
{
 public:
  //inline virtual ~RPG_Graphics_IWindowBase () {}

  virtual void initialize (Common_ILock* = NULL, // screen lock interface handle
                           bool = false) = 0;    // double-buffered screen ?

  virtual void clip (SDL_Surface* = NULL,   // target surface (default: screen)
                     unsigned int = 0,      // offset x (top-left = [0,0])
                     unsigned int = 0) = 0; // offset y (top-left = [0,0]));
  virtual void unclip (SDL_Surface* = NULL) = 0; // target surface (default: screen)

  virtual void draw (SDL_Surface* = NULL,   // target surface (default: screen)
                     unsigned int = 0,      // offset x (top-left = [0,0])
                     unsigned int = 0) = 0; // offset y (top-left = [0,0])

  virtual void invalidate (const SDL_Rect&) = 0; // "dirty" area
  virtual SDL_Rect getDirty () const = 0; // return value: "dirty" area
  virtual void update (SDL_Surface* = NULL) = 0; // target surface (default: screen)

#if defined (SDL_USE)
  virtual void setScreen (SDL_Surface*) = 0; // (default) screen
  virtual SDL_Surface* getScreen () const = 0; // (default) screen
#elif defined (SDL2_USE)
  virtual void setRenderer (SDL_Renderer*) = 0; // (default) renderer
  virtual void setScreen (SDL_Window*) = 0; // (default) screen
  virtual SDL_Renderer* getRenderer () const = 0; // (default) renderer
  virtual SDL_Window* getScreen() const = 0; // (default) screen
#endif // SDL_USE || SDL2_USE
  virtual RPG_Graphics_IWindowBase* getParent () const = 0; // return value: parent window handle (if any)

  virtual void addChild (RPG_Graphics_IWindowBase*) = 0; // window handle
  virtual void removeChild (RPG_Graphics_IWindowBase*) = 0; // window handle
  virtual void refresh (SDL_Surface* = NULL) = 0; // target surface (default: screen)

  virtual RPG_Graphics_WindowType getType () const = 0;
  virtual void getArea (SDL_Rect&,               // return value: window area
                        bool = false) const = 0; // toplevel ?
  virtual void getBorders (unsigned int&,          // return value: size (top)
                           unsigned int&,          // return value: size (bottom)
                           unsigned int&,          // return value: size (left)
                           unsigned int&,          // return value: size (right)
                           bool = true) const = 0; // recursive ?
  virtual RPG_Graphics_IWindowBase* getWindow (const RPG_Graphics_Position_t&) const = 0; // position (e.g. mouse-)

  virtual void handleEvent (const SDL_Event&,          // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            SDL_Rect&) = 0;            // return value: "dirty" region
};

#endif
