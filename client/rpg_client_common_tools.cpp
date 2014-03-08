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

#include "rpg_client_defines.h"

#include "rpg_sound_defines.h"
#include "rpg_sound_dictionary.h"
#include "rpg_sound_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"

#include "rpg_engine_common_tools.h"

#include "rpg_map_common_tools.h"

#include "rpg_player_defines.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"

#include <ace/Global_Macros.h>

bool
RPG_Client_Common_Tools::initSDLInput(const RPG_Client_SDL_InputConfiguration_t& SDLInputConfiguration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initSDLInput"));

  // ***** keyboard setup *****
  // Unicode translation
	int previous_state = SDL_EnableUNICODE((SDLInputConfiguration_in.use_UNICODE ? 1 : 0));
	ACE_UNUSED_ARG(previous_state);

  // key repeat rates
	if (SDL_EnableKeyRepeat(SDLInputConfiguration_in.key_repeat_initial_delay,
		                      SDLInputConfiguration_in.key_repeat_interval))
	{
	  ACE_DEBUG((LM_ERROR,
		           ACE_TEXT("failed to SDL_EnableKeyRepeat(): \"%s\", aborting\n"),
							 ACE_TEXT(SDL_GetError())));

		return false;
	} // end IF

	//   // ignore keyboard events
	//   SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
	//   SDL_EventState(SDL_KEYUP, SDL_IGNORE);

	// SDL event filter (filter mouse motion events and the like)
	//   SDL_SetEventFilter(event_filter_SDL_cb);

	// ***** mouse setup *****
	SDL_ShowCursor(SDL_TRUE); // disable OS mouse cursor over SDL window

	return true;
}

bool
RPG_Client_Common_Tools::init(const RPG_Client_SDL_InputConfiguration_t& inputConfiguration_in,
                              const RPG_Sound_SDLConfiguration_t& audioConfiguration_in,
                              const std::string& soundDirectory_in,
                              const bool& useCD_in,
                              const unsigned int& soundCacheSize_in,
                              const bool& muted_in,
                              const std::string& soundDictionaryFile_in,
                              const std::string& graphicsDirectory_in,
                              const unsigned int& graphicsCacheSize_in,
                              const std::string& graphicsDictionaryFile_in,
                              const bool& initSDL_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::init"));

	// step0a: init input
	if (!RPG_Client_Common_Tools::initSDLInput(inputConfiguration_in))
  {
    ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to RPG_Client_Common_Tools::initSDLInput, aborting\n")));

    return false;
  } // end IF

  // step0b: init audio/video, string conversion facilities and other static data
  if (!RPG_Sound_Common_Tools::init(audioConfiguration_in,
                                    soundDirectory_in,
                                    useCD_in,
                                    soundCacheSize_in,
                                    muted_in))
  {
    ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to RPG_Sound_Common_Tools::init, aborting\n")));

    return false;
  } // end IF
  RPG_Graphics_Common_Tools::init(graphicsDirectory_in,
                                  graphicsCacheSize_in,
                                  initSDL_in);

  // step1: init dictionaries
  // step1a: init graphics dictionary
  if (!graphicsDictionaryFile_in.empty())
  {
    try
    {
      RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(graphicsDictionaryFile_in
#ifdef _DEBUG
                                                          ,true
#else
                                                          ,false
#endif
                                                          );
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Graphics_Dictionary::init, aborting\n")));

      return false;
    }
  } // end IF

  // step1b: init sound dictionary
  if (!soundDictionaryFile_in.empty())
  {
    try
    {
      RPG_SOUND_DICTIONARY_SINGLETON::instance()->init(soundDictionaryFile_in
#ifdef _DEBUG
                                                       ,true
#else
                                                       ,false
#endif
        );
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Sound_Dictionary::init, aborting\n")));

      return false;
    }
  } // end IF

  // step2: init cursor manager singleton
  if (initSDL_in)
  {
    SDL_Rect dirty_region;
    ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
    RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set(CURSOR_NORMAL,
                                                           dirty_region);
  } // end IF

  return true;
}

void
RPG_Client_Common_Tools::fini()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::fini"));

  RPG_Sound_Common_Tools::fini();
}

