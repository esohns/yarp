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
#endif
#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_log_tools.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_XML_tools.h"

#include "rpg_magic_common_tools.h"
#include "rpg_magic_defines.h"
#include "rpg_magic_dictionary.h"

#include "rpg_item_common_tools.h"
#include "rpg_item_defines.h"
#include "rpg_item_dictionary.h"

#include "rpg_character_common_tools.h"
#include "rpg_character_defines.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_player.h"
#include "rpg_player_common.h"
#include "rpg_player_common_tools.h"

#include "rpg_monster.h"
#include "rpg_monster_common_tools.h"
#include "rpg_monster_defines.h"
#include "rpg_monster_dictionary.h"

#include "rpg_combat_common_tools.h"

#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#define COMBAT_SIMULATOR_DEF_NUM_BATTLES   1
#define COMBAT_SIMULATOR_DEF_NUM_FOES      0 // random
#define COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES 1
#define COMBAT_SIMULATOR_DEF_NUM_PLAYERS   1
#define COMBAT_SIMULATOR_DEF_STRESS_TEST   false

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          true);

  std::cout << ACE_TEXT ("usage: ")
            << programName_in
            << ACE_TEXT (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT ("currently available options:") << std::endl;
  std::cout << ACE_TEXT ("-b [VALUE]: number of battles")
            << ACE_TEXT (" [")
            << COMBAT_SIMULATOR_DEF_NUM_BATTLES
            << ACE_TEXT ("] (0: endless)")
            << std::endl;
  std::cout << ACE_TEXT ("-f [VALUE]: total number of foes")
            << ACE_TEXT (" [")
            << COMBAT_SIMULATOR_DEF_NUM_FOES
            << ACE_TEXT ("] (0: random)")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-i [FILE] : item dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR ("monster");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-m [FILE] : monster dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-n [VALUE]: number of different monster types")
            << ACE_TEXT (" [")
            << COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-p [VALUE]: number of players")
            << ACE_TEXT (" [")
            << COMBAT_SIMULATOR_DEF_NUM_PLAYERS
            << ACE_TEXT ("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DIRECTORY_STRING);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-s [FILE] : magic dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT ("-v        : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT ("-x        : stress-test")
            << ACE_TEXT (" [")
            << COMBAT_SIMULATOR_DEF_STRESS_TEST
            << ACE_TEXT ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
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
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          true);

  // init results
  numBattles_out = COMBAT_SIMULATOR_DEF_NUM_BATTLES;
  numFoes_out = COMBAT_SIMULATOR_DEF_NUM_FOES;

  magicDictionaryFilename_out   = configuration_path;
  magicDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magicDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DIRECTORY_STRING);
  magicDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  itemDictionaryFilename_out    = configuration_path;
  itemDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  itemDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR ("item");
  itemDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  monsterDictionaryFilename_out = configuration_path;
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  monsterDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR ("character");
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR ("monster");
  monsterDictionaryFilename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monsterDictionaryFilename_out += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  numMonsterTypes_out           = COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES;
  numPlayers_out                = COMBAT_SIMULATOR_DEF_NUM_PLAYERS;
  traceInformation_out          = false;
  printVersionAndExit_out       = false;
  stressTest_out                = COMBAT_SIMULATOR_DEF_STRESS_TEST;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("b:f:i:m:n:p:s:tvx"));
  int option = 0;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'b':
      {
        std::stringstream str;
        str << argumentParser.opt_arg ();
        str >> numBattles_out;

        break;
      }
      case 'f':
      {
        std::stringstream str;
        str << argumentParser.opt_arg ();
        str >> numFoes_out;

        break;
      }
      case 'i':
      {
        itemDictionaryFilename_out = argumentParser.opt_arg ();

        break;
      }
      case 'm':
      {
        monsterDictionaryFilename_out = argumentParser.opt_arg ();

        break;
      }
      case 'n':
      {
        std::stringstream str;
        str << argumentParser.opt_arg ();
        str >> numMonsterTypes_out;

        break;
      }
      case 'p':
      {
        std::stringstream str;
        str << argumentParser.opt_arg ();
        str >> numPlayers_out;

        break;
      }
      case 's':
      {
        magicDictionaryFilename_out = argumentParser.opt_arg ();

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

unsigned int
do_battle (RPG_Player_Party_t& party_in,
           const RPG_Monster_Encounter_t& encounter_in)
{
  RPG_TRACE (ACE_TEXT ("::do_battle"));

  // step1: instantiate monster(s)
  RPG_Monster_Groups_t monsters;
  RPG_Monster_Group_t groupInstance;
  RPG_Monster* monster_p = NULL;
  struct RPG_Engine_Entity entity;
  entity.character = NULL;
  entity.position = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                    std::numeric_limits<unsigned int>::max ());
  //entity.modes();
  //entity.actions();
  entity.is_spawned = true;
  for (RPG_Monster_EncounterConstIterator_t iterator = encounter_in.begin ();
       iterator != encounter_in.end ();
       iterator++)
  {
    groupInstance.clear ();
    for (unsigned int i = 0;
         i < (*iterator).second;
         i++)
    {
      monster_p = NULL;
      entity.character = NULL;
      unsigned short int max_HP = 1;
      unsigned int wealth = 0;
      RPG_Item_List_t items;
      RPG_Character_Conditions_t condition;
      condition.insert (CONDITION_NORMAL);
      short int HP = max_HP;
      RPG_Magic_Spells_t spells;
      entity = RPG_Engine_Common_Tools::createEntity ((*iterator).first,
                                                      max_HP,
                                                      wealth,
                                                      items,
                                                      condition,
                                                      HP,
                                                      spells);
      if (!entity.character)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to create entity \"%s\", aborting\n"),
                    ACE_TEXT ((*iterator).first.c_str ())));

        break;
      } // end IF
      monster_p = dynamic_cast<RPG_Monster*> (entity.character);
      ACE_ASSERT (monster_p);

