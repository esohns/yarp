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
#endif
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"

#include "common_file_tools.h"
#include "common_tools.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

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
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif

  std::cout << ACE_TEXT ("usage: ")
            << programName_in
            << ACE_TEXT (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT ("currently available options:") << std::endl;
  std::string path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  std::cout << ACE_TEXT ("-f [FILE]: player profile (*")
            << ACE_TEXT (RPG_PLAYER_PROFILE_EXT)
            << ACE_TEXT (") [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-i [FILE]: item dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("magic");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-m [FILE]: magic dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT ("-v       : print version information and exit")
            << std::endl;
} // end print_usage

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     std::string& player_filename_out,
                     std::string& item_dictionary_filename_out,
                     std::string& magic_dictionary_filename_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif

  // init configuration
  player_filename_out =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  player_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  player_filename_out +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  player_filename_out += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  item_dictionary_filename_out  = configuration_path;
  item_dictionary_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  item_dictionary_filename_out += ACE_TEXT_ALWAYS_CHAR ("item");
  item_dictionary_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary_filename_out +=
    ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  magic_dictionary_filename_out = configuration_path;
  magic_dictionary_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magic_dictionary_filename_out += ACE_TEXT_ALWAYS_CHAR ("magic");
  magic_dictionary_filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary_filename_out +=
    ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("i:m:f:tv"));

  int option = 0;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'i':
      {
        item_dictionary_filename_out = argumentParser.opt_arg ();

        break;
      }
      case 'm':
      {
        magic_dictionary_filename_out = argumentParser.opt_arg ();

        break;
      }
      case 'f':
      {
        player_filename_out = argumentParser.opt_arg ();

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
                    ACE_TEXT (argumentParser.last_option ())));
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
  RPG_Dice_Common_Tools::initStringConversionTables ();
  RPG_Common_Tools::initStringConversionTables ();
  if (!RPG_Common_XML_Tools::initialize (schemaRepository_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_XML_Tools::initialize(), returning\n")));
    return;
  } // end IF
  RPG_Magic_Common_Tools::init ();
  RPG_Item_Common_Tools::initStringConversionTables ();
  RPG_Character_Common_Tools::init ();
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
  try
  {
    RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->init (magicDictionaryFilename_in);
    RPG_ITEM_DICTIONARY_SINGLETON::instance ()->init (itemDictionaryFilename_in);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_[Magic|Item]_Dictionary::init, returning\n")));
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
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  if (version_number << ACE::minor_version ())
  {
    version_number << ".";

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
  std::cout << ACE_TEXT ("ACE: ")
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

  // init configuration
  std::string player_filename =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  player_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  player_filename +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  player_filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  std::string item_dictionary_filename  = configuration_path;
  item_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  item_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("item");
  item_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  std::string magic_dictionary_filename = configuration_path;
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("magic");
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  std::string schema_repository         = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR ("engine");
#endif

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
                            magic_dictionary_filename,
                            trace_information,
                            print_version_and_exit))
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
  if (!Common_File_Tools::isReadable (item_dictionary_filename)  ||
      !Common_File_Tools::isReadable (magic_dictionary_filename) ||
      !Common_File_Tools::isReadable (player_filename))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("missing/invalid (XML) filename, aborting\n")));

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
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                        log_file,                  // logfile
                                        false,                     // log to syslog ?
                                        false,                     // trace messages ?
                                        trace_information,         // debug messages ?
                                        NULL))                     // logger
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
  do_work (magic_dictionary_filename,
           item_dictionary_filename,
           player_filename,
           schema_repository);

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

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;
} // end main
