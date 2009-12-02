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

#include "rpg_character_ability.h"
#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_attackform.h"
#include "rpg_character_attribute.h"
#include "rpg_character_condition.h"
#include "rpg_character_environment.h"
#include "rpg_character_feat.h"
#include "rpg_character_gender.h"
#include "rpg_character_metaclass.h"
#include "rpg_character_monstermetatype.h"
#include "rpg_character_monstersubtype.h"
#include "rpg_character_monsterweapon.h"
#include "rpg_character_organization.h"
#include "rpg_character_race.h"
#include "rpg_character_size.h"
#include "rpg_character_skill.h"
#include "rpg_character_subclass.h"

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Character_AbilityToStringTable_t RPG_Character_AbilityHelper::myRPG_Character_AbilityToStringTable;
RPG_Character_AlignmentCivicToStringTable_t RPG_Character_AlignmentCivicHelper::myRPG_Character_AlignmentCivicToStringTable;
RPG_Character_AlignmentEthicToStringTable_t RPG_Character_AlignmentEthicHelper::myRPG_Character_AlignmentEthicToStringTable;
RPG_Character_AttackFormToStringTable_t RPG_Character_AttackFormHelper::myRPG_Character_AttackFormToStringTable;
RPG_Character_AttributeToStringTable_t RPG_Character_AttributeHelper::myRPG_Character_AttributeToStringTable;
RPG_Character_ConditionToStringTable_t RPG_Character_ConditionHelper::myRPG_Character_ConditionToStringTable;
RPG_Character_EnvironmentToStringTable_t RPG_Character_EnvironmentHelper::myRPG_Character_EnvironmentToStringTable;
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
  RPG_Character_EnvironmentHelper::init();
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

  short baseValue = -5;
  baseValue += ((attributeAbility_in & 0x1) == attributeAbility_in) ? ((attributeAbility_in - 1) >> 1)
                                                                    : (attributeAbility_in >> 1);

  return baseValue;
}

const RPG_Chance_DiceType RPG_Character_Common_Tools::getHitDie(const RPG_Character_SubClass& subClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getHitDie"));

  switch (subClass_in)
  {
    case SUBCLASS_BARBARIAN:
    {
      return D_12;
    }
//     case SUBCLASS_WARLORD:
//     {
//       return D_12;
//     }
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    {
      return D_10;
    }
    case SUBCLASS_RANGER:
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
    case SUBCLASS_MONK:
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
    {
      return D_4;
    }
//     case SUBCLASS_WARLOCK:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
//     {
//       return D_4;
//     }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subClass: %d --> check implementation !, aborting\n"),
                 subClass_in));

      break;
    }
  } // end SWITCH

  return RPG_CHANCE_DICETYPE_INVALID;
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
    {
      baseAttackBonus = classLevel_in;

      break;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
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
    {
      baseAttackBonus = (classLevel_in - 1) - ((classLevel_in - 1) / 4);

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subClass: %d --> check implementation !, aborting\n"),
                 subClass_in));

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
