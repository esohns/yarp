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

#include "rpg_map_direction.h"
#include "rpg_map_parser_driver.h"
#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_monster_common.h"
#include "rpg_monster_dictionary.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_xsderrorhandler.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_dice_common.h"
#include "rpg_dice.h"

#include "ace/Log_Msg.h"

RPG_Engine_Level::RPG_Engine_Level()
// : inherited(),
//   myMetaData(),
//   myStyle()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

//  myMetaData.name.clear();

  myMetaData.environment.climate  = RPG_COMMON_CLIMATE_INVALID;
  myMetaData.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  myMetaData.environment.outdoors = false;
  myMetaData.environment.plane    = RPG_COMMON_PLANE_INVALID;
  myMetaData.environment.terrain  = RPG_COMMON_TERRAIN_INVALID;
  myMetaData.environment.time     = RPG_COMMON_TIMEOFDAY_INVALID;

//  myMetaData.spawns.clear();
  myMetaData.max_num_spawned = std::numeric_limits<unsigned int>::max();
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
      RPG_ENGINE_ENVIRONMENT_DEF_CLIMATE;
  level_out.metadata.environment.lighting =
      RPG_ENGINE_ENVIRONMENT_DEF_LIGHTING;
  level_out.metadata.environment.outdoors =
      RPG_ENGINE_ENVIRONMENT_DEF_OUTDOORS;
  level_out.metadata.environment.plane = RPG_ENGINE_ENVIRONMENT_DEF_PLANE;
  level_out.metadata.environment.terrain =
      RPG_ENGINE_ENVIRONMENT_DEF_TERRAIN;
  level_out.metadata.environment.time =
      RPG_ENGINE_ENVIRONMENT_DEF_TIMEOFDAY;

  level_out.metadata.spawns.clear();
  RPG_Engine_Spawn_t default_spawn;
  default_spawn.spawn.type = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENCOUNTER_DEF_TYPE);
  default_spawn.spawn.interval.seconds = RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL;
  default_spawn.spawn.interval.u_seconds = 0;
  default_spawn.spawn.probability = RPG_ENGINE_ENCOUNTER_DEF_PROBABILITY;
  default_spawn.spawn.max_num_spawned = RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX;
  default_spawn.spawn.amble_probability = RPG_ENGINE_ENCOUNTER_DEF_AMBLE_PROBABILITY;
  default_spawn.timer_id = -1;
  level_out.metadata.spawns.push_back(default_spawn);
  level_out.metadata.max_num_spawned = RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX;

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

  level_out.metadata.spawns.clear();
  level_out.metadata.max_num_spawned = 0;

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
                 ACE_TEXT(schemaRepository_in.c_str())));

      return false;
    } // end IF

    base_path = schemaRepository_in;
  } // end ELSE
  std::string schema_filename = base_path;
  schema_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_SCHEMA_FILE);
  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(schema_filename))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               ACE_TEXT(schema_filename.c_str())));

    return false;
  } // end IF
	// *NOTE*: support paths with spaces
	schema_filename = RPG_Common_Tools::sanitizeURI(schema_filename);
	schema_filename.insert(0, ACE_TEXT_ALWAYS_CHAR("file:///"));

	std::string target_name_space =
		ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE);
  props.schema_location(target_name_space,
                        schema_filename);
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
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(exception.what())));

    return false;
  }
  catch (::xml_schema::parsing const& exception)
  {
    std::ostringstream converter;
    converter << exception;
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Level::load(\"%s\"): exception occurred: \"%s\", aborting\n"),
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(converter.str().c_str())));

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
                 ACE_TEXT(filename_in.c_str())));

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
               ACE_TEXT(filename_in.c_str())));

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

bool
RPG_Engine_Level::save(const std::string& filename_in,
                       const RPG_Engine_Level_t& level_in)
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
//                  ACE_TEXT(filename_in.c_str())));
    //
