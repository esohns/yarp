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

#include "rpg_engine_level.h"

#include "rpg_engine_defines.h"

#include "rpg_map_parser_driver.h"
#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_monster_dictionary.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_xsderrorhandler.h"
#include "rpg_common_file_tools.h"

#include "rpg_dice_common.h"
#include "rpg_dice.h"

#include <ace/Log_Msg.h>

RPG_Engine_Level::RPG_Engine_Level()
// : myMetaData(),
//   myStyle()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

//  myMetaData.name.clear();

  myMetaData.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  myMetaData.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  myMetaData.environment.outdoors = false;
  myMetaData.environment.plane = RPG_COMMON_PLANE_INVALID;
  myMetaData.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  myMetaData.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;

//  myMetaData.roaming_monsters.clear();
  myMetaData.spawn_interval = ACE_Time_Value::zero;
  myMetaData.spawn_probability = 0.0F;
  myMetaData.max_spawned = 0;
  myMetaData.spawn_timer = -1;
  myMetaData.amble_probability = 0.0F;
}

RPG_Engine_Level::~RPG_Engine_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::~RPG_Engine_Level"));

}

void
RPG_Engine_Level::create(const RPG_Map_FloorPlan_Configuration_t& mapConfig_in,
                         RPG_Engine_Level_t& level_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::create"));

  level_out.metadata.name = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME);

  level_out.metadata.environment.climate =
      RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_CLIMATE;
  level_out.metadata.environment.lighting =
      RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_LIGHTING;
  level_out.metadata.environment.outdoors =
      RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_OUTDOORS;
  level_out.metadata.environment.plane = RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_PLANE;
  level_out.metadata.environment.terrain =
      RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_TERRAIN;
  level_out.metadata.environment.time =
      RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_TIMEOFDAY;

  level_out.metadata.roaming_monsters.clear();
  level_out.metadata.roaming_monsters.push_back(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_AI_DEF_SPAWN_TYPE));
  level_out.metadata.spawn_interval =
      ACE_Time_Value(RPG_ENGINE_LEVEL_AI_DEF_SPAWN_TIMER_INTERVAL, 0);
  level_out.metadata.spawn_probability =
      RPG_ENGINE_LEVEL_AI_DEF_SPAWN_PROBABILITY;
  level_out.metadata.max_spawned = RPG_ENGINE_LEVEL_AI_NUM_SPAWNED_MAX;
  level_out.metadata.spawn_timer = -1;
  level_out.metadata.amble_probability =
      RPG_ENGINE_LEVEL_AI_DEF_AMBLE_PROBABILITY;

  level_out.map.start =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  level_out.map.seeds.clear();
  level_out.map.plan.size_x = 0;
  level_out.map.plan.size_y = 0;
  level_out.map.plan.unmapped.clear();
  level_out.map.plan.walls.clear();
  level_out.map.plan.doors.clear();
  level_out.map.plan.rooms_are_square = false;
  RPG_Map_Level::create(mapConfig_in,
                        level_out.map);
}

