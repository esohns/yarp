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

#include <rpg_map_parser_driver.h>
#include <rpg_map_common_tools.h>
#include <rpg_map_pathfinding_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_xsderrorhandler.h>
#include <rpg_common_file_tools.h>

#include <ace/Log_Msg.h>

RPG_Engine_Level::RPG_Engine_Level()
// : myLevelMeta()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

  myLevelMeta.name = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_DEF_LEVEL_NAME);
  myLevelMeta.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  myLevelMeta.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  myLevelMeta.environment.outdoors = false;
  myLevelMeta.environment.plane = RPG_COMMON_PLANE_INVALID;
  myLevelMeta.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  myLevelMeta.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;
//  myLevelMeta.roaming_monsters();
  myLevelMeta.spawn_interval = ACE_Time_Value::zero;
  myLevelMeta.spawn_probability = 0.0F;
  myLevelMeta.max_spawned = 0;
  myLevelMeta.spawn_timer = -1;
}

RPG_Engine_Level::~RPG_Engine_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::~RPG_Engine_Level"));

}

void
RPG_Engine_Level::create(const RPG_Map_FloorPlan_Config_t& mapConfig_in,
                         RPG_Engine_Level_t& level_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::create"));

  level_out.level_meta.name = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_DEF_LEVEL_NAME);
  level_out.level_meta.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  level_out.level_meta.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  level_out.level_meta.environment.outdoors = false;
  level_out.level_meta.environment.plane = RPG_COMMON_PLANE_INVALID;
  level_out.level_meta.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  level_out.level_meta.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;
  level_out.level_meta.roaming_monsters.clear();
  level_out.level_meta.spawn_interval = ACE_Time_Value::zero;
  level_out.level_meta.spawn_probability = 0.0F;
  level_out.level_meta.max_spawned = 0;
  level_out.level_meta.spawn_timer = -1;
  RPG_Map_Level::create(mapConfig_in,
                        level_out.map);
}

RPG_Engine_Level_t
RPG_Engine_Level::load(const std::string& filename_in,
                       const std::string& schemaRepository_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::load"));

  RPG_Engine_Level_t result;
  result.level_meta.name = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_DEF_LEVEL_NAME);
  result.level_meta.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  result.level_meta.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  result.level_meta.environment.outdoors = false;
  result.level_meta.environment.plane = RPG_COMMON_PLANE_INVALID;
  result.level_meta.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  result.level_meta.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;
//  result.level_meta.roaming_monsters();
  result.level_meta.spawn_interval = ACE_Time_Value::zero;
  result.level_meta.spawn_probability = 0.0F;
  result.level_meta.max_spawned = 0;
  result.level_meta.spawn_timer = -1;
  result.map.start = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                    std::numeric_limits<unsigned int>::max());
  result.map.seeds.clear();
  result.map.plan.size_x = 0;
  result.map.plan.size_y = 0;
  result.map.plan.unmapped.clear();
  result.map.plan.walls.clear();
  result.map.plan.doors.clear();
  result.map.plan.rooms_are_square = false;

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return result;
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

      return result;
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

    return result;
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
               ACE_TEXT("RPG_Engine_Common_Tools::loadLevel(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               exception.what()));

    return result;
  }
  catch (::xml_schema::parsing const& exception)
  {
    std::ostringstream converter;
    converter << exception;
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::loadLevel(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               converter.str().c_str()));

    return result;
  }
  catch (::xml_schema::exception const& exception)
  {
    ACE_UNUSED_ARG(exception);

    // *NOTE*: maybe this was a MAP file (expected LEVEL file)
    // --> try parsing that instead...
    RPG_Map_ParserDriver parser_driver(false, false);
    parser_driver.init(&result.map.start,
                       &result.map.seeds,
                       &result.map.plan,
                       false, false);
    if (!parser_driver.parse(filename_in, false))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Map_ParserDriver::parse(\"%s\"), aborting\n"),
                 filename_in.c_str()));

      return result;
    } // end IF

    // OK ! --> proceed
    is_level = false;
    ifs.close();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::loadLevel(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));

    return result;
  }
  if (is_level)
  {
    ACE_ASSERT(engine_level_p.get());
    RPG_Engine_Level_XMLTree_Type* level_p = engine_level_p.get();
    ACE_ASSERT(level_p);

    result = RPG_Engine_Level::levelXMLToLevel(*level_p);
  } // end IF
  result.map.plan.rooms_are_square = RPG_Map_Common_Tools::roomsAreSquare(result.map);

  return result;
}

