/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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
#include "rpg_client_window_level.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_common_tools.h>

#include <ace/Log_Msg.h>

RPG_Client_WindowLevel::RPG_Client_WindowLevel(const RPG_Graphics_SDLWindow& parent_in,
                                               const RPG_Graphics_InterfaceWindow_t& type_in,
                                               const RPG_Graphics_Type& graphicType_in)
 : inherited(parent_in,
             type_in,
             graphicType_in),
   myInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::RPG_Client_WindowLevel"));

  myCurrentFloorSet.style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  myCurrentWallSet.style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;

  // init view
  myView = std::make_pair(0, 0);

  // init wall tiles
  myWallTiles.clear();

  // init level properties
  myMap.floorStyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myMap.wallStyle = RPG_GRAPHICS_WALLSTYLE_INVALID;
  myMap.plan.size_x = 0;
  myMap.plan.size_y = 0;
  myMap.plan.unmapped.clear();
  myMap.plan.walls.clear();
  myMap.plan.doors.clear();
}

RPG_Client_WindowLevel::~RPG_Client_WindowLevel()
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::~RPG_Client_WindowLevel"));

}

void
RPG_Client_WindowLevel::setView(const RPG_Graphics_Position_t& view_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setView"));

  myView = view_in;
}

void
RPG_Client_WindowLevel::setMap(const RPG_Client_DungeonLevel& levelMap_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setMap"));

  if (!myInitialized)
    myInitialized = true;
  else
  {
    // clean up
    myWallTiles.clear();
  } // end ELSE

  myMap = levelMap_in;

  // init view
  myView.first = myMap.plan.size_x / 2;
  myView.second = myMap.plan.size_y / 2;

  // init wall tiles
  initWalls(myMap.plan,
            myCurrentWallSet,
            myWallTiles);
}

void
RPG_Client_WindowLevel::draw(SDL_Surface* targetSurface_in,
                             const RPG_Graphics_Position_t& offset_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::draw"));

  // sanity check(s)
  ACE_ASSERT(myInitialized);
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offset_in.first) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offset_in.second) <= targetSurface_in->h);

  // step1: load (floor- and wall-)style tilesets
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = myMap.floorStyle;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = myMap.wallStyle;
  setStyle(style);

  // step2: load tile for unmapped areas
  SDL_Surface* offmap_surface = NULL;
  offmap_surface = RPG_Graphics_Common_Tools::loadGraphic(TYPE_TILE_OFF_MAP,
                                                          true);
  if (!offmap_surface)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(TYPE_TILE_OFF_MAP).c_str()));

    return;
  } // end IF

  // step3: init clipping
  SDL_Rect clipRect;
  clipRect.x = offset_in.first;
  clipRect.y = offset_in.second;
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = myBorderTop;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // position of the top right corner
  RPG_Graphics_Position_t position_tr = std::make_pair(0, 0);
  position_tr.first = (((-RPG_GRAPHICS_V_MAP_YMOD * ((targetSurface_in->w / 2) + 50)) +
                        (RPG_GRAPHICS_V_MAP_XMOD * ((targetSurface_in->h / 2) + 50)) +
                        (RPG_GRAPHICS_V_MAP_XMOD * RPG_GRAPHICS_V_MAP_YMOD)) /
                       (2 * RPG_GRAPHICS_V_MAP_XMOD * RPG_GRAPHICS_V_MAP_YMOD));
  position_tr.second = (((RPG_GRAPHICS_V_MAP_YMOD * ((targetSurface_in->w / 2) + 50)) +
                         (RPG_GRAPHICS_V_MAP_XMOD * ((targetSurface_in->h / 2) + 50)) +
                         (RPG_GRAPHICS_V_MAP_XMOD * RPG_GRAPHICS_V_MAP_YMOD)) /
                        (2 * RPG_GRAPHICS_V_MAP_XMOD * RPG_GRAPHICS_V_MAP_YMOD));

  // *NOTE*: without the "+-1" small corners within the viewport are not drawn
  int diff = 0;
  int sum = 0;
  diff = position_tr.first - position_tr.second - 1;
  sum  = position_tr.first + position_tr.second + 1;

  // draw tiles
  // pass 1:
  //   1. floor
  //   2. floor edges
  //
  // pass 2:
  //   1. north & west walls
  //   2. furniture
  //   3. traps
  //   4. objects
  //   5. monsters
  //   6. effects
  //   7. south & east walls

  int i, j;
  RPG_Map_Position_t current_map_position = std::make_pair(0, 0);
