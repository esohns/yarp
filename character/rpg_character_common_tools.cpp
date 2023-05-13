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

#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <limits>

#include "ace/Log_Msg.h"

#include "rpg_common_camp.h"
#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_dice.h"

#include "rpg_chance_common_tools.h"

#include "rpg_magic_common_tools.h"
#include "rpg_magic_dictionary.h"

#include "rpg_item_common_tools.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"

#include "rpg_character_class_common_tools.h"
#include "rpg_character_defines.h"
#include "rpg_character_race_common.h"
#include "rpg_character_skills_common_tools.h"

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
RPG_Character_EncumbranceToStringTable_t RPG_Character_EncumbranceHelper::myRPG_Character_EncumbranceToStringTable;

RPG_Character_Common_Tools::RPG_Character_EncumbranceTable_t RPG_Character_Common_Tools::myEncumbranceTable;

void
RPG_Character_Common_Tools::initialize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::initialize"));

  initializeStringConversionTables ();
  initializeEncumbranceTable ();
  RPG_Character_Skills_Common_Tools::initialize ();
}

void
RPG_Character_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::initStringConversionTables"));

  RPG_Character_GenderHelper::init ();
  RPG_Character_RaceHelper::init ();
  RPG_Character_MetaClassHelper::init ();
  RPG_Character_AbilityHelper::init ();
  RPG_Character_FeatHelper::init ();
  RPG_Character_AlignmentCivicHelper::init ();
  RPG_Character_AlignmentEthicHelper::init ();
  RPG_Character_EquipmentSlotHelper::init ();
  RPG_Character_OffHandHelper::init ();
  RPG_Character_EncumbranceHelper::init ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Character_Common_Tools: initialized string conversion tables...\n")));
}

void
RPG_Character_Common_Tools::initializeEncumbranceTable()
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::initEncumbranceTable"));

  RPG_Character_Common_Tools::myEncumbranceTable.clear ();

  RPG_Character_EncumbranceEntry_t entry;
  entry.light  = 3;
  entry.medium = 6;
  entry.heavy  = 10;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (1, entry));
  entry.light  = 6;
  entry.medium = 13;
  entry.heavy  = 20;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (2, entry));
  entry.light  = 10;
  entry.medium = 20;
  entry.heavy  = 30;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (3, entry));
  entry.light  = 13;
  entry.medium = 26;
  entry.heavy  = 40;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (4, entry));
  entry.light  = 16;
  entry.medium = 33;
  entry.heavy  = 50;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (5, entry));
  entry.light  = 20;
  entry.medium = 40;
  entry.heavy  = 60;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (6, entry));
  entry.light  = 23;
  entry.medium = 46;
  entry.heavy  = 70;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (7, entry));
  entry.light  = 26;
  entry.medium = 53;
  entry.heavy  = 80;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (8, entry));
  entry.light  = 30;
  entry.medium = 60;
  entry.heavy  = 90;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (9, entry));
  entry.light  = 33;
  entry.medium = 66;
  entry.heavy  = 100;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (10, entry));
  entry.light  = 38;
  entry.medium = 76;
  entry.heavy  = 115;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (11, entry));
  entry.light  = 43;
  entry.medium = 86;
  entry.heavy  = 130;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (12, entry));
  entry.light  = 50;
  entry.medium = 100;
  entry.heavy  = 150;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (13, entry));
  entry.light  = 58;
  entry.medium = 116;
  entry.heavy  = 175;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (14, entry));
  entry.light  = 66;
  entry.medium = 133;
  entry.heavy  = 200;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (15, entry));
  entry.light  = 76;
  entry.medium = 153;
  entry.heavy  = 230;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (16, entry));
  entry.light  = 86;
  entry.medium = 173;
  entry.heavy  = 260;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (17, entry));
  entry.light  = 100;
  entry.medium = 200;
  entry.heavy  = 300;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (18, entry));
  entry.light  = 116;
  entry.medium = 233;
  entry.heavy  = 350;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (19, entry));
  entry.light  = 133;
  entry.medium = 266;
  entry.heavy  = 400;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (20, entry));
  entry.light  = 153;
  entry.medium = 306;
  entry.heavy  = 460;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (21, entry));
  entry.light  = 173;
  entry.medium = 346;
  entry.heavy  = 520;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (22, entry));
  entry.light  = 200;
  entry.medium = 400;
  entry.heavy  = 600;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (23, entry));
  entry.light  = 233;
  entry.medium = 466;
  entry.heavy  = 700;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (24, entry));
  entry.light  = 266;
  entry.medium = 533;
  entry.heavy  = 800;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (25, entry));
  entry.light  = 306;
  entry.medium = 613;
  entry.heavy  = 920;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (26, entry));
  entry.light  = 346;
  entry.medium = 693;
  entry.heavy  = 1040;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (27, entry));
  entry.light  = 400;
  entry.medium = 800;
  entry.heavy  = 1200;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (28, entry));
  entry.light  = 466;
  entry.medium = 933;
  entry.heavy  = 1400;
  RPG_Character_Common_Tools::myEncumbranceTable.insert (std::make_pair (29, entry));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Character_Common_Tools: initialized encumbrance table...\n")));
}

