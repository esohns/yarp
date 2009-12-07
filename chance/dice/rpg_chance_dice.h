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

#include "rpg_chance_dicetype.h"
#include "rpg_chance_diceroll.h"
#include "rpg_chance_valuerange.h"

#include <ace/Global_Macros.h>

#include <set>

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
  static void rangeToDiceRoll(const RPG_Chance_ValueRange&, // range
                              RPG_Chance_DiceRoll&);        // roll specifics

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Chance_Dice());
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice(const RPG_Chance_Dice&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice& operator=(const RPG_Chance_Dice&));

  // helper methods
  static const unsigned int distanceRangeToRange(const RPG_Chance_ValueRange&,
                                                 const RPG_Chance_ValueRange&);

  // helper types
  struct rangeToRollElement
  {
    // needed for proper sorting...
    bool operator<(const rangeToRollElement& rhs_in) const
    {
      ACE_ASSERT((range.begin == rhs_in.range.begin) &&
                 (range.end == rhs_in.range.end));

      // compute distance to other
      RPG_Chance_ValueRange possible_range;
      diceRollToRange(roll, possible_range);
      RPG_Chance_ValueRange possible_range2;
      diceRollToRange(rhs_in.roll, possible_range2);
      int distance = distanceRangeToRange(range, possible_range);
      int distance2 = distanceRangeToRange(range, possible_range2);
      return (distance < distance2);
    };

    RPG_Chance_ValueRange range;
    RPG_Chance_DiceRoll roll;
  };
  typedef std::set<rangeToRollElement> RPG_Chance_Dice_SortedRolls_t;
  typedef RPG_Chance_Dice_SortedRolls_t::const_iterator RPG_Chance_Dice_SortedRollsIterator_t;
};

#endif
