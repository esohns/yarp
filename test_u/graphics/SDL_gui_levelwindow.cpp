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
#include "SDL_gui_levelwindow.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_map_common_tools.h>

#include <ace/Log_Msg.h>

SDL_GUI_LevelWindow::SDL_GUI_LevelWindow(const RPG_Graphics_SDLWindow& parent_in,
                                         const RPG_Graphics_InterfaceWindow_t& type_in,
                                         const RPG_Graphics_FloorStyle& floorStyle_in,
                                         const RPG_Graphics_WallStyle& wallStyle_in,
                                         const RPG_Graphics_DoorStyle& doorStyle_in,
                                         const RPG_Map_FloorPlan_t& floorPlan_in)
 : inherited(parent_in,
             std::make_pair(0, 0),
             type_in),
   myMap(floorPlan_in),
   myCurrentFloorStyle(RPG_GRAPHICS_FLOORSTYLE_INVALID),
//    myCurrentFloorSet(),
   myCurrentWallStyle(RPG_GRAPHICS_WALLSTYLE_INVALID),
//    myCurrentWallSet(),
   myCurrentDoorStyle(RPG_GRAPHICS_DOORSTYLE_INVALID),
//    myCurrentDoorSet(),
   myCurrentOffMapTile(NULL),
//    myWallTiles(),
   myView(std::make_pair(floorPlan_in.size_x / 2,
                         floorPlan_in.size_y / 2))
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::SDL_GUI_LevelWindow"));

  myCurrentWallSet.east = NULL;
  myCurrentWallSet.west = NULL;
  myCurrentWallSet.north = NULL;
  myCurrentWallSet.south = NULL;

  myCurrentDoorSet.horizontal_open   = NULL;
  myCurrentDoorSet.vertical_open     = NULL;
  myCurrentDoorSet.horizontal_closed = NULL;
  myCurrentDoorSet.vertical_closed   = NULL;
  myCurrentDoorSet.broken            = NULL;

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = floorStyle_in;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = wallStyle_in;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = doorStyle_in;
  setStyle(style);
  // load tile for unmapped areas
  myCurrentOffMapTile = RPG_Graphics_Common_Tools::loadGraphic(TYPE_TILE_OFF_MAP, // tile
                                                               false);            // don't cache
  if (!myCurrentOffMapTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(TYPE_TILE_OFF_MAP).c_str()));

  // init wall tiles / position
  initWalls(floorPlan_in,
            myCurrentWallSet,
            myWallTiles);

  // init door tiles / position
  initDoors(floorPlan_in,
            myMap,
            myCurrentDoorSet,
            myDoorTiles);
}

SDL_GUI_LevelWindow::~SDL_GUI_LevelWindow()
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::~SDL_GUI_LevelWindow"));

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
  if (myCurrentDoorSet.horizontal_open)
    SDL_FreeSurface(myCurrentDoorSet.horizontal_open);
  if (myCurrentDoorSet.vertical_open)
    SDL_FreeSurface(myCurrentDoorSet.vertical_open);
  if (myCurrentDoorSet.horizontal_closed)
    SDL_FreeSurface(myCurrentDoorSet.horizontal_closed);
  if (myCurrentDoorSet.vertical_closed)
    SDL_FreeSurface(myCurrentDoorSet.vertical_closed);
  if (myCurrentDoorSet.broken)
    SDL_FreeSurface(myCurrentDoorSet.broken);
  myCurrentDoorSet.horizontal_open = NULL;
  myCurrentDoorSet.vertical_open = NULL;
  myCurrentDoorSet.horizontal_closed = NULL;
  myCurrentDoorSet.vertical_closed = NULL;
  myCurrentDoorSet.broken = NULL;
  myCurrentDoorStyle = RPG_GRAPHICS_DOORSTYLE_INVALID;
  if (myCurrentOffMapTile)
  {
    SDL_FreeSurface(myCurrentOffMapTile);
    myCurrentOffMapTile = NULL;
  } // end IF
}

void
SDL_GUI_LevelWindow::setView(const RPG_Graphics_Position_t& view_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::setView"));

  myView = view_in;
}

