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
#include "rpg_graphics_surface.h"

#include <rpg_common_macros.h>
#include <rpg_common_file_tools.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <png.h>

#include <sstream>

// init statics
RPG_Graphics_CategoryToStringTable_t RPG_Graphics_CategoryHelper::myRPG_Graphics_CategoryToStringTable;
RPG_Graphics_OrientationToStringTable_t RPG_Graphics_OrientationHelper::myRPG_Graphics_OrientationToStringTable;
RPG_Graphics_TileTypeToStringTable_t RPG_Graphics_TileTypeHelper::myRPG_Graphics_TileTypeToStringTable;
RPG_Graphics_FloorStyleToStringTable_t RPG_Graphics_FloorStyleHelper::myRPG_Graphics_FloorStyleToStringTable;
RPG_Graphics_EdgeStyleToStringTable_t RPG_Graphics_EdgeStyleHelper::myRPG_Graphics_EdgeStyleToStringTable;
RPG_Graphics_StairsStyleToStringTable_t RPG_Graphics_StairsStyleHelper::myRPG_Graphics_StairsStyleToStringTable;
RPG_Graphics_WallStyleToStringTable_t RPG_Graphics_WallStyleHelper::myRPG_Graphics_WallStyleToStringTable;
RPG_Graphics_DoorStyleToStringTable_t RPG_Graphics_DoorStyleHelper::myRPG_Graphics_DoorStyleToStringTable;
RPG_Graphics_CursorToStringTable_t RPG_Graphics_CursorHelper::myRPG_Graphics_CursorToStringTable;
RPG_Graphics_FontToStringTable_t RPG_Graphics_FontHelper::myRPG_Graphics_FontToStringTable;
RPG_Graphics_ImageToStringTable_t RPG_Graphics_ImageHelper::myRPG_Graphics_ImageToStringTable;
RPG_Graphics_SpriteToStringTable_t RPG_Graphics_SpriteHelper::myRPG_Graphics_SpriteToStringTable;
RPG_Graphics_TileGraphicToStringTable_t RPG_Graphics_TileGraphicHelper::myRPG_Graphics_TileGraphicToStringTable;
RPG_Graphics_TileSetGraphicToStringTable_t RPG_Graphics_TileSetGraphicHelper::myRPG_Graphics_TileSetGraphicToStringTable;
RPG_Graphics_WindowTypeToStringTable_t RPG_Graphics_WindowTypeHelper::myRPG_Graphics_WindowTypeToStringTable;
RPG_Graphics_InterfaceElementTypeToStringTable_t RPG_Graphics_InterfaceElementTypeHelper::myRPG_Graphics_InterfaceElementTypeToStringTable;
RPG_Graphics_HotspotTypeToStringTable_t RPG_Graphics_HotspotTypeHelper::myRPG_Graphics_HotspotTypeToStringTable;
RPG_Graphics_TileSetTypeToStringTable_t RPG_Graphics_TileSetTypeHelper::myRPG_Graphics_TileSetTypeToStringTable;

std::string                  RPG_Graphics_Common_Tools::myGraphicsDirectory;

ACE_Thread_Mutex             RPG_Graphics_Common_Tools::myCacheLock;
unsigned long                RPG_Graphics_Common_Tools::myOldestCacheEntry = 0;
unsigned long                RPG_Graphics_Common_Tools::myCacheSize = 0;
RPG_Graphics_GraphicsCache_t RPG_Graphics_Common_Tools::myGraphicsCache;

RPG_Graphics_FontCache_t     RPG_Graphics_Common_Tools::myFontCache;

bool                         RPG_Graphics_Common_Tools::myInitialized = false;

void
RPG_Graphics_Common_Tools::init(const std::string& directory_in,
                                const unsigned long& cacheSize_in,
                                const bool& initSDL_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::init"));

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
    if (initSDL_in)
      RPG_Graphics_SDL_Tools::initColors();
  } // end ELSE

  // init fonts
  if (initSDL_in)
    if (!initFonts())
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to initFonts(), aborting\n")));

      return;
    } // end IF

  myInitialized = true;
}

void
RPG_Graphics_Common_Tools::fini()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fini"));

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

  myInitialized = false;
}

const std::string
RPG_Graphics_Common_Tools::styleToString(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::styleToString"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
      return RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle);
    case RPG_Graphics_StyleUnion::EDGESTYLE:
      return RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString(style_in.edgestyle);
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
RPG_Graphics_Common_Tools::typeToString(const RPG_Graphics_GraphicTypeUnion& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::typeToString"));

  switch (type_in.discriminator)
  {
    case RPG_Graphics_GraphicTypeUnion::CURSOR:
      return RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(type_in.cursor);
    case RPG_Graphics_GraphicTypeUnion::FONT:
      return RPG_Graphics_FontHelper::RPG_Graphics_FontToString(type_in.font);
    case RPG_Graphics_GraphicTypeUnion::IMAGE:
      return RPG_Graphics_ImageHelper::RPG_Graphics_ImageToString(type_in.image);
    case RPG_Graphics_GraphicTypeUnion::SPRITE:
      return RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString(type_in.sprite);
    case RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC:
      return RPG_Graphics_TileGraphicHelper::RPG_Graphics_TileGraphicToString(type_in.tilegraphic);
    case RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC:
      return RPG_Graphics_TileSetGraphicHelper::RPG_Graphics_TileSetGraphicToString(type_in.tilesetgraphic);
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), aborting\n"),
                 type_in.discriminator));

      break;
    }
  } // end SWITCH

  return std::string(ACE_TEXT_ALWAYS_CHAR("INVALID"));
}

