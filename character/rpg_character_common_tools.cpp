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

#include "rpg_character_common_tools.h"

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

std::string
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

std::string
RPG_Character_Common_Tools::attributesToString(const RPG_Character_Attributes& attributes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::attributesToString"));

  std::string result;

  std::ostringstream converter;
  result = RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_STRENGTH);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter << static_cast<unsigned int>(attributes_in.strength);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_DEXTERITY);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << static_cast<unsigned int>(attributes_in.dexterity);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_CONSTITUTION);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << static_cast<unsigned int>(attributes_in.constitution);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_INTELLIGENCE);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << static_cast<unsigned int>(attributes_in.intelligence);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_WISDOM);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << static_cast<unsigned int>(attributes_in.wisdom);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(ATTRIBUTE_CHARISMA);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << static_cast<unsigned int>(attributes_in.charisma);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

std::string
RPG_Character_Common_Tools::raceToString(const RPG_Character_Race_t& races_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::raceToString"));

  std::string result;

  if (races_in.none())
  {
    result += RPG_Character_RaceHelper::RPG_Character_RaceToString(RACE_NONE);

    // done
    return result;
  } // end IF
  else if (races_in.count() > 1)
    result += ACE_TEXT_ALWAYS_CHAR("(");

  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < races_in.size();
       index++, race_index++)
  {
    if (races_in.test(index))
    {
      result += RPG_Character_RaceHelper::RPG_Character_RaceToString(static_cast<RPG_Character_Race>(race_index));
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end IF
  } // end FOR
  result.erase(--result.end());
  if (races_in.count() > 1)
    result += ACE_TEXT_ALWAYS_CHAR(")");

  return result;
}

std::string
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

std::string
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

bool
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

signed char
RPG_Character_Common_Tools::getAttributeAbilityModifier(const unsigned char& attributeAbility_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getAttributeAbilityModifier"));

  signed char baseValue = -5;
  baseValue += ((attributeAbility_in & 0x1) == attributeAbility_in) ? ((attributeAbility_in - 1) >> 1)
                                                                    : (attributeAbility_in >> 1);

  return baseValue;
}

bool
RPG_Character_Common_Tools::getAttributeCheck(const unsigned char& attributeAbilityScore_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getAttributeCheck"));

  int result = RPG_Chance_Common_Tools::getCheck(0);

  return (result >= attributeAbilityScore_in);
}

RPG_Dice_DieType
RPG_Character_Common_Tools::getHitDie(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::getHitDie"));

  switch (subClass_in)
  {
//     case SUBCLASS_BARBARIAN:
//     {
//       return D_12;
//     }
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
//     case SUBCLASS_WARLORD:
    {
      return D_10;
    }
    case SUBCLASS_RANGER:
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
//     case SUBCLASS_MONK:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
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
//     case SUBCLASS_WARLOCK:
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

RPG_Character_BaseAttackBonus_t
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
//     case SUBCLASS_BARBARIAN:
//     case SUBCLASS_WARLORD:
    {
      baseAttackBonus = classLevel_in;

      break;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
//     case SUBCLASS_WARLOCK:
    {
      baseAttackBonus = ((classLevel_in & 0x1) == classLevel_in) ? ((classLevel_in - 1) >> 1)
                                                                 : (classLevel_in >> 1);

      break;
    }
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
//     case SUBCLASS_MONK:
    case SUBCLASS_THIEF:
    case SUBCLASS_BARD:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
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
