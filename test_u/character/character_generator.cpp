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
#include <test_u-config.h>
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

#include <rpg_item_base.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

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

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-i [FILE] : item dictionary (*.xml)") << std::endl;
  std::cout << ACE_TEXT("-m [FILE] : magic dictionary (*.xml)") << std::endl;
  std::cout << ACE_TEXT("-n [VALUE]: generate (party of) #players") << std::endl;
  std::cout << ACE_TEXT("-r        : random (non-interactive)") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& itemDictionaryFilename_out,
                  std::string& magicDictionaryFilename_out,
                  unsigned int& generateParty_out,
                  bool& random_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  itemDictionaryFilename_out.clear();
  magicDictionaryFilename_out.clear();
  generateParty_out = 0;
  random_out = false;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("i:m:n:rtv"));

  int option = 0;
  std::stringstream converter;
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

const bool
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
        std::cout << std::setw(3) << std::right << ACE_static_cast(unsigned int, skills_iterator->second) << ACE_TEXT(" ");
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
    skills_in.insert(std::make_pair(iterator->first, ACE_static_cast(unsigned char, 1)));
  } // end ELSE

  return true;
}

const bool
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

const bool
print_spells_table(const RPG_Magic_Spells_t& spells_in,
                   RPG_Magic_SpellType& spell_inout)
{
  RPG_TRACE(ACE_TEXT("::print_spells_table"));

  // init return value
  spell_inout = RPG_MAGIC_SPELLTYPE_INVALID;

  RPG_Magic_SpellsIterator_t iterator = spells_in.begin();
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

RPG_Item_List_t
generate_standard_items(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("::generate_standard_items"));

  RPG_Item_List_t result;

  RPG_Item_Instance_Base* current = NULL;
  try
  {
    switch (subClass_in)
    {
      case SUBCLASS_FIGHTER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_MAIL_SPLINT);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_PALADIN:
      case SUBCLASS_WARLORD:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_PLATE_FULL);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_HEAVY_STEEL);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_RANGER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            RANGED_WEAPON_BOW_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_HIDE);
        ACE_ASSERT(current);
        result.insert(current->getID());

        // *TODO*: no arrows ?

        break;
      }
      case SUBCLASS_BARBARIAN:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_HIDE);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_WIZARD:
      case SUBCLASS_SORCERER:
      case SUBCLASS_WARLOCK:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_CLERIC:
      case SUBCLASS_AVENGER:
      case SUBCLASS_INVOKER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_MACE_HEAVY);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_MAIL_CHAIN);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_DRUID:
      case SUBCLASS_SHAMAN:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            LIGHT_MELEE_WEAPON_SICKLE);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_HIDE);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_LIGHT_WOODEN);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_MONK:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_THIEF:
      case SUBCLASS_BARD:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            LIGHT_MELEE_WEAPON_SWORD_SHORT);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_LEATHER);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_LIGHT_STEEL);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid subclass \"%s\", aborting\n"),
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

        break;
      }
    } // end SWITCH
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught std::bad_alloc, continuing\n")));
  }

  return result;
}

RPG_Character_Player
generate_player_character()
{
  RPG_TRACE(ACE_TEXT("::generate_player_character"));

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
    } while ((sum <= RPG_CHARACTER_PLAYER_ATTR_MIN_SUM) ||
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
    std::cout << ACE_TEXT("strength: ") << ACE_static_cast(int, attributes.strength) << std::endl;
    std::cout << ACE_TEXT("dexterity: ") << ACE_static_cast(int, attributes.dexterity) << std::endl;
    std::cout << ACE_TEXT("constitution: ") << ACE_static_cast(int, attributes.constitution) << std::endl;
    std::cout << ACE_TEXT("intelligence: ") << ACE_static_cast(int, attributes.intelligence) << std::endl;
    std::cout << ACE_TEXT("wisdom: ") << ACE_static_cast(int, attributes.wisdom) << std::endl;
    std::cout << ACE_TEXT("charisma: ") << ACE_static_cast(int, attributes.charisma) << std::endl;
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
  RPG_Magic_Spells_t knownSpells;
  RPG_Magic_SpellList_t spells;
  RPG_Magic_Spells_t available;
  RPG_Magic_SpellType chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
  int numChosen = 0;
  RPG_Magic_SpellsIterator_t available_iterator;
  RPG_Magic_CasterClassUnion casterClass;
  casterClass.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  for (RPG_Character_SubClassesIterator_t iterator = playerClass.subClasses.begin();
       iterator != playerClass.subClasses.end();
       iterator++)
  {
    for (unsigned char i = 0;
         i <= RPG_COMMON_MAX_SPELL_LEVEL;
         i++)
    {
      RPG_Magic_Common_Tools::getNumSpellsPerLevel(*iterator,
                                                   1,
                                                   i,
                                                   numSpells,
                                                   numKnownSpells);
      if ((numSpells == 0) &&
          (numKnownSpells == 0))
        continue;

      // get list of available spells
      casterClass.subclass = *iterator;
      available = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                        i);

      // only Bards and Sorcerers have a limited set of "known" spells to choose from
      if (RPG_Character_Common_Tools::isCasterClass(*iterator) &&
          ((*iterator == SUBCLASS_BARD) ||
          (*iterator == SUBCLASS_SORCERER)))
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("number of initial known spells (lvl %d) for subClass \"%s\" is: %d...\n"),
                   i,
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(playerSubClass).c_str(),
                   numKnownSpells));

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

      // ... other magic-users get to prepare/memorize a number of (available) spells
      // ... again, apart from the Bard/Sorcerer, who don't need to prepare any spells ahead of time
      if (RPG_Character_Common_Tools::isCasterClass(*iterator) &&
          ((*iterator != SUBCLASS_BARD) &&
          (*iterator != SUBCLASS_SORCERER)))
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("number of initial memorized/prepared spells (lvl %d) for subClass \"%s\" is: %d...\n"),
                   i,
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(playerSubClass).c_str(),
                   numSpells));

        numChosen = 0;
        while (numChosen < numSpells)
        {
          chosen_spell = RPG_MAGIC_SPELLTYPE_INVALID;
          // header line
          std::cout << ACE_TEXT("remaining spell(s): ") << (numSpells - numChosen) << std::endl;
          std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

          if (print_spells_table(available,
                                 chosen_spell))
          {
            spells.push_back(chosen_spell);
            numChosen++;
          } // end IF
        } // end WHILE
      } // end IF
    } // end FOR
  } // end FOR

  // step12: initialize condition
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);

  // step13: choose (appropriate) initial set of items
  RPG_Item_List_t items = generate_standard_items(playerSubClass);

  // instantiate player character
  RPG_Character_Player player(// base attributes
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
                              RPG_CHARACTER_PLAYER_START_MONEY,
                              spells,
                              items);

  return player;
}

