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

#include "rpg_player_equipment.h"

#include "rpg_player_equipment_common.h"

#include <rpg_item_base.h>
#include <rpg_item_armor.h>
#include <rpg_item_commodity.h>
#include <rpg_item_weapon.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_common_tools.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Player_Equipment::RPG_Player_Equipment()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::RPG_Player_Equipment"));

}

RPG_Player_Equipment::~RPG_Player_Equipment()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::RPG_Player_Equipment"));

}

void
RPG_Player_Equipment::equip(const RPG_Item_ID_t& itemID_in,
                            const RPG_Character_EquipmentSlot& slot_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::equip"));

  // sanity check(s)
  ACE_ASSERT(slot_in != RPG_CHARACTER_EQUIPMENTSLOT_INVALID);

  RPG_Character_EquipmentSlots slots;
  slots.is_inclusive = false;
  RPG_Player_EquipmentConstIterator_t equipment_iterator;

  // auto-choose appropriate slot ?
  if (slot_in == EQUIPMENTSLOT_ANY)
  {
    RPG_Item_Common_Tools::item2Slot(itemID_in, slots);
    ACE_ASSERT(!slots.slots.empty());

    if (!slots.is_inclusive)
    {
      // (try to) find a suitable (free !) slot
      RPG_Character_EquipmentSlot free_slot = RPG_CHARACTER_EQUIPMENTSLOT_INVALID;
      for (RPG_Character_EquipmentSlotsIterator_t iterator = slots.slots.begin();
           iterator != slots.slots.end();
           iterator++)
      {
        equipment_iterator = myEquipment.find(*iterator);
        if (equipment_iterator == myEquipment.end())
        {
          // --> found free slot
          free_slot = *iterator;

          break;
        } // end IF
      } // end FOR

      // no free slot ? --> unequip something
      if (free_slot == RPG_CHARACTER_EQUIPMENTSLOT_INVALID)
      {
        free_slot = slots.slots.front();
        equipment_iterator = myEquipment.find(free_slot);
        ACE_ASSERT(equipment_iterator != myEquipment.end());
        unequip((*equipment_iterator).second);

        slots.slots.clear();
        slots.slots.push_back(free_slot);
      } // end IF
    } // end IF
  } // end IF
  else
    slots.slots.push_back(slot_in);
  ACE_ASSERT(!slots.slots.empty());

  // debug info
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(itemID_in, handle))
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
                ACE_TEXT("invalid item ID (was: %u), aborting\n"),
                itemID_in));

    return;
  } // end IF
  ACE_ASSERT(handle);

  for (RPG_Character_EquipmentSlotsIterator_t iterator = slots.slots.begin();
       iterator != slots.slots.end();
       iterator++)
  {
    // occupied ? --> unequip first
    unequip(*iterator);

    myEquipment.insert(std::make_pair(*iterator, itemID_in));

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("equipped item (ID: %d, type: \"%s\") @ \"%s\"\n"),
               itemID_in,
               RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str(),
               RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(*iterator).c_str()));
  } // end FOR
}

void
RPG_Player_Equipment::unequip(const RPG_Item_ID_t& itemID_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::unequip"));

  RPG_Player_EquipmentConstIterator_t iterator;
  while (true)
  {
    for (iterator = myEquipment.begin();
         iterator != myEquipment.end();
         iterator++)
      if ((*iterator).second == itemID_in)
        break;

    // found an entry ?
    if (iterator != myEquipment.end())
    {
      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("unequipped \"%s\"\n"),
                 RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str()));
      
      myEquipment.erase(iterator);

      continue;
    } // end IF

    break; // ...done
  } // end WHILE
}

void
RPG_Player_Equipment::unequip(const RPG_Character_EquipmentSlot& slot_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::unequip"));

  // sanity check(s)
  if (slot_in == EQUIPMENTSLOT_ANY)
  {
    strip();

    return;
  } // end IF
  RPG_Player_EquipmentConstIterator_t iterator = myEquipment.find(slot_in);
  if (iterator == myEquipment.end())
    return;

  myEquipment.erase(iterator);

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("unequipped \"%s\"\n"),
             RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(slot_in).c_str()));
}

