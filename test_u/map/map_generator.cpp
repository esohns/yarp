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

// *NOTE*: workaround quirky MSVC...
#ifdef _MSC_VER
#define NOMINMAX
#endif

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "map_generator_defines.h"

#include "rpg_graphics_common_tools.h"

#include "rpg_engine_defines.h"
#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"

#include "rpg_map_defines.h"
#include "rpg_map_common_tools.h"

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "ace/ACE.h"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"

#include <string>
#include <set>
#include <sstream>
#include <iostream>

typedef std::set<char> Map_Generator_Options_t;
typedef Map_Generator_Options_t::const_iterator Map_Generator_OptionsIterator_t;

void
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-a<[VALUE]> : enforce (minimum) room-size")
            << ACE_TEXT(" [")
            << (MAP_GENERATOR_DEF_MIN_ROOMSIZE != 0)
            << ACE_TEXT("] (0:off)")
            << std::endl;
  std::cout << ACE_TEXT("-c          : generate corridor(s)")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_CORRIDORS
            << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-d<[VALUE]> : enforce maximum #doors/room")
            << ACE_TEXT(" [")
            << (MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM != 0)
            << ACE_TEXT(":")
            << MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM
            << ACE_TEXT("] (0:off)")
            << std::endl;
  std::cout << ACE_TEXT("-l          : generate level")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_LEVEL
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-m          : maximize room-size(s)")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE
            << ACE_TEXT("]")
            << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("map");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("data");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path +=
      (MAP_GENERATOR_DEF_LEVEL ? RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME))
                               : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP_FILE));
  path +=
      (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT)
                               : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_FILE_EXT));
  std::cout << ACE_TEXT("-o [FILE]   : output file")
            << ACE_TEXT(" [")
            << path
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-p          : print (==dump) result")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_DUMP
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-r [VALUE]  : #areas")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_NUM_AREAS
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-s          : square room(s)")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_SQUARE_ROOMS
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]  : #columns")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_DIMENSION_X
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-y [VALUE]  : #rows")
            << ACE_TEXT(" [")
            << MAP_GENERATOR_DEF_DIMENSION_Y
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-z          : random")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
}

bool
do_processArguments(const int argc_in,
                    ACE_TCHAR* argv_in[], // cannot be const...
                    unsigned int& minRoomSize_out,
                    bool& corridors_out,
                    unsigned int& maxNumDoorsPerRoom_out,
                    bool& level_out,
                    bool& maximizeRoomSize_out,
                    std::string& outputFile_out,
                    bool& dump_out,
                    unsigned int& numAreas_out,
                    bool& squareRooms_out,
                    bool& traceInformation_out,
                    bool& printVersionAndExit_out,
                    unsigned int& dimensionX_out,
                    unsigned int& dimensionY_out,
                    bool& random_out,
                    Map_Generator_Options_t& options_out)
{
  RPG_TRACE(ACE_TEXT("::do_processArguments"));

  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  // init results
  minRoomSize_out         = MAP_GENERATOR_DEF_MIN_ROOMSIZE;
  corridors_out           = MAP_GENERATOR_DEF_CORRIDORS;
  level_out               = MAP_GENERATOR_DEF_LEVEL;
  maxNumDoorsPerRoom_out  = MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM;
  maximizeRoomSize_out    = MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE;

  outputFile_out          = data_path;
  outputFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  outputFile_out += ACE_TEXT_ALWAYS_CHAR("map");
  outputFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  outputFile_out += ACE_TEXT_ALWAYS_CHAR("data");
  outputFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  outputFile_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  outputFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  std::string default_output_path = outputFile_out;

  dump_out                = MAP_GENERATOR_DEF_DUMP;

  numAreas_out            = MAP_GENERATOR_DEF_NUM_AREAS;
  squareRooms_out         = MAP_GENERATOR_DEF_SQUARE_ROOMS;
  traceInformation_out    = false;
  printVersionAndExit_out = false;
  dimensionX_out          = MAP_GENERATOR_DEF_DIMENSION_X;
  dimensionY_out          = MAP_GENERATOR_DEF_DIMENSION_Y;

  random_out              = false;
  options_out.clear();

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("a::cd::lmo:pr:stvx:y:z"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    options_out.insert(static_cast<char>(option));
    switch (option)
    {
      case 'a':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> minRoomSize_out;

        break;
      }
      case 'c':
      {
        corridors_out = true;

        break;
      }
      case 'd':
      {
        int temp = 0;
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> temp;
        if (temp == -1)
          maxNumDoorsPerRoom_out = std::numeric_limits<unsigned int>::max();

        break;
      }
      case 'l':
      {
        level_out = true;

        break;
      }
      case 'm':
      {
        maximizeRoomSize_out = true;

        break;
      }
      case 'o':
      {
        outputFile_out = argumentParser.opt_arg();

        break;
      }
      case 'p':
      {
        dump_out = true;

        break;
      }
      case 'r':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> numAreas_out;

        break;
      }
      case 's':
      {
        squareRooms_out = true;

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> dimensionX_out;

        break;
      }
      case 'y':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> dimensionY_out;

        break;
      }
      case 'z':
      {
        random_out = true;

        break;
      }
      // error handling
      case ':':
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.last_option())));

        return false;
      }
      case 0: // long option
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%c\", aborting\n"),
                   option));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  if (default_output_path == outputFile_out)
	{
		outputFile_out +=
				(level_out ? RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME))
									 : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP_FILE));
		outputFile_out +=
				(level_out ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT)
									 : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_FILE_EXT));
	} // end IF

  return true;
}

