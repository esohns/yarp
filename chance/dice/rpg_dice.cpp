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

#include "rpg_dice.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Time_Value.h"

#include "common_time_common.h"

#include "rpg_common_macros.h"

#include "rpg_dice_common_tools.h"
#include "rpg_dice_defines.h"

void
RPG_Dice::initialize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::initialize"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing random seed (RAND_MAX = %d)...\n"),
              RAND_MAX));
  // *PORTABILITY*: outside glibc, this is not very portable...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_OS::srand (static_cast<u_int> (COMMON_TIME_NOW.sec ()));
#else
  ::srandom (COMMON_TIME_NOW.sec ());
#endif // ACE_WIN32 || ACE_WIN64
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("initializing random seed...DONE\n")));
}

void
RPG_Dice::generateRandomNumbers (unsigned int range_in,
                                 unsigned int numRolls_in,
                                 RPG_Dice_RollResult_t& results_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::generateRandomNumbers"));

  ACE_ASSERT (range_in);
//  ACE_ASSERT(range_in <= RAND_MAX);

  // initialize result(s)
  results_out.clear ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  unsigned int usecs = static_cast<unsigned int> (COMMON_TIME_NOW.usec ());
#endif // ACE_WIN32 || ACE_WIN64
  for (unsigned int i = 0;
       i < numRolls_in;
       i++)
  {
    // *PORTABILITY*: outside glibc, this is not very portable...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // *NOTE*: use ACE_OS::rand_r() for improved thread safety !
    //results_out.push_back((ACE_OS::rand() % range_in) + 1);
    results_out.push_back ((ACE_OS::rand_r (&usecs) % range_in) + 1);
#else
    results_out.push_back ((::random () % range_in) + 1);
#endif // ACE_WIN32 || ACE_WIN64
  } // end FOR
}

void
RPG_Dice::simulateRoll (const struct RPG_Dice_Roll& rollSpecs_in,
                        unsigned int numRolls_in,
                        RPG_Dice_RollResult_t& results_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::simulateRoll"));

  // initialize result(s)
  results_out.clear ();

//   // debug info
//   std::string die = RPG_Dice_Common_Tools::diceType2String(rollSpecs_in.typeDice);
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("generating %d roll(s) of %d%s (modifier: %d)...\n"),
//              numRolls_in,
//              rollSpecs_in.numDice,
//              die.c_str(),
//              rollSpecs_in.modifier));

  unsigned int tempResult = 0;
  for (unsigned int i = 0;
       i < numRolls_in;
       i++)
  {
    tempResult = 0;
    for (unsigned int j = 0;
         j < rollSpecs_in.numDice;
         j++)
    {
      // *PORTABILITY*: outside glibc, this is not very portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      tempResult += ((::random () % rollSpecs_in.typeDice) + 1);
#else
      // *TODO*: use ACE_OS::rand_r() for improved reentrancy !
      tempResult += ((ACE_OS::rand () % rollSpecs_in.typeDice) + 1);
#endif
    } // end FOR

    results_out.push_back (tempResult + rollSpecs_in.modifier);
  } // end FOR
}

bool
RPG_Dice::probability (float probability_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::probability"));

  // sanity checks
  ACE_ASSERT ((probability_in >= 0.0f) && (probability_in <= 1.0f));

  // step0: fast path ?
  if (probability_in == 0.0F)
    return false;
  else if (probability_in == 1.0F)
    return true;

  // step1: convert decimal to a fraction
  // *TODO*: make this more flexible...
  RPG_Dice_Fraction_t fraction = std::make_pair (0, 0);
  fraction = RPG_Dice::farey (probability_in,
                              RPG_DICE_DEF_FAREY_EPSILON,
                              RPG_DICE_DEF_FAREY_MAX_NOMINATOR);

  // step2: generate randomness
  RPG_Dice_RollResult_t random_number;
  RPG_Dice::generateRandomNumbers (fraction.second,
                                   1,
                                   random_number);

  // step3: test for hit/miss
  return (random_number.front () <= fraction.first);
}

void
RPG_Dice::rollToRange (const struct RPG_Dice_Roll& roll_in,
                       struct RPG_Dice_ValueRange& valueRange_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::rollToRange"));

  valueRange_out.begin =
    roll_in.modifier + (roll_in.typeDice != D_0 ? roll_in.numDice : 0);
  valueRange_out.end = roll_in.modifier + (roll_in.numDice * roll_in.typeDice);
}