//       return false;
//     } // end IF
  } // end IF

  std::ofstream ofs;
  ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
  try
  {
    ofs.open(filename_in.c_str(),
             (std::ios_base::out | std::ios_base::trunc));
  }
  catch (std::ios_base::failure exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to std::ofstream::open(\"%s\"): \"%s\", aborting\n"),
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(exception.what())));

    return false;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to std::ofstream::open(\"%s\"), aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  }

  RPG_Engine_Level_XMLTree_Type* level_xml_p =
      RPG_Engine_Level::levelToLevelXML(level_in);
  if (!level_xml_p)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::levelToLevelXML(), aborting\n")));

    // clean up
    try
    {
      ofs.close();
    }
    catch (std::ios_base::failure exception)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                 ACE_TEXT(filename_in.c_str()),
                 ACE_TEXT(exception.what())));
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"), aborting\n"),
                 ACE_TEXT(filename_in.c_str())));
    }

    return false;
  } // end IF

  ::xml_schema::namespace_infomap map;
  map[""].name = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE);
  map[""].schema = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_SCHEMA_FILE);
  std::string character_set(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_SCHEMA_CHARSET));
  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  try
  {
    ::engine_level_t(ofs,
                     *level_xml_p,
                     map,
                     character_set,
                     flags);
  }
  catch (std::ios_base::failure exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::engine_level_t(\"%s\"): \"%s\", aborting\n"),
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(exception.what())));

    // clean up
    try
    {
      ofs.close();
    }
    catch (std::ios_base::failure exception)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                 ACE_TEXT(filename_in.c_str()),
                 ACE_TEXT(exception.what())));
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"), aborting\n"),
                 ACE_TEXT(filename_in.c_str())));
    }
    delete level_xml_p;

    return false;
  }
  catch (::xml_schema::serialization& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::engine_level_t(\"%s\"): \"%s\", aborting\n"),
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(text.c_str())));

    // clean up
    try
    {
      ofs.close();
    }
    catch (std::ios_base::failure exception)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                 ACE_TEXT(filename_in.c_str()),
                 ACE_TEXT(exception.what())));
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"), aborting\n"),
                 ACE_TEXT(filename_in.c_str())));
    }
    delete level_xml_p;

    return false;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::engine_level_t(\"%s\"), aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    // clean up
    try
    {
      ofs.close();
    }
    catch (std::ios_base::failure exception)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
                 ACE_TEXT(filename_in.c_str()),
                 ACE_TEXT(exception.what())));
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to std::ofstream::close(\"%s\"), aborting\n"),
                 ACE_TEXT(filename_in.c_str())));
    }
    delete level_xml_p;

    return false;
  }
  try
  {
    ofs.close();
  }
  catch (std::ios_base::failure exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to std::ofstream::close(\"%s\"): \"%s\", aborting\n"),
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(exception.what())));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to std::ofstream::close(\"%s\"), aborting\n"),
               ACE_TEXT(filename_in.c_str())));
  }
  delete level_xml_p;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("saved level \"%s\" to file: \"%s\"\n"),
             ACE_TEXT(level_in.metadata.name.c_str()),
             ACE_TEXT(filename_in.c_str())));

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
               ACE_TEXT(level_out.metadata.name.c_str())));
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
               ACE_TEXT(RPG_Common_ClimateHelper::RPG_Common_ClimateToString(level_out.metadata.environment.climate).c_str())));
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
               ACE_TEXT(RPG_Common_AmbientLightingHelper::RPG_Common_AmbientLightingToString(level_out.metadata.environment.lighting).c_str())));
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
               ACE_TEXT(RPG_Common_PlaneHelper::RPG_Common_PlaneToString(level_out.metadata.environment.plane).c_str())));
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
               ACE_TEXT(RPG_Common_TerrainHelper::RPG_Common_TerrainToString(level_out.metadata.environment.terrain).c_str())));
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
               ACE_TEXT(RPG_Common_TimeOfDayHelper::RPG_Common_TimeOfDayToString(level_out.metadata.environment.time).c_str())));
  } // end IF

  // step8: spawns
  RPG_Character_Alignment alignment;
  alignment.civic = ALIGNMENTCIVIC_ANY;
  alignment.ethic = ALIGNMENTETHIC_ANY;
  RPG_Monster_OrganizationSet_t organizations;
  organizations.insert(ORGANIZATION_ANY);
  RPG_Monster_HitDice hit_dice;
  hit_dice.modifier = 0;
  hit_dice.numDice = 1;
  // *TODO*: specify HD
  hit_dice.typeDice = D_6;
  RPG_Monster_List_t possible;
  RPG_MONSTER_DICTIONARY_SINGLETON::instance()->find(alignment,
                                                     level_out.metadata.environment,
                                                     organizations,
                                                     hit_dice,
                                                     possible);
  if (!possible.empty())
  {
    unsigned int number = 0;
    RPG_Dice::generateRandomNumbers((possible.size() < RPG_ENGINE_ENCOUNTER_DEF_TYPES_MAX) ? (possible.size() + 1)
                                                                                           : (RPG_ENGINE_ENCOUNTER_DEF_TYPES_MAX + 1),
                                    1,
                                    result);
    number = (result.front() - 1);

    RPG_Engine_Spawn_t spawn;
    RPG_Monster_ListConstIterator_t iterator;
    bool found = false;
    while (level_out.metadata.spawns.size() < number)
    {
      iterator = possible.begin();
      result.clear();
      RPG_Dice::generateRandomNumbers(possible.size(),
                                      1,
                                      result);
      std::advance(iterator, (result.front() - 1));
      found = false;
      for (RPG_Engine_SpawnsConstIterator_t iterator2 = level_out.metadata.spawns.begin();
           iterator2 != level_out.metadata.spawns.end();
           iterator2++)
        if (*iterator == (*iterator2).spawn.type)
        {
          found = true;
          break;
        } // end IF
      if (found)
        break; // already using this type...

      spawn.spawn.type = *iterator;

      result.clear();
      RPG_Dice::generateRandomNumbers(RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL_MAX,
                                      1,
                                      result);
      spawn.spawn.interval.seconds = result.front();
      spawn.spawn.interval.u_seconds = 0;

      result.clear();
      RPG_Dice::generateRandomNumbers(100,
                                      1,
                                      result);
      spawn.spawn.probability = (static_cast<float>(result.front()) / 100.0F);

      result.clear();
      RPG_Dice::generateRandomNumbers(RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX,
                                      1,
                                      result);
      spawn.spawn.max_num_spawned = result.front();

      result.clear();
      RPG_Dice::generateRandomNumbers(100 + 1,
                                      1,
                                      result);
      spawn.spawn.amble_probability =
          (static_cast<float>(result.front() - 1) / 100.0F);

      spawn.timer_id = -1;

      level_out.metadata.spawns.push_back(spawn);
    } // end WHILE
  } // end IF
  else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("found no suitable monsters, continuing\n")));

  // step9: max # spawned (total)
  result.clear();
  RPG_Dice::generateRandomNumbers(RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX + 1,
                                  1,
                                  result);
  level_out.metadata.max_num_spawned = (result.front() - 1);

  // step13: map
  RPG_Map_Level::random(mapConfig_in,
                        level_out.map);
}

