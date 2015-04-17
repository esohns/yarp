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

#include "ace/Global_Macros.h"

#include "common_file_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_map_common_tools.h"

#include "rpg_engine_common_tools.h"

#include "rpg_sound_common_tools.h"
#include "rpg_sound_defines.h"
#include "rpg_sound_dictionary.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_defines.h"
#include "rpg_client_graphicsmode.h"

// initialize statics
RPG_Client_GraphicsModeToStringTable_t RPG_Client_GraphicsModeHelper::myRPG_Client_GraphicsModeToStringTable;

bool
RPG_Client_Common_Tools::initSDLInput(const RPG_Client_SDL_InputConfiguration_t& SDLInputConfiguration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initSDLInput"));

  // ***** keyboard setup *****
  // Unicode translation
	int previous_state =
		SDL_EnableUNICODE((SDLInputConfiguration_in.use_UNICODE ? 1 : 0));
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
	int show_cursor_status_before = SDL_ShowCursor(-1);
	if (show_cursor_status_before == 1)
		SDL_ShowCursor(SDL_DISABLE); // disable OS mouse cursor over SDL window

	return true;
}

void
RPG_Client_Common_Tools::initUserProfiles()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initUserProfiles"));

  std::string profiles_directory =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  if (Common_File_Tools::isEmptyDirectory(profiles_directory))
  {
    std::string default_profile =
      RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                            false);
    default_profile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    default_profile += RPG_PLAYER_PROFILES_SUB;
    default_profile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    default_profile += RPG_Common_Tools::sanitize (RPG_PLAYER_DEF_NAME);
    default_profile += RPG_PLAYER_PROFILE_EXT;
    if (!Common_File_Tools::copyFile (default_profile,
                                      profiles_directory))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_File_Tools::copyFile(\"%s\",\"%s\"), continuing\n"),
                  ACE_TEXT (default_profile.c_str ()),
                  ACE_TEXT (profiles_directory.c_str ())));
  } // end IF
}

bool
RPG_Client_Common_Tools::initialize (const RPG_Client_SDL_InputConfiguration_t& inputConfiguration_in,
                                     const RPG_Sound_SDLConfiguration_t& audioConfiguration_in,
                                     const std::string& soundDirectory_in,
                                     bool useCD_in,
                                     unsigned int soundCacheSize_in,
                                     bool muted_in,
                                     const std::string& soundDictionaryFile_in,
                                     const std::string& graphicsDirectory_in,
                                     unsigned int graphicsCacheSize_in,
                                     const std::string& graphicsDictionaryFile_in,
                                     bool initSDL_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::initialize"));

  // step0: initialize string conversion facilities
  RPG_Client_GraphicsModeHelper::init();

  // step1: initialize input
  if (!RPG_Client_Common_Tools::initSDLInput (inputConfiguration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Client_Common_Tools::initSDLInput, aborting\n")));
    return false;
  } // end IF

  // step2: initialize sound

  // step2a: initialize sound dictionary
  if (!soundDictionaryFile_in.empty ())
  {
    try
    {
      RPG_SOUND_DICTIONARY_SINGLETON::instance ()->init (soundDictionaryFile_in
#ifdef _DEBUG
                                                         , true
#else
                                                         , false
#endif
                                                         );
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
        ACE_TEXT ("caught exception in RPG_Sound_Dictionary::init, aborting\n")));

      return false;
    }
  } // end IF

  // step2b: initialize audio/video, string conversion facilities and other
  //         (static) data
  if (!RPG_Sound_Common_Tools::init(audioConfiguration_in,
                                    soundDirectory_in,
                                    useCD_in,
                                    soundCacheSize_in,
                                    muted_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Sound_Common_Tools::init, aborting\n")));
    return false;
  } // end IF

  // step3: initialize graphics
  RPG_Graphics_Common_Tools::preInitialize ();

  // step3a: initialize graphics dictionary
  if (!graphicsDictionaryFile_in.empty ())
  {
    try
    {
      RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->init (graphicsDictionaryFile_in
#ifdef _DEBUG
                                                            , true
#else
                                                            , false
#endif
                                                            );
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
        ACE_TEXT ("caught exception in RPG_Graphics_Dictionary::init, aborting\n")));

      return false;
    }
  } // end IF

  if (!RPG_Graphics_Common_Tools::initialize (graphicsDirectory_in,
                                              graphicsCacheSize_in,
                                              initSDL_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initialize, aborting\n")));
    return false;
  } // end IF

  // step4: initialize cursor manager singleton
  if (initSDL_in)
  {
    SDL_Rect dirty_region;
    ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
    RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->setCursor(CURSOR_NORMAL,
                                                                 dirty_region);
  } // end IF

  // step5: initialize user profiles
  RPG_Client_Common_Tools::initUserProfiles();

  return true;
}