std::string
RPG_Character_Common_Tools::toString (const RPG_Character_Alignment& alignment_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::toString"));

  std::string result;

  result += RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(alignment_in.civic);
  result += ACE_TEXT_ALWAYS_CHAR(" | ");
  result += RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(alignment_in.ethic);

  // "Neutral" "Neutral" --> "True Neutral"
  if ((alignment_in.civic == ALIGNMENTCIVIC_NEUTRAL) &&
      (alignment_in.ethic == ALIGNMENTETHIC_NEUTRAL))
  {
    result = ACE_TEXT_ALWAYS_CHAR ("True Neutral");
  } // end IF

  return result;
}

std::string
RPG_Character_Common_Tools::toString (const RPG_Character_Attributes& attributes_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::toString"));

  std::string result;

  std::ostringstream converter;
  result =
    RPG_Common_AttributeHelper::RPG_Common_AttributeToString (ATTRIBUTE_STRENGTH);
  result += ACE_TEXT_ALWAYS_CHAR (": ");
  converter << static_cast<unsigned int> (attributes_in.strength);
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result +=
    RPG_Common_AttributeHelper::RPG_Common_AttributeToString (ATTRIBUTE_DEXTERITY);
  result += ACE_TEXT_ALWAYS_CHAR (": ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << static_cast<unsigned int> (attributes_in.dexterity);
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result +=
    RPG_Common_AttributeHelper::RPG_Common_AttributeToString (ATTRIBUTE_CONSTITUTION);
  result += ACE_TEXT_ALWAYS_CHAR (": ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << static_cast<unsigned int> (attributes_in.constitution);
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result +=
    RPG_Common_AttributeHelper::RPG_Common_AttributeToString (ATTRIBUTE_INTELLIGENCE);
  result += ACE_TEXT_ALWAYS_CHAR (": ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << static_cast<unsigned int> (attributes_in.intelligence);
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result +=
    RPG_Common_AttributeHelper::RPG_Common_AttributeToString (ATTRIBUTE_WISDOM);
  result += ACE_TEXT_ALWAYS_CHAR (": ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << static_cast<unsigned int> (attributes_in.wisdom);
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  result +=
    RPG_Common_AttributeHelper::RPG_Common_AttributeToString (ATTRIBUTE_CHARISMA);
  result += ACE_TEXT_ALWAYS_CHAR (": ");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << static_cast<unsigned int> (attributes_in.charisma);
  result += converter.str ();
  result += ACE_TEXT_ALWAYS_CHAR ("\n");

  return result;
}

std::string
RPG_Character_Common_Tools::toString (const RPG_Character_Race_t& races_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::toString"));

  std::string result;

  if (races_in.none ())
  {
    result += RPG_Character_RaceHelper::RPG_Character_RaceToString (RACE_NONE);
    return result;
  } // end IF

  if (races_in.count () > 1)
    result += ACE_TEXT_ALWAYS_CHAR ("(");

  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < races_in.size ();
       index++, race_index++)
  {
    if (races_in.test (index))
    {
      result += RPG_Character_RaceHelper::RPG_Character_RaceToString (static_cast<RPG_Character_Race> (race_index));
      result += ACE_TEXT_ALWAYS_CHAR ("|");
    } // end IF
  } // end FOR
  result.erase (--result.end ());

  if (races_in.count () > 1)
    result += ACE_TEXT_ALWAYS_CHAR (")");

  return result;
}

std::string
RPG_Character_Common_Tools::toString (const RPG_Character_Class& class_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::toString"));

  std::string result;

  result +=
    RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString (class_in.metaClass);

  if (!class_in.subClasses.empty ())
    result += ACE_TEXT_ALWAYS_CHAR(" (");

  for (RPG_Character_SubClassesIterator_t iterator = class_in.subClasses.begin ();
       iterator != class_in.subClasses.end ();
       iterator++)
  {
    result += RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator);
    result += ACE_TEXT_ALWAYS_CHAR ("|");
  } // end FOR

  if (!class_in.subClasses.empty ())
  {
    result.erase (--result.end ());
    result += ACE_TEXT_ALWAYS_CHAR (")");
  } // end IF

  return result;
}

std::string
RPG_Character_Common_Tools::toString (const RPG_Character_Conditions_t& condition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Common_Tools::toString"));

  std::string result;

  if (condition_in.size () > 1)
    result += ACE_TEXT_ALWAYS_CHAR ("(");

  for (RPG_Character_ConditionsIterator_t iterator = condition_in.begin ();
       iterator != condition_in.end ();
       iterator++)
  {
    result += RPG_Common_ConditionHelper::RPG_Common_ConditionToString (*iterator);
    result += ACE_TEXT_ALWAYS_CHAR ("|");
  } // end FOR
  if (!condition_in.empty ())
    result.erase (--result.end ());

  if (condition_in.size () > 1)
    result += ACE_TEXT_ALWAYS_CHAR (")");

  return result;
}

bool
RPG_Character_Common_Tools::match (const RPG_Character_Alignment& alignmentA_in,
                                   const RPG_Character_Alignment& alignmentB_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::match"));

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
RPG_Character_Common_Tools::getAttributeAbilityModifier (unsigned char attributeAbility_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getAttributeAbilityModifier"));

  signed char baseValue = -5;
  baseValue += ((attributeAbility_in & 0x1) == attributeAbility_in) ? ((attributeAbility_in - 1) >> 1)
                                                                    : (attributeAbility_in >> 1);

  return baseValue;
}

bool
RPG_Character_Common_Tools::getAttributeCheck (unsigned char attributeAbilityScore_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getAttributeCheck"));

  int result = RPG_Chance_Common_Tools::getCheck (0);

  return (result >= attributeAbilityScore_in);
}

enum RPG_Dice_DieType
RPG_Character_Common_Tools::getHitDie (enum RPG_Common_SubClass subClass_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getHitDie"));

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
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid subclass: \"%s\", aborting\n"),
                  RPG_Common_SubClassHelper::RPG_Common_SubClassToString (subClass_in).c_str()));
      break;
    }
  } // end SWITCH

  return RPG_DICE_DIETYPE_INVALID;
}

