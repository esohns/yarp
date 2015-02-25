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
#include "stdafx.h"

#include "rpg_map_level.h"

#include "ace/FILE_Addr.h"
#include "ace/FILE_Connector.h"
#include "ace/FILE.h"
#include "ace/FILE_IO.h"
#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "rpg_common_macros.h"

#include "rpg_dice_common.h"
#include "rpg_dice.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_parser_driver.h"
#include "rpg_map_pathfinding_tools.h"

RPG_Map_Level::RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

  //myMap.name.clear();
  myMap.start = std::make_pair(0, 0);
  //myMap.seeds.clear();
  myMap.plan.size_x = 0;
  myMap.plan.size_y = 0;
  //myMap.plan.unmapped.clear();
  //myMap.plan.walls.clear();
  //myMap.plan.doors.clear();
  myMap.plan.rooms_are_square = false;
}

RPG_Map_Level::RPG_Map_Level(const RPG_Map_t& map_in)
 : myMap(map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::RPG_Map_Level"));

}

RPG_Map_Level::~RPG_Map_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::~RPG_Map_Level"));

}

void
RPG_Map_Level::create(const RPG_Map_FloorPlan_Configuration_t& floorPlanConfig_in,
                      RPG_Map_t& map_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::create"));

  // init return value(s)
  map_out.start = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
  map_out.seeds.clear();
  map_out.plan.size_x = 0;
  map_out.plan.size_y = 0;
  map_out.plan.unmapped.clear();
  map_out.plan.walls.clear();
  map_out.plan.doors.clear();
  map_out.plan.rooms_are_square = floorPlanConfig_in.square_rooms;

  // step1: create floor plan
  RPG_Map_Common_Tools::createFloorPlan(floorPlanConfig_in.map_size_x,
                                        floorPlanConfig_in.map_size_y,
                                        floorPlanConfig_in.num_areas,
                                        floorPlanConfig_in.square_rooms,
                                        floorPlanConfig_in.maximize_rooms,
                                        floorPlanConfig_in.min_room_size,
                                        floorPlanConfig_in.doors,
                                        floorPlanConfig_in.corridors,
                                        true, // currently, a door fills one position...
                                        floorPlanConfig_in.max_num_doors_per_room,
                                        map_out.seeds,
                                        map_out.plan);

  // step2: generate suitable starting position
  RPG_Dice_RollResult_t result_x, result_y;
  do
  {
    result_x.clear();
    result_y.clear();
    RPG_Dice::generateRandomNumbers(map_out.plan.size_x,
                                    1,
                                    result_x);
    RPG_Dice::generateRandomNumbers(map_out.plan.size_y,
                                    1,
                                    result_y);
    map_out.start = std::make_pair(result_x.front() - 1,
                                   result_y.front() - 1);
    if (RPG_Map_Common_Tools::isFloor(map_out.start,
                                      map_out.plan))
      break;
  } while (true); // try again

  // step3: verify # seed positions
  ACE_ASSERT(map_out.seeds.size() == floorPlanConfig_in.num_areas);
}

