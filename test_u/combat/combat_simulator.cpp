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

#include <rpg_engine_common.h>
#include <rpg_engine_common_tools.h>

#include <rpg_monster_defines.h>
#include <rpg_monster.h>
#include <rpg_monster_dictionary.h>
#include <rpg_monster_common_tools.h>

#include <rpg_combat_common_tools.h>

#include <rpg_character_common_tools.h>
#include <rpg_character_skills_common_tools.h>
#include <rpg_character_player.h>
#include <rpg_character_player_common.h>

#include <rpg_item_defines.h>
#include <rpg_item_dictionary.h>
#include <rpg_item_common_tools.h>

#include <rpg_magic_defines.h>
#include <rpg_magic_dictionary.h>
#include <rpg_magic_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>
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

#define COMBAT_SIMULATOR_DEF_NUM_BATTLES   1
#define COMBAT_SIMULATOR_DEF_NUM_FOES      0 // random
#define COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES 1
#define COMBAT_SIMULATOR_DEF_NUM_PLAYERS   1
#define COMBAT_SIMULATOR_DEF_STRESS_TEST   false

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-b [VALUE]: number of battles")  << ACE_TEXT(" [") << COMBAT_SIMULATOR_DEF_NUM_BATTLES << ACE_TEXT("]") << ACE_TEXT(" (0: endless)") << std::endl;
  std::cout << ACE_TEXT("-f [VALUE]: total number of foes")  << ACE_TEXT(" [") << COMBAT_SIMULATOR_DEF_NUM_FOES << ACE_TEXT("]") << ACE_TEXT(" (0: random)") << std::endl;
  std::string path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_ITEM_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-i [FILE] : item dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_MONSTER_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-m [FILE] : monster dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-n [VALUE]: number of different monster types") << ACE_TEXT(" [") << COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-p [VALUE]: number of players") << ACE_TEXT(" [") << COMBAT_SIMULATOR_DEF_NUM_PLAYERS << ACE_TEXT("]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_MAGIC_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-s [FILE] : magic dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x        : stress-test") << ACE_TEXT(" [") << COMBAT_SIMULATOR_DEF_STRESS_TEST << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  unsigned int& numBattles_out,
                  unsigned int& numFoes_out,
                  std::string& magicDictionaryFilename_out,
                  std::string& itemDictionaryFilename_out,
                  std::string& monsterDictionaryFilename_out,
                  unsigned int& numMonsterTypes_out,
                  unsigned int& numPlayers_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  bool& stressTest_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  // init results
  numBattles_out = COMBAT_SIMULATOR_DEF_NUM_BATTLES;
  numFoes_out = COMBAT_SIMULATOR_DEF_NUM_FOES;
  magicDictionaryFilename_out = base_data_path;
  magicDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionaryFilename_out += RPG_MAGIC_DEF_DICTIONARY_FILE;
  itemDictionaryFilename_out = base_data_path;
  itemDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionaryFilename_out += RPG_ITEM_DEF_DICTIONARY_FILE;
  monsterDictionaryFilename_out = base_data_path;
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  monsterDictionaryFilename_out += RPG_MONSTER_DEF_DICTIONARY_FILE;
  numMonsterTypes_out = COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES;
  numPlayers_out = COMBAT_SIMULATOR_DEF_NUM_PLAYERS;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  stressTest_out = COMBAT_SIMULATOR_DEF_STRESS_TEST;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("b:f:i:m:n:p:s:tvx"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'b':
      {
        std::stringstream str;
        str << argumentParser.opt_arg();
        str >> numBattles_out;

        break;
      }
      case 'f':
      {
        std::stringstream str;
        str << argumentParser.opt_arg();
        str >> numFoes_out;

        break;
      }
      case 'i':
      {
        itemDictionaryFilename_out = argumentParser.opt_arg();

        break;
      }
      case 'm':
      {
        monsterDictionaryFilename_out = argumentParser.opt_arg();

        break;
      }
      case 'n':
      {
        std::stringstream str;
        str << argumentParser.opt_arg();
        str >> numMonsterTypes_out;

        break;
      }
      case 'p':
      {
        std::stringstream str;
        str << argumentParser.opt_arg();
        str >> numPlayers_out;

        break;
      }
      case 's':
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
      case 'x':
      {
        stressTest_out = true;

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

const unsigned int
do_battle(RPG_Character_Party_t& party_in,
          const RPG_Monster_Encounter_t& encounter_in)
{
  RPG_TRACE(ACE_TEXT("::do_battle"));

  // step1: instantiate monster(s)
  RPG_Monster_Groups_t monsters;
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);
  // *TODO*: define monster abilities, spells, wealth, inventory (i.e. treasure)...
  RPG_Character_Abilities_t abilities;
  RPG_Magic_Spells_t knownSpells;
  unsigned int wealth = 0;
  RPG_Magic_SpellList_t spells;
  RPG_Item_List_t items;

  RPG_Monster_Group_t groupInstance;
  RPG_Monster_Properties properties;
  for (RPG_Monster_EncounterConstIterator_t iterator = encounter_in.begin();
       iterator != encounter_in.end();
       iterator++)
  {
    groupInstance.clear();
    properties = RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getProperties(iterator->first);

    // compute individual hitpoints
    RPG_Dice_RollResult_t results;
    RPG_Dice::simulateRoll(properties.hitDice,
                           iterator->second,
                           results);
    for (RPG_Dice_RollResultIterator_t iterator2 = results.begin();
         iterator2 != results.end();
         iterator2++)
    {
      RPG_Monster monster(// base attributes
                          (iterator->first).c_str(),
                          properties.type,
                          properties.alignment,
                          properties.attributes,
                          properties.skills,
                          properties.feats,
                          abilities,
                          properties.size,
                          (*iterator2),
                          knownSpells,
                          // current status
                          condition,
                          (*iterator2),
                          wealth,
                          spells,
                          items,
                          false);

//       // debug info
//       monster.dump();

      groupInstance.push_back(monster);
    } // end FOR

    monsters.push_back(groupInstance);
  } // end FOR

  // step2: compute initiative and battle sequence
  RPG_Engine_CombatantSequence_t battleSequence;
  RPG_Engine_Common_Tools::getCombatantSequence(party_in,
                                                monsters,
                                                battleSequence);
  // perform a combat round
  unsigned int numRound = 1;
  do
  {
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("----------------round #%d----------------\n"),
               numRound));

    // *TODO*: consider surprise round
    RPG_Engine_Common_Tools::performCombatRound(ATTACK_NORMAL,
                                                ((numRound == 1) ? DEFENSE_FLATFOOTED
                                                                 : DEFENSE_NORMAL),
                                                battleSequence);

    if (RPG_Engine_Common_Tools::isPartyHelpless(party_in) ||
        RPG_Engine_Common_Tools::areMonstersHelpless(monsters))
      break;

    numRound++;
  } while (true);

  // sanity check
  if (RPG_Engine_Common_Tools::isPartyHelpless(party_in) &&
      RPG_Engine_Common_Tools::areMonstersHelpless(monsters))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("everybody is HELPLESS --> check implementation !\n")));
  } // end IF

  if (!RPG_Engine_Common_Tools::isPartyHelpless(party_in))
  {
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("party has WON !\n")));

    // *TODO*: award treasure and experience...
  } // end IF
  else
  {
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("monsters have WON !\n")));
  } // end ELSE

  return (numRound * 6); // each round takes 6 seconds...
}

