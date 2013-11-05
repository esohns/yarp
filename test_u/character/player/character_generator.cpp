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
#include <rpg_config.h>
#endif

#include <rpg_client_defines.h>

#include <rpg_engine_defines.h>
#include <rpg_engine_common.h>
#include <rpg_engine_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_incl.h>
#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_player_defines.h>
#include <rpg_player.h>
#include <rpg_player_common_tools.h>

#include <rpg_character_alignmentcivic.h>
#include <rpg_character_alignmentethic.h>
#include <rpg_character_alignment.h>
#include <rpg_character_offhand.h>
#include <rpg_character_common_tools.h>
#include <rpg_character_class_common_tools.h>
#include <rpg_character_skills_common_tools.h>

#include <rpg_item_defines.h>
#include <rpg_item_base.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

#include <rpg_magic_defines.h>
#include <rpg_magic_dictionary.h>
#include <rpg_magic_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_subclass.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_dietype.h>
#include <rpg_dice_common_tools.h>

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

#define CHARACTER_GENERATOR_DEF_GENERATE_ENTITY false
#define CHARACTER_GENERATOR_DEF_GENERATE_PARTY  0
#define CHARACTER_GENERATOR_DEF_RANDOM          false

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string base_path;
#ifdef BASEDIR
  base_path = ACE_TEXT_ALWAYS_CHAR(BASEDIR);
#endif
  std::string config_path = RPG_Common_File_Tools::getDataDirectory(base_path,
                                                                    true);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-e        : generate entity") << ACE_TEXT(" [") << CHARACTER_GENERATOR_DEF_GENERATE_ENTITY << ACE_TEXT("]") << std::endl;
  std::string path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("graphics"); // directory
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE] : graphics dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("item"); // directory
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE] : item dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("magic"); // directory
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-m [FILE] : magic dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-n [VALUE]: generate (party of) #players") << ACE_TEXT(" [") << CHARACTER_GENERATOR_DEF_GENERATE_PARTY << ACE_TEXT("; 0:off]") << std::endl;
  std::cout << ACE_TEXT("-r        : random (non-interactive)") << ACE_TEXT(" [") << CHARACTER_GENERATOR_DEF_RANDOM << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  bool& generateEntity_out,
                  std::string& itemDictionary_out,
                  std::string& magicDictionary_out,
                  std::string& graphicsDictionary_out,
                  unsigned int& generateParty_out,
                  bool& random_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  std::string base_path;
#ifdef BASEDIR
  base_path = ACE_TEXT_ALWAYS_CHAR(BASEDIR);
#endif
  std::string config_path = RPG_Common_File_Tools::getDataDirectory(base_path,
                                                                    true);

  // init results
  generateEntity_out = CHARACTER_GENERATOR_DEF_GENERATE_ENTITY;

  itemDictionary_out = config_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  magicDictionary_out = config_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR("magic");
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);

  graphicsDictionary_out = config_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  generateParty_out = CHARACTER_GENERATOR_DEF_GENERATE_PARTY;
  random_out = CHARACTER_GENERATOR_DEF_RANDOM;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("eg:i:m:n:rtv"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'e':
      {
        generateEntity_out = true;

        break;
      }
      case 'g':
      {
        graphicsDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'i':
      {
        itemDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'm':
      {
        magicDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'n':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> generateParty_out;

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

bool
print_skills_table(RPG_Character_Skills_t& skills_in)
{
  RPG_TRACE(ACE_TEXT("::print_skills_table"));

  RPG_Character_SkillsIterator_t skills_iterator = skills_in.end();

  unsigned int skills_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  RPG_Common_SkillToStringTableIterator_t iterator = RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.begin();
  do
  {
    for (unsigned int i = 0;
         i < skills_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator == RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.end())
      {
        break;
      } // end IF

      skills_iterator = skills_in.find(iterator->first);

//       std::cout.setf(ios::right);
      std::cout << ACE_TEXT("[") << std::setw(2) << std::right << index << ACE_TEXT("]: ") << std::setw(20) << std::left << iterator->second.c_str() << ACE_TEXT(": ");
      if (skills_iterator != skills_in.end())
      {
        std::cout << std::setw(3) << std::right << static_cast<unsigned int>(skills_iterator->second) << ACE_TEXT(" ");
      } // end IF
      else
      {
        std::cout << ACE_TEXT("nil ");
      } // end ELSE
//       std::cout.unsetf(ios::right);
    } // end FOR

    std::cout << std::endl;
  } while (iterator != RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.end());

  index--;

  std::cout << ACE_TEXT("enter index: ");
  std::cin >> choice;
  // sanity check
  if ((choice > index) || (choice < 1))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid index %d (max: %d), try again\n"),
               choice,
               index));

    // clean input buffer
    if (!std::cin)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), ACE_TEXT_ALWAYS_CHAR('\n'));
    } // end IF

    return false;
  } // end IF

  // increase skill rank
  choice -= 1;
  iterator = RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.begin();
  ACE_ASSERT(RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable.size() >= choice);
  std::advance(iterator, choice);
  skills_iterator = skills_in.find(iterator->first);
  if (skills_iterator != skills_in.end())
  {
    (skills_iterator->second)++;
  } // end IF
  else
  {
    skills_in.insert(std::make_pair(iterator->first, static_cast<unsigned char>(1)));
  } // end ELSE

  return true;
}

