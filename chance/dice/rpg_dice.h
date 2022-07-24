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

#ifndef RPG_DICE_H
#define RPG_DICE_H

#include <set>
#include <utility>

#include "ace/Global_Macros.h"

#include "rpg_dice_common.h"
#include "rpg_dice_dietype.h"
#include "rpg_dice_roll.h"
#include "rpg_dice_valuerange.h"

/**
simulate rolling (n-sided) dice
*/
class RPG_Dice
{
 public:
  // initialize random seed
  static void initialize ();

  // *TODO*: make these thread-safe !
  // *WARNING*: range cannot exceed [1..(RAND_MAX + 1)] !
  static void generateRandomNumbers (unsigned int,            // range [1..max]
                                     unsigned int,            // number of rolls
                                     RPG_Dice_RollResult_t&); // result(s)
  static void simulateRoll (const RPG_Dice_Roll&,    // specifics (number of dice, type, modifier)
                            unsigned int,            // number of rolls
                            RPG_Dice_RollResult_t&); // result(s)
  static bool probability (float); // probability ([0.0F, 1.0F], percentage)

  static void rollToRange (const RPG_Dice_Roll&,  // roll specifics
                           RPG_Dice_ValueRange&); // result
  static void rangeToRoll (const RPG_Dice_ValueRange&, // range
                           RPG_Dice_Roll&);            // roll specifics

 private:
   ACE_UNIMPLEMENTED_FUNC (RPG_Dice ())
   ACE_UNIMPLEMENTED_FUNC (~RPG_Dice ())
   ACE_UNIMPLEMENTED_FUNC (RPG_Dice (const RPG_Dice&))
   ACE_UNIMPLEMENTED_FUNC (RPG_Dice& operator= (const RPG_Dice&))

  // helper methods
  inline static unsigned int distanceRangeToRange (const RPG_Dice_ValueRange& rangeA_in, const RPG_Dice_ValueRange& rangeB_in) { return (::abs (rangeA_in.begin - rangeB_in.begin) + ::abs (rangeA_in.end - rangeB_in.end)); }

  // helper types
  struct rangeToRollElement
  { // needed for proper sorting
    bool operator< (const rangeToRollElement& rhs_in) const
    { ACE_ASSERT ((range.begin == rhs_in.range.begin) && (range.end == rhs_in.range.end));
      // compute distance to other
      RPG_Dice_ValueRange possible_range;
      rollToRange (roll, possible_range);
      RPG_Dice_ValueRange possible_range2;
      rollToRange (rhs_in.roll, possible_range2);
      int distance = distanceRangeToRange (range, possible_range);
      int distance2 = distanceRangeToRange (range, possible_range2);
      return (distance < distance2);
    };

    RPG_Dice_ValueRange range;
    RPG_Dice_Roll roll;
  };
  typedef std::set<rangeToRollElement> RPG_Dice_SortedRolls_t;
  typedef RPG_Dice_SortedRolls_t::const_iterator RPG_Dice_SortedRollsIterator_t;

  // helper methods
  typedef std::pair<unsigned int, unsigned int> RPG_Dice_Fraction_t;
  static RPG_Dice_Fraction_t farey (float,         // decimal
                                    float,         // epsilon (== precision)
                                    unsigned int); // max. nominator
};

#endif
