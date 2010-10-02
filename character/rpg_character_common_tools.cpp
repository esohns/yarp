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
#include "rpg_character_common_tools.h"

#include "rpg_character_defines.h"
#include "rpg_character_race_common.h"
#include "rpg_character_class_common_tools.h"
#include "rpg_character_skills_common_tools.h"

#include <rpg_item_common_tools.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_dictionary.h>

#include <rpg_magic_dictionary.h>
#include <rpg_magic_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_camp.h>

#include <rpg_dice.h>
#include <rpg_chance_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

// init statics
RPG_Character_GenderToStringTable_t RPG_Character_GenderHelper::myRPG_Character_GenderToStringTable;
RPG_Character_RaceToStringTable_t RPG_Character_RaceHelper::myRPG_Character_RaceToStringTable;
RPG_Character_MetaClassToStringTable_t RPG_Character_MetaClassHelper::myRPG_Character_MetaClassToStringTable;
RPG_Character_AbilityToStringTable_t RPG_Character_AbilityHelper::myRPG_Character_AbilityToStringTable;
RPG_Character_FeatToStringTable_t RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable;
RPG_Character_AlignmentCivicToStringTable_t RPG_Character_AlignmentCivicHelper::myRPG_Character_AlignmentCivicToStringTable;
RPG_Character_AlignmentEthicToStringTable_t RPG_Character_AlignmentEthicHelper::myRPG_Character_AlignmentEthicToStringTable;
RPG_Character_EquipmentSlotToStringTable_t RPG_Character_EquipmentSlotHelper::myRPG_Character_EquipmentSlotToStringTable;
RPG_Character_OffHandToStringTable_t RPG_Character_OffHandHelper::myRPG_Character_OffHandToStringTable;

void
RPG_Character_Common_Tools::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::init"));

  initStringConversionTables();
  RPG_Character_Skills_Common_Tools::init();
}

void
RPG_Character_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::initStringConversionTables"));

  RPG_Character_GenderHelper::init();
  RPG_Character_RaceHelper::init();
  RPG_Character_MetaClassHelper::init();
  RPG_Character_AbilityHelper::init();
  RPG_Character_FeatHelper::init();
  RPG_Character_AlignmentCivicHelper::init();
  RPG_Character_AlignmentEthicHelper::init();
  RPG_Character_EquipmentSlotHelper::init();
  RPG_Character_OffHandHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Common_Tools: initialized string conversion tables...\n")));
}

const std::string
RPG_Character_Common_Tools::alignmentToString(const RPG_Character_Alignment& alignment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::alignmentToString"));

  std::string result;

  result += RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(alignment_in.civic);
  result += ACE_TEXT_ALWAYS_CHAR(" | ");
  result += RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(alignment_in.ethic);

  // "Neutral" "Neutral" --> "True Neutral"
  if ((alignment_in.civic == ALIGNMENTCIVIC_NEUTRAL) &&
      (alignment_in.ethic == ALIGNMENTETHIC_NEUTRAL))
  {
    result = ACE_TEXT_ALWAYS_CHAR("True Neutral");
  } // end IF

  return result;
}

const std::string
RPG_Character_Common_Tools::attributesToString(const RPG_Character_Attributes& attributes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::attributesToString"));

  std::string result;

  std::ostringstream converter;
  result = RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_STRENGTH);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter << ACE_static_cast(unsigned int, attributes_in.strength);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_DEXTERITY);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(unsigned int, attributes_in.dexterity);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_CONSTITUTION);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(unsigned int, attributes_in.constitution);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_INTELLIGENCE);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(unsigned int, attributes_in.intelligence);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_WISDOM);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(unsigned int, attributes_in.wisdom);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_CHARISMA);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(unsigned int, attributes_in.charisma);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string
RPG_Character_Common_Tools::raceToString(const RPG_Character_Race_t& race_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::raceToString"));

  std::string result;

  if (race_in.count() == 0)
  {
    result += RPG_Character_RaceHelper::RPG_Character_RaceToString(RACE_NONE);

    // done
    return result;
  } // end IF
  else if (race_in.count() > 1)
    result += ACE_TEXT_ALWAYS_CHAR("(");

  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < race_in.size();
       index++, race_index++)
  {
    if (race_in.test(index))
    {
      result += RPG_Character_RaceHelper::RPG_Character_RaceToString(ACE_static_cast(RPG_Character_Race,
                                                                                     race_index));
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end IF
  } // end FOR
  result.erase(--result.end());
  if (race_in.count() > 1)
    result += ACE_TEXT_ALWAYS_CHAR(")");

  return result;
}