void
do_work(const std::string& magicDictionaryFilename_in,
        const std::string& itemDictionaryFilename_in,
        const std::string& monsterDictionaryFilename_in,
        const unsigned int& numMonsterTypes_in,
        const unsigned int& numFoes_in,
        const unsigned int& numPlayers_in,
        const unsigned int& numBattles_in,
        const bool& stressTest_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();
  RPG_Magic_Common_Tools::init();
  RPG_Item_Common_Tools::initStringConversionTables();
  RPG_Character_Common_Tools::init();
  RPG_Monster_Common_Tools::initStringConversionTables();
  RPG_Combat_Common_Tools::initStringConversionTables();

  // step2a: init magic dictionary
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

  // step2b: init item dictionary
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

  // step2c: init monster dictionary
  try
  {
    RPG_MONSTER_DICTIONARY_SINGLETON::instance()->init(monsterDictionaryFilename_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Monster_Dictionary::init(), returning\n")));

    return;
  }

  unsigned int gameTime = 0;
  do
  {
    // step3: generate a (random) party
    RPG_Character_Party_t party;
    for (unsigned int i = 0;
        i < numPlayers_in;
        i++)
    {
      RPG_Character_Player player = RPG_Character_Common_Tools::generatePlayerCharacter();
    //   player.dump();

      party.push_back(player);
    } // end FOR

    ACE_DEBUG((LM_INFO,
               ACE_TEXT("generated (random) party of %d player(s)...\n"),
               numPlayers_in));

    unsigned int numBattle = 1;
    do
    {
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("================battle #%d================\n"),
                 numBattle));

      // step4: generate (random) encounter
      RPG_Character_Alignment alignment;
      alignment.civic = ALIGNMENTCIVIC_ANY;
      alignment.ethic = ALIGNMENTETHIC_ANY;
      RPG_Common_Environment environment;
      environment.climate = CLIMATE_ANY;
      environment.terrain = TERRAIN_ANY;
      RPG_Monster_OrganizationSet_t organizations;
      organizations.insert(ORGANIZATION_ANY);
      RPG_Monster_Encounter_t encounter;
      RPG_Monster_Common_Tools::generateRandomEncounter(numMonsterTypes_in,
                                                        numFoes_in,
                                                        alignment,
                                                        environment,
                                                        organizations,
                                                        encounter);

      // step5: FIGHT !
      gameTime += do_battle(party,
                            encounter);

      if (RPG_Engine_Common_Tools::isPartyHelpless(party))
        break;

      // party has survived --> rest/recover...
      gameTime += RPG_Character_Common_Tools::restParty(party);

      if (numBattles_in && (numBattle == numBattles_in))
        break;

      numBattle++;
    } while (true);
  } while (stressTest_in);

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("finished working (game time: %d second(s))...\n"),
             gameTime));
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
    version_number << ACE_TEXT(".");

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
  std::string monsterDictionaryFilename = base_data_path;
  monsterDictionaryFilename += ACE_DIRECTORY_SEPARATOR_STR;
  monsterDictionaryFilename += RPG_MONSTER_DEF_DICTIONARY_FILE;
  unsigned int numFoes = COMBAT_SIMULATOR_DEF_NUM_FOES;
  unsigned int numMonsterTypes = COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES;
  unsigned int numPlayers = COMBAT_SIMULATOR_DEF_NUM_PLAYERS;
  unsigned int numBattles = COMBAT_SIMULATOR_DEF_NUM_BATTLES;
  bool traceInformation = false;
  bool printVersionAndExit = false;
  bool stressTest = COMBAT_SIMULATOR_DEF_STRESS_TEST;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          numBattles,
                          numFoes,
                          magicDictionaryFilename,
                          itemDictionaryFilename,
                          monsterDictionaryFilename,
                          numMonsterTypes,
                          numPlayers,
                          traceInformation,
                          printVersionAndExit,
                          stressTest)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!RPG_Common_File_Tools::isReadable(magicDictionaryFilename) ||
      !RPG_Common_File_Tools::isReadable(itemDictionaryFilename) ||
      !RPG_Common_File_Tools::isReadable(monsterDictionaryFilename))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid (XML) filename, aborting\n")));

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
          monsterDictionaryFilename,
          numMonsterTypes,
          numFoes,
          numPlayers,
          numBattles,
          stressTest);

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