bool
RPG_Engine_Level::load(const std::string& filename_in,
                       const std::string& schemaRepository_in,
                       RPG_Engine_Level_t& level_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::load"));

  // init return value(s)
  level_out.metadata.name.clear();

  level_out.metadata.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  level_out.metadata.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  level_out.metadata.environment.outdoors = false;
  level_out.metadata.environment.plane = RPG_COMMON_PLANE_INVALID;
  level_out.metadata.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  level_out.metadata.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;

  level_out.metadata.roaming_monsters.clear();
  level_out.metadata.spawn_interval = ACE_Time_Value::zero;
  level_out.metadata.spawn_probability = 0.0F;
  level_out.metadata.max_spawned = 0;
  level_out.metadata.spawn_timer = -1;
  level_out.metadata.amble_probability = 0.0F;

  level_out.map.start =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  level_out.map.seeds.clear();
  level_out.map.plan.size_x = 0;
  level_out.map.plan.size_y = 0;
  level_out.map.plan.unmapped.clear();
  level_out.map.plan.walls.clear();
  level_out.map.plan.doors.clear();
  level_out.map.plan.rooms_are_square = false;

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // step1: load player character
  std::ifstream ifs;
  ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
//   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  ::xml_schema::properties props;
  std::string base_path;
  // *NOTE*: use the working directory as a fallback...
  if (schemaRepository_in.empty())
    base_path = RPG_Common_File_Tools::getWorkingDirectory();
  else
  {
    // sanity check(s)
    if (!RPG_Common_File_Tools::isDirectory(schemaRepository_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::isDirectory(\"%s\"), aborting\n"),
                 schemaRepository_in.c_str()));

      return false;
    } // end IF

    base_path = schemaRepository_in;
  } // end ELSE
  std::string schemaFile = base_path;
  schemaFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schemaFile += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_SCHEMA_FILE);
  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(schemaFile))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               schemaFile.c_str()));

    return false;
  } // end IF

  props.schema_location(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE),
                        schemaFile);
//   props.no_namespace_schema_location(RPG_CHARACTER_PLAYER_SCHEMA_FILE);
//   props.schema_location("http://www.w3.org/XML/1998/namespace", "xml.xsd");

  std::auto_ptr<RPG_Engine_Level_XMLTree_Type> engine_level_p;
  bool is_level = true;
  try
  {
    ifs.open(filename_in.c_str(),
             std::ios_base::in);

    engine_level_p = ::engine_level_t(ifs,
                                      RPG_XSDErrorHandler,
                                      flags,
                                      props);

    ifs.close();
  }
  catch (std::ifstream::failure const& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Level::load(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               exception.what()));

    return false;
  }
  catch (::xml_schema::parsing const& exception)
  {
    std::ostringstream converter;
    converter << exception;
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Level::load(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               converter.str().c_str()));

    return false;
  }
  catch (::xml_schema::exception const& exception)
  {
    ACE_UNUSED_ARG(exception);

    // *NOTE*: maybe this was a MAP file (expected LEVEL file)
    // --> try parsing that instead...
    RPG_Map_ParserDriver parser_driver(false, false);
    parser_driver.init(&level_out.map.start,
                       &level_out.map.seeds,
                       &level_out.map.plan,
                       false, false);
    if (!parser_driver.parse(filename_in, false))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Map_ParserDriver::parse(\"%s\"), aborting\n"),
                 filename_in.c_str()));

      return false;
    } // end IF

    // OK ! --> proceed
    is_level = false;
    ifs.close();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Level::load(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));

    return false;
  }
  if (is_level)
  {
    ACE_ASSERT(engine_level_p.get());
    RPG_Engine_Level_XMLTree_Type* level_p = engine_level_p.get();
    ACE_ASSERT(level_p);

    level_out = RPG_Engine_Level::levelXMLToLevel(*level_p);
  } // end IF
  level_out.map.plan.rooms_are_square =
      RPG_Map_Common_Tools::roomsAreSquare(level_out.map);

  return true;
}

