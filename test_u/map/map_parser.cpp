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

#include <rpg_map_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <string>
#include <sstream>
#include <iostream>

#define MAP_PARSER_DEF_DEBUG_SCANNER false
#define MAP_PARSER_DEF_DEBUG_PARSER  false
#define MAP_PARSER_DEF_MAP           ACE_TEXT_ALWAYS_CHAR("/var/tmp/default_plan.txt")

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-m [FILE] : map (*.txt)") << ACE_TEXT(" [\"") << MAP_PARSER_DEF_MAP << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-p        : debug parser") << ACE_TEXT(" [") << MAP_PARSER_DEF_DEBUG_PARSER << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s        : debug scanner") << ACE_TEXT(" [") << MAP_PARSER_DEF_DEBUG_SCANNER << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  bool& debugScanner_out,
                  bool& debugParser_out,
                  std::string& mapFile_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  debugScanner_out = MAP_PARSER_DEF_DEBUG_SCANNER;
  debugParser_out = MAP_PARSER_DEF_DEBUG_PARSER;
  mapFile_out = MAP_PARSER_DEF_MAP;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("m:pstv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'm':
      {
        mapFile_out = argumentParser.opt_arg();

        break;
      }
      case 'p':
      {
        debugParser_out = true;

        break;
      }
      case 's':
      {
        debugScanner_out = true;

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
do_work(const bool& debugScanner_in,
        const bool& debugParser_in,
        const std::string& mapFile_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: load map
  RPG_Map_t map;
  if (!RPG_Map_Common_Tools::load(mapFile_in,
                                  map,
                                  debugScanner_in,
                                  debugParser_in))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to RPG_Map_Common_Tools::load(\"%s\"), aborting\n"),
               mapFile_in.c_str()));

    return;
  } // end IF

  // debug info
  RPG_Map_Common_Tools::print(map);

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
  bool debugScanner                = MAP_PARSER_DEF_DEBUG_SCANNER;
  bool debugParser                 = MAP_PARSER_DEF_DEBUG_PARSER;
  std::string mapFile              = MAP_PARSER_DEF_MAP;
  bool traceInformation            = false;
  bool printVersionAndExit         = false;

  // step1ba: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          debugScanner,
                          debugParser,
                          mapFile,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if (!RPG_Common_File_Tools::isReadable(mapFile))
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
  do_work(debugScanner,
          debugParser,
          mapFile);

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
