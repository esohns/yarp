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
#ifndef SDL_GUI_DEFINES_H
#define SDL_GUI_DEFINES_H

#define SDL_GUI_DEF_MODE                       MODE_LEVEL_MAP

#define SDL_GUI_DEF_MAP_MIN_ROOM_SIZE          0 // 0: don't care
#define SDL_GUI_DEF_MAP_DOORS                  true
#define SDL_GUI_DEF_MAP_CORRIDORS              true
#define SDL_GUI_DEF_MAP_MAX_NUM_DOORS_PER_ROOM 3
#define SDL_GUI_DEF_MAP_MAXIMIZE_ROOMS         true
#define SDL_GUI_DEF_MAP_NUM_AREAS              2
#define SDL_GUI_DEF_MAP_SQUARE_ROOMS           true
#define SDL_GUI_DEF_MAP_SIZE_X                 20
#define SDL_GUI_DEF_MAP_SIZE_Y                 15

#define SDL_GUI_DEF_GRAPHICS_CACHESIZE         50
#define SDL_GUI_DEF_GRAPHICS_WINDOWSTYLE_TYPE  IMAGE_INTERFACE
#define SDL_GUI_DEF_GRAPHICS_MAINWINDOW_TITLE  ACE_TEXT_ALWAYS_CHAR("Project RPG")
// #define SDL_GUI_DEF_GRAPHICS_MAINMENU_TITLE    ACE_TEXT_ALWAYS_CHAR("main menu")
// default style
#define SDL_GUI_DEF_GRAPHICS_FLOORSTYLE        FLOORSTYLE_STONE_COBBLED
#define SDL_GUI_DEF_GRAPHICS_EDGESTYLE         EDGESTYLE_FLOOR_STONE_COBBLED
#define SDL_GUI_DEF_GRAPHICS_WALLSTYLE         WALLSTYLE_BRICK
#define SDL_GUI_DEF_GRAPHICS_WALLSTYLE_HALF    true
#define SDL_GUI_DEF_GRAPHICS_DOORSTYLE         DOORSTYLE_WOOD

#define SDL_GUI_DEF_VIDEO_W                    1024
#define SDL_GUI_DEF_VIDEO_H                    786
#define SDL_GUI_DEF_VIDEO_BPP                  32
#define SDL_GUI_DEF_VIDEO_FULLSCREEN           false
#define SDL_GUI_DEF_VIDEO_DOUBLEBUFFER         true

#define SDL_GUI_SDL_TIMEREVENT                 SDL_USEREVENT
// *NOTE*: this defines the input resolution or "interactivity"
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define SDL_GUI_SDL_EVENT_TIMEOUT              50 // ms

#endif