bool
print_feats_table(const RPG_Common_SubClass& subClass_in,
                  const RPG_Character_Attributes& attributes_in,
                  const RPG_Character_Skills_t& skills_in,
                  const RPG_Character_Abilities_t& abilities_in,
                  RPG_Character_Feats_t& feats_inout)
{
  RPG_TRACE(ACE_TEXT("::print_feats_table"));

  RPG_Character_FeatsIterator_t feats_iterator = feats_inout.end();

  unsigned int feats_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  RPG_Character_FeatToStringTableIterator_t iterator = RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.begin();
  do
  {
    for (unsigned int i = 0;
         i < feats_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator == RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.end())
      {
        break;
      } // end IF

      feats_iterator = feats_inout.find(iterator->first);
      if ((feats_iterator == feats_inout.end()) &&
          (RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites(iterator->first,
                                                                     subClass_in,
                                                                     1,
                                                                     attributes_in,
                                                                     skills_in,
                                                                     feats_inout,
                                                                     abilities_in)))
      {
//         std::cout.setf(ios::right);
        std::cout << ACE_TEXT("[") << std::setw(2) << std::right << index << ACE_TEXT("]: ") << std::setw(20) << std::left << iterator->second.c_str() << ACE_TEXT(" ");
//         std::cout.unsetf(ios::right);
      } // end IF
    } // end FOR

    std::cout << std::endl;
  } while (iterator != RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.end());

  index--;

  std::cout << ACE_TEXT("enter index: ");
  std::cin >> choice;
  // sanity check
  if ((choice > index) || (choice < 1))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid index %d (max: %d), try again\n"),
               choice,
               index));

    // clean input buffer
    if (!std::cin)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), ACE_TEXT_ALWAYS_CHAR('\n'));
    } // end IF

    return false;
  } // end IF

  // (try to) append chosen feat
  choice -= 1;
  iterator = RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.begin();
  ACE_ASSERT(RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable.size() >= choice);
  std::advance(iterator, choice);

  feats_iterator = feats_inout.find(iterator->first);
  if ((feats_iterator != feats_inout.end()) ||
      (!RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites(iterator->first,
                                                                  subClass_in,
                                                                  1,
                                                                  attributes_in,
                                                                  skills_in,
                                                                  feats_inout,
                                                                  abilities_in)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid index %d, try again\n"),
               choice));

    return false;
  } // end IF

  feats_inout.insert(iterator->first);

  return true;
}

bool
print_spells_table(const RPG_Magic_SpellTypes_t& spells_in,
                   RPG_Magic_SpellType& spell_inout)
{
  RPG_TRACE(ACE_TEXT("::print_spells_table"));

  // init return value
  spell_inout = RPG_MAGIC_SPELLTYPE_INVALID;

  RPG_Magic_SpellTypesIterator_t iterator = spells_in.begin();
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
      if (iterator == spells_in.end())
      {
        break;
      } // end IF

//         std::cout.setf(ios::right);
      std::cout << ACE_TEXT("[") << std::setw(2) << std::right << index << ACE_TEXT("]: ") << std::setw(20) << std::left << RPG_Magic_Common_Tools::spellToName(*iterator).c_str();
//         std::cout.unsetf(ios::right);
    } // end FOR

    std::cout << std::endl;
  } while (iterator != spells_in.end());

  index--;

  std::cout << ACE_TEXT("enter index: ");
  std::cin >> choice;
  // sanity check
  if ((choice > index) || (choice < 1))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid index %d (max: %d), try again\n"),
               choice,
               index));

    // clean input buffer
    if (!std::cin)
    {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), ACE_TEXT_ALWAYS_CHAR('\n'));
    } // end IF

    return false;
  } // end IF

  // append chosen spell
  choice -= 1;
  iterator = spells_in.begin();
  ACE_ASSERT(spells_in.size() >= choice);
  std::advance(iterator, choice);

  // select chosen spell type
  spell_inout = *iterator;

  return true;
}

