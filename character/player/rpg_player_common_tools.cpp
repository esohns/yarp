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

#include "rpg_player_common_tools.h"

#include "rpg_player_defines.h"

#include <rpg_item_armor.h>
#include <rpg_item_commodity.h>
#include <rpg_item_weapon.h>
#include <rpg_item_dictionary.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_common_tools.h>

#include <rpg_magic_defines.h>
#include <rpg_magic_common_tools.h>

#include <rpg_character_common_tools.h>
#include <rpg_character_class_common_tools.h>
#include <rpg_character_skills_common_tools.h>
#include <rpg_character_XML_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_tools.h>

#include <rpg_dice_common.h>
#include <rpg_dice.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <numeric>

RPG_Character_Race_t
RPG_Player_Common_Tools::raceXMLTreeToRace(const RPG_Player_PlayerXML_XMLTree_Type::race_sequence& races_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::raceXMLTreeToRace"));

  RPG_Character_Race_t result;

  RPG_Character_Race current;
  for (RPG_Player_PlayerXML_XMLTree_Type::race_const_iterator iterator = races_in.begin();
       iterator != races_in.end();
       iterator++)
  {
    current = RPG_Character_RaceHelper::stringToRPG_Character_Race(*iterator);
    if (current > RACE_NONE)
      result.set(current - 1); // *NOTE*: -1 !
  } // end FOR

  return result;
}

RPG_Character_Abilities_t
RPG_Player_Common_Tools::abilitiesXMLTreeToAbilities(const RPG_Player_Abilities_XMLTree_Type& abilities_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::abilitiesXMLTreeToAbilities"));

  // init result
  RPG_Character_Abilities_t result;

  for (RPG_Player_Abilities_XMLTree_Type::ability_const_iterator iterator = abilities_in.ability().begin();
       iterator != abilities_in.ability().end();
       iterator++)
    result.insert(RPG_Character_AbilityHelper::stringToRPG_Character_Ability(*iterator));

  return result;
}

RPG_Magic_SpellTypes_t
RPG_Player_Common_Tools::spellsXMLTreeToSpellTypes(const RPG_Player_Spells_XMLTree_Type& spells_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::spellsXMLTreeToSpellTypes"));

  RPG_Magic_SpellTypes_t result;

  for (RPG_Player_Spells_XMLTree_Type::spell_const_iterator iterator = spells_in.spell().begin();
       iterator != spells_in.spell().end();
       iterator++)
    result.insert(RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(*iterator));

  return result;
}

RPG_Character_Conditions_t
RPG_Player_Common_Tools::conditionsXMLTreeToConditions(const RPG_Player_Conditions_XMLTree_Type& conditions_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::conditionsXMLTreeToConditions"));

  RPG_Character_Conditions_t result;

  for (RPG_Player_Conditions_XMLTree_Type::condition_const_iterator iterator = conditions_in.condition().begin();
       iterator != conditions_in.condition().end();
       iterator++)
    result.insert(RPG_Common_ConditionHelper::stringToRPG_Common_Condition(*iterator));

  return result;
}

RPG_Magic_Spells_t
RPG_Player_Common_Tools::spellsXMLTreeToSpells(const RPG_Player_Spells_XMLTree_Type& spells_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::spellsXMLTreeToSpells"));

  RPG_Magic_Spells_t result;

  for (RPG_Player_Spells_XMLTree_Type::spell_const_iterator iterator = spells_in.spell().begin();
       iterator != spells_in.spell().end();
       iterator++)
    result.push_back(RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(*iterator));

  return result;
}

RPG_Player*
RPG_Player_Common_Tools::generatePlayer()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::generatePlayer"));

  // step1: name
  std::string name;
  // generate a string of (random ASCII alphabet, printable) characters
  int length = 0;
  RPG_Dice_RollResult_t result;
  RPG_Dice::generateRandomNumbers(10, // maximum length
                                  1,
                                  result);
  length = result.front();
  result.clear();
  RPG_Dice::generateRandomNumbers(26, // characters in (ASCII) alphabet
                                  (2 * length), // first half are characters, last half interpreted as boolean (upper/lower)
                                  result);
  bool lowercase = false;
  for (int i = 0;
       i < length;
       i++)
  {
    // upper/lower ?
    if (result[length + i] > 13)
      lowercase = false;
    else
      lowercase = true;

    name += static_cast<char>((lowercase ? 96 : 64) + result[i]); // 97 == 'a', 65 == 'A'
  } // end FOR

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("generated name: \"%s\"\n"),
//              name.c_str()));

  // step2: gender
  RPG_Character_Gender gender = RPG_CHARACTER_GENDER_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_CHARACTER_GENDER_MAX - 2),
                                  1,
                                  result);
  gender = static_cast<RPG_Character_Gender> (result.front());

  // step3: race
  RPG_Character_Race_t player_race(0);
  // *TODO*: consider allowing multi-race like Half-Elf etc.
  RPG_Character_Race race = RPG_CHARACTER_RACE_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_CHARACTER_RACE_MAX - 1),
                                  1,
                                  result);
  race = static_cast<RPG_Character_Race>(result.front());
  player_race.flip(race - 1);

  // step4: class
  RPG_Character_Class player_class;
  player_class.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  RPG_Common_SubClass player_subclass = RPG_COMMON_SUBCLASS_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_COMMON_SUBCLASS_MAX - 1),
                                  1,
                                  result);
  player_subclass = static_cast<RPG_Common_SubClass>(result.front());
  player_class.metaClass = RPG_Character_Class_Common_Tools::subClassToMetaClass(player_subclass);
  player_class.subClasses.insert(player_subclass);
