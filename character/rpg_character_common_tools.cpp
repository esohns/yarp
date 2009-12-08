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

#include "rpg_character_gender.h"
#include "rpg_character_race.h"
#include "rpg_character_metaclass.h"
#include "rpg_character_subclass.h"
#include "rpg_character_attribute.h"
#include "rpg_character_condition.h"
#include "rpg_character_ability.h"
#include "rpg_character_monstermetatype.h"
#include "rpg_character_monstersubtype.h"
#include "rpg_character_monsterweapon.h"
#include "rpg_character_attackform.h"
#include "rpg_character_size.h"
#include "rpg_character_skill.h"
#include "rpg_character_feat.h"
#include "rpg_character_plane.h"
#include "rpg_character_terrain.h"
#include "rpg_character_climate.h"
#include "rpg_character_organization.h"
#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"

#include "rpg_character_race_common.h"
#include "rpg_character_skills_common_tools.h"

#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

#include <rpg_dice.h>
#include <rpg_chance_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>
#include <cctype>

// init statics
RPG_Character_AbilityToStringTable_t RPG_Character_AbilityHelper::myRPG_Character_AbilityToStringTable;
RPG_Character_AlignmentCivicToStringTable_t RPG_Character_AlignmentCivicHelper::myRPG_Character_AlignmentCivicToStringTable;
RPG_Character_AlignmentEthicToStringTable_t RPG_Character_AlignmentEthicHelper::myRPG_Character_AlignmentEthicToStringTable;
RPG_Character_AttackFormToStringTable_t RPG_Character_AttackFormHelper::myRPG_Character_AttackFormToStringTable;
RPG_Character_AttributeToStringTable_t RPG_Character_AttributeHelper::myRPG_Character_AttributeToStringTable;
RPG_Character_ConditionToStringTable_t RPG_Character_ConditionHelper::myRPG_Character_ConditionToStringTable;
RPG_Character_PlaneToStringTable_t RPG_Character_PlaneHelper::myRPG_Character_PlaneToStringTable;
RPG_Character_ClimateToStringTable_t RPG_Character_ClimateHelper::myRPG_Character_ClimateToStringTable;
RPG_Character_TerrainToStringTable_t RPG_Character_TerrainHelper::myRPG_Character_TerrainToStringTable;
RPG_Character_FeatToStringTable_t RPG_Character_FeatHelper::myRPG_Character_FeatToStringTable;
RPG_Character_GenderToStringTable_t RPG_Character_GenderHelper::myRPG_Character_GenderToStringTable;
RPG_Character_MetaClassToStringTable_t RPG_Character_MetaClassHelper::myRPG_Character_MetaClassToStringTable;
RPG_Character_MonsterMetaTypeToStringTable_t RPG_Character_MonsterMetaTypeHelper::myRPG_Character_MonsterMetaTypeToStringTable;
RPG_Character_MonsterSubTypeToStringTable_t RPG_Character_MonsterSubTypeHelper::myRPG_Character_MonsterSubTypeToStringTable;
RPG_Character_MonsterWeaponToStringTable_t RPG_Character_MonsterWeaponHelper::myRPG_Character_MonsterWeaponToStringTable;
RPG_Character_OrganizationToStringTable_t RPG_Character_OrganizationHelper::myRPG_Character_OrganizationToStringTable;
RPG_Character_RaceToStringTable_t RPG_Character_RaceHelper::myRPG_Character_RaceToStringTable;
RPG_Character_SizeToStringTable_t RPG_Character_SizeHelper::myRPG_Character_SizeToStringTable;
RPG_Character_SkillToStringTable_t RPG_Character_SkillHelper::myRPG_Character_SkillToStringTable;
RPG_Character_SubClassToStringTable_t RPG_Character_SubClassHelper::myRPG_Character_SubClassToStringTable;

void RPG_Character_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::initStringConversionTables"));

  RPG_Character_AbilityHelper::init();
  RPG_Character_AlignmentCivicHelper::init();
  RPG_Character_AlignmentEthicHelper::init();
  RPG_Character_AttackFormHelper::init();
  RPG_Character_AttributeHelper::init();
  RPG_Character_ConditionHelper::init();
  RPG_Character_PlaneHelper::init();
  RPG_Character_ClimateHelper::init();
  RPG_Character_TerrainHelper::init();
  RPG_Character_FeatHelper::init();
  RPG_Character_GenderHelper::init();
  RPG_Character_MetaClassHelper::init();
  RPG_Character_MonsterMetaTypeHelper::init();
  RPG_Character_MonsterSubTypeHelper::init();
  RPG_Character_MonsterWeaponHelper::init();
  RPG_Character_OrganizationHelper::init();
  RPG_Character_RaceHelper::init();
  RPG_Character_SizeHelper::init();
  RPG_Character_SkillHelper::init();
  RPG_Character_SubClassHelper::init();

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