const std::string
RPG_Character_Common_Tools::classToString(const RPG_Character_Class& class_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::classToString"));

  std::string result;

  result += RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(class_in.metaClass);
  if (!class_in.subClasses.empty())
    result += ACE_TEXT_ALWAYS_CHAR(" (");
  for (RPG_Character_SubClassesIterator_t iterator = class_in.subClasses.begin();
       iterator != class_in.subClasses.end();
       iterator++)
  {
    result += RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  } // end FOR
  if (!class_in.subClasses.empty())
  {
    result.erase(--result.end());
    result += ACE_TEXT_ALWAYS_CHAR(")");
  } // end IF

  return result;
}

const std::string
RPG_Character_Common_Tools::conditionToString(const RPG_Character_Conditions_t& condition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::conditionToString"));

  std::string result;

  if (condition_in.size() > 1)
    result += ACE_TEXT_ALWAYS_CHAR("(");
  for (RPG_Character_ConditionsIterator_t iterator = condition_in.begin();
       iterator != condition_in.end();
       iterator++)
  {
    result += RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  } // end FOR
  if (!condition_in.empty())
    result.erase(--result.end());
  if (condition_in.size() > 1)
    result += ACE_TEXT_ALWAYS_CHAR(")");

  return result;
}

const bool
RPG_Character_Common_Tools::match(const RPG_Character_Alignment& alignmentA_in,
                                  const RPG_Character_Alignment& alignmentB_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::match"));

  // check civics
  switch (alignmentA_in.civic)
  {
    case ALIGNMENTCIVIC_ANY:
    {
      // OK
      break;
    }
    default:
    {
      switch (alignmentB_in.civic)
      {
        case ALIGNMENTCIVIC_ANY:
        {
          // OK
          break;
        }
        default:
        {
          if (alignmentA_in.civic != alignmentB_in.civic)
            return false;

          // OK
          break;
        }
      } // end SWITCH
    }
  } // end SWITCH

  // check ethics
  switch (alignmentA_in.ethic)
  {
    case ALIGNMENTETHIC_ANY:
    {
      // OK
      break;
    }
    default:
    {
      switch (alignmentB_in.ethic)
      {
        case ALIGNMENTETHIC_ANY:
        {
          // OK
          break;
        }
        default:
        {
          if (alignmentA_in.ethic != alignmentB_in.ethic)
            return false;

          // OK
          break;
        }
      } // end SWITCH
    }
  } // end SWITCH

  return true;
}

const signed char
RPG_Character_Common_Tools::getAttributeAbilityModifier(const unsigned char& attributeAbility_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getAttributeAbilityModifier"));

  short int baseValue = -5;
  baseValue += ((attributeAbility_in & 0x1) == attributeAbility_in) ? ((attributeAbility_in - 1) >> 1)
                                                                    : (attributeAbility_in >> 1);

  return baseValue;
}

const bool
RPG_Character_Common_Tools::getAttributeCheck(const unsigned char& attributeAbilityScore_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getAttributeCheck"));

  int result = RPG_Chance_Common_Tools::getCheck(0);

  return (result >= attributeAbilityScore_in);
}