//   player_class.subClasses.push_back(player_subclass);

  // step5: alignment
  RPG_Character_Alignment alignment;
  alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_CHARACTER_ALIGNMENTCIVIC_MAX - 2),
                                  2,
                                  result);
  alignment.civic = static_cast<RPG_Character_AlignmentCivic>(result.front());
  alignment.ethic = static_cast<RPG_Character_AlignmentEthic>(result.back());

  // step6: attributes
  RPG_Character_Attributes attributes;
  ACE_OS::memset(&attributes, 0, sizeof(RPG_Character_Attributes));
  unsigned char* p = &(attributes.strength);
  RPG_Dice_Roll roll;
  roll.numDice = 2;
  roll.typeDice = D_10;
  roll.modifier = -2; // interval: 0-18
  // make sure the result is somewhat balanced (average == 6 * 9 ?)...
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

  // step7: (initial) skills
  RPG_Character_Skills_t skills;
  unsigned int initialSkillPoints = 0;
  RPG_Character_Skills_Common_Tools::getSkillPoints(player_subclass,
                                                    RPG_Character_Common_Tools::getAttributeAbilityModifier(attributes.intelligence),
                                                    initialSkillPoints);
  RPG_Character_SkillsIterator_t iterator;
  RPG_Common_Skill skill = RPG_COMMON_SKILL_INVALID;
  for (unsigned int i = 0;
       i < initialSkillPoints;
       i++)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_COMMON_SKILL_MAX,
                                    1,
                                    result);
    skill = static_cast<RPG_Common_Skill>((result.front() - 1));
    iterator = skills.find(skill);
    if (iterator != skills.end())
      (iterator->second)++;
    else
      skills.insert(std::make_pair(skill,
                                   static_cast<char>(1)));
  } // end FOR

  // step8: (initial) feats & abilities
  RPG_Character_Feats_t feats;
  unsigned int initialFeats = 0;
  RPG_Character_Abilities_t abilities;
  RPG_Character_Skills_Common_Tools::getNumFeatsAbilities(race,
                                                          player_subclass,
                                                          1,
                                                          feats,
                                                          initialFeats,
                                                          abilities);
  RPG_Character_FeatsIterator_t iterator2;
  RPG_Character_Feat feat = RPG_CHARACTER_FEAT_INVALID;
  do
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_CHARACTER_FEAT_MAX,
                                    1,
                                    result);
    feat = static_cast<RPG_Character_Feat>((result.front() - 1));

    // check prerequisites
    if (!RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites(feat,
                                                                   player_subclass,
                                                                   1,
                                                                   attributes,
                                                                   skills,
                                                                   feats,
                                                                   abilities))
    {
      // try again
      continue;
    } // end IF

    iterator2 = feats.find(feat);
    if (iterator2 != feats.end())
    {
      // try again
      continue;
    } // end IF

    feats.insert(feat);
  } while (feats.size() < initialFeats);

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

  // step10: (initial) Hit Points
  unsigned short int hitpoints = 0;