bool
RPG_Map_Level::load(const std::string& filename_in,
                    RPG_Map_t& map_out,
                    const bool& traceScanning_in,
                    const bool& traceParsing_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::load"));

  // init return value(s)
  map_out.start = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
  map_out.seeds.clear();
  map_out.plan.size_x = 0;
  map_out.plan.size_y = 0;
  map_out.plan.unmapped.clear();
  map_out.plan.walls.clear();
  map_out.plan.doors.clear();
  map_out.plan.rooms_are_square = false;

  // sanity check(s)
  if (!Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF

  // (try to) parse file
  RPG_Map_ParserDriver parser_driver(traceScanning_in,
                                     traceParsing_in);
  parser_driver.init(&map_out.start,   // starting point
                     &map_out.seeds,   // seed points
                     &map_out.plan,    // floor plan
                     traceScanning_in, // trace scanning ?
                     traceParsing_in); // trace parsing ?
  if (!parser_driver.parse(filename_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Map_ParserDriver::parse(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return false;
  } // end IF

  map_out.plan.rooms_are_square = RPG_Map_Common_Tools::roomsAreSquare(map_out);

  return true;
}

void
RPG_Map_Level::random(const RPG_Map_FloorPlan_Configuration_t& floorPlanConfiguration_in,
                      RPG_Map_t& map_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::random"));

  // init return value(s)
  map_out.start = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
  map_out.seeds.clear();
  map_out.plan.size_x = 0;
  map_out.plan.size_y = 0;
  map_out.plan.unmapped.clear();
  map_out.plan.walls.clear();
  map_out.plan.doors.clear();
  map_out.plan.rooms_are_square = floorPlanConfiguration_in.square_rooms;

  RPG_Dice_RollResult_t result;

  RPG_Map_FloorPlan_Configuration_t map_configuration =
      floorPlanConfiguration_in;

  // step1: min room size
  if (map_configuration.min_room_size ==
      std::numeric_limits<unsigned int>::max())
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_MAP_DEF_MIN_ROOM_SIZE_MAX + 1,
                                    1,
                                    result);
    map_configuration.min_room_size = (result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated min room size: %u\n"),
               map_configuration.min_room_size));
  } // end IF

  // step2: doors
  map_configuration.doors = RPG_Dice::probability(0.5F);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("generated doors: %s\n"),
             (map_configuration.doors ? ACE_TEXT_ALWAYS_CHAR("true")
                                      : ACE_TEXT_ALWAYS_CHAR("false"))));

  // step3: corridors
  map_configuration.corridors = RPG_Dice::probability(0.5F);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("generated corridors: %s\n"),
             (map_configuration.corridors ? ACE_TEXT_ALWAYS_CHAR("true")
                                          : ACE_TEXT_ALWAYS_CHAR("false"))));

  // step4: max # doors/room
  if (map_configuration.max_num_doors_per_room == 0) // <-- ! don't care
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(static_cast<unsigned int>(RAND_MAX) + 1,
                                    1,
                                    result);
    map_configuration.max_num_doors_per_room = (result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated max # doors/room: %u\n"),
               map_configuration.max_num_doors_per_room));
  } // end IF

  // step5: maximize rooms
  map_configuration.maximize_rooms = RPG_Dice::probability(0.5F);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("generated maximize rooms: %s\n"),
             (map_configuration.maximize_rooms ? ACE_TEXT_ALWAYS_CHAR("true")
                                               : ACE_TEXT_ALWAYS_CHAR("false"))));

  // step6: # areas
  if (map_configuration.num_areas == 0)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_MAP_DEF_NUM_AREAS_MAX,
                                    1,
                                    result);
    map_configuration.num_areas = result.front();
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated # areas: %u\n"),
               map_configuration.num_areas));
  } // end IF

  // step7: square rooms
  map_configuration.square_rooms = RPG_Dice::probability(0.5F);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("generated square rooms: %s\n"),
             (map_configuration.square_rooms ? ACE_TEXT_ALWAYS_CHAR("true")
                                             : ACE_TEXT_ALWAYS_CHAR("false"))));

  // step8: size
  if ((map_configuration.map_size_x == 0) ||
      (map_configuration.map_size_y == 0))
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_MAP_DEF_SIZE_MAX + 1 -
                                    RPG_MAP_DEF_SIZE_MIN,
                                    2,
                                    result);
    if (map_configuration.map_size_x == 0)
      map_configuration.map_size_x = (RPG_MAP_DEF_SIZE_MIN +
                                      result.front() - 1);
    if (map_configuration.map_size_y == 0)
      map_configuration.map_size_y = (RPG_MAP_DEF_SIZE_MIN +
                                      result.back() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated size: [%u,%u]\n"),
               map_configuration.map_size_x,
               map_configuration.map_size_y));
  } // end IF

  // step9: create floor plan
  RPG_Map_Common_Tools::createFloorPlan(map_configuration.map_size_x,
                                        map_configuration.map_size_y,
                                        map_configuration.num_areas,
                                        map_configuration.square_rooms,
                                        map_configuration.maximize_rooms,
                                        map_configuration.min_room_size,
                                        map_configuration.doors,
                                        map_configuration.corridors,
                                        true, // currently, a door fills one position...
                                        map_configuration.max_num_doors_per_room,
                                        map_out.seeds,
                                        map_out.plan);
  ACE_ASSERT(map_out.seeds.size() == map_configuration.num_areas);

  // step10: generate suitable starting position
  RPG_Dice_RollResult_t result_x, result_y;
  do
  {
    result_x.clear();
    result_y.clear();
    RPG_Dice::generateRandomNumbers(map_out.plan.size_x,
                                    1,
                                    result_x);
    RPG_Dice::generateRandomNumbers(map_out.plan.size_y,
                                    1,
                                    result_y);
    map_out.start = std::make_pair(result_x.front() - 1,
                                   result_y.front() - 1);
    if (RPG_Map_Common_Tools::isFloor(map_out.start,
                                      map_out.plan))
      break;
  } while (true); // try again
}

