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

#include "rpg_item_exports.h"
#include "rpg_item_common.h"
#include "rpg_item_commoditybeverage.h"
#include "rpg_item_commoditylight.h"
#include "rpg_item_commodityunion.h"

#include <rpg_common.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Export RPG_Item_Common_Tools
{
 public:
  static void initStringConversionTables();

  static std::string weaponDamageTypeToString(const RPG_Item_WeaponDamageType&); // weapon damage
  static RPG_Common_PhysicalDamageList_t weaponDamageTypeToPhysicalDamageType(const RPG_Item_WeaponDamageType&); // weapon damage
  static std::string damageToString(const RPG_Item_Damage&); // damage

  // XML-specifics
  static std::string commoditySubTypeToXMLString(const RPG_Item_CommodityUnion&); // subtype
  static RPG_Item_CommodityUnion XMLStringToCommoditySubType(const std::string&); // subtype (XML string)

  static bool isThrownWeapon(const RPG_Item_WeaponType&); // weapon type
  static bool isProjectileWeapon(const RPG_Item_WeaponType&); // weapon type
  static bool isRangedWeapon(const RPG_Item_WeaponType&); // weapon type
  static bool isTwoHandedWeapon(const RPG_Item_WeaponType&); // weapon type
  static bool isMeleeWeapon(const RPG_Item_WeaponType&); // weapon type

  static bool isShield(const RPG_Item_ArmorType&); // armor type

  // *TODO*: consider shape of the light
  static unsigned char lightingItem2Radius(const RPG_Item_CommodityLight&, // lighting item type
                                           const bool& = true);            // ambience is "bright" ?

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools(const RPG_Item_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools& operator=(const RPG_Item_Common_Tools&));
};

#endif