const std::string
RPG_Graphics_Common_Tools::tileToString(const RPG_Graphics_Tile& tile_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::tileToString"));

  std::string result;

  result += ACE_TEXT("type: ");
  result += RPG_Graphics_TileTypeHelper::RPG_Graphics_TileTypeToString(tile_in.type);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("reference: ");
  switch (tile_in.reference.discriminator)
  {
    case RPG_Graphics_GraphicTypeUnion::CURSOR:
      result += RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(tile_in.reference.cursor); break;
    case RPG_Graphics_GraphicTypeUnion::FONT:
      result += RPG_Graphics_FontHelper::RPG_Graphics_FontToString(tile_in.reference.font); break;
    case RPG_Graphics_GraphicTypeUnion::IMAGE:
      result += RPG_Graphics_ImageHelper::RPG_Graphics_ImageToString(tile_in.reference.image); break;
    case RPG_Graphics_GraphicTypeUnion::SPRITE:
      result += RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString(tile_in.reference.sprite); break;
    case RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC:
      result += RPG_Graphics_TileGraphicHelper::RPG_Graphics_TileGraphicToString(tile_in.reference.tilegraphic); break;
    case RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC:
      result += RPG_Graphics_TileSetGraphicHelper::RPG_Graphics_TileSetGraphicToString(tile_in.reference.tilesetgraphic); break;
    case RPG_Graphics_GraphicTypeUnion::INVALID:
      result += ACE_TEXT("N/A"); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), continuing\n"),
                 tile_in.reference.discriminator));

      break;
    }
  } // end SWITCH
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
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::tileSetToString"));

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
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::elementTypeToString"));

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
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::elementsToString"));

  std::string result;

  std::ostringstream converter;
  unsigned long index = 1;
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

