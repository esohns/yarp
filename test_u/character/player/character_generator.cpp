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
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/OS_main.h"

#include "common_defines.h"
#include "common_file_tools.h"

#include "common_log_tools.h"

#ifdef HAVE_CONFIG_H
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

#include "rpg_magic_common_tools.h"
#include "rpg_magic_defines.h"
#include "rpg_magic_dictionary.h"

#include "rpg_item_base.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_defines.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"

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

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"

#include "rpg_engine_common_tools.h"

#include "rpg_client_defines.h"

#define CHARACTER_GENERATOR_DEF_NUM_PLAYERS 1
#define CHARACTER_GENERATOR_DEF_RANDOM      false

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true);

  std::cout << ACE_TEXT ("usage: ")
            << programName_in
            << ACE_TEXT (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT ("currently available options:")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-g [FILE]  : graphics dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("item"); // directory
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-i [FILE]  : item dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DIRECTORY_STRING); // directory
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT ("-m [FILE]  : magic dictionary (*.xml)")
            << ACE_TEXT (" [\"")
            << path
            << ACE_TEXT ("\"]")
            << std::endl;
  std::cout << ACE_TEXT ("-n [VALUE] : (auto-)generate #players")
            << ACE_TEXT (" [")
            << CHARACTER_GENERATOR_DEF_NUM_PLAYERS
            << ACE_TEXT ("; 0:off]")
            << std::endl;
  path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  std::cout << ACE_TEXT ("-o <[FILE]>: output file")
            << ACE_TEXT (" [")
            << path
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-r         : random (non-interactive)")
            << ACE_TEXT (" [")
            << CHARACTER_GENERATOR_DEF_RANDOM
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-t         : trace information")
            << std::endl;
  std::cout << ACE_TEXT ("-v         : print version information and exit")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     std::string& itemDictionary_out,
                     std::string& magicDictionary_out,
                     std::string& graphicsDictionary_out,
                     unsigned int& numPlayers_out,
                     std::string& outputFile_out,
                     bool& random_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  // init results
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true);
  itemDictionary_out      = configuration_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true);
  magicDictionary_out     = configuration_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true);
  graphicsDictionary_out  = configuration_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  numPlayers_out          = CHARACTER_GENERATOR_DEF_NUM_PLAYERS;

  outputFile_out          =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  outputFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  std::string default_output_path = outputFile_out;

  random_out              = CHARACTER_GENERATOR_DEF_RANDOM;
  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("g:i:m:n:o::rtv"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'g':
      {
        graphicsDictionary_out =
          ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'i':
      {
        itemDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'm':
      {
        magicDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'n':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> numPlayers_out;

        break;
      }
      case 'o':
      {
        ACE_TCHAR* output_file = argumentParser.opt_arg ();
        if (output_file)
          outputFile_out = ACE_TEXT_ALWAYS_CHAR (output_file);

        break;
      }
      case 'r':
      {
        random_out = true;

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

  if (outputFile_out == default_output_path)
  {
    outputFile_out +=
      RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
    outputFile_out += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  } // end IF

  return true;
}

bool
do_printSkillsTable (RPG_Character_Skills_t& skills_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printSkillsTable"));

  RPG_Character_SkillsIterator_t skills_iterator = skills_in.end ();

  unsigned int skills_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  RPG_Common_SkillToStringTableIterator_t iterator =
    RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.begin ();
  do
  {
    for (unsigned int i = 0;
         i < skills_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator ==
          RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.end ())
        break;

      skills_iterator = skills_in.find (iterator->first);

//       std::cout.setf(ios::right);
      std::cout << ACE_TEXT ("[")
                << std::setw (2)
                << std::right
                << index
                << ACE_TEXT ("]: ")
                << std::setw (20)
                << std::left
                << iterator->second
                << ACE_TEXT (": ");
      if (skills_iterator != skills_in.end ())
      {
        std::cout << std::setw (3)
                  << std::right
                  << static_cast<unsigned int> (skills_iterator->second)
                  << ACE_TEXT (" ");
      } // end IF
      else
        std::cout << ACE_TEXT ("nil ");
//       std::cout.unsetf(ios::right);
    } // end FOR

    std::cout << std::endl;
  } while (iterator !=
           RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.end ());

  index--;

  std::cout << ACE_TEXT ("enter index: ");
  std::cin >> choice;
  // sanity check
  if ((choice > index) || (choice < 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid index %d (max: %d), try again\n"),
                choice,
                index));

    // clean input buffer
    if (!std::cin)
    {
      std::cin.clear ();
      std::cin.ignore (std::numeric_limits<std::streamsize>::max (),
                       ACE_TEXT_ALWAYS_CHAR ('\n'));
    } // end IF

    return false;
  } // end IF

  // increase skill rank
  choice -= 1;
  iterator = RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.begin ();
  ACE_ASSERT (RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.size () >=
              choice);
  std::advance (iterator, choice);
  skills_iterator = skills_in.find (iterator->first);
  if (skills_iterator != skills_in.end ())
    (skills_iterator->second)++;
  else
    skills_in.insert (std::make_pair (iterator->first,
                                      static_cast<unsigned char> (1)));

  return true;
}

bool
do_printFeatsTable (const RPG_Common_SubClass& subClass_in,
                    const RPG_Character_Attributes& attributes_in,
                    const RPG_Character_Skills_t& skills_in,
                    const RPG_Character_Abilities_t& abilities_in,
                    RPG_Character_Feats_t& feats_inout)
{
  RPG_TRACE (ACE_TEXT ("::do_printFeatsTable"));

  RPG_Character_FeatsIterator_t feats_iterator = feats_inout.end ();

  unsigned int feats_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  RPG_Character_FeatToStringTableIterator_t iterator =
    RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.begin ();
  do
  {
    for (unsigned int i = 0;
         i < feats_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator ==
          RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.end ())
        break;

      feats_iterator = feats_inout.find (iterator->first);
      if ((feats_iterator == feats_inout.end ()) &&
          (RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites (iterator->first,
                                                                      subClass_in,
                                                                      1,
                                                                      attributes_in,
                                                                      skills_in,
                                                                      feats_inout,
                                                                      abilities_in)))
      {
//         std::cout.setf(ios::right);
        std::cout << ACE_TEXT ("[")
                  << std::setw (2)
                  << std::right
                  << index
                  << ACE_TEXT ("]: ")
                  << std::setw (20)
                  << std::left
                  << ACE_TEXT (iterator->second.c_str ())
                  << ACE_TEXT (" ");
//         std::cout.unsetf(ios::right);
      } // end IF
    } // end FOR

    std::cout << std::endl;
  } while (iterator !=
           RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.end ());

  index--;

  std::cout << ACE_TEXT ("enter index: ");
  std::cin >> choice;
  // sanity check
  if ((choice > index) || (choice < 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid index %d (max: %d), try again\n"),
                choice,
                index));

    // clean input buffer
    if (!std::cin)
    {
      std::cin.clear ();
      std::cin.ignore (std::numeric_limits<std::streamsize>::max (),
                       ACE_TEXT_ALWAYS_CHAR ('\n'));
    } // end IF

    return false;
  } // end IF

  // (try to) append chosen feat
  choice -= 1;
  iterator =
    RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.begin ();
  ACE_ASSERT (RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.size () >=
              choice);
  std::advance (iterator, choice);

  feats_iterator = feats_inout.find (iterator->first);
  if ((feats_iterator != feats_inout.end ()) ||
      (!RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites (iterator->first,
                                                                   subClass_in,
                                                                   1,
                                                                   attributes_in,
                                                                   skills_in,
                                                                   feats_inout,
                                                                   abilities_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid index %d, try again\n"),
                choice));
    return false;
  } // end IF

  feats_inout.insert (iterator->first);

  return true;
}

