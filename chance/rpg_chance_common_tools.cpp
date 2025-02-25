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

#include "rpg_chance_common_tools.h"

#include "ace/Log_Msg.h"

#include "rpg_dice_common.h"
#include "rpg_dice_roll.h"
#include "rpg_dice.h"

#include "rpg_common_macros.h"

int
RPG_Chance_Common_Tools::getCheck (ACE_INT16 modifier_in,
                                   enum RPG_Dice_DieType dieType_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Chance_Common_Tools::getCheck"));

  struct RPG_Dice_Roll roll;
  roll.numDice = 1;
  roll.typeDice = dieType_in;
  roll.modifier = modifier_in;
  RPG_Dice_RollResult_t result;
  RPG_Dice::simulateRoll (roll,
                          1,
                          result);

  return result.front ();
}