//       // debug info
//       monster_p->dump();

      groupInstance.push_back (monster_p);
    } // end FOR

    monsters.push_back (groupInstance);
  } // end FOR

  // step2: compute initiative and battle sequence
  RPG_Engine_CombatantSequence_t battleSequence;
  RPG_Engine_Common_Tools::getCombatantSequence (party_in,
                                                 monsters,
                                                 battleSequence);
  // perform a combat round
  unsigned int numRound = 1;
  do
  {
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("----------------round #%d----------------\n"),
                numRound));

    // *TODO*: consider surprise round
    RPG_Engine_Common_Tools::performCombatRound (ATTACK_NORMAL,
                                                 ((numRound == 1) ? DEFENSE_FLATFOOTED
                                                                  : DEFENSE_NORMAL),
                                                 battleSequence);

    if (RPG_Engine_Common_Tools::isPartyHelpless (party_in) ||
        RPG_Engine_Common_Tools::areMonstersHelpless (monsters))
      break;

    numRound++;
  } while (true);

  // sanity check
  if (RPG_Engine_Common_Tools::isPartyHelpless (party_in) &&
      RPG_Engine_Common_Tools::areMonstersHelpless (monsters))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("everybody is HELPLESS --> check implementation !\n")));
  } // end IF

  if (!RPG_Engine_Common_Tools::isPartyHelpless (party_in))
  {
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("party has WON !\n")));

    // *TODO*: award treasure and experience...
  } // end IF
  else
  {
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("monsters have WON !\n")));
  } // end ELSE

  return (numRound * 6); // each round takes 6 seconds...
}

