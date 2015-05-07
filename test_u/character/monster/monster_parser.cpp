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
#include <iomanip>
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
#include "rpg_common_XML_tools.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_item_common_tools.h"

#include "rpg_magic_common_tools.h"

#include "rpg_character_common_tools.h"
#include "rpg_character_defines.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_combat_common_tools.h"

#include "rpg_monster_common_tools.h"
#include "rpg_monster_defines.h"
#include "rpg_monster_dictionary.h"

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
#endif

  std::cout << ACE_TEXT ("usage: ")
            << programName_in
            << ACE_TEXT (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT ("currently available options:") << std::endl;
  std::cout << ACE_TEXT ("-d       : dump dictionary") << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("monster");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-m [FILE]: monster dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT ("-v       : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT ("-x       : do NOT validate XML") << std::endl;
}

bool
do_processArguments (const int& argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& dumpDictionary_out,
                     std::string& monsterDictionaryFilename_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out,
                     bool& validateXML_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
#endif

  // init results
  dumpDictionary_out            = false;

  monsterDictionaryFilename_out = configuration_path;
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  monsterDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR("character");
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR("monster");
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monsterDictionaryFilename_out +=
    ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  traceInformation_out          = false;
  printVersionAndExit_out       = false;
  validateXML_out               = true;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("dm:tvx"));

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
      case 'm':
      {
        monsterDictionaryFilename_out = argumentParser.opt_arg();

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
do_work (const std::string& schemaRepository_in,
         const std::string& filename_in,
         bool validateXML_in,
         bool dumpDictionary_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: initialize: random seed, string conversion facilities, ...
  RPG_Dice::initialize ();
  RPG_Dice_Common_Tools::initStringConversionTables ();
  RPG_Common_Tools::initStringConversionTables ();
  if (!RPG_Common_XML_Tools::initialize (schemaRepository_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_XML_Tools::initialize(), returning\n")));
    return;
  } // end IF
  RPG_Item_Common_Tools::initStringConversionTables ();
  RPG_Character_Common_Tools::init ();
  RPG_Magic_Common_Tools::init ();
  RPG_Combat_Common_Tools::initStringConversionTables ();
  RPG_Monster_Common_Tools::initStringConversionTables ();

  // step2: init monster dictionary
  try
  {
    RPG_MONSTER_DICTIONARY_SINGLETON::instance ()->init (filename_in,
                                                         validateXML_in);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Character_Monster_Dictionary::init(), returning\n")));
    return;
  }

  // step3: dump monster descriptions
  if (dumpDictionary_in)
    RPG_MONSTER_DICTIONARY_SINGLETON::instance ()->dump ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

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
  if (version_number << ACE::major_version ())
  {
    version_number << ACE_TEXT (".");
  } // end IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert: \"%m\", returning\n")));
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
  std::cout << ACE_TEXT ("ACE: ") << version_number.str () << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // *PORTABILITY*: on Windows, need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // step1: initialize configuration

  // step1a: set defaults
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif

  bool dump_dictionary = false;

  std::string monster_dictionary_filename = configuration_path;
  monster_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  monster_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("character");
  monster_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monster_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("monster");
  monster_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monster_dictionary_filename +=
    ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  std::string schema_directory = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schema_directory = Common_File_Tools::getWorkingDirectory ();
  schema_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_directory += ACE_TEXT_ALWAYS_CHAR ("engine");
#endif

  bool trace_information = false;
  bool print_version_and_exit = false;
  bool validate_XML = true;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            dump_dictionary,
                            monster_dictionary_filename,
                            trace_information,
                            print_version_and_exit,
                            validate_XML))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!Common_File_Tools::isReadable (monster_dictionary_filename) ||
      !Common_File_Tools::isDirectory (schema_directory))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

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
                ACE_TEXT ("failed to RPG_Common_Tools::initLogging(), aborting\n")));

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
    do_printVersion (ACE::basename (argv_in[0]));

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
  do_work (schema_directory,
           monster_dictionary_filename,
           validate_XML,
           dump_dictionary);

  timer.stop ();

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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
