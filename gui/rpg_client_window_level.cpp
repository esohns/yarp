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

#include <rpg_map_common_tools.h>

#include <ace/Log_Msg.h>

RPG_Client_WindowLevel::RPG_Client_WindowLevel(const RPG_Graphics_SDLWindow& parent_in,
                                               const RPG_Graphics_InterfaceWindow_t& type_in,
                                               const RPG_Client_DungeonLevel& level_in)
 : inherited(parent_in,
             type_in),
   myMap(level_in.plan),
   myCurrentFloorStyle(RPG_GRAPHICS_FLOORSTYLE_INVALID),
//    myCurrentFloorSet(),
   myCurrentWallStyle(RPG_GRAPHICS_WALLSTYLE_INVALID),
//    myCurrentWallSet(),
   myCurrentOffMapTile(NULL),
//    myWallTiles(),
   myView(std::make_pair(0, 0))
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::RPG_Client_WindowLevel"));

  myCurrentWallSet.east = NULL;
  myCurrentWallSet.west = NULL;
  myCurrentWallSet.north = NULL;
  myCurrentWallSet.south = NULL;

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = level_in.floorStyle;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = level_in.wallStyle;
  setStyle(style);
  // load tile for unmapped areas
  myCurrentOffMapTile = RPG_Graphics_Common_Tools::loadGraphic(TYPE_TILE_OFF_MAP, // tile
                                                               false);            // don't cache
  if (!myCurrentOffMapTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(TYPE_TILE_OFF_MAP).c_str()));

  // init wall tiles / position
  initWalls(level_in.plan,
            myCurrentWallSet,
            myWallTiles);
}

RPG_Client_WindowLevel::~RPG_Client_WindowLevel()
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::~RPG_Client_WindowLevel"));

  // clean up
  for (RPG_Graphics_FloorTileSetConstIterator_t iterator = myCurrentFloorSet.begin();
       iterator != myCurrentFloorSet.end();
       iterator++)
    SDL_FreeSurface(*iterator);
  myCurrentFloorSet.clear();
  myCurrentFloorStyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  if (myCurrentWallSet.east)
    SDL_FreeSurface(myCurrentWallSet.east);
  if (myCurrentWallSet.west)
    SDL_FreeSurface(myCurrentWallSet.west);
  if (myCurrentWallSet.north)
    SDL_FreeSurface(myCurrentWallSet.north);
  if (myCurrentWallSet.south)
    SDL_FreeSurface(myCurrentWallSet.south);
  myCurrentWallSet.east = NULL;
  myCurrentWallSet.west = NULL;
  myCurrentWallSet.north = NULL;
  myCurrentWallSet.south = NULL;
  myCurrentWallStyle = RPG_GRAPHICS_WALLSTYLE_INVALID;
  if (myCurrentOffMapTile)
  {
    SDL_FreeSurface(myCurrentOffMapTile);
    myCurrentOffMapTile = NULL;
  } // end IF
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

  // clean up
  myWallTiles.clear();

  myMap.init(levelMap_in.plan);

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = levelMap_in.floorStyle;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = levelMap_in.wallStyle;
  setStyle(style);

  // init wall tiles / position
  initWalls(levelMap_in.plan,
            myCurrentWallSet,
            myWallTiles);

  // init view
  myView = std::make_pair(0, 0);
}

