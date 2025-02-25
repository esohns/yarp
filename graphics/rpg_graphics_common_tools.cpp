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
#include "stdafx.h"

#include "rpg_graphics_common_tools.h"

#include <sstream>
#include <numeric>

#include "png.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_file_tools.h"
#include "common_ilock.h"

#include "rpg_common_macros.h"

#include "rpg_dice_common.h"
#include "rpg_dice.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_SDL_tools.h"
#include "rpg_graphics_surface.h"
#if defined (SDL2_USE)
#include "rpg_graphics_texture.h"
#endif // SDL2_USE

// initialize statics
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
// -----------------------------------------------------------------------------
RPG_Graphics_ColorNameToStringTable_t RPG_Graphics_ColorNameHelper::myRPG_Graphics_ColorNameToStringTable;

std::string                  RPG_Graphics_Common_Tools::myGraphicsDirectory;

ACE_Thread_Mutex             RPG_Graphics_Common_Tools::myCacheLock;
unsigned int                 RPG_Graphics_Common_Tools::myOldestCacheEntry = 0;
unsigned int                 RPG_Graphics_Common_Tools::myCacheSize = 0;
RPG_Graphics_GraphicsCache_t RPG_Graphics_Common_Tools::myGraphicsCache;

RPG_Graphics_FontCache_t     RPG_Graphics_Common_Tools::myFontCache;

bool                         RPG_Graphics_Common_Tools::myInitialized = false;
bool                         RPG_Graphics_Common_Tools::myPreInitialized = false;

void
RPG_Graphics_Common_Tools::preInitialize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::preInitialize"));

  // initialize string conversion facilities
  RPG_Graphics_Common_Tools::initializeStringConversionTables ();

  SDL_bool result =
#if defined(SDL_USE) || defined(SDL2_USE)
    SDL_SetHint (ACE_TEXT_ALWAYS_CHAR (SDL_HINT_VIDEODRIVER),
#elif defined(SDL3_USE)
    SDL_SetHint (ACE_TEXT_ALWAYS_CHAR (SDL_HINT_VIDEO_DRIVER),
#endif // SDL_USE || SDL2_USE || SDL3_USE
                 ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME));
  if (result == SDL_FALSE)
#if defined (SDL_USE) || defined (SDL2_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetHint(\"%s\",\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (SDL_HINT_VIDEODRIVER),
                ACE_TEXT (RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME),
                ACE_TEXT (SDL_GetError ())));
#elif defined (SDL3_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetHint(\"%s\",\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (SDL_HINT_VIDEO_DRIVER),
                ACE_TEXT (RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME),
                ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("set video driver to \"%s\"...\n"),
                ACE_TEXT (RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME)));

  myPreInitialized = true;
}

bool
RPG_Graphics_Common_Tools::initialize (const std::string& directory_in,
                                       unsigned int cacheSize_in,
                                       bool initializeSDL_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::initialize"));

  if (!myPreInitialized)
    RPG_Graphics_Common_Tools::preInitialize ();

  if (!directory_in.empty ())
  {
    // sanity check(s)
    if (!Common_File_Tools::isDirectory (directory_in))
    {
      // re-try with resolved path
      std::string resolved_path = Common_File_Tools::realPath (directory_in);
      if (!Common_File_Tools::isDirectory (resolved_path))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid argument \"%s\": not a directory, aborting\n"),
                    ACE_TEXT (directory_in.c_str ())));
        return false;
      } // end IF
    } // end IF
    myGraphicsDirectory = directory_in;
  } // end IF

  myCacheSize = cacheSize_in;

  if (myInitialized)
  {
    // clean up
    finalize ();

    myInitialized = false;
  } // end IF

  // initialize colors ?
  if (initializeSDL_in)
    RPG_Graphics_SDL_Tools::initializeColors ();

  // initialize fonts ?
  if (initializeSDL_in)
    if (!RPG_Graphics_Common_Tools::initializeFonts ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initializeFonts(), aborting\n")));
      return false;
    } // end IF

  myInitialized = true;

  return true;
}

void
RPG_Graphics_Common_Tools::finalize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::finalize"));

  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myCacheLock);
    // clear the graphics cache
    for (RPG_Graphics_GraphicsCacheIterator_t iter = myGraphicsCache.begin ();
         iter != myGraphicsCache.end ();
         iter++)
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface ((*iter).image);
#elif defined (SDL3_USE)
     SDL_DestroySurface ((*iter).image);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    myGraphicsCache.clear ();
    myOldestCacheEntry = 0;

    // clear the font cache
    for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin ();
         iter != myFontCache.end ();
         iter++)
      TTF_CloseFont ((*iter).second);
    myFontCache.clear ();
  } // end lock scope

  myInitialized = false;
}

std::string
RPG_Graphics_Common_Tools::getGraphicsDirectory ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::getGraphicsDirectory"));

  // sanity check
  ACE_ASSERT (!myGraphicsDirectory.empty ());

  return myGraphicsDirectory;
}

std::string
RPG_Graphics_Common_Tools::toString (const struct RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::toString"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
      return RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString (style_in.floorstyle);
    case RPG_Graphics_StyleUnion::EDGESTYLE:
      return RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString (style_in.edgestyle);
    case RPG_Graphics_StyleUnion::STAIRSSTYLE:
      return RPG_Graphics_StairsStyleHelper::RPG_Graphics_StairsStyleToString (style_in.stairsstyle);
    case RPG_Graphics_StyleUnion::WALLSTYLE:
      return RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString (style_in.wallstyle);
    case RPG_Graphics_StyleUnion::DOORSTYLE:
      return RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString (style_in.doorstyle);
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid style (was: %d), aborting\n"),
                  style_in.discriminator));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("INVALID");
}

std::string
RPG_Graphics_Common_Tools::toString (const struct RPG_Graphics_GraphicTypeUnion& type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::toString"));

  switch (type_in.discriminator)
  {
    case RPG_Graphics_GraphicTypeUnion::CURSOR:
      return RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString (type_in.cursor);
    case RPG_Graphics_GraphicTypeUnion::FONT:
      return RPG_Graphics_FontHelper::RPG_Graphics_FontToString (type_in.font);
    case RPG_Graphics_GraphicTypeUnion::IMAGE:
      return RPG_Graphics_ImageHelper::RPG_Graphics_ImageToString (type_in.image);
    case RPG_Graphics_GraphicTypeUnion::SPRITE:
      return RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString (type_in.sprite);
    case RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC:
      return RPG_Graphics_TileGraphicHelper::RPG_Graphics_TileGraphicToString (type_in.tilegraphic);
    case RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC:
      return RPG_Graphics_TileSetGraphicHelper::RPG_Graphics_TileSetGraphicToString (type_in.tilesetgraphic);
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), aborting\n"),
                  type_in.discriminator));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("INVALID");
}