void
RPG_Dice::rangeToRoll (const struct RPG_Dice_ValueRange& valueRange_in,
                       struct RPG_Dice_Roll& roll_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::rangeToRoll"));

  // initialize result
  roll_out.numDice = 0;
  roll_out.typeDice = D_0;
  roll_out.modifier = 0;

  ACE_ASSERT (valueRange_in.begin <= valueRange_in.end);

  RPG_Dice_SortedRolls_t sortedRolls;
  // step1b: find SMALLEST type of die LARGER than range.end to start with
  enum RPG_Dice_DieType current_dieType = D_100;
  if (valueRange_in.end < D_100)
  {
    while (current_dieType > valueRange_in.end)
      current_dieType--;
    current_dieType++;
  } // end IF

  // step2: find best results for all valid types of dice, starting at this root
  do
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("current die type: \"%s\"\n"),
//                RPG_Dice_Common_Tools::diceTypeToString(current_dieType).c_str()));

    struct RPG_Dice_Roll result;
    result.numDice = 0;
    result.typeDice = current_dieType;
    result.modifier = 0;
    struct RPG_Dice_ValueRange range = valueRange_in;
    do
    {
      if ((current_dieType == D_0) ||
          ((range.begin == 0) && (range.end == 0))) // handle special (corner) case gracefully...
      {
        // (already) at D_0 --> compute (static) modifier
        break;
      } // end IF

      result.numDice++;
      range.begin -= 1;
      range.end -= current_dieType;
    } while ((range.end >= current_dieType) &&
             (range.begin != range.end)); // <-- this means we're finished !

    // compute the modifier (*IMPORTANT NOTE*: this MAY be negative !)...
    if (range.begin == range.end) // perfect match !
      result.modifier = range.begin;
    else
    {
      // cannot match the requested range --> approximate
      result.modifier = (std::max (::abs (range.begin),
                                   ::abs(range.end)) -
                         std::min (::abs (range.begin),
                                   ::abs (range.end))) / 2;

      if (current_dieType == D_0)
        result.modifier += valueRange_in.begin; // add the smaller value

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("approximated modifier: %d\n"),
//                  result.modifier));
    } // end ELSE

    // remember result
    struct rangeToRollElement element;
    element.range = valueRange_in;
    element.roll = result;
    sortedRolls.insert (element);

    // finished ?
    if (current_dieType == D_0)
      break; // finished !
    else
      current_dieType--;
  } while (true);

  // return the best result (top of the list !)
  roll_out = (*sortedRolls.begin ()).roll;

  // debug info: test if we have found a match
  RPG_Dice_ValueRange possible_range;
  rollToRange(roll_out, possible_range);
  unsigned int distance = distanceRangeToRange (possible_range, valueRange_in);
  if (distance)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to match range: %s, best result was: %s (distance: %d)...\n"),
                ACE_TEXT (RPG_Dice_Common_Tools::toString (valueRange_in).c_str ()),
                ACE_TEXT (RPG_Dice_Common_Tools::toString (roll_out).c_str ()),
                distance));

    // print all results...
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("results for range \"%s\"\n"),
                ACE_TEXT (RPG_Dice_Common_Tools::toString (valueRange_in).c_str ())));
    bool perfect_match = true;
    int index = 1;
    RPG_Dice_ValueRange range;
    std::string index_string, range_string;
    for (RPG_Dice_SortedRollsIterator_t iterator = sortedRolls.begin ();
         iterator != sortedRolls.end ();
         iterator++, index++)
    {
      perfect_match = true;
      index_string.clear ();
      range_string.clear ();
      rollToRange ((*iterator).roll, range);
      distance = distanceRangeToRange (range, valueRange_in);
      if (distance)
      {
       // not a perfect match...
        perfect_match = false;
        range_string = ACE_TEXT (" --> ");
        range_string += RPG_Dice_Common_Tools::toString (range);
      } // end IF

      index_string = ACE_TEXT ("[");
      index_string += (perfect_match ? ACE_TEXT ("*]") : ACE_TEXT ("]"));
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: \"%s\"%s\n"),
                  ACE_TEXT (index_string.c_str ()),
                  ACE_TEXT (RPG_Dice_Common_Tools::toString ((*iterator).roll).c_str ()),
                  ACE_TEXT (range_string.c_str ())));
    } // end FOR
  } // end IF
}

std::pair<unsigned int, unsigned int>
RPG_Dice::farey (float decimal_in,
                 float epsilon_in,
                 unsigned int maxNominator_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice::farey"));

  unsigned int a = 0;
  unsigned int b = 1;
  unsigned int c = 1;
  unsigned int d = 1;
  float mediant = 0.0f;

  while ((b <= maxNominator_in) && (d <= maxNominator_in))
  {
    mediant = static_cast<float> (a + c) / static_cast<float> (b + d);
    if (std::fabs (decimal_in - mediant) < epsilon_in)
    {
      if (b + d <= maxNominator_in)
        return std::make_pair (a + c, b + d);
      else if (d > b)
        return std::make_pair (c, d);
      else
        return std::make_pair (a, b);
    } // end IF
    else if (decimal_in > mediant)
    {
      a = a + c;
      b = b + d;
    } // end ELSEIF
    else
    {
      c = a + c;
      d = b + d;
    } // end ELSE
  } // end WHILE

  if (b > maxNominator_in)
    return std::make_pair (c, d);

  return std::make_pair (a, b);
}
