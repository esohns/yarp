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
#include "rpg_chance_dice_XML_parser.h"

#include "rpg_chance_dicetype.h"

// void RPG_Chance_DiceType_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Chance_DiceType_Type::pre"));
//
// }

RPG_Chance_DiceType RPG_Chance_DiceType_Type::post_RPG_Chance_DiceType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceType_Type::post_RPG_Chance_DiceType_Type"));

  return RPG_Chance_DiceTypeHelper::stringToRPG_Chance_DiceType(post_string());
}

// void RPG_Chance_DiceRoll_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Chance_DiceRoll_Type::pre"));
//
// }

RPG_Chance_DiceRoll_Type::RPG_Chance_DiceRoll_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceRoll_Type::RPG_Chance_DiceRoll_Type"));

  myCurrentDiceRoll.numDice = 0;
  myCurrentDiceRoll.typeDice = RPG_CHANCE_DICETYPE_INVALID;
  myCurrentDiceRoll.modifier = 0;
}

void RPG_Chance_DiceRoll_Type::numDice(unsigned int numDice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceRoll_Type::numDice"));

  myCurrentDiceRoll.numDice = numDice_in;
}

void RPG_Chance_DiceRoll_Type::typeDice(const RPG_Chance_DiceType& typeDice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceRoll_Type::typeDice"));

  myCurrentDiceRoll.typeDice = typeDice_in;
}

void RPG_Chance_DiceRoll_Type::modifier(long long modifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceRoll_Type::modifier"));

  myCurrentDiceRoll.modifier = modifier_in;
}

RPG_Chance_DiceRoll RPG_Chance_DiceRoll_Type::post_RPG_Chance_DiceRoll_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceRoll_Type::post_RPG_Chance_DiceRoll_Type"));

  RPG_Chance_DiceRoll result = myCurrentDiceRoll;

  // clear structure
  myCurrentDiceRoll.numDice = 0;
  myCurrentDiceRoll.typeDice = RPG_CHANCE_DICETYPE_INVALID;
  myCurrentDiceRoll.modifier = 0;

  return result;
}

// void RPG_Chance_ValueRange_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Chance_ValueRange_Type::pre"));
//
// }

RPG_Chance_ValueRange_Type::RPG_Chance_ValueRange_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_ValueRange_Type::RPG_Chance_ValueRange_Type"));

  myCurrentValueRange.begin = 0;
  myCurrentValueRange.end = 0;
}

void RPG_Chance_ValueRange_Type::begin(long long begin_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_ValueRange_Type::begin"));

  myCurrentValueRange.begin = begin_in;
}

void RPG_Chance_ValueRange_Type::end(long long end_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_ValueRange_Type::end"));

  myCurrentValueRange.end = end_in;
}

RPG_Chance_ValueRange RPG_Chance_ValueRange_Type::post_RPG_Chance_ValueRange_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_ValueRange_Type::post_RPG_Chance_ValueRange_Type"));

  RPG_Chance_ValueRange result = myCurrentValueRange;

  // clear structure
  myCurrentValueRange.begin = 0;
  myCurrentValueRange.end = 0;

  return result;
}
