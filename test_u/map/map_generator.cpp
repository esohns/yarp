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
#define NOMINMAX

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <rpg_config.h>
#endif

#include <rpg_engine_defines.h>
#include <rpg_engine_common.h>
#include <rpg_engine_common_tools.h>

#include <rpg_map_defines.h>
#include <rpg_map_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <string>
#include <sstream>
#include <iostream>

#define MAP_GENERATOR_DEF_MIN_ROOMSIZE          0
#define MAP_GENERATOR_DEF_DOORS                 true
#define MAP_GENERATOR_DEF_CORRIDORS             true
#define MAP_GENERATOR_DEF_LEVEL                 false
#define MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM 3
#define MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE     true
#define MAP_GENERATOR_DEF_NUM_AREAS             5
#define MAP_GENERATOR_DEF_SQUARE_ROOMS          true
#define MAP_GENERATOR_DEF_DIMENSION_X           80
#define MAP_GENERATOR_DEF_DIMENSION_Y           40

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-a<[VALUE]> : enforce (minimum) room-size") << ACE_TEXT(" [") << (MAP_GENERATOR_DEF_MIN_ROOMSIZE != 0) << ACE_TEXT(":") << MAP_GENERATOR_DEF_MIN_ROOMSIZE << ACE_TEXT("; 0:off]") << std::endl;
  std::cout << ACE_TEXT("-c          : corridor(s)") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_CORRIDORS << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-d<[VALUE]> : enforce maximum #doors/room") << ACE_TEXT(" [") << (MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM != 0) << ACE_TEXT(":") << MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM << ACE_TEXT("; 0:off]") << std::endl;
  std::cout << ACE_TEXT("-l          : generate level") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_LEVEL << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-m          : maximize room-size(s)") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE << ACE_TEXT("]") << std::endl;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  std::string path = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY);
#else
  std::string path = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY));
#endif
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP);
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_EXT);
  std::cout << ACE_TEXT("-o ([FILE]) : output file") << ACE_TEXT(" [") << path.c_str() << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-r [VALUE]  : #areas") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_NUM_AREAS << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s          : square room(s)") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_SQUARE_ROOMS << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]  : #columns") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_DIMENSION_X << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-y [VALUE]  : #rows") << ACE_TEXT(" [") << MAP_GENERATOR_DEF_DIMENSION_Y << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  unsigned int& minRoomSize_out,
                  bool& corridors_out,
                  unsigned int& maxNumDoorsPerRoom_out,
                  bool& level_out,
                  bool& maximizeRoomSize_out,
                  std::string& outputFile_out,
                  unsigned int& numAreas_out,
                  bool& squareRooms_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  unsigned int& dimensionX_out,
                  unsigned int& dimensionY_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  minRoomSize_out = MAP_GENERATOR_DEF_MIN_ROOMSIZE;
  corridors_out = MAP_GENERATOR_DEF_CORRIDORS;
  level_out = MAP_GENERATOR_DEF_LEVEL;
  maxNumDoorsPerRoom_out = MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM;
  maximizeRoomSize_out = MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE;

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  outputFile_out = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY);
#else
  outputFile_out = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY));
