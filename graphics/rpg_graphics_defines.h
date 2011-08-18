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

#ifndef RPG_GRAPHICS_DEFINES_H
#define RPG_GRAPHICS_DEFINES_H

// #signature bytes to check for PNG graphics
#define RPG_GRAPHICS_PNG_SIGNATURE_BYTES               8

// min #screen refreshes / second of fading
#define RPG_GRAPHICS_FADE_REFRESH_RATE                 10

// default font pointsize
#define RPG_GRAPHICS_FONT_DEF_POINTSIZE                10

// default text colors
#define RPG_GRAPHICS_FONT_DEF_COLOR                    RPG_Graphics_SDL_Tools::CLR32_WHITE
#define RPG_GRAPHICS_FONT_DEF_SHADECOLOR               RPG_Graphics_SDL_Tools::CLR32_BLACK

// default (tiles) data directory tree organization
#define RPG_GRAPHICS_TILE_DEF_CREATURES_SUB            ACE_TEXT("creatures")
#define RPG_GRAPHICS_TILE_DEF_CURSORS_SUB              ACE_TEXT("cursors")
#define RPG_GRAPHICS_TILE_DEF_DOORS_SUB                ACE_TEXT("doors")
#define RPG_GRAPHICS_TILE_DEF_FLOORS_SUB               ACE_TEXT("floors")
#define RPG_GRAPHICS_TILE_DEF_FONTS_SUB                ACE_TEXT("fonts")
#define RPG_GRAPHICS_TILE_DEF_IMAGES_SUB               ACE_TEXT("images")
#define RPG_GRAPHICS_TILE_DEF_WALLS_SUB                ACE_TEXT("walls")

// pixel coordinate difference from one map square to the next
// *NOTE*: due to isometry, this is not the same as the width/height of a tile
#define RPG_GRAPHICS_TILE_WIDTH_MOD                    56
#define RPG_GRAPHICS_TILE_HEIGHT_MOD                   22

// used for shading different tiles
#define RPG_GRAPHICS_TILE_FLOOR_WIDTH                  109 // pixels
#define RPG_GRAPHICS_TILE_FLOOR_HEIGHT                 44  // pixels

// *TODO*: some are south/east are 56, north/west 57...
#define RPG_GRAPHICS_TILE_WALL_WIDTH                   57 // pixels
#define RPG_GRAPHICS_TILE_WALL_HEIGHT                  98 // pixels
#define RPG_GRAPHICS_TILE_WALL_HEIGHT_HALF             66 // pixels

// *NOTE*: 0.0: transparent --> 1.0: opaque
#define RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY          0.6
#define RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY          0.8

#define RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY        400 // ms
#define RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN      20 // pixels
// *NOTE*: while hovering, scroll only every x ms (roughly)...
#define RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_SCROLL_DELAY 150 // ms
#define RPG_GRAPHICS_WINDOW_HOTSPOT_COLOR              RPG_Graphics_SDL_Tools::CLR32_YELLOW

// *NOTE*: offsets are measured in view coordinates (i.e. map squares)
#define RPG_GRAPHICS_WINDOW_SCROLL_OFFSET              1

#define RPG_GRAPHICS_DEF_DICTIONARY_FILE               ACE_TEXT_ALWAYS_CHAR("rpg_graphics.xml")
// #define RPG_GRAPHICS_DEF_DUMP_DIR                      ACE_TEXT("/var/tmp")

// DATADIR-specific
#define RPG_GRAPHICS_DEF_DATA_SUB                      ACE_TEXT("graphic")

// buffer for printing keyboard events (debug info)
#define RPG_GRAPHICS_SDL_KEYSYM_BUFFER_SIZE            32

// SDL event types
// *WARNING*: make sure that these are used consistently !
#define RPG_GRAPHICS_SDL_HOVEREVENT                    SDL_USEREVENT + 1
#define RPG_GRAPHICS_SDL_MOUSEMOVEOUT                  SDL_USEREVENT + 2

#define RPG_GRAPHICS_DEF_SPRITE                        SPRITE_HUMAN

#endif
