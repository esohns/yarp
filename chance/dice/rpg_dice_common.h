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

#include <vector>

#include "ace/Assert.h"

#include "rpg_dice_dietype.h"
#include "rpg_dice_roll.h"

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
RPG_Dice_DieType& operator++ (RPG_Dice_DieType&);
// postfix increment
RPG_Dice_DieType operator++ (RPG_Dice_DieType&, int);
// prefix decrement
RPG_Dice_DieType& operator-- (RPG_Dice_DieType&);
// postfix decrement
enum RPG_Dice_DieType operator-- (enum RPG_Dice_DieType&, int);

// ---------------------------------------

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

#endif