const std::string
RPG_Graphics_Common_Tools::graphicToString(const RPG_Graphics_t& graphic_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::graphicToString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR("category: ");
  result += RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic_in.category);
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += ACE_TEXT_ALWAYS_CHAR("type: ");
  result += RPG_Graphics_Common_Tools::typeToString(graphic_in.type);
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += ACE_TEXT_ALWAYS_CHAR("tile: ");
  if (graphic_in.tile.type == RPG_GRAPHICS_TILETYPE_INVALID)
    result += ACE_TEXT_ALWAYS_CHAR("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR("\n");
    result += RPG_Graphics_Common_Tools::tileToString(graphic_in.tile);
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR("tileset: ");
  if (graphic_in.tileset.type == RPG_GRAPHICS_TILESETTYPE_INVALID)
    result += ACE_TEXT_ALWAYS_CHAR("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR("\n");
    result += RPG_Graphics_Common_Tools::tileSetToString(graphic_in.tileset);
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR("elements: ");
  if (graphic_in.elements.empty())
    result += ACE_TEXT_ALWAYS_CHAR("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR("\n");
    result += RPG_Graphics_Common_Tools::elementsToString(graphic_in.elements);
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR("file: ");
  if (graphic_in.file.empty())
    result += ACE_TEXT_ALWAYS_CHAR("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR("\"");
    result += graphic_in.file;
    result += ACE_TEXT_ALWAYS_CHAR("\"\n");
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR("size: ");
  if (graphic_in.size == 0)
    result += ACE_TEXT_ALWAYS_CHAR("N/A\n");
  else
  {
    std::ostringstream converter;
    converter << graphic_in.size;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end ELSE

  return result;
}

const RPG_Graphics_GraphicTypeUnion
RPG_Graphics_Common_Tools::styleToType(const RPG_Graphics_StyleUnion& style_in,
                                       const bool& halfHeight_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::styleToType"));

  RPG_Graphics_GraphicTypeUnion result;
  result.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  result.tilesetgraphic = RPG_GRAPHICS_TILESETGRAPHIC_INVALID;

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    {
      switch (style_in.floorstyle)
      {
        case FLOORSTYLE_AIR:
          result.tilesetgraphic = TILESET_FLOOR_AIR; break;
        case FLOORSTYLE_CARPET:
          result.tilesetgraphic = TILESET_FLOOR_CARPET; break;
        case FLOORSTYLE_CERAMIC:
          result.tilesetgraphic = TILESET_FLOOR_CERAMIC; break;
        case FLOORSTYLE_DARK:
          result.tilesetgraphic = TILESET_FLOOR_DARK; break;
        case FLOORSTYLE_ICE:
          result.tilesetgraphic = TILESET_FLOOR_ICE; break;
        case FLOORSTYLE_LAVA:
          result.tilesetgraphic = TILESET_FLOOR_LAVA; break;
        case FLOORSTYLE_MARBLE:
          result.tilesetgraphic = TILESET_FLOOR_MARBLE; break;
        case FLOORSTYLE_MURAL:
          result.tilesetgraphic = TILESET_FLOOR_MURAL; break;
        case FLOORSTYLE_MURAL2:
          result.tilesetgraphic = TILESET_FLOOR_MURAL2; break;
        case FLOORSTYLE_ROUGH:
          result.tilesetgraphic = TILESET_FLOOR_ROUGH; break;
        case FLOORSTYLE_ROUGH_LIT:
          result.tilesetgraphic = TILESET_FLOOR_ROUGH_LIT; break;
        case FLOORSTYLE_STONE_COBBLED:
          result.tilesetgraphic = TILESET_FLOOR_STONE_COBBLED; break;
        case FLOORSTYLE_STONE_MOSS_COVERED:
          result.tilesetgraphic = TILESET_FLOOR_STONE_MOSS_COVERED; break;
        case FLOORSTYLE_WATER:
          result.tilesetgraphic = TILESET_FLOOR_WATER; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid floor-style (was: \"%s\"), aborting\n"),
                     RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle).c_str()));

          return result;
        }
      } // end SWITCH

      break;
    }
    case RPG_Graphics_StyleUnion::EDGESTYLE:
    {
      switch (style_in.edgestyle)
      {
        case EDGESTYLE_FLOOR_STONE_COBBLED:
          result.tilesetgraphic = TILESET_EDGE_FLOOR_STONE_COBBLED; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid edge-style (was: \"%s\"), aborting\n"),
                     RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString(style_in.edgestyle).c_str()));

          return result;
        }
      } // end SWITCH

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      switch (style_in.wallstyle)
      {
        case WALLSTYLE_BRICK:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_BRICK_HALF : TILESET_WALL_BRICK); break;
        case WALLSTYLE_BRICK_BANNER:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_BRICK_BANNER_HALF : TILESET_WALL_BRICK_BANNER); break;
        case WALLSTYLE_BRICK_PAINTING:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_BRICK_PAINTING_HALF : TILESET_WALL_BRICK_PAINTING); break;
        case WALLSTYLE_BRICK_PILLAR:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_BRICK_PILLAR_HALF : TILESET_WALL_BRICK_PILLAR); break;
        case WALLSTYLE_BRICK_POCKET:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_BRICK_POCKET_HALF : TILESET_WALL_BRICK_POCKET); break;
        case WALLSTYLE_DARK:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_DARK_HALF : TILESET_WALL_DARK); break;
        case WALLSTYLE_LIGHT:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_LIGHT_HALF : TILESET_WALL_LIGHT); break;
        case WALLSTYLE_MARBLE:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_MARBLE_HALF : TILESET_WALL_MARBLE); break;
        case WALLSTYLE_ROUGH:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_ROUGH_HALF : TILESET_WALL_ROUGH); break;
        case WALLSTYLE_STUCCO:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_STUCCO_HALF : TILESET_WALL_STUCCO); break;
        case WALLSTYLE_VINE_COVERED:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_VINE_COVERED_HALF : TILESET_WALL_VINE_COVERED); break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid wall-style (was: \"%s\"), aborting\n"),
                     RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle).c_str()));

          return result;
        }
      } // end SWITCH

      break;
    }
    case RPG_Graphics_StyleUnion::DOORSTYLE:
    {
      switch (style_in.doorstyle)
      {
        case DOORSTYLE_WOOD:
          result.tilesetgraphic = TILESET_DOOR_WOOD; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid door-style (was: \"%s\"), aborting\n"),
                     RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle).c_str()));

          return result;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style type (was: %d), aborting\n"),
                 style_in.discriminator));

      return result;
    }
  } // end SWITCH

  result.discriminator = RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC;

  return result;
}

void
RPG_Graphics_Common_Tools::graphicToFile(const RPG_Graphics_t& graphic_in,
                                         std::string& file_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::graphicToFile"));

  // init return value(s)
  file_out = myGraphicsDirectory;
  file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  switch (graphic_in.category)
  {
    case CATEGORY_CURSOR:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_CURSORS_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_FONT:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_FONTS_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_INTERFACE:
    case CATEGORY_IMAGE:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_IMAGES_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_SPRITE:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_CREATURES_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_TILE:
    {
      // follow references
      RPG_Graphics_GraphicTypeUnion current_type = graphic_in.type;
      RPG_Graphics_t graphic = graphic_in;
      while (graphic.tile.reference.discriminator != RPG_Graphics_GraphicTypeUnion::INVALID)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("%s --> %s\n"),
                   RPG_Graphics_Common_Tools::typeToString(current_type).c_str(),
                   RPG_Graphics_Common_Tools::typeToString(graphic.tile.reference).c_str()));

        current_type = graphic.tile.reference;

        // retrieve properties from the dictionary
        graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(graphic.tile.reference);
      } // end WHILE

      // assemble path
      switch (graphic.tile.type)
      {
        case TILETYPE_FLOOR:
          file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_FLOORS_SUB); break;
        case TILETYPE_WALL:
          file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_WALLS_SUB); break;
        case TILETYPE_DOOR:
          file_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_DOORS_SUB); break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid tile type (was: \"%s\"), aborting\n"),
                     RPG_Graphics_TileTypeHelper::RPG_Graphics_TileTypeToString(graphic.tile.type).c_str()));

          file_out.clear();

          return;
        }
      } // end SWITCH
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic.tile.file;

      break;
    }
    case CATEGORY_TILESET:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid category (was: \"%s\"), aborting\n"),
                 RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic_in.category).c_str()));

      file_out.clear();

      return;
    }
  } // end SWITCH
}