void
RPG_Client_WindowLevel::draw(SDL_Surface* targetSurface_in,
                             const RPG_Graphics_Position_t& offset_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::draw"));

  // sanity check(s)
  ACE_ASSERT(inherited::myInitialized);
  ACE_ASSERT(myCurrentOffMapTile);
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offset_in.first) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offset_in.second) <= targetSurface_in->h);

  // init clipping
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
  RPG_Client_WallTilesConstIterator_t iterator;
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
           (current_map_position.second >= myMap.getDimensions().second) ||
           (current_map_position.first < 0) ||
           (current_map_position.first >= myMap.getDimensions().first)) ||
          ((myMap.getElement(current_map_position) != MAPELEMENT_WALL) &&
           (myMap.getElement(current_map_position) != MAPELEMENT_DOOR)))
      {
        RPG_Graphics_Common_Tools::put(x,
                                       y,
                                       *myCurrentOffMapTile,
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
      if (myMap.getElement(current_map_position) == MAPELEMENT_FLOOR)
      {
        RPG_Graphics_Common_Tools::put(x,
                                       y,
                                       *myCurrentFloorSet.front(),
                                       targetSurface_in);

        // step2: walls (west & north)
        iterator = myWallTiles.find(current_map_position);
        ACE_ASSERT(iterator != myWallTiles.end());
        if ((*iterator).second.west)
          RPG_Graphics_Common_Tools::put(x,
                                         y,
                                         *(*iterator).second.west,
                                         targetSurface_in);
        if ((*iterator).second.north)
          RPG_Graphics_Common_Tools::put(x,
                                         y,
                                         *(*iterator).second.north,
                                         targetSurface_in);
        // south & east
        if ((*iterator).second.south)
          RPG_Graphics_Common_Tools::put(x,
                                         y,
                                         *(*iterator).second.south,
                                         targetSurface_in);
        if ((*iterator).second.east)
          RPG_Graphics_Common_Tools::put(x,
                                         y,
                                         *(*iterator).second.east,
                                         targetSurface_in);
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

//   for (__i = - map_tr_y; __i <= map_tr_y; __i++)
//   {
//     i = view_y + __i;
//     if (i < 0 || i >= ROWNO)
//       continue;
//
//     for (__j = diff + __i; __j + __i <= sum; __j++)
//     {
//       j = view_x + __j;
//       if (j < 1 || j >= COLNO)
//         continue;
//       /* Find position of tile centre */
//       x = map_centre_x + V_MAP_XMOD*(__j - __i);
//       y = map_centre_y + V_MAP_YMOD*(__j + __i);
//
//       map_back = map_data->get_glyph(MAP_BACK, j, i);
//       map_obj = map_data->get_glyph(MAP_OBJ, j, i);
//       map_mon = map_data->get_glyph(MAP_MON, j, i);
//
//       /* 1. West and north walls */
//       if (j > 1)
//         vultures_put_tile_shaded(x, y, maptile_wall[i][j].west,
//                                  map_data->get_glyph(MAP_DARKNESS, j-1, i));
//       if (i > 1)
//         vultures_put_tile_shaded(x, y, maptile_wall[i][j].north,
//                                  map_data->get_glyph(MAP_DARKNESS, j, i-1));
//
//       /* shortcut for unmapped case */
//       if (map_back != V_MISC_UNMAPPED_AREA ||
//           map_obj != V_TILE_NONE) {
//         /* 2. Furniture*/
//         vultures_put_tile(x, y, map_data->get_glyph(MAP_FURNITURE, j, i));
//
//
//         /* 3. Traps */
//         vultures_put_tile(x, y, map_data->get_glyph(MAP_TRAP, j, i));
//
//
//         /* 4. Objects */
//         vultures_put_tile(x, y, map_obj);
//
//
//         /* 5. Monsters */
//         if ((cur_tile = vultures_get_tile(map_mon)) != NULL) {
//           vultures_put_tile(x, y, map_mon);
//           if (iflags.hilite_pet && map_data->get_glyph(MAP_PET, j, i))
//             vultures_put_img(x + cur_tile->xmod, y + cur_tile->ymod - 10,
//                              vultures_get_tile(V_MISC_HILITE_PET)->graphic);
//         }
//
//         /* 6. Effects */
//         vultures_put_tile(x, y, map_data->get_glyph(MAP_SPECIAL, j, i));
//           }
//
//           /* 7. South & East walls */
//           if (i < ROWNO - 1)
//             vultures_put_tile_shaded(x, y, maptile_wall[i][j].south,
//                                      map_data->get_glyph(MAP_DARKNESS, j, i+1));
//           if (j < COLNO - 1)
//             vultures_put_tile_shaded(x, y, maptile_wall[i][j].east,
//                                      map_data->get_glyph(MAP_DARKNESS, j+1, i));
//     }
//   }
//
//   /* draw object highlights if requested */
//   if (vultures_map_highlight_objects)
//   {
//     for (__i = - map_tr_y; __i <= map_tr_y; __i++)
//     {
//       i = view_y + __i;
//       if (i < 0 || i >= ROWNO)
//         continue;
//
//       for (__j = diff + __i; __j + __i <= sum; __j++)
//       {
//         j = view_x + __j;
//         if (j < 1 || j >= COLNO)
//           continue;
//
//         x = map_centre_x + V_MAP_XMOD*(__j - __i);
//         y = map_centre_y + V_MAP_YMOD*(__j + __i);
//
//         vultures_put_tilehighlight(x, y, map_data->get_glyph(MAP_OBJ, j, i));
//       }
    } // end FOR
  } // end FOR

  // whole viewport needs a refresh...
  myDirtyRegions.push_back(clipRect);

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
}

