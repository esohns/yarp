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

RPG_Character_Equipment::RPG_Character_Equipment(const RPG_Character_Equipment& equipment_in)
 : myEquipment(equipment_in.myEquipment)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::RPG_Character_Equipment"));

}

RPG_Character_Equipment::~RPG_Character_Equipment()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::RPG_Character_Equipment"));

}

RPG_Character_Equipment& RPG_Character_Equipment::operator=(const RPG_Character_Equipment& equipment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::operator="));

  myEquipment = equipment_in.myEquipment;

  return *this;
}

void RPG_Character_Equipment::equip(const RPG_Item_ID_t& itemID_in,
                                    const RPG_Character_EquipmentSlot& slot_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::equip"));

  myEquipment.insert(std::make_pair(slot_in, itemID_in));

//   // debug info
//   RPG_Item_Base* handle = NULL;
//   if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem(itemID_in, handle))
//   {
//     // debug info
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("invalid item ID: %d, aborting\n"),
//                itemID_in));
//
//     return;
//   } // end IF
//   ACE_ASSERT(handle);
//
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("equipped item (ID: %d, type: \"%s\") @ \"%s\"\n"),
//              itemID_in,
//              RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str(),
//              RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(slot_in).c_str()));
}

void RPG_Character_Equipment::unequip(const RPG_Character_EquipmentSlot& slot_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::unequip"));

  myEquipment.erase(slot_in);

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("unequipped \"%s\"\n"),
//              RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString(slot_in).c_str()));
}

const RPG_Item_WeaponType RPG_Character_Equipment::getPrimaryWeapon(const RPG_Character_OffHand& offHand_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::getPrimaryWeapon"));

  RPG_Character_EquipmentSlot slot = ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_RIGHT_HAND
                                                                   : EQUIPMENTSLOT_LEFT_HAND);

  // *TODO*; consider ambidexterity, etc...
  RPG_Character_EquipmentIterator_t iterator = myEquipment.find(slot);
  if (iterator == myEquipment.end())
  {
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

  RPG_Item_Weapon_Base* weapon_base = ACE_dynamic_cast(RPG_Item_Weapon_Base*,
                                                       handle);
  ACE_ASSERT(weapon_base);

  return weapon_base->getWeaponType();
}

const RPG_Item_ArmorType RPG_Character_Equipment::getArmor() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::getArmor"));

  // *TODO*; consider helmets/gauntlets/boots/shields, etc...
  RPG_Character_EquipmentIterator_t iterator = myEquipment.find(EQUIPMENTSLOT_BODY);
  if (iterator == myEquipment.end())
  {
    // nothing equipped on body --> check torso
    iterator = myEquipment.find(EQUIPMENTSLOT_TORSO);
    if (iterator == myEquipment.end())
    {
      // nothing equipped --> default is "nakedness"...
      return ARMOR_NONE;
    } // end IF
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

  // armor ?
  if (handle->getType() != ITEM_ARMOR);
  {
    // item is not an armor...
    return ARMOR_NONE;
  } // end IF

  RPG_Item_Armor_Base* armor_base = ACE_dynamic_cast(RPG_Item_Armor_Base*, handle);
  ACE_ASSERT(armor_base);

  return armor_base->getArmorType();
}

const RPG_Item_ArmorType RPG_Character_Equipment::getShield(const RPG_Character_OffHand& offHand_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::getShield"));

  RPG_Character_EquipmentSlot slot = ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_LEFT_HAND
                                                                   : EQUIPMENTSLOT_RIGHT_HAND);

  // *TODO*; consider case where shield is in the primary hand, etc...
  RPG_Character_EquipmentIterator_t iterator = myEquipment.find(slot);
  if (iterator == myEquipment.end())
  {
    // nothing equipped...
    return ARMOR_NONE;
  } // end IF

  // find item type
  RPG_Item_Base* handle = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem((*iterator).second, handle))
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               (*iterator).second));

    return RPG_ITEM_ARMORTYPE_INVALID;
  } // end IF

  // armor ?
  if (handle->getType() != ITEM_ARMOR);
  {
    // item is not an armor...
    return ARMOR_NONE;
  } // end IF

  RPG_Item_Armor_Base* armor_base = ACE_dynamic_cast(RPG_Item_Armor_Base*, handle);
  ACE_ASSERT(armor_base);

  // sanity check
  if (!RPG_Item_Common_Tools::isShield(armor_base->getArmorType()))
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("equipped armor (ID: %d, type: \"%s\") is not a shield: returning \"%s\"...\n"),
               (*iterator).second,
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor_base->getArmorType()).c_str(),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(ARMOR_NONE).c_str()));

    // some kind of armor is equipped here, but it's not a shield...
    return ARMOR_NONE;
  } // end IF

  return armor_base->getArmorType();
}

void RPG_Character_Equipment::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Equipment::dump"));

  RPG_Item_Base* handle = NULL;
  for (RPG_Character_EquipmentIterator_t iterator = myEquipment.begin();
       iterator != myEquipment.end();
       iterator++)
  {
    // find item type
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem((*iterator).second, handle))
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 (*iterator).second));

      continue;
    } // end IF

    switch (handle->getType())
    {
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon_Base* weapon_base = ACE_dynamic_cast(RPG_Item_Weapon_Base*, handle);
        ACE_ASSERT(weapon_base);

        // debug info
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("slot \"%s\" --> weapon (ID: %d, type: %s)\n"),
                   RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str(),
                   (*iterator).second,
                   RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon_base->getWeaponType()).c_str()));

        break;
      }
      case ITEM_ARMOR:
      {
        RPG_Item_Armor_Base* armor_base = ACE_dynamic_cast(RPG_Item_Armor_Base*, handle);
        ACE_ASSERT(armor_base);

        // debug info
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("slot \"%s\" --> armor (ID: %d, type: %s)\n"),
                   RPG_Character_EquipmentSlotHelper::RPG_Character_EquipmentSlotToString((*iterator).first).c_str(),
                   (*iterator).second,
                   RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor_base->getArmorType()).c_str()));

        break;
      }
      default:
      {
        // debug info
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