const RPG_Dice_DieType
RPG_Character_Common_Tools::getHitDie(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getHitDie"));

  switch (subClass_in)
  {
    case SUBCLASS_BARBARIAN:
    {
      return D_12;
    }
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_WARLORD:
    {
      return D_10;
    }
    case SUBCLASS_RANGER:
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
    case SUBCLASS_MONK:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
    case SUBCLASS_SHAMAN:
    {
      return D_8;
    }
    case SUBCLASS_THIEF:
    case SUBCLASS_BARD:
    {
      return D_6;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    case SUBCLASS_WARLOCK:
    {
      return D_4;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: \"%s\", aborting\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_DICE_DIETYPE_INVALID;
}

const RPG_Character_BaseAttackBonus_t
RPG_Character_Common_Tools::getBaseAttackBonus(const RPG_Common_SubClass& subClass_in,
                                               const unsigned char& classLevel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getBaseAttackBonus"));

  RPG_Character_BaseAttackBonus_t result;
  unsigned int baseAttackBonus = 0;

  switch (subClass_in)
  {
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_RANGER:
    case SUBCLASS_BARBARIAN:
    case SUBCLASS_WARLORD:
    {
      baseAttackBonus = classLevel_in;

      break;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    case SUBCLASS_WARLOCK:
    {
      baseAttackBonus = ((classLevel_in & 0x1) == classLevel_in) ? ((classLevel_in - 1) >> 1)
                                                                 : (classLevel_in >> 1);

      break;
    }
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
    case SUBCLASS_MONK:
    case SUBCLASS_THIEF:
    case SUBCLASS_BARD:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
    case SUBCLASS_SHAMAN:
    {
      baseAttackBonus = (classLevel_in - 1) - ((classLevel_in - 1) / 4);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: \"%s\", aborting\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

      break;
    }
  } // end SWITCH

  result.push_back(baseAttackBonus);

  // also, all classes gain an extra (-5) attack (maximum of 6) for a baseAttackBonus of +6, +11, +16, +21 and +26
  while ((baseAttackBonus >= 6) &&
         (result.size() < 6))
  {
    baseAttackBonus -= 5;
    result.push_back(baseAttackBonus);
  } // end WHILE

  return result;
}

const bool
RPG_Character_Common_Tools::isCasterClass(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::isCasterClass"));

  switch (subClass_in)
  {
    case SUBCLASS_BARD:
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
    case SUBCLASS_PALADIN:
    case SUBCLASS_RANGER:
    case SUBCLASS_SORCERER:
    case SUBCLASS_WIZARD:
    {
      return true;
    }
    default:
    {
      break;
    }
  } // end SWITCH

  return false;
}

RPG_Character_Player
RPG_Character_Common_Tools::generatePlayerCharacter()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::generatePlayerCharacter"));

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
    if (result[26 + i + 1] > 13)
      lowercase = false;
    else
      lowercase = true;

    name += ACE_static_cast(char, (lowercase ? 96 : 64) + result[i]); // 97 == 'a', 65 == 'A'
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
  gender = ACE_static_cast(RPG_Character_Gender, result.front());

  // step3: race
  RPG_Character_Race_t player_race(0);
  // *TODO*: consider allowing multi-race like Half-Elf etc.
  RPG_Character_Race race = RPG_CHARACTER_RACE_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_CHARACTER_RACE_MAX - 1),
                                  1,
                                  result);
  race = ACE_static_cast(RPG_Character_Race, result.front());
  player_race.flip(race - 1);

  // step4: class
  RPG_Character_Class player_class;
  player_class.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  RPG_Common_SubClass player_subclass = RPG_COMMON_SUBCLASS_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_COMMON_SUBCLASS_MAX - 1),
                                  1,
                                  result);
  player_subclass = ACE_static_cast(RPG_Common_SubClass, result.front());
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
  alignment.civic = ACE_static_cast(RPG_Character_AlignmentCivic, result.front());
  alignment.ethic = ACE_static_cast(RPG_Character_AlignmentEthic, result.back());

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
    skill = ACE_static_cast(RPG_Common_Skill, (result.front() - 1));
    iterator = skills.find(skill);
    if (iterator != skills.end())
    {
      (iterator->second)++;
    } // end IF
    else
    {
      skills.insert(std::make_pair(skill,
                                   ACE_static_cast(char, 1)));
    } // end ELSE
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
    feat = ACE_static_cast(RPG_Character_Feat, (result.front() - 1));

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
  unsigned char numKnownSpells = 0;
  unsigned char numSpells = 0;
  RPG_Magic_Spells_t knownSpells;
  RPG_Magic_SpellList_t spells;
  int numChosen = 0;
  RPG_Magic_Spells_t available;
  RPG_Magic_SpellsIterator_t available_iterator;
  RPG_Magic_CasterClassUnion casterClass;
  casterClass.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin();
       iterator != player_class.subClasses.end();
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
      if (isCasterClass(*iterator) &&
          ((*iterator == SUBCLASS_BARD) ||
           (*iterator == SUBCLASS_SORCERER)))
      {
        // make sure we have enough variety...
        ACE_ASSERT(numKnownSpells <= available.size());

        numChosen = 0;
        while (numChosen < numKnownSpells);
        {
          available_iterator = available.begin();
          result.clear();
          RPG_Dice::generateRandomNumbers(available.size(),
                                          1,
                                          result);
          std::advance(available_iterator, result.front() - 1);
          if (knownSpells.find(*available_iterator) != knownSpells.end())
            continue; // try again

          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("chose known spell #%d: \"%s\"\n"),
                     numChosen + 1,
                     RPG_Magic_Common_Tools::spellToName(*available_iterator).c_str()));

          knownSpells.insert(*available_iterator);
          numChosen++;
        } // end WHILE
      } // end IF

      // ... other magic-users get to prepare/memorize a number of (available) spells
      // ... again, apart from the Bard/Sorcerer, who don't need to prepare any spells ahead of time
      if (isCasterClass(*iterator) &&
          (*iterator != SUBCLASS_BARD) &&
          (*iterator != SUBCLASS_SORCERER))
      {
        // make sure we have enough variety...
        ACE_ASSERT(!available.empty());

        for (unsigned int j = 0;
             j < numSpells;
             j++)
        {
          available_iterator = available.begin();
          result.clear();
          RPG_Dice::generateRandomNumbers(available.size(),
                                          1,
                                          result);
          std::advance(available_iterator, result.front() - 1);

          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("chose prepared/memorized spell #%d: \"%s\"\n"),
                     j + 1,
                     RPG_Magic_Common_Tools::spellToName(*available_iterator).c_str()));

          spells.push_back(*available_iterator);
        } // end FOR
      } // end IF
    } // end FOR
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
      case SUBCLASS_WARLORD:
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
      case SUBCLASS_BARBARIAN:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        items.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_HIDE);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      case SUBCLASS_WIZARD:
      case SUBCLASS_SORCERER:
      case SUBCLASS_WARLOCK:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
      case SUBCLASS_CLERIC:
      case SUBCLASS_AVENGER:
      case SUBCLASS_INVOKER:
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
      case SUBCLASS_SHAMAN:
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
      case SUBCLASS_MONK:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT(current);
        items.insert(current->getID());

        break;
      }
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
               ACE_TEXT("caught std::bad_alloc, continuing\n")));
  }

  // step14: instantiate player character
  RPG_Character_Player player(name,
                              gender,
                              race,
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
                              RPG_CHARACTER_PLAYER_START_MONEY,
                              spells,
                              items);

  return player;
}

