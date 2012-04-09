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

#include "rpg_client_common_tools.h"

#include <rpg_engine_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_cursor_manager.h>

#include <rpg_map_common_tools.h>

#include <rpg_common_macros.h>

void
RPG_Client_Common_Tools::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::init"));

  // step1: init cursor manager singleton
  try
  {
    RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set(CURSOR_NORMAL);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_Cursor_Manager::set, returning\n")));

    return;
  }
}

void
RPG_Client_Common_Tools::initFloorEdges(const RPG_Map_FloorPlan_t& floorPlan_in,
                                        const RPG_Graphics_FloorEdgeTileSet_t& tileSet_in,
                                        RPG_Graphics_FloorEdgeTileMap_t& floorEdgeTiles_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initFloorEdges"));

  // init return value(s)
  floorEdgeTiles_out.clear();

  RPG_Map_Position_t current_position;
  RPG_Map_Position_t east, north, west, south;
  RPG_Map_Position_t north_east, north_west, south_east, south_west;
  RPG_Graphics_FloorEdgeTileSet_t current_floor_edges;
  RPG_Map_Door_t position_door;
  for (unsigned long y = 0;
       y < floorPlan_in.size_y;
       y++)
    for (unsigned long x = 0;
         x < floorPlan_in.size_x;
         x++)
  {
    current_position = std::make_pair(x, y);
    ACE_OS::memset(&current_floor_edges,
                  0,
                  sizeof(current_floor_edges));

    position_door.position = current_position;
    // floor or door ? --> compute floor edges
    if (RPG_Map_Common_Tools::isFloor(current_position, floorPlan_in) ||
        (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end()))
    {
      // step1: find neighboring map elements
      east = current_position;
      east.first++;
      north = current_position;
      north.second--;
      west = current_position;
      west.first--;
      south = current_position;
      south.second++;
      north_east = north;
      north_east.first++;
      north_west = north;
      north_west.first--;
      south_east = south;
      south_east.first++;
      south_west = south;
      south_west.first--;

      if (floorPlan_in.walls.find(east) != floorPlan_in.walls.end())
      {
        current_floor_edges.east = tileSet_in.east;
        if (floorPlan_in.walls.find(north) != floorPlan_in.walls.end())
          current_floor_edges.north_east = tileSet_in.north_east;
        if (floorPlan_in.walls.find(south) != floorPlan_in.walls.end())
          current_floor_edges.south_east = tileSet_in.south_east;
      } // end IF
      else
      {
        // right corner ?
        if ((floorPlan_in.walls.find(north) == floorPlan_in.walls.end()) &&
          (floorPlan_in.walls.find(north_east) != floorPlan_in.walls.end()))
          current_floor_edges.right = tileSet_in.right;
      } // end ELSE
      if ((floorPlan_in.walls.find(west) != floorPlan_in.walls.end()))
      {
        current_floor_edges.west = tileSet_in.west;
        if (floorPlan_in.walls.find(north) != floorPlan_in.walls.end())
          current_floor_edges.north_west = tileSet_in.north_west;
        if (floorPlan_in.walls.find(south) != floorPlan_in.walls.end())
          current_floor_edges.south_west = tileSet_in.south_west;
      } // end IF
      else
      {
        // left corner ?
        if ((floorPlan_in.walls.find(south) == floorPlan_in.walls.end()) &&
            (floorPlan_in.walls.find(south_west) != floorPlan_in.walls.end()))
          current_floor_edges.left = tileSet_in.left;
      } // end ELSE
      if ((floorPlan_in.walls.find(north) != floorPlan_in.walls.end()))
        current_floor_edges.north = tileSet_in.north;
      else
      {
        // top corner ?
        if ((floorPlan_in.walls.find(west) == floorPlan_in.walls.end()) &&
            (floorPlan_in.walls.find(north_west) != floorPlan_in.walls.end()))
          current_floor_edges.top = tileSet_in.top;
      } // end ELSE
      if ((floorPlan_in.walls.find(south) != floorPlan_in.walls.end()))
        current_floor_edges.south = tileSet_in.south;
      else
      {
        // bottom corner ?
        if ((floorPlan_in.walls.find(east) == floorPlan_in.walls.end()) &&
            (floorPlan_in.walls.find(south_east) != floorPlan_in.walls.end()))
          current_floor_edges.bottom = tileSet_in.bottom;
      } // end ELSE

      if (current_floor_edges.east.surface ||
          current_floor_edges.north.surface ||
          current_floor_edges.west.surface ||
          current_floor_edges.south.surface ||
          current_floor_edges.south_east.surface ||
          current_floor_edges.south_west.surface ||
          current_floor_edges.north_west.surface ||
          current_floor_edges.north_east.surface ||
          current_floor_edges.top.surface ||
          current_floor_edges.right.surface ||
          current_floor_edges.left.surface ||
          current_floor_edges.bottom.surface)
        floorEdgeTiles_out.insert(std::make_pair(current_position, current_floor_edges));
    } // end IF
  } // end FOR
}

