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

#include <sstream>

// init statics
RPG_Chance_DiceTypeToStringTable_t RPG_Chance_DiceTypeHelper::myRPG_Chance_DiceTypeToStringTable;

void RPG_Chance_Dice_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::initStringConversionTables"));

  RPG_Chance_DiceTypeHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Chance_Dice_Common_Tools: initialized string conversion table...\n")));
}

const std::string RPG_Chance_Dice_Common_Tools::rollToString(const RPG_Chance_DiceRoll& diceRoll_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::rollToString"));

  std::string result;
  std::stringstream str;

  str << diceRoll_in.numDice;
  result += str.str();
  result += RPG_Chance_DiceTypeHelper::RPG_Chance_DiceTypeToString(diceRoll_in.typeDice);

  if (diceRoll_in.modifier == 0)
  {
    return result;
  } // end IF
  else if (diceRoll_in.modifier > 0)
  {
    result += ACE_TEXT_ALWAYS_CHAR("+");
  } // end IF
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  str << diceRoll_in.modifier;
  result += str.str();

  return result;
}

const std::string RPG_Chance_Dice_Common_Tools::rangeToString(const RPG_Chance_ValueRange& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice_Common_Tools::rangeToString"));

  std::string result;
  std::stringstream str;

  str << range_in.begin;
  str << ACE_TEXT_ALWAYS_CHAR("-");
  str << range_in.end;

  result = str.str();

  return result;
}
