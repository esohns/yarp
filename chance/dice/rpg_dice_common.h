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

#ifndef RPG_DICE_COMMON_H
#define RPG_DICE_COMMON_H

#include "rpg_dice_dietype.h"
#include "rpg_dice_roll.h"

#include <ace/Assert.h>

#include <vector>

// some convenient types...
typedef std::vector<RPG_Dice_Roll> RPG_Dice_Rolls_t;
typedef RPG_Dice_Rolls_t::const_iterator RPG_Dice_RollsIterator_t;

typedef std::vector<unsigned int> RPG_Dice_RollResult_t;
typedef RPG_Dice_RollResult_t::const_iterator RPG_Dice_RollResultIterator_t;

// enum RPG_Dice_DieType
// {
//   // *TODO*: enum values need to reflect the number of faces
//   // --> will be clobbered by subsequent updates of the schema
//   // (refer to rpg_dice_common.h for a template)
//   D_0   = 0,
//   D_2   = 2,
//   D_3   = 3,
//   D_4   = 4,
//   D_6   = 6,
//   D_8   = 8,
//   D_10  = 10,
//   D_12  = 12,
//   D_20  = 20,
//   D_100 = 100,
//   //
//   RPG_DICE_DIETYPE_MAX,
//   RPG_DICE_DIETYPE_INVALID
// };

// enhance some (existing) definition(s)
// prefix increment
inline RPG_Dice_DieType& operator++(RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
//    case D_0: dieType_inout = D_4; break;
    case D_0: dieType_inout = D_2; break;
    case D_2: dieType_inout = D_3; break;
    case D_3: dieType_inout = D_4; break;
    case D_4: dieType_inout = D_6; break;
    case D_6: dieType_inout = D_8; break;
    case D_8: dieType_inout = D_10; break;
    case D_10: dieType_inout = D_12; break;
    case D_12: dieType_inout = D_20; break;
    case D_20: dieType_inout = D_100; break;
    case D_100: dieType_inout = D_0; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return dieType_inout;
};
// postfix increment
inline RPG_Dice_DieType operator++(RPG_Dice_DieType& dieType_inout, int)
{
  RPG_Dice_DieType result = ++dieType_inout;

  return result;
};
// prefix decrement
inline RPG_Dice_DieType& operator--(RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
    case D_0: dieType_inout = D_100; break;
    case D_2: dieType_inout = D_0; break;
    case D_3: dieType_inout = D_2; break;
//    case D_4: dieType_inout = D_0; break;
    case D_4: dieType_inout = D_3; break;
    case D_6: dieType_inout = D_4; break;
    case D_8: dieType_inout = D_6; break;
    case D_10: dieType_inout = D_8; break;
    case D_12: dieType_inout = D_10; break;
    case D_20: dieType_inout = D_12; break;
    case D_100: dieType_inout = D_20; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return dieType_inout;
};
// postfix decrement
inline RPG_Dice_DieType operator--(RPG_Dice_DieType& dieType_inout, int)
{
  RPG_Dice_DieType result = --dieType_inout;

  return result;
};

// allow multiplier
// struct RPG_Dice_Export RPG_Dice_Roll
// {
//   unsigned int numDice;
//   RPG_Dice_DieType typeDice;
//   int modifier;
//
//   // *TODO* will be clobbered by subsequent updates of the schema
//   // (refer to rpg_dice_common.h for a template)
//   RPG_Dice_Roll& operator*=(int);
// };
inline RPG_Dice_Roll& RPG_Dice_Roll::operator*=(int multiplier_in)
{
  numDice *= multiplier_in;
  modifier *= multiplier_in;

  return (*this);
};

#endif