void
RPG_Engine_Level::random(const RPG_Engine_LevelMetaData_t& metaData_in,
                         const RPG_Map_FloorPlan_Configuration_t& mapConfig_in,
                         RPG_Engine_Level_t& level_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::random"));

  RPG_Dice_RollResult_t result;

  level_out.metadata = metaData_in;
  // step1: name
  if (level_out.metadata.name.empty())
  {
    // generate a string of (random ASCII alphabet, printable) characters
    unsigned int length = 0;
    RPG_Dice::generateRandomNumbers(RPG_ENGINE_LEVEL_NAME_MAX_LENGTH, // maximum length
                                    1,
                                    result);
    length = result.front();
    result.clear();
    RPG_Dice::generateRandomNumbers(26,           // characters in (ASCII) alphabet
                                    (2 * length), // first half are characters, last half interpreted as boolean (upper/lower)
                                    result);
    bool lowercase = false;
    for (unsigned int i = 0;
         i < length;
         i++)
    {
      // upper/lower ?
      if (result[length + i] > 13)
        lowercase = false;
      else
        lowercase = true;

      level_out.metadata.name +=
          static_cast<char>((lowercase ? 96 : 64) + result[i]); // 97 == 'a', 65 == 'A'
    } // end FOR
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated level name: \"%s\"\n"),
               level_out.metadata.name.c_str()));
  } // end IF

  // step2: climate
  if (level_out.metadata.environment.climate == RPG_COMMON_CLIMATE_INVALID)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_COMMON_CLIMATE_MAX,
                                    1,
                                    result);
    level_out.metadata.environment.climate =
        static_cast<RPG_Common_Climate>(result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated level climate: \"%s\"\n"),
               RPG_Common_ClimateHelper::RPG_Common_ClimateToString(level_out.metadata.environment.climate).c_str()));
  } // end IF

  // step3: lighting
  if (level_out.metadata.environment.lighting == RPG_COMMON_AMBIENTLIGHTING_INVALID)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_COMMON_AMBIENTLIGHTING_MAX,
                                    1,
                                    result);
    level_out.metadata.environment.lighting =
        static_cast<RPG_Common_AmbientLighting>(result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated level lighting: \"%s\"\n"),
               RPG_Common_AmbientLightingHelper::RPG_Common_AmbientLightingToString(level_out.metadata.environment.lighting).c_str()));
  } // end IF

  // step4: *TODO*: outdoors
//  result.clear();
//  RPG_Dice::generateRandomNumbers(2,
//                                  1,
//                                  result);
//  level_out.metadata.environment.outdoors = (result.front() - 1);
  level_out.metadata.environment.outdoors = false;

  // step5: plane
  if (level_out.metadata.environment.plane == RPG_COMMON_PLANE_INVALID)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_COMMON_PLANE_MAX,
                                    1,
                                    result);
    level_out.metadata.environment.plane =
        static_cast<RPG_Common_Plane>(result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated level plane: \"%s\"\n"),
               RPG_Common_PlaneHelper::RPG_Common_PlaneToString(level_out.metadata.environment.plane).c_str()));
  } // end IF

  // step6: terrain
  if (level_out.metadata.environment.terrain == RPG_COMMON_TERRAIN_INVALID)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_COMMON_TERRAIN_MAX,
                                    1,
                                    result);
    level_out.metadata.environment.terrain =
        static_cast<RPG_Common_Terrain>(result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated level terrain: \"%s\"\n"),
               RPG_Common_TerrainHelper::RPG_Common_TerrainToString(level_out.metadata.environment.terrain).c_str()));
  } // end IF

  // step7: time of day
  if (level_out.metadata.environment.time == RPG_COMMON_TIMEOFDAY_INVALID)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_COMMON_TIMEOFDAY_MAX,
                                    1,
                                    result);
    level_out.metadata.environment.time =
        static_cast<RPG_Common_TimeOfDay>(result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated level time of day: \"%s\"\n"),
               RPG_Common_TimeOfDayHelper::RPG_Common_TimeOfDayToString(level_out.metadata.environment.time).c_str()));
  } // end IF

  // step8: roaming monster(s)
  if ((level_out.metadata.max_spawned > 0) &&
      level_out.metadata.roaming_monsters.empty())
  {
    unsigned int number = 0;
    RPG_Dice::generateRandomNumbers(RPG_ENGINE_LEVEL_AI_SPAWN_TYPES_MAX + 1,
                                    1,
                                    result);
    number = (result.front() - 1);
    RPG_Monster_List_t monsters =
        RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getEntries();
    while (level_out.metadata.roaming_monsters.size() < number)
    {
      result.clear();
      RPG_Dice::generateRandomNumbers(monsters.size(),
                                      1,
                                      result);

      if (std::find(level_out.metadata.roaming_monsters.begin(),
                    level_out.metadata.roaming_monsters.end(),
                    monsters[result.front() - 1]) ==
          level_out.metadata.roaming_monsters.end())
      {
        level_out.metadata.roaming_monsters.push_back(monsters[result.front() - 1]);
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("generated level monster: \"%s\"\n"),
                   monsters[result.front() - 1].c_str()));
      } // end IF
    } // end WHILE
  } // end IF

  // step9: spawn interval
  if ((level_out.metadata.max_spawned > 0) &&
      (level_out.metadata.spawn_interval == ACE_Time_Value::zero))
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_ENGINE_LEVEL_AI_SPAWN_TIMER_INTERVAL_MAX + 1,
                                    1,
                                    result);
    level_out.metadata.spawn_interval = ACE_Time_Value(result.front() - 1, 0);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated spawn interval: \"%#T\"\n"),
               &(level_out.metadata.spawn_interval)));
  } // end IF

  // step10: spawn probability
  if ((level_out.metadata.max_spawned > 0) &&
      (level_out.metadata.spawn_probability == 0.0F))
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(100,
                                    1,
                                    result);
    level_out.metadata.spawn_probability = (result.front() / 100);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated spawn probability: %0.f%%\n"),
               (level_out.metadata.spawn_probability * 100)));
  } // end IF

  // step11: max # spawned
  if (level_out.metadata.max_spawned == 0)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_ENGINE_LEVEL_AI_NUM_SPAWNED_MAX + 1,
                                    1,
                                    result);
    level_out.metadata.max_spawned = (result.front() - 1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated max # spawned: %u\n"),
               level_out.metadata.max_spawned));
  } // end IF

  level_out.metadata.spawn_timer = -1;

  // step12: amble probability
  if ((level_out.metadata.max_spawned > 0) &&
      (level_out.metadata.amble_probability == 0.0F))
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(100 + 1,
                                    1,
                                    result);
    level_out.metadata.amble_probability = (result.front() - 1) / 100;
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated amble probability: %0.f%%\n"),
               (level_out.metadata.amble_probability * 100)));
  } // end IF

  // step13: map
  RPG_Map_Level::random(mapConfig_in,
                        level_out.map);
}