void
RPG_Engine_Level::print(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::print"));

  RPG_Map_Level::print(level_in.map);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("roaming monsters:\n")));
  for (RPG_Monster_ListConstIterator_t iterator = level_in.level_meta.roaming_monsters.begin();
       iterator != level_in.level_meta.roaming_monsters.end();
       iterator++)
     ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("%s\n"),
                (*iterator).c_str()));
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("spawn interval: %T#\nspawn probability (%%): %2f\nmax # spawned: %u\nspawn timer ID: %d\n"),
             &level_in.level_meta.spawn_interval,
             level_in.level_meta.spawn_probability,
             level_in.level_meta.max_spawned,
             level_in.level_meta.spawn_timer));
}

void
RPG_Engine_Level::init(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::init"));

  myLevelMeta = level_in.level_meta;
  inherited::init(level_in.map);
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
             myLevelMeta.name.c_str(),
             filename_in.c_str()));
}

const RPG_Engine_LevelMeta_t&
RPG_Engine_Level::getMeta() const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::getMeta"));

  return myLevelMeta;
}

void
RPG_Engine_Level::handleDoor(const RPG_Map_Position_t& position_in,
                             const bool& open_in,
                             bool& toggled_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleDoor"));

  // init return value(s)
  toggled_out = false;

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  position_door.outside = DIRECTION_INVALID;
  position_door.is_open = false;
  position_door.is_locked = false;
  position_door.is_broken = false;

  RPG_Map_DoorsIterator_t iterator = myMap.plan.doors.find(position_door);
  // sanity check
  ACE_ASSERT(iterator != myMap.plan.doors.end());

  if (open_in)
  {
    // sanity check
    if ((*iterator).is_open)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already open...\n"),
//                  position_in.first,
//                  position_in.second));

      return;
    } // end IF

    // cannot simply open locked doors...
    if ((*iterator).is_locked)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("door [%u,%u] is locked...\n"),
                 position_in.first,
                 position_in.second));

      return;
    } // end IF
  } // end IF
  else
  {
    // sanity check
    if (!(*iterator).is_open)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already closed...\n"),
//                  position_in.first,
//                  position_in.second));

      return;
    } // end IF
  } // end ELSE

  // *WARNING*: set iterators are CONST for a good reason !
  // --> (but we know what we're doing)...
  (const_cast<RPG_Map_Door_t&>(*iterator)).is_open = open_in;

  toggled_out = true;
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
  if (myLevelMeta.environment.climate != RPG_COMMON_CLIMATE_INVALID)
    environment.climate(RPG_Common_ClimateHelper::RPG_Common_ClimateToString(myLevelMeta.environment.climate));
  if (myLevelMeta.environment.lighting != RPG_COMMON_AMBIENTLIGHTING_INVALID)
    environment.lighting(RPG_Common_AmbientLightingHelper::RPG_Common_AmbientLightingToString(myLevelMeta.environment.lighting));
  environment.outdoors(myLevelMeta.environment.outdoors);
  if (myLevelMeta.environment.plane != RPG_COMMON_PLANE_INVALID)
    environment.plane(RPG_Common_PlaneHelper::RPG_Common_PlaneToString(myLevelMeta.environment.plane));
  if (myLevelMeta.environment.terrain != RPG_COMMON_TERRAIN_INVALID)
    environment.terrain(RPG_Common_TerrainHelper::RPG_Common_TerrainToString(myLevelMeta.environment.terrain));
  ACE_ASSERT(myLevelMeta.environment.time == RPG_COMMON_TIMEOFDAY_INVALID);

  RPG_Common_FixedPeriod_XMLTree_Type spawn_interval(static_cast<unsigned int>(myLevelMeta.spawn_interval.sec()),
                                                     static_cast<unsigned int>(myLevelMeta.spawn_interval.usec()));

  std::string map_string = RPG_Map_Common_Tools::map2String(inherited::myMap);

  RPG_Engine_Level_XMLTree_Type* level_p = NULL;
  level_p = new(std::nothrow) RPG_Engine_Level_XMLTree_Type(myLevelMeta.name,
                                                            environment,
                                                            spawn_interval,
                                                            myLevelMeta.spawn_probability,
                                                            myLevelMeta.max_spawned,
                                                            map_string);
  ACE_ASSERT(level_p);
  if (!level_p)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return NULL;
  }

  // *NOTE*: add monster sequence "manually"
  for (RPG_Monster_ListConstIterator_t iterator = myLevelMeta.roaming_monsters.begin();
       iterator != myLevelMeta.roaming_monsters.end();
       iterator++)
    level_p->monster().push_back(*iterator);

  return level_p;
}

