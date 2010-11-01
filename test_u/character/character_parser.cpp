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

#include <rpg_character_defines.h>
#include <rpg_character_alignmentcivic.h>
#include <rpg_character_alignmentethic.h>
#include <rpg_character_alignment.h>
#include <rpg_character_offhand.h>
#include <rpg_character_player.h>
#include <rpg_character_common_tools.h>
#include <rpg_character_class_common_tools.h>
#include <rpg_character_skills_common_tools.h>

#include <rpg_item_defines.h>
#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

#include <rpg_magic_defines.h>
#include <rpg_magic_dictionary.h>
#include <rpg_magic_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_dietype.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_subclass.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <numeric>

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-f [FILE]: player profile (*.xml)") << std::endl;
  std::string path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_ITEM_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-i [FILE]: item dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_MAGIC_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-m [FILE]: magic dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT("-v       : print version information and exit") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& playerFilename_out,
                  std::string& itemDictionaryFilename_out,
                  std::string& magicDictionaryFilename_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  // init results
  itemDictionaryFilename_out = base_data_path;
  itemDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionaryFilename_out += RPG_ITEM_DEF_DICTIONARY_FILE;
  magicDictionaryFilename_out = base_data_path;
  magicDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionaryFilename_out += RPG_MAGIC_DEF_DICTIONARY_FILE;
  playerFilename_out.clear();
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("i:m:f:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'i':
      {
        itemDictionaryFilename_out = argumentParser.opt_arg();

        break;
      }
      case 'm':
      {
        magicDictionaryFilename_out = argumentParser.opt_arg();

        break;
      }
      case 'f':
      {
        playerFilename_out = argumentParser.opt_arg();

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
do_work(const std::string& magicDictionaryFilename_in,
        const std::string& itemDictionaryFilename_in,
        const std::string& playerFilename_in,
        const std::string& schemaRepository_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1a: init randomization
  RPG_Dice::init();

  // step1b: init facilities
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();
  RPG_Magic_Common_Tools::init();
  RPG_Item_Common_Tools::initStringConversionTables();
  RPG_Character_Common_Tools::init();
//   RPG_Monster_Common_Tools::initStringConversionTables();

  // step1c: init magic dictionary
  try
  {
    RPG_MAGIC_DICTIONARY_SINGLETON::instance()->init(magicDictionaryFilename_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Magic_Dictionary::init, returning\n")));

    return;
  }

  // step1d: init item dictionary
  try
  {
    RPG_ITEM_DICTIONARY_SINGLETON::instance()->init(itemDictionaryFilename_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Item_Dictionary::init, returning\n")));

    return;
  }

  // load file
  RPG_Character_Player player = RPG_Character_Player::load(playerFilename_in,
                                                           schemaRepository_in);
  player.dump();

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
            << version_number.str()
            << std::endl;
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
  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  // init results
  std::string itemDictionaryFilename = base_data_path;
  itemDictionaryFilename += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionaryFilename += RPG_ITEM_DEF_DICTIONARY_FILE;
  std::string magicDictionaryFilename = base_data_path;
  magicDictionaryFilename += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionaryFilename += RPG_MAGIC_DEF_DICTIONARY_FILE;
  std::string playerFilename;
  std::string schemaRepository = base_data_path;
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schemaRepository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               schemaRepository.c_str()));

    // try fallback
    schemaRepository.clear();
  } // end IF

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          playerFilename,
                          itemDictionaryFilename,
                          magicDictionaryFilename,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!RPG_Common_File_Tools::isReadable(itemDictionaryFilename) ||
      !RPG_Common_File_Tools::isReadable(magicDictionaryFilename) ||
      !RPG_Common_File_Tools::isReadable(playerFilename))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("missing/invalid (XML) filename, aborting\n")));

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
  do_work(magicDictionaryFilename,
          itemDictionaryFilename,
          playerFilename,
          schemaRepository);

  timer.stop();

//   // debug info
//   std::string working_time_string;
//   ACE_Time_Value working_time;
//   timer.elapsed_time(working_time);
//   RPS_FLB_Common_Tools::Period2String(working_time,
//                                       working_time_string);
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
//              working_time_string.c_str()));

  return EXIT_SUCCESS;
} // end main