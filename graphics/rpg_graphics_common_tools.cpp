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
#include "rpg_graphics_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_SDL_tools.h"

#include <rpg_common_file_tools.h>

#include <ace/Log_Msg.h>

#include <png.h>

#include <sstream>

// init statics
RPG_Graphics_CategoryToStringTable_t RPG_Graphics_CategoryHelper::myRPG_Graphics_CategoryToStringTable;
RPG_Graphics_OrientationToStringTable_t RPG_Graphics_OrientationHelper::myRPG_Graphics_OrientationToStringTable;
RPG_Graphics_TileTypeToStringTable_t RPG_Graphics_TileTypeHelper::myRPG_Graphics_TileTypeToStringTable;
RPG_Graphics_FloorStyleToStringTable_t RPG_Graphics_FloorStyleHelper::myRPG_Graphics_FloorStyleToStringTable;
RPG_Graphics_StairsStyleToStringTable_t RPG_Graphics_StairsStyleHelper::myRPG_Graphics_StairsStyleToStringTable;
RPG_Graphics_WallStyleToStringTable_t RPG_Graphics_WallStyleHelper::myRPG_Graphics_WallStyleToStringTable;
RPG_Graphics_DoorStyleToStringTable_t RPG_Graphics_DoorStyleHelper::myRPG_Graphics_DoorStyleToStringTable;
RPG_Graphics_TypeToStringTable_t RPG_Graphics_TypeHelper::myRPG_Graphics_TypeToStringTable;
RPG_Graphics_InterfaceElementTypeToStringTable_t RPG_Graphics_InterfaceElementTypeHelper::myRPG_Graphics_InterfaceElementTypeToStringTable;
RPG_Graphics_HotspotTypeToStringTable_t RPG_Graphics_HotspotTypeHelper::myRPG_Graphics_HotspotTypeToStringTable;
RPG_Graphics_TileSetTypeToStringTable_t RPG_Graphics_TileSetTypeHelper::myRPG_Graphics_TileSetTypeToStringTable;

std::string                  RPG_Graphics_Common_Tools::myGraphicsDirectory;

ACE_Thread_Mutex             RPG_Graphics_Common_Tools::myCacheLock;
unsigned long                RPG_Graphics_Common_Tools::myOldestCacheEntry = 0;
unsigned long                RPG_Graphics_Common_Tools::myCacheSize = 0;
RPG_Graphics_GraphicsCache_t RPG_Graphics_Common_Tools::myGraphicsCache;

SDL_Surface*                 RPG_Graphics_Common_Tools::myWallBlend = NULL;

RPG_Graphics_FontCache_t     RPG_Graphics_Common_Tools::myFontCache;

bool                         RPG_Graphics_Common_Tools::myInitialized = false;

static void
PNG_read_callback(png_structp png_ptr_in,
                  png_bytep target_inout,
                  png_size_t size_in)
{
  ACE_TRACE(ACE_TEXT("::PNG_read_callback"));

  ACE_OS::memcpy(target_inout,
                 ACE_static_cast(void*, png_ptr_in->io_ptr),
                 size_in);
  png_ptr_in->io_ptr = (ACE_static_cast(unsigned char*, png_ptr_in->io_ptr) + size_in);
}

void
RPG_Graphics_Common_Tools::init(const std::string& directory_in,
                                const unsigned long& cacheSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::init"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(directory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument \"%s\": not a directory, aborting\n"),
               directory_in.c_str()));

    return;
  } // end IF
  myGraphicsDirectory = directory_in;
  myCacheSize = cacheSize_in;

  if (myInitialized)
  {
    // clean up
    fini();

    myInitialized = false;
  } // end IF
  else
  {
    // init colors
    RPG_Graphics_SDL_Tools::initColors();
  } // end ELSE

  // init fonts
  if (!initFonts())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initFonts(), aborting\n")));

    return;
  } // end IF

  myInitialized = initWallBlend();
}

void
RPG_Graphics_Common_Tools::fini()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fini"));

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // clear the graphics cache
    for (RPG_Graphics_GraphicsCacheIterator_t iter = myGraphicsCache.begin();
         iter != myGraphicsCache.end();
         iter++)
    {
      SDL_FreeSurface((*iter).image);
    } // end FOR
    myGraphicsCache.clear();
    myOldestCacheEntry = 0;

    // clear the font cache
    for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin();
         iter != myFontCache.end();
         iter++)
    {
      TTF_CloseFont((*iter).second);
    } // end FOR
    myFontCache.clear();
  } // end lock scope

  // free the wall blend
  if (myWallBlend)
  {
    SDL_FreeSurface(myWallBlend);
    myWallBlend = NULL;
  } // end IF

  myInitialized = false;
}

const std::string
RPG_Graphics_Common_Tools::styleToString(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::styleToString"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
      return RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle);
    case RPG_Graphics_StyleUnion::STAIRSSTYLE:
      return RPG_Graphics_StairsStyleHelper::RPG_Graphics_StairsStyleToString(style_in.stairsstyle);
    case RPG_Graphics_StyleUnion::WALLSTYLE:
      return RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle);
    case RPG_Graphics_StyleUnion::DOORSTYLE:
      return RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle);
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style (was: %d), aborting\n"),
                 style_in.discriminator));

      break;
    }
  } // end SWITCH

  return std::string(ACE_TEXT_ALWAYS_CHAR("INVALID"));
}

