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

#ifndef RPG_CLIENT_DEFINES_H
#define RPG_CLIENT_DEFINES_H

#include "rpg_graphics_colorname.h"

// task-specific
#define RPG_CLIENT_UI_THREAD_GROUP_ID             102
#define RPG_CLIENT_UI_THREAD_NAME                 "RPG UI dispatch"
#define RPG_CLIENT_ENGINE_THREAD_GROUP_ID         104
#define RPG_CLIENT_ENGINE_THREAD_NAME             "RPG graphics engine"
#define RPG_CLIENT_MAX_QUEUE_SLOTS                100

// gnome / gtk
#define RPG_CLIENT_GNOME_APPLICATION_ID           "rpg_client"
#define RPG_CLIENT_GTK_UI_FILE                    "rpg_client.glade"
// gtk widget names
#define RPG_CLIENT_GTK_DIALOG_ENTRY_TITLE         "please insert data..."
#define RPG_CLIENT_GTK_DIALOG_ENTRY_NAME          "entry_dialog"
#define RPG_CLIENT_GTK_DIALOG_ENTRY_ENTRY_NAME    "name_entry"
#define RPG_CLIENT_GTK_DIALOG_MAIN_NAME           "main_dialog"
#define RPG_CLIENT_GTK_DIALOG_ABOUT_NAME          "about_dialog"
#define RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME    "filechooser_dialog"
#define RPG_CLIENT_GTK_DIALOG_CONFIRMATION_NAME   "confirmation_dialog"
#define RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME      "equipment_dialog"
#define RPG_CLIENT_GTK_FRAME_CHARACTER_NAME       "character_frame"
#define RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME    "character_repository_combobox"
#define RPG_CLIENT_GTK_COMBOBOX_MAP_NAME          "map_repository_combobox"
#define RPG_CLIENT_GTK_COMBOBOX_SERVER_NAME       "server_repository_combobox"
#define RPG_CLIENT_GTK_BUTTON_CREATE_NAME         "create"
#define RPG_CLIENT_GTK_BUTTON_DROP_NAME           "drop"
#define RPG_CLIENT_GTK_BUTTON_LOAD_NAME           "load"
#define RPG_CLIENT_GTK_BUTTON_SAVE_NAME           "save"
#define RPG_CLIENT_GTK_BUTTON_REFRESH_NAME        "refresh"
#define RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME     "map_create"
#define RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME       "map_drop"
#define RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME       "map_load"
#define RPG_CLIENT_GTK_BUTTON_MAP_SAVE_NAME       "map_save"
#define RPG_CLIENT_GTK_BUTTON_MAP_REFRESH_NAME    "map_refresh"
#define RPG_CLIENT_GTK_BUTTON_SERVER_REFRESH_NAME "server_refresh"
#define RPG_CLIENT_GTK_BUTTON_QUIT_NAME           "quit"
#define RPG_CLIENT_GTK_BUTTON_ABOUT_NAME          "about"
#define RPG_CLIENT_GTK_BUTTON_JOIN_NAME           "join"
#define RPG_CLIENT_GTK_BUTTON_PART_NAME           "part"
#define RPG_CLIENT_GTK_BUTTON_EQUIP_NAME          "equip"
#define RPG_CLIENT_GTK_BUTTON_REST_NAME           "rest"

// CONFIGDIR-specific
#define RPG_CLIENT_CONFIG_SUB                     "client"

#define RPG_CLIENT_DEF_INI_FILE                   "rpg_client.ini"
#define RPG_CLIENT_CNF_CLIENT_SECTION_HEADER      "client"
#define RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER  "connection"

#define RPG_CLIENT_DEF_GRAPHICS_CACHESIZE         50
#define RPG_CLIENT_DEF_GRAPHICS_WINDOWSTYLE_TYPE  IMAGE_INTERFACE_MAIN
#define RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE  "Yarp"

// default style
#define RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE        FLOORSTYLE_STONE_COBBLED
#define RPG_CLIENT_DEF_GRAPHICS_EDGESTYLE         EDGESTYLE_FLOOR_STONE_COBBLED
#define RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE         WALLSTYLE_BRICK
#define RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF    true
#define RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE         DOORSTYLE_WOOD

