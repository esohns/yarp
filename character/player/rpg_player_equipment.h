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

#ifndef RPG_PLAYER_EQUIPMENT_H
#define RPG_PLAYER_EQUIPMENT_H

#include "rpg_player_exports.h"
#include "rpg_player_equipment_common.h"

#include <rpg_character_offhand.h>
#include <rpg_character_equipmentslot.h>

#include <rpg_item_instance_common.h>
#include <rpg_item_weapontype.h>
#include <rpg_item_armortype.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Player_Export RPG_Player_Equipment
{
 public:
  RPG_Player_Equipment();
  virtual ~RPG_Player_Equipment();

  // *NOTE*: equipped items SHALL be part of the inventory !
  void equip(const RPG_Item_ID_t&,                                    // item ID
             const RPG_Character_OffHand&,                            // off-hand
             const RPG_Character_EquipmentSlot& = EQUIPMENTSLOT_ANY); // hint: where ?
  void unequip(const RPG_Item_ID_t&); // what ?
  void unequip(const RPG_Character_EquipmentSlot&); // where ?
  void strip();

  // weapon in EQUIPMENTSLOT_RIGHT_HAND/EQUIPMENTSLOT_LEFT_HAND
  RPG_Item_WeaponType getPrimaryWeapon(const RPG_Character_OffHand&) const;
  RPG_Item_WeaponType getSecondaryWeapon(const RPG_Character_OffHand&) const;
  // armor in EQUIPMENTSLOT_BODY/TORSO
  RPG_Item_ArmorType getBodyArmor() const;
  // armor in EQUIPMENTSLOT_LEFT/RIGHT_HAND, IF ARMOR (!)
  RPG_Item_ArmorType getShield(const RPG_Character_OffHand&) const;
  // light source in EQUIPMENTSLOT_LEFT/RIGHT_HAND, if any
  RPG_Item_CommodityLight getLightSource() const;

  bool isEquipped(const RPG_Character_EquipmentSlot&, // slot
                  RPG_Item_ID_t&) const;              // return value: item ID (if any)
  bool isEquipped(const RPG_Item_ID_t&,                // item ID
                  RPG_Character_EquipmentSlot&) const; // return value: slot (if any)

  // dump equipment
  void dump() const;

  // *WARNING*: do NOT insert/remove elements directly (use the API instead)
  RPG_Player_Equipment_t myEquipment;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Player_Equipment& operator=(const RPG_Player_Equipment&));
};

#endif