void
RPG_Map_Level::print(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::print"));

  std::string map_string = RPG_Map_Level::info(map_in);
  std::cout << map_string;
  map_string = RPG_Map_Level::string(map_in);
  std::cout << map_string << std::endl;
}

std::string
RPG_Map_Level::string(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::string"));

  std::ostringstream converter;
  RPG_Map_Position_t current_position;
  RPG_Map_Door_t current_position_door;
  for (unsigned int y = 0;
       y < map_in.plan.size_y;
       y++)
  {
    for (unsigned int x = 0;
         x < map_in.plan.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      if (current_position == map_in.start)
        converter << 'X';
      else if (map_in.seeds.find(current_position) != map_in.seeds.end())
        converter << '@';
      else
      {
        // unmapped, floor, wall, or door ?
        current_position_door.position = current_position;
        if (map_in.plan.unmapped.find(current_position) !=
            map_in.plan.unmapped.end())
          converter << ' '; // unmapped
        else if (map_in.plan.walls.find(current_position) !=
                 map_in.plan.walls.end())
          converter << '#'; // wall
        else if (map_in.plan.doors.find(current_position_door) !=
                 map_in.plan.doors.end())
          converter << '='; // door
        else
          converter << '.'; // floor
      } // end ELSE
    } // end FOR
    converter << std::endl;
  } // end FOR

  return converter.str();
}

std::string
RPG_Map_Level::info(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::info"));

  std::string result;

  std::ostringstream converter;
  result += ACE_TEXT("start position: [");
  converter << map_in.start.first;
  result += converter.str();
  result += ACE_TEXT(",");
  converter.str(ACE_TEXT(""));
  converter << map_in.start.second;
  result += converter.str();
  result += ACE_TEXT("]\n");

  result += ACE_TEXT("seed point(s)[");
  converter.str(ACE_TEXT(""));
  converter << map_in.seeds.size();
  result += converter.str();
  result += ACE_TEXT("]: ");
  RPG_Map_PositionsConstIterator_t next = map_in.seeds.begin();
  for (RPG_Map_PositionsConstIterator_t iterator = map_in.seeds.begin();
       iterator != map_in.seeds.end();
       iterator++)
  {
    result += ACE_TEXT("[");
    converter.str(ACE_TEXT(""));
    converter << (*iterator).first;
    result += converter.str();
    result += ACE_TEXT(",");
    converter.str(ACE_TEXT(""));
    converter << (*iterator).second;
    result += converter.str();
    result += ACE_TEXT("]");

    next = iterator;
    std::advance(next, 1);
    if (next != map_in.seeds.end())
      result += ACE_TEXT(", ");
  } // end FOR
  result += ACE_TEXT("]\n");

  result += ACE_TEXT("map size: [");
  converter.str(ACE_TEXT(""));
  converter << map_in.plan.size_x;
  result += converter.str();
  result += ACE_TEXT(",");
  converter.str(ACE_TEXT(""));
  converter << map_in.plan.size_y;
  result += converter.str();
  result += ACE_TEXT("]\n");

  result += ACE_TEXT("unmapped: ");
  converter.str(ACE_TEXT(""));
  converter << map_in.plan.unmapped.size();
  result += converter.str();
  result += ACE_TEXT("\nwalls: ");
  converter.str(ACE_TEXT(""));
  converter << map_in.plan.walls.size();
  result += converter.str();
  result += ACE_TEXT("\n");

  result += ACE_TEXT("door(s)[");
  converter.str(ACE_TEXT(""));
  converter << map_in.plan.doors.size();
  result += converter.str();
  result += ACE_TEXT("]: ");
  RPG_Map_DoorsConstIterator_t next_door = map_in.plan.doors.begin();
  for (RPG_Map_DoorsConstIterator_t iterator = map_in.plan.doors.begin();
       iterator != map_in.plan.doors.end();
       iterator++)
  {
    result += ACE_TEXT("[");
    converter.str(ACE_TEXT(""));
    converter << (*iterator).position.first;
    result += converter.str();
    result += ACE_TEXT(",");
    converter.str(ACE_TEXT(""));
    converter << (*iterator).position.second;
    result += converter.str();
    result += ACE_TEXT("]");

    next_door = iterator;
    std::advance(next_door, 1);
    if (next_door != map_in.plan.doors.end())
      result += ACE_TEXT(", ");
  } // end FOR
  result += ACE_TEXT("]\n");

  return result;
}