#define RPG_CLIENT_DEF_VIDEO_W                    1024
#define RPG_CLIENT_DEF_VIDEO_H                    786
#define RPG_CLIENT_DEF_VIDEO_BPP                  32
#define RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER         false
#define RPG_CLIENT_DEF_VIDEO_OPENGL               false
#define RPG_CLIENT_DEF_VIDEO_FULLSCREEN           false
#define RPG_CLIENT_DEF_VIDEO_INIT                 false
// #define RPG_CLIENT_DEF_VIDEO_FRAMERATE              30

#define RPG_CLIENT_DEF_MAP_MIN_ROOM_SIZE          0 // 0: don't care
#define RPG_CLIENT_DEF_MAP_DOORS                  true
#define RPG_CLIENT_DEF_MAP_CORRIDORS              true
#define RPG_CLIENT_DEF_MAP_MAX_NUM_DOORS_PER_ROOM 3
#define RPG_CLIENT_DEF_MAP_MAXIMIZE_ROOMS         true
#define RPG_CLIENT_DEF_MAP_NUM_AREAS              5
#define RPG_CLIENT_DEF_MAP_SQUARE_ROOMS           true
#define RPG_CLIENT_DEF_MAP_SIZE_X                 80
#define RPG_CLIENT_DEF_MAP_SIZE_Y                 40

#define RPG_CLIENT_SDL_TIMEREVENT                 SDL_USEREVENT
// *WARNING*: SDL_USEREVENT+1/2 are used by RPG_Graphics !!!
//#define RPG_CLIENT_SDL_GTKEVENT                    SDL_USEREVENT+3
// *NOTE*: this defines the input resolution or "interactivity"
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define RPG_CLIENT_SDL_EVENT_TIMEOUT              100 // ms
//// *NOTE*: sensible values are multiples of RPG_CLIENT_SDL_EVENT_TIMEOUT, as
//// that is the resolution factor
//#define RPG_CLIENT_SDL_GTKEVENT_RESOLUTION         100 // ms

//#define RPG_CLIENT_ENGINE_IDLE_DELAY               10 // ms

// message window
#define RPG_CLIENT_DEF_MESSAGE_ISON               true
#define RPG_CLIENT_MESSAGE_FONT                   FONT_MAIN_SMALL
#define RPG_CLIENT_DEF_MESSAGE_LINES              5
#define RPG_CLIENT_MESSAGE_SHADE_LINES            true
#define RPG_CLIENT_MESSAGE_COLOR                  COLOR_WHITE
#define RPG_CLIENT_MESSAGE_SHADECOLOR             COLOR_BLACK
#define RPG_CLIENT_DEF_MESSAGE_CACHE_SIZE         50

// minimap window
// colors
#define RPG_CLIENT_MINIMAPCOLOR_UNMAPPED          COLOR_BLACK
#define RPG_CLIENT_MINIMAPCOLOR_CORRIDOR          COLOR_PURPLE44
#define RPG_CLIENT_MINIMAPCOLOR_DOOR              COLOR_BROWN
#define RPG_CLIENT_MINIMAPCOLOR_FLOOR             COLOR_PURPLE44
#define RPG_CLIENT_MINIMAPCOLOR_MONSTER           COLOR_RED
#define RPG_CLIENT_MINIMAPCOLOR_PLAYER            COLOR_GREEN
#define RPG_CLIENT_MINIMAPCOLOR_PLAYER_ACTIVE     COLOR_WHITE
#define RPG_CLIENT_MINIMAPCOLOR_STAIRS            COLOR_LIGHTPINK
#define RPG_CLIENT_MINIMAPCOLOR_WALL              COLOR_BLACK
// (initial) position (*NOTE*: offset_x from right side !)
#define RPG_CLIENT_DEF_MINIMAP_ISON               false
#define RPG_CLIENT_DEF_MINIMAP_OFFSET_X           10
#define RPG_CLIENT_DEF_MINIMAP_OFFSET_Y           10

// player-specific
#define RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER    false

#define RPG_CLIENT_DEF_SCREENSHOT_PREFIX          "screenshot"
#define RPG_CLIENT_DEF_SCREENSHOT_EXT             ".png"

#define RPG_CLIENT_DEF_WINDOW_EDGE_AUTOSCROLL     false

#define RPG_CLIENT_DEF_DEBUG                      false

#endif