void
SDL_GUI_LevelWindow::init(const RPG_Graphics_FloorStyle& floorStyle_in,
                          const RPG_Graphics_WallStyle& wallStyle_in,
                          const RPG_Graphics_DoorStyle& doorStyle_in,
                          const RPG_Map_FloorPlan_t& floorPlan_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::init"));

  // clean up
  myWallTiles.clear();

  myMap.init(floorPlan_in);

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = floorStyle_in;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = wallStyle_in;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = doorStyle_in;
  setStyle(style);

  // init wall tiles / position
  initWalls(floorPlan_in,
            myCurrentWallSet,
            myWallTiles);

  // init door tiles / position
  initDoors(floorPlan_in,
            myMap,
            myCurrentDoorSet,
            myDoorTiles);

  // init view
  myView = std::make_pair(floorPlan_in.size_x / 2,
                          floorPlan_in.size_y / 2);
}

void
SDL_GUI_LevelWindow::draw(SDL_Surface* targetSurface_in,
                          const RPG_Graphics_Position_t& offset_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::draw"));

  // sanity check(s)
  ACE_ASSERT(inherited::myInitialized);
  ACE_ASSERT(myCurrentOffMapTile);
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offset_in.first) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offset_in.second) <= targetSurface_in->h);

  // init clipping
  SDL_Rect clipRect;
  clipRect.x = offset_in.first + myBorderLeft + myOffset.first;
  clipRect.y = offset_in.second + myBorderTop + myOffset.second;
  clipRect.w = (targetSurface_in->w - offset_in.first - (myBorderLeft + myBorderRight) - myOffset.first);
  clipRect.h = (targetSurface_in->h - offset_in.second - (myBorderTop + myBorderBottom) - myOffset.second);
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // position of the top right corner
  RPG_Graphics_Position_t position_tr = std::make_pair(0, 0);
  position_tr.first = (((-RPG_GRAPHICS_MAPTILE_SIZE_Y * ((targetSurface_in->w / 2) + 50)) +
                        (RPG_GRAPHICS_MAPTILE_SIZE_X * ((targetSurface_in->h / 2) + 50)) +
                        (RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y)) /
                       (2 * RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y));
  position_tr.second = (((RPG_GRAPHICS_MAPTILE_SIZE_Y * ((targetSurface_in->w / 2) + 50)) +
                         (RPG_GRAPHICS_MAPTILE_SIZE_X * ((targetSurface_in->h / 2) + 50)) +
                         (RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y)) /
                        (2 * RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y));

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
  unsigned long x, y;
  // pass 1
  for (i = -position_tr.second;
       i <= ACE_static_cast(int, position_tr.second);
       i++)
  {
    current_map_position.second = myView.second + i;
    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;

      // transform map coordinates into screen coordinates
      x = (targetSurface_in->w / 2) + (RPG_GRAPHICS_MAPTILE_SIZE_X * (j - i));
      y = (targetSurface_in->h / 2) + (RPG_GRAPHICS_MAPTILE_SIZE_Y * (j + i));

      // off the map ?
      if ((myMap.getElement(current_map_position) == MAPELEMENT_UNMAPPED) ||
          // *NOTE*: walls are drawn together with the floor...
          (myMap.getElement(current_map_position) == MAPELEMENT_WALL) ||
          ((current_map_position.second < 0) ||
           (current_map_position.second >= myMap.getDimensions().second) ||
           (current_map_position.first < 0) ||
           (current_map_position.first >= myMap.getDimensions().first)))
      {
        RPG_Graphics_Surface::put(x,
                                  y,
                                  *myCurrentOffMapTile,
                                  targetSurface_in);

        continue;
      } // end IF

      // step1: floor
      if ((myMap.getElement(current_map_position) == MAPELEMENT_FLOOR) ||
          (myMap.getElement(current_map_position) == MAPELEMENT_DOOR))
      {
        RPG_Graphics_Surface::put(x,
                                  y,
                                  *myCurrentFloorSet.front(),
                                  targetSurface_in);
      } // end IF
    } // end FOR
  } // end FOR

  // pass 2
  RPG_Graphics_WallTileMapConstIterator_t wall_iterator = myWallTiles.end();
  RPG_Graphics_DoorTileMapConstIterator_t door_iterator = myDoorTiles.end();
  for (i = -position_tr.second;
       i <= ACE_static_cast(int, position_tr.second);
       i++)
  {
    current_map_position.second = myView.second + i;
    // off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= myMap.getDimensions().second))
      continue;

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;
      // off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= myMap.getDimensions().first))
        continue;

      // transform map coordinates into screen coordinates
      x = (targetSurface_in->w / 2) + (RPG_GRAPHICS_MAPTILE_SIZE_X * (j - i));
      y = (targetSurface_in->h / 2) + (RPG_GRAPHICS_MAPTILE_SIZE_Y * (j + i));

      wall_iterator = myWallTiles.find(current_map_position);
      door_iterator = myDoorTiles.find(current_map_position);
      // step1: walls (west & north)
      if (wall_iterator != myWallTiles.end())
      {
        if ((*wall_iterator).second.west)
        {
          // *NOTE*: west is just a "darkened" version of east...
          RPG_Graphics_Surface::put(x,
                                    y - (*wall_iterator).second.west->h + (myCurrentFloorSet.front()->h / 2),
                                    *(myCurrentWallSet.east),
                                    targetSurface_in);
//           RPG_Graphics_Common_Tools::put(x,
//                                          y - (*wall_iterator).second.west->h + (myCurrentFloorSet.front()->h / 2),
//                                          *(*wall_iterator).second.west,
//                                          targetSurface_in);
        } // end IF
        if ((*wall_iterator).second.north)
        {
          // *NOTE*: north is just a "darkened" version of south...
          RPG_Graphics_Surface::put(x + (myCurrentFloorSet.front()->w / 2),
                                    y - (*wall_iterator).second.north->h + (myCurrentFloorSet.front()->h / 2),
                                    *(myCurrentWallSet.south),
                                    targetSurface_in);
//           RPG_Graphics_Common_Tools::put(x + (myCurrentFloorSet.front()->w / 2),
//                                          y - (*wall_iterator).second.north->h + (myCurrentFloorSet.front()->h / 2),
//                                          *(*wall_iterator).second.north,
//                                          targetSurface_in);
        } // end IF
      } // end IF

      // step2: doors
      if (door_iterator != myDoorTiles.end())
      {
        RPG_Graphics_Surface::put(x + (myCurrentFloorSet.front()->w / 4),
                                  y - (*door_iterator).second->h + (myCurrentDoorSet.horizontal_open->h / 2) - (myCurrentFloorSet.front()->h / 4),
                                  *(*door_iterator).second,
                                  targetSurface_in);
      } // end IF

      // step3: walls (south & east)
      if (wall_iterator != myWallTiles.end())
      {
        if ((*wall_iterator).second.south)
          RPG_Graphics_Surface::put(x,
                                    y - (*wall_iterator).second.south->h + myCurrentFloorSet.front()->h,
                                    *(*wall_iterator).second.south,
                                    targetSurface_in);
        if ((*wall_iterator).second.east)
          RPG_Graphics_Surface::put(x + (myCurrentFloorSet.front()->w / 2),
                                    y - (*wall_iterator).second.east->h + myCurrentFloorSet.front()->h,
                                    *(*wall_iterator).second.east,
                                    targetSurface_in);
      } // end IF
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
SDL_GUI_LevelWindow::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::setStyle"));

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
    case RPG_Graphics_StyleUnion::DOORSTYLE:
    {
      // clean up
      if (myCurrentDoorSet.horizontal_open)
        SDL_FreeSurface(myCurrentDoorSet.horizontal_open);
      if (myCurrentDoorSet.vertical_open)
        SDL_FreeSurface(myCurrentDoorSet.vertical_open);
      if (myCurrentDoorSet.horizontal_closed)
        SDL_FreeSurface(myCurrentDoorSet.horizontal_closed);
      if (myCurrentDoorSet.vertical_closed)
        SDL_FreeSurface(myCurrentDoorSet.vertical_closed);
      if (myCurrentDoorSet.broken)
        SDL_FreeSurface(myCurrentDoorSet.broken);
      myCurrentDoorSet.horizontal_open = NULL;
      myCurrentDoorSet.vertical_open = NULL;
      myCurrentDoorSet.horizontal_closed = NULL;
      myCurrentDoorSet.vertical_closed = NULL;
      myCurrentDoorSet.broken = NULL;

      RPG_Graphics_Common_Tools::loadDoorTileSet(style_in.doorstyle,
                                                 myCurrentDoorSet);
      // sanity check
      if ((myCurrentDoorSet.horizontal_open == NULL) ||
          (myCurrentDoorSet.vertical_open == NULL) ||
          (myCurrentDoorSet.horizontal_closed == NULL) ||
          (myCurrentDoorSet.vertical_closed == NULL) ||
          (myCurrentDoorSet.broken == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("door-style \"%s\" is incomplete, continuing\n"),
                   RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle).c_str()));
      } // end IF
      myCurrentDoorStyle = style_in.doorstyle;

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
SDL_GUI_LevelWindow::initWalls(const RPG_Map_FloorPlan_t& levelMap_in,
                               const RPG_Graphics_WallTileSet_t& tileSet_in,
                               RPG_Graphics_WallTileMap_t& wallTiles_out)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::initWalls"));

  // init return value(s)
  wallTiles_out.clear();

  RPG_Map_Position_t current_position;
  RPG_Map_Position_t east, north, west, south;
  RPG_Graphics_WallTileSet_t current_walls;
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

      if ((myMap.getElement(current_position) == MAPELEMENT_FLOOR) ||
          (myMap.getElement(current_position) == MAPELEMENT_DOOR))
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

        if (current_walls.east ||
            current_walls.north ||
            current_walls.west ||
            current_walls.south)
          myWallTiles.insert(std::make_pair(current_position, current_walls));
      } // end IF
    } // end FOR
}