const std::string RPG_Character_Common_Tools::attributesToString(const RPG_Character_Attributes& attributes_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::attributesToString"));

  std::string result;
  std::stringstream str;
  result = RPG_Character_AttributeHelper::RPG_Character_AttributeToString(ATTRIBUTE_STRENGTH);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.strength);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Character_AttributeHelper::RPG_Character_AttributeToString(ATTRIBUTE_DEXTERITY);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.dexterity);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Character_AttributeHelper::RPG_Character_AttributeToString(ATTRIBUTE_CONSTITUTION);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.constitution);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Character_AttributeHelper::RPG_Character_AttributeToString(ATTRIBUTE_INTELLIGENCE);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.intelligence);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Character_AttributeHelper::RPG_Character_AttributeToString(ATTRIBUTE_WISDOM);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.wisdom);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += RPG_Character_AttributeHelper::RPG_Character_AttributeToString(ATTRIBUTE_CHARISMA);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.charisma);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const short int RPG_Character_Common_Tools::getAttributeAbilityModifier(const unsigned char& attributeAbility_in)
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

const RPG_Dice_DieType RPG_Character_Common_Tools::getHitDie(const RPG_Character_SubClass& subClass_in)
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
                 RPG_Character_SubClassHelper::RPG_Character_SubClassToString(subClass_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_DICE_DIETYPE_INVALID;
}

const RPG_Character_BaseAttackBonus_t RPG_Character_Common_Tools::getBaseAttackBonus(const RPG_Character_SubClass& subClass_in,
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
                 RPG_Character_SubClassHelper::RPG_Character_SubClassToString(subClass_in).c_str()));

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
  player_class.subClass = RPG_CHARACTER_SUBCLASS_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers((RPG_CHARACTER_SUBCLASS_MAX - 1),
                                  1,
                                  result);
  player_class.subClass = ACE_static_cast(RPG_Character_SubClass, result.front());
  player_class.metaClass = RPG_Character_Class_Common_Tools::subClassToMetaClass(player_class.subClass);

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
  RPG_Character_Skills_Common_Tools::getSkillPoints(player_class.subClass,
                                                    RPG_Character_Common_Tools::getAttributeAbilityModifier(attributes.intelligence),
                                                    initialSkillPoints);
  RPG_Character_SkillsIterator_t iterator;
  RPG_Character_Skill skill = RPG_CHARACTER_SKILL_INVALID;
  for (unsigned int i = 0;
       i < initialSkillPoints;
       i++)
  {
    result.clear();
    RPG_Dice::generateRandomNumbers(RPG_CHARACTER_SKILL_MAX,
                                    1,
                                    result);
    skill = ACE_static_cast(RPG_Character_Skill, (result.front() - 1));
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
                                                          player_class.subClass,
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
                                                                   player_class.subClass,
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

  // step9: (initial) Hit Points
  unsigned short int hitpoints = 0;
  roll.numDice = 1;
  roll.typeDice = RPG_Character_Common_Tools::getHitDie(player_class.subClass);
  roll.modifier = 0;
  result.clear();
  RPG_Dice::simulateRoll(roll,
                         1,
                         result);
  hitpoints = result[0];

  // step10: (initial) set of items
  // *TODO*: somehow generate these at random too ?
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
    case SUBCLASS_WARLORD:
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
    case SUBCLASS_WARLOCK:
    {
      weapon = new RPG_Item_Weapon(TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);

      items.push_back(weapon->getID());

      break;
    }
    case SUBCLASS_CLERIC:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
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
    case SUBCLASS_SHAMAN:
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
      std::string subClass_string = RPG_Character_SubClassHelper::RPG_Character_SubClassToString(player_class.subClass);
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid player sub-class \"%s\", continuing\n"),
                 subClass_string.c_str()));

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
                              0,
                              hitpoints,
                              0,
                              items);

  return player;
}