//   roll.numDice = 1;
//   roll.typeDice = RPG_Character_Common_Tools::getHitDie(player_class.subClass);
//   roll.modifier = 0;
//   result.clear();
//   RPG_Dice::simulateRoll(roll,
//                          1,
//                          result);
//   hitpoints = result.front();
  // *NOTE*: players start with maxed HP...
  hitpoints = RPG_Character_Common_Tools::getHitDie(player_subclass);

  // step11: (initial) set of spells
  unsigned int numKnownSpells = 0;
  unsigned int numSpells = 0;
  RPG_Magic_SpellTypes_t knownSpells;
  RPG_Magic_Spells_t spells;
  unsigned int numChosen = 0;
  RPG_Magic_SpellTypes_t available;
  RPG_Magic_SpellTypesIterator_t available_iterator;
  RPG_Magic_CasterClassUnion casterClass;
  casterClass.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin();
       iterator != player_class.subClasses.end();
       iterator++)
  {
    if (!RPG_Magic_Common_Tools::isCasterClass(*iterator))
      continue;

    casterClass.subclass = *iterator;

    switch (*iterator)
    {
      case SUBCLASS_BARD:
      case SUBCLASS_SORCERER:
      {
        for (unsigned char i = 0;
             i <= RPG_COMMON_MAX_SPELL_LEVEL;
             i++)
        {
          numKnownSpells = 0;

          // step1: get list of available spells
          available = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                            i);

          // step2: compute # known spells
          numKnownSpells = RPG_Magic_Common_Tools::getNumKnownSpells(*iterator,
                                                                     1,
                                                                     i);

          //         ACE_DEBUG((LM_DEBUG,
          //                    ACE_TEXT("number of initial known spells (lvl %d) for subClass \"%s\" is: %d...\n"),
          //                    i,
          //                    RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str(),
          //                    numKnownSpells));

          // make sure we have enough variety...
          ACE_ASSERT(numKnownSpells <= available.size());

          // step3: choose known spells
          if (numKnownSpells == 0)
            break; // done

          numChosen = 0;
          while (numChosen < numKnownSpells)
          {
            result.clear();
            RPG_Dice::generateRandomNumbers(available.size(),
                                            (numKnownSpells - numChosen),
                                            result);

            for (RPG_Dice_RollResultIterator_t iterator2 = result.begin();
                 iterator2 != result.end();
                 iterator2++)
            {
              available_iterator = available.begin();
              std::advance(available_iterator, *iterator2 - 1);
              if (knownSpells.find(*available_iterator) != knownSpells.end())
                continue; // try again

              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("chose known spell #%d: \"%s\"\n"),
                         numChosen + 1,
                         RPG_Magic_Common_Tools::spellToName(*available_iterator).c_str()));

              knownSpells.insert(*available_iterator);
              numChosen++;
            } // end FOR
          } // end WHILE
        } // end FOR

        // *WARNING*: falls through !
      }
      case SUBCLASS_CLERIC:
      case SUBCLASS_DRUID:
      case SUBCLASS_PALADIN:
      case SUBCLASS_RANGER:
      {
        for (unsigned char i = 0;
             i <= RPG_COMMON_MAX_SPELL_LEVEL;
             i++)
        {
          numSpells = 0;

          // step1: get list of available spells
          available = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                            i);

          // step2: compute # prepared spells
          numSpells = RPG_Magic_Common_Tools::getNumSpells(*iterator,
                                                           1,
                                                           i);

    //       ACE_DEBUG((LM_DEBUG,
    //                  ACE_TEXT("number of initial memorized/prepared spells (lvl %d) for subClass \"%s\" is: %d...\n"),
    //                  i,
    //                  RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str(),
    //                  numSpells));

          // step3: choose prepared spells
          if (numSpells == 0)
            break; // done

          result.clear();
          RPG_Dice::generateRandomNumbers((RPG_Magic_Common_Tools::isDivineCasterClass(*iterator) ? available.size()
                                                                                                  : knownSpells.size()),
                                          numSpells,
                                          result);
          int index = 0;
          for (RPG_Dice_RollResultIterator_t iterator2 = result.begin();
               iterator2 != result.end();
               iterator2++, index++)
          {
            available_iterator = (RPG_Magic_Common_Tools::isDivineCasterClass(*iterator) ? available.begin()
                                                                                         : knownSpells.begin());
            std::advance(available_iterator, *iterator2 - 1);

            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("chose prepared spell #%d: \"%s\"\n"),
                       index + 1,
                       RPG_Magic_Common_Tools::spellToName(*available_iterator).c_str()));

            spells.push_back(*available_iterator);
          } // end FOR
        } // end FOR

        break;
      }
      case SUBCLASS_WIZARD:
      {
        numKnownSpells = RPG_MAGIC_DEF_NUM_NEW_SPELLS_PER_LEVEL;

        // step1: collect list of available spells
        RPG_Magic_SpellTypes_t current;
        char i = 0;
        for (;
             i <= RPG_COMMON_MAX_SPELL_LEVEL;
             i++)
        {
          if (RPG_Magic_Common_Tools::getNumSpells(*iterator,
                                                   1,
                                                   i) == 0)
          {
            i--;

            break; // done
          } // end FOR

          current = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                          i);
          available.insert(current.begin(), current.end());
        } // end FOR

        // step2: chose # known spells
        numChosen = 0;
        while (numChosen < numKnownSpells)
        {
          result.clear();
          RPG_Dice::generateRandomNumbers(available.size(),
                                          (numKnownSpells - numChosen),
                                          result);

          for (RPG_Dice_RollResultIterator_t iterator2 = result.begin();
               iterator2 != result.end();
               iterator2++)
          {
            available_iterator = available.begin();
            std::advance(available_iterator, *iterator2 - 1);
            if (knownSpells.find(*available_iterator) != knownSpells.end())
              continue; // try again

            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("chose known spell #%d: \"%s\"\n"),
                       numChosen + 1,
                       RPG_Magic_Common_Tools::spellToName(*available_iterator).c_str()));

            knownSpells.insert(*available_iterator);
            numChosen++;
          } // end FOR
        } // end WHILE

        // step3: chose # prepared spells
        for (char j = i;
             j >= 0;
             j--)
        {
          numSpells += RPG_Magic_Common_Tools::getNumSpells(*iterator,
                                                            1,
                                                            j);
          ACE_ASSERT(numSpells);

          current = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                          j);
          available.clear();
          for (RPG_Magic_SpellTypesIterator_t iterator2 = current.begin();
               iterator2 != current.end();
               iterator2++)
            if (knownSpells.find(*iterator2) != knownSpells.end())
              available.insert(*iterator2);
