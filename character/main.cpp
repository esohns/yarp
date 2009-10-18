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
// *IMPORTANT NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rpg_chance_dice.h"
#include "rpg_character_player.h"

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

void print_usage()
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: generator [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT("-t         : trace information")
            << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit")
            << std::endl;
} // end print_usage

const bool process_arguments(const int argc_in,
                             ACE_TCHAR* argv_in[], // cannot be const...
                             bool& traceInformation_out,
                             bool& printVersionAndExit_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
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

void do_work()
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step1: init randomization
  try
  {
    RPG_Chance_Dice::init();
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Chance_Dice::init, returning\n")));

    return;
  }

  // step2: generate/init new player character
  std::string name;
  std::cout << ACE_TEXT("type player name: ");
  std::cin >> name;

  RPG_Character_Gender gender = GENDER_NONE;
  char c = 'm';
  while (gender == GENDER_NONE)
  {
    std::cout << ACE_TEXT("gender (m/f): ");
    std::cin >> c;
    switch (c)
    {
      case 'm':
      {
        gender = GENDER_MALE;
        break;
      }
      case 'f':
      {
        gender = GENDER_FEMALE;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("unrecognized (gender) option \"%c\", try again\n"),
                  c));
        break;
      }
    } // end SWITCH
  } // end WHILE

  RPG_Character_Race race = RACE_BASE;
  c = 'f';
  while (race == RACE_BASE)
  {
    std::cout << ACE_TEXT("race (h/d/e/f/g/o): ");
    std::cin >> c;
    switch (c)
    {
      case 'h':
      {
        race = RACE_HUMAN;
        break;
      }
      case 'd':
      {
        race = RACE_DWARF;
        break;
      }
      case 'e':
      {
        race = RACE_ELF;
        break;
      }
      case 'f':
      {
        race = RACE_HALFLING;
        break;
      }
      case 'g':
      {
        race = RACE_GNOME;
        break;
      }
      case 'o':
      {
        race = RACE_ORC;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("unrecognized (race) option \"%c\", try again\n"),
                  c));
        break;
      }
    } // end SWITCH
  } // end WHILE

  RPG_Character_Class player_class;
  player_class.metaClass = METACLASS_BASE;
  player_class.subClass = SUBCLASS_BASE;
  c = 'f';
  while ((player_class.metaClass == METACLASS_BASE) &&
         (player_class.subClass == SUBCLASS_BASE))
  {
    std::cout << ACE_TEXT("class (f/m/c/t): ");
    std::cin >> c;
    switch (c)
    {
      case 'f':
      {
        player_class.metaClass = METACLASS_WARRIOR;
        player_class.subClass = SUBCLASS_FIGHTER;
        break;
      }
      case 'm':
      {
        player_class.metaClass = METACLASS_WIZARD;
        player_class.subClass = SUBCLASS_WIZARD;
        break;
      }
      case 'c':
      {
        player_class.metaClass = METACLASS_PRIEST;
        player_class.subClass = SUBCLASS_CLERIC;
        break;
      }
      case 't':
      {
        player_class.metaClass = METACLASS_ROGUE;
        player_class.subClass = SUBCLASS_THIEF;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized (class) option \"%c\", try again\n"),
                   c));
        break;
      }
    } // end SWITCH
  } // end WHILE

  RPG_Character_Alignment alignment;
  alignment.civicAlignment = ALIGNMENTCIVIC_INVALID;
  alignment.ethicAlignment = ALIGNMENTETHIC_INVALID;
  c = 'f';
  while ((alignment.civicAlignment == ALIGNMENTCIVIC_INVALID) &&
         (alignment.ethicAlignment == ALIGNMENTETHIC_INVALID))
  {
    std::cout << ACE_TEXT("alignment - civic (l/n/c): ");
    std::cin >> c;
    switch (c)
    {
      case 'l':
      {
        alignment.civicAlignment = ALIGNMENTCIVIC_LAWFUL;
        break;
      }
      case 'n':
      {
        alignment.civicAlignment = ALIGNMENTCIVIC_NEUTRAL;
        break;
      }
      case 'c':
      {
        alignment.civicAlignment = ALIGNMENTCIVIC_CHAOTIC;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized (alignment - civic) option \"%c\", try again\n"),
                   c));
        continue;
      }
    } // end SWITCH

    std::cout << ACE_TEXT("alignment - ethic (g/n/e): ");
    std::cin >> c;
    switch (c)
    {
      case 'g':
      {
        alignment.ethicAlignment = ALIGNMENTETHIC_GOOD;
        break;
      }
      case 'n':
      {
        alignment.ethicAlignment = ALIGNMENTETHIC_NEUTRAL;
        break;
      }
      case 'e':
      {
        alignment.ethicAlignment = ALIGNMENTETHIC_EVIL;
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized (alignment - ethic) option \"%c\", try again\n"),
                   c));
        continue;
      }
    } // end SWITCH
  } // end WHILE

  RPG_Character_Attributes attributes;
  unsigned char* p = NULL;
  RPG_Chance_Roll roll;
  roll.numDice = 2;
  roll.typeDice = D_10;
  roll.modifier = -2; // add +1 if result is 0 --> stats interval 1-18
  RPG_Chance_Dice::RPG_CHANCE_DICE_RESULT_T result;
  c = 'n';
  while (c == 'n')
  {
    p = &(attributes.strength);
    result.clear();
    RPG_Chance_Dice::simulateDiceRoll(roll,
                                      6,
                                      result);
    for (int i = 0;
         i < 6;
         i++, p++)
    {
      *p = (result[i] == 0 ? 1 : result[i]);
    } // end FOR

    std::cout << ACE_TEXT("attributes: ") << std::endl;
    std::cout << ACE_TEXT("strength: ") << attributes.strength << std::endl;
    std::cout << ACE_TEXT("dexterity: ") << attributes.dexterity << std::endl;
    std::cout << ACE_TEXT("constitution: ") << attributes.constitution << std::endl;
    std::cout << ACE_TEXT("intelligence: ") << attributes.intelligence << std::endl;
    std::cout << ACE_TEXT("wisdom: ") << attributes.wisdom << std::endl;
    std::cout << ACE_TEXT("charisma: ") << attributes.charisma << std::endl;

    std::cout << ACE_TEXT("OK ? (y/n): ");
    std::cin >> c;
  } // end WHILE

  RPG_CHARACTER_SKILLS_T skills;
  unsigned short hitpoints = 0;
  RPG_ITEM_LIST_T items;

  RPG_Character_Player player(name,
                              gender,
                              race,
                              player_class,
                              alignment,
                              attributes,
                              skills,
                              0,
                              hitpoints,
                              0,
                              items);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void do_printVersion()
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << ACE_TEXT("generator: ")
            << VERSION
            << std::endl;

  // create version string...
  // *IMPORTANT NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  if (version_number << ACE::major_version())
  {
    version_number << ACE_TEXT(".");
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

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
                 ACE_TEXT("failed to convert: \"%s\", returning\n"),
                 ACE_OS::strerror(errno)));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ")
            << version_number.str()
            << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int ACE_TMAIN(int argc,
              ACE_TCHAR* argv[])
{
  ACE_TRACE(ACE_TEXT("::main"));

  // step1: init
  // step1a set defaults
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage();

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments


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
    do_printVersion();

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work();

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