std::string
RPG_Graphics_Common_Tools::toString (const struct RPG_Graphics_Tile& tile_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::toString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR ("type: ");
  result +=
      RPG_Graphics_TileTypeHelper::RPG_Graphics_TileTypeToString (tile_in.type);
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("reference: ");
  switch (tile_in.reference.discriminator)
  {
    case RPG_Graphics_GraphicTypeUnion::CURSOR:
      result +=
          RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString (tile_in.reference.cursor); break;
    case RPG_Graphics_GraphicTypeUnion::FONT:
      result +=
          RPG_Graphics_FontHelper::RPG_Graphics_FontToString (tile_in.reference.font); break;
    case RPG_Graphics_GraphicTypeUnion::IMAGE:
      result +=
          RPG_Graphics_ImageHelper::RPG_Graphics_ImageToString (tile_in.reference.image); break;
    case RPG_Graphics_GraphicTypeUnion::SPRITE:
      result +=
          RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString (tile_in.reference.sprite); break;
    case RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC:
      result +=
          RPG_Graphics_TileGraphicHelper::RPG_Graphics_TileGraphicToString (tile_in.reference.tilegraphic); break;
    case RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC:
      result +=
          RPG_Graphics_TileSetGraphicHelper::RPG_Graphics_TileSetGraphicToString (tile_in.reference.tilesetgraphic); break;
    case RPG_Graphics_GraphicTypeUnion::INVALID:
      result += ACE_TEXT_ALWAYS_CHAR ("N/A"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), continuing\n"),
                  tile_in.reference.discriminator));
      break;
    }
  } // end SWITCH
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("style: ");
  if (tile_in.style.discriminator == RPG_Graphics_StyleUnion::INVALID)
    result += ACE_TEXT_ALWAYS_CHAR ("N/A");
  else
    result += RPG_Graphics_Common_Tools::toString (tile_in.style);
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("orientation: ");
  result +=
      ((tile_in.orientation ==
        RPG_GRAPHICS_ORIENTATION_INVALID) ? ACE_TEXT ("N/A")
                                          : RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString (tile_in.orientation));
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("file: ");
  result += tile_in.file;
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  std::ostringstream converter;
  result += ACE_TEXT_ALWAYS_CHAR ("offset (x,y): ");
  converter << tile_in.offsetX;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR (",");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << tile_in.offsetY;
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("open/broken: ");
  result += (tile_in.open ? ACE_TEXT_ALWAYS_CHAR ("true")
                          : ACE_TEXT_ALWAYS_CHAR ("false"));
  result += ACE_TEXT ("/");
  result += (tile_in.broken ? ACE_TEXT_ALWAYS_CHAR ("true")
                            : ACE_TEXT_ALWAYS_CHAR ("false"));
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  return result;
}

std::string
RPG_Graphics_Common_Tools::toString (const struct RPG_Graphics_TileSet& tileSet_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::toString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR ("type: ");
  result +=
      RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString (tileSet_in.type);
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("style: ");
  result += RPG_Graphics_Common_Tools::toString (tileSet_in.style);
  if (tileSet_in.type == TILESETTYPE_WALL)
    result += (tileSet_in.half ? ACE_TEXT_ALWAYS_CHAR ("[half height]")
                               : ACE_TEXT_ALWAYS_CHAR ("[full height]"));
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("tiles[");
  std::ostringstream converter;
  converter << tileSet_in.tiles.size ();
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("]:\n");
  result += ACE_TEXT_ALWAYS_CHAR ("------\n");
  for (RPG_Graphics_TileSetConstIterator_t iterator = tileSet_in.tiles.begin ();
       iterator != tileSet_in.tiles.end ();
       iterator++)
  {
    result += RPG_Graphics_Common_Tools::toString (*iterator);
    result += ACE_TEXT_ALWAYS_CHAR ("------\n");
  } // end FOR

  return result;
}

std::string
RPG_Graphics_Common_Tools::toString (const struct RPG_Graphics_ElementTypeUnion& elementType_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::toString"));

  switch (elementType_in.discriminator)
  {
    case RPG_Graphics_ElementTypeUnion::INTERFACEELEMENTTYPE:
      return RPG_Graphics_InterfaceElementTypeHelper::RPG_Graphics_InterfaceElementTypeToString (elementType_in.interfaceelementtype);
    case RPG_Graphics_ElementTypeUnion::HOTSPOTTYPE:
      return RPG_Graphics_HotspotTypeHelper::RPG_Graphics_HotspotTypeToString (elementType_in.hotspottype);
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid element type (was: %d), aborting\n"),
                  elementType_in.discriminator));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("INVALID");
}

std::string
RPG_Graphics_Common_Tools::toString (const RPG_Graphics_Elements_t& elements_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::elementsToString"));

  std::string result;

  std::ostringstream converter;
  unsigned long index = 1;
  for (RPG_Graphics_ElementsConstIterator_t iterator = elements_in.begin ();
       iterator != elements_in.end ();
       iterator++, index++)
  {
    result += ACE_TEXT_ALWAYS_CHAR ("#");
    converter.clear ();
    converter.str(ACE_TEXT_ALWAYS_CHAR (""));
    converter << index;
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR (" ");
    result += RPG_Graphics_Common_Tools::toString ((*iterator).type);
    result += ACE_TEXT_ALWAYS_CHAR (" {x: ");
    converter.clear ();
    converter.str(ACE_TEXT_ALWAYS_CHAR (""));
    converter << (*iterator).offsetX;
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR (", y: ");
    converter.clear ();
    converter.str(ACE_TEXT_ALWAYS_CHAR (""));
    converter << (*iterator).offsetY;
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR (", w: ");
    converter.clear ();
    converter.str(ACE_TEXT_ALWAYS_CHAR (""));
    converter << (*iterator).width;
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR (", h: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR (""));
    converter << (*iterator).height;
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR ("}\n");
  } // end FOR

  return result;
}

std::string
RPG_Graphics_Common_Tools::toString (const RPG_Graphics_t& graphic_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::graphicToString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR ("category: ");
  result +=
      RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic_in.category);
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result += ACE_TEXT_ALWAYS_CHAR ("type: ");
  result += RPG_Graphics_Common_Tools::toString (graphic_in.type);
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result += ACE_TEXT_ALWAYS_CHAR ("tile: ");
  if (graphic_in.tile.type == RPG_GRAPHICS_TILETYPE_INVALID)
    result += ACE_TEXT_ALWAYS_CHAR ("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
    result += RPG_Graphics_Common_Tools::toString (graphic_in.tile);
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR ("tileset: ");
  if (graphic_in.tileset.type == RPG_GRAPHICS_TILESETTYPE_INVALID)
    result += ACE_TEXT_ALWAYS_CHAR ("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
    result += RPG_Graphics_Common_Tools::toString (graphic_in.tileset);
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR ("elements: ");
  if (graphic_in.elements.empty ())
    result += ACE_TEXT_ALWAYS_CHAR ("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
    result += RPG_Graphics_Common_Tools::toString (graphic_in.elements);
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR ("file: ");
  if (graphic_in.file.empty ())
    result += ACE_TEXT_ALWAYS_CHAR ("N/A\n");
  else
  {
    result += ACE_TEXT_ALWAYS_CHAR ("\"");
    result += graphic_in.file;
    result += ACE_TEXT_ALWAYS_CHAR ("\"\n");
  } // end ELSE

  result += ACE_TEXT_ALWAYS_CHAR ("size: ");
  if (graphic_in.size == 0)
    result += ACE_TEXT_ALWAYS_CHAR ("N/A\n");
  else
  {
    std::ostringstream converter;
    converter << graphic_in.size;
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end ELSE

  return result;
}

RPG_Graphics_GraphicTypeUnion
RPG_Graphics_Common_Tools::styleToType (const struct RPG_Graphics_StyleUnion& style_in,
                                        bool halfHeight_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::styleToType"));

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
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid floor-style (was: \"%s\"), aborting\n"),
                      ACE_TEXT (RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString (style_in.floorstyle).c_str ())));
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
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid edge-style (was: \"%s\"), aborting\n"),
                      ACE_TEXT (RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString (style_in.edgestyle).c_str ())));
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
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_BRICK_HALF
                                                 : TILESET_WALL_BRICK); break;
        case WALLSTYLE_BRICK_BANNER:
          result.tilesetgraphic =
              (halfHeight_in ? TILESET_WALL_BRICK_BANNER_HALF
                             : TILESET_WALL_BRICK_BANNER); break;
        case WALLSTYLE_BRICK_PAINTING:
          result.tilesetgraphic =
              (halfHeight_in ? TILESET_WALL_BRICK_PAINTING_HALF
                             : TILESET_WALL_BRICK_PAINTING); break;
        case WALLSTYLE_BRICK_PILLAR:
          result.tilesetgraphic =
              (halfHeight_in ? TILESET_WALL_BRICK_PILLAR_HALF
                             : TILESET_WALL_BRICK_PILLAR); break;
        case WALLSTYLE_BRICK_POCKET:
          result.tilesetgraphic =
              (halfHeight_in ? TILESET_WALL_BRICK_POCKET_HALF
                             : TILESET_WALL_BRICK_POCKET); break;
        case WALLSTYLE_DARK:
          result.tilesetgraphic =
              (halfHeight_in ? TILESET_WALL_DARK_HALF
                             : TILESET_WALL_DARK); break;
        case WALLSTYLE_LIGHT:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_LIGHT_HALF
                                                 : TILESET_WALL_LIGHT); break;
        case WALLSTYLE_MARBLE:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_MARBLE_HALF
                                                 : TILESET_WALL_MARBLE); break;
        case WALLSTYLE_ROUGH:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_ROUGH_HALF
                                                 : TILESET_WALL_ROUGH); break;
        case WALLSTYLE_STUCCO:
          result.tilesetgraphic = (halfHeight_in ? TILESET_WALL_STUCCO_HALF
                                                 : TILESET_WALL_STUCCO); break;
        case WALLSTYLE_VINE_COVERED:
          result.tilesetgraphic =
              (halfHeight_in ? TILESET_WALL_VINE_COVERED_HALF
                             : TILESET_WALL_VINE_COVERED); break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid wall-style (was: \"%s\"), aborting\n"),
                      ACE_TEXT (RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString (style_in.wallstyle).c_str ())));
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
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("unsupported door-style (was: \"%s\"), aborting\n"),
                      ACE_TEXT (RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString (style_in.doorstyle).c_str ())));
          return result;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid style type (was: %d), aborting\n"),
                  style_in.discriminator));

      return result;
    }
  } // end SWITCH

  result.discriminator = RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC;

  return result;
}