const std::string
RPG_Graphics_Common_Tools::tileToString(const RPG_Graphics_Tile& tile_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::tileToString"));

  std::string result;

  result += ACE_TEXT("type: ");
  result += RPG_Graphics_TileTypeHelper::RPG_Graphics_TileTypeToString(tile_in.type);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("style: ");
  if (tile_in.style.discriminator == RPG_Graphics_StyleUnion::INVALID)
    result += ACE_TEXT("N/A");
  else
    result += RPG_Graphics_Common_Tools::styleToString(tile_in.style);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("orientation: ");
  result += ((tile_in.orientation == RPG_GRAPHICS_ORIENTATION_INVALID) ? ACE_TEXT("N/A")
  : RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(tile_in.orientation));
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("file: ");
  result += tile_in.file;
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  std::ostringstream converter;
  result += ACE_TEXT("offset (x,y): ");
  converter << tile_in.offsetX;
  result += converter.str();
  result += ACE_TEXT(",");
  converter.str(ACE_TEXT(""));
  converter.clear();
  converter << tile_in.offsetY;
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("open/broken: ");
  result += (tile_in.open ? ACE_TEXT("true") : ACE_TEXT("false"));
  result += ACE_TEXT("/");
  result += (tile_in.broken ? ACE_TEXT("true") : ACE_TEXT("false"));
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string
RPG_Graphics_Common_Tools::tileSetToString(const RPG_Graphics_TileSet& tileSet_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::tileSetToString"));

  std::string result;

  result += ACE_TEXT("type: ");
  result += RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(tileSet_in.type);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("style: ");
  result += RPG_Graphics_Common_Tools::styleToString(tileSet_in.style);
  if (tileSet_in.type == TILESETTYPE_WALL)
    result += (tileSet_in.half ? ACE_TEXT("[half height]") : ACE_TEXT("[full height]"));
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("tiles[");
  std::ostringstream converter;
  converter << tileSet_in.tiles.size();
  result += converter.str();
  result += ACE_TEXT("]:\n");
  result += ACE_TEXT("------\n");
  for (RPG_Graphics_TileSetConstIterator_t iterator = tileSet_in.tiles.begin();
       iterator != tileSet_in.tiles.end();
       iterator++)
  {
    result += RPG_Graphics_Common_Tools::tileToString(*iterator);
    result += ACE_TEXT("------\n");
  } // end FOR

  return result;
}

const std::string
RPG_Graphics_Common_Tools::elementTypeToString(const RPG_Graphics_ElementTypeUnion& elementType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::elementTypeToString"));

  switch (elementType_in.discriminator)
  {
    case RPG_Graphics_ElementTypeUnion::INTERFACEELEMENTTYPE:
      return RPG_Graphics_InterfaceElementTypeHelper::RPG_Graphics_InterfaceElementTypeToString(elementType_in.interfaceelementtype);
    case RPG_Graphics_ElementTypeUnion::HOTSPOTTYPE:
      return RPG_Graphics_HotspotTypeHelper::RPG_Graphics_HotspotTypeToString(elementType_in.hotspottype);
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid element type (was: %d), aborting\n"),
                 elementType_in.discriminator));

      break;
    }
  } // end SWITCH

  return std::string(ACE_TEXT_ALWAYS_CHAR("INVALID"));
}

const std::string
RPG_Graphics_Common_Tools::elementsToString(const RPG_Graphics_Elements_t& elements_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::elementsToString"));

  std::string result;

  std::ostringstream converter;
  unsigned long index = 0;
  for (RPG_Graphics_ElementsConstIterator_t iterator = elements_in.begin();
       iterator != elements_in.end();
       iterator++, index++)
  {
    result += ACE_TEXT_ALWAYS_CHAR("#");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << index;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(" ");
    result += RPG_Graphics_Common_Tools::elementTypeToString((*iterator).type);
    result += ACE_TEXT_ALWAYS_CHAR(" {x: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).offsetX;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(", y: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).offsetY;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(", w: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).width;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(", h: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).height;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("}\n");
  } // end FOR

  return result;
}

const RPG_Graphics_Type
RPG_Graphics_Common_Tools::styleToType(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::styleToType"));

  RPG_Graphics_Type result = RPG_GRAPHICS_TYPE_INVALID;

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    {
      switch (style_in.floorstyle)
      {
        case FLOORSTYLE_AIR:
          result = TYPE_TILESET_FLOOR_AIR; break;
        case FLOORSTYLE_DARK:
          result = TYPE_TILESET_FLOOR_DARK; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid floor-style (was: %d), aborting\n"),
                     RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle).c_str()));

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      switch (style_in.wallstyle)
      {
        case WALLSTYLE_BRICK:
          result = TYPE_TILESET_WALL_BRICK_FULL; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid wall-style (was: %d), aborting\n"),
                     RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle).c_str()));

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Graphics_StyleUnion::DOORSTYLE:
    {
      switch (style_in.doorstyle)
      {
        case DOORSTYLE_WOOD:
          result = TYPE_TILESET_DOOR_WOOD; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid door-style (was: %d), aborting\n"),
                     RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle).c_str()));

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style (was: %d), aborting\n"),
                 style_in.discriminator));

      break;
    }
  } // end SWITCH

  return result;
}

const RPG_Graphics_TextSize_t
RPG_Graphics_Common_Tools::textSize(const RPG_Graphics_Type& type_in,
                                    const std::string& textString_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::textSize"));

  int width = 0;
  int height = 0;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find(type_in);
    ACE_ASSERT(iterator != myFontCache.end());

    if (TTF_SizeText((*iterator).second,
                     textString_in.c_str(),
                     &width,
                     &height))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to TTF_SizeText(\"%s\", \"%s\"): %s, aborting\n"),
                 RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str(),
                 textString_in.c_str(),
                 SDL_GetError()));

      return RPG_Graphics_TextSize_t(0, 0);
    } // end IF
  } // end lock scope

  return RPG_Graphics_TextSize_t(ACE_static_cast(unsigned long, width),
                                 ACE_static_cast(unsigned long, height));
}

void
RPG_Graphics_Common_Tools::loadFloorTileSet(const RPG_Graphics_FloorStyle& style_in,
                                            RPG_Graphics_FloorTileSet_t& tileSet_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadFloorTileSet"));

  // init return value(s)
  for (RPG_Graphics_FloorTileSetConstIterator_t iterator = tileSet_out.begin();
       iterator != tileSet_out.end();
       iterator++)
    SDL_FreeSurface(*iterator);
  tileSet_out.clear();

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = style_in;
  RPG_Graphics_Type graphic_type = RPG_GRAPHICS_TYPE_INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style);
  // sanity check(s)
  if (graphic_type == RPG_GRAPHICS_TYPE_INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve list of tiles
  RPG_Graphics_t graphic;
  graphic.type = RPG_GRAPHICS_TYPE_INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(graphic_type);
  ACE_ASSERT(graphic.type == graphic_type);
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadTileSet(\"%s\"): not a tileset (was: %s), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  path_base += RPG_GRAPHICS_TILES_DEF_FLOORS_SUB;
  path_base += ACE_DIRECTORY_SEPARATOR_STR;

  std::string path = path_base;
  SDL_Surface* current_surface = NULL;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_surface = RPG_Graphics_Common_Tools::loadFile(path,             // file
                                                          SDL_ALPHA_OPAQUE, // opaque
                                                          true);            // convert to display format
    if (!current_surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      for (RPG_Graphics_FloorTileSetConstIterator_t iterator = tileSet_out.begin();
           iterator != tileSet_out.end();
           iterator++)
        SDL_FreeSurface(*iterator);
      tileSet_out.clear();

      return;
    } // end IF

    tileSet_out.push_back(current_surface);
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
             RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str(),
             tileSet_out.size()));
}