void
SDL_GUI_LevelWindow::initDoors(const RPG_Map_FloorPlan_t& levelMap_in,
                               const RPG_Map_Level& levelState_in,
                               const RPG_Graphics_DoorTileSet_t& tileSet_in,
                               RPG_Graphics_DoorTileMap_t& doorTiles_out)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::initDoors"));

  // init return value(s)
  doorTiles_out.clear();

  SDL_Surface* current = NULL;
  RPG_Map_Door_t current_door;
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  for (RPG_Map_PositionsConstIterator_t iterator = levelMap_in.doors.begin();
       iterator != levelMap_in.doors.end();
       iterator++)
  {
    current = NULL;
    current_door = levelState_in.getDoor(*iterator);
    orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
    if (current_door.is_broken)
    {
      current = tileSet_in.broken;
      continue;
    } // end IF

    orientation = SDL_GUI_LevelWindow::getDoorOrientation(levelMap_in,
                                                          *iterator);
    switch (orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        current = (current_door.is_open ? tileSet_in.horizontal_open
                                        : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current = (current_door.is_open ? tileSet_in.vertical_open
                                        : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door orientation \"%s\", aborting\n"),
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));

        // clean up
        myDoorTiles.clear();

        return;
      }
    } // end SWITCH

    myDoorTiles.insert(std::make_pair(*iterator, current));
  } // end FOR
}

