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
#ifndef RPG_CHANCE_DICE_COMMON_H
#define RPG_CHANCE_DICE_COMMON_H

#include <ace/Assert.h>

#include <vector>

enum RPG_Chance_DiceType
{
  D_0   = 0,
  D_2   = 2,
  D_3   = 3,
  D_4   = 4,
  D_6   = 6,
  D_8   = 8,
  D_10  = 10,
  D_12  = 12,
  D_20  = 20,
  D_100 = 100,
  //
  D_TYPE_MAX,
  D_TYPE_INVALID
};

// prefix increment
inline RPG_Chance_DiceType& operator++(RPG_Chance_DiceType& diceType_in)
{
  switch (diceType_in)
  {
    case D_0: diceType_in = D_2; break;
    case D_2: diceType_in = D_3; break;
    case D_3: diceType_in = D_4; break;
    case D_4: diceType_in = D_6; break;
    case D_6: diceType_in = D_8; break;
    case D_8: diceType_in = D_10; break;
    case D_10: diceType_in = D_12; break;
    case D_12: diceType_in = D_20; break;
    case D_20: diceType_in = D_100; break;
    case D_100: diceType_in = D_0; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return diceType_in;
};

// postfix increment
inline RPG_Chance_DiceType operator++(RPG_Chance_DiceType& diceType_in, int)
{
  RPG_Chance_DiceType result = ++diceType_in;
  return result;
};

// prefix decrement
inline RPG_Chance_DiceType& operator--(RPG_Chance_DiceType& diceType_in)
{
  switch (diceType_in)
  {
    case D_0: diceType_in = D_100; break;
    case D_2: diceType_in = D_0; break;
    case D_3: diceType_in = D_2; break;
    case D_4: diceType_in = D_3; break;
    case D_6: diceType_in = D_4; break;
    case D_8: diceType_in = D_6; break;
    case D_10: diceType_in = D_8; break;
    case D_12: diceType_in = D_10; break;
    case D_20: diceType_in = D_12; break;
    case D_100: diceType_in = D_20; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return diceType_in;
};

// postfix decrement
inline RPG_Chance_DiceType operator--(RPG_Chance_DiceType& diceType_in, int)
{
  RPG_Chance_DiceType result = --diceType_in;
  return result;
};

struct RPG_Chance_DiceRoll
{
  unsigned int        numDice;  // number of dice to roll
  RPG_Chance_DiceType typeDice; // the type of dice to roll
  int                 modifier; // +/-x modifier (e.g. 2D4+4)
};

struct RPG_Chance_ValueRange
{
  int begin;
  int end;
};

// some convenient types...
typedef std::vector<RPG_Chance_DiceRoll> RPG_Chance_DiceRolls_t;
typedef RPG_Chance_DiceRolls_t::const_iterator RPG_Chance_DiceRollsIterator_t;
typedef std::vector<int> RPG_Chance_DiceRollResult_t;
typedef RPG_Chance_DiceRollResult_t::const_iterator RPG_Chance_DiceRollResultIterator_t;

#endif