void
RPG_Graphics_Common_Tools::loadWallTileSet(const RPG_Graphics_WallStyle& style_in,
                                           RPG_Graphics_WallTileSet_t& tileSet_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadWallTileSet"));

  // init return value(s)
  if (tileSet_out.west)
    SDL_FreeSurface(tileSet_out.west);
  if (tileSet_out.east)
    SDL_FreeSurface(tileSet_out.east);
  if (tileSet_out.north)
    SDL_FreeSurface(tileSet_out.north);
  if (tileSet_out.south)
    SDL_FreeSurface(tileSet_out.south);
  tileSet_out.west = NULL;
  tileSet_out.east = NULL;
  tileSet_out.north = NULL;
  tileSet_out.south = NULL;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = style_in;
  RPG_Graphics_Type graphic_type = RPG_GRAPHICS_TYPE_INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style);
  // sanity check(s)
  if (graphic_type == RPG_GRAPHICS_TYPE_INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve list of tiles
  RPG_Graphics_t graphic;
  graphic.type = RPG_GRAPHICS_TYPE_INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(graphic_type);
  ACE_ASSERT(graphic.type == graphic_type);
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadTileSet(\"%s\"): not a tileset (was: %s), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  path_base += RPG_GRAPHICS_TILES_DEF_WALLS_SUB;
  path_base += ACE_DIRECTORY_SEPARATOR_STR;

  // debug info
  std::string dump_path_base = RPG_GRAPHICS_DUMP_DIR;
  dump_path_base += ACE_DIRECTORY_SEPARATOR_STR;
  std::string dump_path = dump_path_base;

  std::string path = path_base;
  SDL_Surface* current_surface = NULL;
  Uint8 opacity = SDL_ALPHA_OPAQUE;

  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_surface = NULL;
    opacity = SDL_ALPHA_OPAQUE;

    // set wall opacity
    switch ((*iterator).orientation)
    {
      case ORIENTATION_SOUTH:
      case ORIENTATION_EAST:
        opacity = ACE_static_cast(Uint8, (RPG_GRAPHICS_WALLTILE_SE_OPACITY * SDL_ALPHA_OPAQUE)); break;
      case ORIENTATION_NORTH:
      case ORIENTATION_WEST:
        opacity = ACE_static_cast(Uint8, (RPG_GRAPHICS_WALLTILE_NW_OPACITY * SDL_ALPHA_OPAQUE)); break;
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid orientation \"%s\", aborting\n"),
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString((*iterator).orientation).c_str()));

        // clean up
        if (tileSet_out.west)
          SDL_FreeSurface(tileSet_out.west);
        if (tileSet_out.east)
          SDL_FreeSurface(tileSet_out.east);
        if (tileSet_out.north)
          SDL_FreeSurface(tileSet_out.north);
        if (tileSet_out.south)
          SDL_FreeSurface(tileSet_out.south);
        tileSet_out.west = NULL;
        tileSet_out.east = NULL;
        tileSet_out.north = NULL;
        tileSet_out.south = NULL;

        return;
      }
    } // end SWITCH

    // load file
    path = path_base;
    path += (*iterator).file;
    current_surface = RPG_Graphics_Common_Tools::loadFile(path,    // file
                                                          opacity, // opacity
                                                          true);   // convert to display format
    if (!current_surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      if (tileSet_out.west)
        SDL_FreeSurface(tileSet_out.west);
      if (tileSet_out.east)
        SDL_FreeSurface(tileSet_out.east);
      if (tileSet_out.north)
        SDL_FreeSurface(tileSet_out.north);
      if (tileSet_out.south)
        SDL_FreeSurface(tileSet_out.south);
      tileSet_out.west = NULL;
      tileSet_out.east = NULL;
      tileSet_out.north = NULL;
      tileSet_out.south = NULL;

      return;
    } // end IF

    // debug info
    dump_path = dump_path_base;
    dump_path += (*iterator).file;
    RPG_Graphics_Common_Tools::savePNG(*current_surface, // image
                                       dump_path,        // file
                                       true);            // WITH alpha

    switch ((*iterator).orientation)
    {
      case ORIENTATION_EAST:
      {
//         // blend surface
//         RPG_Graphics_Common_Tools::shade(*myWallBlend,
//                                          current_surface);
        tileSet_out.east = current_surface;

        break;
      }
      case ORIENTATION_WEST:
        tileSet_out.west = current_surface; break;
      case ORIENTATION_NORTH:
        tileSet_out.north = current_surface; break;
      case ORIENTATION_SOUTH:
      {
//         // blend surface
//         RPG_Graphics_Common_Tools::shade(*myWallBlend,
//                                          current_surface);
        tileSet_out.south = current_surface;

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid orientation \"%s\", aborting\n"),
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString((*iterator).orientation).c_str()));

        // clean up
        if (tileSet_out.west)
          SDL_FreeSurface(tileSet_out.west);
        if (tileSet_out.east)
          SDL_FreeSurface(tileSet_out.east);
        if (tileSet_out.north)
          SDL_FreeSurface(tileSet_out.north);
        if (tileSet_out.south)
          SDL_FreeSurface(tileSet_out.south);
        tileSet_out.west = NULL;
        tileSet_out.east = NULL;
        tileSet_out.north = NULL;
        tileSet_out.south = NULL;

        return;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
             RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str(),
             4));
}