void
RPG_Graphics_Common_Tools::graphicToFile (const RPG_Graphics_t& graphic_in,
                                          std::string& file_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::graphicToFile"));

  // initialize return value(s)
  file_out = myGraphicsDirectory;
  file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  switch (graphic_in.category)
  {
    case CATEGORY_CURSOR:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_CURSORS_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_FONT:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_FONTS_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_INTERFACE:
    case CATEGORY_IMAGE:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_IMAGES_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += graphic_in.file;

      break;
    }
    case CATEGORY_SPRITE:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_CREATURES_SUB);
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
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s --> %s\n"),
                    ACE_TEXT (RPG_Graphics_Common_Tools::toString (current_type).c_str ()),
                    ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.tile.reference).c_str ())));

        current_type = graphic.tile.reference;

        // retrieve properties from the dictionary
        graphic =
            RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (graphic.tile.reference);
      } // end WHILE

      // assemble path
      switch (graphic.tile.type)
      {
        case TILETYPE_FLOOR:
          file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_FLOORS_SUB); break;
        case TILETYPE_WALL:
          file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_WALLS_SUB); break;
        case TILETYPE_DOOR:
          file_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_DOORS_SUB); break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid tile type (was: \"%s\"), aborting\n"),
                      ACE_TEXT (RPG_Graphics_TileTypeHelper::RPG_Graphics_TileTypeToString (graphic.tile.type).c_str ())));
          file_out.clear ();
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
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid category (was: \"%s\"), aborting\n"),
                  ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic_in.category).c_str ())));
      file_out.clear ();
      return;
    }
  } // end SWITCH
}

RPG_Graphics_TextSize_t
RPG_Graphics_Common_Tools::textSize (enum RPG_Graphics_Font font_in,
                                     const std::string& textString_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::textSize"));

  int width = 0;
  int height = 0;

  // synch cache access
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, RPG_Graphics_TextSize_t (0, 0));
    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find (font_in);
    if (iterator == myFontCache.end ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("font \"%s\" not in cache (yet), aborting\n"),
                  ACE_TEXT (RPG_Graphics_FontHelper::RPG_Graphics_FontToString (font_in).c_str ())));
      return RPG_Graphics_TextSize_t (0, 0);
    } // end IF

#if defined (SDL_USE)
    if (TTF_SizeText ((*iterator).second,
                      textString_in.c_str (),
                      &width, &height))
#elif defined (SDL2_USE) || defined (SDL3_USE)
    if (TTF_SizeUTF8 ((*iterator).second,
                      textString_in.c_str (),
                      &width, &height))
#endif // SDL_USE || SDL2_USE
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to TTF_SizeText(\"%s\", \"%s\"): %s, aborting\n"),
                  ACE_TEXT (RPG_Graphics_FontHelper::RPG_Graphics_FontToString (font_in).c_str ()),
                  ACE_TEXT (textString_in.c_str ()),
                  ACE_TEXT (TTF_GetError ())));
      return RPG_Graphics_TextSize_t (0, 0);
    } // end IF
  } // end lock scope

  return std::make_pair (static_cast<unsigned int> (width),
                         static_cast<unsigned int> (height));
}

void
RPG_Graphics_Common_Tools::loadFloorEdgeTileSet (enum RPG_Graphics_EdgeStyle style_in,
                                                 struct RPG_Graphics_FloorEdgeTileSet& tileSet_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::loadFloorEdgeTileSet"));

  // initialize return value(s)
  if (tileSet_out.east.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.east.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.east.surface = NULL;
  } // end IF
  if (tileSet_out.west.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.west.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.west.surface = NULL;
  } // end IF
  if (tileSet_out.north.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.north.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.north.surface = NULL;
  } // end IF
  if (tileSet_out.south.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.south.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.south.surface = NULL;
  } // end IF
  if (tileSet_out.south_west.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.south_west.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.south_west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.south_west.surface = NULL;
  } // end IF
  if (tileSet_out.south_east.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.south_east.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.south_east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.south_east.surface = NULL;
  } // end IF
  if (tileSet_out.north_west.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.north_west.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.north_west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.north_west.surface = NULL;
  } // end IF
  if (tileSet_out.north_east.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.north_east.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.north_east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.north_east.surface = NULL;
  } // end IF

  if (tileSet_out.top.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.top.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.top.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.top.surface = NULL;
  } // end IF
  if (tileSet_out.right.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.right.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.right.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.right.surface = NULL;
  } // end IF
  if (tileSet_out.left.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.left.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.left.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.left.surface = NULL;
  } // end IF
  if (tileSet_out.bottom.surface)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.bottom.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.bottom.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    tileSet_out.bottom.surface = NULL;
  } // end IF

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type =
    RPG_Graphics_Common_Tools::styleToType (style);
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (style).c_str ())));
    return;
  } // end IF

  // step1: retrieve (list of) tiles
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic =
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (graphic_type);
  ACE_ASSERT (graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: \"%s\"), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
                ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_FLOORS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  struct RPG_Graphics_TileElement current_tile;
  unsigned int type = 0;
  std::istringstream converter;
  size_t edge_position = std::string::npos;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin ();
       iterator != graphic.tileset.tiles.end ();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface =
        RPG_Graphics_Surface::load (path,  // file
                                    true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), returning\n"),
                  ACE_TEXT (path.c_str ())));

      // clean up
      if (tileSet_out.east.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.east.surface = NULL;
      } // end IF
      if (tileSet_out.west.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.west.surface = NULL;
      } // end IF
      if (tileSet_out.north.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north.surface = NULL;
      } // end IF
      if (tileSet_out.south.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south.surface = NULL;
      } // end IF
      if (tileSet_out.south_west.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south_west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south_west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south_west.surface = NULL;
      } // end IF
      if (tileSet_out.south_east.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south_east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south_east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south_east.surface = NULL;
      } // end IF
      if (tileSet_out.north_west.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north_west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north_west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north_west.surface = NULL;
      } // end IF
      if (tileSet_out.north_east.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north_east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north_east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north_east.surface = NULL;
      } // end IF

      if (tileSet_out.top.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.top.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.top.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.top.surface = NULL;
      } // end IF
      if (tileSet_out.right.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.right.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.right.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.right.surface = NULL;
      } // end IF
      if (tileSet_out.left.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.left.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.left.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.left.surface = NULL;
      } // end IF
      if (tileSet_out.bottom.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.bottom.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.bottom.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.bottom.surface = NULL;
      } // end IF

      return;
    } // end IF

    // extract position/type from the filename (i.e. "floor_xxx_yyy_zzz_edge#.png")
    converter.clear ();
    converter.str ((*iterator).file);
    edge_position = (*iterator).file.find (ACE_TEXT_ALWAYS_CHAR ("edge"), 7, 4);
    if (edge_position == std::string::npos)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("could not find \"edge\" sequence in filename (was: \"%s\"), returning\n"),
                  ACE_TEXT ((*iterator).file.c_str ())));

      // clean up
      if (tileSet_out.east.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.east.surface = NULL;
      } // end IF
      if (tileSet_out.west.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.west.surface = NULL;
      } // end IF
      if (tileSet_out.north.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north.surface = NULL;
      } // end IF
      if (tileSet_out.south.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south.surface = NULL;
      } // end IF
      if (tileSet_out.south_west.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south_west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south_west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south_west.surface = NULL;
      } // end IF
      if (tileSet_out.south_east.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south_east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south_east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south_east.surface = NULL;
      } // end IF
      if (tileSet_out.north_west.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north_west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north_west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north_west.surface = NULL;
      } // end IF
      if (tileSet_out.north_east.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north_east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north_east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north_east.surface = NULL;
      } // end IF

      if (tileSet_out.top.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.top.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.top.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.top.surface = NULL;
      } // end IF
      if (tileSet_out.right.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.right.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.right.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.right.surface = NULL;
      } // end IF
      if (tileSet_out.left.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.left.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.left.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.left.surface = NULL;
      } // end IF
      if (tileSet_out.bottom.surface)
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.bottom.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.bottom.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.bottom.surface = NULL;
      } // end IF

      return;
    } // end IF
    converter.ignore (edge_position + 4);
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid edgetile type (was: %u), continuing\n"),
                    type));
        break;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded tileset \"%s\" (type: \"%s\", style: \"%s\")...\n"),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
              ACE_TEXT (RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString (graphic.tileset.type).c_str ()),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.tileset.style).c_str ())));
}

