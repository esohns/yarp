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

#include <iostream>
#include <sstream>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_map_common.h"
#include "rpg_map_common_tools.h"
#include "rpg_map_level.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_engine_defines.h"

#define PATH_FINDER_DEF_CORRIDORS    false
#define PATH_FINDER_DEF_DEBUG_PARSER false

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          false);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-b       : build corridors")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << PATH_FINDER_DEF_CORRIDORS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d       : debug parser")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << PATH_FINDER_DEF_DEBUG_PARSER
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("map");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR ("data");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAP_MAPS_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path +=
      RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p [FILE]: level plan (*")
            << ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT)
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t       : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR("-v        : print version information and exit")
            << std::endl;
} // end print_usage

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& buildCorridors_out,
                     bool& debugParser_out,
                     std::string& floorPlan_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAP_SUB_DIRECTORY_STRING),
                                                          false);

  // init results
  buildCorridors_out      = PATH_FINDER_DEF_CORRIDORS;
  debugParser_out         = PATH_FINDER_DEF_DEBUG_PARSER;

  floorPlan_out           = data_path;
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  floorPlan_out +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAP_FILE));
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_FILE_EXT);

  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("bdp:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'b':
      {
        buildCorridors_out = true;

        break;
      }
      case 'd':
      {
        debugParser_out = true;

        break;
      }
      case 'p':
      {
        floorPlan_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%c\", aborting\n"),
                    option));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_work (bool buildCorridors_in,
         bool debugParser_in,
         const std::string& filename_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step0: initialize: random seed, string conversion facilities, ...
  RPG_Dice::initialize ();
  RPG_Dice_Common_Tools::initializeStringConversionTables ();
  RPG_Common_Tools::initializeStringConversionTables ();

  // step1: load floor plan
  struct RPG_Map map;
  if (!RPG_Map_Level::load (filename_in,
                            map,
                            false,
                            debugParser_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to load floor plan \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loaded floor plan %s\n"),
              ACE_TEXT (filename_in.c_str ()),
              ACE_TEXT (RPG_Map_Level::info (map).c_str ())));

  // step2: process doors
  RPG_Map_Positions_t door_positions;
  for (RPG_Map_DoorsConstIterator_t iterator = map.plan.doors.begin ();
       iterator != map.plan.doors.end ();
       iterator++)
  {
    // *WARNING*: set iterators are CONST for a good reason !
    // --> (but we know what we're doing)...
    const_cast<struct RPG_Map_Door&> (*iterator).outside =
      RPG_Map_Common_Tools::doorToExitDirection ((*iterator).position,
                                                 map.plan);

    door_positions.insert ((*iterator).position);
  } // end FOR

  // step3: find paths between doors
  RPG_Map_PathList_t paths;
  RPG_Map_Path_t current_path;
  RPG_Map_Positions_t used_positions;
  RPG_Dice_RollResult_t result;
  RPG_Map_DoorsConstIterator_t target_door = map.plan.doors.end ();
  unsigned int index = 1;
  for (RPG_Map_DoorsConstIterator_t iterator = map.plan.doors.begin ();
       iterator != map.plan.doors.end ();
       iterator++)
  {
    // find target door:
    // - ignore source door
    // - ignore doors that are already connected
    do
    {
      target_door = map.plan.doors.begin ();
      result.clear ();
      RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (map.plan.doors.size ()),
                                       1,
                                       result);
      std::advance (target_door, result.front () - 1);
    } while ((target_door == iterator) ||
             (used_positions.find ((*target_door).position) != used_positions.end ()));

    RPG_Map_Pathfinding_Tools::findPath (std::make_pair (map.plan.size_x,
                                                         map.plan.size_y),
                                         map.plan.walls,
                                         (*iterator).position,
                                         (*iterator).outside,
                                         (*target_door).position,
                                         current_path);
    if (current_path.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("could not find a path [%u,%u] --> [%u,%u], continuing\n"),
                  (*iterator).position.first,
                  (*iterator).position.second,
                  (*target_door).position.first,
                  (*target_door).position.second));
      continue;
    } // end IF
    ACE_ASSERT ((current_path.front ().first == (*iterator).position) &&
                (current_path.back ().first  == (*target_door).position));

    // print path
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT("path [%u, %u step(s)]: [%u,%u] --> [%u,%u]: "),
                index++,
                current_path.size(),
                (*iterator).position.first, (*iterator).position.second,
                (*target_door).position.first, (*target_door).position.second));
    for (RPG_Map_PathConstIterator_t path_iterator = current_path.begin ();
         path_iterator != current_path.end ();
         path_iterator++)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("[%u,%u] "),
                  (*path_iterator).first.first,
                  (*path_iterator).first.second));
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\n")));

    // remember connected positions
    used_positions.insert ((*iterator).position);
    used_positions.insert ((*target_door).position);

    paths.push_back (current_path);

    // minimum connectivity reached ?
    if (used_positions == door_positions)
      break; // done
  } // end FOR

  // step4: build corridors ?
  if (buildCorridors_in)
  {
    for (RPG_Map_PathListConstIterator_t iterator = paths.begin ();
         iterator != paths.end ();
         iterator++)
    {
      used_positions.clear ();
      RPG_Map_Common_Tools::buildCorridor (*iterator,
                                           used_positions);
      if (!used_positions.empty ())
        map.plan.walls.insert (used_positions.begin (),
                               used_positions.end ());
    } // end FOR
  } // end IF

  // step5: display the result
  RPG_Map_Position_t current_position;
  struct RPG_Map_Door current_position_door;
  std::ostringstream converter;
  bool done = false;
  for (unsigned int y = 0;
       y < map.plan.size_y;
       y++)
  {
    for (unsigned int x = 0;
         x < map.plan.size_x;
         x++)
    {
      current_position = std::make_pair (x, y);
      current_position_door.position = current_position;

      // unmapped, floor, wall, or door ?
      if (map.plan.unmapped.find (current_position) != map.plan.unmapped.end ())
        converter << ACE_TEXT_ALWAYS_CHAR (" "); // unmapped
      else if (map.plan.walls.find (current_position) != map.plan.walls.end ())
        converter << ACE_TEXT_ALWAYS_CHAR ("#"); // wall
      else if (map.plan.doors.find (current_position_door) != map.plan.doors.end ())
        converter << ACE_TEXT_ALWAYS_CHAR ("="); // door
      else
      {
        // floor or path ?
        done = false;
        for (RPG_Map_PathListConstIterator_t iterator2 = paths.begin ();
             iterator2 != paths.end ();
             iterator2++)
        {
          for (RPG_Map_PathConstIterator_t iterator3 = (*iterator2).begin ();
               iterator3 != (*iterator2).end ();
               iterator3++)
            if ((*iterator3).first == current_position)
            {
              converter << ACE_TEXT_ALWAYS_CHAR ("x"); // path
              done = true;

              break;
            } // end IF

          if (done)
            break;
        } // end FOR

        if (!done)
          converter << ACE_TEXT_ALWAYS_CHAR ("."); // floor
      } // end ELSE
    } // end FOR
    converter << std::endl;
  } // end FOR

  // --> dump to stdout...
  std::cout << converter.str ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" : ")
            << ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_VERSION)
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... Need this, as the library soname is compared to this
  // string
  std::ostringstream version_number;
  if (version_number << ACE::major_version ())
  {
    version_number << ACE_TEXT_ALWAYS_CHAR (".");
  } // end IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert: \"%m\", returning\n")));
    return;
  } // end ELSE
  if (version_number << ACE::minor_version ())
  {
    version_number << ACE_TEXT_ALWAYS_CHAR (".");
    if (version_number << ACE::beta_version ())
    {

    } // end IF
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert: \"%m\", returning\n")));
      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert: \"%m\", returning\n")));
    return;
  } // end ELSE
  std::cout << ACE_TEXT_ALWAYS_CHAR ("ACE: ")
            << version_number.str ()
            << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // step1: initialize ACE
// *PORTABILITY*: on Windows, need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAP_SUB_DIRECTORY_STRING),
                                                          false);

  // step1: init
  // step1a set defaults
  bool build_corridors        = PATH_FINDER_DEF_CORRIDORS;
  bool debug_parser           = PATH_FINDER_DEF_DEBUG_PARSER;

  std::string floor_plan      = data_path;
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  floor_plan +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_MAP_DEF_MAP_FILE));
  floor_plan += ACE_TEXT_ALWAYS_CHAR (RPG_MAP_FILE_EXT);

  bool trace_information      = false;
  bool print_version_and_exit = false;

  // step1ba: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            build_corridors,
                            debug_parser,
                            floor_plan,
                            trace_information,
                            print_version_and_exit))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if (!Common_File_Tools::isReadable (floor_plan))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("invalid argument(s), aborting\n")));

    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])), // program name
                                            log_file,                  // logfile
                                            false,                     // log to syslog ?
                                            false,                     // trace messages ?
                                            trace_information,         // debug messages ?
                                            NULL))                     // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (build_corridors,
           debug_parser,
           floor_plan);
  timer.stop ();

  // debug info
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