void
RPG_Engine_Level::print(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::print"));

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("level:\t\t\"%s\"\n"),
             ACE_TEXT(level_in.metadata.name.c_str())));
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("environment:\n\tplane:\t%s\n\tterrain:\t%s\n\tclimate:\t%s\n\ttime:\t%s\n\tlighting:\t%s\n\toutdoors:\t%s\n"),
             ACE_TEXT(RPG_Common_PlaneHelper::RPG_Common_PlaneToString(level_in.metadata.environment.plane).c_str()),
             ACE_TEXT(RPG_Common_TerrainHelper::RPG_Common_TerrainToString(level_in.metadata.environment.terrain).c_str()),
             ACE_TEXT(RPG_Common_ClimateHelper::RPG_Common_ClimateToString(level_in.metadata.environment.climate).c_str()),
             ACE_TEXT(RPG_Common_TimeOfDayHelper::RPG_Common_TimeOfDayToString(level_in.metadata.environment.time).c_str()),
             ACE_TEXT(RPG_Common_AmbientLightingHelper::RPG_Common_AmbientLightingToString(level_in.metadata.environment.lighting).c_str()),
             (level_in.metadata.environment.outdoors ? ACE_TEXT("true")
                                                     : ACE_TEXT("false"))));
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("monsters:\nspawns\t[interval(sec,usec), probability(%%), max #, amble probability(%%), timer ID]:\n")));
  unsigned int i = 1;
  ACE_Time_Value time_value;
  for (RPG_Engine_SpawnsConstIterator_t iterator =
       level_in.metadata.spawns.begin();
       iterator != level_in.metadata.spawns.end();
       iterator++, i++)
  {
    time_value.set((*iterator).spawn.interval.seconds,
                   (*iterator).spawn.interval.u_seconds);
     ACE_DEBUG((LM_INFO,
                ACE_TEXT("\t#%u %s: %#T\t%.0f\t%u\t%.0f\t%d\n"),
                i, ACE_TEXT((*iterator).spawn.type.c_str()),
                &time_value,
                ((*iterator).spawn.probability * 100.0F),
                (*iterator).spawn.max_num_spawned,
                ((*iterator).spawn.amble_probability * 100.0F),
                (*iterator).timer_id));
  } // end FOR
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("max # (total): %u\n"),
             level_in.metadata.max_num_spawned));

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

  RPG_Engine_Level_t level;
  level.map = inherited::myMap;
  level.metadata = myMetaData;
  if (!RPG_Engine_Level::save(filename_in,
                              level))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::save(\"%s\"), returning\n"),
               ACE_TEXT(filename_in.c_str())));
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
  position_door.outside = RPG_MAP_DIRECTION_INVALID;
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
                                      RPG_MAP_DIRECTION_INVALID,
                                      end_in,
                                      path_out);

  return ((path_out.size() >= 2) &&
          (path_out.front().first == start_in) &&
          (path_out.back().first == end_in));
}