void
RPG_Graphics_Common_Tools::loadFloorTileSet (enum RPG_Graphics_FloorStyle style_in,
                                             struct RPG_Graphics_FloorTileSet& tileSet_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::loadFloorTileSet"));

  // initialize return value(s)
  for (RPG_Graphics_FloorTilesConstIterator_t iterator = tileSet_out.tiles.begin ();
       iterator != tileSet_out.tiles.end ();
       iterator++)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface ((*iterator).surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface ((*iterator).surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  tileSet_out.tiles.clear ();
  tileSet_out.columns = 0;
  tileSet_out.rows = 0;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type =
    RPG_Graphics_Common_Tools::styleToType (style);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (style).c_str ())));

    return;
  } // end IF

  // step1: retrieve (list of) tiles
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic =
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (graphic_type);
  ACE_ASSERT (graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: \"%s\"), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
                ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_FLOORS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  struct RPG_Graphics_TileElement current_tile;
  unsigned long column = 0;
  unsigned long row = 0;
  std::istringstream converter;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin ();
       iterator != graphic.tileset.tiles.end ();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface =
        RPG_Graphics_Surface::load (path,  // file
                                    true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), returning\n"),
                  ACE_TEXT (path.c_str ())));

      // clean up
      for (RPG_Graphics_FloorTilesConstIterator_t iterator = tileSet_out.tiles.begin ();
           iterator != tileSet_out.tiles.end ();
           iterator++)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface ((*iterator).surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface ((*iterator).surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      tileSet_out.tiles.clear ();

      return;
    } // end IF

    // extract column/row from the filename (i.e. "floor_xxx_yyy_zzz_..._column#_row#.png")
    converter.clear ();
    converter.str ((*iterator).file);
    converter.ignore ((*iterator).file.size (), '_');
    while (ACE_OS::ace_isdigit (converter.peek ()) == 0)
      converter.ignore ((*iterator).file.size (), '_');
    converter >> column;
    converter.ignore ((*iterator).file.size (), '_');
    converter >> row;

    if (column > tileSet_out.columns)
      tileSet_out.columns = column;
    if (row > tileSet_out.rows)
      tileSet_out.rows = row;
    tileSet_out.tiles.push_back (current_tile);
  } // end FOR
  tileSet_out.columns++; tileSet_out.rows++;
  ACE_ASSERT (tileSet_out.tiles.size () == (tileSet_out.columns * tileSet_out.rows));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s) [%u x %u])...\n"),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
              ACE_TEXT (RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString (graphic.tileset.type).c_str ()),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.tileset.style).c_str ()),
              tileSet_out.tiles.size (),
              tileSet_out.columns,
              tileSet_out.rows));
}

void
RPG_Graphics_Common_Tools::loadWallTileSet (enum RPG_Graphics_WallStyle style_in,
                                            bool halfHeight_in,
                                            struct RPG_Graphics_WallTileSet& tileSet_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::loadWallTileSet"));

  // initialize return value(s)
  if (tileSet_out.west.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.west.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  tileSet_out.west.surface = NULL;
  if (tileSet_out.east.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.east.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  tileSet_out.east.surface = NULL;
  if (tileSet_out.north.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.north.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  tileSet_out.north.surface = NULL;
  if (tileSet_out.south.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.south.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  tileSet_out.south.surface = NULL;
  tileSet_out.west.offset_x = 0;
  tileSet_out.west.offset_y = 0;
  tileSet_out.east.offset_x = 0;
  tileSet_out.east.offset_y = 0;
  tileSet_out.north.offset_x = 0;
  tileSet_out.north.offset_y = 0;
  tileSet_out.south.offset_x = 0;
  tileSet_out.south.offset_y = 0;

  // step0: retrieve appropriate graphic type
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = style_in;
  RPG_Graphics_GraphicTypeUnion graphic_type =
    RPG_Graphics_Common_Tools::styleToType (style, halfHeight_in);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (style).c_str ())));
    return;
  } // end IF

  // step1: retrieve list of tiles
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic =
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (graphic_type);
  ACE_ASSERT (graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: %s), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
                ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_WALLS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  struct RPG_Graphics_TileElement current_tile;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin ();
       iterator != graphic.tileset.tiles.end ();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface =
        RPG_Graphics_Surface::load (path,  // file
                                    true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), returning\n"),
                  ACE_TEXT (path.c_str ())));

      // clean up
      if (tileSet_out.west.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.west.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      tileSet_out.west.surface = NULL;
      if (tileSet_out.east.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.east.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      tileSet_out.east.surface = NULL;
      if (tileSet_out.north.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.north.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      tileSet_out.north.surface = NULL;
      if (tileSet_out.south.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.south.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      tileSet_out.south.surface = NULL;
      tileSet_out.west.offset_x = 0;
      tileSet_out.west.offset_y = 0;
      tileSet_out.east.offset_x = 0;
      tileSet_out.east.offset_y = 0;
      tileSet_out.north.offset_x = 0;
      tileSet_out.north.offset_y = 0;
      tileSet_out.south.offset_x = 0;
      tileSet_out.south.offset_y = 0;

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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid orientation \"%s\", returning\n"),
                    ACE_TEXT (RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString ((*iterator).orientation).c_str ())));

        // clean up
        if (tileSet_out.west.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.west.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.west.surface = NULL;
        if (tileSet_out.east.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.east.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.east.surface = NULL;
        if (tileSet_out.north.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.north.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.north.surface = NULL;
        if (tileSet_out.south.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.south.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        tileSet_out.south.surface = NULL;
        tileSet_out.west.offset_x = 0;
        tileSet_out.west.offset_y = 0;
        tileSet_out.east.offset_x = 0;
        tileSet_out.east.offset_y = 0;
        tileSet_out.north.offset_x = 0;
        tileSet_out.north.offset_y = 0;
        tileSet_out.south.offset_x = 0;
        tileSet_out.south.offset_y = 0;

        return;
      }
    } // end SWITCH
  } // end FOR

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
              ACE_TEXT (RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString (graphic.tileset.type).c_str ()),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.tileset.style).c_str ()),
              4));
}

