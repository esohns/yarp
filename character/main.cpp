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

#include <rpg_chance_dice.h>
#include <rpg_chance_dice_common_tools.h>
#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

#include "rpg_character_dictionary.h"
#include "rpg_character_player.h"
#include "rpg_character_common_tools.h"
#include "rpg_character_skills_common_tools.h"
#include "rpg_character_monster_common_tools.h"

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <iostream>
#include <iomanip>
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
  std::cout << ACE_TEXT("-f [VALUE] : filename (*.xml)")
            << std::endl;
  std::cout << ACE_TEXT("-t         : trace information")
            << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit")
            << std::endl;
} // end print_usage

const bool process_arguments(const int argc_in,
                             ACE_TCHAR* argv_in[], // cannot be const...
                             std::string& filename_out,
                             bool& traceInformation_out,
                             bool& printVersionAndExit_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("f:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'f':
      {
        filename_out = argumentParser.opt_arg();

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

const bool print_skills_table(RPG_Character_Skills_t& skills_in)
{
  ACE_TRACE(ACE_TEXT("::print_skills_table"));

  RPG_Character_SkillsIterator_t skills_iterator = skills_in.end();

  unsigned int skills_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  RPG_Character_Skills_Common_Tools::RPG_Character_SkillToStringTableIterator_t iterator = RPG_Character_Skills_Common_Tools::mySkillToStringTable.begin();
  do
  {
    for (unsigned int i = 0;
         i < skills_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator == RPG_Character_Skills_Common_Tools::mySkillToStringTable.end())
      {
        break;
      } // end IF

      skills_iterator = skills_in.find(iterator->first);

//       std::cout.setf(ios::right);
      std::cout << ACE_TEXT("[") << std::setw(2) << std::right << index << ACE_TEXT("]: ") << std::setw(20) << std::left << iterator->second.c_str() << ACE_TEXT(": ");
      if (skills_iterator != skills_in.end())
      {
        std::cout << std::setw(3) << std::right << ACE_static_cast(unsigned int, skills_iterator->second) << ACE_TEXT(" ");
      } // end IF
      else
      {
        std::cout << ACE_TEXT("nil ");
      } // end ELSE
//       std::cout.unsetf(ios::right);
    } // end FOR

    std::cout << std::endl;
  } while (iterator != RPG_Character_Skills_Common_Tools::mySkillToStringTable.end());

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
  iterator = RPG_Character_Skills_Common_Tools::mySkillToStringTable.begin();
  std::advance(iterator, choice);
  skills_iterator = skills_in.find(iterator->first);
  if (skills_iterator != skills_in.end())
  {
    (skills_iterator->second)++;
  } // end IF
  else
  {
    skills_in.insert(std::make_pair(iterator->first, ACE_static_cast(unsigned char, 1)));
  } // end ELSE

  return true;
}

const bool print_feats_table(const RPG_Character_SubClass& subClass_in,
                             const RPG_Character_Attributes& attributes_in,
                             const RPG_Character_Skills_t& skills_in,
                             const RPG_Character_Abilities_t& abilities_in,
                             RPG_Character_Feats_t& feats_inout)
{
  ACE_TRACE(ACE_TEXT("::print_feats_table"));

  RPG_Character_FeatsIterator_t feats_iterator = feats_inout.end();

  unsigned int feats_per_line = 4;
  unsigned int index = 1;
  unsigned int choice = 0;
  RPG_Character_Skills_Common_Tools::RPG_Character_FeatToStringTableIterator_t iterator = RPG_Character_Skills_Common_Tools::myFeatToStringTable.begin();
  do
  {
    for (unsigned int i = 0;
         i < feats_per_line;
         i++, iterator++, index++)
    {
      // finished ?
      if (iterator == RPG_Character_Skills_Common_Tools::myFeatToStringTable.end())
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
        std::cout << ACE_TEXT("[") << std::setw(2) << std::right << index << ACE_TEXT("]: ") << std::setw(20) << std::left << iterator->second.c_str();
//         std::cout.unsetf(ios::right);
      } // end IF
    } // end FOR

    std::cout << std::endl;
  } while (iterator != RPG_Character_Skills_Common_Tools::myFeatToStringTable.end());

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
  iterator = RPG_Character_Skills_Common_Tools::myFeatToStringTable.begin();
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

void do_work(const std::string filename_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step1a: init randomization
  RPG_Chance_Dice::init();

  // step1b: init string conversion facilities
  RPG_Chance_Dice_Common_Tools::initStringConversionTables();
  RPG_Item_Common_Tools::initStringConversionTables();
  RPG_Character_Common_Tools::initStringConversionTables();
  RPG_Character_Skills_Common_Tools::init();

  // step1c: init item dictionary
  try
  {
    RPG_ITEM_DICTIONARY_SINGLETON::instance()->initItemDictionary(filename_in);
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Item_Dictionary::initCharacterDictionary, returning\n")));

    return;
  }

  // step2: generate/init new player character
  std::string name;
  std::cout << ACE_TEXT("type player name: ");
  std::cin >> name;

  RPG_Character_Gender gender = GENDER_NONE;
  char c = 'm';
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

  RPG_Character_Race race = RACE_BASE;
  c = 'f';
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
  } while (race == RACE_BASE);

  RPG_Character_Class player_class;
  player_class.metaClass = METACLASS_BASE;
  player_class.subClass = SUBCLASS_BASE;
  c = 'f';
  do
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
  } while ((player_class.metaClass == METACLASS_BASE) &&
           (player_class.subClass == SUBCLASS_BASE));

  RPG_Character_Alignment alignment;
  alignment.civicAlignment = ALIGNMENTCIVIC_INVALID;
  alignment.ethicAlignment = ALIGNMENTETHIC_INVALID;
  c = 'f';
  do
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
        break;
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
        break;
      }
    } // end SWITCH
  } while ((alignment.civicAlignment == ALIGNMENTCIVIC_INVALID) ||
           (alignment.ethicAlignment == ALIGNMENTETHIC_INVALID));

  RPG_Character_Attributes attributes;
  unsigned char* p = NULL;
  RPG_Chance_DiceRoll roll;
  roll.numDice = 2;
  roll.typeDice = D_10;
  roll.modifier = -2; // add +1 if result is 0 --> stats interval 1-18
  RPG_Chance_DiceRollResult_t result;
  c = 'n';
  do
  {
    p = &(attributes.strength);

    // make sure the result is somewhat balanced...
    // *IMPORTANT NOTE*: INT must be > 2 (smaller values are reserved for animals...)
    int sum = 0;
    do
    {
      result.clear();
      RPG_Chance_Dice::simulateDiceRoll(roll,
                                        6,
                                        result);
      sum = result[0] + result[1] + result[2] + result[3] + result[4] + result[5];
    } while ((sum <= 54) || (*(std::min_element(result.begin(), result.end())) <= 9));

    for (int i = 0;
         i < 6;
         i++, p++)
    {
      *p = (result[i] == 0 ? 1 : result[i]);
    } // end FOR

    std::cout << ACE_TEXT("base attributes: ") << std::endl;
    std::cout << ACE_TEXT("strength: ") << ACE_static_cast(int, attributes.strength) << std::endl;
    std::cout << ACE_TEXT("dexterity: ") << ACE_static_cast(int, attributes.dexterity) << std::endl;
    std::cout << ACE_TEXT("constitution: ") << ACE_static_cast(int, attributes.constitution) << std::endl;
    std::cout << ACE_TEXT("intelligence: ") << ACE_static_cast(int, attributes.intelligence) << std::endl;
    std::cout << ACE_TEXT("wisdom: ") << ACE_static_cast(int, attributes.wisdom) << std::endl;
    std::cout << ACE_TEXT("charisma: ") << ACE_static_cast(int, attributes.charisma) << std::endl;
    std::cout << ACE_TEXT("OK ? (y/n): ");
    std::cin >> c;
  } while (c == 'n');

  RPG_Character_Skills_t skills;
  short int INTmodifier = RPG_Character_Common_Tools::getAttributeAbilityModifier(attributes.intelligence);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("INT modifier (attribute value: %d) is: %d...\n"),
             attributes.intelligence,
             INTmodifier));

  unsigned int initialSkillPoints = 0;
  RPG_Character_Skills_Common_Tools::getSkillPoints(player_class.subClass,
                                                    INTmodifier,
                                                    initialSkillPoints);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initial skill points for subClass \"%s\" (INT modifier: %d) is: %d...\n"),
             RPG_Character_Common_Tools::subClassToString(player_class.subClass).c_str(),
             INTmodifier,
             initialSkillPoints));

  do
  {
    // header line
    std::cout << ACE_TEXT("remaining skill points: ") << initialSkillPoints << std::endl;
    std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

    if (print_skills_table(skills))
    {
      initialSkillPoints--;
    } // end IF
  } while (initialSkillPoints);

  RPG_Character_Feats_t feats;
  unsigned int initialFeats = 0;
  RPG_Character_Abilities_t abilities;
  RPG_Character_Skills_Common_Tools::getNumFeatsAbilities(race,
                                                          player_class.subClass,
                                                          1,
                                                          feats,
                                                          initialFeats,
                                                          abilities);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initial feats: %d...\n"),
             initialFeats));
  do
  {
    // header line
    std::cout << ACE_TEXT("remaining feats: ") << initialFeats << std::endl;
    std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

    if (print_feats_table(player_class.subClass,
                          attributes,
                          skills,
                          abilities,
                          feats))
    {
      initialFeats--;
    } // end IF
  } while (initialFeats);

  roll.numDice = 1;
  roll.typeDice = RPG_Character_Common_Tools::getHitDie(player_class.subClass);
  roll.modifier = 0;
  result.clear();
  RPG_Chance_Dice::simulateDiceRoll(roll,
                                    1,
                                    result);
  unsigned short int hitpoints = result[0];
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initial hit points (hit die: \"%s\"): %d...\n"),
             RPG_Chance_Dice_Common_Tools::diceTypeToString(roll.typeDice).c_str(),
             hitpoints));

  // *TODO*: step3: choose appropriate initial set of items
  RPG_Item_List_t items;
  RPG_Item_Armor* armor = NULL;
  RPG_Item_Armor* shield = NULL;
  RPG_Item_Weapon* weapon = NULL;
  RPG_Item_Weapon* bow = NULL;

  switch (player_class.subClass)
  {
    case SUBCLASS_FIGHTER:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      armor  = new RPG_Item_Armor(ARMOR_MAIL_SPLINT);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_HEAVY_WOODEN);

      items.push_back(weapon->getID());
      items.push_back(armor->getID());
      items.push_back(shield->getID());

      break;
    }
    case SUBCLASS_PALADIN:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      armor  = new RPG_Item_Armor(ARMOR_PLATE_FULL);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_HEAVY_STEEL);

      items.push_back(weapon->getID());
      items.push_back(armor->getID());
      items.push_back(shield->getID());

      break;
    }
    case SUBCLASS_RANGER:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      bow    = new RPG_Item_Weapon(RANGED_WEAPON_BOW_LONG);
      armor  = new RPG_Item_Armor(ARMOR_HIDE);

      items.push_back(weapon->getID());
      items.push_back(bow->getID());
      items.push_back(armor->getID());

      break;
    }
    case SUBCLASS_BARBARIAN:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      armor  = new RPG_Item_Armor(ARMOR_HIDE);

      items.push_back(weapon->getID());
      items.push_back(armor->getID());

      break;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    {
      weapon = new RPG_Item_Weapon(TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);

      items.push_back(weapon->getID());

      break;
    }
    case SUBCLASS_CLERIC:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_MACE_HEAVY);
      armor  = new RPG_Item_Armor(ARMOR_MAIL_CHAIN);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_HEAVY_WOODEN);

      items.push_back(weapon->getID());
      items.push_back(armor->getID());
      items.push_back(shield->getID());

      break;
    }
    case SUBCLASS_DRUID:
    {
      weapon = new RPG_Item_Weapon(LIGHT_MELEE_WEAPON_SICKLE);
      armor  = new RPG_Item_Armor(ARMOR_HIDE);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_LIGHT_WOODEN);

      items.push_back(weapon->getID());
      items.push_back(armor->getID());
      items.push_back(shield->getID());

      break;
    }
    case SUBCLASS_MONK:
    {
      weapon = new RPG_Item_Weapon(TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);

      items.push_back(weapon->getID());

      break;
    }
    case SUBCLASS_THIEF:
    case SUBCLASS_BARD:
    {
      weapon = new RPG_Item_Weapon(LIGHT_MELEE_WEAPON_SWORD_SHORT);
      armor  = new RPG_Item_Armor(ARMOR_LEATHER);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_LIGHT_STEEL);

      items.push_back(weapon->getID());
      items.push_back(armor->getID());
      items.push_back(shield->getID());

      break;
    }
    default:
    {
      std::string subClass_string = RPG_Character_Common_Tools::subClassToString(player_class.subClass);
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid class \"%s\", aborting\n"),
                 subClass_string.c_str()));

      return;
    }
  } // end SWITCH

  // step4: instantiate player character
  RPG_Character_Player player(name,
                              gender,
                              race,
                              player_class,
                              alignment,
                              attributes,
                              skills,
                              feats,
                              abilities,
                              0,
                              hitpoints,
                              0,
                              items);
  // debug info
  player.dump();

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
  std::string filename;
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          filename,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage();

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (filename.empty())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid (XML) filename \"%s\", aborting\n"),
               filename.c_str()));

    // make 'em learn...
    print_usage();

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
    do_printVersion();

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(filename);

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
