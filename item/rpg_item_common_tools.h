/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns                                      *
 *   erik.sohns@web.de                                                     *
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
#ifndef RPG_ITEM_COMMON_TOOLS_H
#define RPG_ITEM_COMMON_TOOLS_H

#include "rpg_item_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Common_Tools
{
 public:
  static void initStringConversionTables();

  static const std::string weaponDamageToString(const RPG_Item_WeaponDamage&); // weapon damage
  static const std::string damageToString(const RPG_Item_Damage&); // damage

  static const bool isShield(const RPG_Item_ArmorType&); // armor type

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools(const RPG_Item_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools& operator=(const RPG_Item_Common_Tools&));
};

#endif
