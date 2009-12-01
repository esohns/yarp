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
#include "rpg_item_weapon_base.h"

#include "rpg_item_dictionary.h"
#include "rpg_item_weapontype.h"
#include "rpg_item_weaponcategory.h"
#include "rpg_item_weaponclass.h"
#include "rpg_item_common_tools.h"

#include <rpg_chance_dice_common_tools.h>

#include <ace/Log_Msg.h>

RPG_Item_Weapon_Base::RPG_Item_Weapon_Base(const RPG_Item_WeaponType& weaponType_in,
                                           const RPG_Item_ID_t& itemID_in)
 : inherited(ITEM_WEAPON,
             itemID_in),
   myWeaponType(weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Weapon_Base::RPG_Item_Weapon_Base"));

}

RPG_Item_Weapon_Base::~RPG_Item_Weapon_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Weapon_Base::~RPG_Item_Weapon_Base"));

}

const RPG_Item_WeaponType RPG_Item_Weapon_Base::getWeaponType() const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Weapon_Base::getWeaponType"));

  return myWeaponType;
}

void RPG_Item_Weapon_Base::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Weapon_Base::dump"));

  // retrieve properties
  RPG_Item_WeaponProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(myWeaponType);

  std::string weaponType = RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(myWeaponType);
  std::string weaponCategory = RPG_Item_WeaponCategoryHelper::RPG_Item_WeaponCategoryToString(properties.weaponCategory);
  std::string weaponClass = RPG_Item_WeaponClassHelper::RPG_Item_WeaponClassToString(properties.weaponClass);
  std::string baseDamage = RPG_Chance_Dice_Common_Tools::rollToString(properties.baseDamage);
  std::string typeOfDamage = RPG_Item_Common_Tools::weaponDamageToString(properties.typeOfDamage);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Item: Weapon\nType: %s\nCategory: %s\nClass: %s\nPrice: %d GP, %d SP\nDamage: %s\ncritical Mod: %d, x%d\nRange: %d\nWeight: %d\nDamage Type: %s\n"),
             weaponType.c_str(),
             weaponCategory.c_str(),
             weaponClass.c_str(),
             properties.baseStorePrice.numGoldPieces,
             properties.baseStorePrice.numSilverPieces,
             baseDamage.c_str(),
             properties.criticalHitModifier.minToHitRoll,
             properties.criticalHitModifier.damageModifier,
             properties.rangeIncrement,
             properties.baseWeight,
             typeOfDamage.c_str()));
}
