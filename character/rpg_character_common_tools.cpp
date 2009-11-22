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

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Character_Common_Tools::RPG_Character_String2Gender_t         RPG_Character_Common_Tools::myString2GenderTable;
RPG_Character_Common_Tools::RPG_Character_String2AlignmentCivic_t RPG_Character_Common_Tools::myString2AlignmentCivicTable;
RPG_Character_Common_Tools::RPG_Character_String2AlignmentEthic_t RPG_Character_Common_Tools::myString2AlignmentEthicTable;
RPG_Character_Common_Tools::RPG_Character_String2Attribute_t      RPG_Character_Common_Tools::myString2AttributeTable;
RPG_Character_Common_Tools::RPG_Character_String2Condition_t      RPG_Character_Common_Tools::myString2ConditionTable;

RPG_Character_Common_Tools::RPG_Character_String2Race_t           RPG_Character_Common_Tools::myString2RaceTable;
RPG_Character_Common_Tools::RPG_Character_String2MetaClass_t      RPG_Character_Common_Tools::myString2MetaClassTable;
RPG_Character_Common_Tools::RPG_Character_String2SubClass_t       RPG_Character_Common_Tools::myString2SubClassTable;

void RPG_Character_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::initStringConversionTables"));

  // clean tables
  myString2GenderTable.clear();
  myString2AlignmentCivicTable.clear();
  myString2AlignmentEthicTable.clear();
  myString2AttributeTable.clear();
  myString2ConditionTable.clear();

  myString2RaceTable.clear();
  myString2MetaClassTable.clear();
  myString2SubClassTable.clear();

  // RPG_Character_Gender
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("None"), GENDER_NONE));
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Male"), GENDER_MALE));
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Female"), GENDER_FEMALE));
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Any"), GENDER_ANY));

  // RPG_Character_AlignmentCivic
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_LAWFUL"), ALIGNMENTCIVIC_LAWFUL));
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_NEUTRAL"), ALIGNMENTCIVIC_NEUTRAL));
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_CHAOTIC"), ALIGNMENTCIVIC_CHAOTIC));
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_ANY"), ALIGNMENTCIVIC_ANY));

  // RPG_Character_AlignmentEthic
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_GOOD"), ALIGNMENTETHIC_GOOD));
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_NEUTRAL"), ALIGNMENTETHIC_NEUTRAL));
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_EVIL"), ALIGNMENTETHIC_EVIL));
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_ANY"), ALIGNMENTETHIC_ANY));

  // RPG_Character_Attribute
  myString2AttributeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Strength"), ATTRIBUTE_STRENGTH));
  myString2AttributeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Dexterity"), ATTRIBUTE_DEXTERITY));
  myString2AttributeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Constitution"), ATTRIBUTE_CONSTITUTION));
  myString2AttributeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Intelligence"), ATTRIBUTE_INTELLIGENCE));
  myString2AttributeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Wisdom"), ATTRIBUTE_WISDOM));
  myString2AttributeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Charisma"), ATTRIBUTE_CHARISMA));

  // RPG_Character_Condition
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_NORMAL"), CONDITION_NORMAL));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_ABILITY_DAMAGED"), CONDITION_ABILITY_DAMAGED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_ABILITY_DRAINED"), CONDITION_ABILITY_DRAINED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_BLINDED"), CONDITION_BLINDED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_BLOWN_AWAY"), CONDITION_BLOWN_AWAY));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_CHECKED"), CONDITION_CHECKED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_CONFUSED"), CONDITION_CONFUSED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_COWERING"), CONDITION_COWERING));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_DAZED"), CONDITION_DAZED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_DAZZLED"), CONDITION_DAZZLED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_DEAD"), CONDITION_DEAD));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_DEAFENED"), CONDITION_DEAFENED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_DISABLED"), CONDITION_DISABLED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_DYING"), CONDITION_DYING));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_ENERGY_DRAINED"), CONDITION_ENERGY_DRAINED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_ENTANGLED"), CONDITION_ENTANGLED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_EXHAUSTED"), CONDITION_EXHAUSTED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_FASCINATED"), CONDITION_FASCINATED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_FATIGUED"), CONDITION_FATIGUED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_FLAT_FOOTED"), CONDITION_FLAT_FOOTED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_FRIGHTENED"), CONDITION_FRIGHTENED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_GRAPPLING"), CONDITION_GRAPPLING));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_HELPLESS"), CONDITION_HELPLESS));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_INCORPOREAL"), CONDITION_INCORPOREAL));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_INVISIBLE"), CONDITION_INVISIBLE));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_KNOCKED_DOWN"), CONDITION_KNOCKED_DOWN));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_NAUSEATED"), CONDITION_NAUSEATED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_PANICKED"), CONDITION_PANICKED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_PARALYZED"), CONDITION_PARALYZED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_PETRIFIED"), CONDITION_PETRIFIED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_PINNED"), CONDITION_PINNED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_PRONE"), CONDITION_PRONE));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_SHAKEN"), CONDITION_SHAKEN));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_SICKENED"), CONDITION_SICKENED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_STABLE"), CONDITION_STABLE));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_STAGGERED"), CONDITION_STAGGERED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_STUNNED"), CONDITION_STUNNED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_TURNED"), CONDITION_TURNED));
  myString2ConditionTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("CONDITION_UNCONSCIOUS"), CONDITION_UNCONSCIOUS));

  // RPG_Character_Race
  myString2RaceTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Human"), RACE_HUMAN));
  myString2RaceTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Dwarf"), RACE_DWARF));
  myString2RaceTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Elf"), RACE_ELF));
  myString2RaceTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Halfling"), RACE_HALFLING));
  myString2RaceTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Gnome"), RACE_GNOME));
  myString2RaceTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Orc"), RACE_ORC));

  // RPG_Character_MetaClass
  myString2MetaClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("METACLASS_WARRIOR"), METACLASS_WARRIOR));
  myString2MetaClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("METACLASS_WIZARD"), METACLASS_WIZARD));
  myString2MetaClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("METACLASS_PRIEST"), METACLASS_PRIEST));
  myString2MetaClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("METACLASS_ROGUE"), METACLASS_ROGUE));

  // RPG_Character_SubClass
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Fighter"), SUBCLASS_FIGHTER));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Paladin"), SUBCLASS_PALADIN));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Ranger"), SUBCLASS_RANGER));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Barbarian"), SUBCLASS_BARBARIAN));
//   myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Warlord"), SUBCLASS_WARLORD));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Wizard"), SUBCLASS_WIZARD));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Sorcerer"), SUBCLASS_SORCERER));
//   myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Warlock"), SUBCLASS_WARLOCK));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Cleric"), SUBCLASS_CLERIC));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Druid"), SUBCLASS_DRUID));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Monk"), SUBCLASS_MONK));
//   myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Avenger"), SUBCLASS_AVENGER));
//   myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Invoker"), SUBCLASS_INVOKER));
//   myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Shaman"), SUBCLASS_SHAMAN));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Thief"), SUBCLASS_THIEF));
  myString2SubClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Bard"), SUBCLASS_BARD));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Common_Tools: initialized string conversion tables...\n")));
}