RPG_Player*
generate_player()
{
  RPG_TRACE(ACE_TEXT("::generate_player"));

  // step1: name
  std::string name;
  std::cout << ACE_TEXT("name: ");
  std::cin >> name;

  // step2: gender
  RPG_Character_Gender gender = GENDER_NONE;
  char c = ' ';
  do
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
  } while (gender == GENDER_NONE);

  // step3: race
  RPG_Character_Race race = RACE_NONE;
  c = ' ';
  do
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
  } while (race == RACE_NONE);
  RPG_Character_Race_t playerRace(0);
  playerRace.flip(race - 1);

  // step4: class(es)
  RPG_Character_Class playerClass;
  playerClass.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  RPG_Common_SubClass playerSubClass = RPG_COMMON_SUBCLASS_INVALID;
  c = ' ';
  do
  {
    std::cout << ACE_TEXT("class (f/m/c/t): ");
    std::cin >> c;
    switch (c)
    {
      case 'f':
      {
        playerSubClass = SUBCLASS_FIGHTER;
        break;
      }
      case 'm':
      {
        playerSubClass = SUBCLASS_WIZARD;
        break;
      }
      case 'c':
      {
        playerSubClass = SUBCLASS_CLERIC;
        break;
      }
      case 't':
      {
        playerSubClass = SUBCLASS_THIEF;
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
  } while (playerSubClass == RPG_COMMON_SUBCLASS_INVALID);
  playerClass.subClasses.insert(playerSubClass);
  playerClass.metaClass = RPG_Character_Class_Common_Tools::subClassToMetaClass(playerSubClass);

  // step5: alignment
  RPG_Character_Alignment alignment;
  alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  c = ' ';
  do
  {
    std::cout << ACE_TEXT("alignment - civic (l/n/c): ");
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
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized (alignment - civic) option \"%c\", try again\n"),
                   c));
        break;
      }
    } // end SWITCH

    std::cout << ACE_TEXT("alignment - ethic (g/n/e): ");
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
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized (alignment - ethic) option \"%c\", try again\n"),
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
      result.clear();
      RPG_Dice::simulateRoll(roll,
                             6,
                             result);
      sum = std::accumulate(result.begin(),
                            result.end(),
                            0);
    } while ((sum <= RPG_PLAYER_ATTR_MIN_SUM) ||
             (*(std::min_element(result.begin(),
                                 result.end())) <= 9) ||
             (result[3] < 3)); // Note: this is already covered by the last case...
    for (int i = 0;
         i < 6;
         i++, p++)
    {
      // add +1 if result is 0 --> stats interval 1-18
      *p = (result[i] == 0 ? 1 : result[i]);
    } // end FOR

    std::cout << ACE_TEXT("base attributes: ") << std::endl;
    std::cout << ACE_TEXT("strength: ") << static_cast<int>(attributes.strength) << std::endl;
    std::cout << ACE_TEXT("dexterity: ") << static_cast<int>(attributes.dexterity) << std::endl;
    std::cout << ACE_TEXT("constitution: ") << static_cast<int>(attributes.constitution) << std::endl;
    std::cout << ACE_TEXT("intelligence: ") << static_cast<int>(attributes.intelligence) << std::endl;
    std::cout << ACE_TEXT("wisdom: ") << static_cast<int>(attributes.wisdom) << std::endl;
    std::cout << ACE_TEXT("charisma: ") << static_cast<int>(attributes.charisma) << std::endl;
    std::cout << ACE_TEXT("[sum: ") << sum << ACE_TEXT("] --> OK ? (y/n): ");
    std::cin >> c;
  } while (c == 'n');

  // step7: skills
  RPG_Character_Skills_t skills;
  short int INTmodifier = RPG_Character_Common_Tools::getAttributeAbilityModifier(attributes.intelligence);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("INT modifier (attribute value: %d) is: %d...\n"),
             attributes.intelligence,
             INTmodifier));

  unsigned int initialSkillPoints = 0;
  RPG_Character_Skills_Common_Tools::getSkillPoints(playerSubClass,
                                                    INTmodifier,
                                                    initialSkillPoints);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initial skill points for subClass \"%s\" (INT modifier: %d) is: %d...\n"),
             RPG_Common_SubClassHelper::RPG_Common_SubClassToString(playerSubClass).c_str(),
             INTmodifier,
             initialSkillPoints));

  do
  {
    // header line
    std::cout << ACE_TEXT("remaining skill points: ") << initialSkillPoints << std::endl;
    std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

    if (print_skills_table(skills))
      initialSkillPoints--;
  } while (initialSkillPoints);

  // step8: feats & abilities
  RPG_Character_Feats_t feats;
  unsigned int initialFeats = 0;
  RPG_Character_Abilities_t abilities;
  RPG_Character_Skills_Common_Tools::getNumFeatsAbilities(race,           // race
                                                          playerSubClass, // subclass
                                                          1,              // current level
                                                          feats,          // (base) feats
                                                          initialFeats,   // initial feats (level 1)
                                                          abilities);     // (base) abilities
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initial feat(s): %d...\n"),
             initialFeats));
  do
  {
    // header line
    std::cout << ACE_TEXT("remaining feat(s): ") << initialFeats << std::endl;
    std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

    if (print_feats_table(playerSubClass,
                          attributes,
                          skills,
                          abilities,
                          feats))
      initialFeats--;
  } while (initialFeats);

  // step9: off-hand
  RPG_Character_OffHand offHand = OFFHAND_LEFT;
  roll.numDice = 1;
  roll.typeDice = D_100;
  roll.modifier = 0;
  // *TODO*: 10% (?) of people are "lefties"...
  result.clear();
  RPG_Dice::simulateRoll(roll,
                         1,
                         result);
  if (result.front() <= 10)
    offHand = OFFHAND_RIGHT;

  // step10: HP
  unsigned short int hitPoints = 0;
  roll.numDice = 1;
  roll.typeDice = RPG_Character_Common_Tools::getHitDie(playerSubClass);
  roll.modifier = 0;
  result.clear();
