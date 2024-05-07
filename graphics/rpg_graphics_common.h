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

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "ace/Log_Msg.h"

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"
#include "SDL_ttf.h"

#include "rpg_graphics_incl.h"

// *NOTE* types as used by SDL
struct RPG_Graphics_SDL_VideoConfiguration
{
  int         screen_width;
  int         screen_height;
  int         screen_colordepth; // bits/pixel
  //Uint32      screen_flags;
  bool        double_buffer;
  bool        use_OpenGL;
  bool        full_screen;
  std::string video_driver; // [wayland|x11|directx|windib|...]
  bool        debug;
};

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
  RPG_Graphics_SpriteDictionary_t  sprites;
  RPG_Graphics_CursorDictionary_t  cursors;
  RPG_Graphics_FontDictionary_t    fonts;
  RPG_Graphics_ImageDictionary_t   images;
  RPG_Graphics_TileDictionary_t    tiles;
  RPG_Graphics_TileSetDictionary_t tilesets;
};

struct RPG_Graphics_GraphicsCacheNode
{
  RPG_Graphics_GraphicTypeUnion type;
//#if defined (SDL_USE)
  SDL_Surface*                  image;
//#elif defined (SDL2_USE)
//  SDL_Texture*                  image;
//#endif // SDL_USE || SDL2_USE

  bool operator== (const struct RPG_Graphics_GraphicsCacheNode& rhs_in) const
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), continuing\n"),
                    type.discriminator));
        break;
      }
    } // end SWITCH

    return false;
  }
  inline bool operator!= (const struct RPG_Graphics_GraphicsCacheNode& rhs_in) const { return !(*this == rhs_in); }
};
typedef std::vector<struct RPG_Graphics_GraphicsCacheNode> RPG_Graphics_GraphicsCache_t;
typedef RPG_Graphics_GraphicsCache_t::iterator RPG_Graphics_GraphicsCacheIterator_t;

typedef std::map<enum RPG_Graphics_Font, TTF_Font*> RPG_Graphics_FontCache_t;
typedef RPG_Graphics_FontCache_t::iterator RPG_Graphics_FontCacheIterator_t;

typedef std::map<enum RPG_Graphics_InterfaceElementType, SDL_Surface*> RPG_Graphics_InterfaceElements_t;
typedef RPG_Graphics_InterfaceElements_t::const_iterator RPG_Graphics_InterfaceElementsConstIterator_t;

typedef std::vector<struct RPG_Graphics_Element> RPG_Graphics_Elements_t;
typedef RPG_Graphics_Elements_t::const_iterator RPG_Graphics_ElementsConstIterator_t;

struct RPG_Graphics_Font_t
{
  enum RPG_Graphics_Font type;
  unsigned int           size;
  std::string            file;
};
typedef std::vector<RPG_Graphics_Font_t> RPG_Graphics_Fonts_t;
typedef RPG_Graphics_Fonts_t::const_iterator RPG_Graphics_FontsConstIterator_t;

typedef std::pair<unsigned int, unsigned int> RPG_Graphics_TextSize_t;

typedef std::pair<unsigned int, unsigned int> RPG_Graphics_Position_t;
typedef std::vector<RPG_Graphics_Position_t> RPG_Graphics_Positions_t;
typedef RPG_Graphics_Positions_t::iterator RPG_Graphics_PositionsIterator_t;
typedef RPG_Graphics_Positions_t::const_iterator RPG_Graphics_PositionsConstIterator_t;

typedef std::pair<int, int> RPG_Graphics_Offset_t;
typedef std::vector<RPG_Graphics_Offset_t> RPG_Graphics_Offsets_t;
typedef RPG_Graphics_Offsets_t::const_iterator RPG_Graphics_OffsetsConstIterator_t;

typedef std::pair<unsigned int, unsigned int> RPG_Graphics_Size_t;

// a set of (floor-, wall-, door-, ...) tiles
struct RPG_Graphics_TileElement
{
  int          offset_x;
  int          offset_y;
  SDL_Surface* surface;
};

typedef std::vector<struct RPG_Graphics_TileElement> RPG_Graphics_FloorTiles_t;
typedef RPG_Graphics_FloorTiles_t::const_iterator RPG_Graphics_FloorTilesConstIterator_t;
struct RPG_Graphics_FloorTileSet
{
  unsigned int              columns;
  unsigned int              rows;
  RPG_Graphics_FloorTiles_t tiles;
};

struct RPG_Graphics_FloorEdgeTileSet
{
  // straight edges
  struct RPG_Graphics_TileElement west;
  struct RPG_Graphics_TileElement north;
  struct RPG_Graphics_TileElement east;
  struct RPG_Graphics_TileElement south;
  // corner edges
  struct RPG_Graphics_TileElement south_west;
  struct RPG_Graphics_TileElement south_east;
  struct RPG_Graphics_TileElement north_west;
  struct RPG_Graphics_TileElement north_east;
  // (square) corner edges
  struct RPG_Graphics_TileElement top;
  struct RPG_Graphics_TileElement right;
  struct RPG_Graphics_TileElement left;
  struct RPG_Graphics_TileElement bottom;
};
// *NOTE*: map coordinates !
typedef std::map<RPG_Graphics_Position_t, struct RPG_Graphics_FloorEdgeTileSet> RPG_Graphics_FloorEdgeTileMap_t;
typedef RPG_Graphics_FloorEdgeTileMap_t::iterator RPG_Graphics_FloorEdgeTileMapIterator_t;

struct RPG_Graphics_WallTileSet
{
  struct RPG_Graphics_TileElement west;
  struct RPG_Graphics_TileElement north;
  struct RPG_Graphics_TileElement east;
  struct RPG_Graphics_TileElement south;
};
// *NOTE*: map coordinates !
typedef std::map<RPG_Graphics_Position_t, struct RPG_Graphics_WallTileSet> RPG_Graphics_WallTileMap_t;
typedef RPG_Graphics_WallTileMap_t::iterator RPG_Graphics_WallTileMapIterator_t;

struct RPG_Graphics_DoorTileSet
{
  struct RPG_Graphics_TileElement horizontal_open;
  struct RPG_Graphics_TileElement vertical_open;
  struct RPG_Graphics_TileElement horizontal_closed;
  struct RPG_Graphics_TileElement vertical_closed;
  struct RPG_Graphics_TileElement broken;
};
// *NOTE*: map coordinates !
typedef std::map<RPG_Graphics_Position_t, struct RPG_Graphics_TileElement> RPG_Graphics_DoorTileMap_t;
typedef RPG_Graphics_DoorTileMap_t::iterator RPG_Graphics_DoorTileMapIterator_t;

typedef std::vector<struct RPG_Graphics_Tile>::const_iterator RPG_Graphics_TileSetConstIterator_t;

// *NOTE*: map coordinates !
typedef std::vector<std::pair<RPG_Graphics_Position_t, SDL_Surface*> > RPG_Graphics_TileCache_t;
typedef RPG_Graphics_TileCache_t::const_iterator RPG_Graphics_TileCacheConstIterator_t;
typedef RPG_Graphics_TileCache_t::iterator RPG_Graphics_TileCacheIterator_t;

typedef std::map<enum RPG_Graphics_ColorName, struct RPG_Graphics_ColorRGBA> RPG_Graphics_ColorMap_t;
typedef RPG_Graphics_ColorMap_t::const_iterator RPG_Graphics_ColorMapConstIterator_t;
typedef RPG_Graphics_ColorMap_t::iterator RPG_Graphics_ColorMapIterator_t;

#endif