void
do_work (const std::string& schemaRepository_in,
         const std::string& magicDictionaryFilename_in,
         const std::string& itemDictionaryFilename_in,
         const std::string& monsterDictionaryFilename_in,
         unsigned int numMonsterTypes_in,
         unsigned int numFoes_in,
         unsigned int numPlayers_in,
         unsigned int numBattles_in,
         bool stressTest_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: initialize random seed, string conversion facilities, ...
  RPG_Dice::initialize ();
  RPG_Dice_Common_Tools::initializeStringConversionTables ();
  RPG_Common_Tools::initializeStringConversionTables ();
  if (!RPG_Common_XML_Tools::initialize (schemaRepository_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_XML_Tools::initialize(), returning\n")));
    return;
  } // end IF
  RPG_Magic_Common_Tools::initialize ();
  RPG_Item_Common_Tools::initializeStringConversionTables ();
  RPG_Character_Common_Tools::initialize ();
  RPG_Monster_Common_Tools::initializeStringConversionTables ();
  RPG_Combat_Common_Tools::initializeStringConversionTables ();

  // step2a: initialize dictionaries
  try
  {
    RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->initialize (magicDictionaryFilename_in);
    RPG_ITEM_DICTIONARY_SINGLETON::instance ()->init (itemDictionaryFilename_in);
    RPG_MONSTER_DICTIONARY_SINGLETON::instance ()->init (monsterDictionaryFilename_in);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize [magic|item|monster] dictionary, returning\n")));
    return;
  }

  unsigned int gameTime = 0;
  RPG_Player* player_p = NULL;
  do
  {
    // step3: generate a (random) party
    RPG_Player_Party_t party;
    for (unsigned int i = 0;
         i < numPlayers_in;
         i++)
    {
      player_p = RPG_Player::random ();
      if (!player_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Player::random(), continuing\n")));
        continue;
      } // end IF
      ACE_ASSERT (player_p);
//       player_p->dump();

      party.push_back (player_p);
    } // end FOR

    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("generated (random) party of %d player(s)...\n"),
                party.size ()));

    unsigned int numBattle = 1;
    do
    {
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("================battle #%d================\n"),
                  numBattle));

      // step4: generate (random) encounter
      RPG_Character_Alignment alignment;
      alignment.civic = ALIGNMENTCIVIC_ANY;
      alignment.ethic = ALIGNMENTETHIC_ANY;
      RPG_Common_Environment environment;
      environment.climate = CLIMATE_ANY;
      environment.terrain = TERRAIN_ANY;
      RPG_Monster_OrganizationSet_t organizations;
      organizations.insert (ORGANIZATION_ANY);
      RPG_Monster_HitDice hit_dice;
      hit_dice.typeDice = RPG_DICE_DIETYPE_INVALID;
      hit_dice.numDice = 0;
      hit_dice.modifier = 0;
      RPG_Monster_Encounter_t encounter;
      RPG_Monster_Common_Tools::generateRandomEncounter (numMonsterTypes_in,
                                                         numFoes_in,
                                                         alignment,
                                                         environment,
                                                         organizations,
                                                         hit_dice,
                                                         encounter);

      // step5: FIGHT !
      gameTime += do_battle (party,
                             encounter);

      if (RPG_Engine_Common_Tools::isPartyHelpless (party))
        break;

      // party has survived --> rest/recover...
      gameTime += RPG_Player_Common_Tools::restParty (party);

      if (numBattles_in && (numBattle == numBattles_in))
        break;

      numBattle++;
    } while (true);
  } while (stressTest_in);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("finished working (game time: %d second(s))...\n"),
              gameTime));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            //<< YARP_PACKAGE_VERSION
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
  if (version_number << ACE::minor_version ())
  {
    version_number << ACE_TEXT (".");

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
  Common_File_Tools::initialize (argv_in[0]);

  // step1a: set defaults
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          true);

  std::string item_dictionary_filename    = configuration_path;
  item_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  item_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("item");
  item_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  std::string magic_dictionary_filename   = configuration_path;
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DIRECTORY_STRING);
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  std::string monster_dictionary_filename = configuration_path;
  monster_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  monster_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("character");
  monster_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monster_dictionary_filename += ACE_TEXT_ALWAYS_CHAR ("monster");
  monster_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monster_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  std::string schema_repository = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR ("engine");
#endif // DEBUG_DEBUGGER

  unsigned int num_foes                   = COMBAT_SIMULATOR_DEF_NUM_FOES;
  unsigned int num_monster_types          = COMBAT_SIMULATOR_DEF_NUM_FOE_TYPES;
  unsigned int num_players                = COMBAT_SIMULATOR_DEF_NUM_PLAYERS;
  unsigned int num_battles                = COMBAT_SIMULATOR_DEF_NUM_BATTLES;
  bool trace_information                  = false;
  bool print_version_and_exit             = false;
  bool stress_test                        = COMBAT_SIMULATOR_DEF_STRESS_TEST;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            num_battles,
                            num_foes,
                            magic_dictionary_filename,
                            item_dictionary_filename,
                            monster_dictionary_filename,
                            num_monster_types,
                            num_players,
                            trace_information,
                            print_version_and_exit,
                            stress_test))
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
  if (!Common_File_Tools::isReadable (magic_dictionary_filename)   ||
      !Common_File_Tools::isReadable (item_dictionary_filename)    ||
      !Common_File_Tools::isReadable (monster_dictionary_filename) ||
      !Common_File_Tools::isDirectory (schema_repository))
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
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]),   // program name
                                            log_file,                    // logfile
                                            false,                       // log to syslog ?
                                            false,                       // trace messages ?
                                            trace_information,           // debug messages ?
                                            NULL))                       // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

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
  do_work (schema_repository,
           magic_dictionary_filename,
           item_dictionary_filename,
           monster_dictionary_filename,
           num_monster_types,
           num_foes,
           num_players,
           num_battles,
           stress_test);

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
