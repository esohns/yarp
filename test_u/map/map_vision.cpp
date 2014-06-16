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

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_engine_defines.h"
#include "rpg_engine_common.h"
#include "rpg_engine_level.h"

#include "rpg_map_common.h"
#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include <ace/ACE.h>
#include <ace/High_Res_Timer.h>
#include <ace/Get_Opt.h>
#include <ace/Log_Msg.h>

#include <sstream>
#include <iostream>

#define MAP_VISION_DEF_DEBUG_PARSER false
#define MAP_VISION_DEF_FLOOR_PLAN   "test_plan"

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
  std::cout << ACE_TEXT("-d        : debug parser")
		        << ACE_TEXT(" [")
						<< MAP_VISION_DEF_DEBUG_PARSER
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
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  std::cout << ACE_TEXT("-p [FILE] : level plan (*")
		        << ACE_TEXT(RPG_ENGINE_LEVEL_FILE_EXT)
						<< ACE_TEXT(") [\"")
						<< path
						<< ACE_TEXT("\"]")
						<< std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit")
		        << std::endl;
}

bool
do_processArguments(const int argc_in,
                    ACE_TCHAR* argv_in[], // cannot be const...
										bool& debugParser_out,
										std::string& floorPlan_out,
										bool& traceInformation_out,
										bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::do_processArguments"));

  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  // init results
  debugParser_out         = MAP_VISION_DEF_DEBUG_PARSER;

  floorPlan_out = data_path;
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR("map");
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR("data");
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  floorPlan_out +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("dp:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        debugParser_out = true;

        break;
      }
      case 'p':
      {
        floorPlan_out = argumentParser.opt_arg();

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
      // error handling
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.last_option())));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%c\", aborting\n"),
                   option));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_work(const bool& debugParser_in,
        const std::string& filename_in,
        const std::string& schemaRepository_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();

  // step1: load floor plan
  RPG_Engine_Level_t level;
  if (!RPG_Engine_Level::load(filename_in,
                              schemaRepository_in,
                              level))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to load level plan (was: \"%s\"), aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded level plan %s\n"),
             ACE_TEXT(filename_in.c_str()),
             ACE_TEXT(RPG_Map_Level::info(level.map).c_str())));

  // step2: find source / target positions (just use any two seed positions...)
  if (level.map.seeds.size() < 2)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("cannot proceed (need >= 2 seed positions, had: %d), aborting\n"),
               level.map.seeds.size()));

    return;
  } // end IF
  RPG_Map_Position_t source, target;
  RPG_Map_PositionsConstIterator_t iterator;
  RPG_Dice_RollResult_t result;
  do
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(level.map.seeds.size(),
                                    2,
                                    result);
  } while (result.front() == result.back());
  iterator = level.map.seeds.begin();
  std::advance(iterator, result.front() - 1);
  source = *iterator;
  iterator = level.map.seeds.begin();
  std::advance(iterator, result.back() - 1);
  target = *iterator;

  // step3: compute shortest route (== line-of-sight)
  RPG_Map_PositionList_t line_of_sight;
  RPG_Map_Pathfinding_Tools::findPath(source,
                                      target,
                                      line_of_sight);
  if (line_of_sight.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Map_Pathfinding_Tools::findPath([%u,%u] --> [%u,%u]), aborting\n"),
               source.first, source.second,
               target.first, target.second));

    return;
  } // end IF

  // step4: display the result
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("line of sight: [%u,%u] --> [%u,%u]\n"),
             source.first, source.second,
             target.first, target.second));

  RPG_Map_Position_t current_position;
  RPG_Map_Door_t current_position_door;
  std::ostringstream converter;
  bool done = false;
  RPG_Map_PositionListConstIterator_t line_of_sight_iterator;
  for (unsigned int y = 0;
       y < level.map.plan.size_y;
       y++)
  {
    for (unsigned int x = 0;
         x < level.map.plan.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      current_position_door.position = current_position;

      // unmapped, floor, wall, or door ?
      if (level.map.plan.unmapped.find(current_position) !=
				  level.map.plan.unmapped.end())
        converter << ACE_TEXT(" "); // unmapped
      else if (level.map.plan.walls.find(current_position) !=
				       level.map.plan.walls.end())
        converter << ACE_TEXT("#"); // wall
      else if (level.map.plan.doors.find(current_position_door) !=
				       level.map.plan.doors.end())
        converter << ACE_TEXT("="); // door
      else
      {
        // floor or line-of-sight ?
        if (current_position == line_of_sight.front())
        {
           converter << ACE_TEXT("A"); // source
           continue;
        } // end IF
        else if (current_position == line_of_sight.back())
        {
           converter << ACE_TEXT("B"); // target
           continue;
        } // end IF
        done = false;
        for (line_of_sight_iterator = line_of_sight.begin();
             line_of_sight_iterator != line_of_sight.end();
             line_of_sight_iterator++)
          if ((*line_of_sight_iterator) == current_position)
          {
            converter << ACE_TEXT("x"); // ray
            done = true;

            break;
          } // end IF

        if (!done)
          converter << ACE_TEXT("."); // floor
      } // end ELSE
    } // end FOR
    converter << std::endl;
  } // end FOR

  // --> dump to stdout...
  std::cout << converter.str();

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
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
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
          ACE_TCHAR** argv_in)
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

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  // step1: init
  // step1a set defaults
  bool debug_parser             = MAP_VISION_DEF_DEBUG_PARSER;

  std::string floor_plan        = data_path;
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  floor_plan += ACE_TEXT_ALWAYS_CHAR("map");
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  floor_plan += ACE_TEXT_ALWAYS_CHAR("data");
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  floor_plan += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  floor_plan +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  floor_plan += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  std::string schema_repository = configuration_path;
#if defined(DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif

  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schema_repository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               ACE_TEXT(schema_repository.c_str())));

    // try fallback
    schema_repository.clear();
  } // end IF
  
  bool trace_information        = false;
  bool print_version_and_exit   = false;

  // step1ba: parse/process/validate configuration
  if (!do_processArguments(argc_in,
		                       argv_in,
                           debug_parser,
                           floor_plan,
                           trace_information,
                           print_version_and_exit))
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

  // step1bb: validate arguments
  if (!RPG_Common_File_Tools::isReadable(floor_plan))
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
  if (!RPG_Common_Tools::initLogging(ACE::basename(argv_in[0]), // program name
                                     log_file,                  // logfile
                                     false,                     // log to syslog ?
                                     false,                     // trace messages ?
                                     trace_information,         // debug messages ?
                                     NULL))                     // logger
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
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
  do_work(debug_parser,
          floor_plan,
          schema_repository);
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

  // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
	if (ACE::fini() == -1)
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;
} // end main
