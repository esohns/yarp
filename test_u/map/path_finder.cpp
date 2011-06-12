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

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <rpg_config.h>
#endif

#include <rpg_map_common.h>
#include <rpg_map_common_tools.h>
#include <rpg_map_pathfinding_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <ace/ACE.h>
#include <ace/High_Res_Timer.h>
#include <ace/Get_Opt.h>
#include <ace/Log_Msg.h>

#include <sstream>
#include <iostream>

#define PATH_FINDER_DEF_CORRIDORS    false
#define PATH_FINDER_DEF_DEBUG_PARSER false
#define PATH_FINDER_DEF_FLOOR_PLAN   ACE_TEXT_ALWAYS_CHAR("/var/tmp/test_plan.txt")

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-b        : build corridors") << ACE_TEXT(" [") << PATH_FINDER_DEF_CORRIDORS << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-d        : debug parser") << ACE_TEXT(" [") << PATH_FINDER_DEF_DEBUG_PARSER << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-p [FILE] : floor plan (*.txt)") << ACE_TEXT(" [\"") << PATH_FINDER_DEF_FLOOR_PLAN << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  bool& buildCorridors_out,
                  bool& debugParser_out,
                  std::string& floorPlan_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  buildCorridors_out      = PATH_FINDER_DEF_CORRIDORS;
  debugParser_out         = PATH_FINDER_DEF_DEBUG_PARSER;
  floorPlan_out           = PATH_FINDER_DEF_FLOOR_PLAN;
  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("bdp:tv"));

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
do_work(const bool& buildCorridors_in,
        const bool& debugParser_in,
        const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();

  // step1: load floor plan
  RPG_Map_Position_t  startingPosition;
  RPG_Map_Positions_t seedPoints;
  RPG_Map_FloorPlan_t floorPlan;
  RPG_Map_Common_Tools::load(filename_in,
                             startingPosition,
                             seedPoints,
                             floorPlan,
                             false,
                             debugParser_in);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded floor plan (\"%s\": size [%u,%u]...\n"),
             filename_in.c_str(),
             floorPlan.size_x,
             floorPlan.size_y));

  // step2: process doors
  RPG_Map_Positions_t door_positions;
  for (RPG_Map_DoorsConstIterator_t iterator = floorPlan.doors.begin();
       iterator != floorPlan.doors.end();
       iterator++)
  {
    // *WARNING*: set iterators are CONST for a good reason !
    // --> (but we know what we're doing)...
    const_cast<RPG_Map_Door_t&> (*iterator).outside = RPG_Map_Common_Tools::door2exitDirection((*iterator).position,
                                                                                 floorPlan);

    door_positions.insert((*iterator).position);
  } // end FOR

  // step3: find paths between doors
  RPG_Map_PathList_t paths;
  RPG_Map_Path_t current_path;
  RPG_Map_Positions_t used_positions;
  RPG_Dice_RollResult_t result;
  RPG_Map_DoorsConstIterator_t target_door = floorPlan.doors.end();
  for (RPG_Map_DoorsConstIterator_t iterator = floorPlan.doors.begin();
       iterator != floorPlan.doors.end();
       iterator++)
  {
    // find target door:
    // - ignore source door
    // - ignore doors that are already connected
    do
    {
      target_door = floorPlan.doors.begin();
      result.clear();
      RPG_Dice::generateRandomNumbers(floorPlan.doors.size(),
                                      1,
                                      result);
      std::advance(target_door, result.front() - 1);
    } while ((target_door == iterator) ||
             (used_positions.find((*target_door).position) != used_positions.end()));

    if (!RPG_Map_Pathfinding_Tools::findPath(floorPlan.size_x,
                                             floorPlan.size_y,
                                             floorPlan.walls,
                                             (*iterator).position,
                                             (*iterator).outside,
                                             (*target_door).position,
                                             current_path))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("could not find a path [%u,%u] --> [%u,%u], continuing...\n"),
                 (*iterator).position.first,
                 (*iterator).position.second,
                 (*target_door).position.first,
                 (*target_door).position.second));

      continue;
    } // end IF

    // remember connected positions
    used_positions.insert((*iterator).position);
    used_positions.insert((*target_door).position);

    paths.push_back(current_path);

    // minimum connectivity reached ?
    if (used_positions == door_positions)
      break; // done
  } // end FOR

  // step4: build corridors ?
  if (buildCorridors_in)
  {
    for (RPG_Map_PathListConstIterator_t iterator = paths.begin();
         iterator != paths.end();
         iterator++)
    {
      used_positions.clear();
      RPG_Map_Common_Tools::buildCorridor(*iterator,
                                           used_positions);
      if (!used_positions.empty())
        floorPlan.walls.insert(used_positions.begin(), used_positions.end());
    } // end FOR
  } // end IF

  // step5: display the result
  RPG_Map_Position_t current_position;
  RPG_Map_Door_t current_position_door;
  std::ostringstream converter;
  bool done = false;
  for (unsigned long y = 0;
       y < floorPlan.size_y;
       y++)
  {
    for (unsigned long x = 0;
         x < floorPlan.size_x;
         x++)
    {
      current_position = std::make_pair(x, y);
      current_position_door.position = current_position;

      // unmapped, floor, wall, or door ?
      if (floorPlan.unmapped.find(current_position) != floorPlan.unmapped.end())
        converter << ACE_TEXT(" "); // unmapped
      else if (floorPlan.walls.find(current_position) != floorPlan.walls.end())
        converter << ACE_TEXT("#"); // wall
      else if (floorPlan.doors.find(current_position_door) != floorPlan.doors.end())
        converter << ACE_TEXT("="); // door
      else
      {
        // floor or path ?
        done = false;
        for (RPG_Map_PathListConstIterator_t iterator2 = paths.begin();
             iterator2 != paths.end();
             iterator2++)
        {
          for (RPG_Map_PathConstIterator_t iterator3 = (*iterator2).begin();
               iterator3 != (*iterator2).end();
               iterator3++)
            if ((*iterator3).first == current_position)
            {
              converter << ACE_TEXT("x"); // path
              done = true;

              break;
            } // end IF

          if (done)
            break;
        } // end FOR

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
            << ACE_TEXT(" : ")
            << RPG_VERSION
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
  bool buildCorridors      = PATH_FINDER_DEF_CORRIDORS;
  bool debugParser         = PATH_FINDER_DEF_DEBUG_PARSER;
  std::string floorPlan    = PATH_FINDER_DEF_FLOOR_PLAN;
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1ba: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          buildCorridors,
                          debugParser,
                          floorPlan,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if (!RPG_Common_File_Tools::isReadable(floorPlan))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid argument(s), aborting\n")));

    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_INFO |  // <-- DISABLE trace messages !
                                    //LM_DEBUG |
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
  do_work(buildCorridors,
          debugParser,
          floorPlan);
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
