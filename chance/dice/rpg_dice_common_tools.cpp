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

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Dice_Common_Tools: initialized string conversion table...\n")));
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

//////////////////////////////////////////

RPG_Dice_DieType&
operator++ (RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
//    case D_0: dieType_inout = D_4; break;
    case D_0: dieType_inout = D_2; break;
    case D_2: dieType_inout = D_3; break;
    case D_3: dieType_inout = D_4; break;
    case D_4: dieType_inout = D_6; break;
    case D_6: dieType_inout = D_8; break;
    case D_8: dieType_inout = D_10; break;
    case D_10: dieType_inout = D_12; break;
    case D_12: dieType_inout = D_20; break;
    case D_20: dieType_inout = D_100; break;
    case D_100: dieType_inout = D_0; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return dieType_inout;
};

RPG_Dice_DieType
operator++ (RPG_Dice_DieType& dieType_inout, int)
{
  RPG_Dice_DieType result = ++dieType_inout;

  return result;
};

RPG_Dice_DieType&
operator-- (RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
    case D_0: dieType_inout = D_100; break;
    case D_2: dieType_inout = D_0; break;
    case D_3: dieType_inout = D_2; break;
//    case D_4: dieType_inout = D_0; break;
    case D_4: dieType_inout = D_3; break;
    case D_6: dieType_inout = D_4; break;
    case D_8: dieType_inout = D_6; break;
    case D_10: dieType_inout = D_8; break;
    case D_12: dieType_inout = D_10; break;
    case D_20: dieType_inout = D_12; break;
    case D_100: dieType_inout = D_20; break;
    default: ACE_ASSERT (false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return dieType_inout;
};

enum RPG_Dice_DieType
operator-- (enum RPG_Dice_DieType& dieType_inout, int)
{
  RPG_Dice_DieType result = --dieType_inout;

  return result;
};

//////////////////////////////////////////

RPG_Dice_Roll&
RPG_Dice_Roll::operator*= (int multiplier_in)
{
  numDice *= multiplier_in;
  modifier *= multiplier_in;

  return (*this);
};