bool
do_printSpellsTable (const RPG_Magic_SpellTypes_t& spells_in,
                     RPG_Magic_SpellType& spell_inout)
{
  RPG_TRACE (ACE_TEXT ("::do_printSpellsTable"));

  // init return value
  spell_inout = RPG_MAGIC_SPELLTYPE_INVALID;

  RPG_Magic_SpellTypesIterator_t iterator = spells_in.begin ();
  unsigned int spells_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  do
  {
    for (unsigned int i = 0;
         i < spells_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator == spells_in.end ())
        break;

//         std::cout.setf(ios::right);
      std::cout << ACE_TEXT ("[")
                << std::setw (2)
                << std::right
                << index
                << ACE_TEXT ("]: ")
                << std::setw (20)
                << std::left
                << ACE_TEXT (RPG_Magic_Common_Tools::spellToName (*iterator).c_str ());
//         std::cout.unsetf(ios::right);
    } // end FOR

    std::cout << std::endl;
  } while (iterator != spells_in.end ());

  index--;

  std::cout << ACE_TEXT ("enter index: ");
  std::cin >> choice;
  // sanity check
  if ((choice > index) || (choice < 1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid index %d (max: %d), try again\n"),
                choice,
                index));

    // clean input buffer
    if (!std::cin)
    {
      std::cin.clear ();
      std::cin.ignore (std::numeric_limits<std::streamsize>::max (),
                       ACE_TEXT_ALWAYS_CHAR ('\n'));
    } // end IF

    return false;
  } // end IF

  // append chosen spell
  choice -= 1;
  iterator = spells_in.begin ();
  ACE_ASSERT (spells_in.size () >= choice);
  std::advance (iterator, choice);

  // select chosen spell type
  spell_inout = *iterator;

  return true;
}