void
RPG_Client_Common_Tools::finalize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Common_Tools::fini"));

  RPG_Sound_Common_Tools::fini ();
  RPG_Graphics_Common_Tools::finalize ();
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

    if (current_floor_edges.east.surface       ||
        current_floor_edges.north.surface      ||
        current_floor_edges.west.surface       ||
        current_floor_edges.south.surface      ||
        current_floor_edges.south_east.surface ||
        current_floor_edges.south_west.surface ||
        current_floor_edges.north_west.surface ||
        current_floor_edges.north_east.surface ||
        current_floor_edges.top.surface        ||
        current_floor_edges.right.surface      ||
        current_floor_edges.left.surface       ||
        current_floor_edges.bottom.surface)
      floorEdgeTiles_out.insert(std::make_pair(current_position,
                                               current_floor_edges));
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
	engine_in.lock();
  RPG_Map_Positions_t doors = engine_in.getDoors(false);
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

    door_state = engine_in.state(*iterator,
			                           false);
    ACE_ASSERT(door_state != RPG_MAP_DOORSTATE_INVALID);
    if (door_state == DOORSTATE_BROKEN)
    {
      doorTiles_out.insert(std::make_pair(*iterator, tileSet_in.broken));
      continue;
    } // end IF

    orientation = RPG_Client_Common_Tools::getDoorOrientation(*iterator,
                                                              engine_in,
                                                              false);
    switch (orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        current_tile =
            ((door_state == DOORSTATE_OPEN) ? tileSet_in.horizontal_open
                                            : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current_tile =
            ((door_state == DOORSTATE_OPEN) ? tileSet_in.vertical_open
                                            : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door [%u,%u] orientation (was: \"%s\"), continuing\n"),
                   (*iterator).first, (*iterator).second,
                   ACE_TEXT(RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str())));

        continue;
      }
    } // end SWITCH

    doorTiles_out.insert(std::make_pair(*iterator, current_tile));
  } // end FOR
	engine_in.unlock();
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
	engine_in.lock();
  for (RPG_Graphics_DoorTileMapIterator_t iterator = doorTiles_inout.begin();
       iterator != doorTiles_inout.end();
       iterator++)
  {
    ACE_OS::memset(&current_tile,
                   0,
                   sizeof(current_tile));
    orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
    door_state = RPG_MAP_DOORSTATE_INVALID;

    door_state = engine_in.state((*iterator).first,
			                           false);
    ACE_ASSERT(door_state != RPG_MAP_DOORSTATE_INVALID);
    if (door_state == DOORSTATE_BROKEN)
    {
      (*iterator).second = tileSet_in.broken;
      continue;
    } // end IF

    orientation = RPG_Client_Common_Tools::getDoorOrientation((*iterator).first,
                                                              engine_in,
                                                              false);
    switch (orientation)
    {
      case ORIENTATION_HORIZONTAL:
      {
        current_tile =
            ((door_state == DOORSTATE_OPEN) ? tileSet_in.horizontal_open
                                            : tileSet_in.horizontal_closed);
        break;
      }
      case ORIENTATION_VERTICAL:
      {
        current_tile =
            ((door_state == DOORSTATE_OPEN) ? tileSet_in.vertical_open
                                            : tileSet_in.vertical_closed);
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid door [%u,%u] orientation (was: \"%s\"), continuing\n"),
                   (*iterator).first.first, (*iterator).first.second,
                   ACE_TEXT(RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str())));

        continue;
      }
    } // end SWITCH

    (*iterator).second = current_tile;
  } // end FOR
	engine_in.unlock();
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
  if (lockedAccess_in)
    engine_in.lock();
  element_east  = engine_in.getElement(east,
                                       false);
  element_west  = engine_in.getElement(west,
                                       false);
  element_north = engine_in.getElement(north,
                                       false);
  element_south = engine_in.getElement(south,
                                       false);
  if (lockedAccess_in)
    engine_in.unlock();

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
  bool is_corner_east, is_corner_west, is_corner_north, is_corner_south;
  bool has_ceiling_east, has_ceiling_west, has_ceiling_north, has_ceiling_south;
  if (lockedAccess_in)
    engine_in.lock();
  // SW
  if (((element_west == MAPELEMENT_FLOOR) ||
       (element_west == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_FLOOR) ||
       (element_south == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_UNMAPPED) ||
       (element_north == MAPELEMENT_WALL)) &&
      ((element_east == MAPELEMENT_UNMAPPED) ||
       (element_east == MAPELEMENT_WALL)))
  {
    is_corner_north = engine_in.isCorner(north, false);
    is_corner_east  = engine_in.isCorner(east, false);
    has_ceiling_north = RPG_Client_Common_Tools::hasCeiling(north,
                                                            engine_in,
                                                            false);
    has_ceiling_east  = RPG_Client_Common_Tools::hasCeiling(east,
                                                            engine_in,
                                                            false);
    if (lockedAccess_in)
      engine_in.unlock();
    return (is_corner_north   ||
            is_corner_east    ||
            has_ceiling_north ||
            has_ceiling_east);
  } // end IF
  // SE
  if (((element_east == MAPELEMENT_FLOOR) ||
       (element_east == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_FLOOR) ||
       (element_south == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_UNMAPPED) ||
       (element_north == MAPELEMENT_WALL)) &&
      ((element_west == MAPELEMENT_UNMAPPED) ||
       (element_west == MAPELEMENT_WALL)))
  {
    is_corner_north = engine_in.isCorner(north, false);
    is_corner_west  = engine_in.isCorner(west, false);
    has_ceiling_north = RPG_Client_Common_Tools::hasCeiling(north,
                                                            engine_in,
                                                            false);
    has_ceiling_west  = RPG_Client_Common_Tools::hasCeiling(west,
                                                            engine_in,
                                                            false);
    if (lockedAccess_in)
      engine_in.unlock();
    return (is_corner_north   ||
            is_corner_west    ||
            has_ceiling_north ||
            has_ceiling_west);
  } // end IF
  // NW
  if (((element_west == MAPELEMENT_FLOOR) ||
       (element_west == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_FLOOR) ||
       (element_north == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_UNMAPPED) ||
       (element_south == MAPELEMENT_WALL)) &&
      ((element_east == MAPELEMENT_UNMAPPED) ||
       (element_east == MAPELEMENT_WALL)))
  {
    is_corner_south = engine_in.isCorner(south, false);
    is_corner_east  = engine_in.isCorner(east, false);
    has_ceiling_south = RPG_Client_Common_Tools::hasCeiling(south,
                                                            engine_in,
                                                            false);
    has_ceiling_east  = RPG_Client_Common_Tools::hasCeiling(east,
                                                            engine_in,
                                                            false);
    if (lockedAccess_in)
      engine_in.unlock();
    return (is_corner_south   ||
            is_corner_east    ||
            has_ceiling_south ||
            has_ceiling_east);
  } // end IF
  // NE
  if (((element_east == MAPELEMENT_FLOOR) ||
       (element_east == MAPELEMENT_DOOR)) &&
      ((element_north == MAPELEMENT_FLOOR) ||
       (element_north == MAPELEMENT_DOOR)) &&
      ((element_south == MAPELEMENT_UNMAPPED) ||
       (element_south == MAPELEMENT_WALL)) &&
      ((element_west == MAPELEMENT_UNMAPPED) ||
       (element_west == MAPELEMENT_WALL)))
  {
    is_corner_south = engine_in.isCorner(south, false);
    is_corner_west  = engine_in.isCorner(west, false);
    has_ceiling_south = RPG_Client_Common_Tools::hasCeiling(south,
                                                            engine_in,
                                                            false);
    has_ceiling_west  = RPG_Client_Common_Tools::hasCeiling(west,
                                                            engine_in,
                                                            false);
    if (lockedAccess_in)
      engine_in.unlock();
    return (is_corner_south   ||
            is_corner_west    ||
            has_ceiling_south ||
            has_ceiling_west);
  } // end IF
  if (lockedAccess_in)
    engine_in.unlock();

  return false;
}