void
RPG_Graphics_Common_Tools::loadDoorTileSet (enum RPG_Graphics_DoorStyle style_in,
                                            struct RPG_Graphics_DoorTileSet& tileSet_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::loadDoorTileSet"));

  // initialize return value(s)
  if (tileSet_out.horizontal_open.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.horizontal_open.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.horizontal_open.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  if (tileSet_out.vertical_open.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.vertical_open.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.vertical_open.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  if (tileSet_out.horizontal_closed.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.horizontal_closed.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.horizontal_closed.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  if (tileSet_out.vertical_closed.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.vertical_closed.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.vertical_closed.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  if (tileSet_out.broken.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (tileSet_out.broken.surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface (tileSet_out.broken.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
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
  RPG_Graphics_GraphicTypeUnion graphic_type =
    RPG_Graphics_Common_Tools::styleToType (style);
  // sanity check(s)
  if (graphic_type.discriminator == RPG_Graphics_GraphicTypeUnion::INVALID)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::styleToType(\"%s\"), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (style).c_str ())));
    return;
  } // end IF

  // step1: retrieve list of tiles
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic =
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (graphic_type);
  ACE_ASSERT (graphic.type.discriminator == graphic_type.discriminator); // too weak
  // sanity check(s)
  if (graphic.category != CATEGORY_TILESET)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Dictionary::get(\"%s\"): not a tileset (was: %s), returning\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
                ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
    return;
  } // end IF

  // assemble base path
  std::string path_base = myGraphicsDirectory;
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path_base += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DOORS_SUB);
  path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  std::string path = path_base;
  struct RPG_Graphics_TileElement current_tile;
  for (RPG_Graphics_TileSetConstIterator_t iterator = graphic.tileset.tiles.begin ();
       iterator != graphic.tileset.tiles.end ();
       iterator++)
  {
    current_tile.offset_x = (*iterator).offsetX;
    current_tile.offset_y = (*iterator).offsetY;
    current_tile.surface = NULL;

    // load file
    path = path_base;
    path += (*iterator).file;
    current_tile.surface =
        RPG_Graphics_Surface::load (path,  // file
                                    true); // convert to display format
    if (!current_tile.surface)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), returning\n"),
                  ACE_TEXT (path.c_str ())));

      // clean up
      if (tileSet_out.horizontal_open.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.horizontal_open.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.horizontal_open.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (tileSet_out.vertical_open.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.vertical_open.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.vertical_open.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (tileSet_out.horizontal_closed.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.horizontal_closed.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.horizontal_closed.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (tileSet_out.vertical_closed.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.vertical_closed.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.vertical_closed.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (tileSet_out.broken.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (tileSet_out.broken.surface);
#elif defined (SDL3_USE)
        SDL_DestroySurface (tileSet_out.broken.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid orientation \"%s\", returning\n"),
                    ACE_TEXT (RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString ((*iterator).orientation).c_str ())));

        // clean up
        if (tileSet_out.horizontal_open.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.horizontal_open.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.horizontal_open.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        if (tileSet_out.vertical_open.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.vertical_open.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.vertical_open.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        if (tileSet_out.horizontal_closed.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.horizontal_closed.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.horizontal_closed.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        if (tileSet_out.vertical_closed.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.vertical_closed.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.vertical_closed.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        if (tileSet_out.broken.surface)
#if defined (SDL_USE) || defined (SDL2_USE)
          SDL_FreeSurface (tileSet_out.broken.surface);
#elif defined (SDL3_USE)
          SDL_DestroySurface (tileSet_out.broken.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
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

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded tileset \"%s\" (type: %s, style: %s, %u tile(s))...\n"),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ()),
              ACE_TEXT (RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString (graphic.tileset.type).c_str ()),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.tileset.style).c_str ()),
              5));
}

SDL_Surface*
RPG_Graphics_Common_Tools::loadGraphic (const struct RPG_Graphics_GraphicTypeUnion& type_in,
                                        bool convertToDisplayFormat_in,
                                        bool cacheGraphic_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::loadGraphic"));

  RPG_Graphics_GraphicsCacheIterator_t iter;
  struct RPG_Graphics_GraphicsCacheNode node;
  node.type = type_in;
  // initialize return value(s)
  node.image = NULL;

  // step1: graphic already cached ?
  if (cacheGraphic_in)
  {
    // synch access to graphics cache
    { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, NULL);
      for (iter = myGraphicsCache.begin ();
           iter != myGraphicsCache.end ();
           iter++)
        if ((*iter) == node)
          break;

      if (iter != myGraphicsCache.end ())
        return (*iter).image;
    } // end lock scope
  } // end IF

  // step2: load image from file
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic =
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type_in);
  ACE_ASSERT ((graphic.category != RPG_GRAPHICS_CATEGORY_INVALID) && (graphic.type.discriminator == type_in.discriminator)); // too weak
  // sanity check
  if ((graphic.category != CATEGORY_CURSOR)    &&
      (graphic.category != CATEGORY_INTERFACE) &&
      (graphic.category != CATEGORY_IMAGE)     &&
      (graphic.category != CATEGORY_SPRITE)    &&
      (graphic.category != CATEGORY_TILE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid category (was: \"%s\"): \"%s\" not an image type, aborting\n"),
                ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ()),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ())));
    return NULL;
  } // end IF

  // assemble path
  std::string filename;
  RPG_Graphics_Common_Tools::graphicToFile (graphic, filename);
  ACE_ASSERT (!filename.empty ());

  // load file
  node.image =
    RPG_Graphics_Surface::load (filename,                   // file
                                convertToDisplayFormat_in); // convert to display format
  if (!node.image)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return NULL;
  } // end IF

  // step3: update cache
  if (cacheGraphic_in)
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, NULL); // *TODO*: leaks node.image
    if (myGraphicsCache.size () == myCacheSize)
    {
      iter = myGraphicsCache.begin ();
      ACE_ASSERT (myGraphicsCache.size () >= myOldestCacheEntry);
      std::advance (iter, myOldestCacheEntry);
      // *TODO*: what if it's still being used ?...
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface ((*iter).image);
#elif defined (SDL3_USE)
      SDL_DestroySurface ((*iter).image);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myGraphicsCache.erase (iter);
      myOldestCacheEntry++;
      if (myOldestCacheEntry == myCacheSize)
        myOldestCacheEntry = 0;
    } // end IF

    myGraphicsCache.push_back (node);
  } // end IF | lock scope

  return node.image;
}
#if defined (SDL2_USE) || defined (SDL3_USE)
SDL_Texture*
RPG_Graphics_Common_Tools::loadGraphic (SDL_Renderer* renderer_in,
                                        const struct RPG_Graphics_GraphicTypeUnion& type_in/*,
                                        bool cacheGraphic_in*/)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::loadGraphic"));

  RPG_Graphics_GraphicsCacheIterator_t iter;
  struct RPG_Graphics_GraphicsCacheNode node;
  node.type = type_in;
  // initialize return value(s)
  node.image = NULL;
  SDL_Texture* result = NULL;

  // step1: graphic already cached ?
  //if (cacheGraphic_in)
  //{
  //  // synch access to graphics cache
  //  { ACE_Guard<ACE_Thread_Mutex> aGuard (myCacheLock);
  //    for (iter = myGraphicsCache.begin();
  //         iter != myGraphicsCache.end();
  //         iter++)
  //      if ((*iter) == node)
  //        break;

  //    if (iter != myGraphicsCache.end())
  //      return (*iter).image;
  //  } // end lock scope
  //} // end IF

  // step2: load image from file
  // retrieve properties from the dictionary
  RPG_Graphics_t graphic =
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type_in);
  ACE_ASSERT ((graphic.category != RPG_GRAPHICS_CATEGORY_INVALID) && (graphic.type.discriminator == type_in.discriminator)); // too weak
  // sanity check
  if ((graphic.category != CATEGORY_CURSOR)    &&
      (graphic.category != CATEGORY_INTERFACE) &&
      (graphic.category != CATEGORY_IMAGE)     &&
      (graphic.category != CATEGORY_SPRITE)    &&
      (graphic.category != CATEGORY_TILE))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid category (was: \"%s\"): \"%s\" not an image type, aborting\n"),
                ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ()),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (graphic.type).c_str ())));
    return NULL;
  } // end IF

  // assemble path
  std::string filename;
  RPG_Graphics_Common_Tools::graphicToFile (graphic, filename);
  ACE_ASSERT (!filename.empty ());

  // load file
  SDL_Surface* surface_p = RPG_Graphics_Surface::load (filename,
                                                       true); // convert to display format ?
  if (!surface_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str())));
    return NULL;
  } // end IF
  //node.image = RPG_Graphics_Texture::load (filename);
  //if (!node.image)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to RPG_Graphics_Texture::load(\"%s\"), aborting\n"),
  //              ACE_TEXT (filename.c_str())));
  //  return NULL;
  //} // end IF

  //// step3: update cache
  //if (cacheGraphic_in)
  //{
  //  // synch cache access
  //  { ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);
  //    if (myGraphicsCache.size () == myCacheSize)
  //    {
  //      iter = myGraphicsCache.begin ();
  //      ACE_ASSERT(myGraphicsCache.size () >= myOldestCacheEntry);
  //      std::advance (iter, myOldestCacheEntry);
  //      // *TODO*: what if it's still being used ?...
  //      SDL_DestroyTexture((*iter).image);
  //      myGraphicsCache.erase (iter);
  //      myOldestCacheEntry++;
  //      if (myOldestCacheEntry == myCacheSize)
  //        myOldestCacheEntry = 0;
  //    } // end IF
  //    myGraphicsCache.push_back (node);
  //  } // end lock scope
  //} // end IF

  //return node.image;

  result = SDL_CreateTextureFromSurface (renderer_in,
                                         surface_p);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateTextureFromSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (surface_p);
