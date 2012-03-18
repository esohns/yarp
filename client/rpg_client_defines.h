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

#define RPG_CLIENT_DEF_ACE_USES_TP                     false
#define RPG_CLIENT_DEF_ACE_NUM_TP_THREADS              5

#define RPG_CLIENT_DEF_GNOME_APPLICATION_ID            "rpg_client"
#define RPG_CLIENT_DEF_GNOME_UI_FILE                   "rpg_client.glade"
#define RPG_CLIENT_DEF_GNOME_MAINENTRYDIALOG_TITLE     "please insert data..."
#define RPG_CLIENT_DEF_GNOME_MAINENTRYDIALOG_NAME      "main_entry_dialog"
#define RPG_CLIENT_DEF_GNOME_MAINENTRYDIALOGENTRY_NAME "main_entry_dialog_entry"
#define RPG_CLIENT_DEF_GNOME_MAINDIALOG_NAME           "main_dialog"
#define RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME          "about_dialog"
#define RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME    "filechooser_dialog"
#define RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME            "character_frame"
#define RPG_CLIENT_DEF_GNOME_CHARBOX_NAME              "character_repository_combobox"
#define RPG_CLIENT_DEF_GNOME_MAPBOX_NAME               "map_repository_combobox"
#define RPG_CLIENT_DEF_GNOME_SERVERBOX_NAME            "server_repository_combobox"

// CONFIGDIR-specific
#define RPG_CLIENT_DEF_CONFIG_SUB                      "client"

#define RPG_CLIENT_DEF_INI_FILE                        "rpg_client.ini"
#define RPG_CLIENT_CNF_CLIENT_SECTION_HEADER           "client"
#define RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER       "connection"

#define RPG_CLIENT_DEF_GRAPHICS_CACHESIZE              50
#define RPG_CLIENT_DEF_GRAPHICS_WINDOWSTYLE_TYPE       IMAGE_INTERFACE_MAIN
#define RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE       "Yarp"

// default style
#define RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE             FLOORSTYLE_STONE_COBBLED
#define RPG_CLIENT_DEF_GRAPHICS_EDGESTYLE              EDGESTYLE_FLOOR_STONE_COBBLED
#define RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE              WALLSTYLE_BRICK
#define RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF         true
#define RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE              DOORSTYLE_WOOD

#define RPG_CLIENT_DEF_SOUND_CACHESIZE                 50

#define RPG_CLIENT_DEF_AUDIO_FREQUENCY                 44100
#define RPG_CLIENT_DEF_AUDIO_FORMAT                    AUDIO_S16SYS
#define RPG_CLIENT_DEF_AUDIO_CHANNELS                  2
#define RPG_CLIENT_DEF_AUDIO_SAMPLES                   4096

#define RPG_CLIENT_DEF_VIDEO_W                         1024
#define RPG_CLIENT_DEF_VIDEO_H                         786
#define RPG_CLIENT_DEF_VIDEO_BPP                       32
#define RPG_CLIENT_DEF_VIDEO_FULLSCREEN                false
#define RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER              false
// #define RPG_CLIENT_DEF_VIDEO_FRAMERATE              30

#define RPG_CLIENT_DEF_MAP_MIN_ROOM_SIZE               0 // 0: don't care
#define RPG_CLIENT_DEF_MAP_DOORS                       true
#define RPG_CLIENT_DEF_MAP_CORRIDORS                   true
#define RPG_CLIENT_DEF_MAP_MAX_NUM_DOORS_PER_ROOM      3
#define RPG_CLIENT_DEF_MAP_MAXIMIZE_ROOMS              true
#define RPG_CLIENT_DEF_MAP_NUM_AREAS                   5
#define RPG_CLIENT_DEF_MAP_SQUARE_ROOMS                true
#define RPG_CLIENT_DEF_MAP_SIZE_X                      80
#define RPG_CLIENT_DEF_MAP_SIZE_Y                      40

#define RPG_CLIENT_SDL_TIMEREVENT                      SDL_USEREVENT
// *WARNING*: SDL_USEREVENT+1/2 are used by RPG_Graphics !!!
#define RPG_CLIENT_SDL_GTKEVENT                        SDL_USEREVENT+3
// *NOTE*: this defines the input resolution or "interactivity"
// *WARNING*: apparently, the maximum SDL timer resolution is 10 ms
#define RPG_CLIENT_SDL_EVENT_TIMEOUT                   50 // ms
// *NOTE*: sensible values are multiples of RPG_CLIENT_SDL_EVENT_TIMEOUT, as
// that is the resolution factor
#define RPG_CLIENT_SDL_GTKEVENT_RESOLUTION             200 // ms

#define RPG_CLIENT_ENGINE_IDLE_DELAY                   10 // ms

// minimap colors
#define RPG_CLIENT_DEF_MINIMAPCOLOR_CORRIDOR           RPG_Graphics_SDL_Tools::CLR32_PURPLE44
#define RPG_CLIENT_DEF_MINIMAPCOLOR_DOOR               RPG_Graphics_SDL_Tools::CLR32_BROWN
#define RPG_CLIENT_DEF_MINIMAPCOLOR_FLOOR              RPG_Graphics_SDL_Tools::CLR32_PURPLE44
#define RPG_CLIENT_DEF_MINIMAPCOLOR_MONSTER            RPG_Graphics_SDL_Tools::CLR32_RED
#define RPG_CLIENT_DEF_MINIMAPCOLOR_PLAYER             RPG_Graphics_SDL_Tools::CLR32_WHITE
#define RPG_CLIENT_DEF_MINIMAPCOLOR_STAIRS             RPG_Graphics_SDL_Tools::CLR32_LIGHTPINK
#define RPG_CLIENT_DEF_MINIMAPCOLOR_WALL               RPG_Graphics_SDL_Tools::CLR32_BLACK

// (initial) minimap position (*NOTE*: offset_x from right side !)
#define RPG_CLIENT_DEF_MINIMAP_ISON                    false
#define RPG_CLIENT_DEF_MINIMAP_OFFSET_X                10
#define RPG_CLIENT_DEF_MINIMAP_OFFSET_Y                10

// player-specific
#define RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER         false

#define RPG_CLIENT_DEF_SCREENSHOT_PREFIX               "screenshot"
#define RPG_CLIENT_DEF_SCREENSHOT_EXT                  ".png"

#define RPG_CLIENT_DEF_WINDOW_EDGE_AUTOSCROLL          false

#endif