void
RPG_Client_WindowLevel::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setStyle"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    {
      // clean up
      for (RPG_Graphics_FloorTileSetConstIterator_t iterator = myCurrentFloorSet.begin();
           iterator != myCurrentFloorSet.end();
           iterator++)
        SDL_FreeSurface(*iterator);
      myCurrentFloorSet.clear();

      RPG_Graphics_Common_Tools::loadFloorTileSet(style_in.floorstyle,
                                                  myCurrentFloorSet);
      // sanity check
      if (myCurrentFloorSet.empty())
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("floor-style \"%s\" has no tiles, continuing\n"),
                   RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle).c_str()));
      } // end IF
      myCurrentFloorStyle = style_in.floorstyle;

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      // clean up
      if (myCurrentWallSet.east)
        SDL_FreeSurface(myCurrentWallSet.east);
      if (myCurrentWallSet.west)
        SDL_FreeSurface(myCurrentWallSet.west);
      if (myCurrentWallSet.north)
        SDL_FreeSurface(myCurrentWallSet.north);
      if (myCurrentWallSet.south)
        SDL_FreeSurface(myCurrentWallSet.south);
      myCurrentWallSet.east = NULL;
      myCurrentWallSet.west = NULL;
      myCurrentWallSet.north = NULL;
      myCurrentWallSet.south = NULL;

      RPG_Graphics_Common_Tools::loadWallTileSet(style_in.wallstyle,
                                                 myCurrentWallSet);
      // sanity check
      if ((myCurrentWallSet.east == NULL) ||
          (myCurrentWallSet.west == NULL) ||
          (myCurrentWallSet.north == NULL) ||
          (myCurrentWallSet.south == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("wall-style \"%s\" is incomplete, continuing\n"),
                   RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle).c_str()));
      } // end IF
      myCurrentWallStyle = style_in.wallstyle;

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
}

void
RPG_Client_WindowLevel::initWalls(const RPG_Map_FloorPlan_t& levelMap_in,
                                  const RPG_Graphics_WallTileSet_t& tileSet_in,
                                  RPG_Client_WallTiles_t& wallTiles_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowLevel::initWalls"));

  // init return value(s)
  wallTiles_out.clear();

  RPG_Map_Position_t current_position;
  RPG_Map_Position_t east, north, west, south;
  RPG_Client_WallTileSet current_walls;
  for (unsigned long y = 0;
       y < levelMap_in.size_y;
       y++)
    for (unsigned long x = 0;
         x < levelMap_in.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      current_walls.east = NULL;
      current_walls.west = NULL;
      current_walls.north = NULL;
      current_walls.south = NULL;

      if (myMap.getElement(current_position) == MAPELEMENT_FLOOR)
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

        if (myMap.getElement(east) == MAPELEMENT_WALL)
          current_walls.east = myCurrentWallSet.east;
        if (myMap.getElement(west) == MAPELEMENT_WALL)
          current_walls.west = myCurrentWallSet.west;
        if (myMap.getElement(north) == MAPELEMENT_WALL)
          current_walls.north = myCurrentWallSet.north;
        if (myMap.getElement(south) == MAPELEMENT_WALL)
          current_walls.south = myCurrentWallSet.south;
      } // end IF

      myWallTiles.insert(std::make_pair(current_position, current_walls));
    } // end FOR
}
