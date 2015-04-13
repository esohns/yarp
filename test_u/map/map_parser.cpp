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
#include <string>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"

#include "common_file_tools.h"
#include "common_tools.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_level.h"

#include "rpg_engine_defines.h"

#define MAP_PARSER_DEF_DEBUG_SCANNER false
#define MAP_PARSER_DEF_DEBUG_PARSER  false

#define MAP_GENERATOR_DEF_LEVEL      false

void
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR ("engine")
                                   : ACE_TEXT_ALWAYS_CHAR ("map"));
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("data");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += (MAP_GENERATOR_DEF_LEVEL ? RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME))
                                   : ACE_TEXT_ALWAYS_CHAR (RPG_MAP_DEF_MAP_FILE));
  path += (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT)
                                   : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_FILE_EXT));
  std::cout << ACE_TEXT ("-m [FILE] : map (*")
            << ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT)
            << ACE_TEXT (",*")
            << ACE_TEXT_ALWAYS_CHAR (RPG_MAP_FILE_EXT)
            << ACE_TEXT (") [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-p        : debug parser")
            << ACE_TEXT (" [")
            << MAP_PARSER_DEF_DEBUG_PARSER
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT("-s        : debug scanner")
            << ACE_TEXT (" [")
            << MAP_PARSER_DEF_DEBUG_SCANNER
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT ("-v        : print version information and exit") << std::endl;
}

bool
do_processArguments (const int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& debugScanner_out,
                     bool& debugParser_out,
                     std::string& mapFile_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  // init results
  debugScanner_out = MAP_PARSER_DEF_DEBUG_SCANNER;
  debugParser_out = MAP_PARSER_DEF_DEBUG_PARSER;

  mapFile_out = data_path;
  mapFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  mapFile_out += (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR ("engine")
                                          : ACE_TEXT_ALWAYS_CHAR ("map"));
  mapFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  mapFile_out += ACE_TEXT_ALWAYS_CHAR("data");
  mapFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  mapFile_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  mapFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  mapFile_out += (MAP_GENERATOR_DEF_LEVEL ? RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME))
                                          : ACE_TEXT_ALWAYS_CHAR (RPG_MAP_DEF_MAP_FILE));
  mapFile_out += (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT)
                                          : ACE_TEXT_ALWAYS_CHAR(RPG_MAP_FILE_EXT));

  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("m:pstv"));

  int option = 0;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'm':
      {
        mapFile_out = argumentParser.opt_arg ();

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
do_work (bool debugScanner_in,
         bool debugParser_in,
         const std::string& mapFile_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: load map
  RPG_Map_t map;
  RPG_Map_Level::load (mapFile_in, map);

  // debug info
  RPG_Map_Level::print (map);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
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
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  RPG_TRACE (ACE_TEXT ("::main"));

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  // step1: init
  // step1a set defaults
  bool debug_scanner          = MAP_PARSER_DEF_DEBUG_SCANNER;
  bool debug_parser           = MAP_PARSER_DEF_DEBUG_PARSER;
  std::string map_file        = data_path;
  map_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  map_file += (MAP_GENERATOR_DEF_LEVEL ? RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR ("engine"))
                                       : ACE_TEXT_ALWAYS_CHAR ("map"));
  map_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  map_file += ACE_TEXT_ALWAYS_CHAR ("data");
  map_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  map_file += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_MAPS_SUB);
  map_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  map_file += (MAP_GENERATOR_DEF_LEVEL ? RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME))
                                       : ACE_TEXT_ALWAYS_CHAR (RPG_MAP_DEF_MAP_FILE));
  map_file += (MAP_GENERATOR_DEF_LEVEL ? ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT)
                                       : ACE_TEXT_ALWAYS_CHAR (RPG_MAP_FILE_EXT));
  bool trace_information      = false;
  bool print_version_and_exit = false;

  // step1ba: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            debug_scanner,
                            debug_parser,
                            map_file,
                            trace_information,
                            print_version_and_exit))
  {
    // make 'em learn...
    do_printUsage (std::string (ACE::basename (argv_in[0])));

    // clean up
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if (!Common_File_Tools::isReadable (map_file))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("invalid argument(s), aborting\n")));

    // make 'em learn...
    do_printUsage (std::string (ACE::basename (argv_in[0])));

    // clean up
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),   // program name
                                        log_file,                    // logfile
                                        false,                       // log to syslog ?
                                        false,                       // trace messages ?
                                        trace_information,           // debug messages ?
                                        NULL))                       // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // clean up
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (std::string (ACE::basename (argv_in[0])));

    // clean up
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (debug_scanner,
           debug_parser,
           map_file);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  RPG_Common_Tools::period2String (working_time,
                                   working_time_string);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;
}