RPG_Character_BaseAttackBonus_t
RPG_Character_Common_Tools::getBaseAttackBonus (enum RPG_Common_SubClass subClass_in,
                                                unsigned char classLevel_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getBaseAttackBonus"));

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
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid subclass: \"%s\", aborting\n"),
                  RPG_Common_SubClassHelper::RPG_Common_SubClassToString (subClass_in).c_str ()));
      break;
    }
  } // end SWITCH

  result.push_back (baseAttackBonus);

  // also, all classes gain an extra (-5) attack (maximum of 6) for a baseAttackBonus of +6, +11, +16, +21 and +26
  while ((baseAttackBonus >= 6) &&
         (result.size() < 6))
  {
    baseAttackBonus -= 5;
    result.push_back (baseAttackBonus);
  } // end WHILE

  return result;
}

RPG_Character_Encumbrance
RPG_Character_Common_Tools::getEncumbrance (unsigned char strength_in,
                                            enum RPG_Common_Size size_in,
                                            unsigned short weight_in,
                                            bool isBiped_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getEncumbrance"));

  // step1: find corresponding (base) table entry
  RPG_Character_EncumbranceTableConstIterator_t iterator;
  bool tremendous_strength = false;
  if (strength_in > myEncumbranceTable.size ())
  {
    tremendous_strength = true;
    iterator = myEncumbranceTable.find (20);
    ACE_ASSERT(iterator != myEncumbranceTable.end ());
    std::advance (iterator, static_cast<unsigned int> (strength_in) % 10);
  } // end IF
  else
    iterator = myEncumbranceTable.find (strength_in);

  // step2: consider size
  float relative_weight = RPG_Common_Tools::getSizeModifierLoad (size_in,
                                                                 isBiped_in);
  relative_weight *= static_cast<float> (weight_in);

  // step3: consider tremendous strength
  if (tremendous_strength)
    relative_weight /= (4.0F * static_cast<float>(static_cast<unsigned int>(strength_in) / 10));

  // step4: determine load
  if (relative_weight <= (*iterator).second.light)
    return LOAD_LIGHT;
  else if (relative_weight <= (*iterator).second.medium)
    return LOAD_MEDIUM;

  return LOAD_HEAVY;
}

