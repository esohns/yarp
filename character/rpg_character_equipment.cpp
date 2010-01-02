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
#include <rpg_item_armor_base.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_common_tools.h>

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
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_WEAPONTYPE_INVALID;
  } // end IF

  ACE_ASSERT(handle->getType() == ITEM_WEAPON);
  RPG_Item_Weapon_Base* weapon_base = ACE_dynamic_cast(RPG_Item_Weapon_Base*,
                                                       handle);
  ACE_ASSERT(weapon_base);

  return weapon_base->getWeaponType();
}

const RPG_Item_ArmorType RPG_Character_Equipment::getArmor() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::getArmor"));

  // find item ID
  RPG_Item_ID_t id = 0;

  // *TODO*; consider helmets/gauntlets/boots/shields, etc...
  RPG_Character_EquipmentIterator_t iterator = myEquipment.find(EQUIPMENTSLOT_BODY);
  if (iterator == myEquipment.end())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("nothing equipped in slot (\"%s\"): returning \"%s\"...\n"),
               RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(EQUIPMENTSLOT_BODY).c_str(),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(ARMOR_NONE).c_str()));

    // nothing equipped --> default is "nakedness"...
    return ARMOR_NONE;
  } // end IF

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem((*iterator).second,
                                                                handle))
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_ARMORTYPE_INVALID;
  } // end IF

  ACE_ASSERT(handle->getType() == ITEM_ARMOR);
  RPG_Item_Armor_Base* armor_base = ACE_dynamic_cast(RPG_Item_Armor_Base*,
                                                     handle);
  ACE_ASSERT(armor_base);

  return armor_base->getArmorType();
}

const RPG_Item_ArmorType RPG_Character_Equipment::getShield() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::getShield"));

  // find item ID
  RPG_Item_ID_t id = 0;

  // *TODO*; consider case where shield is in right hand, etc...
  RPG_Character_EquipmentIterator_t iterator = myEquipment.find(EQUIPMENTSLOT_LEFT_HAND);
  if (iterator == myEquipment.end())
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("nothing equipped in slot (\"%s\"): returning \"%s\"...\n"),
               RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(EQUIPMENTSLOT_LEFT_HAND).c_str(),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(ARMOR_NONE).c_str()));

    // nothing equipped...
    return ARMOR_NONE;
  } // end IF

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem((*iterator).second,
       handle))
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_ARMORTYPE_INVALID;
  } // end IF

  ACE_ASSERT(handle->getType() == ITEM_ARMOR);
  RPG_Item_Armor_Base* armor_base = ACE_dynamic_cast(RPG_Item_Armor_Base*,
                                                     handle);
  ACE_ASSERT(armor_base);
  if (!RPG_Item_Common_Tools::isShield(armor_base->getArmorType()))
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("equipped armor (type: \"%s\") is not a shield: returning \"%s\"...\n"),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor_base->getArmorType()).c_str(),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(ARMOR_NONE).c_str()));

    // some kind of armor is equipped, but it's not a shield...
    return ARMOR_NONE;
  } // end IF

  return armor_base->getArmorType();
}
