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
#include "rpg_chance_dice.h"

#include "rpg_chance_dice_common_tools.h"

#include <ace/OS.h>
#include <ace/Log_Msg.h>

RPG_Chance_Dice::RPG_Chance_Dice()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::RPG_Chance_Dice"));

}

RPG_Chance_Dice::~RPG_Chance_Dice()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::~RPG_Chance_Dice"));

}

void RPG_Chance_Dice::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::init"));

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

void RPG_Chance_Dice::generateRandomNumbers(const unsigned int& range_in,
                                            const unsigned int& numRolls_in,
                                            RPG_Chance_DiceRollResult_t& results_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::generateRandomNumbers"));

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

void RPG_Chance_Dice::simulateDiceRoll(const RPG_Chance_DiceRoll& rollSpecs_in,
                                       const unsigned int& numRolls_in,
                                       RPG_Chance_DiceRollResult_t& results_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::simulateDiceRoll"));

  // init result(s)
  results_out.clear();

//   // debug info
//   std::string die = RPG_Chance_Dice_Common_Tools::diceType2String(rollSpecs_in.typeDice);
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

void RPG_Chance_Dice::diceRollToRange(const RPG_Chance_DiceRoll& diceRoll_in,
                                      RPG_Chance_ValueRange& valueRange_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::diceRollToRange"));

  valueRange_out.begin = diceRoll_in.modifier + (diceRoll_in.typeDice != D_0 ? diceRoll_in.numDice : 0);
  valueRange_out.end = diceRoll_in.modifier + (diceRoll_in.numDice * diceRoll_in.typeDice);
}

void RPG_Chance_Dice::rangeToDiceRoll(const RPG_Chance_ValueRange& valueRange_in,
                                      RPG_Chance_DiceRoll& diceRoll_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::rangeToDiceRoll"));

  // init result
  diceRoll_out.numDice = 0;
  diceRoll_out.typeDice = D_0;
  diceRoll_out.modifier = 0;

  ACE_ASSERT(valueRange_in.begin <= valueRange_in.end);

  RPG_Chance_Dice_SortedRolls_t sortedRolls;
  // step1b: find SMALLEST type of die LARGER than range.end to start with
  RPG_Chance_DiceType current_dieType = D_100;
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
//                RPG_Chance_Dice_Common_Tools::diceTypeToString(current_dieType).c_str()));

    RPG_Chance_DiceRoll result;
    result.numDice = 0;
    result.typeDice = current_dieType;
    result.modifier = 0;
    RPG_Chance_ValueRange range = valueRange_in;
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
    };
  } while (true);

  // return the best result (top of the list !)
  diceRoll_out = (*sortedRolls.begin()).roll;

  // debug info: test if we have found a match
  RPG_Chance_ValueRange possible_range;
  diceRollToRange(diceRoll_out, possible_range);
  unsigned int distance = distanceRangeToRange(possible_range, valueRange_in);
  if (distance)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("failed to match range: %s, best result was: %s (distance: %d)...\n"),
               RPG_Chance_Dice_Common_Tools::rangeToString(valueRange_in).c_str(),
               RPG_Chance_Dice_Common_Tools::rollToString(diceRoll_out).c_str(),
               distance));

    // print all results...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("results for range \"%s\"\n"),
               RPG_Chance_Dice_Common_Tools::rangeToString(valueRange_in).c_str()));
    bool perfect_match = true;
    int index = 1;
    RPG_Chance_ValueRange range;
    std::string index_string, range_string;
    for (RPG_Chance_Dice_SortedRollsIterator_t iterator = sortedRolls.begin();
         iterator != sortedRolls.end();
         iterator++, index++)
    {
      perfect_match = true;
      index_string.clear();
      range_string.clear();
      RPG_Chance_Dice::diceRollToRange((*iterator).roll, range);
      distance = distanceRangeToRange(range, valueRange_in);
      if (distance)
      {
       // not a perfect match...
        perfect_match = false;
        range_string = ACE_TEXT(" --> ");
        range_string += RPG_Chance_Dice_Common_Tools::rangeToString(range);
      } // end IF

      index_string = ACE_TEXT("[");
      index_string += (perfect_match ? ACE_TEXT("*]") : ACE_TEXT("]"));
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("%s: \"%s\"%s\n"),
                 index_string.c_str(),
                 RPG_Chance_Dice_Common_Tools::rollToString((*iterator).roll).c_str(),
                 range_string.c_str()));
    } // end FOR
  } // end IF
}

const unsigned int RPG_Chance_Dice::distanceRangeToRange(const RPG_Chance_ValueRange& rangeA_in,
                                                         const RPG_Chance_ValueRange& rangeB_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Dice::distanceRangeToRange"));

  return (::abs(rangeA_in.begin - rangeB_in.begin) +
          ::abs(rangeA_in.end - rangeB_in.end));
}
