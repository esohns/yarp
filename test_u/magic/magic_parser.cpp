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

#include "rpg_magic_defines.h"
#include "rpg_magic_common_tools.h"
#include "rpg_magic_dictionary.h"

#include "rpg_character_common_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_dice_common_tools.h"

#include "ace/ACE.h"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

void
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-d       : dump spell dictionary") << std::endl;
  std::cout << ACE_TEXT("-l       : group levels") << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("magic");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-m [FILE]: spell dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT("-v       : print version information and exit")
            << std::endl;
}

bool
do_processArguments(const int& argc_in,
                    ACE_TCHAR** argv_in, // cannot be const...
                    bool& dumpDictionary_out,
                    bool& groupLevels_out,
                    std::string& magicDictionaryFilename_out,
                    bool& traceInformation_out,
                    bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::do_processArguments"));

  // init results
  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  dumpDictionary_out          = false;
  groupLevels_out             = false;

  magicDictionaryFilename_out = configuration_path;
  magicDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  magicDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR("magic");
  magicDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionaryFilename_out +=
      ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);

  traceInformation_out        = false;
  printVersionAndExit_out     = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("dlm:tv"));

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
      case 'l':
      {
        groupLevels_out = true;

        break;
      }
      case 'm':
      {
        magicDictionaryFilename_out = argumentParser.opt_arg();

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
do_work(const bool& dumpDictionary_in,
        const bool& groupLevels_in,
        const std::string& fileName_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init string conversion tables
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();
  RPG_Character_Common_Tools::init();
  RPG_Magic_Common_Tools::init();

  // step2: init spell dictionary
  try
  {
    RPG_MAGIC_DICTIONARY_SINGLETON::instance()->init(fileName_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Magic_Dictionary::init, returning\n")));

    return;
  }

  if (dumpDictionary_in)
  {
    RPG_MAGIC_DICTIONARY_SINGLETON::instance()->dump(groupLevels_in);
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
  // version number... Need this, as the library soname is compared to this
  // string
  std::ostringstream version_number;
  if (version_number << ACE::major_version())
  {
    version_number << ACE_TEXT(".");
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  if (version_number << ACE::minor_version())
  {
    version_number << ".";

    if (version_number << ACE::beta_version())
    {

    } // end IF
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert: \"%m\", returning\n")));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ")
            << ACE_TEXT(version_number.str())
            << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR** argv_in)
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step0: init ACE
  // *PORTABILITY*: on Windows, init ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1: init
  // step1a set defaults
  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  bool dump_dictionary                  = false;
  bool group_levels                     = false;

  std::string magic_dictionary_filename = configuration_path;
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR("magic");
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);

  bool trace_information                = false;
  bool print_version_and_exit           = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments(argc_in,
                           argv_in,
                           dump_dictionary,
                           group_levels,
                           magic_dictionary_filename,
                           trace_information,
                           print_version_and_exit))
  {
    // make 'em learn...
    do_printUsage(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		if (ACE::fini() == -1)
			ACE_DEBUG((LM_ERROR,
								 ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!RPG_Common_File_Tools::isReadable(magic_dictionary_filename))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid (XML) filename \"%s\", aborting\n"),
               ACE_TEXT(magic_dictionary_filename.c_str())));

    // make 'em learn...
    do_printUsage(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		if (ACE::fini() == -1)
			ACE_DEBUG((LM_ERROR,
								 ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
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

    // *PORTABILITY*: on Windows, fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		if (ACE::fini() == -1)
			ACE_DEBUG((LM_ERROR,
								 ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		if (ACE::fini() == -1)
			ACE_DEBUG((LM_ERROR,
								 ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(dump_dictionary,
          group_levels,
          magic_dictionary_filename);

  timer.stop();

//   // debug info
//   std::string working_time_string;
//   ACE_Time_Value working_time;
//   timer.elapsed_time(working_time);
//   RPG_Common_Tools::Period2String(working_time,
//                                   working_time_string);
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
//              ACE_TEXT(working_time_string.c_str())));

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