const RPG_Graphics_TextSize_t
RPG_Graphics_Common_Tools::textSize(const RPG_Graphics_Font& font_in,
                                    const std::string& textString_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::textSize"));

  int width = 0;
  int height = 0;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find(font_in);
    ACE_ASSERT(iterator != myFontCache.end());

    if (TTF_SizeText((*iterator).second,
                     textString_in.c_str(),
                     &width,
                     &height))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to TTF_SizeText(\"%s\", \"%s\"): %s, aborting\n"),
                 RPG_Graphics_FontHelper::RPG_Graphics_FontToString(font_in).c_str(),
                 textString_in.c_str(),
                 SDL_GetError()));

      return RPG_Graphics_TextSize_t(0, 0);
    } // end IF
  } // end lock scope

  return std::make_pair(static_cast<unsigned long> (width),
                        static_cast<unsigned long> (height));
}

void
RPG_Graphics_Common_Tools::loadFloorEdgeTileSet(const RPG_Graphics_EdgeStyle& style_in,
                                                RPG_Graphics_FloorEdgeTileSet_t& tileSet_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadFloorEdgeTileSet"));

  // init return value(s)
  if (tileSet_out.east.surface)
  {
    SDL_FreeSurface(tileSet_out.east.surface);
    tileSet_out.east.surface = NULL;
  } // end IF
  if (tileSet_out.west.surface)
  {
    SDL_FreeSurface(tileSet_out.west.surface);
    tileSet_out.west.surface = NULL;
  } // end IF
  if (tileSet_out.north.surface)
  {
    SDL_FreeSurface(tileSet_out.north.surface);
    tileSet_out.north.surface = NULL;
  } // end IF
  if (tileSet_out.south.surface)
  {
    SDL_FreeSurface(tileSet_out.south.surface);
    tileSet_out.south.surface = NULL;
  } // end IF
  if (tileSet_out.south_west.surface)
  {
    SDL_FreeSurface(tileSet_out.south_west.surface);
    tileSet_out.south_west.surface = NULL;
  } // end IF
  if (tileSet_out.south_east.surface)
  {
    SDL_FreeSurface(tileSet_out.south_east.surface);
    tileSet_out.south_east.surface = NULL;
  } // end IF
  if (tileSet_out.north_west.surface)
  {
    SDL_FreeSurface(tileSet_out.north_west.surface);
    tileSet_out.north_west.surface = NULL;
  } // end IF
  if (tileSet_out.north_east.surface)
  {
    SDL_FreeSurface(tileSet_out.north_east.surface);
    tileSet_out.north_east.surface = NULL;
  } // end IF
  if (tileSet_out.top.surface)
  {
    SDL_FreeSurface(tileSet_out.top.surface);
    tileSet_out.top.surface = NULL;
  } // end IF
  if (tileSet_out.right.surface)
  {
    SDL_FreeSurface(tileSet_out.right.surface);
    tileSet_out.right.surface = NULL;
  } // end IF
  if (tileSet_out.left.surface)
  {
    SDL_FreeSurface(tileSet_out.left.surface);
    tileSet_out.left.surface = NULL;
  } // end IF
  if (tileSet_out.bottom.surface)
  {
    SDL_FreeSurface(tileSet_out.bottom.surface);
    tileSet_out.bottom.surface = NULL;
  } // end IF

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type;
  graphic_type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve (list of) tiles
  RPG_Graphics_t graphic;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(graphic_type);
  ACE_ASSERT(graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: \"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_FLOORS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  RPG_Graphics_Tile_t current_tile;
  unsigned long type = 0;
  std::istringstream converter;
//  size_t edge_position = std::string::npos;
  size_t edge_position = -1;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface = RPG_Graphics_Surface::load(path,  // file
                                                      true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      if (tileSet_out.east.surface)
      {
        SDL_FreeSurface(tileSet_out.east.surface);
        tileSet_out.east.surface = NULL;
      } // end IF
      if (tileSet_out.west.surface)
      {
        SDL_FreeSurface(tileSet_out.west.surface);
        tileSet_out.west.surface = NULL;
      } // end IF
      if (tileSet_out.north.surface)
      {
        SDL_FreeSurface(tileSet_out.north.surface);
        tileSet_out.north.surface = NULL;
      } // end IF
      if (tileSet_out.south.surface)
      {
        SDL_FreeSurface(tileSet_out.south.surface);
        tileSet_out.south.surface = NULL;
      } // end IF
      if (tileSet_out.south_west.surface)
      {
        SDL_FreeSurface(tileSet_out.south_west.surface);
        tileSet_out.south_west.surface = NULL;
      } // end IF
      if (tileSet_out.south_east.surface)
      {
        SDL_FreeSurface(tileSet_out.south_east.surface);
        tileSet_out.south_east.surface = NULL;
      } // end IF
      if (tileSet_out.north_west.surface)
      {
        SDL_FreeSurface(tileSet_out.north_west.surface);
        tileSet_out.north_west.surface = NULL;
      } // end IF
      if (tileSet_out.north_east.surface)
      {
        SDL_FreeSurface(tileSet_out.north_east.surface);
        tileSet_out.north_east.surface = NULL;
      } // end IF
      if (tileSet_out.top.surface)
      {
        SDL_FreeSurface(tileSet_out.top.surface);
        tileSet_out.top.surface = NULL;
      } // end IF
      if (tileSet_out.right.surface)
      {
        SDL_FreeSurface(tileSet_out.right.surface);
        tileSet_out.right.surface = NULL;
      } // end IF
      if (tileSet_out.left.surface)
      {
        SDL_FreeSurface(tileSet_out.left.surface);
        tileSet_out.left.surface = NULL;
      } // end IF
      if (tileSet_out.bottom.surface)
      {
        SDL_FreeSurface(tileSet_out.bottom.surface);
        tileSet_out.bottom.surface = NULL;
      } // end IF

      return;
    } // end IF

    // extract position/type from the filename (i.e. "floor_xxx_yyy_zzz_edge#.png")
    converter.clear();
    converter.str((*iterator).file);
    edge_position = (*iterator).file.find(ACE_TEXT_ALWAYS_CHAR("edge"), 7, 4);
//    if (edge_position == std::string::npos)
    if (edge_position == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to find \"edge\" sequence in filename (was: \"%s\"), aborting\n"),
                 (*iterator).file.c_str()));

      // clean up
      if (tileSet_out.east.surface)
      {
        SDL_FreeSurface(tileSet_out.east.surface);
        tileSet_out.east.surface = NULL;
      } // end IF
      if (tileSet_out.west.surface)
      {
        SDL_FreeSurface(tileSet_out.west.surface);
        tileSet_out.west.surface = NULL;
      } // end IF
      if (tileSet_out.north.surface)
      {
        SDL_FreeSurface(tileSet_out.north.surface);
        tileSet_out.north.surface = NULL;
      } // end IF
      if (tileSet_out.south.surface)
      {
        SDL_FreeSurface(tileSet_out.south.surface);
        tileSet_out.south.surface = NULL;
      } // end IF
      if (tileSet_out.south_west.surface)
      {
        SDL_FreeSurface(tileSet_out.south_west.surface);
        tileSet_out.south_west.surface = NULL;
      } // end IF
      if (tileSet_out.south_east.surface)
      {
        SDL_FreeSurface(tileSet_out.south_east.surface);
        tileSet_out.south_east.surface = NULL;
      } // end IF
      if (tileSet_out.north_west.surface)
      {
        SDL_FreeSurface(tileSet_out.north_west.surface);
        tileSet_out.north_west.surface = NULL;
      } // end IF
      if (tileSet_out.north_east.surface)
      {
        SDL_FreeSurface(tileSet_out.north_east.surface);
        tileSet_out.north_east.surface = NULL;
      } // end IF
      if (tileSet_out.top.surface)
      {
        SDL_FreeSurface(tileSet_out.top.surface);
        tileSet_out.top.surface = NULL;
      } // end IF
      if (tileSet_out.right.surface)
      {
        SDL_FreeSurface(tileSet_out.right.surface);
        tileSet_out.right.surface = NULL;
      } // end IF
      if (tileSet_out.left.surface)
      {
        SDL_FreeSurface(tileSet_out.left.surface);
        tileSet_out.left.surface = NULL;
      } // end IF
      if (tileSet_out.bottom.surface)
      {
        SDL_FreeSurface(tileSet_out.bottom.surface);
        tileSet_out.bottom.surface = NULL;
      } // end IF

      return;
    } // end IF
    converter.ignore(edge_position + 4);
    converter >> type;
    switch (type)
    {
      case 1:
        tileSet_out.west = current_tile; break;
      case 2:
        tileSet_out.north = current_tile; break;
      case 3:
        tileSet_out.east = current_tile; break;
      case 4:
        tileSet_out.south = current_tile; break;
      case 5:
        tileSet_out.south_west = current_tile; break;
      case 6:
        tileSet_out.north_west = current_tile; break;
      case 7:
        tileSet_out.north_east = current_tile; break;
      case 8:
        tileSet_out.south_east = current_tile; break;
      case 9:
        tileSet_out.left = current_tile; break;
      case 10:
        tileSet_out.top = current_tile; break;
      case 11:
        tileSet_out.right = current_tile; break;
      case 12:
        tileSet_out.bottom = current_tile; break;
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid edgetile type (was: %u), continuing\n"),
                   type));

        break;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: \"%s\", style: \"%s\")...\n"),
             RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str()));
}

