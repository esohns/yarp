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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

#include "ace/ACE.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"
#include "ace/OS_main.h"

#include "common_defines.h"

#include "common_file_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"
#include "rpg_dice_dietype.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_subclass.h"
#include "rpg_common_tools.h"
#include "rpg_common_XML_tools.h"

#include "rpg_magic_common_tools.h"
#include "rpg_magic_defines.h"
#include "rpg_magic_dictionary.h"

#include "rpg_item_armor.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_defines.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_weapon.h"

#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_alignment.h"
#include "rpg_character_class_common_tools.h"
#include "rpg_character_common_tools.h"
#include "rpg_character_offhand.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_player.h"
#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#include "rpg_graphics_common_tools.h"

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
  std::string path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [FILE]: player profile (*")
            << ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT)
            << ACE_TEXT_ALWAYS_CHAR (") [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [FILE]: item dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l         : log to a file")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m [FILE]: magic dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t       : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v       : print version information and exit")
            << std::endl;
} // end print_usage

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     std::string& player_filename_out,
                     std::string& item_dictionary_filename_out,
                     bool& logToFile_out,
                     std::string& magic_dictionary_filename_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize configuration
  player_filename_out =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  player_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  player_filename_out +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  player_filename_out += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true);
  item_dictionary_filename_out  = configuration_path;
  item_dictionary_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  item_dictionary_filename_out +=
    ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  logToFile_out                 = false;

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true);
  magic_dictionary_filename_out = configuration_path;
  magic_dictionary_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magic_dictionary_filename_out +=
    ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("f:i:lm:tv"));

  int option = 0;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'f':
      {
        player_filename_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'i':
      {
        item_dictionary_filename_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        magic_dictionary_filename_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
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
do_work (const std::string& magicDictionaryFilename_in,
         const std::string& itemDictionaryFilename_in,
         const std::string& profileFilename_in,
         const std::string& schemaRepository_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1a: initialize randomization
  RPG_Dice::initialize ();

  // step1b: initialize facilities
  RPG_Dice_Common_Tools::initializeStringConversionTables ();
  RPG_Common_Tools::initializeStringConversionTables ();
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
  RPG_Magic_Common_Tools::initialize ();
  RPG_Item_Common_Tools::initializeStringConversionTables ();
  RPG_Character_Common_Tools::initialize ();
  std::string directory;
  if (!RPG_Graphics_Common_Tools::initialize (directory,
                                              0,
                                              false))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initialize(), returning\n")));
    return;
  } // end IF

  // step1c: initialize dictionaries
  try {
    RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->initialize (magicDictionaryFilename_in);
    RPG_ITEM_DICTIONARY_SINGLETON::instance ()->initialize (itemDictionaryFilename_in);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_[Magic|Item]_Dictionary::initialize(), returning\n")));
    return;
  }

  // load file
  RPG_Character_Conditions_t condition;
  condition.insert (CONDITION_NORMAL);
  short int hitpoints = std::numeric_limits<short int>::max ();
  RPG_Magic_Spells_t spells;
  RPG_Player* player_p = RPG_Player::load (profileFilename_in,
                                           schemaRepository_in,
                                           condition,
                                           hitpoints,
                                           spells);
  if (!player_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Player::load(\"%s\"), returning\n"),
                ACE_TEXT (profileFilename_in.c_str ())));
    return;
  } // end IF
  player_p->dump ();

  // clean up
  delete player_p;
  RPG_Graphics_Common_Tools::finalize ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  std::cout << programName_in
#if defined (HAVE_CONFIG_H)
            << ACE_TEXT_ALWAYS_CHAR (" : ")
            << yarp_PACKAGE_VERSION
#endif // HAVE_CONFIG_H
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

  // *PORTABILITY*: on Windows, need to initialize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // step1: initialize configuration
  Common_File_Tools::initialize (argv_in[0]);

  // step1a: set defaults
  // init configuration
  std::string player_filename =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  player_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  player_filename +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  player_filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true);
  std::string item_dictionary_filename  = configuration_path;
  item_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  item_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  bool log_to_file                      = false;

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true);
  std::string magic_dictionary_filename = configuration_path;
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  std::string schema_repository = Common_File_Tools::getWorkingDirectory ();

  bool trace_information                = false;
  bool print_version_and_exit           = false;

  // sanity check
  if (!Common_File_Tools::isDirectory (schema_repository))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("invalid XML schema repository (was: \"%s\"), continuing\n"),
                ACE_TEXT (schema_repository.c_str ())));

    // try fallback
    schema_repository.clear();
  } // end IF

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            player_filename,
                            item_dictionary_filename,
                            log_to_file,
                            magic_dictionary_filename,
                            trace_information,
                            print_version_and_exit))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!Common_File_Tools::isReadable (item_dictionary_filename)  ||
      !Common_File_Tools::isReadable (magic_dictionary_filename) ||
      !Common_File_Tools::isReadable (player_filename))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("missing/invalid (XML) filename, aborting\n")));

    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (log_to_file)
    log_file = Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                 ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])), // program name
                                     log_file,                  // logfile
                                     false,                     // log to syslog ?
                                     false,                     // trace messages ?
                                     trace_information,         // debug messages ?
                                     NULL))                     // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));

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

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();

  // step2: do actual work
  do_work (magic_dictionary_filename,
           item_dictionary_filename,
           player_filename,
           schema_repository);

  timer.stop ();

  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  Common_Log_Tools::finalize ();
  // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