//           std::set_intersection(current.begin(),
//                                 current.end(),
//                                 knownSpells.begin(),
//                                 knownSpells.end(),
//                                 available.begin());
          if (available.empty())
            continue; // done

          result.clear();
          RPG_Dice::generateRandomNumbers(available.size(),
                                          numSpells,
                                          result);
          int index = 0;
          for (RPG_Dice_RollResultIterator_t iterator2 = result.begin();
               iterator2 != result.end();
               iterator2++, index++)
          {
            available_iterator = available.begin();
            std::advance(available_iterator, *iterator2 - 1);

            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("chose prepared spell #%d: \"%s\"\n"),
                       index + 1,
                       RPG_Magic_Common_Tools::spellToName(*available_iterator).c_str()));

            spells.push_back(*available_iterator);
          } // end FOR

          numSpells = 0;
        } // end FOR

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid class (was: %s), aborting\n"),
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str()));

        break;
      }
    } // end SWITCH
  } // end FOR

  // step12: initialize condition
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL); // start "normal"

  // step13: (initial) set of items
  // *TODO*: somehow generate these at random too ?
  RPG_Item_List_t items;
  RPG_Item_Instance_Base* current = NULL;
  try
  {
    switch (player_subclass)
    {
      case SUBCLASS_FIGHTER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_MAIL_SPLINT);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      case SUBCLASS_PALADIN:
//       case SUBCLASS_WARLORD:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_PLATE_FULL);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_SHIELD_HEAVY_STEEL);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      case SUBCLASS_RANGER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          RANGED_WEAPON_BOW_LONG);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_HIDE);
        ACE_ASSERT(current);
        items.insert(current->getID());

        // *TODO*: no arrows ?

        break;
      }
//       case SUBCLASS_BARBARIAN:
//       {
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
//             ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
//         ACE_ASSERT(current);
//         items.insert(current->getID());
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
//             ARMOR_HIDE);
//         ACE_ASSERT(current);
//         items.insert(current->getID());
//
//         break;
//       }
      case SUBCLASS_WIZARD:
      case SUBCLASS_SORCERER:
//       case SUBCLASS_WARLOCK:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      case SUBCLASS_CLERIC:
//       case SUBCLASS_AVENGER:
//       case SUBCLASS_INVOKER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          ONE_HANDED_MELEE_WEAPON_MACE_HEAVY);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_MAIL_CHAIN);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      case SUBCLASS_DRUID:
//       case SUBCLASS_SHAMAN:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          LIGHT_MELEE_WEAPON_SICKLE);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_HIDE);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_SHIELD_LIGHT_WOODEN);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
//       case SUBCLASS_MONK:
//       {
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
//             TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
//         ACE_ASSERT(current);
//         items.insert(current->getID());
//
//         break;
//       }
      case SUBCLASS_THIEF:
      case SUBCLASS_BARD:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                          LIGHT_MELEE_WEAPON_SWORD_SHORT);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_LEATHER);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                          ARMOR_SHIELD_LIGHT_STEEL);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid subclass \"%s\", aborting\n"),
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(player_subclass).c_str()));

        break;
      }
    } // end SWITCH
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::generatePlayerCharacter(): caught exception: \"%s\", continuing\n"),
               exception.what()));
  }

  // step14: instantiate player character
  RPG_Player* player_p = NULL;
  try
  {
    player_p = new RPG_Player(name,
                              gender,
                              player_race,
							  player_class,
                              alignment,
                              attributes,
                              skills,
                              feats,
                              abilities,
                              offHand,
                              hitpoints,
                              knownSpells,
                              condition,
                              hitpoints, // start healthy
                              0,
                              RPG_PLAYER_START_MONEY,
                              spells,
                              items);
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::generatePlayerCharacter(): exception occurred: \"%s\", aborting\n"),
               exception.what()));

    return player_p;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::generatePlayerCharacter(): exception occurred, aborting\n")));

    return player_p;
  }
  ACE_ASSERT(player_p);

  return player_p;
}