void
RPG_Graphics_Common_Tools::loadFloorTileSet(const RPG_Graphics_FloorStyle& style_in,
                                            RPG_Graphics_FloorTileSet_t& tileSet_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadFloorTileSet"));

  // init return value(s)
  for (RPG_Graphics_FloorTilesConstIterator_t iterator = tileSet_out.tiles.begin();
       iterator != tileSet_out.tiles.end();
       iterator++)
    SDL_FreeSurface((*iterator).surface);
  tileSet_out.tiles.clear();
  tileSet_out.columns = 0;
  tileSet_out.rows = 0;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type;
  graphic_type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve (list of) tiles
  RPG_Graphics_t graphic;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(graphic_type);
  ACE_ASSERT(graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: \"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_FLOORS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  RPG_Graphics_Tile_t current_tile;
  unsigned long column = 0;
  unsigned long row = 0;
  std::istringstream converter;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface = RPG_Graphics_Surface::load(path,  // file
                                                      true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      for (RPG_Graphics_FloorTilesConstIterator_t iterator = tileSet_out.tiles.begin();
           iterator != tileSet_out.tiles.end();
           iterator++)
        SDL_FreeSurface((*iterator).surface);
      tileSet_out.tiles.clear();

      return;
    } // end IF

    // extract column/row from the filename (i.e. "floor_xxx_yyy_zzz_..._column#_row#.png")
    converter.clear();
    converter.str((*iterator).file);
    converter.ignore((*iterator).file.size(), '_');
    while (ACE_OS::ace_isdigit(converter.peek()) == 0)
      converter.ignore((*iterator).file.size(), '_');
    converter >> column;
    converter.ignore((*iterator).file.size(), '_');
    converter >> row;

    if (column > tileSet_out.columns)
      tileSet_out.columns = column;
    if (row > tileSet_out.rows)
      tileSet_out.rows = row;
    tileSet_out.tiles.push_back(current_tile);
  } // end FOR
  tileSet_out.columns++; tileSet_out.rows++;
  ACE_ASSERT(tileSet_out.tiles.size() == (tileSet_out.columns * tileSet_out.rows));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s) [%u x %u])...\n"),
             RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str(),
             tileSet_out.tiles.size(),
             tileSet_out.columns,
             tileSet_out.rows));
}