//   RPG_Map_PositionsConstIterator_t iterator;
  unsigned long x, y;
  for (i = -position_tr.second;
       i <= position_tr.second;
       i++)
  {
    current_map_position.second = myView.second + i;
    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;

      // transform map coordinates into screen coordinates
      x = (targetSurface_in->w / 2) + (RPG_GRAPHICS_V_MAP_XMOD * (j - i));
      y = (targetSurface_in->h / 2) + (RPG_GRAPHICS_V_MAP_YMOD * (j + i));

      // off the map ?
      // *TODO*: n < 0 ?
      if (((current_map_position.second < 1) ||
           (current_map_position.second >= myMap.plan.size_y) ||
           (current_map_position.first < 0) ||
           (current_map_position.first >= myMap.plan.size_x)) ||
          ((myMap.plan.walls.find(current_map_position) == myMap.plan.walls.end()) &&
           (myMap.plan.doors.find(current_map_position) == myMap.plan.doors.end())))
      {
        RPG_Graphics_Common_Tools::put(x,
                                       y,
                                       *offmap_surface,
                                       targetSurface_in);

        continue;
      } // end IF

//       map_back = map_data->get_glyph(MAP_BACK, j, i);
//       map_darkness = map_data->get_glyph(MAP_DARKNESS, j, i);
//
//       /* 0. init walls and floor edges for this tile*/
//       if (map_back == V_TILE_WALL_GENERIC ||
//           map_back == V_MISC_UNMAPPED_AREA)
//         get_wall_tiles(i, j);
//       else
//         /* certain events (amnesia or a secret door being discovered)
//         * require us to clear walls again. since we cannot check for those cases
//         * we simply clear walls whenever we don't set any... */
//         clear_walls(i, j);
//
//       if (map_back == V_TILE_FLOOR_WATER ||
//           map_back == V_TILE_FLOOR_ICE ||
//           map_back == V_TILE_FLOOR_LAVA ||
//           map_back == V_TILE_FLOOR_AIR)
//         /* these tiles get edges */
//         get_floor_edges(i, j);
//       else
//         /* everything else doesn't. However we can't just assume a tile that doesn't need egdes doesn't have any:
//         * pools may be dried out by fireballs, and suddenly we have a pit with edges :(
//         * Therefore we always clear them explicitly */
//         clear_floor_edges(i ,j);

      // step1: floor
      if (RPG_Map_Common_Tools::isFloor(current_map_position,
                                        myMap.plan))
      {
        RPG_Graphics_Common_Tools::put(x,
                                       y,
                                       *myCurrentFloorSet.front(),
                                       targetSurface_in);

        // step2: walls
        if (RPG_Map_Common_Tools::isFloor(current_map_position,
                                          myMap.plan))
        {
          RPG_Graphics_Common_Tools::put(x,
                                         y,
                                         *myCurrentFloorSet.front(),
                                             targetSurface_in);

          continue;
        } // end IF
        continue;
      } // end IF

//       tile_id = map_back;
//       shadelevel = 0;
//
//       if ((tile_id >= V_TILE_FLOOR_COBBLESTONE) &&
//            (tile_id <= V_TILE_FLOOR_DARK)) {
//         tile_id = get_floor_tile(tile_id, i, j);
//         shadelevel = map_darkness;
//            }
//            else if(tile_id == V_TILE_NONE || tile_id == V_TILE_WALL_GENERIC)
//              tile_id = V_MISC_UNMAPPED_AREA;
//
//            vultures_put_tile_shaded(x, y, tile_id, shadelevel);
//
//            /* shortcut for unmapped case */
//            if (tile_id == V_MISC_UNMAPPED_AREA)
//              continue;
//
//            if (vultures_opts.highlight_cursor_square &&
//                (j == map_highlight.x && i == map_highlight.y))
//              vultures_put_tile(x, y, V_MISC_FLOOR_HIGHLIGHT);
//
//            /* 3. Floor edges */
//            for (dir_idx = 0; dir_idx < 12; dir_idx++)
//              vultures_put_tile(x, y, maptile_floor_edge[i][j].dir[dir_idx]);

  for (__i = - map_tr_y; __i <= map_tr_y; __i++)
  {
    i = view_y + __i;
    if (i < 0 || i >= ROWNO)
      continue;

    for (__j = diff + __i; __j + __i <= sum; __j++)
    {
      j = view_x + __j;
      if (j < 1 || j >= COLNO)
        continue;
      /* Find position of tile centre */
      x = map_centre_x + V_MAP_XMOD*(__j - __i);
      y = map_centre_y + V_MAP_YMOD*(__j + __i);

      map_back = map_data->get_glyph(MAP_BACK, j, i);
      map_obj = map_data->get_glyph(MAP_OBJ, j, i);
      map_mon = map_data->get_glyph(MAP_MON, j, i);

      /* 1. West and north walls */
      if (j > 1)
        vultures_put_tile_shaded(x, y, maptile_wall[i][j].west,
                                 map_data->get_glyph(MAP_DARKNESS, j-1, i));
      if (i > 1)
        vultures_put_tile_shaded(x, y, maptile_wall[i][j].north,
                                 map_data->get_glyph(MAP_DARKNESS, j, i-1));

      /* shortcut for unmapped case */
      if (map_back != V_MISC_UNMAPPED_AREA ||
          map_obj != V_TILE_NONE) {
        /* 2. Furniture*/
        vultures_put_tile(x, y, map_data->get_glyph(MAP_FURNITURE, j, i));


        /* 3. Traps */
        vultures_put_tile(x, y, map_data->get_glyph(MAP_TRAP, j, i));


        /* 4. Objects */
        vultures_put_tile(x, y, map_obj);


        /* 5. Monsters */
        if ((cur_tile = vultures_get_tile(map_mon)) != NULL) {
          vultures_put_tile(x, y, map_mon);
          if (iflags.hilite_pet && map_data->get_glyph(MAP_PET, j, i))
            vultures_put_img(x + cur_tile->xmod, y + cur_tile->ymod - 10,
                             vultures_get_tile(V_MISC_HILITE_PET)->graphic);
        }

        /* 6. Effects */
        vultures_put_tile(x, y, map_data->get_glyph(MAP_SPECIAL, j, i));
          }

          /* 7. South & East walls */
          if (i < ROWNO - 1)
            vultures_put_tile_shaded(x, y, maptile_wall[i][j].south,
                                     map_data->get_glyph(MAP_DARKNESS, j, i+1));
          if (j < COLNO - 1)
            vultures_put_tile_shaded(x, y, maptile_wall[i][j].east,
                                     map_data->get_glyph(MAP_DARKNESS, j+1, i));
    }
  }

  /* draw object highlights if requested */
  if (vultures_map_highlight_objects)
  {
    for (__i = - map_tr_y; __i <= map_tr_y; __i++)
    {
      i = view_y + __i;
      if (i < 0 || i >= ROWNO)
        continue;

      for (__j = diff + __i; __j + __i <= sum; __j++)
      {
        j = view_x + __j;
        if (j < 1 || j >= COLNO)
          continue;

        x = map_centre_x + V_MAP_XMOD*(__j - __i);
        y = map_centre_y + V_MAP_YMOD*(__j + __i);

        vultures_put_tilehighlight(x, y, map_data->get_glyph(MAP_OBJ, j, i));
      }
    }
  }
  /* Restore drawing region */
  vultures_set_draw_region(0, 0, vultures_screen->w-1, vultures_screen->h-1);

  vultures_invalidate_region(clip_tl_x, clip_tl_y,
                             clip_br_x - clip_tl_x,
                             clip_br_y - clip_tl_y);

  clip_tl_x = 999999;
  clip_tl_y = 999999;
  clip_br_x = 0;
  clip_br_y = 0;

  vultures_tilecache_age();

  vultures_map_draw_msecs = SDL_GetTicks() - startticks;
  vultures_map_draw_lastmove = moves;

  return true;

  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = offset_in.first + myBorderLeft;
       i < (ACE_static_cast(unsigned long, targetSurface_in->w) - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Common_Tools::put(i,
                                   offset_in.second,
                                   *(*iterator).second,
                                   targetSurface_in);

  // reset clipping area
  clipRect.x = 0;
  clipRect.y = 0;
  clipRect.w = targetSurface_in->w;
  clipRect.h = targetSurface_in->h;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // whole window needs a refresh...
  myDirtyRegions.push_back(clipRect);
}

void
RPG_Client_WindowLevel::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setStyle"));

  RPG_Graphics_Type current = RPG_GRAPHICS_TYPE_INVALID;
  unsigned long numTiles = RPG_GRAPHICS_TILES_DEF_NUMSETTILES;
  RPG_Graphics_Tiles_t* tileset = NULL;
  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    {
      // init current set
      myCurrentFloorSet.style.floorstyle = style_in.floorstyle;
      tileset = &myCurrentFloorSet.tiles;
      switch (myCurrentFloorSet.style.floorstyle)
      {
        case FLOORSTYLE_AIR:
        {
          current = TYPE_TILE_FLOOR_AIR_1;

          break;
        }
        case FLOORSTYLE_DARK:
        {
          current = TYPE_TILE_FLOOR_DARK_1;

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid floor-style: \"%s\", aborting\n"),
                     RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(myCurrentFloorSet.style.floorstyle).c_str()));

          return;
        }
      } // end SWITCH

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      // init current set
      myCurrentWallSet.style.wallstyle = style_in.wallstyle;
      tileset = &myCurrentWallSet.tiles;
      switch (myCurrentWallSet.style.wallstyle)
      {
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid wall-style: \"%s\", aborting\n"),
                     RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(myCurrentWallSet.style.wallstyle).c_str()));

          return;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style (was: %d), aborting\n"),
                 style_in.discriminator));

      return;
    }
  } // end SWITCH

  // load appropriate graphics
  RPG_Client_WindowLevel::loadTileset(current,
                                      numTiles,
                                      *tileset);
}