#elif defined (SDL3_USE)
    SDL_DestroySurface (surface_p);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    return NULL;
  } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_FreeSurface (surface_p);
#elif defined (SDL3_USE)
  SDL_DestroySurface (surface_p);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  surface_p = NULL;

  return result;
}
#endif // SDL2_USE || SDL3_USE

SDL_Surface*
RPG_Graphics_Common_Tools::renderText (enum RPG_Graphics_Font font_in,
                                       const std::string& textString_in,
                                       const struct SDL_Color& fgColor_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::renderText"));

  // initialize return value(s)
  SDL_Surface* result = NULL;

  // synch cache access
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, NULL);
    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find (font_in);
    ACE_ASSERT (iterator != myFontCache.end ());
#if defined (SDL_USE)
    result = TTF_RenderText_Blended ((*iterator).second,
                                     textString_in.c_str (),
                                     fgColor_in);
#elif defined (SDL2_USE) || defined (SDL3_USE)
    result = TTF_RenderUTF8_Blended ((*iterator).second,
                                     textString_in.c_str (),
                                     fgColor_in);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  } // end lock scope
  if (!result)
  {
#if defined (SDL_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to TTF_RenderText_Blended(\"%s\"): %s, aborting\n"),
                ACE_TEXT (textString_in.c_str ()),
                ACE_TEXT (SDL_GetError ())));
#elif defined (SDL2_USE) || defined (SDL3_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to TTF_RenderUTF8_Blended(\"%s\"): %s, aborting\n"),
                ACE_TEXT (textString_in.c_str ()),
                ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
    return NULL;
  } // end IF

  return result;
}

#if defined (SDL_USE)
void
RPG_Graphics_Common_Tools::fade (bool fadeIn_in,
                                 float interval_in,
                                 Uint32 color_in,
                                 Common_ILock* screenLock_in,
                                 SDL_Surface* targetSurface_in)
#elif defined (SDL2_USE) || defined (SDL3_USE)
void
RPG_Graphics_Common_Tools::fade (bool fadeIn_in,
                                 float interval_in,
                                 Uint32 color_in,
                                 Common_ILock* screenLock_in,
                                 SDL_Window* targetWindow_in)
#endif // SDL_USE || SDL2_USE || SDL3_USE
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::fade"));

  // sanity check(s)
#if defined (SDL_USE)
  SDL_Surface* surface_p = targetSurface_in;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (targetWindow_in);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (surface_p);

  // step1: create a screen-sized surface without an alpha-channel
  // --> i.e. (alpha mask == 0)
  SDL_Surface* target_image =
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_CreateRGBSurface (RPG_Graphics_Surface::SDL_surface_flags,
                          surface_p->w,
                          surface_p->h,
                          surface_p->format->BitsPerPixel,
                          surface_p->format->Rmask,
                          surface_p->format->Gmask,
                          surface_p->format->Bmask,
                          0);
#elif defined (SDL3_USE)
    SDL_CreateSurface (surface_p->w,
                       surface_p->h,
                       SDL_GetPixelFormatEnumForMasks (surface_p->format->bits_per_pixel,
                                                       surface_p->format->Rmask,
                                                       surface_p->format->Gmask,
                                                       surface_p->format->Bmask,
                                                       surface_p->format->Amask));