void
RPG_Graphics_Common_Tools::loadWallTileSet(const RPG_Graphics_WallStyle& style_in,
                                           const bool& halfHeight_in,
                                           RPG_Graphics_WallTileSet_t& tileSet_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadWallTileSet"));

  // init return value(s)
  if (tileSet_out.west.surface)
    SDL_FreeSurface(tileSet_out.west.surface);
  if (tileSet_out.east.surface)
    SDL_FreeSurface(tileSet_out.east.surface);
  if (tileSet_out.north.surface)
    SDL_FreeSurface(tileSet_out.north.surface);
  if (tileSet_out.south.surface)
    SDL_FreeSurface(tileSet_out.south.surface);
  tileSet_out.west.offset_x = 0;
  tileSet_out.west.offset_y = 0;
  tileSet_out.west.surface = NULL;
  tileSet_out.east.offset_x = 0;
  tileSet_out.east.offset_y = 0;
  tileSet_out.east.surface = NULL;
  tileSet_out.north.offset_x = 0;
  tileSet_out.north.offset_y = 0;
  tileSet_out.north.surface = NULL;
  tileSet_out.south.offset_x = 0;
  tileSet_out.south.offset_y = 0;
  tileSet_out.south.surface = NULL;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type;
  graphic_type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style, halfHeight_in);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve list of tiles
  RPG_Graphics_t graphic;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(graphic_type);
  ACE_ASSERT(graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: %s), aborting\n"),
               RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_WALLS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  RPG_Graphics_Tile_t current_tile;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface = RPG_Graphics_Surface::load(path,  // file
                                                      true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      if (tileSet_out.west.surface)
        SDL_FreeSurface(tileSet_out.west.surface);
      if (tileSet_out.east.surface)
        SDL_FreeSurface(tileSet_out.east.surface);
      if (tileSet_out.north.surface)
        SDL_FreeSurface(tileSet_out.north.surface);
      if (tileSet_out.south.surface)
        SDL_FreeSurface(tileSet_out.south.surface);
      tileSet_out.west.offset_x = 0;
      tileSet_out.west.offset_y = 0;
      tileSet_out.west.surface = NULL;
      tileSet_out.east.offset_x = 0;
      tileSet_out.east.offset_y = 0;
      tileSet_out.east.surface = NULL;
      tileSet_out.north.offset_x = 0;
      tileSet_out.north.offset_y = 0;
      tileSet_out.north.surface = NULL;
      tileSet_out.south.offset_x = 0;
      tileSet_out.south.offset_y = 0;
      tileSet_out.south.surface = NULL;

      return;
    } // end IF

    switch ((*iterator).orientation)
    {
      case ORIENTATION_EAST:
        tileSet_out.east = current_tile; break;
      case ORIENTATION_WEST:
        tileSet_out.west = current_tile; break;
      case ORIENTATION_NORTH:
        tileSet_out.north = current_tile; break;
      case ORIENTATION_SOUTH:
        tileSet_out.south = current_tile; break;
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid orientation \"%s\", aborting\n"),
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString((*iterator).orientation).c_str()));

        // clean up
        if (tileSet_out.west.surface)
          SDL_FreeSurface(tileSet_out.west.surface);
        if (tileSet_out.east.surface)
          SDL_FreeSurface(tileSet_out.east.surface);
        if (tileSet_out.north.surface)
          SDL_FreeSurface(tileSet_out.north.surface);
        if (tileSet_out.south.surface)
          SDL_FreeSurface(tileSet_out.south.surface);
        tileSet_out.west.offset_x = 0;
        tileSet_out.west.offset_y = 0;
        tileSet_out.west.surface = NULL;
        tileSet_out.east.offset_x = 0;
        tileSet_out.east.offset_y = 0;
        tileSet_out.east.surface = NULL;
        tileSet_out.north.offset_x = 0;
        tileSet_out.north.offset_y = 0;
        tileSet_out.north.surface = NULL;
        tileSet_out.south.offset_x = 0;
        tileSet_out.south.offset_y = 0;
        tileSet_out.south.surface = NULL;

        return;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
             RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str(),
             4));
}