RPG_Player*
do_generatePlayer ()
{
  RPG_TRACE (ACE_TEXT ("::do_generatePlayer"));

  // step1: name
  std::string name;
  std::cout << ACE_TEXT ("name: ");
  std::cin >> name;

  // step2: gender
  RPG_Character_Gender gender = GENDER_NONE;
  char c = ' ';
  do
  {
    std::cout << ACE_TEXT ("gender (m/f): ");
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized (gender) option \"%c\", try again\n"),
                    c));
        break;
      }
    } // end SWITCH
  } while (gender == GENDER_NONE);

  // step3: race
  RPG_Character_Race race = RACE_NONE;
  c = ' ';
  do
  {
    std::cout << ACE_TEXT ("race (h/d/e/f/g/o): ");
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized (race) option \"%c\", try again\n"),
                    c));
        break;
      }
    } // end SWITCH
  } while (race == RACE_NONE);
  RPG_Character_Race_t player_race (0);
  player_race.flip (race - 1);

  // step4: class(es)
  RPG_Character_Class player_class;
  player_class.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  RPG_Common_SubClass player_subclass = RPG_COMMON_SUBCLASS_INVALID;
  c = ' ';
  do
  {
    std::cout << ACE_TEXT ("class (f/m/c/t): ");
    std::cin >> c;
    switch (c)
    {
      case 'f':
      {
        player_subclass = SUBCLASS_FIGHTER;
        break;
      }
      case 'm':
      {
        player_subclass = SUBCLASS_WIZARD;
        break;
      }
      case 'c':
      {
        player_subclass = SUBCLASS_CLERIC;
        break;
      }
      case 't':
      {
        player_subclass = SUBCLASS_THIEF;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized (class) option \"%c\", try again\n"),
                    c));
        break;
      }
    } // end SWITCH
  } while (player_subclass == RPG_COMMON_SUBCLASS_INVALID);
  player_class.subClasses.insert (player_subclass);
  player_class.metaClass =
    RPG_Character_Class_Common_Tools::subClassToMetaClass (player_subclass);

  // step5: alignment
  RPG_Character_Alignment alignment;
  alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  c = ' ';
  do
  {
    std::cout << ACE_TEXT ("alignment - civic (l/n/c): ");
    std::cin >> c;
    switch (c)
    {
      case 'l':
      {
        alignment.civic = ALIGNMENTCIVIC_LAWFUL;
        break;
      }
      case 'n':
      {
        alignment.civic = ALIGNMENTCIVIC_NEUTRAL;
        break;
      }
      case 'c':
      {
        alignment.civic = ALIGNMENTCIVIC_CHAOTIC;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized (alignment - civic) option \"%c\", try again\n"),
                    c));
        break;
      }
    } // end SWITCH

    std::cout << ACE_TEXT ("alignment - ethic (g/n/e): ");
    std::cin >> c;
    switch (c)
    {
      case 'g':
      {
        alignment.ethic = ALIGNMENTETHIC_GOOD;
        break;
      }
      case 'n':
      {
        alignment.ethic = ALIGNMENTETHIC_NEUTRAL;
        break;
      }
      case 'e':
      {
        alignment.ethic = ALIGNMENTETHIC_EVIL;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized (alignment - ethic) option \"%c\", try again\n"),
                    c));
        break;
      }
    } // end SWITCH
  } while ((alignment.civic == RPG_CHARACTER_ALIGNMENTCIVIC_INVALID) ||
           (alignment.ethic == RPG_CHARACTER_ALIGNMENTETHIC_INVALID));

  // step6: attributes
  RPG_Character_Attributes attributes;
  unsigned char* p = NULL;
  RPG_Dice_Roll roll;
  roll.numDice = 2;
  roll.typeDice = D_10;
  roll.modifier = -2; // interval: 0-18
  RPG_Dice_RollResult_t result;
  c = 'n';
  do
  {
    p = &(attributes.strength);

    // make sure the result is somewhat balanced...
    // *NOTE*: INT must be > 2 (smaller values are reserved for animals...)
    int sum = 0;
    do
    {
      sum = 0;
      result.clear ();
      RPG_Dice::simulateRoll (roll,
                              6,
                              result);
      sum = std::accumulate (result.begin (),
                             result.end (),
                             0);
    } while ((sum <= RPG_PLAYER_ATTRIBUTE_MINIMUM_SUM) ||
             (*(std::min_element (result.begin (),
                                  result.end ())) <= 9) ||
             (result[3] < 3)); // Note: already covered by the last case
    for (int i = 0;
         i < 6;
         i++, p++)
    {
      // add +1 if result is 0 --> stats interval 1-18
      *p = (result[i] == 0 ? 1 : result[i]);
    } // end FOR

    std::cout << ACE_TEXT ("base attributes: ")
              << std::endl;
    std::cout << ACE_TEXT ("strength: ")
              << static_cast<int> (attributes.strength)
              << std::endl;
    std::cout << ACE_TEXT ("dexterity: ")
              << static_cast<int> (attributes.dexterity)
              << std::endl;
    std::cout << ACE_TEXT ("constitution: ")
              << static_cast<int> (attributes.constitution)
              << std::endl;
    std::cout << ACE_TEXT ("intelligence: ")
              << static_cast<int> (attributes.intelligence)
              << std::endl;
    std::cout << ACE_TEXT ("wisdom: ")
              << static_cast<int> (attributes.wisdom)
              << std::endl;
    std::cout << ACE_TEXT ("charisma: ")
              << static_cast<int> (attributes.charisma)
              << std::endl;
    std::cout << ACE_TEXT ("[sum: ")
              << sum
              << ACE_TEXT ("] --> OK ? (y/n): ");
    std::cin >> c;
  } while (c == 'n');

  // step7: skills
  RPG_Character_Skills_t skills;
  ACE_INT8 INT_modifier =
    RPG_Character_Common_Tools::getAttributeAbilityModifier (attributes.intelligence);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("INT modifier (attribute value: %d) is: %d...\n"),
              attributes.intelligence,
              INT_modifier));

  unsigned int initial_skill_points = 0;
  RPG_Character_Skills_Common_Tools::getSkillPoints (player_subclass,
                                                     INT_modifier,
                                                     initial_skill_points);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initial skill points for subClass \"%s\" (INT modifier: %d) is: %d...\n"),
              ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (player_subclass).c_str ()),
              INT_modifier,
              initial_skill_points));

  do
  {
    // header line
    std::cout << ACE_TEXT ("remaining skill points: ")
              << initial_skill_points
              << std::endl;
    std::cout << std::setw (80)
              << std::setfill (ACE_TEXT_ALWAYS_CHAR ('-'))
              << ACE_TEXT ("")
              << std::setfill (ACE_TEXT_ALWAYS_CHAR (' '))
              << std::endl;

    if (do_printSkillsTable (skills))
      initial_skill_points--;
  } while (initial_skill_points);

  // step8: feats & abilities
  RPG_Character_Feats_t feats;
  unsigned int initial_feats = 0;
  RPG_Character_Abilities_t abilities;
  RPG_Character_Skills_Common_Tools::getNumFeatsAbilities (race,            // race
                                                           player_subclass, // subclass
                                                           1,               // current level
                                                           feats,           // (base) feats
                                                           initial_feats,   // initial feats (level 1)
                                                           abilities);      // (base) abilities
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initial feat(s): %d...\n"),
              initial_feats));
  do
  {
    // header line
    std::cout << ACE_TEXT ("remaining feat(s): ")
              << initial_feats
              << std::endl;
    std::cout << std::setw (80)
              << std::setfill (ACE_TEXT_ALWAYS_CHAR ('-'))
              << ACE_TEXT ("")
              << std::setfill (ACE_TEXT_ALWAYS_CHAR (' '))
              << std::endl;

    if (do_printFeatsTable (player_subclass,
                            attributes,
                            skills,
                            abilities,
                            feats))
      initial_feats--;
  } while (initial_feats);

  // step9: off-hand
  RPG_Character_OffHand off_hand = OFFHAND_LEFT;
  roll.numDice = 1;
  roll.typeDice = D_100;
  roll.modifier = 0;
  // *TODO*: 10% (?) of people are "lefties"...
  result.clear ();
  RPG_Dice::simulateRoll (roll,
                          1,
                          result);
  if (result.front () <= 10)
    off_hand = OFFHAND_RIGHT;

  // step10: HP
  unsigned short int hit_points = 0;
  roll.numDice = 1;
  roll.typeDice = RPG_Character_Common_Tools::getHitDie (player_subclass);
  roll.modifier = 0;
  result.clear ();