void
RPG_Graphics_Common_Tools::loadDoorTileSet(const RPG_Graphics_DoorStyle& style_in,
                                           RPG_Graphics_DoorTileSet_t& tileSet_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadDoorTileSet"));

  // init return value(s)
  if (tileSet_out.horizontal_open)
    SDL_FreeSurface(tileSet_out.horizontal_open);
  if (tileSet_out.vertical_open)
    SDL_FreeSurface(tileSet_out.vertical_open);
  if (tileSet_out.horizontal_closed)
    SDL_FreeSurface(tileSet_out.horizontal_closed);
  if (tileSet_out.vertical_closed)
    SDL_FreeSurface(tileSet_out.vertical_closed);
  if (tileSet_out.broken)
    SDL_FreeSurface(tileSet_out.broken);
  tileSet_out.horizontal_open = NULL;
  tileSet_out.vertical_open = NULL;
  tileSet_out.horizontal_closed = NULL;
  tileSet_out.vertical_closed = NULL;
  tileSet_out.broken = NULL;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = style_in;
  RPG_Graphics_Type graphic_type = RPG_GRAPHICS_TYPE_INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style);
  // sanity check(s)
  if (graphic_type == RPG_GRAPHICS_TYPE_INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve list of tiles
  RPG_Graphics_t graphic;
  graphic.type = RPG_GRAPHICS_TYPE_INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(graphic_type);
  ACE_ASSERT(graphic.type == graphic_type);
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadTileSet(\"%s\"): not a tileset (was: %s), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_STR;
  path_base += RPG_GRAPHICS_TILES_DEF_DOORS_SUB;
  path_base += ACE_DIRECTORY_SEPARATOR_STR;

  std::string path = path_base;
  SDL_Surface* current_surface = NULL;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_surface = RPG_Graphics_Common_Tools::loadFile(path,             // file
                                                          SDL_ALPHA_OPAQUE, // opaque
                                                          true);            // convert to display format
    if (!current_surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      if (tileSet_out.horizontal_open)
        SDL_FreeSurface(tileSet_out.horizontal_open);
      if (tileSet_out.vertical_open)
        SDL_FreeSurface(tileSet_out.vertical_open);
      if (tileSet_out.horizontal_closed)
        SDL_FreeSurface(tileSet_out.horizontal_closed);
      if (tileSet_out.vertical_closed)
        SDL_FreeSurface(tileSet_out.vertical_closed);
      if (tileSet_out.broken)
        SDL_FreeSurface(tileSet_out.broken);
      tileSet_out.horizontal_open = NULL;
      tileSet_out.vertical_open = NULL;
      tileSet_out.horizontal_closed = NULL;
      tileSet_out.vertical_closed = NULL;
      tileSet_out.broken = NULL;

      return;
    } // end IF

    switch ((*iterator).orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        if ((*iterator).open)
          tileSet_out.horizontal_open = current_surface;
        else
          tileSet_out.horizontal_closed = current_surface;

        break;
      }
      case ORIENTATION_VERTICAL:
      {
        if ((*iterator).open)
          tileSet_out.vertical_open = current_surface;
        else
          tileSet_out.vertical_closed = current_surface;

        break;
      }
      case RPG_GRAPHICS_ORIENTATION_INVALID:
      {
        if ((*iterator).broken)
        {
          tileSet_out.broken = current_surface;

          break;
        } // end IF
        // *WARNING*: fall through !
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid orientation \"%s\", aborting\n"),
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString((*iterator).orientation).c_str()));

        // clean up
        if (tileSet_out.horizontal_open)
          SDL_FreeSurface(tileSet_out.horizontal_open);
        if (tileSet_out.vertical_open)
          SDL_FreeSurface(tileSet_out.vertical_open);
        if (tileSet_out.horizontal_closed)
          SDL_FreeSurface(tileSet_out.horizontal_closed);
        if (tileSet_out.vertical_closed)
          SDL_FreeSurface(tileSet_out.vertical_closed);
        if (tileSet_out.broken)
          SDL_FreeSurface(tileSet_out.broken);
        tileSet_out.horizontal_open = NULL;
        tileSet_out.vertical_open = NULL;
        tileSet_out.horizontal_closed = NULL;
        tileSet_out.vertical_closed = NULL;
        tileSet_out.broken = NULL;

        return;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
             RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str(),
             5));
}

SDL_Surface*
RPG_Graphics_Common_Tools::loadGraphic(const RPG_Graphics_Type& type_in,
                                       const bool& cacheGraphic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadGraphic"));

  RPG_Graphics_GraphicsCacheIterator_t iter;
  RPG_Graphics_GraphicsCacheNode_t node;
  node.type = type_in;
  // init return value(s)
  node.image = NULL;

  // step1: graphic already cached ?
  if (cacheGraphic_in)
  {
    // synch access to graphics cache
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

      for (iter = myGraphicsCache.begin();
           iter != myGraphicsCache.end();
           iter++)
        if ((*iter) == node)
          break;

      if (iter != myGraphicsCache.end())
        return (*iter).image;
    } // end lock scope
  } // end IF

  // step2: load image from file
  RPG_Graphics_t graphic;
  graphic.type = RPG_GRAPHICS_TYPE_INVALID;

  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(type_in);
  ACE_ASSERT(graphic.type == type_in);

  // sanity check
  if ((graphic.category != CATEGORY_INTERFACE) &&
      (graphic.category != CATEGORY_IMAGE) &&
      (graphic.category != CATEGORY_TILE))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid graphics category (was: \"%s\"): type (\"%s\") is not an image, aborting\n"),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str(),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str()));

    return NULL;
  } // end IF

  // assemble path
  std::string path = myGraphicsDirectory;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  if (graphic.category == CATEGORY_TILE)
  {
    switch (graphic.tile.type)
    {
      case TILETYPE_FLOOR:
        path += RPG_GRAPHICS_TILES_DEF_FLOORS_SUB; break;
      case TILETYPE_WALL:
        path += RPG_GRAPHICS_TILES_DEF_WALLS_SUB; break;
      case TILETYPE_DOOR:
        path += RPG_GRAPHICS_TILES_DEF_DOORS_SUB; break;
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid tile type (was: \"%s\"), continuing\n"),
                   RPG_Graphics_TileTypeHelper::RPG_Graphics_TileTypeToString(graphic.tile.type).c_str()));

        break;
      }
    } // end SWITCH
    path += ACE_DIRECTORY_SEPARATOR_STR;
    path += graphic.tile.file;
  } // end IF
  else
    path += graphic.file;

  node.image = RPG_Graphics_Common_Tools::loadFile(path,             // file
                                                   SDL_ALPHA_OPAQUE, // opaque
                                                   true);            // convert to display format
  if (!node.image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
               path.c_str()));

    return NULL;
  } // end IF

  if (cacheGraphic_in)
  {
    // synch cache access
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

      if (myGraphicsCache.size() == myCacheSize)
      {
        iter = myGraphicsCache.begin();
        std::advance(iter, myOldestCacheEntry);
        // *TODO*: what if it's still being used ?...
        SDL_FreeSurface((*iter).image);
        myGraphicsCache.erase(iter);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF

      myGraphicsCache.push_back(node);
    } // end lock scope
  } // end IF

  return node.image;
}

