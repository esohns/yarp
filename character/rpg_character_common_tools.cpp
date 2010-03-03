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

#include "rpg_character_race_common.h"
#include "rpg_character_skills_common_tools.h"

#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

#include <rpg_magic_dictionary.h>
#include <rpg_magic_common_tools.h>

#include <rpg_common_defines.h>
#include <rpg_common_camp.h>

#include <rpg_dice.h>
#include <rpg_chance_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>
#include <cctype>

// init statics
RPG_Character_GenderToStringTable_t RPG_Character_GenderHelper::myRPG_Character_GenderToStringTable;
RPG_Character_RaceToStringTable_t RPG_Character_RaceHelper::myRPG_Character_RaceToStringTable;
RPG_Character_MetaClassToStringTable_t RPG_Character_MetaClassHelper::myRPG_Character_MetaClassToStringTable;
RPG_Character_AbilityToStringTable_t RPG_Character_AbilityHelper::myRPG_Character_AbilityToStringTable;
RPG_Character_FeatToStringTable_t RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable;
RPG_Character_PlaneToStringTable_t RPG_Character_PlaneHelper::myRPG_Character_PlaneToStringTable;
RPG_Character_TerrainToStringTable_t RPG_Character_TerrainHelper::myRPG_Character_TerrainToStringTable;
RPG_Character_ClimateToStringTable_t RPG_Character_ClimateHelper::myRPG_Character_ClimateToStringTable;
RPG_Character_AlignmentCivicToStringTable_t RPG_Character_AlignmentCivicHelper::myRPG_Character_AlignmentCivicToStringTable;
RPG_Character_AlignmentEthicToStringTable_t RPG_Character_AlignmentEthicHelper::myRPG_Character_AlignmentEthicToStringTable;
RPG_Character_EquipmentSlotToStringTable_t RPG_Character_EquipmentSlotHelper::myRPG_Character_EquipmentSlotToStringTable;
RPG_Character_OffHandToStringTable_t RPG_Character_OffHandHelper::myRPG_Character_OffHandToStringTable;

void RPG_Character_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::initStringConversionTables"));

  RPG_Character_GenderHelper::init();
  RPG_Character_RaceHelper::init();
  RPG_Character_MetaClassHelper::init();
  RPG_Character_AbilityHelper::init();
  RPG_Character_FeatHelper::init();
  RPG_Character_PlaneHelper::init();
  RPG_Character_TerrainHelper::init();
  RPG_Character_ClimateHelper::init();
  RPG_Character_AlignmentCivicHelper::init();
  RPG_Character_AlignmentEthicHelper::init();
  RPG_Character_EquipmentSlotHelper::init();
  RPG_Character_OffHandHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Common_Tools: initialized string conversion tables...\n")));
}

const std::string RPG_Character_Common_Tools::alignmentToString(const RPG_Character_Alignment& alignment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::alignmentToString"));

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

const std::string RPG_Character_Common_Tools::environmentToString(const RPG_Character_Environment& environment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::environmentToString"));

  std::string result;

  result += RPG_Character_TerrainHelper::RPG_Character_TerrainToString(environment_in.terrain);
  if (environment_in.climate != RPG_CHARACTER_CLIMATE_INVALID)
  {
    result += ACE_TEXT_ALWAYS_CHAR("|");
    result += RPG_Character_ClimateHelper::RPG_Character_ClimateToString(environment_in.climate);
  } // end IF

  return result;
}

const std::string RPG_Character_Common_Tools::attributesToString(const RPG_Character_Attributes& attributes_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::attributesToString"));

  std::string result;
  std::stringstream str;
  result = RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_STRENGTH);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.strength);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_DEXTERITY);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.dexterity);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_CONSTITUTION);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.constitution);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_INTELLIGENCE);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.intelligence);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_WISDOM);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.wisdom);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_CHARISMA);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.charisma);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string RPG_Character_Common_Tools::classToString(const RPG_Character_Class& class_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::classToString"));

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

const std::string RPG_Character_Common_Tools::conditionToString(const RPG_Character_Conditions_t& condition_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::conditionToString"));

  std::string result;
  for (RPG_Character_ConditionsIterator_t iterator = condition_in.begin();
       iterator != condition_in.end();
       iterator++)
  {
    result += RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR(" | ");
  } // end FOR
  if (!result.empty())
    result.erase(--result.end());

  return result;
}

const signed char RPG_Character_Common_Tools::getAttributeAbilityModifier(const unsigned char& attributeAbility_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getAttributeAbilityModifier"));

  short int baseValue = -5;
  baseValue += ((attributeAbility_in & 0x1) == attributeAbility_in) ? ((attributeAbility_in - 1) >> 1)
                                                                    : (attributeAbility_in >> 1);

  return baseValue;
}

const bool RPG_Character_Common_Tools::getAttributeCheck(const unsigned char& attributeAbilityScore_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getAttributeCheck"));

  int result = RPG_Chance_Common_Tools::getCheck(0);

  return (result >= attributeAbilityScore_in);
}

const RPG_Dice_DieType RPG_Character_Common_Tools::getHitDie(const RPG_Common_SubClass& subClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getHitDie"));

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
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: \"%s\" --> check implementation !, aborting\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_DICE_DIETYPE_INVALID;
}

const RPG_Character_BaseAttackBonus_t RPG_Character_Common_Tools::getBaseAttackBonus(const RPG_Common_SubClass& subClass_in,
                                                                                     const unsigned char& classLevel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getBaseAttackBonus"));

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
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: \"%s\" --> check implementation !, aborting\n"),
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

const bool RPG_Character_Common_Tools::isCasterClass(const RPG_Common_SubClass& subClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::isCasterClass"));

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
//       // debug info
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("invalid subclass: \"%s\" --> check implementation !, aborting\n"),
//                  RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

      break;
    }
  } // end SWITCH

  return false;
}