void
do_work(const RPG_Map_FloorPlan_Configuration_t& mapConfig_in,
        const bool& generateLevel_in,
        const std::string& outputFile_in,
        const bool& dump_in,
        const bool& random_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();
  RPG_Graphics_Common_Tools::preInit();

  // step2: generate level data
  RPG_Engine_Level_t level;
  if (generateLevel_in)
  {
    if (random_in)
    {
      level.metadata.environment.plane    = RPG_COMMON_PLANE_INVALID;
      level.metadata.environment.terrain  = RPG_COMMON_TERRAIN_INVALID;
      level.metadata.environment.climate  = RPG_COMMON_CLIMATE_INVALID;
      level.metadata.environment.time     = RPG_COMMON_TIMEOFDAY_INVALID;
      level.metadata.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
      level.metadata.environment.outdoors = false;

      level.metadata.max_num_spawned      = 0;
      level.metadata.name.clear();
      level.metadata.spawns.clear();

      RPG_Engine_Level::random(level.metadata,
                               mapConfig_in,
                               level);
    } // end IF
    else
    {
      level.metadata.name                 =
          ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME);

      level.metadata.environment.plane    =
          RPG_ENGINE_ENVIRONMENT_DEF_PLANE;
      level.metadata.environment.terrain  =
          RPG_ENGINE_ENVIRONMENT_DEF_TERRAIN;
      level.metadata.environment.climate  =
          RPG_ENGINE_ENVIRONMENT_DEF_CLIMATE;
      level.metadata.environment.time     =
          RPG_ENGINE_ENVIRONMENT_DEF_TIMEOFDAY;
      level.metadata.environment.lighting =
          RPG_ENGINE_ENVIRONMENT_DEF_LIGHTING;
      level.metadata.environment.outdoors =
          RPG_ENGINE_ENVIRONMENT_DEF_OUTDOORS;

      RPG_Engine_Spawn_t spawn;
      spawn.spawn.amble_probability = RPG_ENGINE_ENCOUNTER_DEF_AMBLE_PROBABILITY;
      spawn.spawn.interval.seconds =
          RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL;
      spawn.spawn.interval.u_seconds = 0;
      spawn.spawn.max_num_spawned = RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX;
      spawn.spawn.probability = RPG_ENGINE_ENCOUNTER_DEF_PROBABILITY;
      spawn.spawn.type =
          ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENCOUNTER_DEF_TYPE);
      spawn.timer_id = -1;
      level.metadata.spawns.push_back(spawn);
      level.metadata.max_num_spawned = RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX;

      RPG_Engine_Level::create(mapConfig_in,
                               level);
    } // end ELSE
  } // end IF
  else
  {
    if (random_in)
      RPG_Map_Level::random(mapConfig_in,
                            level.map);
    else
      RPG_Map_Level::create(mapConfig_in,
                            level.map);
  } // end ELSE

  // step3: write output file (if any)
  RPG_Engine_Level engine_level;
  engine_level.init(level);
  RPG_Map_Level map_level(level.map);
  if (!outputFile_in.empty())
  {
    if (generateLevel_in)
      engine_level.save(outputFile_in);
    else
      map_level.save(outputFile_in);
  } // end IF

  // step4: display the result
  if (dump_in)
  {
    if (generateLevel_in)
      engine_level.print(level);
    else
      map_level.print(level.map);
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            << RPG_VERSION
#endif
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... this is needed, as the library soname is compared to this
  // string
  std::ostringstream version_number;
  if (version_number << ACE::major_version())
  {
    version_number << ACE_TEXT(".");
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", aborting\n")));

    return;
  } // end ELSE
  if (version_number << ACE::minor_version())
  {
    version_number << ACE_TEXT(".");
    if (version_number << ACE::beta_version())
    {

    } // end IF
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert: \"%m\", aborting\n")));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", aborting\n")));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
{
  RPG_TRACE(ACE_TEXT("::main"));

	// *PORTABILITY*: on Windows, need to init ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
	if (ACE::init() == -1)
	{
		ACE_DEBUG((LM_ERROR,
			         ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
	} // end IF
#endif

  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  // step1: init
  // step1a set defaults
  RPG_Map_FloorPlan_Configuration_t configuration;
  configuration.corridors              = MAP_GENERATOR_DEF_CORRIDORS;
  configuration.doors                  = MAP_GENERATOR_DEF_DOORS;
  configuration.map_size_x             = MAP_GENERATOR_DEF_DIMENSION_X;
  configuration.map_size_y             = MAP_GENERATOR_DEF_DIMENSION_Y;
  configuration.max_num_doors_per_room = MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM;
  configuration.maximize_rooms         = MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE;
  configuration.min_room_size          = MAP_GENERATOR_DEF_MIN_ROOMSIZE;
  configuration.num_areas              = MAP_GENERATOR_DEF_NUM_AREAS;
  configuration.square_rooms           = MAP_GENERATOR_DEF_SQUARE_ROOMS;

  bool generate_level             = MAP_GENERATOR_DEF_LEVEL;

  std::string output_file         = data_path;
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  output_file += ACE_TEXT_ALWAYS_CHAR("map");
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  output_file += ACE_TEXT_ALWAYS_CHAR("data");
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  output_file += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  output_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  std::string default_output_file = output_file;
  default_output_file +=
      (MAP_GENERATOR_DEF_LEVEL ? RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME))
                               : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP_FILE));
  default_output_file +=
      (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT)
                               : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_FILE_EXT));
  output_file = default_output_file;

  bool dump_result                = MAP_GENERATOR_DEF_DUMP;
  bool trace_information          = false;
  bool print_version_and_exit     = false;
  bool random                     = false;

  // step1ba: parse/process/validate configuration
  Map_Generator_Options_t set_options;
  if (!do_processArguments(argc_in,
                           argv_in,
                           configuration.min_room_size,
                           configuration.corridors,
                           configuration.max_num_doors_per_room,
                           generate_level,
                           configuration.maximize_rooms,
                           output_file,
                           dump_result,
                           configuration.num_areas,
                           configuration.square_rooms,
                           trace_information,
                           print_version_and_exit,
                           configuration.map_size_x,
                           configuration.map_size_y,
                           random,
                           set_options))
  {
    // make 'em learn...
    do_printUsage(std::string(ACE::basename(argv_in[0])));

		// *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		if (ACE::fini() == -1)
			ACE_DEBUG((LM_ERROR,
			           ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (random)
  {
    Map_Generator_OptionsIterator_t iterator = set_options.find('a');
    if (iterator == set_options.end())
      configuration.min_room_size = 0;
//    iterator = set_options.find('c');
//    if (iterator == set_options.end())
//      configuration.corridors = false;
    iterator = set_options.find('d');
    if (iterator == set_options.end())
      configuration.max_num_doors_per_room = 0;
//    iterator = set_options.find('m');
//    if (iterator == set_options.end())
//      configuration.maximize_rooms = false;
    iterator = set_options.find('r');
    if (iterator == set_options.end())
      configuration.num_areas = 0;
//    iterator = set_options.find('s');
//    if (iterator == set_options.end())
//      configuration.square_rooms = false;
    iterator = set_options.find('x');
    if (iterator == set_options.end())
      configuration.map_size_x = 0;
    iterator = set_options.find('y');
    if (iterator == set_options.end())
      configuration.map_size_y = 0;
  } // end IF

  // step1bb: validate arguments
  if ((!random &&
       ((configuration.num_areas == 0)  ||
        (configuration.map_size_x == 0) ||
        (configuration.map_size_y == 0)))         ||
      (configuration.max_num_doors_per_room == 1) || // cannot enforce this (yet: *TODO*)
      (configuration.corridors &&
       !configuration.doors))                        // cannot have corridors without doors...
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid argument(s), aborting\n")));

    // make 'em learn...
    do_printUsage(std::string(ACE::basename(argv_in[0])));

		// *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		if (ACE::fini() == -1)
			ACE_DEBUG((LM_ERROR,
			           ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!RPG_Common_Tools::initLogging(ACE::basename(argv_in[0]),   // program name
                                     log_file,                    // logfile
                                     false,                       // log to syslog ?
                                     false,                       // trace messages ?
                                     trace_information,           // debug messages ?
                                     NULL))                       // logger
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(configuration,
          generate_level,
          output_file,
          dump_result,
          random);

  timer.stop();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             ACE_TEXT(working_time_string.c_str())));

  // *PORTABILITY*: on Windows, fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