void
RPG_Character_Common_Tools::getLoadModifiers (enum RPG_Character_Encumbrance encumbrance_in,
                                              unsigned char baseSpeed_in,
                                              signed char& maxDexModifierAC_out,
                                              signed char& armorCheckPenalty_out,
                                              unsigned char& speed_out,
                                              unsigned char& runModifier_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getLoadModifiers"));

  // initialize return value(s)
  maxDexModifierAC_out = std::numeric_limits<signed char>::max();
  armorCheckPenalty_out = 0;
  speed_out = baseSpeed_in;
  runModifier_out = RPG_CHARACTER_RUN_MODIFIER_MEDIUM;

  switch (encumbrance_in)
  {
    case LOAD_LIGHT:
      break;
    case LOAD_MEDIUM:
    {
      maxDexModifierAC_out = 3;
      armorCheckPenalty_out = -3;
      speed_out = getReducedSpeed(baseSpeed_in);

      break;
    }
    case LOAD_HEAVY:
    {
      maxDexModifierAC_out = 1;
      armorCheckPenalty_out = -6;
      speed_out = getReducedSpeed(baseSpeed_in);
      runModifier_out = RPG_CHARACTER_RUN_MODIFIER_HEAVY;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid load (was: %u), aborting\n"),
                  ACE_TEXT (RPG_Character_EncumbranceHelper::RPG_Character_EncumbranceToString (encumbrance_in).c_str ())));
      break;
    }
  } // end SWITCH
}

unsigned char
RPG_Character_Common_Tools::getReducedSpeed (unsigned char baseSpeed_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Character_Common_Tools::getReducedSpeed"));

  switch (baseSpeed_in)
  {
    case 20:
      return 15;
    case 30:
      return 20;
    case 40:
      return 30;
    case 50:
      return 35;
    case 60:
      return 40;
    case 70:
      return 50;
    case 80:
      return 55;
    case 90:
      return 60;
    case 100:
      return 70;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid (base) speed (was: %u), aborting\n"),
                  static_cast<unsigned int> (baseSpeed_in)));
      break;
    }
  } // end SWITCH

  return 0;
}