const RPG_Character_Plane RPG_Character_Common_Tools::terrainToPlane(const RPG_Character_Terrain& terrain_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::terrainToPlane"));

  switch (terrain_in)
  {
    case TERRAIN_UNDERGROUND:
    case TERRAIN_PLAINS:
    case TERRAIN_FORESTS:
    case TERRAIN_HILLS:
    case TERRAIN_MOUNTAINS:
    case TERRAIN_DESERTS:
    case TERRAIN_MATERIALPLANE_ANY:
    {
      return PLANE_MATERIAL;
    }
    case TERRAIN_TRANSITIVEPLANE_ASTRAL:
    case TERRAIN_TRANSITIVEPLANE_ETHERAL:
    case TERRAIN_TRANSITIVEPLANE_SHADOW:
    case TERRAIN_TRANSITIVEPLANE_ANY:
    {
      return PLANE_TRANSITIVE;
    }
    case TERRAIN_INNERPLANE_AIR:
    case TERRAIN_INNERPLANE_EARTH:
    case TERRAIN_INNERPLANE_FIRE:
    case TERRAIN_INNERPLANE_WATER:
    case TERRAIN_INNERPLANE_POSITIVE:
    case TERRAIN_INNERPLANE_NEGATIVE:
    case TERRAIN_INNERPLANE_ANY:
    {
      return PLANE_INNER;
    }
    case TERRAIN_OUTERPLANE_LAWFUL_ANY:
    case TERRAIN_OUTERPLANE_CHAOTIC_ANY:
    case TERRAIN_OUTERPLANE_GOOD_ANY:
    case TERRAIN_OUTERPLANE_EVIL_ANY:
    case TERRAIN_OUTERPLANE_LAWFUL_GOOD:
    case TERRAIN_OUTERPLANE_LAWFUL_EVIL:
    case TERRAIN_OUTERPLANE_CHAOTIC_GOOD:
    case TERRAIN_OUTERPLANE_CHAOTIC_EVIL:
    case TERRAIN_OUTERPLANE_NEUTRAL:
    case TERRAIN_OUTERPLANE_MILD_ANY:
    case TERRAIN_OUTERPLANE_STRONG_ANY:
    case TERRAIN_OUTERPLANE_ANY:
    {
      return PLANE_OUTER;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid terrain: \"%s\" --> check implementation !, aborting\n"),
                 RPG_Character_TerrainHelper::RPG_Character_TerrainToString(terrain_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_CHARACTER_PLANE_INVALID;
}

const RPG_Character_Player RPG_Character_Common_Tools::generatePlayerCharacter()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::generatePlayerCharacter"));

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

//   // debug info
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
  RPG_Character_PlayerRace player_race(0);
  // *TODO*: consider allowing multi-race like Half-Elf etc.
  RPG_Character_Race race = RPG_CHARACTER_RACE_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_CHARACTER_RACE_MAX - 1),
                                  1,
                                  result);
  race = ACE_static_cast(RPG_Character_Race, result.front());
  player_race.set(race - 1);

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
  roll.modifier = -2; // add +1 if result is 0 --> stats interval 1-18
  // make sure the result is somewhat balanced (average == 6 * 9)...
  // *IMPORTANT NOTE*: INT must be > 2 (smaller values are reserved for animals...)
  int sum = 0;
  do
  {
    result.clear();
    RPG_Dice::simulateRoll(roll,
                           6,
                           result);
    sum = result[0] + result[1] + result[2] + result[3] + result[4] + result[5];
  } while ((sum <= 54) ||
           (*(std::min_element(result.begin(), result.end())) <= 9) ||
           (result[3] < 3)); // Note: this is already covered by the last case...
  for (int i = 0;
       i < 6;
       i++, p++)
  {
    *p = result[i];
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
  // our players start with maxed HP...
  hitpoints = RPG_Character_Common_Tools::getHitDie(player_subclass);

  // step11: (initial) set of spells
  unsigned char numKnownSpells = 0;
  unsigned char numSpells = 0;
  RPG_Magic_Spells_t knownSpells;
  RPG_Magic_SpellList_t spells;
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
                                                   0,
                                                   i,
                                                   numSpells,
                                                   numKnownSpells);

      // get list of available spells
      casterClass.subclass = *iterator;
      available = RPG_MAGIC_DICTIONARY_SINGLETON::instance()->getSpells(casterClass,
                                                                        i);

      // only Bards and Sorcerers have a limited set of "known" spells to choose from
      if ((*iterator == SUBCLASS_BARD) ||
          (*iterator == SUBCLASS_SORCERER))
      {
        do
        {
          available_iterator = available.begin();
          result.clear();
          RPG_Dice::generateRandomNumbers(available.size(),
                                          1,
                                          result);
          for (int j = 0;
               j <= (result.front() - 1);
               j++)
            available_iterator++;

          if (knownSpells.find(*available_iterator) != knownSpells.end())
            continue; // try again

          knownSpells.insert(*available_iterator);
        } while (knownSpells.size() < numKnownSpells);
      } // end IF

      // ... everybody else simply gets to prepare/memorize a number of (available) spells
      // ... apart from the Bard, who doesn't need to prepare any spells ahead of time
      if (isCasterClass(*iterator) &&
          (*iterator != SUBCLASS_BARD))
      {
        for (unsigned int j = 0;
              j < numSpells;
              j++)
        {
          available_iterator = available.begin();
          result.clear();
          RPG_Dice::generateRandomNumbers(available.size(),
                                          1,
                                          result);
          for (int j = 0;
                j <= (result.front() - 1);
                j++)
            available_iterator++;

          spells.push_back(*available_iterator);
        } // end FOR
      } // end IF
    } // end FOR
  } // end FOR

  // step12: (initial) set of items
  // *TODO*: somehow generate these at random too ?
  RPG_Item_List_t items;
  RPG_Item_Armor* armor = NULL;
  RPG_Item_Armor* shield = NULL;
  RPG_Item_Weapon* weapon = NULL;
  RPG_Item_Weapon* bow = NULL;
  switch (player_subclass)
  {
    case SUBCLASS_FIGHTER:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      armor  = new RPG_Item_Armor(ARMOR_MAIL_SPLINT);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_HEAVY_WOODEN);

      items.insert(weapon->getID());
      items.insert(armor->getID());
      items.insert(shield->getID());

      break;
    }
    case SUBCLASS_PALADIN:
    case SUBCLASS_WARLORD:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      armor  = new RPG_Item_Armor(ARMOR_PLATE_FULL);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_HEAVY_STEEL);

      items.insert(weapon->getID());
      items.insert(armor->getID());
      items.insert(shield->getID());

      break;
    }
    case SUBCLASS_RANGER:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      bow    = new RPG_Item_Weapon(RANGED_WEAPON_BOW_LONG);
      armor  = new RPG_Item_Armor(ARMOR_HIDE);

      items.insert(weapon->getID());
      items.insert(bow->getID());
      items.insert(armor->getID());

      break;
    }
    case SUBCLASS_BARBARIAN:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
      armor  = new RPG_Item_Armor(ARMOR_HIDE);

      items.insert(weapon->getID());
      items.insert(armor->getID());

      break;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    case SUBCLASS_WARLOCK:
    {
      weapon = new RPG_Item_Weapon(TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);

      items.insert(weapon->getID());

      break;
    }
    case SUBCLASS_CLERIC:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
    {
      weapon = new RPG_Item_Weapon(ONE_HANDED_MELEE_WEAPON_MACE_HEAVY);
      armor  = new RPG_Item_Armor(ARMOR_MAIL_CHAIN);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_HEAVY_WOODEN);

      items.insert(weapon->getID());
      items.insert(armor->getID());
      items.insert(shield->getID());

      break;
    }
    case SUBCLASS_DRUID:
    case SUBCLASS_SHAMAN:
    {
      weapon = new RPG_Item_Weapon(LIGHT_MELEE_WEAPON_SICKLE);
      armor  = new RPG_Item_Armor(ARMOR_HIDE);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_LIGHT_WOODEN);

      items.insert(weapon->getID());
      items.insert(armor->getID());
      items.insert(shield->getID());

      break;
    }
    case SUBCLASS_MONK:
    {
      weapon = new RPG_Item_Weapon(TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);

      items.insert(weapon->getID());

      break;
    }
    case SUBCLASS_THIEF:
    case SUBCLASS_BARD:
    {
      weapon = new RPG_Item_Weapon(LIGHT_MELEE_WEAPON_SWORD_SHORT);
      armor  = new RPG_Item_Armor(ARMOR_LEATHER);
      shield  = new RPG_Item_Armor(ARMOR_SHIELD_LIGHT_STEEL);

      items.insert(weapon->getID());
      items.insert(armor->getID());
      items.insert(shield->getID());

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid player sub-class \"%s\", continuing\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(player_subclass).c_str()));

      break;
    }
  } // end SWITCH

  // step11: instantiate player character
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
                              0,
                              hitpoints,
                              0,
                              knownSpells,
                              items);

  return player;
}

const unsigned int RPG_Character_Common_Tools::restParty(RPG_Character_Party_t& party_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::restParty"));

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