void
RPG_Engine_Level::print(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::print"));

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("level: \"%s\"\n"),
             level_in.metadata.name.c_str()));
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("environment:\n\tplane: %s\n\tterrain: %s\n\tclimate: %s\n\ttime: %s\n\tlighting: %s\n\toutdoors: %s\n"),
             RPG_Common_PlaneHelper::RPG_Common_PlaneToString(level_in.metadata.environment.plane).c_str(),
             RPG_Common_TerrainHelper::RPG_Common_TerrainToString(level_in.metadata.environment.terrain).c_str(),
             RPG_Common_ClimateHelper::RPG_Common_ClimateToString(level_in.metadata.environment.climate).c_str(),
             RPG_Common_TimeOfDayHelper::RPG_Common_TimeOfDayToString(level_in.metadata.environment.time).c_str(),
             RPG_Common_AmbientLightingHelper::RPG_Common_AmbientLightingToString(level_in.metadata.environment.lighting).c_str(),
             (level_in.metadata.environment.outdoors ? ACE_TEXT_ALWAYS_CHAR("true")
                                                     : ACE_TEXT_ALWAYS_CHAR("false"))));
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("monsters:\n\troaming monsters:\n")));
  for (RPG_Monster_ListConstIterator_t iterator =
       level_in.metadata.roaming_monsters.begin();
       iterator != level_in.metadata.roaming_monsters.end();
       iterator++)
     ACE_DEBUG((LM_INFO,
                ACE_TEXT("\t\t* %s\n"),
                (*iterator).c_str()));
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("\tspawn interval: %#T\n\tspawn probability (%%): %.2f\n\tmax # spawned: %u\n\tspawn timer ID: %d\n"),
             &(level_in.metadata.spawn_interval),
             (level_in.metadata.spawn_probability * 100.0F),
             level_in.metadata.max_spawned,
             level_in.metadata.spawn_timer));

  RPG_Map_Level::print(level_in.map);
}

