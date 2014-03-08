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

#ifndef RPG_CLIENT_IWINDOW_H
#define RPG_CLIENT_IWINDOW_H

#include <SDL.h>

class RPG_Client_IWindow
{
 public:
  // *NOTE*: to shut up the compiler (gcc4) complaining about missing virtual dtors, set
  // -Wno-non-virtual-dtor in the project settings...

  // exposed interface
  virtual void drawBorder(SDL_Surface* = NULL,          // target surface (default: screen)
                          const unsigned int& = 0,      // offset x (top-left = [0,0])
                          const unsigned int& = 0) = 0; // offset y (top-left = [0,0])
  virtual void draw(SDL_Surface* = NULL,          // target surface (default: screen)
                    const unsigned int& = 0,      // offset x (top-left = [0,0])
                    const unsigned int& = 0) = 0; // offset y (top-left = [0,0])
};

#endif