void
RPG_Client_Common_Tools::updateFloorEdges(const RPG_Graphics_FloorEdgeTileSet_t& tileSet_in,
                                          RPG_Graphics_FloorEdgeTileMap_t& floorEdgeTiles_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::updateFloorEdges"));

  for (RPG_Graphics_FloorEdgeTileMapIterator_t iterator = floorEdgeTiles_inout.begin();
       iterator != floorEdgeTiles_inout.end();
       iterator++)
  {
    if ((*iterator).second.west.surface)
      (*iterator).second.west = tileSet_in.west;
    if ((*iterator).second.north.surface)
      (*iterator).second.north = tileSet_in.north;
    if ((*iterator).second.east.surface)
      (*iterator).second.east = tileSet_in.east;
    if ((*iterator).second.south.surface)
      (*iterator).second.south = tileSet_in.south;
    if ((*iterator).second.south_west.surface)
      (*iterator).second.south_west = tileSet_in.south_west;
    if ((*iterator).second.north_west.surface)
      (*iterator).second.north_west = tileSet_in.north_west;
    if ((*iterator).second.south_east.surface)
      (*iterator).second.south_east = tileSet_in.south_east;
    if ((*iterator).second.north_east.surface)
      (*iterator).second.north_east = tileSet_in.north_east;
    if ((*iterator).second.top.surface)
      (*iterator).second.top = tileSet_in.top;
    if ((*iterator).second.right.surface)
      (*iterator).second.right = tileSet_in.right;
    if ((*iterator).second.left.surface)
      (*iterator).second.left = tileSet_in.left;
    if ((*iterator).second.bottom.surface)
      (*iterator).second.bottom = tileSet_in.bottom;
  } // end FOR
}

void
RPG_Client_Common_Tools::initWalls(const RPG_Map_FloorPlan_t& floorPlan_in,
                                   const RPG_Graphics_WallTileSet_t& tileSet_in,
                                   RPG_Graphics_WallTileMap_t& wallTiles_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initWalls"));

  // init return value(s)
  wallTiles_out.clear();

  RPG_Map_Position_t current_position;
  RPG_Map_Position_t east, north, west, south;
  RPG_Graphics_WallTileSet_t current_walls;
  RPG_Map_Door_t position_door;
  bool has_walls = false;
  for (unsigned long y = 0;
       y < floorPlan_in.size_y;
       y++)
    for (unsigned long x = 0;
         x < floorPlan_in.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      ACE_OS::memset(&current_walls,
                     0,
                     sizeof(current_walls));
      has_walls = false;

      position_door.position = current_position;
      // floor or door ? --> compute walls
      if (RPG_Map_Common_Tools::isFloor(current_position, floorPlan_in) ||
          (floorPlan_in.doors.find(position_door) != floorPlan_in.doors.end()))
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

        if ((floorPlan_in.walls.find(east) != floorPlan_in.walls.end()) ||
            (current_position.first == (floorPlan_in.size_x - 1))) // perimeter
        {
          current_walls.east = tileSet_in.east;
          has_walls = true;
        } // end IF
        if ((floorPlan_in.walls.find(west) != floorPlan_in.walls.end()) ||
            (current_position.first == 0)) // perimeter
        {
          current_walls.west = tileSet_in.west;
          has_walls = true;
        } // end IF
        if ((floorPlan_in.walls.find(north) != floorPlan_in.walls.end()) ||
            (current_position.second == 0)) // perimeter
        {
          current_walls.north = tileSet_in.north;
          has_walls = true;
        } // end IF
        if ((floorPlan_in.walls.find(south) != floorPlan_in.walls.end()) ||
            (current_position.second == (floorPlan_in.size_y - 1))) // perimeter
        {
          current_walls.south = tileSet_in.south;
          has_walls = true;
        } // end IF

        if (has_walls)
          wallTiles_out.insert(std::make_pair(current_position, current_walls));
      } // end IF
    } // end FOR
}