#endif
  outputFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  outputFile_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP);
  outputFile_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_EXT);

  numAreas_out = MAP_GENERATOR_DEF_NUM_AREAS;
  squareRooms_out = MAP_GENERATOR_DEF_SQUARE_ROOMS;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  dimensionX_out = MAP_GENERATOR_DEF_DIMENSION_X;
  dimensionY_out = MAP_GENERATOR_DEF_DIMENSION_Y;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("a::cd::lmo:r:stvx:y:"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
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
          maxNumDoorsPerRoom_out = std::numeric_limits<unsigned long>::max();

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
      // error handling
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   argumentParser.last_option()));

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
do_work(const std::string& name_in,
        const RPG_Map_FloorPlan_Config_t& mapConfig_in,
        const bool& generateLevel_in,
        const std::string& outputFile_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();

  // step2: generate random map
  RPG_Map_t map;
  RPG_Map_Common_Tools::create(name_in,
                               mapConfig_in,
                               map);

  // step2: generate level data
  RPG_Engine_Level_t level;
  if (generateLevel_in)
  {
    level.level_meta.monsters.push_back(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_DEF_AI_SPAWN_TYPE));
    level.level_meta.probability = RPG_ENGINE_DEF_AI_SPAWN_PROBABILITY;
    level.level_meta.spawn_interval.set(RPG_ENGINE_DEF_AI_SPAWN_TIMER_SEC, 0);
    level.map = map;
  } // end IF

  // step3: write output file (if any)
  if (!outputFile_in.empty())
  {
    if (generateLevel_in)
    {
      if (!RPG_Engine_Common_Tools::saveLevel(level,
                                              outputFile_in))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Engine_Common_Tools::saveLevel(\"%s\"), aborting\n"),
                   outputFile_in.c_str()));

        return;
      } // end IF
    } // end IF
    else if (!RPG_Map_Common_Tools::save(outputFile_in,
                                         map))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), aborting\n"),
                 outputFile_in.c_str()));

      return;
    } // end IF
  } // end IF

  // step4: display the result
  if (generateLevel_in)
    RPG_Engine_Common_Tools::dump(level);
  else
    RPG_Map_Common_Tools::dump(map);

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
ACE_TMAIN(int argc,
          ACE_TCHAR* argv[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step1: init
  // step1a set defaults
  unsigned int minRoomSize        = MAP_GENERATOR_DEF_MIN_ROOMSIZE;
  bool doors                      = MAP_GENERATOR_DEF_DOORS;
  bool corridors                  = MAP_GENERATOR_DEF_CORRIDORS;
  unsigned int maxNumDoorsPerRoom = MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM;
  bool generateLevel              = MAP_GENERATOR_DEF_LEVEL;
  bool maximizeRoomSize           = MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE;

  std::string outputFile;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  outputFile = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY);
#else
  outputFile = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY));
#endif
  outputFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  outputFile += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP);
  std::string default_output_file = outputFile;

  unsigned int numAreas           = MAP_GENERATOR_DEF_NUM_AREAS;
  bool squareRooms                = MAP_GENERATOR_DEF_SQUARE_ROOMS;
  bool traceInformation           = false;
  bool printVersionAndExit        = false;
  unsigned int dimension_X        = MAP_GENERATOR_DEF_DIMENSION_X;
  unsigned int dimension_Y        = MAP_GENERATOR_DEF_DIMENSION_Y;

  // step1ba: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          minRoomSize,
                          corridors,
                          maxNumDoorsPerRoom,
                          generateLevel,
                          maximizeRoomSize,
                          outputFile,
                          numAreas,
                          squareRooms,
                          traceInformation,
                          printVersionAndExit,
                          dimension_X,
                          dimension_Y)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if ((numAreas == 0) ||
      (dimension_X == 0) ||
      (dimension_Y == 0) ||
      (maxNumDoorsPerRoom == 1) || // cannot enforce this (just think about it !)
      (corridors && !doors)) // cannot have corridors without doors...
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid argument(s), aborting\n")));

    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // handle extension
  if (outputFile == default_output_file)
    outputFile += (generateLevel ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_EXT)
                                 : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_EXT));

  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_TRACE
                                    //LM_DEBUG | // <-- DISABLE trace messages !
                                    LM_INFO |
                                    LM_NOTICE |
                                    LM_WARNING |
                                    LM_STARTUP |
                                    LM_ERROR |
                                    LM_CRITICAL |
                                    LM_ALERT |
                                    LM_EMERGENCY);

    // set new mask...
    ACE_LOG_MSG->priority_mask(process_priority_mask,
                               ACE_Log_Msg::PROCESS);

    //ACE_LOG_MSG->stop_tracing();

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF

  // step1d: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv[0])));

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  std::string name = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_NAME);
  RPG_Map_FloorPlan_Config_t config;
  config.corridors = corridors;
  config.doors = doors;
  config.map_size_x = dimension_X;
  config.map_size_y = dimension_Y;
  config.max_num_doors_per_room = maxNumDoorsPerRoom;
  config.maximize_rooms = maximizeRoomSize;
  config.min_room_size = minRoomSize;
  config.num_areas = numAreas;
  config.square_rooms = squareRooms;
  do_work(name,
          config,
          generateLevel,
          outputFile);

  timer.stop();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             working_time_string.c_str()));

  return EXIT_SUCCESS;
} // end main
