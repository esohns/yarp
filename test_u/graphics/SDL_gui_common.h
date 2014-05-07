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

#ifndef SDL_GUI_COMMON_H
#define SDL_GUI_COMMON_H

#include <SDL.h>

#include <ace/Synch.h>

struct state_t
{
 SDL_Surface*     screen;
 ACE_Thread_Mutex hover_lock;
 unsigned int     hover_time;
 bool             debug;
};

enum userMode_t
{
  SDL_GUI_USERMODE_SLIDESHOW = 0,
  SDL_GUI_USERMODE_FLOOR_PLAN,
  //
  SDL_GUI_USERMODE_MAX,
  SDL_GUI_USERMODE_INVALID,
};

enum graphicsMode_t
{
  SDL_GUI_GRAPHICSMODE_ISOMETRIC = 0, // isometric perspective (2.5D)
  SDL_GUI_GRAPHICSMODE_3D,            // 3D (OpenGL)
  //
  SDL_GUI_GRAPHICSMODE_MAX,
  SDL_GUI_GRAPHICSMODE_INAVLID
};

#endif