//   RPG_Dice::simulateRoll(roll,
//                          1,
//                          result);
//   hitPoints = result[0];
  // *NOTE*: players start with maxed HP...
  hit_points = RPG_Character_Common_Tools::getHitDie (player_subclass);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initial hit points (hit die: \"%s\"): %d...\n"),
              ACE_TEXT (RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString (roll.typeDice).c_str ()),
              hit_points));

  // step11: spells
  unsigned char num_known_spells = 0;
  unsigned char num_spells = 0;
  RPG_Magic_SpellTypes_t known_spells;
  RPG_Magic_Spells_t spells;
  RPG_Magic_SpellTypes_t available;
  RPG_Magic_SpellType chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
  int num_chosen = 0;
  RPG_Magic_SpellsIterator_t available_iterator;
  RPG_Magic_CasterClassUnion caster_class;
  caster_class.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin ();
       iterator != player_class.subClasses.end ();
       iterator++)
  {
    if (!RPG_Magic_Common_Tools::isCasterClass (*iterator))
      continue;

    for (unsigned char i = 0;
         i <= RPG_COMMON_MAX_SPELL_LEVEL;
         i++)
    {
      num_known_spells = 0;
      num_spells = 0;

      // step1: get list of available spells
      caster_class.subclass = *iterator;
      available =
        RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpells (caster_class,
                                                                i);

      // *NOTE*: divine casters know ALL spells from the levels they can cast
      if (!RPG_Magic_Common_Tools::isDivineCasterClass (*iterator))
      {
        // step2: compute # known spells
        num_known_spells =
          static_cast<unsigned char> (RPG_Magic_Common_Tools::getNumKnownSpells (*iterator,
                                                                                 1,
                                                                                 i));

        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("number of initial known spells (lvl %d) for subClass \"%s\" is: %d...\n"),
                    i,
                    ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator).c_str ()),
                    num_known_spells));

        // step3: choose known spells
        num_chosen = 0;
        while (num_chosen < num_known_spells)
        {
          chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
          // header line
          std::cout << ACE_TEXT ("remaining spell(s): ")
                    << (num_known_spells - num_chosen)
                    << std::endl;
          std::cout << std::setw (80)
                    << std::setfill (ACE_TEXT_ALWAYS_CHAR ('-'))
                    << ACE_TEXT ("")
                    << std::setfill (ACE_TEXT_ALWAYS_CHAR (' '))
                    << std::endl;

          if (do_printSpellsTable (available,
                                   chosen_spell))
          {
            known_spells.insert (chosen_spell);
            available.erase (chosen_spell);
            num_chosen++;
          } // end IF
        } // end WHILE
      } // end IF

      // step4: compute # prepared spells
      num_spells = RPG_Magic_Common_Tools::getNumSpells (*iterator,
                                                         1,
                                                         i);

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("number of initial memorized/prepared spells (lvl %d) for subClass \"%s\" is: %d...\n"),
                  i,
                  ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator).c_str ()),
                  num_spells));

      // step5: choose prepared spells
      num_chosen = 0;
      while (num_chosen < num_spells)
      {
        chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
        // header line
        std::cout << ACE_TEXT ("remaining spell(s): ")
                  << (num_spells - num_chosen)
                  << std::endl;
        std::cout << std::setw (80)
                  << std::setfill (ACE_TEXT_ALWAYS_CHAR ('-'))
                  << ACE_TEXT ("")
                  << std::setfill (ACE_TEXT_ALWAYS_CHAR (' '))
                  << std::endl;

        if (do_printSpellsTable ((RPG_Magic_Common_Tools::isDivineCasterClass (*iterator) ? available
                                                                                          : known_spells),
                                 chosen_spell))
        {
          spells.push_back (chosen_spell);
          num_chosen++;
        } // end IF
      } // end WHILE
    } // end FOR
  } // end FOR

  // step12: initialize condition
  RPG_Character_Conditions_t condition;
  condition.insert (CONDITION_NORMAL);

  // step13: choose (appropriate) initial set of items
  RPG_Item_List_t items =
    RPG_Engine_Common_Tools::generateStandardItems (player_subclass);

  // instantiate player character
  RPG_Player* player_p = NULL;
  ACE_NEW_NORETURN (player_p,
                    RPG_Player (// base attributes
                                name,
                                gender,
                                player_race,
                                player_class,
                                alignment,
                                attributes,
                                skills,
                                feats,
                                abilities,
                                off_hand,
                                hit_points,
                                known_spells,
                                // extended data
                                0,
                                RPG_PLAYER_START_MONEY,
                                items,
                                // current status
                                condition,
                                hit_points,
                                spells));
  if (!player_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("unable to allocate memory(%U), aborting\n"),
                sizeof (RPG_Player)));
    return NULL;
  } // end IF

  return player_p;
}

