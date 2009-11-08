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
#include "rpg_chance_dice_common_tools.h"

#include <ace/Log_Msg.h>

// init statics
RPG_Chance_Dice_Common_Tools::RPG_String2DiceType_t RPG_Chance_Dice_Common_Tools::myString2DiceTypeTable;

void RPG_Chance_Dice_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::initStringConversionTables"));

  // clean tables
  myString2DiceTypeTable.clear();

  // RPG_Chance_DiceType
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_2"), D_2));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_3"), D_3));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_4"), D_4));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_6"), D_6));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_8"), D_8));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_10"), D_10));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_12"), D_12));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_20"), D_20));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_100"), D_100));
  myString2DiceTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("D_TYPE_INVALID"), D_TYPE_INVALID));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Chance_Dice_Common_Tools: initialized string conversion tables...\n")));
}

const RPG_Chance_DiceType RPG_Chance_Dice_Common_Tools::stringToDiceType(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::stringToDiceType"));

  RPG_String2DiceTypeIterator_t iterator = myString2DiceTypeTable.find(string_in);
  if (iterator == myString2DiceTypeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid dice type: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return D_TYPE_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Chance_Dice_Common_Tools::diceType2String(const RPG_Chance_DiceType& diceType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::diceType2String"));

  RPG_String2DiceTypeIterator_t iterator = myString2DiceTypeTable.begin();
  do
  {
    if (iterator->second == diceType_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2DiceTypeTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid dice type: %d --> check implementation !, aborting\n"),
             diceType_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("D_TYPE_INVALID"));
}

const std::string RPG_Chance_Dice_Common_Tools::rollToString(const RPG_Chance_Roll& diceRoll_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::rollToString"));

  std::string result;

  result += diceRoll_in.numDice;
  result += ACE_TEXT_ALWAYS_CHAR(" ");
  result += RPG_Chance_Dice_Common_Tools::diceType2String(diceRoll_in.typeDice);

  if (diceRoll_in.modifier == 0)
  {
    return result;
  } // end IF

  result += ACE_TEXT_ALWAYS_CHAR(" ");
  if (diceRoll_in.modifier > 0)
  {
    result += ACE_TEXT_ALWAYS_CHAR("+");
  } // end IF
  result += diceRoll_in.modifier;

  return result;
}
