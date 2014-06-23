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

#include "rpg_client_common.h"

#include "rpg_graphics_floorstyle.h"
#include "rpg_graphics_edgestyle.h"
#include "rpg_graphics_wallstyle.h"
#include "rpg_graphics_doorstyle.h"
#include "rpg_graphics_style.h"

#include "rpg_map_common.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include "ace/Synch.h"

struct state_t
{
  SDL_Surface*               screen;
  ACE_Thread_Mutex           hover_lock;
  unsigned int               hover_time;
  bool                       debug;
  RPG_Graphics_Style         style;
	RPG_Client_SelectionMode   selection_mode;
	RPG_Client_SeenPositions_t seen_positions;
	//
	RPG_Map_Path_t             path;
	RPG_Map_Position_t         source;
	RPG_Map_PositionList_t     positions;
	unsigned char              radius; // map squares
  //
  GLfloat                    angle;
};

enum userMode_t
{
  SDL_GUI_USERMODE_SLIDESHOW = 0,
  SDL_GUI_USERMODE_FLOOR_PLAN,
  //
  SDL_GUI_USERMODE_MAX,
  SDL_GUI_USERMODE_INVALID,
};

#endif