#endif // SDL_USE || SDL2_USE || SDL3_USE
  if (!target_image)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateRGBSurface(): %s, returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  if (fadeIn_in)
  {
    // ...copy the pixel data from the framebuffer
    if (SDL_BlitSurface (surface_p,
                         NULL,
                         target_image,
                         NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_BlitSurface(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (target_image);
#elif defined (SDL3_USE)
      SDL_DestroySurface (target_image);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      return;
    } // end IF

    // set the screen to the background color (black ?)
    if (screenLock_in)
      screenLock_in->lock ();
#if defined (SDL_USE) || defined (SDL2_USE)
    if (SDL_FillRect (surface_p, // target surface
                      NULL,      // fill screen
                      color_in)) // black ?
#elif defined (SDL3_USE)
    if (SDL_FillSurfaceRect (surface_p, // target surface
                             NULL,      // fill screen
                             color_in)) // black ?
#endif // SDL_USE || SDL2_USE || SDL3_USE
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_FillRect(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (target_image);
#elif defined (SDL3_USE)
      SDL_DestroySurface (target_image);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      return;
    } // end IF
    if (screenLock_in)
      screenLock_in->unlock ();
    // ...and display that
#if defined (SDL_USE)
    if (SDL_Flip (surface_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_Flip(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));
      SDL_FreeSurface (target_image);
      return;
    } // end IF
#elif defined (SDL2_USE) || defined (SDL3_USE)
    if (SDL_UpdateWindowSurface (targetWindow_in) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_UpdateWindowSurface(%@): \"%s\", continuing\n"),
                  targetWindow_in,
                  ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
  } // end IF
  else
  {
    // fill the target image with the requested color
#if defined (SDL_USE) || defined (SDL2_USE)
    if (SDL_FillRect (target_image, // target image
                      NULL,         // fill image
                      color_in))    // target color
#elif defined (SDL3_USE)
    if (SDL_FillSurfaceRect (target_image, // target image
                             NULL,         // fill image
                             color_in))    // target color
#endif // SDL_USE || SDL2_USE || SDL3_USE
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_FillRect(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (target_image);
#elif defined (SDL3_USE)
      SDL_DestroySurface (target_image);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      return;
    } // end IF
  } // end ELSE

  // step4: slowly fade in/out
#if defined (SDL_USE)
  fade (interval_in,
        target_image,
        screenLock_in,
        targetSurface_in);
#elif defined (SDL2_USE)
  fade (interval_in,
        target_image,
        screenLock_in,
        targetWindow_in);
#endif // SDL_USE || SDL2_USE

  // clean up
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_FreeSurface (target_image);
#elif defined (SDL3_USE)
  SDL_DestroySurface (target_image);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  target_image = NULL;
}

RPG_Graphics_Style
RPG_Graphics_Common_Tools::random (const struct RPG_Graphics_Style& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::random"));

  struct RPG_Graphics_Style result;

  // step1: door-style
  RPG_Dice_RollResult_t roll_result;
  if (style_in.door == RPG_GRAPHICS_DOORSTYLE_INVALID)
  {
    roll_result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_DOORSTYLE_MAX,
                                     1,
                                     roll_result);
    result.door =
        static_cast<enum RPG_Graphics_DoorStyle> (roll_result.front () - 1);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("generated level door-style: \"%s\"\n"),
                ACE_TEXT (RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString (result.door).c_str ())));
  } // end IF
  else
    result.door = style_in.door;

  // step2: edge-style
  if (style_in.edge == RPG_GRAPHICS_EDGESTYLE_INVALID)
  {
    roll_result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_EDGESTYLE_MAX,
                                     1,
                                     roll_result);
    result.edge =
        static_cast<RPG_Graphics_EdgeStyle> (roll_result.front () - 1);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("generated level edge-style: \"%s\"\n"),
                ACE_TEXT (RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString (result.edge).c_str ())));
  } // end IF
  else
    result.edge = style_in.edge;

  // step3: floor-style
  if (style_in.floor == RPG_GRAPHICS_FLOORSTYLE_INVALID)
  {
    roll_result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_FLOORSTYLE_MAX,
                                     1,
                                     roll_result);
    result.floor =
        static_cast<RPG_Graphics_FloorStyle> (roll_result.front () - 1);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("generated level floor-style: \"%s\"\n"),
                ACE_TEXT (RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString (result.floor).c_str ())));
  } // end IF
  else
    result.floor = style_in.floor;

//  // step4: half-height walls
//  result.half_height_walls = RPG_Dice::probability(0.5F);
//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("generated level half-height walls: %s\n"),
//             (result.half_height_walls ? ACE_TEXT_ALWAYS_CHAR("true")
//                                       : ACE_TEXT_ALWAYS_CHAR("false"))));

  // step5: wall-style
  if (style_in.wall == RPG_GRAPHICS_WALLSTYLE_INVALID)
  {
    roll_result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_WALLSTYLE_MAX,
                                     1,
                                     roll_result);
    result.wall =
        static_cast<RPG_Graphics_WallStyle> (roll_result.front () - 1);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("generated level wall-style: \"%s\"\n"),
                ACE_TEXT (RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString (result.wall).c_str ())));
  } // end IF
  else
    result.wall = style_in.wall;

  return result;
}

void
RPG_Graphics_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::initStringConversionTables"));

  RPG_Graphics_CategoryHelper::init ();
  RPG_Graphics_OrientationHelper::init ();
  RPG_Graphics_TileTypeHelper::init ();
  RPG_Graphics_FloorStyleHelper::init ();
  RPG_Graphics_EdgeStyleHelper::init ();
  RPG_Graphics_StairsStyleHelper::init ();
  RPG_Graphics_WallStyleHelper::init ();
  RPG_Graphics_DoorStyleHelper::init ();
  RPG_Graphics_CursorHelper::init ();
  RPG_Graphics_FontHelper::init ();
  RPG_Graphics_ImageHelper::init ();
  RPG_Graphics_SpriteHelper::init ();
  RPG_Graphics_TileGraphicHelper::init ();
  RPG_Graphics_TileSetGraphicHelper::init ();
  RPG_Graphics_WindowTypeHelper::init ();
  RPG_Graphics_InterfaceElementTypeHelper::init ();
  RPG_Graphics_HotspotTypeHelper::init ();
  RPG_Graphics_TileSetTypeHelper::init ();
  // ---------------------------------------------------------------------------
  RPG_Graphics_ColorNameHelper::init ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Graphics_Common_Tools: initialized string conversion tables...\n")));
}

bool
RPG_Graphics_Common_Tools::initializeFonts ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::initializeFonts"));

  // step1: retrieve list of configured fonts
  RPG_Graphics_Fonts_t fonts =
      RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->getFonts ();

  // step2: load all fonts into the cache
  std::string path = myGraphicsDirectory;
  TTF_Font* font = NULL;

  // synch cache access
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, false);
    for (RPG_Graphics_FontsConstIterator_t iterator = fonts.begin ();
         iterator != fonts.end ();
         iterator++)
    {
      font = NULL;

      // construct FQ filename
      path = myGraphicsDirectory;
      path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_FONTS_SUB);
      path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      path += (*iterator).file;
      // sanity check
      if (!Common_File_Tools::isReadable (path))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid file(\"%s\"): not readable, aborting\n"),
                    ACE_TEXT (path.c_str ())));

        // clean up
        for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin ();
             iter != myFontCache.end ();
             iter++)
          TTF_CloseFont ((*iter).second);
        myFontCache.clear ();

        return false;
      } // end IF

      font = TTF_OpenFont (path.c_str (),     // filename
                           (*iterator).size); // point size
      if (!font)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to TTF_OpenFont(\"%s\", %u): %s, aborting\n"),
                    ACE_TEXT (path.c_str ()),
                    (*iterator).size,
                    ACE_TEXT (SDL_GetError ())));

        // clean up
        for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin ();
             iter != myFontCache.end ();
             iter++)
          TTF_CloseFont ((*iter).second);
        myFontCache.clear ();

        return false;
      } // end IF

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("loaded font \"%s - %s\" %d face(s) [%s], height [+/-]: %d (%d,%d), space: %d\n"),
                  ACE_TEXT (TTF_FontFaceFamilyName (font)),
                  ACE_TEXT (TTF_FontFaceStyleName (font)),
                  ACE_TEXT (TTF_FontFaces (font)),
                  (TTF_FontFaceIsFixedWidth (font) ? ACE_TEXT ("fixed") : ACE_TEXT ("variable")),
                  TTF_FontHeight (font),
                  TTF_FontAscent (font),
                  TTF_FontDescent (font),
                  TTF_FontLineSkip (font)));

      // cache this font
      myFontCache.insert (std::make_pair ((*iterator).type, font));
    } // end FOR
  } // end lock scope

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Graphics_Common_Tools: loaded %u font(s)...\n"),
              fonts.size ()));

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
//                  ACE_TEXT(SDL_GetError())));
//
//       return;
//     } // end IF
//     // *NOTE*: only change the framebuffer !
//     if (SDL_Flip(screen_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                  ACE_TEXT(SDL_GetError())));
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
//                  ACE_TEXT(SDL_GetError())));
//
//       return;
//     } // end IF
//     if (SDL_Flip(screen_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                  ACE_TEXT(SDL_GetError())));
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
//                ACE_TEXT(SDL_GetError())));
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
//                ACE_TEXT(SDL_GetError())));
//
//     return;
//   } // end IF
//   if (SDL_Flip(screen_in))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                ACE_TEXT(SDL_GetError())));
//
//     return;
//   } // end IF
// }