RPG_Character_Player
generate_random_player_character()
{
  RPG_TRACE(ACE_TEXT("::generate_random_player_character"));

//   // init result
//   RPG_Character_Player player = RPG_Character_Player::dummy();
//
//   char c = ' ';
//   do
//   {
//     c = ' ';
//
//     // generate random player
//     player = RPG_Character_Common_Tools::generatePlayerCharacter();
//
//     // dump player
//     std::cout << player;
//
//     std::cout << ACE_TEXT("OK ? (y/n): ");
//     std::cin >> c;
//   } while (c == 'n');
//
//   return player;

  return RPG_Character_Common_Tools::generatePlayerCharacter();
}

void
do_work(const std::string magicDictionaryFilename_in,
        const std::string itemDictionaryFilename_in,
        const unsigned int& numPartyMembers_in,
        const bool& random_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1a: init randomization
  RPG_Dice::init();

  // step1b: init string conversion facilities
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

  // step2: display menu options
  RPG_Character_Player player = RPG_Character_Player::dummy();
  bool done = false;
  char c = ' ';
  unsigned int numPlayers = 0;
  do
  {
    // step1: generate new player character
    player = (random_in ? generate_random_player_character()
                        : generate_player_character());
    player.dump();

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
            std::string path = RPG_CHARACTER_DUMP_DIR;
            path += ACE_DIRECTORY_SEPARATOR_STR;
            path += player.getName();
            path += ACE_TEXT_ALWAYS_CHAR(".xml");

            // sanity check
            if (RPG_Common_File_Tools::isReadable(path))
            {
              bool proceed = false;
              c = ' ';
              do
              {
                std::cout << ACE_TEXT("file \"") << path << ACE_TEXT("\" exists, overwrite ? (y/n): ");
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
                               ACE_TEXT("unrecognized (gender) option \"%c\", try again\n"),
                               c));
                    break;
                  }
                } // end SWITCH
              } while ((c != 'y') &&
                       (c != 'n'));

              if (!proceed)
                break;
            } // end IF

            if (!player.save(path))
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to RPG_Character_Player::save(\"%s\"), continuing\n"),
                         path.c_str()));
            else
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("saved player \"%s\" to file: \"%s\"\n"),
                         player.getName().c_str(),
                         path.c_str()));

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
                       ACE_TEXT("unrecognized (gender) option \"%c\", try again\n"),
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
      std::string path = RPG_CHARACTER_DUMP_DIR;
      path += ACE_DIRECTORY_SEPARATOR_STR;
      path += player.getName();
      path += ACE_TEXT_ALWAYS_CHAR(".xml");
      if (!player.save(path))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Character_Player::save(\"%s\"), continuing\n"),
                   path.c_str()));
      else
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("saved player \"%s\" to file: \"%s\"\n"),
                   player.getName().c_str(),
                   path.c_str()));

      numPlayers++;
      if (numPlayers == numPartyMembers_in)
        done = true;
    } // end ELSE
  } while (!done);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void do_printVersion()
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << ACE_TEXT("generator: ")
            << TEST_U_VERSION
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
  RPG_TRACE(ACE_TEXT("::main"));

  // step1: init
  // step1a set defaults
  std::string itemDictionaryFilename;
  std::string magicDictionaryFilename;
  unsigned int numPartyMembers = 0;
  bool random                  = false;
  bool traceInformation        = false;
  bool printVersionAndExit     = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          itemDictionaryFilename,
                          magicDictionaryFilename,
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
  if (magicDictionaryFilename.empty() ||
      itemDictionaryFilename.empty())
  {
    ACE_DEBUG((LM_DEBUG,
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
    do_printVersion();

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(magicDictionaryFilename,
          itemDictionaryFilename,
          numPartyMembers,
          random);

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