const unsigned int
RPG_Player_Common_Tools::restParty(RPG_Player_Party_t& party_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::restParty"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("party status:\n-------------\n")));
  int index = 1;
  RPG_Player_PartyIterator_t iterator;
  for (iterator = party_in.begin();
       iterator != party_in.end();
       iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("#%d] \"%s\" (lvl: %d), HP: %d/%d --> %s\n"),
               index,
               (*iterator)->getName().c_str(),
               (*iterator)->getLevel(),
               (*iterator)->getNumHitPoints(),
               (*iterator)->getNumTotalHitPoints(),
               ((*iterator)->hasCondition(CONDITION_NORMAL) ? ACE_TEXT_ALWAYS_CHAR("OK") : ACE_TEXT_ALWAYS_CHAR("DOWN"))));
  } // end FOR

  // check party status
  unsigned int diff = 0;
  unsigned int fraction = 0;
  unsigned int recoveryTime = 0;
  unsigned int maxRecoveryTime = 0;
  for (iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    // *TODO*: consider dead/dying players !
    if ((*iterator)->getNumHitPoints() < 0)
      continue;

    diff = ((*iterator)->getNumTotalHitPoints() - (*iterator)->getNumHitPoints());
    fraction = (diff % ((*iterator)->getLevel() * 2));
    diff -= fraction;
    recoveryTime = ((diff / ((*iterator)->getLevel() * 2)) + // days of complete bed-rest +
                    (fraction / (*iterator)->getLevel()));   // days of good night's sleep
    if (recoveryTime > maxRecoveryTime)
      maxRecoveryTime = recoveryTime;
  } // end FOR

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("max. recovery time: %d days\n"),
             maxRecoveryTime));

  for (iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    (*iterator)->rest(REST_FULL,
                     (maxRecoveryTime * 24));
  } // end FOR

  return (maxRecoveryTime * 24 * 3600);
}

RPG_Player*
RPG_Player_Common_Tools::playerXMLToPlayer(const RPG_Player_PlayerXML_XMLTree_Type& player_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::playerXMLToPlayer"));

  RPG_Character_Alignment alignment;
  alignment.civic = RPG_Character_AlignmentCivicHelper::stringToRPG_Character_AlignmentCivic(player_in.alignment().civic());
  alignment.ethic = RPG_Character_AlignmentEthicHelper::stringToRPG_Character_AlignmentEthic(player_in.alignment().ethic());
  RPG_Character_Attributes attributes;
  attributes.strength = player_in.attributes().strength();
  attributes.dexterity = player_in.attributes().dexterity();
  attributes.constitution = player_in.attributes().constitution();
  attributes.intelligence = player_in.attributes().intelligence();
  attributes.wisdom = player_in.attributes().wisdom();
  attributes.charisma = player_in.attributes().charisma();
  RPG_Character_Skills_t skills;
  if (player_in.skills().present())
    skills = RPG_Character_XML_Tools::skillsXMLTreeToSkills(player_in.skills().get());
  RPG_Character_Feats_t feats;
  if (player_in.feats().present())
    feats = RPG_Character_XML_Tools::featsXMLTreeToFeats(player_in.feats().get());
  RPG_Character_Abilities_t abilities;
  if (player_in.abilities().present())
    abilities = RPG_Player_Common_Tools::abilitiesXMLTreeToAbilities(player_in.abilities().get());
  RPG_Magic_SpellTypes_t known_spells;
  if (player_in.knownSpells().present())
    known_spells = RPG_Player_Common_Tools::spellsXMLTreeToSpellTypes(player_in.knownSpells().get());
  RPG_Magic_Spells_t spells;
  if (player_in.spells().present())
    spells = RPG_Player_Common_Tools::spellsXMLTreeToSpells(player_in.spells().get());

  RPG_Player* player_p = NULL;
  try
  {
    player_p = new RPG_Player(player_in.name(),
                              RPG_Character_GenderHelper::stringToRPG_Character_Gender(player_in.gender()),
                              RPG_Player_Common_Tools::raceXMLTreeToRace(player_in.race()),
                              RPG_Character_Class_Common_Tools::classXMLTreeToClass(player_in.classXML()),
                              alignment,
                              attributes,
                              skills,
                              feats,
                              abilities,
                              RPG_Character_OffHandHelper::stringToRPG_Character_OffHand(player_in.offhand()),
                              player_in.maxHP(),
                              known_spells,
                              RPG_Player_Common_Tools::conditionsXMLTreeToConditions(player_in.conditions()),
                              player_in.HP(),
                              player_in.XP(),
                              player_in.gold(),
                              spells,
                              RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->instantiate(player_in.inventory()));
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::playerXMLToPlayer(): exception occurred: \"%s\", aborting\n"),
               exception.what()));

    return player_p;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::playerXMLToPlayer(): exception occurred, aborting\n")));

    return player_p;
  }
  ACE_ASSERT(player_p);

  return player_p;
}