void
RPG_Graphics_Common_Tools::loadDoorTileSet(const RPG_Graphics_DoorStyle& style_in,
                                           RPG_Graphics_DoorTileSet_t& tileSet_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadDoorTileSet"));

  // init return value(s)
  if (tileSet_out.horizontal_open.surface)
    SDL_FreeSurface(tileSet_out.horizontal_open.surface);
  if (tileSet_out.vertical_open.surface)
    SDL_FreeSurface(tileSet_out.vertical_open.surface);
  if (tileSet_out.horizontal_closed.surface)
    SDL_FreeSurface(tileSet_out.horizontal_closed.surface);
  if (tileSet_out.vertical_closed.surface)
    SDL_FreeSurface(tileSet_out.vertical_closed.surface);
  if (tileSet_out.broken.surface)
    SDL_FreeSurface(tileSet_out.broken.surface);
  tileSet_out.horizontal_open.offset_x = 0;
  tileSet_out.horizontal_open.offset_y = 0;
  tileSet_out.horizontal_open.surface = NULL;
  tileSet_out.vertical_open.offset_x = 0;
  tileSet_out.vertical_open.offset_y = 0;
  tileSet_out.vertical_open.surface = NULL;
  tileSet_out.horizontal_closed.offset_x = 0;
  tileSet_out.horizontal_closed.offset_y = 0;
  tileSet_out.horizontal_closed.surface = NULL;
  tileSet_out.vertical_closed.offset_x = 0;
  tileSet_out.vertical_closed.offset_y = 0;
  tileSet_out.vertical_closed.surface = NULL;
  tileSet_out.broken.offset_x = 0;
  tileSet_out.broken.offset_y = 0;
  tileSet_out.broken.surface = NULL;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type;
  graphic_type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  graphic_type = RPG_Graphics_Common_Tools::styleToType(style);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::styleToString(style).c_str()));

    return;
  } // end IF

  // step1: retrieve list of tiles
  RPG_Graphics_t graphic;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(graphic_type);
  ACE_ASSERT(graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: %s), aborting\n"),
               RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str()));

    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_DOORS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  RPG_Graphics_Tile_t current_tile;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin();
       iterator != graphic.tileset.tiles.end();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface = RPG_Graphics_Surface::load(path,  // file
                                                      true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                 path.c_str()));

      // clean up
      if (tileSet_out.horizontal_open.surface)
        SDL_FreeSurface(tileSet_out.horizontal_open.surface);
      if (tileSet_out.vertical_open.surface)
        SDL_FreeSurface(tileSet_out.vertical_open.surface);
      if (tileSet_out.horizontal_closed.surface)
        SDL_FreeSurface(tileSet_out.horizontal_closed.surface);
      if (tileSet_out.vertical_closed.surface)
        SDL_FreeSurface(tileSet_out.vertical_closed.surface);
      if (tileSet_out.broken.surface)
        SDL_FreeSurface(tileSet_out.broken.surface);
      tileSet_out.horizontal_open.offset_x = 0;
      tileSet_out.horizontal_open.offset_y = 0;
      tileSet_out.horizontal_open.surface = NULL;
      tileSet_out.vertical_open.offset_x = 0;
      tileSet_out.vertical_open.offset_y = 0;
      tileSet_out.vertical_open.surface = NULL;
      tileSet_out.horizontal_closed.offset_x = 0;
      tileSet_out.horizontal_closed.offset_y = 0;
      tileSet_out.horizontal_closed.surface = NULL;
      tileSet_out.vertical_closed.offset_x = 0;
      tileSet_out.vertical_closed.offset_y = 0;
      tileSet_out.vertical_closed.surface = NULL;
      tileSet_out.broken.offset_x = 0;
      tileSet_out.broken.offset_y = 0;
      tileSet_out.broken.surface = NULL;

      return;
    } // end IF

    switch ((*iterator).orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        if ((*iterator).open)
          tileSet_out.horizontal_open = current_tile;
        else
          tileSet_out.horizontal_closed = current_tile;

        break;
      }
      case ORIENTATION_VERTICAL:
      {
        if ((*iterator).open)
          tileSet_out.vertical_open = current_tile;
        else
          tileSet_out.vertical_closed = current_tile;

        break;
      }
      case RPG_GRAPHICS_ORIENTATION_INVALID:
      {
        if ((*iterator).broken)
        {
          tileSet_out.broken = current_tile;

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
        if (tileSet_out.horizontal_open.surface)
          SDL_FreeSurface(tileSet_out.horizontal_open.surface);
        if (tileSet_out.vertical_open.surface)
          SDL_FreeSurface(tileSet_out.vertical_open.surface);
        if (tileSet_out.horizontal_closed.surface)
          SDL_FreeSurface(tileSet_out.horizontal_closed.surface);
        if (tileSet_out.vertical_closed.surface)
          SDL_FreeSurface(tileSet_out.vertical_closed.surface);
        if (tileSet_out.broken.surface)
          SDL_FreeSurface(tileSet_out.broken.surface);
        tileSet_out.horizontal_open.offset_x = 0;
        tileSet_out.horizontal_open.offset_y = 0;
        tileSet_out.horizontal_open.surface = NULL;
        tileSet_out.vertical_open.offset_x = 0;
        tileSet_out.vertical_open.offset_y = 0;
        tileSet_out.vertical_open.surface = NULL;
        tileSet_out.horizontal_closed.offset_x = 0;
        tileSet_out.horizontal_closed.offset_y = 0;
        tileSet_out.horizontal_closed.surface = NULL;
        tileSet_out.vertical_closed.offset_x = 0;
        tileSet_out.vertical_closed.offset_y = 0;
        tileSet_out.vertical_closed.surface = NULL;
        tileSet_out.broken.offset_x = 0;
        tileSet_out.broken.offset_y = 0;
        tileSet_out.broken.surface = NULL;

        return;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
             RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str(),
             RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString(graphic.tileset.type).c_str(),
             RPG_Graphics_Common_Tools::styleToString(graphic.tileset.style).c_str(),
             5));
}

