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

#ifndef RPG_DICE_COMMON_TOOLS_H
#define RPG_DICE_COMMON_TOOLS_H

#include <string>

#include "ace/Global_Macros.h"

#include "rpg_dice_dietype.h"
#include "rpg_dice_exports.h"
#include "rpg_dice_roll.h"
#include "rpg_dice_valuerange.h"

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Dice_Export RPG_Dice_Common_Tools
{
 public:
  // initialize string tables
  static void initStringConversionTables ();

  static const std::string rollToString (const RPG_Dice_Roll&); // roll
  static const std::string rangeToString (const RPG_Dice_ValueRange&); // range

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Dice_Common_Tools ());
  ACE_UNIMPLEMENTED_FUNC (~RPG_Dice_Common_Tools ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Dice_Common_Tools (const RPG_Dice_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Dice_Common_Tools& operator= (const RPG_Dice_Common_Tools&));
};

#endif