void
do_work (const std::string& schemaRepository_in,
         const std::string& magicDictionaryFilename_in,
         const std::string& itemDictionaryFilename_in,
         const std::string& graphicsDictionary_in,
         const std::string& graphicsDirectory_in,
         unsigned int graphicsCacheSize_in,
         unsigned int numPlayers_in,
         const std::string outputFile_in,
         bool random_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: init engine (randomization, string conversion facilities, ...)
  std::string schema_repository_string = schemaRepository_in;
  schema_repository_string += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository_string += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING);
  schema_repository_string += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository_string += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::vector<std::string> schema_directories_a;
  schema_directories_a.push_back (schema_repository_string);
  RPG_Engine_Common_Tools::initialize (schema_directories_a,
                                       magicDictionaryFilename_in,
                                       itemDictionaryFilename_in,
                                       std::string ());
  if (!RPG_Graphics_Common_Tools::initialize (graphicsDirectory_in,
                                              graphicsCacheSize_in,
                                              false)) // DON'T init SDL bits...
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initialize(): \"%m\", returning\n")));
    return;
  } // end IF

  // init graphics dictionary
  if (!RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->initialize (graphicsDictionary_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Dictionary::init, returning\n")));
    return;
  }

  // step2: display menu options
  bool done = false;
  char c = ' ';
  unsigned int num_players = 0;
  RPG_Player* player_p = NULL;
  do
  {
    if (player_p)
    {
      delete player_p; player_p = NULL;
    } // end IF

    // step1: generate new player character
    player_p = (random_in ? RPG_Player::random ()
                          : do_generatePlayer ());
    ACE_ASSERT (player_p);
    player_p->dump ();

    if (!random_in)
    {
      // step2: display menu options
      c = ' ';
      do
      {
        std::cout << ACE_TEXT ("menu options (rETRY/sAVE/qUIT): ");
        std::cin >> c;
        switch (c)
        {
          case 'r':
            break;
          case 's':
          {
            std::string path = outputFile_in;
            if (path.empty ())
            {
              path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
              path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
              path += player_p->getName ();
              path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
            } // end IF

            // sanity check
            if (Common_File_Tools::isReadable (path))
            {
              bool proceed = false;
              c = ' ';
              do
              {
                std::cout << ACE_TEXT ("file \"")
                          << path
                          << ACE_TEXT ("\" exists, overwrite ? (y/n): ");
                std::cin >> c;
                switch (c)
                {
                  case 'y':
                  {
                    proceed = true;

                    break;
                  }
                  case 'n':
                    break;
                  default:
                  {
                    ACE_DEBUG ((LM_ERROR,
                                ACE_TEXT ("unrecognized option \"%c\", try again\n"),
                                c));
                    break;
                  }
                } // end SWITCH
              } while ((c != 'y') &&
                       (c != 'n'));

              if (!proceed)
                break;
            } // end IF

            if (!player_p->save (path))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to RPG_Character_Player::save(\"%s\"), continuing\n"),
                          ACE_TEXT (path.c_str ())));

            break;
          }
          case 'q':
          {
            done = true;

            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("unrecognized option \"%c\", try again\n"),
                        c));
            break;
          }
        } // end SWITCH
      } while ((c != 'r') &&
               (c != 'q'));
    } // end IF
    else
    {
      // save player
      std::string path = outputFile_in;
      if (path.empty ())
      {
        path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
        path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
        path += player_p->getName ();
        path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
      } // end IF

      //// sanity check
      //if (RPG_Common_File_Tools::isReadable(path))
      //{
      //	bool proceed = false;
      //	c = ' ';
      //	do
      //	{
      //		std::cout << ACE_TEXT("file \"")
      //			<< path
      //			<< ACE_TEXT("\" exists, overwrite ? (y/n): ");
      //		std::cin >> c;
      //		switch (c)
      //		{
      //			case 'y':
      //			{
      //				proceed = true;

      //				break;
      //			}
      //			case 'n':
      //				break;
      //			default:
      //			{
      //				ACE_DEBUG((LM_ERROR,
      //									 ACE_TEXT("unrecognized option \"%c\", try again\n"),
      //								   c));
      //				break;
      //			}
      //		} // end SWITCH
      //	} while ((c != 'y') &&
      //					 (c != 'n'));

      //	if (!proceed)
      //		continue;
      //} // end IF

      if (!player_p->save (path))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Character_Player::save(\"%s\"), continuing\n"),
                    ACE_TEXT (path.c_str ())));