bool
RPG_Client_Common_Tools::isVisible(const RPG_Graphics_Positions_t& positions_in,
                                   const RPG_Graphics_Size_t& windowSize_in,
                                   const RPG_Graphics_Position_t& viewport_in,
                                   const SDL_Rect& windowArea_in,
                                   const bool& anyAll_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::isVisible"));

  RPG_Graphics_Offset_t screen_position;
  SDL_Rect tile_area, overlap;
  for (RPG_Graphics_PositionsConstIterator_t iterator = positions_in.begin();
       iterator != positions_in.end();
       iterator++)
  {
    screen_position =
        RPG_Graphics_Common_Tools::map2Screen(*iterator,
                                              windowSize_in,
                                              viewport_in);
    tile_area.x = static_cast<Sint16>(screen_position.first);
    tile_area.y = static_cast<Sint16>(screen_position.second);
    tile_area.w = RPG_GRAPHICS_TILE_FLOOR_WIDTH;
    tile_area.h = RPG_GRAPHICS_TILE_FLOOR_HEIGHT;
    overlap = RPG_Graphics_SDL_Tools::intersect(windowArea_in,
                                                tile_area);
    if ((overlap.w || overlap.h) && anyAll_in)
      return true;
    else if ((!overlap.w || !overlap.h) && !anyAll_in)
      return false;
  } // end FOR

  return (!anyAll_in ? true : false);
}

