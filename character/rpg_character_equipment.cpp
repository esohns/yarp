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
#include "rpg_character_equipment.h"

#include <rpg_item_base.h>
#include <rpg_item_weapon_base.h>
#include <rpg_item_instance_manager.h>

#include <ace/Log_Msg.h>

RPG_Character_Equipment::RPG_Character_Equipment()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::RPG_Character_Equipment"));

}

RPG_Character_Equipment::~RPG_Character_Equipment()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::RPG_Character_Equipment"));

}

const bool RPG_Character_Equipment::equip(const RPG_Item_ID_t& itemID_in,
                                          const RPG_Character_EquipmentSlot& slot_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::equip"));

  myEquipment.insert(std::make_pair(slot_in, itemID_in));
}

const bool RPG_Character_Equipment::unequip(const RPG_Character_EquipmentSlot& slot_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::unequip"));

  myEquipment.erase(slot_in);
}

const RPG_Item_WeaponType RPG_Character_Equipment::getPrimaryWeapon() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::getPrimaryWeapon"));

  // find item ID
  RPG_Item_ID_t id = 0;

  // *TODO*; consider ambidexterity/left-right-handedness, etc...
  RPG_Character_EquipmentIterator_t iterator = myEquipment.find(EQUIPMENTSLOT_RIGHT_HAND);
  if (iterator == myEquipment.end())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("nothing equipped in slot (\"%s\"): returning \"%s\"...\n"),
               RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(EQUIPMENTSLOT_RIGHT_HAND).c_str(),
               RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(UNARMED_WEAPON_STRIKE).c_str()));

    // nothing equipped --> default is the bare fist...
    return UNARMED_WEAPON_STRIKE;
  } // end IF

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem((*iterator).second,
                                                                handle))
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d) aborting\n"),
               (*iterator).second));

    return RPG_ITEM_WEAPONTYPE_INVALID;
  } // end IF

  ACE_ASSERT(handle->getType() == ITEM_WEAPON);
  RPG_Item_Weapon_Base* weapon_base = ACE_dynamic_cast(RPG_Item_Weapon_Base*,
                                                       handle);
  ACE_ASSERT(weapon_base);

  return weapon_base->getWeaponType();
}
