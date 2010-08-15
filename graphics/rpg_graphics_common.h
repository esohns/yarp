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
#ifndef RPG_GRAPHICS_COMMON_H
#define RPG_GRAPHICS_COMMON_H

#include "rpg_graphics_incl.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <string>
#include <map>
#include <vector>

enum RPG_Graphics_WindowType
{
  WINDOWTYPE_HOTSPOT = 0,
  WINDOWTYPE_MAP,
  WINDOWTYPE_MENU, // sub-window
  WINDOWTYPE_TOPLEVEL,
  //
  RPG_GRAPHICS_WINDOWTYPE_MAX,
  RPG_GRAPHICS_WINDOWTYPE_INVALID
};

typedef RPG_Graphics_Graphic RPG_Graphics_t;
typedef std::map<RPG_Graphics_Type, RPG_Graphics_t> RPG_Graphics_Dictionary_t;
typedef RPG_Graphics_Dictionary_t::const_iterator RPG_Graphics_DictionaryIterator_t;

struct RPG_Graphics_GraphicsCacheNode_t
{
  RPG_Graphics_Type type;
  SDL_Surface* image;

  inline bool operator==(const RPG_Graphics_GraphicsCacheNode_t& rhs_in) const
  { return (this->type == rhs_in.type); }
};
typedef std::vector<RPG_Graphics_GraphicsCacheNode_t> RPG_Graphics_GraphicsCache_t;
typedef RPG_Graphics_GraphicsCache_t::iterator RPG_Graphics_GraphicsCacheIterator_t;

typedef std::map<RPG_Graphics_Type, TTF_Font*> RPG_Graphics_FontCache_t;
typedef RPG_Graphics_FontCache_t::iterator RPG_Graphics_FontCacheIterator_t;

typedef std::map<RPG_Graphics_InterfaceElementType, SDL_Surface*> RPG_Graphics_InterfaceElements_t;
typedef RPG_Graphics_InterfaceElements_t::const_iterator RPG_Graphics_InterfaceElementsConstIterator_t;

typedef std::vector<RPG_Graphics_Element> RPG_Graphics_Elements_t;
typedef RPG_Graphics_Elements_t::const_iterator RPG_Graphics_ElementsConstIterator_t;

struct RPG_Graphics_Font_t
{
  RPG_Graphics_Type type;
  unsigned int size;
  std::string file;
};
typedef std::vector<RPG_Graphics_Font_t> RPG_Graphics_Fonts_t;
typedef RPG_Graphics_Fonts_t::const_iterator RPG_Graphics_FontsConstIterator_t;

typedef std::pair<unsigned long, unsigned long> RPG_Graphics_TextSize_t;

typedef std::pair<unsigned long, unsigned long> RPG_Graphics_Position_t;
typedef RPG_Graphics_Position_t RPG_Graphics_Offset_t;

typedef std::pair<unsigned long, unsigned long> RPG_Graphics_WindowSize_t;

// a set of (floor-, wall-, door-, ...) tiles
struct RPG_Graphics_Tile_t
{
  int offset_x;
  int offset_y;
  SDL_Surface* surface;
};
typedef std::vector<RPG_Graphics_Tile_t> RPG_Graphics_FloorTileSet_t;
typedef RPG_Graphics_FloorTileSet_t::const_iterator RPG_Graphics_FloorTileSetConstIterator_t;
struct RPG_Graphics_WallTileSet_t
{
  RPG_Graphics_Tile_t west;
  RPG_Graphics_Tile_t north;
  RPG_Graphics_Tile_t east;
  RPG_Graphics_Tile_t south;
};
typedef std::map<RPG_Graphics_Position_t, RPG_Graphics_WallTileSet_t> RPG_Graphics_WallTileMap_t;
typedef RPG_Graphics_WallTileMap_t::const_iterator RPG_Graphics_WallTileMapConstIterator_t;
struct RPG_Graphics_DoorTileSet_t
{
  RPG_Graphics_Tile_t horizontal_open;
  RPG_Graphics_Tile_t vertical_open;
  RPG_Graphics_Tile_t horizontal_closed;
  RPG_Graphics_Tile_t vertical_closed;
  RPG_Graphics_Tile_t broken;
};
typedef std::map<RPG_Graphics_Position_t, RPG_Graphics_Tile_t> RPG_Graphics_DoorTileMap_t;
typedef RPG_Graphics_DoorTileMap_t::const_iterator RPG_Graphics_DoorTileMapConstIterator_t;

typedef std::vector<RPG_Graphics_Tile>::const_iterator RPG_Graphics_TileSetConstIterator_t;

struct RPG_Graphics_MapStyle_t
{
  RPG_Graphics_FloorStyle floor_style;
  RPG_Graphics_WallStyle wall_style;
  bool half_height_walls;
  RPG_Graphics_DoorStyle door_style;
};

#endif
