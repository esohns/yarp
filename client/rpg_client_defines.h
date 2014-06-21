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

// *NOTE*: need this to import correct PACKAGE_STRING/VERSION/... !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_graphics_colorname.h"

// task-specific
#define RPG_CLIENT_GTK_UI_THREAD_GROUP_ID                 102
#define RPG_CLIENT_GTK_UI_THREAD_NAME                     "RPG UI dispatch"
#define RPG_CLIENT_ENGINE_THREAD_GROUP_ID                 104
#define RPG_CLIENT_ENGINE_THREAD_NAME                     "RPG graphics engine"
#define RPG_CLIENT_ENGINE_MAX_QUEUE_SLOTS                 100

// gnome / gtk
#define RPG_CLIENT_GNOME_APPLICATION_ID                   "rpg_client"
#define RPG_CLIENT_GTK_UI_FILE                            "rpg_client.glade"
// gtk widget names
#define RPG_CLIENT_GTK_BUTTON_ABOUT_NAME                  "about"
#define RPG_CLIENT_GTK_BUTTON_QUIT_NAME                   "quit"
//
#define RPG_CLIENT_GTK_BUTTON_CREATE_NAME                 "create"
#define RPG_CLIENT_GTK_BUTTON_DROP_NAME                   "drop"
#define RPG_CLIENT_GTK_BUTTON_LOAD_NAME                   "load"
#define RPG_CLIENT_GTK_BUTTON_STORE_NAME                  "store"
#define RPG_CLIENT_GTK_BUTTON_REFRESH_NAME                "refresh"
//
#define RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME             "map_create"
#define RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME               "map_drop"
#define RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME               "map_load"
#define RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME              "map_store"
#define RPG_CLIENT_GTK_BUTTON_MAP_REFRESH_NAME            "map_refresh"
//
#define RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME            "server_join"
#define RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME            "server_part"
#define RPG_CLIENT_GTK_BUTTON_SERVER_REFRESH_NAME         "server_refresh"
//
#define RPG_CLIENT_GTK_BUTTON_ENGINESTATE_LOAD_NAME       "engine_state_load"
#define RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME      "engine_state_store"
#define RPG_CLIENT_GTK_BUTTON_ENGINESTATE_REFRESH_NAME    "engine_state_refresh"
//
#define RPG_CLIENT_GTK_BUTTON_EQUIP_NAME                  "equip"
#define RPG_CLIENT_GTK_BUTTON_REST_NAME                   "rest"
//
#define RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME            "character_repository_combobox"
#define RPG_CLIENT_GTK_COMBOBOX_MAP_NAME                  "map_repository_combobox"
#define RPG_CLIENT_GTK_COMBOBOX_ENGINESTATE_NAME          "engine_state_repository_combobox"
#define RPG_CLIENT_GTK_COMBOBOX_SERVER_NAME               "server_repository_combobox"
//
#define RPG_CLIENT_GTK_DIALOG_ABOUT_NAME                  "about_dialog"
#define RPG_CLIENT_GTK_DIALOG_CONFIRMATION_NAME           "confirmation_dialog"
#define RPG_CLIENT_GTK_DIALOG_ENTRY_NAME                  "entry_dialog"
#define RPG_CLIENT_GTK_DIALOG_ENTRY_TITLE                 "please insert data..."
#define RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME              "equipment_dialog"
#define RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME            "filechooser_dialog"
#define RPG_CLIENT_GTK_DIALOG_MAIN_NAME                   "main_dialog"
//
#define RPG_CLIENT_GTK_ENTRY_NAME                         "name_entry"
//
#define RPG_CLIENT_GTK_HBOX_MAIN_NAME                     "main_hbox"
#define RPG_CLIENT_GTK_VBOX_TOOLS_NAME                    "tools_vbox"
#define RPG_CLIENT_GTK_FRAME_CHARACTER_NAME               "character_frame"

// CONFIGDIR-specific
#define RPG_CLIENT_CONFIG_SUB                             "client"

// configuration file
#define RPG_CLIENT_CONFIGURATION_FILE                     "rpg_client.ini"
#define RPG_CLIENT_CONFIGURATION_SECTIONHEADER_CLIENT     "client"
#define RPG_CLIENT_CONFIGURATION_SECTIONHEADER_CONNECTION "connection"
//
#define RPG_CLIENT_CONFIGURATION_VARIABLE_SERVER          "server"
#define RPG_CLIENT_CONFIGURATION_VARIABLE_PORT            "port"
#define RPG_CLIENT_CONFIGURATION_VARIABLE_PASSWORD        "password"
#define RPG_CLIENT_CONFIGURATION_VARIABLE_NICKNAME        "nick"
#define RPG_CLIENT_CONFIGURATION_VARIABLE_USERNAME        "user"
#define RPG_CLIENT_CONFIGURATION_VARIABLE_REALNAME        "realname"
#define RPG_CLIENT_CONFIGURATION_VARIABLE_CHANNEL         "channel"

#define RPG_CLIENT_GRAPHICS_DEF_CACHESIZE                 50
#define RPG_CLIENT_GRAPHICS_DEF_WINDOWSTYLE_TYPE          IMAGE_INTERFACE_MAIN
#define RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE         RPG_PACKAGE_STRING