void
RPG_Map_Level::init(const RPG_Map_t& map_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::init"));

  myMap = map_in;
}

void
RPG_Map_Level::save(const std::string& filename_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::save"));

  // sanity check(s)
  if (Common_File_Tools::isReadable(filename_in))
    if (!Common_File_Tools::deleteFile(filename_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));

      return;
    } // end IF

  ACE_FILE_IO fileStream;
  ACE_FILE_Addr file_addr(ACE_TEXT_CHAR_TO_TCHAR(filename_in.c_str()));
  // don't block on file creation (see ACE doc)...
  ACE_Time_Value zero_timeout(0, 0);
  ACE_FILE_Connector file_connector;
  if (file_connector.connect(fileStream,
                             file_addr,
                             &zero_timeout,
                             ACE_Addr::sap_any,
                             0,
                             (O_WRONLY |
                              O_APPEND |
                              O_CREAT  |
                              O_TRUNC  |
                              O_TEXT), // clobber existing files...
                             ACE_DEFAULT_FILE_PERMS) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_Connector::connect(\"%s\"): %m, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("created/openend TXT file: \"%s\"...\n"),
              ACE_TEXT (filename_in.c_str ())));

  ssize_t sent_bytes = 0;
  std::string row;
  RPG_Map_Position_t current_position;
  RPG_Map_Door_t current_position_door;
  for (unsigned int y = 0;
       y < myMap.plan.size_y;
       y++)
  {
    row.clear();
    for (unsigned int x = 0;
         x < myMap.plan.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      current_position_door.position = current_position;

      // starting position ?
      if (current_position == myMap.start)
      {
        row += 'X'; // starting position
        continue;
      } // end IF

      // seed point ?
      if (myMap.seeds.find(current_position) != myMap.seeds.end())
      {
        // *TODO*: with the current (naive) formatting, seed points can only be
        // stored as long as they don't interfere with the integrity of other
        // structures (i.e. walls and doors)
        if ((myMap.plan.walls.find(current_position)      != myMap.plan.walls.end()) ||
            (myMap.plan.doors.find(current_position_door) != myMap.plan.doors.end()))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("cannot save seed position (%u,%u), continuing\n"),
                      current_position.first,
                      current_position.second));
        else
        {
          row += '@'; // seed
          continue;
        } // end ELSE
      } // end IF

      // unmapped, floor, wall, or door ?
      if (myMap.plan.unmapped.find(current_position) !=
          myMap.plan.unmapped.end())
        row += ' '; // unmapped
      else if (myMap.plan.walls.find(current_position) !=
               myMap.plan.walls.end())
        row += '#'; // wall
      else if (myMap.plan.doors.find(current_position_door) !=
               myMap.plan.doors.end())
        row += '='; // door
      else
        row += '.'; // floor
    } // end FOR
    row += '\n';

    // write the row to the file
    sent_bytes = fileStream.send(row.c_str(),
                                 row.size());
    switch (sent_bytes)
    {
      case -1:
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_FILE_IO::send(%u): %m, aborting\n"),
                    row.size ()));

        // clean up
        if (fileStream.close() == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_FILE_IO::close(): %m, aborting\n")));
        } // end IF

        return;
      }
      default:
      {
        if (static_cast<size_t>(sent_bytes) != row.size())
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_FILE_IO::send(%u) (result was: %d), aborting\n"),
                      row.size(),
                      sent_bytes));

          // clean up
          if (fileStream.close() == -1)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_FILE_IO::close(): %m, aborting\n")));
          } // end IF

          return;
        } // end IF

        break;
      }
    } // end SWITCH
  } // end FOR

  // retrieve status info...
  ACE_FILE_Info info;
  if (fileStream.get_info(info) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_FILE_IO::get_info(): %m, aborting\n")));

    // clean up
    if (fileStream.close() == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_IO::close(): %m, aborting\n")));
    } // end IF

    return;
  } // end IF

  // sanity check: empty ?
  if (info.size_ == 0)
  {
    // --> remove() instead of close()...
    if (fileStream.remove() == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_IO::remove(): %m, aborting\n")));

      return;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("removed empty file \"%s\"...\n"),
                ACE_TEXT (filename_in.c_str ())));
  } // end IF
  else
  {
    if (fileStream.close() == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_FILE_IO::close(): %m, aborting\n")));

      return;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("wrote file \"%s\" (%u byte(s))...\n"),
                ACE_TEXT (filename_in.c_str ()),
                static_cast<unsigned int> (info.size_)));
  } // end ELSE
}