const RPG_Character_Gender RPG_Character_Common_Tools::stringToGender(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToGender"));

  RPG_Character_String2GenderIterator_t iterator = myString2GenderTable.find(string_in);
  if (iterator == myString2GenderTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid gender: \"%s\" --> check implementation !, aborting\n"),
               string_in.c_str()));

    return GENDER_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Character_Common_Tools::genderToString(const RPG_Character_Gender& gender_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::genderToString"));

  RPG_Character_String2GenderIterator_t iterator = myString2GenderTable.begin();
  do
  {
    if (iterator->second == gender_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2GenderTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid gender: %d --> check implementation !, aborting\n"),
             gender_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("GENDER_INVALID"));
}

const RPG_Character_AlignmentCivic RPG_Character_Common_Tools::stringToAlignmentCivic(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToAlignmentCivic"));

  RPG_Character_String2AlignmentCivicIterator_t iterator = myString2AlignmentCivicTable.find(string_in);
  if (iterator == myString2AlignmentCivicTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid (civic) alignment: \"%s\" --> check implementation !, aborting\n"),
               string_in.c_str()));

    return ALIGNMENTCIVIC_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_AlignmentEthic RPG_Character_Common_Tools::stringToAlignmentEthic(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToAlignmentEthic"));

  RPG_Character_String2AlignmentEthicIterator_t iterator = myString2AlignmentEthicTable.find(string_in);
  if (iterator == myString2AlignmentEthicTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid (ethic) alignment: \"%s\" --> check implementation !, aborting\n"),
               string_in.c_str()));

    return ALIGNMENTETHIC_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Character_Common_Tools::alignmentToString(const RPG_Character_Alignment& alignment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::alignmentToString"));

  std::string result;

  RPG_Character_String2AlignmentCivicIterator_t iterator = myString2AlignmentCivicTable.begin();
  do
  {
    if (iterator->second == alignment_in.civicAlignment)
    {
      // found first part
      result = iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2AlignmentCivicTable.end());

  // sanity check
  if (result.empty())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid alignment (civic): %d --> check implementation !, aborting\n"),
               alignment_in.civicAlignment));

    return std::string(ACE_TEXT_ALWAYS_CHAR("ALIGNMENT_INVALID"));
  } // end IF

  RPG_Character_String2AlignmentEthicIterator_t iterator2 = myString2AlignmentEthicTable.begin();
  do
  {
    if (iterator2->second == alignment_in.ethicAlignment)
    {
      // found second part
      result += ACE_TEXT_ALWAYS_CHAR(" ");
      result += iterator2->first;
    } // end IF

    iterator2++;
  } while (iterator2 != myString2AlignmentEthicTable.end());

  // "Neutral" "Neutral" --> "True Neutral"
  if ((alignment_in.civicAlignment == ALIGNMENTCIVIC_NEUTRAL) &&
      (alignment_in.ethicAlignment == ALIGNMENTETHIC_NEUTRAL))
  {
    result = ACE_TEXT_ALWAYS_CHAR("True Neutral");
  } // end IF

  // sanity check
  if (result.find(ACE_TEXT_ALWAYS_CHAR(" ")) == std::string::npos)
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid alignment (ethic): %d --> check implementation !, aborting\n"),
               alignment_in.ethicAlignment));

    return std::string(ACE_TEXT_ALWAYS_CHAR("ALIGNMENT_INVALID"));
  } // end IF

  return result;
}

