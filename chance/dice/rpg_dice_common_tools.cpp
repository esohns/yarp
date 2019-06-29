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

#include "rpg_dice_common_tools.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "rpg_dice_dietype.h"

#include "rpg_common_macros.h"

// initialize statics
RPG_Dice_DieTypeToStringTable_t RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToStringTable;

void
RPG_Dice_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice_Common_Tools::initializeStringConversionTables"));

  RPG_Dice_DieTypeHelper::initialize ();
#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("RPG_Dice_Common_Tools: initialized string conversion table...\n")));
#endif // _DEBUG
}

std::string
RPG_Dice_Common_Tools::toString (const RPG_Dice_Roll& roll_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice_Common_Tools::rollToString"));

  std::string result;
  std::stringstream converter;

  converter << roll_in.numDice;
  result += converter.str ();
  result += RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString (roll_in.typeDice);

  if (roll_in.modifier == 0)
    return result;
  else if (roll_in.modifier > 0)
    result += ACE_TEXT_ALWAYS_CHAR ("+");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << roll_in.modifier;
  result += converter.str ();

  return result;
}

std::string
RPG_Dice_Common_Tools::toString (const RPG_Dice_ValueRange& range_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Dice_Common_Tools::rangeToString"));

  std::string result;
  std::stringstream converter;

  converter << range_in.begin;
  if (range_in.begin != range_in.end)
  {
    converter << ACE_TEXT_ALWAYS_CHAR("-");
    converter << range_in.end;
  } // end IF
  result = converter.str ();

  return result;
}