void
RPG_Map_Level::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::dump_state"));

  RPG_Map_Level::print(myMap);
}

const RPG_Map_Position_t&
RPG_Map_Level::getStartPosition() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getStartPosition"));

  return myMap.start;
}

const RPG_Map_Positions_t&
RPG_Map_Level::getSeedPoints() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getSeedPoints"));

  return myMap.seeds;
}

const RPG_Map_FloorPlan_t&
RPG_Map_Level::getFloorPlan() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getFloorPlan"));

  return myMap.plan;
}

RPG_Map_Size_t
RPG_Map_Level::getSize() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getSize"));

  return std::make_pair(myMap.plan.size_x, myMap.plan.size_y);
}

bool
RPG_Map_Level::isValid(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::isValid"));

  RPG_Map_Element element = getElement(position_in);
  RPG_Map_DoorState door_state = RPG_MAP_DOORSTATE_INVALID;
  switch (element)
  {
    case MAPELEMENT_FLOOR:
      return true;
    case MAPELEMENT_DOOR:
    {
      door_state = state(position_in);

      return ((door_state == DOORSTATE_OPEN) ||
              (door_state == DOORSTATE_BROKEN));
    }
    default:
      break;
  } // end SWITCH

  return false;
}

bool
RPG_Map_Level::isCorner(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::isCorner"));

  RPG_Map_Position_t east, west, south, north;
  east = position_in;
  east.first++;
  west = position_in;
  west.first--;
  north = position_in;
  north.second--;
  south = position_in;
  south.second++;

  return ((((getElement(west) == MAPELEMENT_FLOOR) ||
            (getElement(west) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_FLOOR) ||
            (getElement(south) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_UNMAPPED) ||
            (getElement(north) == MAPELEMENT_WALL)) &&
           ((getElement(east) == MAPELEMENT_UNMAPPED) ||
            (getElement(east) == MAPELEMENT_WALL))) || // SW
          (((getElement(east) == MAPELEMENT_FLOOR) ||
            (getElement(east) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_FLOOR) ||
            (getElement(south) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_UNMAPPED) ||
            (getElement(north) == MAPELEMENT_WALL)) &&
           ((getElement(west) == MAPELEMENT_UNMAPPED) ||
            (getElement(west) == MAPELEMENT_WALL))) || // SE
          (((getElement(west) == MAPELEMENT_FLOOR) ||
            (getElement(west) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_FLOOR) ||
            (getElement(north) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_UNMAPPED) ||
            (getElement(south) == MAPELEMENT_WALL)) &&
           ((getElement(east) == MAPELEMENT_UNMAPPED) ||
            (getElement(east) == MAPELEMENT_WALL))) || // NW
          (((getElement(east) == MAPELEMENT_FLOOR) ||
            (getElement(east) == MAPELEMENT_DOOR)) &&
           ((getElement(north) == MAPELEMENT_FLOOR) ||
            (getElement(north) == MAPELEMENT_DOOR)) &&
           ((getElement(south) == MAPELEMENT_UNMAPPED) ||
            (getElement(south) == MAPELEMENT_WALL)) &&
           ((getElement(west) == MAPELEMENT_UNMAPPED) ||
            (getElement(west) == MAPELEMENT_WALL)))); // NE
}