SDL_Surface*
RPG_Graphics_Common_Tools::loadFile(const std::string& filename_in,
                                    const Uint8& alpha_in,
                                    const bool& convertToDisplayFormat_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadFile"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // sanity check
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
               filename_in.c_str()));

    return NULL;
  } // end IF

  // load complete file into memory
  // *TODO*: this isn't really necessary...
  unsigned char* srcbuf = NULL;
  if (!RPG_Common_File_Tools::loadFile(filename_in,
                                       srcbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::loadFile(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return NULL;
  } // end IF

  // extract SDL surface from PNG
  if (!loadPNG(srcbuf,
//                alpha_in,
               result))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadPNG(\"%s\"), aborting\n"),
               filename_in.c_str()));

    // clean up
    delete[] srcbuf;

    return NULL;
  } // end IF

  // clean up
  delete[] srcbuf;

  if (convertToDisplayFormat_in)
  {
    SDL_Surface* convert = NULL;

    if (alpha_in != SDL_ALPHA_OPAQUE)
    {
      // *NOTE*: SDL_SetAlpha will not work, as we want transparent pixels to remain that way...
      // --> do it manually
        // lock surface during pixel access
      if (SDL_MUSTLOCK(result))
        if (SDL_LockSurface(result))
        {
          ACE_DEBUG((LM_ERROR,
                    ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                    SDL_GetError()));

          // clean up
          SDL_FreeSurface(result);

          return NULL;
        } // end IF

        Uint32* pixels = ACE_static_cast(Uint32*, result->pixels);
        Uint32 zero_alpha_mask = ~ACE_static_cast(Uint32, (SDL_ALPHA_OPAQUE << result->format->Ashift));
        for (unsigned long j = 0;
             j < ACE_static_cast(unsigned long, result->h);
             j++)
          for (unsigned long i = 0;
               i < ACE_static_cast(unsigned long, result->w);
               i++)
          {
//             ACE_DEBUG((LM_DEBUG,
//                        ACE_TEXT("[%u,%u]; %x (alpha: %u)\n"),
//                        i,
//                        j,
//                        pixels[(result->w * j) + i],
//                        ((pixels[(result->w * j) + i] & result->format->Amask) >> result->format->Ashift)));

            // ignore transparent pixels
            if (((pixels[(result->w * j) + i] & result->format->Amask) >> result->format->Ashift) == SDL_ALPHA_TRANSPARENT)
              continue;

            pixels[(result->w * j) + i] &= zero_alpha_mask;
            pixels[(result->w * j) + i] |= (ACE_static_cast(Uint32, alpha_in) << result->format->Ashift);

//             ACE_DEBUG((LM_DEBUG,
//                        ACE_TEXT("[%u,%u]; %x (alpha: %u) AFTER\n"),
//                        i,
//                        j,
//                        pixels[(result->w * j) + i],
//                        ((pixels[(result->w * j) + i] & result->format->Amask) >> result->format->Ashift)));
          } // end FOR

        if (SDL_MUSTLOCK(result))
          SDL_UnlockSurface(result);

  //     // enable "per-surface" alpha blending
  //     if (SDL_SetAlpha(result,
  //                      (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
  //                      alpha_in))
  //     {
  //       ACE_DEBUG((LM_ERROR,
  //                  ACE_TEXT("failed to SDL_SetAlpha(%u): %s, aborting\n"),
  //                  ACE_static_cast(unsigned long, alpha_in),
  //                  SDL_GetError()));
  //
  //       // clean up
  //       SDL_FreeSurface(result);
  //
  //       return NULL;
  //     } // end IF
    } // end IF

    // convert surface to the pixel format and colors of the
    // video framebuffer
    // --> allows fast(er) blitting
//     convert = SDL_DisplayFormat(result);
    convert = SDL_DisplayFormatAlpha(result);
    if (!convert)
    {
      ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_DisplayFormat(): %s, aborting\n"),
                 ACE_TEXT("failed to SDL_DisplayFormatAlpha(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(result);

      return NULL;
    } // end IF

    // clean up
    SDL_FreeSurface(result);
    result = convert;
  } // end IF

  return result;
}

SDL_Surface*
RPG_Graphics_Common_Tools::renderText(const RPG_Graphics_Type& type_in,
                                      const std::string& textString_in,
                                      const SDL_Color& color_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::renderText"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find(type_in);
    ACE_ASSERT(iterator != myFontCache.end());

    result = TTF_RenderText_Blended((*iterator).second,
                                    textString_in.c_str(),
                                    color_in);
  } // end lock scope
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to TTF_RenderText_Blended(\"%s\"): %s, aborting\n"),
               textString_in.c_str(),
               SDL_GetError()));

    return NULL;
  } // end IF

  return result;
}

SDL_Surface*
RPG_Graphics_Common_Tools::get(const unsigned long& offsetX_in,
                               const unsigned long& offsetY_in,
                               const unsigned long& width_in,
                               const unsigned long& height_in,
                               const SDL_Surface& image_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::get"));

  // sanity check(s)
  ACE_ASSERT(width_in <= ACE_static_cast(unsigned long, image_in.w));
  ACE_ASSERT((offsetX_in + width_in) <= (ACE_static_cast(unsigned long, image_in.w) - 1));
  ACE_ASSERT(height_in <= ACE_static_cast(unsigned long, image_in.h));
  ACE_ASSERT((offsetY_in + height_in) <= (ACE_static_cast(unsigned long, image_in.h) - 1));

  // init return value
  SDL_Surface* result = NULL;
  result = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                 SDL_ASYNCBLIT |
                                 SDL_SRCCOLORKEY |
                                 SDL_SRCALPHA),
                                width_in,
                                height_in,
                                image_in.format->BitsPerPixel,
                                image_in.format->Rmask,
                                image_in.format->Gmask,
                                image_in.format->Bmask,
                                image_in.format->Amask);

  // *NOTE*: blitting does not preserve the alpha channel...

  // lock surface during pixel access
  if (SDL_MUSTLOCK((&image_in)))
    if (SDL_LockSurface(&ACE_const_cast(SDL_Surface&, image_in)))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(result);

      return NULL;
    } // end IF

  for (unsigned long i = 0;
       i < height_in;
       i++)
    ::memcpy((ACE_static_cast(unsigned char*, result->pixels) + (result->pitch * i)),
             (ACE_static_cast(unsigned char*, image_in.pixels) + ((offsetY_in + i) * image_in.pitch) + (offsetX_in * 4)),
             (width_in * image_in.format->BytesPerPixel)); // RGBA --> 4 bytes (?!!!)

  if (SDL_MUSTLOCK((&image_in)))
    SDL_UnlockSurface(&ACE_const_cast(SDL_Surface&, image_in));

//   // bounding box
//   SDL_Rect toRect;
//   toRect.x = topLeftX_in;
//   toRect.y = topLeftY_in;
//   toRect.w = (bottomRightX_in - topLeftX_in) + 1;
//   toRect.h = (bottomRightY_in - topLeftY_in) + 1;
//   if (SDL_BlitSurface(ACE_const_cast(SDL_Surface*, image_in), // source
//                       &toRect,                                // aspect
//                       subImage_out,                           // target
//                       NULL))                                  // aspect (--> everything)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF

  return result;
}

void
RPG_Graphics_Common_Tools::put(const unsigned long& offsetX_in,
                               const unsigned long& offsetY_in,
                               const SDL_Surface& image_in,
                               SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::put"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);

  // compute bounding box
  SDL_Rect toRect;
  toRect.x = offsetX_in;
  toRect.y = offsetY_in;
  toRect.w = image_in.w;
  toRect.h = image_in.h;

  if (SDL_BlitSurface(&ACE_const_cast(SDL_Surface&, image_in), // source
                      NULL,                                    // aspect (--> everything)
                      targetSurface_in,                        // target
                      &toRect))                                // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}

void
RPG_Graphics_Common_Tools::putText(const RPG_Graphics_Type& type_in,
                                   const std::string& textString_in,
                                   const SDL_Color& color_in,
                                   const bool& shade_in,
                                   const SDL_Color& shadeColor_in,
                                   const unsigned long& offsetX_in,
                                   const unsigned long& offsetY_in,
                                   SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::putText"));

  // render text
  SDL_Surface* rendered_text = NULL;
  if (shade_in)
  {
    rendered_text = renderText(type_in,
                               textString_in,
                               shadeColor_in);
    if (!rendered_text)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::renderText(\"%s\", \"%s\"), aborting\n"),
                 RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str(),
                 textString_in.c_str()));

      return;
    } // end IF

    put(offsetX_in + 1,
        offsetY_in + 1,
        *rendered_text,
        targetSurface_in);

    // clean up
    SDL_FreeSurface(rendered_text);
    rendered_text = NULL;
  } // end IF
  rendered_text = renderText(type_in,
                             textString_in,
                             color_in);
  if (!rendered_text)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::renderText(\"%s\", \"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str(),
               textString_in.c_str()));

    return;
  } // end IF

  put(offsetX_in,
      offsetY_in,
      *rendered_text,
      targetSurface_in);

  // clean up
  SDL_FreeSurface(rendered_text);
}