void
RPG_Client_Common_Tools::initFloorEdges(const RPG_Engine& engine_in,
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
  RPG_Map_Element current_element;
  RPG_Map_Size_t map_size = engine_in.getSize();
  RPG_Map_Positions_t walls = engine_in.getWalls();
  for (unsigned int y = 0;
       y < map_size.second;
       y++)
    for (unsigned int x = 0;
         x < map_size.first;
         x++)
  {
    current_position = std::make_pair(x, y);
    ACE_OS::memset(&current_floor_edges,
                   0,
                   sizeof(current_floor_edges));

    // floor or door ? --> compute floor edges
    current_element = engine_in.getElement(current_position);
    if ((current_element != MAPELEMENT_FLOOR) &&
        (current_element != MAPELEMENT_DOOR))
      continue;

    // find neighboring map elements
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

    if (walls.find(east) != walls.end())
    {
      current_floor_edges.east = tileSet_in.east;
      if (walls.find(north) != walls.end())
        current_floor_edges.north_east = tileSet_in.north_east;
      if (walls.find(south) != walls.end())
        current_floor_edges.south_east = tileSet_in.south_east;
    } // end IF
    else
    {
      // right corner ?
      if ((walls.find(north) == walls.end()) &&
        (walls.find(north_east) != walls.end()))
        current_floor_edges.right = tileSet_in.right;
    } // end ELSE
    if ((walls.find(west) != walls.end()))
    {
      current_floor_edges.west = tileSet_in.west;
      if (walls.find(north) != walls.end())
        current_floor_edges.north_west = tileSet_in.north_west;
      if (walls.find(south) != walls.end())
        current_floor_edges.south_west = tileSet_in.south_west;
    } // end IF
    else
    {
      // left corner ?
      if ((walls.find(south) == walls.end()) &&
          (walls.find(south_west) != walls.end()))
        current_floor_edges.left = tileSet_in.left;
    } // end ELSE
    if ((walls.find(north) != walls.end()))
      current_floor_edges.north = tileSet_in.north;
    else
    {
      // top corner ?
      if ((walls.find(west) == walls.end()) &&
          (walls.find(north_west) != walls.end()))
        current_floor_edges.top = tileSet_in.top;
    } // end ELSE
    if ((walls.find(south) != walls.end()))
      current_floor_edges.south = tileSet_in.south;
    else
    {
      // bottom corner ?
      if ((walls.find(east) == walls.end()) &&
          (walls.find(south_east) != walls.end()))
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
RPG_Client_Common_Tools::initWalls(const RPG_Engine& engine_in,
                                   const RPG_Graphics_WallTileSet_t& tileSet_in,
                                   RPG_Graphics_WallTileMap_t& wallTiles_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initWalls"));

  // init return value(s)
  wallTiles_out.clear();

  RPG_Map_Position_t current_position;
  RPG_Map_Position_t east, north, west, south;
  RPG_Graphics_WallTileSet_t current_walls;
  bool has_walls = false;
  RPG_Map_Element current_element;
  RPG_Map_Size_t map_size = engine_in.getSize();
  RPG_Map_Positions_t walls = engine_in.getWalls();
  for (unsigned int y = 0;
       y < map_size.second;
       y++)
    for (unsigned int x = 0;
         x < map_size.first;
         x++)
    {
      current_position = std::make_pair(x, y);
      ACE_OS::memset(&current_walls,
                     0,
                     sizeof(current_walls));
      has_walls = false;

      // floor or door ? --> compute walls
      current_element = engine_in.getElement(current_position);
      if ((current_element != MAPELEMENT_FLOOR) &&
          (current_element != MAPELEMENT_DOOR))
        continue;

      // find neighboring walls
      east = current_position;
      east.first++;
      north = current_position;
      north.second--;
      west = current_position;
      west.first--;
      south = current_position;
      south.second++;

      if ((walls.find(east) != walls.end()) ||
          (current_position.first == (map_size.first - 1))) // perimeter
      {
        current_walls.east = tileSet_in.east;
        has_walls = true;
      } // end IF
      if ((walls.find(west) != walls.end()) ||
          (current_position.first == 0)) // perimeter
      {
        current_walls.west = tileSet_in.west;
        has_walls = true;
      } // end IF
      if ((walls.find(north) != walls.end()) ||
          (current_position.second == 0)) // perimeter
      {
        current_walls.north = tileSet_in.north;
        has_walls = true;
      } // end IF
      if ((walls.find(south) != walls.end()) ||
          (current_position.second == (map_size.second - 1))) // perimeter
      {
        current_walls.south = tileSet_in.south;
        has_walls = true;
      } // end IF

      if (has_walls)
        wallTiles_out.insert(std::make_pair(current_position, current_walls));
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
RPG_Client_Common_Tools::initDoors(const RPG_Engine& engine_in,
                                   const RPG_Graphics_DoorTileSet_t& tileSet_in,
                                   RPG_Graphics_DoorTileMap_t& doorTiles_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initDoors"));

  // init return value(s)
  doorTiles_out.clear();

  RPG_Graphics_Tile_t current_tile;
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  RPG_Map_Positions_t doors = engine_in.getDoors();
  RPG_Map_DoorState door_state = RPG_MAP_DOORSTATE_INVALID;
  for (RPG_Map_PositionsConstIterator_t iterator = doors.begin();
       iterator != doors.end();
       iterator++)
  {
    ACE_OS::memset(&current_tile,
                   0,
                   sizeof(current_tile));
    orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
    door_state = RPG_MAP_DOORSTATE_INVALID;

    door_state = engine_in.state(*iterator);
    ACE_ASSERT(door_state != RPG_MAP_DOORSTATE_INVALID);
    if (door_state == DOORSTATE_BROKEN)
    {
      doorTiles_out.insert(std::make_pair(*iterator, tileSet_in.broken));
      continue;
    } // end IF

    orientation = RPG_Client_Common_Tools::getDoorOrientation(engine_in,
                                                              *iterator);
    switch (orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        current_tile = ((door_state == DOORSTATE_OPEN) ? tileSet_in.horizontal_open
                                                       : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current_tile = ((door_state == DOORSTATE_OPEN) ? tileSet_in.vertical_open
                                                       : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door [%u,%u] orientation (was: \"%s\"), continuing\n"),
                   (*iterator).first, (*iterator).second,
                   RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));

        continue;
      }
    } // end SWITCH

    doorTiles_out.insert(std::make_pair(*iterator, current_tile));
  } // end FOR
}

void
RPG_Client_Common_Tools::updateDoors(const RPG_Graphics_DoorTileSet_t& tileSet_in,
                                     const RPG_Engine& engine_in,
                                     RPG_Graphics_DoorTileMap_t& doorTiles_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::updateDoors"));

  RPG_Graphics_Tile_t current_tile;
  RPG_Graphics_Orientation orientation;
  RPG_Map_DoorState door_state;
  for (RPG_Graphics_DoorTileMapIterator_t iterator = doorTiles_inout.begin();
       iterator != doorTiles_inout.end();
       iterator++)
  {
    ACE_OS::memset(&current_tile,
                   0,
                   sizeof(current_tile));
    orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
    door_state = RPG_MAP_DOORSTATE_INVALID;

    door_state = engine_in.state((*iterator).first);
    ACE_ASSERT(door_state != RPG_MAP_DOORSTATE_INVALID);
    if (door_state == DOORSTATE_BROKEN)
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
        current_tile = ((door_state == DOORSTATE_OPEN) ? tileSet_in.horizontal_open
                                                       : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current_tile = ((door_state == DOORSTATE_OPEN) ? tileSet_in.vertical_open
                                                       : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door [%u,%u] orientation (was: \"%s\"), continuing\n"),
                   (*iterator).first.first, (*iterator).first.second,
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
                                    const RPG_Engine& engine_in,
                                    const bool& lockedAccess_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::hasCeiling"));

  RPG_Map_Element current_element = engine_in.getElement(position_in,
                                                         lockedAccess_in);

  // shortcut: floors, doors never get a ceiling
  if ((current_element == MAPELEMENT_FLOOR) ||
      (current_element == MAPELEMENT_DOOR))
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
  RPG_Map_Element element_east, element_west, element_north, element_south;
  element_east  = engine_in.getElement(east,
                                       lockedAccess_in);
  element_west  = engine_in.getElement(west,
                                       lockedAccess_in);
  element_north = engine_in.getElement(north,
                                       lockedAccess_in);
  element_south = engine_in.getElement(south,
                                       lockedAccess_in);

  // *NOTE*: walls should get a ceiling if they're either:
  // - "internal" (e.g. (single) strength room/corridor walls)
  // - "outer" walls get a (single strength) ceiling "margin"



  // "corridors"
  // vertical
  if (((element_east == MAPELEMENT_FLOOR) ||
       (element_east == MAPELEMENT_DOOR)) &&
      ((element_west == MAPELEMENT_FLOOR) ||
       (element_west == MAPELEMENT_DOOR)))
    return true;
  // horizontal
  if (((element_north == MAPELEMENT_FLOOR) ||
       (element_north == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_FLOOR) ||
       (element_south == MAPELEMENT_DOOR)))
    return true;

  // (internal) "corners"
  // SW
  if (((element_west == MAPELEMENT_FLOOR) ||
       (element_west == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_FLOOR) ||
       (element_south == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_UNMAPPED) ||
       (element_north == MAPELEMENT_WALL)) &&
      ((element_east == MAPELEMENT_UNMAPPED) ||
       (element_east == MAPELEMENT_WALL)))
    return (engine_in.isCorner(north, lockedAccess_in) ||
            engine_in.isCorner(east, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(north, engine_in, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(east, engine_in, lockedAccess_in));
  // SE
  if (((element_east == MAPELEMENT_FLOOR) ||
       (element_east == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_FLOOR) ||
       (element_south == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_UNMAPPED) ||
       (element_north == MAPELEMENT_WALL)) &&
      ((element_west == MAPELEMENT_UNMAPPED) ||
       (element_west == MAPELEMENT_WALL)))
    return (engine_in.isCorner(north, lockedAccess_in) ||
            engine_in.isCorner(west, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(north, engine_in, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(west, engine_in, lockedAccess_in));
  // NW
  if (((element_west == MAPELEMENT_FLOOR) ||
       (element_west == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_FLOOR) ||
       (element_north == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_UNMAPPED) ||
       (element_south == MAPELEMENT_WALL)) &&
      ((element_east == MAPELEMENT_UNMAPPED) ||
       (element_east == MAPELEMENT_WALL)))
    return (engine_in.isCorner(south, lockedAccess_in) ||
            engine_in.isCorner(east, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(south, engine_in, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(east, engine_in, lockedAccess_in));
  // NE
  if (((element_east == MAPELEMENT_FLOOR) ||
       (element_east == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_FLOOR) ||
       (element_north == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_UNMAPPED) ||
       (element_south == MAPELEMENT_WALL)) &&
      ((element_west == MAPELEMENT_UNMAPPED) ||
       (element_west == MAPELEMENT_WALL)))
    return (engine_in.isCorner(south, lockedAccess_in) ||
            engine_in.isCorner(west, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(south, engine_in, lockedAccess_in) ||
            RPG_Client_Common_Tools::hasCeiling(west, engine_in, lockedAccess_in));

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
                                   const RPG_Engine_EntityID_t& id_in,
                                   const bool& hasSeen_in,
                                   const RPG_Client_SelectionMode& mode_in,
                                   const RPG_Engine& engine_in,
                                   const bool& lockedAcces_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::getCursor"));

  RPG_Graphics_Cursor result = CURSOR_NORMAL;

  RPG_Map_Position_t entity_position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                      std::numeric_limits<unsigned int>::max());
  if (id_in)
    entity_position = engine_in.getPosition(id_in, lockedAcces_in);

  switch (mode_in)
  {
    case SELECTIONMODE_AIM_CIRCLE:
    case SELECTIONMODE_AIM_SQUARE:
      return CURSOR_TARGET;
    case SELECTIONMODE_NORMAL:
      break;
    case SELECTIONMODE_PATH:
    {
      if (id_in == 0)
        break;

      if ((entity_position != position_in) &&
          (engine_in.isValid(position_in, lockedAcces_in) &&
          hasSeen_in))
        return CURSOR_TRAVEL;

      return CURSOR_TARGET_INVALID;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid selection mode (was: %d), aborting\n"),
                 mode_in));

      return result;
    }
  } // end SWITCH

  // monster ?
  RPG_Engine_EntityID_t entity_id = engine_in.hasEntity(position_in, lockedAcces_in);
  if (id_in &&
      entity_id &&
      engine_in.isMonster(entity_id, lockedAcces_in))
    return CURSOR_TARGET;

  // (closed/locked) door ?
  if (engine_in.getElement(position_in, lockedAcces_in) == MAPELEMENT_DOOR)
  {
    RPG_Map_DoorState door_state = engine_in.state(position_in, lockedAcces_in);
    if (((door_state == DOORSTATE_CLOSED) ||
         (door_state == DOORSTATE_LOCKED)) &&
        id_in &&
        RPG_Map_Common_Tools::isAdjacent(entity_position,
                                         position_in))
      return CURSOR_DOOR_OPEN;
  } // end IF

  return result;
}