void
RPG_Engine_Level::init(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::init"));

  inherited::init(level_in.map);
  myMetaData = level_in.metadata;
}

void
RPG_Engine_Level::save(const std::string& filename_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::save"));

  // sanity check(s)
  if (RPG_Common_File_Tools::isReadable(filename_in))
  {
    // *TODO*: warn user ?
//     if (!RPG_Common_File_Tools::deleteFile(filename_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
//                  filename_in.c_str()));
    //
//       return false;
//     } // end IF
  } // end IF

  std::ofstream ofs;
  ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
  ::xml_schema::namespace_infomap map;
  map[""].name = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE);
  map[""].schema = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_SCHEMA_FILE);
  std::string character_set(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_SCHEMA_CHARSET));
  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;

  RPG_Engine_Level_XMLTree_Type* level_xml_p = toLevelXML();
  ACE_ASSERT(level_xml_p);

  try
  {
    ofs.open(filename_in.c_str(),
             (std::ios_base::out | std::ios_base::trunc));

    ::engine_level_t(ofs,
                     *level_xml_p,
                     map,
                     character_set,
                     flags);

    ofs.close();
  }
  catch (const std::ofstream::failure& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("\"%s\": unable to open or write error, aborting\n"),
               filename_in.c_str()));

    // clean up
    delete level_xml_p;

    throw exception;
  }
  catch (const ::xml_schema::serialization& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::saveLevel(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               text.c_str()));

    // clean up
    delete level_xml_p;

    throw exception;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::saveLevel(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));

    // clean up
    delete level_xml_p;

    throw;
  }

  // clean up
  delete level_xml_p;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("saved level \"%s\" to file: \"%s\"\n"),
             myMetaData.name.c_str(),
             filename_in.c_str()));
}

void
RPG_Engine_Level::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::dump_state"));

  RPG_Engine_Level_t level = {myMetaData, inherited::myMap};

  RPG_Engine_Level::print(level);
}

RPG_Engine_LevelMetaData_t
RPG_Engine_Level::getMetaData() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getMetaData"));

  return myMetaData;
}

bool
RPG_Engine_Level::handleDoor(const RPG_Map_Position_t& position_in,
                             const bool& open_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleDoor"));

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  position_door.outside = DIRECTION_INVALID;
  position_door.state = RPG_MAP_DOORSTATE_INVALID;
  RPG_Map_DoorsIterator_t iterator = myMap.plan.doors.find(position_door);
  // sanity check
  ACE_ASSERT(iterator != myMap.plan.doors.end());

  if (open_in)
  {
    // sanity check
    if ((*iterator).state == DOORSTATE_OPEN)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already open...\n"),
//                  position_in.first,
//                  position_in.second));

      return false;
    } // end IF

    // cannot simply open locked doors...
    if ((*iterator).state == DOORSTATE_LOCKED)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("door [%u,%u] is locked...\n"),
                 position_in.first,
                 position_in.second));

      return false;
    } // end IF

    // cannot open broken doors...
    if ((*iterator).state == DOORSTATE_BROKEN)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("door [%u,%u] is broken...\n"),
                 position_in.first,
                 position_in.second));

      return false;
    } // end IF
  } // end IF
  else
  {
    // sanity check
    if (((*iterator).state == DOORSTATE_CLOSED) ||
        ((*iterator).state == DOORSTATE_LOCKED))
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already closed...\n"),
//                  position_in.first,
//                  position_in.second));

      return false;
    } // end IF

    // cannot close broken doors...
    if ((*iterator).state == DOORSTATE_BROKEN)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("door [%u,%u] is broken...\n"),
                 position_in.first,
                 position_in.second));

      return false;
    } // end IF
  } // end ELSE

  // *WARNING*: set iterators are CONST for a good reason !
  // --> (but we know what we're doing)...
  (const_cast<RPG_Map_Door_t&>(*iterator)).state = (open_in ? DOORSTATE_OPEN
                                                            : DOORSTATE_CLOSED);

  return true;
}

