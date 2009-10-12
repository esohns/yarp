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
#ifndef RPG_ITEM_COMMON_TOOLS_H
#define RPG_ITEM_COMMON_TOOLS_H

#include "rpg_item_common.h"

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Common_Tools
{
 public:
  // some handy types
  typedef std::map<std::string, RPG_Item_Type> RPG_String2ItemType_Table;
  typedef RPG_String2ItemType_Table::const_iterator RPG_String2ItemType_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Money_Type> RPG_String2MoneyType_Table;
  typedef RPG_String2MoneyType_Table::const_iterator RPG_String2MoneyType_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Weapon_Category> RPG_String2WeaponCategory_Table;
  typedef RPG_String2WeaponCategory_Table::const_iterator RPG_String2WeaponCategory_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Weapon_Class> RPG_String2WeaponClass_Table;
  typedef RPG_String2WeaponClass_Table::const_iterator RPG_String2WeaponClass_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Weapon_Type> RPG_String2WeaponType_Table;
  typedef RPG_String2WeaponType_Table::const_iterator RPG_String2WeaponType_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Weapon_Damage_Type> RPG_String2WeaponDamageType_Table;
  typedef RPG_String2WeaponDamageType_Table::const_iterator RPG_String2WeaponDamageType_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Armor_Category> RPG_String2ArmorCategory_Table;
  typedef RPG_String2ArmorCategory_Table::const_iterator RPG_String2ArmorCategory_Table_Iterator;
  typedef std::map<std::string, RPG_Item_Armor_Type> RPG_String2ArmorType_Table;
  typedef RPG_String2ArmorType_Table::const_iterator RPG_String2ArmorType_Table_Iterator;

  static RPG_String2ItemType_Table         myString2ItemTypeTable;
  static RPG_String2MoneyType_Table        myString2MoneyTypeTable;
  static RPG_String2WeaponCategory_Table   myString2WeaponCategoryTable;
  static RPG_String2WeaponClass_Table      myString2WeaponClassTable;
  static RPG_String2WeaponType_Table       myString2WeaponTypeTable;
  static RPG_String2WeaponDamageType_Table myString2WeaponDamageTypeTable;
  static RPG_String2ArmorCategory_Table    myString2ArmorCategoryTable;
  static RPG_String2ArmorType_Table        myString2ArmorTypeTable;

  static void initStringConversionTables();

  static void stringToWeaponCategory(const std::string&,         // string
                                     RPG_Item_Weapon_Category&); // return value: weapon category
  static void stringToWeaponClass(const std::string&,      // string
                                  RPG_Item_Weapon_Class&); // return value: weapon class
  static void stringToWeaponType(const std::string&,     // string
                                 RPG_Item_Weapon_Type&); // return value: weapon type
  static void stringToWeaponDamageType(const std::string&,            // string
                                       RPG_Item_Weapon_Damage_Type&); // return value: damage type
  static void stringToArmorCategory(const std::string&,        // string
                                    RPG_Item_Armor_Category&); // return value: armor category
  static void stringToArmorType(const std::string&,    // string
                                RPG_Item_Armor_Type&); // return value: armor type

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools(const RPG_Item_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools& operator=(const RPG_Item_Common_Tools&));
};

#endif