const RPG_Graphics_Orientation
SDL_GUI_LevelWindow::getDoorOrientation(const RPG_Map_Level& level_in,
                                        const RPG_Map_Position_t& position_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::getDoorOrientation"));

//   RPG_Graphics_Orientation result = RPG_GRAPHICS_ORIENTATION_INVALID;

  RPG_Map_Position_t east;//, south;
  east = position_in;
  east.first++;
//   south = position_in;
//   south.second++;

  if (level_in.getElement(east) == MAPELEMENT_WALL)
    return ORIENTATION_HORIZONTAL;

  return ORIENTATION_VERTICAL;
}

const RPG_Graphics_Position_t
SDL_GUI_LevelWindow::screen2Map(const RPG_Graphics_Position_t& position_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::screen2Map"));

  RPG_Graphics_Position_t offset, map_position;

  offset.first = (position_in.first - (mySize.first / 2) + ((myView.first - myView.second) * RPG_GRAPHICS_MAPTILE_SIZE_X));
  offset.second = (position_in.second - (mySize.second / 2) + ((myView.first + myView.second) * RPG_GRAPHICS_MAPTILE_SIZE_Y));

  map_position.first = ((RPG_GRAPHICS_MAPTILE_SIZE_Y * offset.first) +
                        (RPG_GRAPHICS_MAPTILE_SIZE_X * offset.second) +
                        (RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y)) /
                       (2 * RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y);
  map_position.second = ((-RPG_GRAPHICS_MAPTILE_SIZE_Y * offset.first) +
                         (RPG_GRAPHICS_MAPTILE_SIZE_X * offset.second) +
                         (RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y)) /
                        (2 * RPG_GRAPHICS_MAPTILE_SIZE_X * RPG_GRAPHICS_MAPTILE_SIZE_Y);

  return map_position;
}

const RPG_Graphics_Position_t
SDL_GUI_LevelWindow::map2Screen(const RPG_Graphics_Position_t& position_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::map2Screen"));

  RPG_Graphics_Position_t map_center, screen_position;

  map_center.first = mySize.first / 2;
  map_center.second = mySize.second / 2;

  screen_position.first = map_center.first +
                          (RPG_GRAPHICS_MAPTILE_SIZE_X *
                           (position_in.first - position_in.second + myView.second - myView.first));
  screen_position.second = map_center.second +
                           (RPG_GRAPHICS_MAPTILE_SIZE_Y *
                            (position_in.first + position_in.second - myView.second - myView.first));

  // *TODO* fix underruns (why does this happen ?)
  return screen_position;
}