bool
RPG_Engine_Level::findPath(const RPG_Map_Position_t& start_in,
                           const RPG_Map_Position_t& end_in,
                           const RPG_Map_Positions_t& obstacles_in,
                           RPG_Map_Path_t& path_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::findPath"));

  // init result
  path_out.clear();

  // sanity check
  if (start_in == end_in)
    return true;

  RPG_Map_Pathfinding_Tools::findPath(getSize(),
                                      obstacles_in,
                                      start_in,
                                      DIRECTION_INVALID,
                                      end_in,
                                      path_out);

  return ((path_out.size() >= 2) &&
          (path_out.front().first == start_in) &&
          (path_out.back().first == end_in));
}

RPG_Engine_Level_XMLTree_Type*
RPG_Engine_Level::toLevelXML() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::toLevelXML"));

  RPG_Common_Environment_XMLTree_Type environment;
  if (myMetaData.environment.plane != RPG_COMMON_PLANE_INVALID)
    environment.plane(static_cast<RPG_Common_Plane_XMLTree_Type::value>(myMetaData.environment.plane));
  if (myMetaData.environment.terrain != RPG_COMMON_TERRAIN_INVALID)
    environment.terrain(static_cast<RPG_Common_Terrain_XMLTree_Type::value>(myMetaData.environment.terrain));
  if (myMetaData.environment.climate != RPG_COMMON_CLIMATE_INVALID)
    environment.climate(static_cast<RPG_Common_Climate_XMLTree_Type::value>(myMetaData.environment.climate));
  if (myMetaData.environment.time != RPG_COMMON_TIMEOFDAY_INVALID)
    environment.time(static_cast<RPG_Common_TimeOfDay_XMLTree_Type::value>(myMetaData.environment.time));
  if (myMetaData.environment.lighting != RPG_COMMON_AMBIENTLIGHTING_INVALID)
    environment.lighting(static_cast<RPG_Common_AmbientLighting_XMLTree_Type::value>(myMetaData.environment.lighting));
  environment.outdoors(myMetaData.environment.outdoors);

  RPG_Common_FixedPeriod_XMLTree_Type spawn_interval(static_cast<unsigned int>(myMetaData.spawn_interval.sec()),
                                                     static_cast<unsigned int>(myMetaData.spawn_interval.usec()));

  std::string map_string = RPG_Map_Common_Tools::map2String(inherited::myMap);

  RPG_Engine_Level_XMLTree_Type* level_p = NULL;
  ACE_NEW_NORETURN(level_p,
                   RPG_Engine_Level_XMLTree_Type(myMetaData.name,
                                                 environment,
                                                 spawn_interval,
                                                 myMetaData.spawn_probability,
                                                 myMetaData.max_spawned,
                                                 myMetaData.amble_probability,
                                                 map_string));
  if (!level_p)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return NULL;
  }

  // *NOTE*: add monster sequence "manually"
  for (RPG_Monster_ListConstIterator_t iterator =
       myMetaData.roaming_monsters.begin();
       iterator != myMetaData.roaming_monsters.end();
       iterator++)
    level_p->monster().push_back(*iterator);
	// *NOTE*: add door sequence "manually"
	for (RPG_Map_DoorsConstIterator_t iterator = myMap.plan.doors.begin();
       iterator != myMap.plan.doors.end();
       iterator++)
	{
		RPG_Map_DoorState_XMLTree_Type state(static_cast<RPG_Map_DoorState_XMLTree_Type::value>((*iterator).state));
		RPG_Map_Door_XMLTree_Type temp(RPG_Map_Position_XMLTree_Type((*iterator).position.first,
			                                                           (*iterator).position.second),
																   state);
    level_p->door().push_back(temp);
	} // end FOR

  return level_p;
}

