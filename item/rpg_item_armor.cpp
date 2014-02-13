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

#include "rpg_item_armor.h"

#include "rpg_item_dictionary.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Item_Armor::RPG_Item_Armor(const RPG_Item_ArmorType& armorType_in)
 : inherited(), // *NOTE*: this generates a new item ID
   inherited2(ITEM_ARMOR,
              getID()), // <-- retrieve generated item ID
   myArmorType(armorType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Armor::RPG_Item_Armor"));

}

RPG_Item_Armor::~RPG_Item_Armor()
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Armor::~RPG_Item_Armor"));

}

const RPG_Item_ArmorType
RPG_Item_Armor::getArmorType() const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Armor::getArmorType"));

  return myArmorType;
}

void
RPG_Item_Armor::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Armor::dump"));

  // retrieve properties
  RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(myArmorType);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Item: Armor\nType: %s\nCategory: %s\nPrice: %d GP, %d SP\nAC Bonus: %d\nmax Dex Bonus: %d\nPenalty: %d\nSpell Failure: %d%%\nSpeed: %d\nWeight: %d\n"),
             RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(myArmorType).c_str(),
             RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString(properties.category).c_str(),
             properties.baseStorePrice.numGoldPieces,
             properties.baseStorePrice.numSilverPieces,
             properties.baseBonus,
             properties.maxDexterityBonus,
             properties.checkPenalty,
             properties.arcaneSpellFailure,
             properties.baseSpeed,
             properties.baseWeight));
}
