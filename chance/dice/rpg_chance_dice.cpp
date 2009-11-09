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
#include <ace/Trace.h>
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

void RPG_Chance_Dice::simulateDiceRoll(const RPG_Chance_Roll& rollSpecs_in,
                                       const unsigned int& numRolls_in,
                                       RPG_CHANCE_DICE_RESULT_T& results_out)
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