//   RPG_Dice::simulateRoll(roll,
//                          1,
//                          result);
//   hitPoints = result[0];
  // *NOTE*: players start with maxed HP...
  hitPoints = RPG_Character_Common_Tools::getHitDie(playerSubClass);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initial hit points (hit die: \"%s\"): %d...\n"),
             RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString(roll.typeDice).c_str(),
             hitPoints));

  // step11: spells
  unsigned char numKnownSpells = 0;
  unsigned char numSpells = 0;
  RPG_Magic_SpellTypes_t knownSpells;
  RPG_Magic_Spells_t spells;
  RPG_Magic_SpellTypes_t available;
  RPG_Magic_SpellType chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
  int numChosen = 0;
  RPG_Magic_SpellsIterator_t available_iterator;
  RPG_Magic_CasterClassUnion casterClass;
  casterClass.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  for (RPG_Character_SubClassesIterator_t iterator = playerClass.subClasses.begin();
       iterator != playerClass.subClasses.end();
       iterator++)
  {
    if (!RPG_Magic_Common_Tools::isCasterClass(*iterator))
      continue;

    for (unsigned char i = 0;
         i <= RPG_COMMON_MAX_SPELL_LEVEL;
         i++)
    {
      numKnownSpells = 0;
      numSpells = 0;

      // step1: get list of available spells
      casterClass.subclass = *iterator;
      available = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                        i);

      // *NOTE*: divine casters know ALL spells from the levels they can cast
      if (!RPG_Magic_Common_Tools::isDivineCasterClass(*iterator))
      {
        // step2: compute # known spells
        numKnownSpells = RPG_Magic_Common_Tools::getNumKnownSpells(*iterator,
                                                                   1,
                                                                   i);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("number of initial known spells (lvl %d) for subClass \"%s\" is: %d...\n"),
                   i,
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str(),
                   numKnownSpells));

        // step3: choose known spells
        numChosen = 0;
        while (numChosen < numKnownSpells)
        {
          chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
          // header line
          std::cout << ACE_TEXT("remaining spell(s): ") << (numKnownSpells - numChosen) << std::endl;
          std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

          if (print_spells_table(available,
                                 chosen_spell))
          {
            knownSpells.insert(chosen_spell);
            available.erase(chosen_spell);
            numChosen++;
          } // end IF
        } // end WHILE
      } // end IF

      // step4: compute # prepared spells
      numSpells = RPG_Magic_Common_Tools::getNumSpells(*iterator,
                                                       1,
                                                       i);

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("number of initial memorized/prepared spells (lvl %d) for subClass \"%s\" is: %d...\n"),
                 i,
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str(),
                 numSpells));

      // step5: choose prepared spells
      numChosen = 0;
      while (numChosen < numSpells)
      {
        chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
        // header line
        std::cout << ACE_TEXT("remaining spell(s): ") << (numSpells - numChosen) << std::endl;
        std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

        if (print_spells_table((RPG_Magic_Common_Tools::isDivineCasterClass(*iterator) ? available
                                                                                       : knownSpells),
                               chosen_spell))
        {
          spells.push_back(chosen_spell);
          numChosen++;
        } // end IF
      } // end WHILE
    } // end FOR
  } // end FOR

  // step12: initialize condition
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);

  // step13: choose (appropriate) initial set of items
  RPG_Item_List_t items = RPG_Engine_Common_Tools::generateStandardItems(playerSubClass);

  // instantiate player character
  RPG_Player* player_p = NULL;
  try
  {
    player_p = new RPG_Player(// base attributes
                              name,
                              gender,
                              playerRace,
                              playerClass,
                              alignment,
                              attributes,
                              skills,
                              feats,
                              abilities,
                              offHand,
                              hitPoints,
                              knownSpells,
                              // current status
                              condition,
                              hitPoints,
                              0,
                              RPG_PLAYER_START_MONEY,
                              spells,
                              items);
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Character_Player(): exception occurred: \"%s\", aborting\n"),
               exception.what()));

    return player_p;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Character_Player(): exception occurred, aborting\n")));

    return player_p;
  }
  ACE_ASSERT(player_p);

  return player_p;
}

