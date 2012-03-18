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

#include <ace/Log_Msg.h>

#include <string>
#include <map>
#include <vector>

typedef RPG_Graphics_Graphic RPG_Graphics_t;
typedef std::map<RPG_Graphics_Sprite, RPG_Graphics_t> RPG_Graphics_SpriteDictionary_t;
typedef RPG_Graphics_SpriteDictionary_t::const_iterator RPG_Graphics_SpriteDictionaryIterator_t;
typedef std::map<RPG_Graphics_Cursor, RPG_Graphics_t> RPG_Graphics_CursorDictionary_t;
typedef RPG_Graphics_CursorDictionary_t::const_iterator RPG_Graphics_CursorDictionaryIterator_t;
typedef std::map<RPG_Graphics_Font, RPG_Graphics_t> RPG_Graphics_FontDictionary_t;
typedef RPG_Graphics_FontDictionary_t::const_iterator RPG_Graphics_FontDictionaryIterator_t;
typedef std::map<RPG_Graphics_Image, RPG_Graphics_t> RPG_Graphics_ImageDictionary_t;
typedef RPG_Graphics_ImageDictionary_t::const_iterator RPG_Graphics_ImageDictionaryIterator_t;
typedef std::map<RPG_Graphics_TileGraphic, RPG_Graphics_t> RPG_Graphics_TileDictionary_t;
typedef RPG_Graphics_TileDictionary_t::const_iterator RPG_Graphics_TileDictionaryIterator_t;
typedef std::map<RPG_Graphics_TileSetGraphic, RPG_Graphics_t> RPG_Graphics_TileSetDictionary_t;
typedef RPG_Graphics_TileSetDictionary_t::const_iterator RPG_Graphics_TileSetDictionaryIterator_t;
struct RPG_Graphics_Dictionary_t
{
  RPG_Graphics_SpriteDictionary_t sprites;
  RPG_Graphics_CursorDictionary_t cursors;
  RPG_Graphics_FontDictionary_t fonts;
  RPG_Graphics_ImageDictionary_t images;
  RPG_Graphics_TileDictionary_t tiles;
  RPG_Graphics_TileSetDictionary_t tilesets;
};

struct RPG_Graphics_GraphicsCacheNode_t
{
  RPG_Graphics_GraphicTypeUnion type;
  SDL_Surface* image;

  inline bool operator==(const RPG_Graphics_GraphicsCacheNode_t& rhs_in) const
  {
    if (type.discriminator != rhs_in.type.discriminator)
      return false;

    switch (type.discriminator)
    {
      case RPG_Graphics_GraphicTypeUnion::CURSOR:
        return (type.cursor == rhs_in.type.cursor);
      case RPG_Graphics_GraphicTypeUnion::FONT:
          return (type.font == rhs_in.type.font);
      case RPG_Graphics_GraphicTypeUnion::IMAGE:
        return (type.image == rhs_in.type.image);
      case RPG_Graphics_GraphicTypeUnion::SPRITE:
        return (type.sprite == rhs_in.type.sprite);
      case RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC:
        return (type.tilegraphic == rhs_in.type.tilegraphic);
      case RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC:
        return (type.tilesetgraphic == rhs_in.type.tilesetgraphic);
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), continuing\n"),
                   type.discriminator));

        break;
      }
    } // end SWITCH

    return false; }
  inline bool operator!=(const RPG_Graphics_GraphicsCacheNode_t& rhs_in) const
  {
    return !(*this == rhs_in); }
};
typedef std::vector<RPG_Graphics_GraphicsCacheNode_t> RPG_Graphics_GraphicsCache_t;
typedef RPG_Graphics_GraphicsCache_t::iterator RPG_Graphics_GraphicsCacheIterator_t;

typedef std::map<RPG_Graphics_Font, TTF_Font*> RPG_Graphics_FontCache_t;
typedef RPG_Graphics_FontCache_t::iterator RPG_Graphics_FontCacheIterator_t;

typedef std::map<RPG_Graphics_InterfaceElementType, SDL_Surface*> RPG_Graphics_InterfaceElements_t;
typedef RPG_Graphics_InterfaceElements_t::const_iterator RPG_Graphics_InterfaceElementsConstIterator_t;

typedef std::vector<RPG_Graphics_Element> RPG_Graphics_Elements_t;
typedef RPG_Graphics_Elements_t::const_iterator RPG_Graphics_ElementsConstIterator_t;

