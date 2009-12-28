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
#include "rpg_dice.h"

#include "rpg_dice_common_tools.h"

#include <ace/Time_Value.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>
#include <ace/Assert.h>

#include <stdlib.h>
#include <algorithm>
#include <string>

void RPG_Dice::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Dice::init"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initializing random seed (RAND_MAX = %d)...\n"),
             RAND_MAX));

  // *PORTABILITY*: this is most probably not portable...
  ACE_Time_Value now = ACE_OS::gettimeofday();
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  ::srandom(now.sec());
#else
  // *TODO*
  ACE_ASSERT(false);
#endif

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("initializing random seed...DONE\n")));
}

void RPG_Dice::generateRandomNumbers(const unsigned int& range_in,
                                     const unsigned int& numRolls_in,
                                     RPG_Dice_RollResult_t& results_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Dice::generateRandomNumbers"));

  // init result(s)
  results_out.clear();

  for (unsigned int i = 0;
       i < numRolls_in;
       i++)
  {
    // *PORTABILITY*: this is most probably not portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    results_out.push_back((::random() % range_in) + 1);
#else
    // *TODO*
    ACE_ASSERT(false);
#endif
  } // end FOR
}

void RPG_Dice::simulateRoll(const RPG_Dice_Roll& rollSpecs_in,
                            const unsigned int& numRolls_in,
                            RPG_Dice_RollResult_t& results_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Dice::simulateRoll"));

  // init result(s)
  results_out.clear();

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
    // *PORTABILITY*: this is most probably not portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      tempResult += ((::random() % rollSpecs_in.typeDice) + 1);
#else
    // *TODO*
      ACE_ASSERT(false);
#endif
    } // end FOR

    results_out.push_back(tempResult + rollSpecs_in.modifier);
  } // end FOR
}

void RPG_Dice::rollToRange(const RPG_Dice_Roll& roll_in,
                           RPG_Dice_ValueRange& valueRange_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Dice::rollToRange"));

  valueRange_out.begin = roll_in.modifier + (roll_in.typeDice != D_0 ? roll_in.numDice : 0);
  valueRange_out.end = roll_in.modifier + (roll_in.numDice * roll_in.typeDice);
}

void RPG_Dice::rangeToRoll(const RPG_Dice_ValueRange& valueRange_in,
                           RPG_Dice_Roll& roll_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Dice::rangeToRoll"));

  // init result
  roll_out.numDice = 0;
  roll_out.typeDice = D_0;
  roll_out.modifier = 0;

  ACE_ASSERT(valueRange_in.begin <= valueRange_in.end);

  RPG_Dice_SortedRolls_t sortedRolls;
  // step1b: find SMALLEST type of die LARGER than range.end to start with
  RPG_Dice_DieType current_dieType = D_100;
  if (valueRange_in.end < D_100)
  {
    while (current_dieType > valueRange_in.end)
    {
      current_dieType--;
    };
    current_dieType++;
  } // end IF

  // step2: find best results for all valid types of dice, starting at this root
  do
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("current die type: \"%s\"\n"),
//                RPG_Dice_Common_Tools::diceTypeToString(current_dieType).c_str()));

    RPG_Dice_Roll result;
    result.numDice = 0;
    result.typeDice = current_dieType;
    result.modifier = 0;
    RPG_Dice_ValueRange range = valueRange_in;
    do
    {
      if ((current_dieType == D_0) ||
          ((range.begin == 0) && (range.end == 0))) // handle special (corner) case gracefully...
      {
        // we're (already) at D_0 --> compute (static) modifier
        break;
      } // end IF

      result.numDice++;
      range.begin -= 1;
      range.end -= current_dieType;
    } while ((range.end >= current_dieType) &&
             (range.begin != range.end)); // <-- this means we're finished !

    // compute the modifier (*IMPORTANT NOTE*: this MAY be negative !)...
    if (range.begin == range.end) // perfect match !
    {
      result.modifier = range.begin;
    } // end IF
    else
    {
      // cannot match the requested range --> approximate
      result.modifier = (std::max(::abs(range.begin),
                                  ::abs(range.end)) -
                         std::min(::abs(range.begin),
                                  ::abs(range.end))) / 2;

      if (current_dieType == D_0)
      {
        // add the smaller value
        result.modifier += valueRange_in.begin;
      } // end IF

//       // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("approximated modifier: %d\n"),
//                  result.modifier));
    } // end ELSE

    // remember result
    rangeToRollElement element;
    element.range = valueRange_in;
    element.roll = result;
    sortedRolls.insert(element);

    // finished ?
    if (current_dieType == D_0)
    {
      // finished !
      break;
    } // end IF
    else
    {
      current_dieType--;
    }
  } while (true);

  // return the best result (top of the list !)
  roll_out = (*sortedRolls.begin()).roll;

  // debug info: test if we have found a match
  RPG_Dice_ValueRange possible_range;
  rollToRange(roll_out, possible_range);
  unsigned int distance = distanceRangeToRange(possible_range, valueRange_in);
  if (distance)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("failed to match range: %s, best result was: %s (distance: %d)...\n"),
               RPG_Dice_Common_Tools::rangeToString(valueRange_in).c_str(),
               RPG_Dice_Common_Tools::rollToString(roll_out).c_str(),
               distance));

    // print all results...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("results for range \"%s\"\n"),
               RPG_Dice_Common_Tools::rangeToString(valueRange_in).c_str()));
    bool perfect_match = true;
    int index = 1;
    RPG_Dice_ValueRange range;
    std::string index_string, range_string;
    for (RPG_Dice_SortedRollsIterator_t iterator = sortedRolls.begin();
         iterator != sortedRolls.end();
         iterator++, index++)
    {
      perfect_match = true;
      index_string.clear();
      range_string.clear();
      rollToRange((*iterator).roll, range);
      distance = distanceRangeToRange(range, valueRange_in);
      if (distance)
      {
       // not a perfect match...
        perfect_match = false;
        range_string = ACE_TEXT(" --> ");
        range_string += RPG_Dice_Common_Tools::rangeToString(range);
      } // end IF

      index_string = ACE_TEXT("[");
      index_string += (perfect_match ? ACE_TEXT("*]") : ACE_TEXT("]"));
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("%s: \"%s\"%s\n"),
                 index_string.c_str(),
                 RPG_Dice_Common_Tools::rollToString((*iterator).roll).c_str(),
                 range_string.c_str()));
    } // end FOR
  } // end IF
}

const unsigned int RPG_Dice::distanceRangeToRange(const RPG_Dice_ValueRange& rangeA_in,
                                                  const RPG_Dice_ValueRange& rangeB_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Dice::distanceRangeToRange"));

  return (::abs(rangeA_in.begin - rangeB_in.begin) +
          ::abs(rangeA_in.end - rangeB_in.end));
}