// default style
#define RPG_CLIENT_GRAPHICS_DEF_FLOORSTYLE                FLOORSTYLE_STONE_COBBLED
#define RPG_CLIENT_GRAPHICS_DEF_EDGESTYLE                 EDGESTYLE_FLOOR_STONE_COBBLED
#define RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE                 WALLSTYLE_BRICK
#define RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE_HALF            true
#define RPG_CLIENT_GRAPHICS_DEF_DOORSTYLE                 DOORSTYLE_WOOD

#define RPG_CLIENT_VIDEO_DEF_WIDTH                        800
#define RPG_CLIENT_VIDEO_DEF_HEIGHT                       600
#define RPG_CLIENT_VIDEO_DEF_BPP                          32
#define RPG_CLIENT_VIDEO_DEF_DOUBLEBUFFER                 false
#define RPG_CLIENT_VIDEO_DEF_OPENGL                       false
#define RPG_CLIENT_VIDEO_DEF_FULLSCREEN                   false
#define RPG_CLIENT_VIDEO_DEF_INIT                         false
//#define RPG_CLIENT_VIDEO_DEF_FRAMERATE                    30

#define RPG_CLIENT_MAP_DEF_MIN_ROOM_SIZE                  0 // 0: don't care
#define RPG_CLIENT_MAP_DEF_DOORS                          true
#define RPG_CLIENT_MAP_DEF_CORRIDORS                      true
#define RPG_CLIENT_MAP_DEF_MAX_NUM_DOORS_PER_ROOM         3
#define RPG_CLIENT_MAP_DEF_MAXIMIZE_ROOMS                 true
#define RPG_CLIENT_MAP_DEF_NUM_AREAS                      5
#define RPG_CLIENT_MAP_DEF_SQUARE_ROOMS                   true
#define RPG_CLIENT_MAP_DEF_SIZE_X                         80
#define RPG_CLIENT_MAP_DEF_SIZE_Y                         40

#define RPG_CLIENT_SDL_TIMEREVENT                         SDL_USEREVENT
// *WARNING*: SDL_USEREVENT+1/2 are used by RPG_Graphics !!!
//#define RPG_CLIENT_SDL_GTKEVENT                    SDL_USEREVENT+3
// *NOTE*: this defines the input resolution or "interactivity"
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define RPG_CLIENT_SDL_EVENT_TIMEOUT                      100 // ms
//// *NOTE*: sensible values are multiples of RPG_CLIENT_SDL_EVENT_TIMEOUT, as
//// that is the resolution factor
//#define RPG_CLIENT_SDL_GTKEVENT_RESOLUTION         100 // ms

//#define RPG_CLIENT_ENGINE_IDLE_DELAY               10 // ms

// message window
#define RPG_CLIENT_MESSAGE_DEF_ISON                       true
#define RPG_CLIENT_MESSAGE_FONT                           FONT_MAIN_SMALL
#define RPG_CLIENT_MESSAGE_DEF_NUM_LINES                  5
#define RPG_CLIENT_MESSAGE_SHADE_LINES                    true
#define RPG_CLIENT_MESSAGE_COLOR                          COLOR_WHITE
#define RPG_CLIENT_MESSAGE_SHADECOLOR                     COLOR_BLACK
#define RPG_CLIENT_MESSAGE_CACHE_SIZE                     50

// minimap window
// colors
#define RPG_CLIENT_MINIMAPCOLOR_UNMAPPED                  COLOR_BLACK
#define RPG_CLIENT_MINIMAPCOLOR_CORRIDOR                  COLOR_PURPLE44
#define RPG_CLIENT_MINIMAPCOLOR_DOOR                      COLOR_BROWN
#define RPG_CLIENT_MINIMAPCOLOR_FLOOR                     COLOR_PURPLE44
#define RPG_CLIENT_MINIMAPCOLOR_MONSTER                   COLOR_RED
#define RPG_CLIENT_MINIMAPCOLOR_PLAYER                    COLOR_GREEN
#define RPG_CLIENT_MINIMAPCOLOR_PLAYER_ACTIVE             COLOR_WHITE
#define RPG_CLIENT_MINIMAPCOLOR_STAIRS                    COLOR_LIGHTPINK
#define RPG_CLIENT_MINIMAPCOLOR_WALL                      COLOR_BLACK
// (initial) position (*NOTE*: offset_x from right side !)
#define RPG_CLIENT_MINIMAP_DEF_ISON                       false
#define RPG_CLIENT_MINIMAP_DEF_OFFSET_X                   10
#define RPG_CLIENT_MINIMAP_DEF_OFFSET_Y                   10

// player-specific
#define RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER            false

#define RPG_CLIENT_SCREENSHOT_DEF_PREFIX                  "screenshot"
#define RPG_CLIENT_SCREENSHOT_DEF_EXT                     ".png"

#define RPG_CLIENT_WINDOW_DEF_EDGE_AUTOSCROLL             true

#define RPG_CLIENT_DEF_DEBUG                              false

#define RPG_CLIENT_STATE_DEF_DESCRIPTOR                   "descriptor"

#endif