void
RPG_Graphics_Common_Tools::fade(const bool& fadeIn_in,
                                const float& interval_in,
                                const Uint32& color_in,
                                SDL_Surface* screen_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));

  SDL_Surface* target_image = NULL;
  // step1: create a screen-sized surface without an alpha-channel
  // --> i.e. (alpha mask == 0)
  target_image = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                       SDL_ASYNCBLIT |
                                       SDL_SRCCOLORKEY |
                                       SDL_SRCALPHA),
                                      screen_in->w,
                                      screen_in->h,
                                      screen_in->format->BitsPerPixel,
                                      screen_in->format->Rmask,
                                      screen_in->format->Gmask,
                                      screen_in->format->Bmask,
                                      0);
  if (!target_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  if (fadeIn_in)
  {
    // ...copy the pixel data from the framebuffer
    if (SDL_BlitSurface(screen_in,
                        NULL,
                        target_image,
                        NULL))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF

    // set the screen to the background color (black ?)
    if (SDL_FillRect(screen_in, // screen
                     NULL,      // fill screen
                     color_in)) // black ?
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF
    // ...and display that
    if (SDL_Flip(screen_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF
  } // end IF
  else
  {
    // fill the target image with the requested color
    if (SDL_FillRect(target_image, // screen
                     NULL,         // fill screen
                     color_in))    // target color
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF
  } // end ELSE

  // step4: slowly fade in/out
  fade(interval_in,
       target_image,
       screen_in);

  // clean up
  SDL_FreeSurface(target_image);
}

void
RPG_Graphics_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initStringConversionTables"));

  RPG_Graphics_CategoryHelper::init();
  RPG_Graphics_OrientationHelper::init();
  RPG_Graphics_TileTypeHelper::init();
  RPG_Graphics_FloorStyleHelper::init();
  RPG_Graphics_StairsStyleHelper::init();
  RPG_Graphics_WallStyleHelper::init();
  RPG_Graphics_DoorStyleHelper::init();
  RPG_Graphics_TypeHelper::init();
  RPG_Graphics_InterfaceElementTypeHelper::init();
  RPG_Graphics_HotspotTypeHelper::init();
  RPG_Graphics_TileSetTypeHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_Common_Tools: initialized string conversion tables...\n")));
}

const bool
RPG_Graphics_Common_Tools::initFonts()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initFonts"));

  // step1: retrieve list of configured fonts
  RPG_Graphics_Fonts_t fonts = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getFonts();

  // step2: load all fonts into the cache
  std::string path = myGraphicsDirectory;
  TTF_Font* font = NULL;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    for (RPG_Graphics_FontsConstIterator_t iterator = fonts.begin();
         iterator != fonts.end();
         iterator++)
    {
      font = NULL;

      // construct FQ filename
      path = myGraphicsDirectory;
      path += ACE_DIRECTORY_SEPARATOR_STR;
      path += (*iterator).file;
      // sanity check
      if (!RPG_Common_File_Tools::isReadable(path))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid file(\"%s\"): not readable, aborting\n"),
                   path.c_str()));

        // clean up
        for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin();
             iter != myFontCache.end();
             iter++)
        {
          TTF_CloseFont((*iter).second);
        } // end FOR
        myFontCache.clear();

        return false;
      } // end IF

      font = TTF_OpenFont(path.c_str(),      // filename
                          (*iterator).size); // point size
      if (!font)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to TTF_OpenFont(\"%s\", %u): %s, continuing\n"),
                   path.c_str(),
                   (*iterator).size,
                   SDL_GetError()));

        // clean up
        for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin();
             iter != myFontCache.end();
             iter++)
        {
          TTF_CloseFont((*iter).second);
        } // end FOR
        myFontCache.clear();

        return false;
      } // end IF

      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("loaded font \"%s - %s\" %d face(s) [%s], height: %d (%d,%d), space: %d\n"),
                 TTF_FontFaceFamilyName(font),
                 TTF_FontFaceStyleName(font),
                 TTF_FontFaces(font),
                 (TTF_FontFaceIsFixedWidth(font) ? ACE_TEXT("fixed") : ACE_TEXT("variable")),
                 TTF_FontHeight(font),
                 TTF_FontAscent(font),
                 TTF_FontDescent(font),
                 TTF_FontLineSkip(font)));

      // cache this font
      myFontCache.insert(std::make_pair((*iterator).type, font));
    } // end FOR
  } // end lock scope

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_Common_Tools: loaded %u font(s)...\n"),
             fonts.size()));

  return true;
}

const bool
RPG_Graphics_Common_Tools::initWallBlend()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initWallBlend"));

  // sanity check
  ACE_ASSERT(myWallBlend == NULL);

  myWallBlend = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                      SDL_ASYNCBLIT |
                                      SDL_SRCCOLORKEY |
                                      SDL_SRCALPHA),
                                     RPG_GRAPHICS_WALLTILE_SIZE_X,
                                     RPG_GRAPHICS_WALLTILE_SIZE_Y,
//                                      (bit_depth * 8),
                                     32,
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x000000FF : 0xFF000000),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x0000FF00 : 0x00FF0000),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x00FF0000 : 0x0000FF00),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0xFF000000 : 0x000000FF));
//                                      Rmask,
//                                      Gmask,
//                                      Bmask,
//                                      Amask);
  if (!myWallBlend)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF

  if (SDL_FillRect(myWallBlend,
                   NULL,
                   RPG_Graphics_SDL_Tools::CLR32_BLACK_A30))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(myWallBlend);
    myWallBlend = NULL;

    return false;
  } // end IF

  return true;
}