RPG_Engine_Entity
generate_entity(const RPG_Player& player_in,
                const RPG_Graphics_Sprite& sprite_in)
{
  RPG_TRACE(ACE_TEXT("::generate_entity"));

  RPG_Engine_Entity result;
  result.character = &const_cast<RPG_Player&>(player_in);
  result.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                   std::numeric_limits<unsigned int>::max());
  result.sprite = sprite_in;
  result.is_spawned = false;

  return result;
}

void
do_work(const bool& generateEntity_in,
        const std::string& schemaDirectory_in,
        const std::string& magicDictionaryFilename_in,
        const std::string& itemDictionaryFilename_in,
        const std::string& graphicsDictionary_in,
        const std::string& graphicsDirectory_in,
        const unsigned int& graphicsCacheSize_in,
        const unsigned int& numPartyMembers_in,
        const bool& random_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1a: init randomization
  RPG_Dice::init();

  // step1b: init string conversion facilities
  RPG_Engine_Common_Tools::init(schemaDirectory_in,
                                magicDictionaryFilename_in,
                                itemDictionaryFilename_in,
                                std::string());
  RPG_Graphics_Common_Tools::init(graphicsDirectory_in,
                                  graphicsCacheSize_in,
                                  false); // DON'T init SDL bits...

  // init graphics dictionary
  try
  {
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(graphicsDictionary_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_Dictionary::init, returning\n")));

    return;
  }

  // step2: display menu options
  bool done = false;
  char c = ' ';
  unsigned int numPlayers = 0;
  RPG_Player* player_p = NULL;
  RPG_Engine_Entity entity;
  do
  {
    if (player_p)
    {
      delete player_p;
      player_p = NULL;
    } // end IF

    // step1: generate new player character
    player_p = (random_in ? RPG_Player::create()
                          : ::generate_player());
    ACE_ASSERT(player_p);
    player_p->dump();

    if (numPartyMembers_in == 0)
    {
      // step2: display menu options
      c = ' ';
      do
      {
        std::cout << ACE_TEXT("menu options (rETRY/sAVE/qUIT): ");
        std::cin >> c;
        switch (c)
        {
          case 'r':
            break;
          case 's':
          {
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
            std::string path = ACE_TEXT(RPG_PLAYER_DEF_ENTITY_REPOSITORY);
#else
            std::string path = ACE_OS::getenv(ACE_TEXT(RPG_PLAYER_DEF_ENTITY_REPOSITORY));
#endif
            path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
            path += player_p->getName();
            path += (generateEntity_in ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENTITY_FILE_EXT)
                                       : ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT));

            // sanity check
            if (RPG_Common_File_Tools::isReadable(path))
            {
              bool proceed = false;
              c = ' ';
              do
              {
                std::cout << ACE_TEXT("file \"")
					                << path
						              << ACE_TEXT("\" exists, overwrite ? (y/n): ");
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
                    ACE_DEBUG((LM_ERROR,
                               ACE_TEXT("unrecognized option \"%c\", try again\n"),
                               c));
                    break;
                  }
                } // end SWITCH
              } while ((c != 'y') &&
                       (c != 'n'));

              if (!proceed)
                break;
            } // end IF

            if (generateEntity_in)
            {
              entity = generate_entity(*player_p,
                                       RPG_ENGINE_DEF_ENTITY_SPRITE);
              if (!RPG_Engine_Common_Tools::saveEntity(entity,
                                                       path))
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("failed to RPG_Engine_Common_Tools::saveEntity(\"%s\"), continuing\n"),
                           path.c_str()));
            } // end IF
            else
            {
              if (!player_p->save(path))
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("failed to RPG_Character_Player::save(\"%s\"), continuing\n"),
                           path.c_str()));
            } // end ELSE

            break;
          }
          case 'q':
          {
            done = true;

            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("unrecognized option \"%c\", try again\n"),
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
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      std::string path = ACE_TEXT(RPG_PLAYER_DEF_ENTITY_REPOSITORY);
#else
      std::string path = ACE_OS::getenv(ACE_TEXT(RPG_PLAYER_DEF_ENTITY_REPOSITORY));
#endif
      path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      path += player_p->getName();
      path += (generateEntity_in ? ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENTITY_FILE_EXT)
                                 : ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT));

      if (generateEntity_in)
      {
        entity = generate_entity(*player_p,
                                 RPG_ENGINE_DEF_ENTITY_SPRITE);
        if (!RPG_Engine_Common_Tools::saveEntity(entity,
                                                 path))
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to RPG_Engine_Common_Tools::saveEntity(\"%s\"), continuing\n"),
                     path.c_str()));
        else
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("saved entity \"%s\" to file: \"%s\"\n"),
                     player_p->getName().c_str(),
                     path.c_str()));
      } // end IF
      else
      {
        if (!player_p->save(path))
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to RPG_Character_Player::save(\"%s\"), continuing\n"),
                     path.c_str()));
        else
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("saved player \"%s\" to file: \"%s\"\n"),
                     player_p->getName().c_str(),
                     path.c_str()));
      } // end ELSE

      numPlayers++;
      if (numPlayers == numPartyMembers_in)
        done = true;
    } // end ELSE
  } while (!done);

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
  std::string base_path;
