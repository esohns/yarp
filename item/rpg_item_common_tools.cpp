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
#include "rpg_item_common_tools.h"

// init statics
RPG_Item_Common_Tools::RPG_String2ItemType_Table            RPG_Item_Common_Tools::myString2ItemTypeTable;
RPG_Item_Common_Tools::RPG_String2WealthType_Table          RPG_Item_Common_Tools::myString2WealthTypeTable;
RPG_Item_Common_Tools::RPG_String2WeaponCategory_Table      RPG_Item_Common_Tools::myString2WeaponCategoryTable;
RPG_Item_Common_Tools::RPG_String2WeaponClass_Table         RPG_Item_Common_Tools::myString2WeaponClassTable;
RPG_Item_Common_Tools::RPG_String2WeaponType_Table          RPG_Item_Common_Tools::myString2WeaponTypeTable;
RPG_Item_Common_Tools::RPG_String2DamageType_Table          RPG_Item_Common_Tools::myString2DamageTypeTable;
RPG_Item_Common_Tools::RPG_String2CriticalHitModifier_Table RPG_Item_Common_Tools::myString2CriticalHitModifierTable;
RPG_Item_Common_Tools::RPG_String2ArmorCategory_Table       RPG_Item_Common_Tools::myString2ArmorCategoryTable;
RPG_Item_Common_Tools::RPG_String2ArmorType_Table           RPG_Item_Common_Tools::myString2ArmorTypeTable;

void RPG_Item_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::initStringConversionTables"));

  // RPG_Item_Type
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ITEM_ARMOR"), ITEM_ARMOR));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ITEM_GOODS"), ITEM_GOODS));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ITEM_OTHER"), ITEM_OTHER));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ITEM_VALUABLE"), ITEM_VALUABLE));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ITEM_WEAPON"), ITEM_WEAPON));

}

void RPG_Item_Common_Tools::stringToWeaponCategory(const std::string& string_in,
                                                   RPG_Item_Weapon_Category& category_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToWeaponCategory"));

}
