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

typedef std::vector<int> RPG_Dice_RollResult_t;
typedef RPG_Dice_RollResult_t::const_iterator RPG_Dice_RollResultIterator_t;

// enhance some (existing) definition(s)
// prefix increment
inline RPG_Dice_DieType& operator++(RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
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

// // allow multiplier
// inline RPG_Dice_Roll& RPG_Dice_Roll::operator*=(const int& multiplier_in)
// {
//   numDice *= multiplier_in;
//   modifier *= multiplier_in;
//   return (*this);
// };

#endif