void
RPG_Client_Common_Tools::updateWalls(const RPG_Graphics_WallTileSet_t& tileSet_in,
                                     RPG_Graphics_WallTileMap_t& wallTiles_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::updateWalls"));

  for (RPG_Graphics_WallTileMapIterator_t iterator = wallTiles_inout.begin();
       iterator != wallTiles_inout.end();
       iterator++)
  {
    if ((*iterator).second.west.surface)
      (*iterator).second.west = tileSet_in.west;
    if ((*iterator).second.north.surface)
      (*iterator).second.north = tileSet_in.north;
    if ((*iterator).second.east.surface)
      (*iterator).second.east = tileSet_in.east;
    if ((*iterator).second.south.surface)
      (*iterator).second.south = tileSet_in.south;
  } // end FOR
}

void
RPG_Client_Common_Tools::initDoors(const RPG_Map_FloorPlan_t& floorPlan_in,
                                   const RPG_Engine& engine_in,
                                   const RPG_Graphics_DoorTileSet_t& tileSet_in,
                                   RPG_Graphics_DoorTileMap_t& doorTiles_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initDoors"));

  // init return value(s)
  doorTiles_out.clear();

  RPG_Graphics_Tile_t current_tile;
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  for (RPG_Map_DoorsConstIterator_t iterator = floorPlan_in.doors.begin();
       iterator != floorPlan_in.doors.end();
       iterator++)
  {
    ACE_OS::memset(&current_tile,
                   0,
                   sizeof(current_tile));
    orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
    if ((*iterator).is_broken)
    {
      doorTiles_out.insert(std::make_pair((*iterator).position, tileSet_in.broken));
      continue;
    } // end IF

    orientation = RPG_Client_Common_Tools::getDoorOrientation(engine_in,
                                                              (*iterator).position);
    switch (orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        current_tile = ((*iterator).is_open ? tileSet_in.horizontal_open
                                            : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current_tile = ((*iterator).is_open ? tileSet_in.vertical_open
                                            : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door [%u,%u] orientation (was: \"%s\"), continuing\n"),
                   (*iterator).position.first,
                   (*iterator).position.second,
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));

        continue;
      }
    } // end SWITCH

    doorTiles_out.insert(std::make_pair((*iterator).position, current_tile));
  } // end FOR
}

void
RPG_Client_Common_Tools::updateDoors(const RPG_Graphics_DoorTileSet_t& tileSet_in,
                                     const RPG_Engine& engine_in,
                                     RPG_Graphics_DoorTileMap_t& doorTiles_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::updateDoors"));

  RPG_Graphics_Tile_t current_tile;
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  RPG_Map_Door_t current_door;
  for (RPG_Graphics_DoorTileMapIterator_t iterator = doorTiles_inout.begin();
       iterator != doorTiles_inout.end();
       iterator++)
  {
    ACE_OS::memset(&current_tile,
                   0,
                   sizeof(current_tile));
    orientation = RPG_GRAPHICS_ORIENTATION_INVALID;

    current_door = engine_in.getDoor((*iterator).first);
    if (current_door.is_broken)
    {
      (*iterator).second = tileSet_in.broken;
      continue;
    } // end IF

    orientation = RPG_Client_Common_Tools::getDoorOrientation(engine_in,
                                                              (*iterator).first);
    switch (orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        current_tile = (current_door.is_open ? tileSet_in.horizontal_open
                                             : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current_tile = (current_door.is_open ? tileSet_in.vertical_open
                                             : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door [%u,%u] orientation (was: \"%s\"), continuing\n"),
                   (*iterator).first.first,
                   (*iterator).first.second,
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));

        continue;
      }
    } // end SWITCH

    (*iterator).second = current_tile;
  } // end FOR
}

RPG_Graphics_Sprite
RPG_Client_Common_Tools::class2Sprite(const RPG_Character_Class& class_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::class2Sprite"));

  switch (class_in.metaClass)
  {
    case METACLASS_PRIEST:
      return SPRITE_PRIEST;
    default:
      break;
  } // end SWITCH

  return SPRITE_HUMAN;
}

RPG_Graphics_Sprite
RPG_Client_Common_Tools::monster2Sprite(const std::string& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::monster2Sprite"));

  // *TODO*: 
  return SPRITE_GOBLIN;
}