void
RPG_Graphics_Common_Tools::savePNG(const SDL_Surface& surface_in,
                                   const std::string& targetFile_in,
                                   const bool& alpha_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::savePNG"));

  // sanity check(s)
  if (RPG_Common_File_Tools::isReadable(targetFile_in))
  {
    if (!RPG_Common_File_Tools::deleteFile(targetFile_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                 targetFile_in.c_str()));

      return;
    } // end IF
  } // end IF

  unsigned char* output = NULL;
  try
  {
    output = new unsigned char[(surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                                        : (surface_in.format->BytesPerPixel - 1)))];
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                        : (surface_in.format->BytesPerPixel - 1)))));

    return;
  }
  if (!output)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                        : (surface_in.format->BytesPerPixel - 1)))));

    return;
  } // end IF

  png_bytep* image = NULL;
  try
  {
    image = new png_bytep[surface_in.h];
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * surface_in.h)));

    return;
  }
  if (!image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * surface_in.h)));

    return;
  } // end IF

  // lock surface during pixel access
  if (SDL_MUSTLOCK((&surface_in)))
    if (SDL_LockSurface(&ACE_const_cast(SDL_Surface&, surface_in)))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

  // (if neccessary,) strip out alpha bytes and reorder the image bytes to RGB
  Uint32* pixels = ACE_static_cast(Uint32*, surface_in.pixels);
  for (unsigned long j = 0;
       j < ACE_static_cast(unsigned long, surface_in.h);
       j++)
  {
    image[j] = output;

    for (unsigned long i = 0;
         i < ACE_static_cast(unsigned long, surface_in.w);
         i++)
    {
      *output++ = (((*pixels) & surface_in.format->Rmask) >> surface_in.format->Rshift);   /* red   */
      *output++ = (((*pixels) & surface_in.format->Gmask) >> surface_in.format->Gshift);   /* green */
      *output++ = (((*pixels) & surface_in.format->Bmask) >> surface_in.format->Bshift);   /* blue  */
      if (alpha_in)
        *output++ = (((*pixels) & surface_in.format->Amask) >> surface_in.format->Ashift); /* alpha */

      pixels++;
    } // end FOR
  } // end FOR

  if (SDL_MUSTLOCK((&surface_in)))
    SDL_UnlockSurface(&ACE_const_cast(SDL_Surface&, surface_in));

  // open the file
  FILE* fp = NULL;
  fp = ACE_OS::fopen(targetFile_in.c_str(),
                     ACE_TEXT_ALWAYS_CHAR("wb"));
  if (!fp)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to open file(\"%s\"): %m, aborting\n"),
               targetFile_in.c_str()));

    // clean up
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // create png image data structures
  png_structp png_ptr = NULL;
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, // version
                                    NULL,                  // error
                                    NULL,                  // error handler
                                    NULL);                 // warning handler
  if (!png_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_write_struct(): %m, aborting\n")));

    // clean up
    if (ACE_OS::fclose(fp))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
                 targetFile_in.c_str()));
    } // end IF
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  png_infop info_ptr = NULL;
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_info_struct(): %m, aborting\n")));

    // clean up
    png_destroy_write_struct(&png_ptr,
                             NULL);
    if (ACE_OS::fclose(fp))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
                 targetFile_in.c_str()));
    } // end IF
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // save stack context, set up error handling
  if (::setjmp(png_ptr->jmpbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::setjmp(): %m, aborting\n")));

    // clean up
    png_destroy_write_struct(&png_ptr,
                             &info_ptr);
    if (ACE_OS::fclose(fp))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
                 targetFile_in.c_str()));
    } // end IF
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // write the image
  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr,                         // context
               info_ptr,                        // header info
               surface_in.w,                    // width
               surface_in.h,                    // height
               8,                               // bit-depth
               (alpha_in ? PNG_COLOR_TYPE_RGBA  // color-type
                         : PNG_COLOR_TYPE_RGB),
               PNG_INTERLACE_NONE,              // interlace
               PNG_COMPRESSION_TYPE_DEFAULT,    // compression
               PNG_FILTER_TYPE_DEFAULT);        // filter

  png_set_rows(png_ptr,
               info_ptr,
               image);

  png_write_png(png_ptr,  // context
                info_ptr, // header info
                0,        // transforms
                NULL);    // params

  // clean up
  png_destroy_write_struct(&png_ptr,
                           &info_ptr);
  if (ACE_OS::fclose(fp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
               targetFile_in.c_str()));
  } // end IF
  // *NOTE*: output has been modified, but the value is still present in image[0]
  delete[] image[0];
  delete[] image;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("wrote PNG file \"%s\"...\n"),
             targetFile_in.c_str()));
}

const bool
RPG_Graphics_Common_Tools::loadPNG(const unsigned char* buffer_in,
//                                    const unsigned char& alpha_in,
                                   SDL_Surface*& surface_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadPNG"));

  // init return value(s)
  surface_out = NULL;

  // sanity check
  //--> buffer must contain a PNG file
  if (png_sig_cmp(ACE_const_cast(unsigned char*, buffer_in), // buffer
                  0,                                         // start at the beginning
                  RPG_GRAPHICS_PNG_SIGNATURE_BYTES))         // #signature bytes to check
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_sig_cmp(): %m, aborting\n")));

    return false;
  } // end IF

  png_structp png_ptr = NULL;
  // create the PNG loading context structure
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, // version
                                   NULL,                  // error
                                   NULL,                  // error handler
                                   NULL);                 // warning handler
  if (!png_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_read_struct(): %m, aborting\n")));

    return false;
  } // end IF

  // *NOTE* need to cleanup "png_ptr" beyond this point

  png_infop info_ptr = NULL;
  // allocate/initialize memory for any image information [REQUIRED]
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_info_struct(): %m, aborting\n")));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            NULL,
                            NULL);

    return false;
  } // end IF

  // *NOTE* need to cleanup "png_ptr" and "info_ptr" beyond this point

  // save stack context, set up error handling
  if (::setjmp(png_ptr->jmpbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::setjmp(): %m, aborting\n")));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);

    return false;
  } // end IF

  // set up data input callback
  // *TODO*: use png_init_io() instead (load directly from the file...)
  png_set_read_fn(png_ptr,
                  ACE_const_cast(unsigned char*, buffer_in),
                  PNG_read_callback);

  // read PNG header info
  png_read_info(png_ptr,
                info_ptr);
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace = 0;
  int compression = 0;
  int filter = 0;
  png_get_IHDR(png_ptr,
               info_ptr,
               &width,
               &height,
               &bit_depth,
               &color_type,
               &interlace,
               &compression,
               &filter);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loading PNG [w,h,d,#c,t,i,c,f]: %u,%u,%d,%u,%d,%d,%d,%d...\n"),
             width,
             height,
             bit_depth,
             ACE_static_cast(unsigned long, png_get_channels(png_ptr, info_ptr)),
             color_type,
             interlace,
             compression,
             filter));

  // reduce 16 bit per channel to 8 bit per channel
  // --> strip the second byte of information from a 16-bit depth file
  png_set_strip_16(png_ptr);

  // expand:
  // - 1,2,4 bit grayscale to 8 bit grayscale
  // - paletted images to RGB
  // - tRNS to true alpha channel
  // --> expand data to 24-bit RGB, or 8-bit grayscale, with alpha if available
  png_set_expand(png_ptr);

  // expand grayscale to full RGB
  // --> expand the grayscale to 24-bit RGB if necessary
  png_set_gray_to_rgb(png_ptr);