void
RPG_Player_Equipment::strip()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::strip"));

  myEquipment.clear();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("unequipped ALL\n")));
}

RPG_Item_WeaponType
RPG_Player_Equipment::getPrimaryWeapon(const RPG_Character_OffHand& offHand_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::getPrimaryWeapon"));

  RPG_Character_EquipmentSlot slot = ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_RIGHT
                                                                   : EQUIPMENTSLOT_HAND_LEFT);

  // *TODO*; consider ambidexterity, etc...
  RPG_Player_EquipmentConstIterator_t iterator = myEquipment.find(slot);
  if (iterator == myEquipment.end())
    return UNARMED_WEAPON_STRIKE; // nothing equipped --> default is the bare fist...

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get((*iterator).second,
                                                            handle))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_WEAPONTYPE_INVALID;
  } // end IF

  // weapon ?
  if (handle->getType() != ITEM_WEAPON)
  {
    // equipped item is not a weapon...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("equipped item (ID: %d, type: \"%s\") is not a weapon: aborting\n"),
               (*iterator).second,
               RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str()));

    return RPG_ITEM_WEAPONTYPE_INVALID;
  } // end IF

  RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(handle);
  ACE_ASSERT(weapon);

  return weapon->getWeaponType();
}

RPG_Item_ArmorType
RPG_Player_Equipment::getBodyArmor() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::getBodyArmor"));

  // *TODO*; consider helmets/gauntlets/boots/shields, etc...
  RPG_Player_EquipmentConstIterator_t iterator = myEquipment.find(EQUIPMENTSLOT_BODY);
  if (iterator == myEquipment.end())
  {
    // nothing equipped on body --> check torso
    iterator = myEquipment.find(EQUIPMENTSLOT_TORSO);
    if (iterator == myEquipment.end())
      return ARMOR_NONE; // nothing equipped --> default is "nakedness"...
  } // end IF

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get((*iterator).second,
                                                            handle))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_ARMORTYPE_INVALID;
  } // end IF

  // armor ?
  if (handle->getType() != ITEM_ARMOR)
    return ARMOR_NONE; // item is not an armor...

  RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(handle);
  ACE_ASSERT(armor);

  return armor->getArmorType();
}

RPG_Item_ArmorType
RPG_Player_Equipment::getShield(const RPG_Character_OffHand& offHand_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::getShield"));

  RPG_Character_EquipmentSlot slot = ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                                   : EQUIPMENTSLOT_HAND_RIGHT);

  // *TODO*; consider case where shield is in the primary hand, etc...
  RPG_Player_EquipmentConstIterator_t iterator = myEquipment.find(slot);
  if (iterator == myEquipment.end())
    return ARMOR_NONE; // nothing equipped...

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get((*iterator).second,
                                                            handle))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_ARMORTYPE_INVALID;
  } // end IF

  // armor ?
  if (handle->getType() != ITEM_ARMOR)
    return ARMOR_NONE; // item is not an armor...

  RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(handle);
  ACE_ASSERT(armor);

  // sanity check
  if (!RPG_Item_Common_Tools::isShield(armor->getArmorType()))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("equipped armor (ID: %d, type: \"%s\") is not a shield: returning \"%s\"...\n"),
               (*iterator).second,
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()).c_str(),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(ARMOR_NONE).c_str()));

    // some kind of armor is equipped here, but it's not a shield...
    return ARMOR_NONE;
  } // end IF

  return armor->getArmorType();
}