SDL_Surface*
RPG_Graphics_Common_Tools::loadGraphic(const RPG_Graphics_GraphicTypeUnion& type_in,
                                       const bool& convertToDisplayFormat_in,
                                       const bool& cacheGraphic_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadGraphic"));

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
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic;
  graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type_in);
  ACE_ASSERT((graphic.category != RPG_GRAPHICS_CATEGORY_INVALID) &&
             (graphic.type.discriminator == type_in.discriminator)); // too weak
  // sanity check
  if ((graphic.category != CATEGORY_CURSOR) &&
      (graphic.category != CATEGORY_INTERFACE) &&
      (graphic.category != CATEGORY_IMAGE) &&
      (graphic.category != CATEGORY_SPRITE) &&
      (graphic.category != CATEGORY_TILE))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid category (was: \"%s\"): \"%s\" not an image type, aborting\n"),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str(),
               RPG_Graphics_Common_Tools::typeToString(graphic.type).c_str()));

    return NULL;
  } // end IF

  // assemble path
  std::string filename;
  RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);
  ACE_ASSERT(!filename.empty());

  // load file
  node.image = RPG_Graphics_Surface::load(filename,                   // file
                                          convertToDisplayFormat_in); // convert to display format
  if (!node.image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
               filename.c_str()));

    return NULL;
  } // end IF

  // step3: update cache
  if (cacheGraphic_in)
  {
    // synch cache access
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

      if (myGraphicsCache.size() == myCacheSize)
      {
        iter = myGraphicsCache.begin();
        ACE_ASSERT(myGraphicsCache.size() >= myOldestCacheEntry);
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
RPG_Graphics_Common_Tools::renderText(const RPG_Graphics_Font& font_in,
                                      const std::string& textString_in,
                                      const SDL_Color& color_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::renderText"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find(font_in);
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

void
RPG_Graphics_Common_Tools::fade(const bool& fadeIn_in,
                                const float& interval_in,
                                const Uint32& color_in,
                                SDL_Surface* screen_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));

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
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initStringConversionTables"));

  RPG_Graphics_CategoryHelper::init();
  RPG_Graphics_OrientationHelper::init();
  RPG_Graphics_TileTypeHelper::init();
  RPG_Graphics_FloorStyleHelper::init();
  RPG_Graphics_EdgeStyleHelper::init();
  RPG_Graphics_StairsStyleHelper::init();
  RPG_Graphics_WallStyleHelper::init();
  RPG_Graphics_DoorStyleHelper::init();
  RPG_Graphics_CursorHelper::init();
  RPG_Graphics_FontHelper::init();
  RPG_Graphics_ImageHelper::init();
  RPG_Graphics_SpriteHelper::init();
  RPG_Graphics_TileGraphicHelper::init();
  RPG_Graphics_TileSetGraphicHelper::init();
  RPG_Graphics_WindowTypeHelper::init();
  RPG_Graphics_InterfaceElementTypeHelper::init();
  RPG_Graphics_HotspotTypeHelper::init();
  RPG_Graphics_TileSetTypeHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_Common_Tools: initialized string conversion tables...\n")));
}

const bool
RPG_Graphics_Common_Tools::initFonts()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initFonts"));

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
      path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_FONTS_SUB);
      path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
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
                 ACE_TEXT("loaded font \"%s - %s\" %d face(s) [%s], height [+/-]: %d (%d,%d), space: %d\n"),
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

// void
// RPG_Graphics_Common_Tools::fade(const double& interval_in,
//                                 SDL_Surface* targetImage_in,
//                                 SDL_Surface* screen_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));
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
//     percentage = (static_cast<float> ((cur_clock - start_clock)) /
//                   static_cast<float> ((end_clock - start_clock)));
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
RPG_Graphics_Common_Tools::fade(const float& interval_in,
                                SDL_Surface* targetImage_in,
                                SDL_Surface* screen_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));

  // calculate the number of blends
  int n_steps = static_cast<int>(RPG_GRAPHICS_FADE_REFRESH_RATE * interval_in);
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
  end_clock = start_clock + static_cast<int>(interval_in * 1000);
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