#ifdef BASEDIR
  base_path = ACE_TEXT_ALWAYS_CHAR(BASEDIR);
#endif
  std::string config_path = RPG_Common_File_Tools::getDataDirectory(base_path,
                                                                    true);
  std::string base_data_path = RPG_Common_File_Tools::getDataDirectory(base_path,
                                                                       false);

  // init configuration
  bool generateEntity          = CHARACTER_GENERATOR_DEF_GENERATE_ENTITY;

  std::string itemDictionaryFilename = config_path;
  itemDictionaryFilename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  itemDictionaryFilename += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionaryFilename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionaryFilename += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  std::string magicDictionaryFilename = config_path;
  magicDictionaryFilename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  magicDictionaryFilename += ACE_TEXT_ALWAYS_CHAR("magic");
  magicDictionaryFilename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionaryFilename += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);

  std::string graphicsDictionaryFilename = config_path;
  graphicsDictionaryFilename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  graphicsDictionaryFilename += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionaryFilename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionaryFilename += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  std::string graphicsDirectory = base_data_path;
  graphicsDirectory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  graphicsDirectory += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDirectory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphicsDirectory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DATA_SUB);
#else
  graphicsDirectory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
#endif

  unsigned int numPartyMembers = CHARACTER_GENERATOR_DEF_GENERATE_PARTY;
  bool random                  = CHARACTER_GENERATOR_DEF_RANDOM;
  bool traceInformation        = false;
  bool printVersionAndExit     = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          generateEntity,
                          itemDictionaryFilename,
                          magicDictionaryFilename,
                          graphicsDictionaryFilename,
                          numPartyMembers,
                          random,
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
      !RPG_Common_File_Tools::isReadable(graphicsDictionaryFilename) ||
      !RPG_Common_File_Tools::isDirectory(graphicsDirectory))
  {
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
  do_work(generateEntity,
          config_path,
          magicDictionaryFilename,
          itemDictionaryFilename,
          graphicsDictionaryFilename,
          graphicsDirectory,
          RPG_CLIENT_DEF_GRAPHICS_CACHESIZE,
          numPartyMembers,
          random);

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
//              working_time_string.c_str()));

  return EXIT_SUCCESS;
} // end main
