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

#include "rpg_item_weapon.h"

#include "ace/Log_Msg.h"

#include "rpg_dice_common_tools.h"

#include "rpg_common_macros.h"

#include "rpg_item_common_tools.h"
#include "rpg_item_dictionary.h"

RPG_Item_Weapon::RPG_Item_Weapon (enum RPG_Item_WeaponType type_in)
 : inherited () // *NOTE*: this generates a new item ID
 , inherited2 (ITEM_WEAPON,
               id ()) // <-- retrieve generated item ID
 , weaponType_ (type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Weapon::RPG_Item_Weapon"));

}

void
RPG_Item_Weapon::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Weapon::dump"));

  // retrieve properties
  RPG_Item_WeaponProperties properties =
    RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (weaponType_);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Item: Weapon\nType: %s\nCategory: %s\nClass: %s\nPrice: %d GP, %d SP\nDamage: %s\ncritical: %d, x%d\nRange: %d\nWeight: %d\nDamage Type: %s\n"),
             ACE_TEXT (RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weaponType_).c_str()),
             ACE_TEXT (RPG_Item_WeaponCategoryHelper::RPG_Item_WeaponCategoryToString(properties.category).c_str()),
             ACE_TEXT (RPG_Item_WeaponClassHelper::RPG_Item_WeaponClassToString(properties.weaponClass).c_str()),
             properties.baseStorePrice.numGoldPieces,
             properties.baseStorePrice.numSilverPieces,
             ACE_TEXT (RPG_Dice_Common_Tools::toString(properties.baseDamage).c_str()),
             properties.criticalHit.minToHitRoll,
             properties.criticalHit.damageModifier,
             properties.rangeIncrement,
             properties.baseWeight,
             ACE_TEXT (RPG_Item_Common_Tools::toString(properties.typeOfDamage).c_str())));
}