//      else
//        ACE_DEBUG((LM_DEBUG,
//                   ACE_TEXT("saved player \"%s\" to file: \"%s\"\n"),
//                   ACE_TEXT(player_p->getName().c_str()),
//                   ACE_TEXT(path.c_str())));

      num_players++;
      if (num_players == numPlayers_in)
        done = true;
    } // end ELSE
  } while (!done);

  // clean up
  RPG_Graphics_Common_Tools::finalize ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            //<< RPG_VERSION
#endif
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... We need this, as the library soname is compared to this
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
  Common_File_Tools::initialize (argv_in[0]);

  // step1a: set defaults
  // init configuration
  std::string schema_repository = Common_File_Tools::getWorkingDirectory ();

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true);
  std::string item_dictionary_filename = configuration_path;
  item_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  item_dictionary_filename += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true);
  std::string magic_dictionary_filename = configuration_path;
  magic_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magic_dictionary_filename +=
    ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true);
  std::string graphics_dictionary_filename = configuration_path;
  graphics_dictionary_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_dictionary_filename +=
    ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          false);
  std::string graphics_directory = data_path;

  unsigned int num_players = CHARACTER_GENERATOR_DEF_NUM_PLAYERS;

  std::string output_filename =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  output_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  output_filename +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  output_filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  bool random = CHARACTER_GENERATOR_DEF_RANDOM;
  bool trace_information = false;
  bool print_version_and_exit = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            item_dictionary_filename,
                            magic_dictionary_filename,
                            graphics_dictionary_filename,
                            num_players,
                            output_filename,
                            random,
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

  // step1c: validate arguments
  if (!Common_File_Tools::isReadable (item_dictionary_filename)     ||
      !Common_File_Tools::isReadable (magic_dictionary_filename)    ||
      !Common_File_Tools::isReadable (graphics_dictionary_filename) ||
      !Common_File_Tools::isDirectory (graphics_directory))
  {
    // make 'em learn...
    do_printUsage (std::string (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step2: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                            log_file,                   // logfile
                                            false,                      // log to syslog ?
                                            false,                      // trace messages ?
                                            trace_information,          // debug messages ?
                                            NULL))                      // logger
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

  // step3: handle specific program modes
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
  // step4: do work
  do_work (schema_repository,
           magic_dictionary_filename,
           item_dictionary_filename,
           graphics_dictionary_filename,
           graphics_directory,
           RPG_GRAPHICS_DEF_CACHESIZE,
           num_players,
           output_filename,
           random);
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
}