bool
RPG_Client_Common_Tools::hasCeiling(const RPG_Map_Position_t& position_in,
                                    const RPG_Engine& engine_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::hasCeiling"));

  // shortcut: floors, doors never get a ceiling
  if ((engine_in.getElement(position_in) == MAPELEMENT_FLOOR) ||
      (engine_in.getElement(position_in) == MAPELEMENT_DOOR))
    return false;

  RPG_Map_Position_t east, west, south, north;
  east = position_in;
  east.first++;
  west = position_in;
  west.first--;
  north = position_in;
  north.second--;
  south = position_in;
  south.second++;

  // *NOTE*: walls should get a ceiling if they're either:
  // - "internal" (e.g. (single) strength room/corridor walls)
  // - "outer" walls get a (single strength) ceiling "margin"

  // "corridors"
  // vertical
  if (((engine_in.getElement(east) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(east) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(west) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(west) == MAPELEMENT_DOOR)))
    return true;
  // horizontal
  if (((engine_in.getElement(north) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(north) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(south) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(south) == MAPELEMENT_DOOR)))
    return true;

  // (internal) "corners"
  // SW
  if (((engine_in.getElement(west) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(west) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(south) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(south) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(north) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(north) == MAPELEMENT_WALL)) &&
      ((engine_in.getElement(east) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(east) == MAPELEMENT_WALL)))
    return (engine_in.isCorner(north) ||
            engine_in.isCorner(east) ||
            RPG_Client_Common_Tools::hasCeiling(north, engine_in) ||
            RPG_Client_Common_Tools::hasCeiling(east, engine_in));
  // SE
  if (((engine_in.getElement(east) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(east) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(south) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(south) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(north) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(north) == MAPELEMENT_WALL)) &&
      ((engine_in.getElement(west) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(west) == MAPELEMENT_WALL)))
    return (engine_in.isCorner(north) ||
            engine_in.isCorner(west) ||
            RPG_Client_Common_Tools::hasCeiling(north, engine_in) ||
            RPG_Client_Common_Tools::hasCeiling(west, engine_in));
  // NW
  if (((engine_in.getElement(west) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(west) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(north) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(north) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(south) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(south) == MAPELEMENT_WALL)) &&
      ((engine_in.getElement(east) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(east) == MAPELEMENT_WALL)))
    return (engine_in.isCorner(south) ||
            engine_in.isCorner(east) ||
            RPG_Client_Common_Tools::hasCeiling(south, engine_in) ||
            RPG_Client_Common_Tools::hasCeiling(east, engine_in));
  // NE
  if (((engine_in.getElement(east) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(east) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(north) == MAPELEMENT_FLOOR) ||
       (engine_in.getElement(north) == MAPELEMENT_DOOR)) &&
      ((engine_in.getElement(south) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(south) == MAPELEMENT_WALL)) &&
      ((engine_in.getElement(west) == MAPELEMENT_UNMAPPED) ||
       (engine_in.getElement(west) == MAPELEMENT_WALL)))
    return (engine_in.isCorner(south) ||
            engine_in.isCorner(west) ||
            RPG_Client_Common_Tools::hasCeiling(south, engine_in) ||
            RPG_Client_Common_Tools::hasCeiling(west, engine_in));

  return false;
}

RPG_Graphics_Orientation
RPG_Client_Common_Tools::getDoorOrientation(const RPG_Engine& engine_in,
                                            const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::getDoorOrientation"));

  RPG_Map_Position_t east;//, south;
  east = position_in;
  east.first++;
//   south = position_in;
//   south.second++;

  if (engine_in.getElement(east) == MAPELEMENT_WALL) // &&
//     (engine_in.getElement(west) == MAPELEMENT_WALL))
  {
    return ORIENTATION_HORIZONTAL;
  } // end IF

  return ORIENTATION_VERTICAL;
}

RPG_Graphics_Cursor
RPG_Client_Common_Tools::getCursor(const RPG_Map_Position_t& position_in,
                                   const RPG_Engine& engine_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::getCursor"));

  RPG_Graphics_Cursor result = CURSOR_NORMAL;

  // monster ?
  RPG_Engine_EntityID_t entity_id = engine_in.hasEntity(position_in);
  if (entity_id &&
      engine_in.isMonster(entity_id))
  {
    // && in reach ?
    entity_id = engine_in.getActive();
    if (entity_id &&
        RPG_Map_Common_Tools::isAdjacent(engine_in.getPosition(entity_id), position_in))
      return CURSOR_TARGET;
  } // end IF

  // (closed) door ?
  if (engine_in.getElement(position_in) == MAPELEMENT_DOOR)
  {
    RPG_Map_Door_t door = engine_in.getDoor(position_in);
    RPG_Engine_EntityID_t entity_id = engine_in.getActive();
    if (!door.is_open &&
        entity_id &&
        (RPG_Map_Common_Tools::distance(engine_in.getPosition(entity_id),
                                        position_in) == 1))
      result = CURSOR_DOOR_OPEN;
  } // end IF

  return result;
}