RPG_Engine_Level_XMLTree_Type*
RPG_Engine_Level::levelToLevelXML(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::levelToLevelXML"));

  RPG_Common_Environment_XMLTree_Type environment;
  if (level_in.metadata.environment.plane != RPG_COMMON_PLANE_INVALID)
    environment.plane(static_cast<RPG_Common_Plane_XMLTree_Type::value>(level_in.metadata.environment.plane));
  if (level_in.metadata.environment.terrain != RPG_COMMON_TERRAIN_INVALID)
    environment.terrain(static_cast<RPG_Common_Terrain_XMLTree_Type::value>(level_in.metadata.environment.terrain));
  if (level_in.metadata.environment.climate != RPG_COMMON_CLIMATE_INVALID)
    environment.climate(static_cast<RPG_Common_Climate_XMLTree_Type::value>(level_in.metadata.environment.climate));
  if (level_in.metadata.environment.time != RPG_COMMON_TIMEOFDAY_INVALID)
    environment.time(static_cast<RPG_Common_TimeOfDay_XMLTree_Type::value>(level_in.metadata.environment.time));
  if (level_in.metadata.environment.lighting != RPG_COMMON_AMBIENTLIGHTING_INVALID)
    environment.lighting(static_cast<RPG_Common_AmbientLighting_XMLTree_Type::value>(level_in.metadata.environment.lighting));
  environment.outdoors(level_in.metadata.environment.outdoors);

  RPG_Engine_Level_XMLTree_Type* level_p = NULL;
  ACE_NEW_NORETURN(level_p,
                   RPG_Engine_Level_XMLTree_Type(level_in.metadata.name,
                                                 environment,
                                                 RPG_Map_Common_Tools::map2String(level_in.map),
                                                 level_in.metadata.max_num_spawned));
  if (!level_p)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u), aborting\n"),
               sizeof(RPG_Engine_Level_XMLTree_Type)));

    return NULL;
  } // end IF

  // *NOTE*: add monster sequence "manually"
  RPG_Engine_Level_XMLTree_Type::spawn_sequence& spawns =
      level_p->spawn();
  for (RPG_Engine_SpawnsConstIterator_t iterator = level_in.metadata.spawns.begin();
       iterator != level_in.metadata.spawns.end();
       iterator++)
  {
    RPG_Common_FixedPeriod_XMLTree_Type spawn_interval((*iterator).spawn.interval.seconds,
                                                       (*iterator).spawn.interval.u_seconds);

    RPG_Monster_Spawn_XMLTree_Type spawn((*iterator).spawn.type,
                                         spawn_interval,
                                         (*iterator).spawn.probability,
                                         (*iterator).spawn.max_num_spawned,
                                         (*iterator).spawn.amble_probability);
    spawns.push_back(spawn);
  } // end FOR
  level_p->spawn(spawns);

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

  RPG_Engine_Spawn_t spawn;
  const RPG_Engine_Level_XMLTree_Type::spawn_sequence& spawns =
      level_in.spawn();
  for (RPG_Engine_Level_XMLTree_Type::spawn_const_iterator iterator =
       spawns.begin();
       iterator != spawns.end();
       iterator++)
  {
    spawn.spawn.type = (*iterator).type();
    const RPG_Common_FixedPeriod_XMLTree_Type& interval =
        (*iterator).interval();
    spawn.spawn.interval.seconds = interval.seconds();
		spawn.spawn.interval.u_seconds = 0;
    if (interval.u_seconds().present())
      spawn.spawn.interval.u_seconds = interval.u_seconds().get();
    spawn.spawn.probability = (*iterator).probability();
    spawn.spawn.max_num_spawned = (*iterator).max_num_spawned();
    spawn.spawn.amble_probability = (*iterator).amble_probability();
    spawn.timer_id = -1;
    result.metadata.spawns.push_back(spawn);
  } // end IF

  result.metadata.max_num_spawned = level_in.max_num_spawned();

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

  return result;
}