RPG_Engine_Level_t
RPG_Engine_Level::levelXMLToLevel(const RPG_Engine_Level_XMLTree_Type& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::levelXMLToLevel"));

  RPG_Engine_Level_t result;
  result.level_meta.name = level_in.name();
  result.level_meta.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  if (level_in.environment().climate().present())
    result.level_meta.environment.climate = RPG_Common_ClimateHelper::stringToRPG_Common_Climate(level_in.environment().climate().get());
  result.level_meta.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  if (level_in.environment().lighting().present())
    result.level_meta.environment.lighting = RPG_Common_AmbientLightingHelper::stringToRPG_Common_AmbientLighting(level_in.environment().lighting().get());
  result.level_meta.environment.outdoors = level_in.environment().outdoors();
  result.level_meta.environment.plane = RPG_COMMON_PLANE_INVALID;
  if (level_in.environment().plane().present())
    result.level_meta.environment.plane = RPG_Common_PlaneHelper::stringToRPG_Common_Plane(level_in.environment().plane().get());
  result.level_meta.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  if (level_in.environment().terrain().present())
    result.level_meta.environment.terrain = RPG_Common_TerrainHelper::stringToRPG_Common_Terrain(level_in.environment().terrain().get());
  result.level_meta.environment.time = RPG_COMMON_TIMEOFDAY_INVALID;
  ACE_ASSERT(!level_in.environment().time().present());
  for (RPG_Engine_Level_XMLTree_Type::monster_const_iterator iterator = level_in.monster().begin();
       iterator != level_in.monster().end();
       iterator++)
    result.level_meta.roaming_monsters.push_back(*iterator);
  //result.level_meta.roaming_monsters.insert(level_in.monster().begin(), level_in.monster().end());
  unsigned int u_seconds = 0;
  if (level_in.spawn_interval().u_seconds().present())
    u_seconds = level_in.spawn_interval().u_seconds().get();
  result.level_meta.spawn_interval.set(level_in.spawn_interval().seconds(),
                                       u_seconds);
  result.level_meta.spawn_probability = level_in.spawn_probability();
  result.level_meta.max_spawned = level_in.max_spawned();
  result.level_meta.spawn_timer = -1;
  RPG_Map_ParserDriver parser_driver(false, false);
  parser_driver.init(&result.map.start,
                     &result.map.seeds,
                     &result.map.plan,
                     false, false);
  if (!parser_driver.parse(level_in.map(), true))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to parse map, continuing\n")));

  return result;
}
