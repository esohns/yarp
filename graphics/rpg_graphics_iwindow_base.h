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

#include "rpg_graphics_windowtype.h"

#include <SDL.h>

class RPG_Graphics_IWindowBase
{
 public:
  // exposed interface
  virtual void clip(SDL_Surface* = NULL,          // target surface (default: screen)
                    const unsigned int& = 0,      // offset x (top-left = [0,0])
                    const unsigned int& = 0) = 0; // offset y (top-left = [0,0]));
  virtual void unclip(SDL_Surface* = NULL) const = 0; // target surface (default: screen)

  virtual void draw(SDL_Surface* = NULL,          // target surface (default: screen)
                    const unsigned int& = 0,      // offset x (top-left = [0,0])
                    const unsigned int& = 0) = 0; // offset y (top-left = [0,0])

  virtual void invalidate(const SDL_Rect&) = 0; // "dirty" area
  virtual void update(SDL_Surface* = NULL) = 0; // target surface (default: screen)

  virtual void setScreen(SDL_Surface*) = 0; // (default) screen
  virtual SDL_Surface* getScreen() const = 0; // (default) screen

  virtual RPG_Graphics_WindowType getType() const = 0;
  virtual void getArea(SDL_Rect&) const = 0; // return value: window area

  virtual void handleEvent(const SDL_Event&,          // event
                           RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&) = 0;            // return value: "dirty" region
};

#endif
