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

#ifndef RPG_GRAPHICS_IWINDOW_H
#define RPG_GRAPHICS_IWINDOW_H

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_SDL_tools.h"

#include <SDL/SDL.h>

class RPG_Graphics_IWindow
{
 public:
  // *NOTE*: to shut up the compiler (gcc4) complaining about missing virtual dtors, set
  // -Wno-non-virtual-dtor in the project settings...

  // exposed interface
  virtual void setScreen(SDL_Surface*) = 0; // (default) screen
  virtual SDL_Surface* getScreen() const = 0; // (default) screen

  virtual RPG_Graphics_WindowType getType() const = 0;
  virtual RPG_Graphics_Size_t getSize(const bool& = false) const = 0; // top-level ?
  //virtual void setView(const RPG_Graphics_Position_t&) = 0; // view (map coordinates !)
  //virtual RPG_Graphics_Position_t getView() const = 0; // return value: view (map coordinates !)

  virtual void clear(const Uint32& = RPG_Graphics_SDL_Tools::CLR32_BLACK) = 0;
  virtual void draw(SDL_Surface* = NULL,          // target surface (default: screen)
                    const unsigned int& = 0,      // offset x (top-left = [0,0])
                    const unsigned int& = 0) = 0; // offset y (top-left = [0,0])
  virtual void refresh(SDL_Surface* = NULL) = 0; // target surface (default: screen)

  virtual void clip(SDL_Surface* = NULL,          // target surface (default: screen)
                    const unsigned int& = 0,      // offset x (top-left = [0,0])
                    const unsigned int& = 0) = 0; // offset y (top-left = [0,0]));
  virtual void unclip(SDL_Surface* = NULL) const = 0; // target surface (default: screen)

  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&) = 0;            // return value: redraw ?
  virtual void notify(const RPG_Graphics_Cursor&) const = 0;
};

#endif