RPG_Item_CommodityLight
RPG_Player_Equipment::getLightSource() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::getLightSource"));

  RPG_Item_CommodityLight lightsource_1 = RPG_ITEM_COMMODITYLIGHT_INVALID;
  RPG_Item_CommodityLight lightsource_2 = RPG_ITEM_COMMODITYLIGHT_INVALID;
  RPG_Item_Base* handle = NULL;
  RPG_Item_Commodity* commodity_handle = NULL;
  RPG_Item_CommodityUnion commodity_union;
  RPG_Player_EquipmentConstIterator_t iterator = myEquipment.find(EQUIPMENTSLOT_HAND_LEFT);
  if (iterator != myEquipment.end())
  {
    // find item type
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get((*iterator).second,
                                                              handle))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), aborting\n"),
                 (*iterator).second));

      ACE_ASSERT(false);

      return RPG_ITEM_COMMODITYLIGHT_INVALID;
    } // end IF

    // commodity ?
    if (handle->getType() == ITEM_COMMODITY)
    {
      commodity_handle = NULL;
      commodity_handle = dynamic_cast<RPG_Item_Commodity*>(handle);
      ACE_ASSERT(commodity_handle);
      commodity_union = commodity_handle->getCommoditySubType();

      // light ?
      if (commodity_union.discriminator == RPG_Item_CommodityUnion::COMMODITYLIGHT)
        lightsource_1 = commodity_union.commoditylight;
    } // end IF
  } // end IF
  iterator = myEquipment.find(EQUIPMENTSLOT_HAND_RIGHT);
  if (iterator != myEquipment.end())
  {
    // find item type
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get((*iterator).second,
                                                              handle))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), aborting\n"),
                 (*iterator).second));

      ACE_ASSERT(false);

      return RPG_ITEM_COMMODITYLIGHT_INVALID;
    } // end IF

    // commodity ?
    if (handle->getType() == ITEM_COMMODITY)
    {
      commodity_handle = NULL;
      commodity_handle = dynamic_cast<RPG_Item_Commodity*>(handle);
      ACE_ASSERT(commodity_handle);
      commodity_union = commodity_handle->getCommoditySubType();

      // light ?
      if (commodity_union.discriminator == RPG_Item_CommodityUnion::COMMODITYLIGHT)
        lightsource_2 = commodity_union.commoditylight;
    } // end IF
  } // end IF

  if (lightsource_1 == RPG_ITEM_COMMODITYLIGHT_INVALID)
    return lightsource_2;
  else if (lightsource_2 == RPG_ITEM_COMMODITYLIGHT_INVALID)
    return lightsource_1;

  // return the brighter lightsource...
  unsigned char brightness_1 = RPG_Item_Common_Tools::lightingItem2Radius(lightsource_1);
  unsigned char brightness_2 = RPG_Item_Common_Tools::lightingItem2Radius(lightsource_2);

  return ((brightness_1 > brightness_2) ? lightsource_1 : lightsource_2);
}

bool
RPG_Player_Equipment::isEquipped(const RPG_Item_ID_t& itemID_in,
                                 RPG_Character_EquipmentSlot& slot_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::isEquipped"));

  // init return value(s)
  slot_out = RPG_CHARACTER_EQUIPMENTSLOT_INVALID;

  for (RPG_Player_EquipmentConstIterator_t iterator = myEquipment.begin();
       iterator != myEquipment.end();
       iterator++)
    if ((*iterator).second == itemID_in)
    {
      slot_out = (*iterator).first;

      return true;
    } // end IF

  return false;
}

void
RPG_Player_Equipment::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Equipment::dump"));

  RPG_Item_Base* handle = NULL;
  for (RPG_Player_EquipmentConstIterator_t iterator = myEquipment.begin();
       iterator != myEquipment.end();
       iterator++)
  {
    // find item type
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get((*iterator).second,
                                                              handle))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 (*iterator).second));

      continue;
    } // end IF

    switch (handle->getType())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(handle);
        ACE_ASSERT(armor);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("slot \"%s\" --> armor (ID: %d, type: %s)\n"),
                   RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str(),
                   (*iterator).second,
                   RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()).c_str()));

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = dynamic_cast<RPG_Item_Commodity*>(handle);
        ACE_ASSERT(commodity);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("slot \"%s\" --> commodity (ID: %d, type: %s)\n"),
                   RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str(),
                   (*iterator).second,
                   RPG_Item_CommodityTypeHelper::RPG_Item_CommodityTypeToString(commodity->getCommodityType()).c_str()));

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(handle);
        ACE_ASSERT(weapon);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("slot \"%s\" --> weapon (ID: %d, type: %s)\n"),
                   RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str(),
                   (*iterator).second,
                   RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon->getWeaponType()).c_str()));

        break;
      }
      default:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("slot \"%s\" --> item (ID: %d, type: %s)\n"),
                   RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str(),
                   (*iterator).second,
                   RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str()));

        break;
      }
    } // end SWITCH
  } // end FOR
}
