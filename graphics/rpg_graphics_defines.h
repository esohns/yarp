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

#include "rpg_graphics_colorname.h"

#ifndef RPG_GRAPHICS_DEFINES_H
#define RPG_GRAPHICS_DEFINES_H

// #signature bytes to check for PNG graphics
#define RPG_GRAPHICS_PNG_SIGNATURE_BYTES                    8

// min #screen refreshes / second of fading
#define RPG_GRAPHICS_FADE_REFRESH_RATE                      10

#define RPG_GRAPHICS_FONT_DEF_POINTSIZE                     10
#define RPG_GRAPHICS_FONT_DEF_COLOR                         COLOR_WHITE
#define RPG_GRAPHICS_FONT_DEF_SHADECOLOR                    COLOR_BLACK

// directory tree organization
#define RPG_GRAPHICS_DATA_SUB                               "graphics"
// (tiles) data directory tree organization
#define RPG_GRAPHICS_TILE_CREATURES_SUB                     "creatures"
#define RPG_GRAPHICS_TILE_CURSORS_SUB                       "cursors"
#define RPG_GRAPHICS_TILE_DOORS_SUB                         "doors"
#define RPG_GRAPHICS_TILE_FLOORS_SUB                        "floors"
#define RPG_GRAPHICS_TILE_FONTS_SUB                         "fonts"
#define RPG_GRAPHICS_TILE_IMAGES_SUB                        "images"
#define RPG_GRAPHICS_TILE_WALLS_SUB                         "walls"

// pixel coordinate difference from one map square to the next
// *NOTE*: used in isometric tile coordinate calculation
#define RPG_GRAPHICS_TILE_WIDTH_MOD                         56
#define RPG_GRAPHICS_TILE_HEIGHT_MOD                        22
// used for shading different tiles
#define RPG_GRAPHICS_TILE_FLOOR_WIDTH                       109 // pixels
#define RPG_GRAPHICS_TILE_FLOOR_HEIGHT                      44  // pixels

// *TODO*: some south/east are 56, north/west 57...
#define RPG_GRAPHICS_TILE_WALL_WIDTH                        57 // pixels
#define RPG_GRAPHICS_TILE_WALL_HEIGHT                       98 // pixels
#define RPG_GRAPHICS_TILE_WALL_HEIGHT_HALF                  66 // pixels

// *NOTE*: 0.0: transparent --> 1.0: opaque (black)
// used for tile shading (to black)
#define RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY               0.6f
#define RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY               0.8f
#define RPG_GRAPHICS_TILE_PREVSEEN_DEF_OPACITY              0.9f

#define RPG_GRAPHICS_WINDOW_HOTSPOT_DEF_COLOR               COLOR_YELLOW
#define RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN           20 // pixels
#define RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY             300 // ms
// *NOTE*: while hovering, scroll only every x ms (roughly)...
#define RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_SCROLL_DELAY      150 // ms

// *NOTE*: offsets are measured in view coordinates (i.e. map squares)
#define RPG_GRAPHICS_WINDOW_SCROLL_OFFSET                   1

#define RPG_GRAPHICS_DEF_CACHESIZE                          50 // #graphics
#define RPG_GRAPHICS_SCHEMA_FILE                            "rpg_graphics.xsd"
#define RPG_GRAPHICS_DICTIONARY_FILE                        "rpg_graphics.xml"
#define RPG_GRAPHICS_DICTIONARY_INSTANCE                    "graphicsDictionary"
#define RPG_GRAPHICS_SUB_DIRECTORY_STRING                   "graphics"

// buffer for printing keyboard events (debug info)
#define RPG_GRAPHICS_SDL_KEYSYM_BUFFER_SIZE                 32

#if defined (SDL_USE)
#define RPG_GRAPHICS_SDL_VIDEO_DRIVER_ENV_VAR               "SDL_VIDEODRIVER"
#elif defined (SDL2_USE) || defined (SDL3_USE)
#define RPG_GRAPHICS_SDL_VIDEO_DRIVER_ENV_VAR               "SDL_VIDEO_DRIVER"
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (SDL_USE)
#define RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME              "windib"
#elif defined (SDL2_USE)
#define RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME              "windows"
#endif // SDL_USE || SDL2_USE
#elif defined (ACE_LINUX)
#if defined (SDL_USE)
#define RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME              "x11"
#elif defined (SDL2_USE) || defined (SDL3_USE)
#define RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME              "x11"
//#define RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME              "wayland"
#endif // SDL_USE || SDL2_USE || SDL3_USE
#else
#define RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME              ""
#endif // ACE_WINXX || ACE_LINUX

#define RPG_GRAPHICS_OPENGL_DEF_HW_ACCELERATION             true
// see also http://wiki.libsdl.org/SDL_GLcontextFlag
#define RPG_GRAPHICS_OPENGL_CONTEXT_FORWARD_COMPATIBLE_FLAG false
#define RPG_GRAPHICS_OPENGL_CONTEXT_ROBUST_ACCESS_FLAG      false
#define RPG_GRAPHICS_OPENGL_CONTEXT_RESET_ISOLATION_FLAG    false

#define RPG_GRAPHICS_OPENGL_DEF_CONTEXT_PROFILE_ES          false

// *NOTE*: requires double-buffered screen ?
#define RPG_GRAPHICS_DEF_FLIP                               false // ? : update dirty region(s)

// SDL event types
// *WARNING*: make sure that these are used consistently !
#if defined (SDL_USE) || defined (SDL2_USE)
#define RPG_GRAPHICS_SDL_HOVEREVENT                         SDL_USEREVENT + 1
#define RPG_GRAPHICS_SDL_MOUSEMOVEOUT                       SDL_USEREVENT + 2
#elif defined (SDL3_USE)
#define RPG_GRAPHICS_SDL_HOVEREVENT                         SDL_EVENT_USER + 1
#define RPG_GRAPHICS_SDL_MOUSEMOVEOUT                       SDL_EVENT_USER + 2
#endif // SDL_USE || SDL2_USE || SDL3_USE

#endif
