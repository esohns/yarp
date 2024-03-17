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

#include <string>

#include "ace/Global_Macros.h"

#include "rpg_common.h"

#include "rpg_character_common.h"
#include "rpg_character_offhand.h"

#include "rpg_item_common.h"
#include "rpg_item_commoditybeverage.h"
#include "rpg_item_commoditylight.h"
#include "rpg_item_commodityunion.h"
#include "rpg_item_instance_common.h"

class RPG_Item_Common_Tools
{
 public:
  static void initializeStringConversionTables ();

  static std::string toString (const RPG_Item_WeaponDamageType&); // weapon damage
  static RPG_Common_PhysicalDamageList_t weaponDamageTypeToPhysicalDamageType (const RPG_Item_WeaponDamageType&); // weapon damage
  static std::string toString (const RPG_Item_Damage&); // damage

  // XML-specifics
  static std::string commoditySubTypeToXMLString (const RPG_Item_CommodityUnion&); // subtype
  static RPG_Item_CommodityUnion XMLStringToCommoditySubType (const std::string&); // subtype (XML string)

  static bool isThrownWeapon (enum RPG_Item_WeaponType); // weapon type
  static bool isProjectileWeapon (enum RPG_Item_WeaponType); // weapon type
  static bool isRangedWeapon (enum RPG_Item_WeaponType); // weapon type
  static bool isTwoHandedWeapon (enum RPG_Item_WeaponType); // weapon type
  static bool isMeleeWeapon (enum RPG_Item_WeaponType); // weapon type

  static bool isShield (enum RPG_Item_ArmorType); // armor type

  static bool hasAbsoluteReach (enum RPG_Item_WeaponType); // weapon type

  // *TODO*: consider shape of the light
  static ACE_UINT8 lightingItemToRadius (enum RPG_Item_CommodityLight, // lighting item type
                                         bool = true);                 // ambience is "bright" ?

  static void itemToSlot (RPG_Item_ID_t,                  // item id
                          enum RPG_Character_OffHand,     // off-hand
                          RPG_Character_EquipmentSlots&); // return value: possible slot(s)

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Item_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Common_Tools (const RPG_Item_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Common_Tools& operator= (const RPG_Item_Common_Tools&))
};

#endif