RPG_Map_Element
RPG_Map_Level::getElement(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getElement"));

  // sanity check
  if ((position_in.first  == std::numeric_limits<unsigned int>::max()) ||
      (position_in.second == std::numeric_limits<unsigned int>::max()))
    return MAPELEMENT_INVALID;

  // unmapped/off-map ?
  if ((position_in.first  >= myMap.plan.size_x) ||
      (position_in.second >= myMap.plan.size_y) ||
      (myMap.plan.unmapped.find(position_in) != myMap.plan.unmapped.end()))
    return MAPELEMENT_UNMAPPED;

  if (myMap.plan.walls.find(position_in) != myMap.plan.walls.end())
    return MAPELEMENT_WALL;

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  if (myMap.plan.doors.find(position_door) != myMap.plan.doors.end())
    return MAPELEMENT_DOOR;

  return MAPELEMENT_FLOOR;
}

RPG_Map_DoorState
RPG_Map_Level::state(const RPG_Map_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::state"));

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  position_door.outside = RPG_MAP_DIRECTION_INVALID;
  position_door.state = RPG_MAP_DOORSTATE_INVALID;

  RPG_Map_DoorsConstIterator_t iterator = myMap.plan.doors.find(position_door);
  // sanity check
  ACE_ASSERT(iterator != myMap.plan.doors.end());
  if (iterator == myMap.plan.doors.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument (was: [%u,%u]), aborting\n"),
               position_in.first, position_in.second));

    return RPG_MAP_DOORSTATE_INVALID;
  } // end IF

  return (*iterator).state;
}

RPG_Map_Positions_t
RPG_Map_Level::getObstacles() const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::getObstacles"));

  // *NOTE*: obstacles are:
  // - walls
  // - ![open | broken] doors
  RPG_Map_Positions_t result = myMap.plan.walls;
  for (RPG_Map_DoorsConstIterator_t iterator = myMap.plan.doors.begin();
       iterator != myMap.plan.doors.end();
       iterator++)
    if (((*iterator).state != DOORSTATE_OPEN) &&
        ((*iterator).state != DOORSTATE_BROKEN))
      result.insert((*iterator).position);

  return result;
}

bool
RPG_Map_Level::findPath(const RPG_Map_Position_t& start_in,
                        const RPG_Map_Position_t& end_in,
                        RPG_Map_Path_t& path_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::findPath"));

  // init result
  path_out.clear();

  // sanity check
  if (start_in == end_in)
    return true;

  RPG_Map_Pathfinding_Tools::findPath(getSize(),
                                      getObstacles(), // walls & closed doors
                                      start_in,
                                      RPG_MAP_DIRECTION_INVALID,
                                      end_in,
                                      path_out);

  return ((path_out.size() >= 2) &&
          (path_out.front().first == start_in) &&
          (path_out.back().first == end_in));
}

void
RPG_Map_Level::findValid(const RPG_Map_Position_t& center_in,
                         const unsigned int& radius_in,
                         RPG_Map_Positions_t& area_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Level::findValid"));

  // init return value(s)
  area_out.clear();

  // step1: compute square perimeter
  //RPG_Map_Positions_t square;
  RPG_Map_Common_Tools::buildSquare(center_in,
                                    getSize(),
                                    radius_in,
                                    true,
                                    area_out);

  // step2: remove invalid (== blocked) positions
  //RPG_Map_Positions_t invalid;
  //for (RPG_Map_PositionsConstIterator_t iterator = square.begin();
  //     iterator != square.end();
  //     iterator++)
  //  if (!isValid(*iterator))
  //    invalid.insert(*iterator);
  //std::vector<RPG_Map_Position_t> difference(area_out.size());
  //std::vector<RPG_Map_Position_t>::iterator difference_end;
  //difference_end = std::set_difference(area_out.begin(), area_out.end(),
  //                                     invalid.begin(), invalid.end(),
  //                                     difference.begin());
  //area_out.insert(difference.begin(), difference_end);
  // *WARNING*: this works for associative containers ONLY
  for (RPG_Map_PositionsIterator_t iterator = area_out.begin();
       iterator != area_out.end();
       )
    if (isValid(*iterator))
      iterator++;
    else
      area_out.erase(iterator++);
}