RPG_Player_PlayerXML_XMLTree_Type*
RPG_Player_Common_Tools::playerToPlayerXML(const RPG_Player& player_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::playerToPlayerXML"));

  RPG_Character_Alignment_XMLTree_Type alignment(RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(player_in.getAlignment().civic),
  RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_in.getAlignment().ethic));

  RPG_Character_Attributes_XMLTree_Type attributes(player_in.getAttribute(ATTRIBUTE_STRENGTH),
                                                   player_in.getAttribute(ATTRIBUTE_DEXTERITY),
                                                   player_in.getAttribute(ATTRIBUTE_CONSTITUTION),
                                                   player_in.getAttribute(ATTRIBUTE_INTELLIGENCE),
                                                   player_in.getAttribute(ATTRIBUTE_WISDOM),
                                                   player_in.getAttribute(ATTRIBUTE_CHARISMA));

  RPG_Player_Conditions_XMLTree_Type conditions;
  RPG_Character_Conditions_t character_condition = player_in.getCondition();
  for (RPG_Character_ConditionsIterator_t iterator = character_condition.begin();
       iterator != character_condition.end();
       iterator++)
    conditions.condition().push_back(RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator));

  RPG_Item_InventoryXML_XMLTree_Type inventory;
  RPG_Player_Inventory character_inventory = player_in.getInventory();
  RPG_Item_Base* item_base = NULL;
  for (RPG_Item_ListIterator_t iterator = character_inventory.myItems.begin();
       iterator != character_inventory.myItems.end();
       iterator++)
  {
    // retrieve item details
    RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                         item_base);
    ACE_ASSERT(item_base);
    RPG_Item_XML_XMLTree_Type item(RPG_Item_TypeHelper::RPG_Item_TypeToString(item_base->getType()));
    switch (item_base->getType())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(item_base);
        ACE_ASSERT(armor);

        RPG_Item_ArmorProperties armor_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor->getArmorType());

        RPG_Item_StorePrice_XMLTree_Type store_price;
        if (armor_properties.baseStorePrice.numGoldPieces)
          store_price.numGoldPieces(armor_properties.baseStorePrice.numGoldPieces);
        if (armor_properties.baseStorePrice.numSilverPieces)
          store_price.numSilverPieces(armor_properties.baseStorePrice.numSilverPieces);
        RPG_Item_ArmorPropertiesXML_XMLTree_Type armor_properties_xml(armor_properties.baseWeight,
                                                                      store_price,
                                                                      RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()),
                                                                      RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString(armor_properties.category),
                                                                      armor_properties.baseBonus,
                                                                      armor_properties.maxDexterityBonus,
                                                                      armor_properties.checkPenalty,
                                                                      armor_properties.arcaneSpellFailure,
                                                                      armor_properties.baseSpeed);
        if (armor_properties.defenseModifier)
          armor_properties_xml.defenseModifier(armor_properties.defenseModifier);
        if (armor_properties.aura != RPG_MAGIC_SCHOOL_INVALID)
          armor_properties_xml.aura(RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString(armor_properties.aura));
        if (armor_properties.prerequisites.minCasterLevel)
        {
          RPG_Item_MagicalPrerequisites_XMLTree_Type magical_prerequisites;
          magical_prerequisites.minCasterLevel(armor_properties.prerequisites.minCasterLevel);
          armor_properties_xml.prerequisites(magical_prerequisites);
        } // end IF
        if (armor_properties.costToCreate.numGoldPieces ||
            armor_properties.costToCreate.numExperiencePoints)
        {
          RPG_Item_CreationCost_XMLTree_Type costs_to_create;
          if (armor_properties.costToCreate.numGoldPieces)
            costs_to_create.numGoldPieces(armor_properties.costToCreate.numGoldPieces);
          if (armor_properties.costToCreate.numExperiencePoints)
            costs_to_create.numExperiencePoints(armor_properties.costToCreate.numExperiencePoints);
          armor_properties_xml.costToCreate(costs_to_create);
        } // end IF

        item.armor(armor_properties_xml);
        inventory.item().push_back(item);

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = dynamic_cast<RPG_Item_Commodity*>(item_base);
        ACE_ASSERT(commodity);

        RPG_Item_CommodityProperties commodity_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getCommodityProperties(commodity->getCommoditySubType());

        RPG_Item_StorePrice_XMLTree_Type store_price;
        if (commodity_properties.baseStorePrice.numGoldPieces)
          store_price.numGoldPieces(commodity_properties.baseStorePrice.numGoldPieces);
        if (commodity_properties.baseStorePrice.numSilverPieces)
          store_price.numSilverPieces(commodity_properties.baseStorePrice.numSilverPieces);
        RPG_Item_CommodityPropertiesBase_XMLTree_Type commodity_properties_xml(commodity_properties.baseWeight,
                                                                               store_price,
                                                                               RPG_Item_CommodityTypeHelper::RPG_Item_CommodityTypeToString(commodity->getCommodityType()),
                                                                               RPG_Item_Common_Tools::commoditySubTypeToXMLString(commodity->getCommoditySubType()));
        if (commodity_properties.aura != RPG_MAGIC_SCHOOL_INVALID)
          commodity_properties_xml.aura(RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString(commodity_properties.aura));
        if (commodity_properties.prerequisites.minCasterLevel)
        {
          RPG_Item_MagicalPrerequisites_XMLTree_Type magical_prerequisites;
          magical_prerequisites.minCasterLevel(commodity_properties.prerequisites.minCasterLevel);
          commodity_properties_xml.prerequisites(magical_prerequisites);
        } // end IF
        if (commodity_properties.costToCreate.numGoldPieces ||
            commodity_properties.costToCreate.numExperiencePoints)
        {
          RPG_Item_CreationCost_XMLTree_Type costs_to_create;
          if (commodity_properties.costToCreate.numGoldPieces)
            costs_to_create.numGoldPieces(commodity_properties.costToCreate.numGoldPieces);
          if (commodity_properties.costToCreate.numExperiencePoints)
            costs_to_create.numExperiencePoints(commodity_properties.costToCreate.numExperiencePoints);
          commodity_properties_xml.costToCreate(costs_to_create);
        } // end IF

        item.commodity(commodity_properties_xml);
        inventory.item().push_back(item);

        break;
      }
      case ITEM_OTHER:
      case ITEM_VALUABLE:
      {
        // *TODO*
        ACE_ASSERT(false);

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(item_base);
        ACE_ASSERT(weapon);

        RPG_Item_WeaponProperties weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon->getWeaponType());

        RPG_Item_XML_XMLTree_Type item(RPG_Item_TypeHelper::RPG_Item_TypeToString(item_base->getType()));
        RPG_Item_StorePrice_XMLTree_Type store_price;
        if (weapon_properties.baseStorePrice.numGoldPieces)
          store_price.numGoldPieces(weapon_properties.baseStorePrice.numGoldPieces);
        if (weapon_properties.baseStorePrice.numSilverPieces)
          store_price.numSilverPieces(weapon_properties.baseStorePrice.numSilverPieces);
        RPG_Dice_Roll_XMLTree_Type base_damage(RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString(weapon_properties.baseDamage.typeDice));
        if (weapon_properties.baseDamage.numDice)
          base_damage.numDice(weapon_properties.baseDamage.numDice);
        if (weapon_properties.baseDamage.modifier)
          base_damage.modifier(weapon_properties.baseDamage.modifier);
        RPG_Item_CriticalHitProperties_XMLTree_Type critical_hit_properties(weapon_properties.criticalHit.minToHitRoll,
                                                                            weapon_properties.criticalHit.damageModifier);
        RPG_Item_WeaponPropertiesXML_XMLTree_Type weapon_properties_xml(weapon_properties.baseWeight,
                                                                        store_price,
                                                                        RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon->getWeaponType()),
                                                                        RPG_Item_WeaponCategoryHelper::RPG_Item_WeaponCategoryToString(weapon_properties.category),
                                                                        RPG_Item_WeaponClassHelper::RPG_Item_WeaponClassToString(weapon_properties.weaponClass),
                                                                        base_damage,
                                                                        critical_hit_properties);
        if (weapon_properties.toHitModifier)
          weapon_properties_xml.toHitModifier(weapon_properties.toHitModifier);
        if (weapon_properties.rangeIncrement)
          weapon_properties_xml.rangeIncrement(weapon_properties.rangeIncrement);
        RPG_Item_WeaponPropertiesBase_XMLTree_Type::typeOfDamage_sequence type_of_damage;
        int index = PHYSICALDAMAGE_NONE; index++;
        for (unsigned int i = 0;
             i < weapon_properties.typeOfDamage.size();
             i++, index++)
         if (weapon_properties.typeOfDamage.test(i))
           type_of_damage.push_back(RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString(static_cast<RPG_Common_PhysicalDamageType>(index)));
        weapon_properties_xml.typeOfDamage(type_of_damage);
        weapon_properties_xml.isNonLethal(weapon_properties.isNonLethal);
        weapon_properties_xml.isReachWeapon(weapon_properties.isReachWeapon);
        weapon_properties_xml.isDoubleWeapon(weapon_properties.isDoubleWeapon);
        if (weapon_properties.aura != RPG_MAGIC_SCHOOL_INVALID)
          weapon_properties_xml.aura(RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString(weapon_properties.aura));
        if (weapon_properties.prerequisites.minCasterLevel)
        {
          RPG_Item_MagicalPrerequisites_XMLTree_Type magical_prerequisites;
          magical_prerequisites.minCasterLevel(weapon_properties.prerequisites.minCasterLevel);
          weapon_properties_xml.prerequisites(magical_prerequisites);
        } // end IF
        if (weapon_properties.costToCreate.numGoldPieces ||
            weapon_properties.costToCreate.numExperiencePoints)
        {
          RPG_Item_CreationCost_XMLTree_Type costs_to_create;
          if (weapon_properties.costToCreate.numGoldPieces)
            costs_to_create.numGoldPieces(weapon_properties.costToCreate.numGoldPieces);
          if (weapon_properties.costToCreate.numExperiencePoints)
            costs_to_create.numExperiencePoints(weapon_properties.costToCreate.numExperiencePoints);
          weapon_properties_xml.costToCreate(costs_to_create);
        } // end IF

        item.weapon(weapon_properties_xml);
        inventory.item().push_back(item);

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid item type (was: \"%s\"), aborting\n"),
                   RPG_Item_TypeHelper::RPG_Item_TypeToString(item_base->getType()).c_str()));

        return false;
      }
    } // end SWITCH
  } // end FOR

  RPG_Character_Class character_class = player_in.getClass();
  RPG_Character_ClassXML_XMLTree_Type classXML(RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(character_class.metaClass));
  for (RPG_Character_SubClassesIterator_t iterator = character_class.subClasses.begin();
       iterator != character_class.subClasses.end();
       iterator++)
    classXML.subClass().push_back(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator));

  RPG_Player_PlayerXML_XMLTree_Type* player_p = NULL;
  try
  {
    player_p = new RPG_Player_PlayerXML_XMLTree_Type(player_in.getName(),
                                                     alignment,
                                                     attributes,
                                                     RPG_Common_SizeHelper::RPG_Common_SizeToString(player_in.getSize()),
                                                     player_in.getNumTotalHitPoints(),
                                                     conditions,
                                                     player_in.getNumHitPoints(),
                                                     player_in.getExperience(),
                                                     player_in.getWealth(),
                                                     inventory,
                                                     RPG_Character_GenderHelper::RPG_Character_GenderToString(player_in.getGender()),
                                                     classXML,
                                                     RPG_Character_OffHandHelper::RPG_Character_OffHandToString(player_in.getOffHand()));
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::playerToPlayerXML(): exception occurred: \"%s\", aborting\n"),
               exception.what()));

    return player_p;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player_Common_Tools::playerToPlayerXML(): exception occurred, aborting\n")));

    return player_p;
  }
  ACE_ASSERT(player_p);

  // *NOTE*: add race, skills, feats, abilities, known spells, prepared spells sequences "manually"
  RPG_Character_Race_t character_race = player_in.getRace();
  int index = RACE_NONE; index++;
  for (unsigned int i = 0;
       i < character_race.size();
       i++, index++)
    if (character_race.test(i))
      player_p->race().push_back(RPG_Character_RaceHelper::RPG_Character_RaceToString(static_cast<RPG_Character_Race>(index)));

  RPG_Character_Skills_XMLTree_Type skills;
  RPG_Character_Skills_t character_skills = player_in.getSkills();
  for (RPG_Character_SkillsConstIterator_t iterator = character_skills.begin();
       iterator != character_skills.end();
       iterator++)
  {
    RPG_Character_SkillValue_XMLTree_Type skill(RPG_Common_SkillHelper::RPG_Common_SkillToString((*iterator).first),
                                                (*iterator).second);
    skills.skill().push_back(skill);
  } // end FOR
  player_p->skills().set(skills);

  RPG_Character_Feats_XMLTree_Type feats;
  RPG_Character_Feats_t character_feats = player_in.getFeats();
  for (RPG_Character_FeatsConstIterator_t iterator = character_feats.begin();
       iterator != character_feats.end();
       iterator++)
    feats.feat().push_back(RPG_Character_FeatHelper::RPG_Character_FeatToString(*iterator));
  player_p->feats().set(feats);

  RPG_Player_Abilities_XMLTree_Type abilities;
  RPG_Character_Abilities_t character_abilities = player_in.getAbilities();
  for (RPG_Character_AbilitiesConstIterator_t iterator = character_abilities.begin();
       iterator != character_abilities.end();
       iterator++)
    abilities.ability().push_back(RPG_Character_AbilityHelper::RPG_Character_AbilityToString(*iterator));
  player_p->abilities().set(abilities);

  RPG_Player_Spells_XMLTree_Type spell_list;
  RPG_Magic_SpellTypes_t character_known_spells = player_in.getKnownSpells();
  for (RPG_Magic_SpellTypesIterator_t iterator = character_known_spells.begin();
       iterator != character_known_spells.end();
       iterator++)
    spell_list.spell().push_back(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator));
  player_p->knownSpells().set(spell_list);

  spell_list.spell().clear();
  RPG_Magic_Spells_t character_spells = player_in.getSpells();
  for (RPG_Magic_SpellsIterator_t iterator = character_spells.begin();
       iterator != character_spells.end();
       iterator++)
    spell_list.spell().push_back(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator));
  player_p->spells().set(spell_list);

  return player_p;
}
