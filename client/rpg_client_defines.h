﻿/***************************************************************************
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

// task-specific
#define RPG_CLIENT_GTK_UI_THREAD_GROUP_ID                 102
#define RPG_CLIENT_GTK_UI_THREAD_NAME                     "UI dispatch"
#define RPG_CLIENT_ENGINE_THREAD_GROUP_ID                 104
#define RPG_CLIENT_ENGINE_THREAD_NAME                     "RPG client"
#define RPG_CLIENT_ENGINE_MAX_QUEUE_SLOTS                 100

// gnome / gtk
#define RPG_CLIENT_GNOME_APPLICATION_ID                   "rpg_client"
#if defined (GTK2_USE)
#define RPG_CLIENT_GTK_UI_FILE                            "rpg_client.gtk2"
#elif defined (GTK3_USE)
#define RPG_CLIENT_GTK_UI_FILE                            "rpg_client.gtk3"
#endif // GTK2_USE || GTK3_USE
#define RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN         "client"

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
#define RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME "togglebutton_join_part"
#define RPG_CLIENT_GTK_BUTTON_SERVER_REFRESH_NAME         "server_refresh"
//
#define RPG_CLIENT_GTK_BUTTON_ENGINESTATE_LOAD_NAME       "engine_state_load"
#define RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME      "engine_state_store"
#define RPG_CLIENT_GTK_BUTTON_ENGINESTATE_REFRESH_NAME    "engine_state_refresh"
//
#define RPG_CLIENT_GTK_BUTTON_EQUIP_NAME                  "equip"
#define RPG_CLIENT_GTK_BUTTON_REST_NAME                   "rest"
#define RPG_CLIENT_GTK_BUTTON_LEVELUP_NAME                "levelup"
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
#define RPG_CLIENT_GTK_DIALOG_LEVELUP_NAME                "levelup_dialog"
#define RPG_CLIENT_GTK_DIALOG_MAIN_NAME                   "main_dialog"
#define RPG_CLIENT_GTK_DIALOG_REST_NAME                   "rest_dialog"
//
#define RPG_CLIENT_GTK_ENTRY_HOURS_NAME                   "hours_entry"
#define RPG_CLIENT_GTK_ENTRY_NAME                         "name_entry"
//
#define RPG_CLIENT_GTK_HBOX_MAIN_NAME                     "main_hbox"
#define RPG_CLIENT_GTK_VBOX_TOOLS_NAME                    "tools_vbox"
//
#define RPG_CLIENT_GTK_FRAME_CHARACTER_NAME               "character_frame"
//
#define RPG_CLIENT_GTK_LABEL_HITPOINTS_NAME               "hitpoints_label2"
#define RPG_CLIENT_GTK_LABEL_HITDICE_NAME                 "hitdice_label"
#define RPG_CLIENT_GTK_LABEL_POINTSREMAINING_NAME         "pointsremaining"
#define RPG_CLIENT_GTK_LABEL_FEATSREMAINING_NAME          "featsremaining"
#define RPG_CLIENT_GTK_LABEL_SKILLSREMAINING_NAME         "skillsremaining"
#define RPG_CLIENT_GTK_LABEL_SPELLSREMAINING_NAME         "spellsremaining"
//
#define RPG_CLIENT_GTK_LISTSTORE_FEATS_NAME               "liststore_feats"
#define RPG_CLIENT_GTK_LISTSTORE_SKILLS_NAME              "liststore_skills"
#define RPG_CLIENT_GTK_LISTSTORE_SPELLS_NAME              "liststore_spells"
//
#define RPG_CLIENT_GTK_SPINBUTTON_STRENGTH_NAME           "spinbutton_strength"
#define RPG_CLIENT_GTK_SPINBUTTON_DEXTERITY_NAME          "spinbutton_dexterity"
#define RPG_CLIENT_GTK_SPINBUTTON_CONSTITUTION_NAME       "spinbutton_constitution"
#define RPG_CLIENT_GTK_SPINBUTTON_INTELLIGENCE_NAME       "spinbutton_intelligence"
#define RPG_CLIENT_GTK_SPINBUTTON_WISDOM_NAME             "spinbutton_wisdom"
#define RPG_CLIENT_GTK_SPINBUTTON_CHARISMA_NAME           "spinbutton_charisma"
//
#define RPG_CLIENT_GTK_TABLE_EQUIPENT_NAME                "equipment_table"
//
#define RPG_CLIENT_GTK_TREEVIEW_FEATS_NAME                "treeview_feats"
#define RPG_CLIENT_GTK_TREEVIEW_SKILLS_NAME               "treeview_skills"
#define RPG_CLIENT_GTK_TREEVIEW_SPELLS_NAME               "treeview_spells"

// CONFIGDIR-specific
#define RPG_CLIENT_CONFIG_SUB                             "client"

#define RPG_CLIENT_SUB_DIRECTORY_STRING                   "client"

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
#define RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE         yarp_PACKAGE_STRING

// default style
#define RPG_CLIENT_GRAPHICS_DEF_FLOORSTYLE                FLOORSTYLE_STONE_COBBLED
#define RPG_CLIENT_GRAPHICS_DEF_EDGESTYLE                 EDGESTYLE_FLOOR_STONE_COBBLED
#define RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE                 WALLSTYLE_BRICK
#define RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE_HALF            true
#define RPG_CLIENT_GRAPHICS_DEF_DOORSTYLE                 DOORSTYLE_WOOD

#define RPG_CLIENT_VIDEO_DEF_WIDTH                        800
#define RPG_CLIENT_VIDEO_DEF_HEIGHT                       600
#define RPG_CLIENT_VIDEO_DEF_BPP                          32
#define RPG_CLIENT_VIDEO_DEF_DOUBLEBUFFER                 true
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_CLIENT_VIDEO_DEF_OPENGL                       false
#else
#define RPG_CLIENT_VIDEO_DEF_OPENGL                       false
#endif // ACE_WIN32 || ACE_WIN64
#define RPG_CLIENT_VIDEO_DEF_FULLSCREEN                   false
//#define RPG_CLIENT_VIDEO_DEF_FRAMERATE                    30

#define RPG_CLIENT_MAP_DEF_MIN_ROOM_SIZE                  0 // 0: don't care
#define RPG_CLIENT_MAP_DEF_DOORS                          true
#define RPG_CLIENT_MAP_DEF_CORRIDORS                      true
#define RPG_CLIENT_MAP_DEF_MAX_NUM_DOORS_PER_ROOM         8
#define RPG_CLIENT_MAP_DEF_MAXIMIZE_ROOMS                 true
#define RPG_CLIENT_MAP_DEF_NUM_AREAS                      5
#define RPG_CLIENT_MAP_DEF_SQUARE_ROOMS                   true
#define RPG_CLIENT_MAP_DEF_SIZE_X                         80
#define RPG_CLIENT_MAP_DEF_SIZE_Y                         40

#if defined (SDL_USE) || defined (SDL2_USE)
#define RPG_CLIENT_SDL_TIMEREVENT                         SDL_USEREVENT
#elif defined (SDL3_USE)
#define RPG_CLIENT_SDL_TIMEREVENT                         SDL_EVENT_USER
#endif // SDL_USE || SDL2_USE || SDL3_USE
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
