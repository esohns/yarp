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

#define _SDL_main_h
#include "SDL.h"

#include "rpg_graphics_iwindow_base.h"

class RPG_Client_IWindow
 : public virtual RPG_Graphics_IWindowBase
{
 public:
  virtual ~RPG_Client_IWindow () {};

  virtual void drawBorder (SDL_Surface* = NULL,   // target surface (default: screen)
                           unsigned int = 0,      // offset x (top-left = [0,0])
                           unsigned int = 0) = 0; // offset y (top-left = [0,0])
};

#endif
