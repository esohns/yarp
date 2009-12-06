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
#include "rpg_chance_common_tools.h"

#include "rpg_chance_dicetype.h"
#include "rpg_chance_diceroll.h"
#include "rpg_chance_dice.h"

#include <ace/Log_Msg.h>

const int RPG_Chance_Common_Tools::getCheck(const short int& modifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Common_Tools::getCheck"));

  RPG_Chance_DiceRoll roll;
  roll.numDice = 1;
  roll.typeDice = D_20;
  roll.modifier = modifier_in;
  RPG_Chance_DiceRollResult_t result;
  RPG_Chance_Dice::simulateDiceRoll(roll,
                                    1,
                                    result);

  return result.front();
}
