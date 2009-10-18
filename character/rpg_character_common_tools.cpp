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

// init statics
RPG_Character_Common_Tools::RPG_String2Gender_t         RPG_Character_Common_Tools::myString2GenderTable;
RPG_Character_Common_Tools::RPG_String2AlignmentCivic_t RPG_Character_Common_Tools::myString2AlignmentCivicTable;
RPG_Character_Common_Tools::RPG_String2AlignmentEthic_t RPG_Character_Common_Tools::myString2AlignmentEthicTable;
RPG_Character_Common_Tools::RPG_String2Condition_t      RPG_Character_Common_Tools::myString2ConditionTable;

void RPG_Character_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::initStringConversionTables"));

  // clean tables
  myString2GenderTable.clear();
  myString2AlignmentCivicTable.clear();
  myString2AlignmentEthicTable.clear();
  myString2ConditionTable.clear();

  // RPG_Character_Gender
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("GENDER_NONE"), GENDER_NONE));
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("GENDER_MALE"), GENDER_MALE));
  myString2GenderTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("GENDER_FEMALE"), GENDER_FEMALE));

  // RPG_Character_AlignmentCivic
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_LAWFUL"), ALIGNMENTCIVIC_LAWFUL));
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_NEUTRAL"), ALIGNMENTCIVIC_NEUTRAL));
  myString2AlignmentCivicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTCIVIC_CHAOTIC"), ALIGNMENTCIVIC_CHAOTIC));

  // RPG_Character_AlignmentEthic
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_GOOD"), ALIGNMENTETHIC_GOOD));
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_NEUTRAL"), ALIGNMENTETHIC_NEUTRAL));
  myString2AlignmentEthicTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ALIGNMENTETHIC_EVIL"), ALIGNMENTETHIC_EVIL));

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
}

RPG_Character_Gender RPG_Character_Common_Tools::stringToGender(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToGender"));

  RPG_String2GenderIterator_t iterator = myString2GenderTable.find(string_in);
  if (iterator == myString2GenderTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid gender: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return GENDER_INVALID;
  } // end IF

  return iterator->second;
}

RPG_Character_AlignmentCivic RPG_Character_Common_Tools::stringToAlignmentCivic(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToAlignmentCivic"));

  RPG_String2AlignmentCivicIterator_t iterator = myString2AlignmentCivicTable.find(string_in);
  if (iterator == myString2AlignmentCivicTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid (civic) alignment: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ALIGNMENTCIVIC_INVALID;
  } // end IF

  return iterator->second;
}

RPG_Character_AlignmentEthic RPG_Character_Common_Tools::stringToAlignmentEthic(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToAlignmentEthic"));

  RPG_String2AlignmentEthicIterator_t iterator = myString2AlignmentEthicTable.find(string_in);
  if (iterator == myString2AlignmentEthicTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid (ethic) alignment: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ALIGNMENTETHIC_INVALID;
  } // end IF

  return iterator->second;
}

RPG_Character_Condition RPG_Character_Common_Tools::stringToCondition(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Common_Tools::stringToCondition"));

  RPG_String2ConditionIterator_t iterator = myString2ConditionTable.find(string_in);
  if (iterator == myString2ConditionTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid condition: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return CONDITION_INVALID;
  } // end IF

  return iterator->second;
}
