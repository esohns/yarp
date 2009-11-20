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
#ifndef RPG_CHANCE_DICE_COMMON_TOOLS_H
#define RPG_CHANCE_DICE_COMMON_TOOLS_H

#include "rpg_chance_dice_common.h"

#include <ace/Global_Macros.h>

#include <map>
#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Chance_Dice_Common_Tools
{
 public:
  // init string tables
  static void initStringConversionTables();

  static const RPG_Chance_DiceType stringToDiceType(const std::string&); // string
  static const std::string diceTypeToString(const RPG_Chance_DiceType&); // dice type
  static const std::string rollToString(const RPG_Chance_DiceRoll&); // roll
  static const std::string rangeToString(const RPG_Chance_ValueRange&); // range

 private:
  // some handy types
  typedef std::map<std::string, RPG_Chance_DiceType> RPG_Chance_Dice_StringToDiceType_t;
  typedef RPG_Chance_Dice_StringToDiceType_t::const_iterator RPG_Chance_Dice_StringToDiceTypeIterator_t;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Chance_Dice_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice_Common_Tools(const RPG_Chance_Dice_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Chance_Dice_Common_Tools& operator=(const RPG_Chance_Dice_Common_Tools&));

  static RPG_Chance_Dice_StringToDiceType_t myStringToDiceTypeTable;
};

#endif