void
RPG_Client_WindowLevel::loadTileset(const RPG_Graphics_Type& typeOffset_in,
                                    const unsigned long& numTiles_in,
                                    RPG_Graphics_Tiles_t& tileset_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::loadTileset"));

  // init return value(s)
  tileset_out.clear();

  SDL_Surface* current_surface = NULL;
  int current_type = typeOffset_in;
  for (unsigned long i = 0;
       i < numTiles_in;
       i++, current_type++)
  {
    current_surface = NULL;
    current_surface = RPG_Graphics_Common_Tools::loadGraphic(ACE_static_cast(RPG_Graphics_Type, current_type),
                                                             true);
    if (!current_surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
                 RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(ACE_static_cast(RPG_Graphics_Type, current_type)).c_str()));

      // clean up
      tileset_out.clear();

      return;
    } // end IF

    tileset_out.push_back(current_surface);
  } // end FOR
}

void
RPG_Client_WindowLevel::initWalls(const RPG_Map_FloorPlan_t& levelMap_in,
                                  const RPG_Graphics_TileSet_t& tileSet_in,
                                  RPG_Client_WallTiles_t& wallTiles_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::initWalls"));

  // init return value(s)
  wallTiles_out.clear();

  RPG_Map_Position_t current_position;
  RPG_Map_Position_t east, north, west, south;
  for (unsigned long y = 0;
       y < levelMap_in.size_y;
       y++)
    for (unsigned long x = 0;
         x < levelMap_in.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      if (RPG_Map_Common_Tools::isFloor(current_position,
                                        levelMap_in))
      {
        // step1: find neighboring walls
        east = current_position;
        east.first++;
        north = current_position;
        north.second--;
        west = current_position;
        west.first--;
        south = current_position;
        south.second++;


      } // end IF

    } // end FOR

  SDL_Surface* current_surface = NULL;
  int current_type = typeOffset_in;
  for (unsigned long i = 0;
       i < numTiles_in;
       i++, current_type++)
  {
    current_surface = NULL;
    current_surface = RPG_Graphics_Common_Tools::loadGraphic(ACE_static_cast(RPG_Graphics_Type, current_type),
        true);
    if (!current_surface)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
                          RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(ACE_static_cast(RPG_Graphics_Type, current_type)).c_str()));

      // clean up
      tileset_out.clear();

      return;
    } // end IF

    tileset_out.push_back(current_surface);
  } // end FOR
}