#if defined (SDL_USE)
void
RPG_Graphics_Common_Tools::fade (float interval_in,
                                 SDL_Surface* targetImage_in,
                                 Common_ILock* screenLock_in,
                                 SDL_Surface* targetSurface_in)
#elif defined (SDL2_USE) || defined (SDL3_USE)
void
RPG_Graphics_Common_Tools::fade (float interval_in,
                                 SDL_Surface* targetImage_in,
                                 Common_ILock* screenLock_in,
                                 SDL_Window* targetWindow_in)
#endif // SDL_USE || SDL2_USE || SDL3_USE
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::fade"));

  // sanity check(s)
#if defined (SDL_USE)
  SDL_Surface* surface_p = targetSurface_in;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (targetWindow_in);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (surface_p);

  // calculate the number of blends
  int n_steps = static_cast<int> (RPG_GRAPHICS_FADE_REFRESH_RATE * interval_in);
#if defined (SDL_USE)
  if (SDL_SetAlpha (targetImage_in,
                    (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
                    (SDL_ALPHA_OPAQUE / n_steps)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetAlpha(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
#elif defined (SDL2_USE) || defined (SDL3_USE)
  if (SDL_SetSurfaceAlphaMod (targetImage_in,
                              (SDL_ALPHA_OPAQUE / n_steps)) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetSurfaceAlphaMod(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
#endif // SDL_USE || SDL2_USE || SDL3_USE

  Uint32 cur_clock, end_clock, start_clock, sleeptime_ms, elapsed;
  start_clock = cur_clock = SDL_GetTicks ();
  end_clock = start_clock + static_cast<int> (interval_in * 1000);
  while (cur_clock <= end_clock)
  {
    if (screenLock_in)
      screenLock_in->lock ();
    // *NOTE*: blitting the image onto the screen repeatedly
    // will slowly add up to full brightness, while
    // drawing over the screen with semi-transparent
    // darkness repeatedly gives a fade-out effect
    if (SDL_BlitSurface (targetImage_in,
                         NULL,
                         surface_p,
                         NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_BlitSurface(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF
    if (screenLock_in)
      screenLock_in->unlock ();
#if defined (SDL_USE)
    if (SDL_Flip (surface_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_Flip(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF
#elif defined (SDL2_USE) || defined (SDL3_USE)
    if (SDL_UpdateWindowSurface (targetWindow_in) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_UpdateWindowSurface(%@): \"%s\", continuing\n"),
                  targetWindow_in,
                  ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE

    // delay a little while, to impress the blended image
    elapsed = SDL_GetTicks () - cur_clock;
    sleeptime_ms = ((1000 / RPG_GRAPHICS_FADE_REFRESH_RATE) > elapsed) ? ((1000 / RPG_GRAPHICS_FADE_REFRESH_RATE) - elapsed)
                                                                       : 0;
    SDL_Delay (sleeptime_ms);

    cur_clock += (elapsed + sleeptime_ms);
  } // end WHILE

  // ensure that the target image is fully faded in
#if defined (SDL_USE)
  if (SDL_SetAlpha (targetImage_in,
                    (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
                    255))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetAlpha(): %s, returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
#elif defined (SDL2_USE) || defined (SDL3_USE)
  if (SDL_SetSurfaceAlphaMod (targetImage_in,
                              255) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetSurfaceAlphaMod(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
#endif // SDL_USE || SDL2_USE || SDL3_USE

  if (screenLock_in)
    screenLock_in->lock ();
  if (SDL_BlitSurface (targetImage_in,
                       NULL,
                       surface_p,
                       NULL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_BlitSurface(): %s, returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  if (screenLock_in)
    screenLock_in->unlock ();
#if defined (SDL_USE)
  if (SDL_Flip (surface_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Flip(): %s, returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
#elif defined (SDL2_USE) || defined (SDL3_USE)
  if (SDL_UpdateWindowSurface (targetWindow_in) < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_UpdateWindowSurface(%@): \"%s\", continuing\n"),
                targetWindow_in,
                ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
}

RPG_Graphics_Position_t
RPG_Graphics_Common_Tools::screenToMap (const RPG_Graphics_Position_t& position_in,
                                        const RPG_Map_Size_t& mapSize_in,
                                        const RPG_Graphics_Size_t& windowSize_in,
                                        const RPG_Graphics_Position_t& viewport_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::screenToMap"));

  RPG_Graphics_Position_t offset, map_position;

  offset.first =
      position_in.first         -
      (windowSize_in.first / 2) +
      (viewport_in.first - viewport_in.second) * RPG_GRAPHICS_TILE_WIDTH_MOD;
  offset.second =
      position_in.second         -
      (windowSize_in.second / 2) +
      (viewport_in.first + viewport_in.second) * RPG_GRAPHICS_TILE_HEIGHT_MOD;

  map_position.first =
      (RPG_GRAPHICS_TILE_HEIGHT_MOD * offset.first +
       RPG_GRAPHICS_TILE_WIDTH_MOD  * offset.second +
       RPG_GRAPHICS_TILE_WIDTH_MOD  * RPG_GRAPHICS_TILE_HEIGHT_MOD) /
      (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD);
  map_position.second =
      (-RPG_GRAPHICS_TILE_HEIGHT_MOD * offset.first +
        RPG_GRAPHICS_TILE_WIDTH_MOD  * offset.second +
        RPG_GRAPHICS_TILE_WIDTH_MOD  * RPG_GRAPHICS_TILE_HEIGHT_MOD) /
      (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD);

  map_position.first -= 1;

  // sanity check: off-map position ?
  if ((map_position.first  >= mapSize_in.first) ||
      (map_position.second >= mapSize_in.second))
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT("screen coords [%u,%u] --> [%u,%u] --> off-map, continuing\n"),
    //            position_in.first, position_in.second,
    //            map_position.first, map_position.second));

    map_position.first = std::numeric_limits<unsigned int>::max ();
    map_position.second = std::numeric_limits<unsigned int>::max ();
  } // end IF

  return map_position;
}

RPG_Graphics_Offset_t
RPG_Graphics_Common_Tools::mapToScreen (const RPG_Graphics_Position_t& position_in,
                                        const RPG_Graphics_Size_t& windowSize_in,
                                        const RPG_Graphics_Position_t& viewport_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Common_Tools::mapToScreen"));

  // initialize return value(s)
  RPG_Graphics_Offset_t result =
      std::make_pair (std::numeric_limits<int>::max (),
                      std::numeric_limits<int>::max ());

  RPG_Graphics_Position_t map_center =
    std::make_pair (windowSize_in.first / 2,
                    windowSize_in.second / 2);
  result.first = map_center.first +
                 (RPG_GRAPHICS_TILE_WIDTH_MOD *
                  (position_in.first -
                   position_in.second +
                   viewport_in.second -
                   viewport_in.first));
  result.second = map_center.second +
                  (RPG_GRAPHICS_TILE_HEIGHT_MOD *
                   (position_in.first +
                    position_in.second -
                    viewport_in.second -
                    viewport_in.first));

  // sanity check(s)
  if ((result.first  >= static_cast<int> (windowSize_in.first)) ||
      (result.second >= static_cast<int> (windowSize_in.second)))
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("map coords [%u,%u] --> [%d,%d] --> off-screen, continuing\n"),
    //            position_in.first, position_in.second,
    //            result.first, result.second));

    result = std::make_pair (std::numeric_limits<int>::max (),
                             std::numeric_limits<int>::max ());
  } // end IF

  return result;
}