struct RPG_Graphics_Font_t
{
  RPG_Graphics_Font type;
  unsigned int size;
  std::string file;
};
typedef std::vector<RPG_Graphics_Font_t> RPG_Graphics_Fonts_t;
typedef RPG_Graphics_Fonts_t::const_iterator RPG_Graphics_FontsConstIterator_t;

typedef std::pair<unsigned int, unsigned int> RPG_Graphics_TextSize_t;

typedef std::pair<unsigned int, unsigned int> RPG_Graphics_Position_t;
typedef RPG_Graphics_Position_t RPG_Graphics_Offset_t;

typedef std::pair<unsigned int, unsigned int> RPG_Graphics_Size_t;

// a set of (floor-, wall-, door-, ...) tiles
struct RPG_Graphics_Tile_t
{
  int offset_x;
  int offset_y;
  SDL_Surface* surface;
};

typedef std::vector<RPG_Graphics_Tile_t> RPG_Graphics_FloorTiles_t;
typedef RPG_Graphics_FloorTiles_t::const_iterator RPG_Graphics_FloorTilesConstIterator_t;
struct RPG_Graphics_FloorTileSet_t
{
  unsigned int columns;
  unsigned int rows;
  RPG_Graphics_FloorTiles_t tiles;
};

struct RPG_Graphics_FloorEdgeTileSet_t
{
  // straight edges
  RPG_Graphics_Tile_t west;
  RPG_Graphics_Tile_t north;
  RPG_Graphics_Tile_t east;
  RPG_Graphics_Tile_t south;
  // corner edges
  RPG_Graphics_Tile_t south_west;
  RPG_Graphics_Tile_t south_east;
  RPG_Graphics_Tile_t north_west;
  RPG_Graphics_Tile_t north_east;
  // (square) corner edges
  RPG_Graphics_Tile_t top;
  RPG_Graphics_Tile_t right;
  RPG_Graphics_Tile_t left;
  RPG_Graphics_Tile_t bottom;
};
// *NOTE*: map coordinates !
typedef std::map<RPG_Graphics_Position_t, RPG_Graphics_FloorEdgeTileSet_t> RPG_Graphics_FloorEdgeTileMap_t;
typedef RPG_Graphics_FloorEdgeTileMap_t::iterator RPG_Graphics_FloorEdgeTileMapIterator_t;

struct RPG_Graphics_WallTileSet_t
{
  RPG_Graphics_Tile_t west;
  RPG_Graphics_Tile_t north;
  RPG_Graphics_Tile_t east;
  RPG_Graphics_Tile_t south;
};
// *NOTE*: map coordinates !
typedef std::map<RPG_Graphics_Position_t, RPG_Graphics_WallTileSet_t> RPG_Graphics_WallTileMap_t;
typedef RPG_Graphics_WallTileMap_t::iterator RPG_Graphics_WallTileMapIterator_t;

struct RPG_Graphics_DoorTileSet_t
{
  RPG_Graphics_Tile_t horizontal_open;
  RPG_Graphics_Tile_t vertical_open;
  RPG_Graphics_Tile_t horizontal_closed;
  RPG_Graphics_Tile_t vertical_closed;
  RPG_Graphics_Tile_t broken;
};
// *NOTE*: map coordinates !
typedef std::map<RPG_Graphics_Position_t, RPG_Graphics_Tile_t> RPG_Graphics_DoorTileMap_t;
typedef RPG_Graphics_DoorTileMap_t::iterator RPG_Graphics_DoorTileMapIterator_t;

typedef std::vector<RPG_Graphics_Tile>::const_iterator RPG_Graphics_TileSetConstIterator_t;

struct RPG_Graphics_MapStyle_t
{
  RPG_Graphics_FloorStyle floor_style;
  RPG_Graphics_EdgeStyle edge_style;
  RPG_Graphics_WallStyle wall_style;
  bool half_height_walls;
  RPG_Graphics_DoorStyle door_style;
};

// *NOTE*: map coordinates !
typedef std::vector<std::pair<RPG_Graphics_Position_t, SDL_Surface*> > RPG_Graphics_TileCache_t;
typedef RPG_Graphics_TileCache_t::const_iterator RPG_Graphics_TileCacheConstIterator_t;

#endif
