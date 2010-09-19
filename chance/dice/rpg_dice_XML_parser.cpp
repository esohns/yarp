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
#include "rpg_dice_XML_parser.h"

#include <rpg_common_macros.h>

// void RPG_Dice_DieType_Type::pre()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Dice_DieType_Type::pre"));
//
// }

RPG_Dice_DieType RPG_Dice_DieType_Type::post_RPG_Dice_DieType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_DieType_Type::post_RPG_Dice_DieType_Type"));

  return RPG_Dice_DieTypeHelper::stringToRPG_Dice_DieType(post_string());
}

// void RPG_Dice_Roll_Type::pre()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Dice_Roll_Type::pre"));
//
// }

RPG_Dice_Roll_Type::RPG_Dice_Roll_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Roll_Type::RPG_Dice_Roll_Type"));

  myCurrentRoll.numDice = 0;
  myCurrentRoll.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentRoll.modifier = 0;
}

void RPG_Dice_Roll_Type::numDice(unsigned int numDice_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Roll_Type::numDice"));

  myCurrentRoll.numDice = numDice_in;
}

void RPG_Dice_Roll_Type::typeDice(const RPG_Dice_DieType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Roll_Type::typeDice"));

  myCurrentRoll.typeDice = type_in;
}

void RPG_Dice_Roll_Type::modifier(long long modifier_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Roll_Type::modifier"));

  myCurrentRoll.modifier = modifier_in;
}

RPG_Dice_Roll RPG_Dice_Roll_Type::post_RPG_Dice_Roll_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_Roll_Type::post_RPG_Dice_Roll_Type"));

  RPG_Dice_Roll result = myCurrentRoll;

  // clear structure
  myCurrentRoll.numDice = 0;
  myCurrentRoll.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentRoll.modifier = 0;

  return result;
}

// void RPG_Dice_ValueRange_Type::pre()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Dice_ValueRange_Type::pre"));
//
// }

RPG_Dice_ValueRange_Type::RPG_Dice_ValueRange_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_ValueRange_Type::RPG_Dice_ValueRange_Type"));

  myCurrentValueRange.begin = 0;
  myCurrentValueRange.end = 0;
}

void RPG_Dice_ValueRange_Type::begin(long long begin_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_ValueRange_Type::begin"));

  myCurrentValueRange.begin = begin_in;
}

void RPG_Dice_ValueRange_Type::end(long long end_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_ValueRange_Type::end"));

  myCurrentValueRange.end = end_in;
}

RPG_Dice_ValueRange RPG_Dice_ValueRange_Type::post_RPG_Dice_ValueRange_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Dice_ValueRange_Type::post_RPG_Dice_ValueRange_Type"));

  RPG_Dice_ValueRange result = myCurrentValueRange;

  // clear structure
  myCurrentValueRange.begin = 0;
  myCurrentValueRange.end = 0;

  return result;
}
