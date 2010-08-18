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
#include "rpg_character_player.h"

#include <rpg_item_instance_common.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_base.h>
#include <rpg_item_armor_base.h>
#include <rpg_item_weapon_base.h>
#include <rpg_item_common_tools.h>

#include <ace/Log_Msg.h>

RPG_Character_Player::RPG_Character_Player(const std::string& name_in,
                                           const RPG_Character_Gender& gender_in,
                                           const RPG_Character_Race& race_in,
                                           const RPG_Character_Class& class_in,
                                           const RPG_Character_Alignment& alignment_in,
                                           const RPG_Character_Attributes& attributes_in,
                                           const RPG_Character_Skills_t& skills_in,
                                           const RPG_Character_Feats_t& feats_in,
                                           const RPG_Character_Abilities_t& abilities_in,
                                           const RPG_Character_OffHand& offhand_in,
                                           const unsigned int& experience_in,
                                           const unsigned short int& hitpoints_in,
                                           const unsigned int& wealth_in,
                                           const RPG_Magic_Spells_t& knownSpells_in,
                                           const RPG_Magic_SpellList_t& spells_in,
                                           const RPG_Item_List_t& inventory_in)
 : inherited(name_in,
             gender_in,
             race_in,
             class_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
             offhand_in,
             experience_in,
             hitpoints_in,
             wealth_in,
             knownSpells_in,
             spells_in,
             inventory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::RPG_Character_Player"));

  // wear default items...
  defaultEquip();
//   myEquipment.dump();
}

RPG_Character_Player::RPG_Character_Player(const RPG_Character_Player& player_in)
 : inherited(player_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::RPG_Character_Player"));

}

RPG_Character_Player::~RPG_Character_Player()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::~RPG_Character_Player"));

}

RPG_Character_Player&
RPG_Character_Player::operator=(const RPG_Character_Player& player_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::operator="));

  inherited::operator=(player_in);

  return *this;
}

void
RPG_Character_Player::defaultEquip()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::operator="));

  RPG_Item_Base* handle = NULL;
  for (RPG_Item_ListIterator_t iterator = myInventory.myItems.begin();
       iterator != myInventory.myItems.end();
       iterator++)
  {
    handle = NULL;
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem(*iterator, handle))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item ID: %d, aborting\n"),
                 *iterator));

      return;
    } // end IF
    ACE_ASSERT(handle);

    switch (handle->getType())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor_Base* armor_base = ACE_dynamic_cast(RPG_Item_Armor_Base*, handle);
        ACE_ASSERT(armor_base);
//         RPG_Item_ArmorProperties armor_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor_base->getArmorType());
        // shield or (body) armor ?
        // *TODO*: what about other types of armor ?
        RPG_Character_EquipmentSlot slot = EQUIPMENTSLOT_BODY;
        if (RPG_Item_Common_Tools::isShield(armor_base->getArmorType()))
        {
          slot = ((getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                 : EQUIPMENTSLOT_HAND_RIGHT);
        } // end IF
        myEquipment.equip(*iterator, slot);

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon_Base* weapon_base = ACE_dynamic_cast(RPG_Item_Weapon_Base*, handle);
        ACE_ASSERT(weapon_base);
//         RPG_Item_WeaponProperties weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_base->getWeaponType());
        // - by default, equip melee weapons only
        // *TODO*: what about other types of weapons ?
        if (!RPG_Item_Common_Tools::isMeleeWeapon(weapon_base->getWeaponType()))
          break;

        RPG_Character_EquipmentSlot slot = ((getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_RIGHT
                                                                           : EQUIPMENTSLOT_HAND_LEFT);
        myEquipment.equip(*iterator, slot);
        if (RPG_Item_Common_Tools::isTwoHandedWeapon(weapon_base->getWeaponType()))
        {
          slot = ((getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                 : EQUIPMENTSLOT_HAND_RIGHT);
          myEquipment.equip(*iterator, slot);
        } // end IF

        break;
      }
      default:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("item ID %d: invalid type: \"%s\", continuing\n"),
                   *iterator,
                   RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str()));

        break;
      }
    } // end SWITCH
  } // end FOR
}
