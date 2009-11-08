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
#include "rpg_item_armor_base.h"

#include "rpg_item_common_tools.h"
#include "rpg_item_dictionary.h"

#include <ace/Log_Msg.h>

RPG_Item_Armor_Base::RPG_Item_Armor_Base(const RPG_Item_ArmorType& armorType_in,
                                         const RPG_Item_ID_t& itemID_in)
 : inherited(ITEM_ARMOR,
             itemID_in),
   myArmorType(armorType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Armor_Base::RPG_Item_Armor_Base"));

}

RPG_Item_Armor_Base::~RPG_Item_Armor_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Armor_Base::~RPG_Item_Armor_Base"));

}

const RPG_Item_ArmorType RPG_Item_Armor_Base::getArmorType() const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Armor_Base::getArmorType"));

  return myArmorType;
}

void RPG_Item_Armor_Base::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Armor_Base::dump"));

  // retrieve properties
  RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(myArmorType);

  std::string armorType = RPG_Item_Common_Tools::armorTypeToString(myArmorType);
  std::string armorCategory = RPG_Item_Common_Tools::armorCategoryToString(properties.armorCategory);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Item: Armor\nType: %s\nCategory: %s\nPrice: %d GP, %d SP\nAC Bonus: %d\nmax Dex Bonus: %d\nPenalty: %d\nSpell Failure: %d%%\nSpeed: %d\nWeight: %d\n"),
             armorType.c_str(),
             armorCategory.c_str(),
             properties.baseStorePrice.numGoldPieces,
             properties.baseStorePrice.numSilverPieces,
             properties.baseArmorBonus,
             properties.maxDexterityBonus,
             properties.armorCheckPenalty,
             properties.arcaneSpellFailure,
             properties.baseSpeed,
             properties.baseWeight));
}