//   // *NOTE*: we're using per-surface alpha only
//   // --> strip any alpha channel
//   png_set_strip_alpha(png_ptr);

  // add an (opaque) alpha channel to anything that doesn't have one yet
  // --> add a filler byte to 8-bit Gray or 24-bit RGB images
  png_set_filler(png_ptr,
                 0xFF,
//                  alpha_in,
                 PNG_FILLER_AFTER);
  png_set_add_alpha(png_ptr,
                    0xFF,
//                     alpha_in,
                    PNG_FILLER_AFTER);
  info_ptr->color_type |= PNG_COLOR_MASK_ALPHA;

  // allocate surface
  // *NOTE*:
  // - always TRY to (!) place the surface in VideoRAM; this may not work (check flags after loading !)
  // - conversion between PNG header info <--> SDL argument formats requires some casts
  surface_out = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                      SDL_ASYNCBLIT |
                                      SDL_SRCCOLORKEY |
                                      SDL_SRCALPHA),
                                     ACE_static_cast(int, width),
                                     ACE_static_cast(int, height),
//                                      (bit_depth * 8),
                                     32,
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x000000FF : 0xFF000000),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x0000FF00 : 0x00FF0000),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x00FF0000 : 0x0000FF00),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0xFF000000 : 0x000000FF));
//                                      Rmask,
//                                      Gmask,
//                                      Bmask,
//                                      Amask);
  if (!surface_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);

    return false;
  } // end IF

  // *NOTE* need to cleanup "png_ptr", "info_ptr" and "surface_out" beyond this point

  // create the array of row pointers to image data
  png_bytep* row_pointers = NULL;
  try
  {
    row_pointers = new png_bytep[height];
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * height)));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);
    SDL_FreeSurface(surface_out);
    surface_out = NULL;

    return false;
  }
  if (!row_pointers)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * height)));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);
    SDL_FreeSurface(surface_out);
    surface_out = NULL;

    return false;
  } // end IF

  // *NOTE* need to cleanup beyond this point:
  // - "png_ptr", "info_ptr"
  // - "surface_out"
  // - "row_pointers"

  // lock surface during pixel access
  if (SDL_MUSTLOCK(surface_out))
    if (SDL_LockSurface(surface_out))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      delete[] row_pointers;
      png_destroy_read_struct(&png_ptr,
                              &info_ptr,
                              NULL);
      SDL_FreeSurface(surface_out);
      surface_out = NULL;

      return false;
    } // end IF

  for (unsigned long row = 0;
       row < height;
       row++)
    row_pointers[row] = ACE_static_cast(png_bytep,
                                        ACE_static_cast(Uint8*,
                                                        surface_out->pixels) + (row * surface_out->pitch));

  // read the image from the memory buffer onto the surface buffer
  // --> read the whole image into memory at once
  png_read_image(png_ptr, row_pointers);

  // unlock surface
  if (SDL_MUSTLOCK(surface_out))
    SDL_UnlockSurface(surface_out);

  // clean up
  delete[] row_pointers;
  png_destroy_read_struct(&png_ptr,
                          &info_ptr,
                          NULL);

  return true;
}

// void
// RPG_Graphics_Common_Tools::fade(const double& interval_in,
//                                 SDL_Surface* targetImage_in,
//                                 SDL_Surface* screen_in)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));
//
//   // *NOTE*: fading works by blitting the target image onto the screen,
//   // slowly decreasing transparency to reach full brightness/darkness
//
//   // initialize/start with maximal transparency
//   int alpha = SDL_ALPHA_TRANSPARENT; // transparent
//
//   Uint32 cur_clock, end_clock, start_clock;
//   float percentage = 1.0;
//   start_clock = cur_clock = SDL_GetTicks();
//   end_clock = start_clock + (interval_in * 1000);
//   while (cur_clock <= end_clock)
//   {
//     // adjust transparency
//     if (SDL_SetAlpha(targetImage_in,
//                      (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
//                      (SDL_ALPHA_TRANSPARENT + alpha)))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//     // *NOTE*: only change the framebuffer !
//     if (SDL_Flip(screen_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//     if (SDL_BlitSurface(targetImage_in,
//                         NULL,
//                         screen_in,
//                         NULL))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//     if (SDL_Flip(screen_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//
//     cur_clock = SDL_GetTicks();
//     percentage = (ACE_static_cast(float, (cur_clock - start_clock)) /
//                   ACE_static_cast(float, (end_clock - start_clock)));
//     alpha = ((SDL_ALPHA_OPAQUE - SDL_ALPHA_TRANSPARENT) * percentage);
//   } // end WHILE
//
//   // ensure that the target image is fully faded in
//   if (SDL_SetAlpha(targetImage_in,
//                    (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
//                    SDL_ALPHA_OPAQUE))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
//   if (SDL_BlitSurface(targetImage_in,
//                       NULL,
//                       screen_in,
//                       NULL))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
//   if (SDL_Flip(screen_in))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
// }

void
RPG_Graphics_Common_Tools::shade(const SDL_Surface& source_in,
                                 SDL_Surface* target_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::shade"));

  if (SDL_BlitSurface(&ACE_const_cast(SDL_Surface&, source_in),
                      NULL,
                      target_out,
                      NULL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, continuing\n"),
               SDL_GetError()));
  } // end IF
}

void
RPG_Graphics_Common_Tools::fade(const float& interval_in,
                                SDL_Surface* targetImage_in,
                                SDL_Surface* screen_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));

  // calculate the number of blends
  int n_steps = RPG_GRAPHICS_FADE_REFRESH_RATE * interval_in;
  if (SDL_SetAlpha(targetImage_in,
                   (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
                   (SDL_ALPHA_OPAQUE / n_steps)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  Uint32 cur_clock, end_clock, start_clock, sleeptime, elapsed;
  start_clock = cur_clock = SDL_GetTicks();
  end_clock = start_clock + (interval_in * 1000);
  while (cur_clock <= end_clock)
  {
    // *NOTE*: blitting the image onto the screen repeatedly
    // will slowly add up to full brightness, while
    // drawing over the screen with semi-transparent
    // darkness repeatedly gives a fade-out effect
    if (SDL_BlitSurface(targetImage_in,
                        NULL,
                        screen_in,
                        NULL))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF
    if (SDL_Flip(screen_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

    // delay a little while, to impress the blended image
    elapsed = SDL_GetTicks() - cur_clock;
    sleeptime = ((1000/RPG_GRAPHICS_FADE_REFRESH_RATE) > elapsed) ? ((1000/RPG_GRAPHICS_FADE_REFRESH_RATE) - elapsed)
                                                                  : 0;
    SDL_Delay(sleeptime);

    cur_clock += (elapsed + sleeptime);
  } // end WHILE

  // ensure that the target image is fully faded in
  if (SDL_SetAlpha(targetImage_in,
                   0, // alpha blending/RLE acceleration
                   0))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  if (SDL_BlitSurface(targetImage_in,
                      NULL,
                      screen_in,
                      NULL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  if (SDL_Flip(screen_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}
