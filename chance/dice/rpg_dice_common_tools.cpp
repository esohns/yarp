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

#include "rpg_dice_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
RPG_Dice_DieTypeToStringTable_t RPG_Dice_DieTypeHelper::myRPG_Dice_DieTypeToStringTable;

void RPG_Dice_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Common_Tools::initStringConversionTables"));

  RPG_Dice_DieTypeHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Dice_Common_Tools: initialized string conversion table...\n")));
}

const std::string RPG_Dice_Common_Tools::rollToString(const RPG_Dice_Roll& roll_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Common_Tools::rollToString"));

  std::string result;
  std::stringstream str;

  str << roll_in.numDice;
  result += str.str();
  result += RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString(roll_in.typeDice);

  if (roll_in.modifier == 0)
  {
    return result;
  } // end IF
  else if (roll_in.modifier > 0)
  {
    result += ACE_TEXT_ALWAYS_CHAR("+");
  } // end IF
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  str << roll_in.modifier;
  result += str.str();

  return result;
}

const std::string RPG_Dice_Common_Tools::rangeToString(const RPG_Dice_ValueRange& range_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Common_Tools::rangeToString"));

  std::string result;
  std::stringstream str;

  str << range_in.begin;
  if (range_in.begin != range_in.end)
  {
    str << ACE_TEXT_ALWAYS_CHAR("-");
    str << range_in.end;
  } // end IF

  result = str.str();

  return result;
}
