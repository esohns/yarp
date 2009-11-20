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
#ifndef RPG_CHANCE_DICE_H
#define RPG_CHANCE_DICE_H

#include "rpg_chance_dice_common.h"

#include <ace/Global_Macros.h>

/**
emulate rolling an n-sided die

	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Chance_Dice
{
 public:
  // init random seed
  static void init();

  static void generateRandomNumbers(const unsigned int&,           // range [1..max]
                                    const unsigned int&,           // number of rolls
                                    RPG_Chance_DiceRollResult_t&); // result(s)
  static void simulateDiceRoll(const RPG_Chance_DiceRoll&,    // specifics (number of dice, type, modifier)
                               const unsigned int&,           // number of rolls
                               RPG_Chance_DiceRollResult_t&); // result(s)
  static void diceRollToRange(const RPG_Chance_DiceRoll&, // roll specifics
                              RPG_Chance_ValueRange&);    // result

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Chance_Dice());
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice(const RPG_Chance_Dice&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice& operator=(const RPG_Chance_Dice&));
};

#endif