RPG_Engine_Level_t
RPG_Engine_Level::levelXMLToLevel(const RPG_Engine_Level_XMLTree_Type& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::levelXMLToLevel"));

	// init return value
  RPG_Engine_Level_t result;

  result.metadata.name = level_in.name();

  const RPG_Common_Environment_XMLTree_Type& environment =
      level_in.environment();
  result.metadata.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  if (environment.climate().present())
    result.metadata.environment.climate =
        RPG_Common_ClimateHelper::stringToRPG_Common_Climate(environment.climate().get());
  result.metadata.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  if (environment.lighting().present())
    result.metadata.environment.lighting =
        RPG_Common_AmbientLightingHelper::stringToRPG_Common_AmbientLighting(environment.lighting().get());
  result.metadata.environment.plane = RPG_COMMON_PLANE_INVALID;
  if (environment.plane().present())
    result.metadata.environment.plane =
        RPG_Common_PlaneHelper::stringToRPG_Common_Plane(environment.plane().get());
  result.metadata.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  if (environment.terrain().present())
    result.metadata.environment.terrain =
        RPG_Common_TerrainHelper::stringToRPG_Common_Terrain(environment.terrain().get());
  result.metadata.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;
  if (environment.time().present())
    result.metadata.environment.time =
        RPG_Common_TimeOfDayHelper::stringToRPG_Common_TimeOfDay(environment.time().get());
  if (environment.outdoors().present())
    result.metadata.environment.outdoors = environment.outdoors().get();
  else
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("environment.outdoors not set (assuming \"false\"), continuing\n")));

  for (RPG_Engine_Level_XMLTree_Type::monster_const_iterator iterator =
       level_in.monster().begin();
       iterator != level_in.monster().end();
       iterator++)
    result.metadata.roaming_monsters.push_back(*iterator);
  //result.metadata.roaming_monsters.insert(level_in.monster().begin(), level_in.monster().end());
  unsigned int u_seconds = 0;
  if (level_in.spawn_interval().u_seconds().present())
    u_seconds = level_in.spawn_interval().u_seconds().get();
  result.metadata.spawn_interval.set(level_in.spawn_interval().seconds(),
                                       u_seconds);
  result.metadata.spawn_probability = level_in.spawn_probability();
  result.metadata.max_spawned = level_in.max_spawned();
  result.metadata.spawn_timer = -1;
  result.metadata.amble_probability = level_in.amble_probability();

  RPG_Map_ParserDriver parser_driver(false, false);
  parser_driver.init(&result.map.start,
                     &result.map.seeds,
                     &result.map.plan,
                     false, false);
  if (!parser_driver.parse(level_in.map(), true))
	{
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse map, aborting\n")));

		// clean up
		ACE_OS::memset(&result, 0, sizeof(result));

		return result;
	} // end IF

	RPG_Map_Door_t temp;
	RPG_Map_DoorsIterator_t iterator;
	for (RPG_Engine_Level_XMLTree_Type::door_const_iterator iterator2 =
			 level_in.door().begin();
       iterator2 != level_in.door().end();
       iterator2++)
	{
		temp.position = std::make_pair((*iterator2).position().x(),
			                             (*iterator2).position().y());
		iterator = result.map.plan.doors.find(temp);
		if (iterator == result.map.plan.doors.end())
		{
			ACE_DEBUG((LM_ERROR,
								 ACE_TEXT("could not find door (position: [%u,%u]), aborting\n"),
								 temp.position.first,
								 temp.position.second));

			// clean up
			ACE_OS::memset(&result, 0, sizeof(result));

			return result;
		} // end IF
		RPG_Map_DoorState_XMLTree_Type::value temp_state = (*iterator2).state();
		(*iterator).state = static_cast<RPG_Map_DoorState>(temp_state);
	} // end FOR

  return result;
}