bool
RPG_Client_Common_Tools::hasHighlight(const RPG_Map_Position_t& position_in,
                                      const RPG_Engine& engine_in,
                                      const bool& lockedAccess_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::hasHighlight"));

  RPG_Map_Element current_element = engine_in.getElement(position_in,
                                                         lockedAccess_in);

  // highlight visible floors and doors
  return (((current_element == MAPELEMENT_FLOOR) ||
           (current_element == MAPELEMENT_DOOR)) &&
          engine_in.canSee(engine_in.getActive(lockedAccess_in),
                           position_in,
                           lockedAccess_in));
}

RPG_Graphics_Orientation
RPG_Client_Common_Tools::getDoorOrientation(const RPG_Map_Position_t& position_in,
                                            const RPG_Engine& engine_in,
                                            const bool& lockedAccess_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::getDoorOrientation"));

  RPG_Map_Position_t east;//, south;
  east = position_in;
  east.first++;
//   south = position_in;
//   south.second++;

  if (engine_in.getElement(east,
                           lockedAccess_in) == MAPELEMENT_WALL) // &&
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

  RPG_Map_Position_t entity_position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
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
  RPG_Engine_EntityID_t entity_id =
      engine_in.hasEntity(position_in, lockedAcces_in);
  if (id_in &&
      entity_id &&
      engine_in.isMonster(entity_id, lockedAcces_in))
    return CURSOR_TARGET;

  // (closed/locked) door ?
  if (engine_in.getElement(position_in, lockedAcces_in) == MAPELEMENT_DOOR)
  {
    RPG_Map_DoorState door_state = engine_in.state(position_in, lockedAcces_in);
    if (((door_state == DOORSTATE_CLOSED) ||
         (door_state == DOORSTATE_LOCKED))               &&
        id_in                                            &&
        RPG_Map_Common_Tools::isAdjacent(entity_position,
                                         position_in))
      return CURSOR_DOOR_OPEN;
  } // end IF

  return result;
}

RPG_Graphics_Style
RPG_Client_Common_Tools::environment2Style(const RPG_Common_Environment& environment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Common_Tools::environment2Style"));

  // *TODO*
  ACE_UNUSED_ARG(environment_in);

  RPG_Graphics_Style result;
  result.door = RPG_CLIENT_GRAPHICS_DEF_DOORSTYLE;
  result.edge = RPG_CLIENT_GRAPHICS_DEF_EDGESTYLE;
  result.floor = RPG_CLIENT_GRAPHICS_DEF_FLOORSTYLE;
  result.half_height_walls = RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE_HALF;
  result.wall = RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE;

  return result;
}
