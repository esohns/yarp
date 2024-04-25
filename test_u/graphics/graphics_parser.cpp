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

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_XML_tools.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"

#include "rpg_engine_defines.h"

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          true);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d       : dump dictionary")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g [FILE]: graphics dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t       : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v       : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x       : do NOT validate XML")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     bool& dumpDictionary_out,
                     std::string& filename_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out,
                     bool& validateXML_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize configuration
  dumpDictionary_out      = false;

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true);
  filename_out            = configuration_path;
  filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  traceInformation_out    = false;
  printVersionAndExit_out = false;
  validateXML_out         = true;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("dg:tvx"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        dumpDictionary_out = true;

        break;
      }
      case 'g':
      {
        filename_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

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
        validateXML_out = false;

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
do_work (const std::string& schemaRepository_in,
         bool dumpDictionary_in,
         const std::string& dictionary_in,
         bool validateXML_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step0: initialize: random seed, string conversion facilities, ...
  RPG_Dice::initialize ();
  RPG_Dice_Common_Tools::initializeStringConversionTables ();
  std::string schema_repository_string = schemaRepository_in;
  schema_repository_string += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository_string += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING);
  schema_repository_string += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository_string += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::vector<std::string> schema_directories_a;
  schema_directories_a.push_back (schema_repository_string);
  if (!RPG_Common_XML_Tools::initialize (schema_directories_a))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_XML_Tools::initialize(), returning\n")));
    return;
  } // end IF
  std::string directory;
  if (!RPG_Graphics_Common_Tools::initialize (directory,
                                              0,
                                              false))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initialize(), returning\n")));
    return;
  } // end IF

  // step1a: initialize graphics dictionary
  if (!RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->initialize (dictionary_in,
                                                                   validateXML_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::initialize(), returning\n")));
    return;
  }

  // step2: dump graphics descriptions
  if (dumpDictionary_in)
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->dump ();

  // step3: clean up
  RPG_Graphics_Common_Tools::finalize ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" : ")
            << yarp_PACKAGE_VERSION
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

  // step0: initialize ACE
  // *PORTABILITY*: on Windows, init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // step1: initialize
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));
  // step1a set defaults
  // initialize configuration
  bool dump_dictionary           = false;

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true);
  std::string filename           = configuration_path;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  std::string schema_repository = Common_File_Tools::getWorkingDirectory ();

  bool trace_information         = false;
  bool print_version_and_exit    = false;
  bool validate_XML              = true;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            dump_dictionary,
                            filename,
                            trace_information,
                            print_version_and_exit,
                            validate_XML))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!Common_File_Tools::isReadable (filename)         ||
      !Common_File_Tools::isDirectory (schema_repository))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
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

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_SUCCESS;
  } // end IF

  // step2: do actual work
  ACE_High_Res_Timer timer;
  timer.start ();
  do_work (schema_repository,
           dump_dictionary,
           filename,
           validate_XML);
  timer.stop ();

  // debug info
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // step4a: fini SDL
  TTF_Quit ();
  SDL_Quit ();

  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