const unsigned int
RPG_Character_Common_Tools::restParty(RPG_Character_Party_t& party_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::restParty"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("party status:\n-------------\n")));
  int index = 1;
  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("#%d] \"%s\" (lvl: %d), HP: %d/%d --> %s\n"),
               index,
               (*iterator).getName().c_str(),
               (*iterator).getLevel(),
               (*iterator).getNumHitPoints(),
               (*iterator).getNumTotalHitPoints(),
               ((*iterator).hasCondition(CONDITION_NORMAL) ? ACE_TEXT_ALWAYS_CHAR("OK") : ACE_TEXT_ALWAYS_CHAR("DOWN"))));
  } // end FOR

  // check party status
  unsigned int diff = 0;
  unsigned int fraction = 0;
  unsigned int recoveryTime = 0;
  unsigned int maxRecoveryTime = 0;
  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    // *TODO*: consider dead/dying players !
    if ((*iterator).getNumHitPoints() < 0)
      continue;

    diff = ((*iterator).getNumTotalHitPoints() - (*iterator).getNumHitPoints());
    fraction = (diff % ((*iterator).getLevel() * 2));
    diff -= fraction;
    recoveryTime = ((diff / ((*iterator).getLevel() * 2)) + // days of complete bed-rest +
                    (fraction / (*iterator).getLevel()));   // days of good night's sleep
    if (recoveryTime > maxRecoveryTime)
      maxRecoveryTime = recoveryTime;
  } // end FOR

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("max. recovery time: %d days\n"),
             maxRecoveryTime));

  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    (*iterator).rest(REST_FULL,
                     (maxRecoveryTime * 24));
  } // end FOR

  return (maxRecoveryTime * 24 * 3600);
}