const RPG_Character_Condition RPG_Character_Common_Tools::stringToCondition(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToCondition"));

  RPG_Character_String2ConditionIterator_t iterator = myString2ConditionTable.find(string_in);
  if (iterator == myString2ConditionTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid condition: \"%s\" --> check implementation !, aborting\n"),
               string_in.c_str()));

    return CONDITION_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Character_Common_Tools::raceToString(const RPG_Character_Race& race_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::raceToString"));

  RPG_Character_String2RaceIterator_t iterator = myString2RaceTable.begin();
  do
  {
    if (iterator->second == race_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2RaceTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid subClass: %d --> check implementation !, aborting\n"),
             race_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("RACE_INVALID"));
}

const RPG_Character_MetaClass RPG_Character_Common_Tools::stringToMetaClass(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToMetaClass"));

  RPG_Character_String2MetaClassIterator_t iterator = myString2MetaClassTable.find(string_in);
  if (iterator == myString2MetaClassTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid metaClass: \"%s\" --> check implementation !, aborting\n"),
               string_in.c_str()));

    return METACLASS_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_SubClass RPG_Character_Common_Tools::stringToSubClass(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToSubClass"));

  RPG_Character_String2SubClassIterator_t iterator = myString2SubClassTable.find(string_in);
  if (iterator == myString2SubClassTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid subClass: \"%s\" --> check implementation !, aborting\n"),
               string_in.c_str()));

    return SUBCLASS_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Character_Common_Tools::subClassToString(const RPG_Character_SubClass& subClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::subClassToString"));

  RPG_Character_String2SubClassIterator_t iterator = myString2SubClassTable.begin();
  do
  {
    if (iterator->second == subClass_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2SubClassTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid subClass: %d --> check implementation !, aborting\n"),
             subClass_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("SUBCLASS_INVALID"));
}

const std::string RPG_Character_Common_Tools::attributeToString(const RPG_Character_Attribute& attribute_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::attributeToString"));

  RPG_Character_String2AttributeIterator_t iterator = myString2AttributeTable.begin();
  do
  {
    if (iterator->second == attribute_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2AttributeTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid attribute: %d --> check implementation !, aborting\n"),
             attribute_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_INVALID"));
}

const std::string RPG_Character_Common_Tools::attributesToString(const RPG_Character_Attributes& attributes_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::attributesToString"));

  std::string result;
  std::stringstream str;
  result = attributeToString(ATTRIBUTE_STRENGTH);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.strength);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += attributeToString(ATTRIBUTE_DEXTERITY);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.dexterity);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += attributeToString(ATTRIBUTE_CONSTITUTION);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.constitution);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += attributeToString(ATTRIBUTE_INTELLIGENCE);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.intelligence);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += attributeToString(ATTRIBUTE_WISDOM);
  result += ACE_TEXT_ALWAYS_CHAR(": ");
  str << ACE_static_cast(unsigned int, attributes_in.wisdom);
  result += str.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += attributeToString(ATTRIBUTE_CHARISMA);
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

  return D_TYPE_INVALID;
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
