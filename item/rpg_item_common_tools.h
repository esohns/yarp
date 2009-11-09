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
#ifndef RPG_ITEM_COMMON_TOOLS_H
#define RPG_ITEM_COMMON_TOOLS_H

#include "rpg_item_common.h"

#include <ace/Global_Macros.h>

#include <string>
#include <map>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Common_Tools
{
 public:
  // some useful types
  typedef std::map<std::string, RPG_Item_Type> RPG_String2ItemTypeTable_t;
  typedef RPG_String2ItemTypeTable_t::const_iterator RPG_String2ItemTypeTableIterator_t;
  typedef std::map<std::string, RPG_Item_MoneyType> RPG_String2MoneyTypeTable_t;
  typedef RPG_String2MoneyTypeTable_t::const_iterator RPG_String2MoneyTypeTableIterator_t;
  typedef std::map<std::string, RPG_Item_WeaponCategory> RPG_String2WeaponCategoryTable_t;
  typedef RPG_String2WeaponCategoryTable_t::const_iterator RPG_String2WeaponCategoryTableIterator_t;
  typedef std::map<std::string, RPG_Item_WeaponClass> RPG_String2WeaponClassTable_t;
  typedef RPG_String2WeaponClassTable_t::const_iterator RPG_String2WeaponClassTableIterator_t;
  typedef std::map<std::string, RPG_Item_WeaponType> RPG_String2WeaponTypeTable_t;
  typedef RPG_String2WeaponTypeTable_t::const_iterator RPG_String2WeaponTypeTableIterator_t;
  typedef std::map<std::string, RPG_Item_WeaponDamageType> RPG_String2WeaponDamageTypeTable_t;
  typedef RPG_String2WeaponDamageTypeTable_t::const_iterator RPG_String2WeaponDamageTypeTableIterator_t;
  typedef std::map<std::string, RPG_Item_ArmorCategory> RPG_String2ArmorCategoryTable_t;
  typedef RPG_String2ArmorCategoryTable_t::const_iterator RPG_String2ArmorCategoryTableIterator_t;
  typedef std::map<std::string, RPG_Item_ArmorType> RPG_String2ArmorTypeTable_t;
  typedef RPG_String2ArmorTypeTable_t::const_iterator RPG_String2ArmorTypeTableIterator_t;

  static void initStringConversionTables();
  static const std::string             itemTypeToString(const RPG_Item_Type&); // item type
  static const RPG_Item_WeaponCategory stringToWeaponCategory(const std::string&); // string
  static const std::string             weaponCategoryToString(const RPG_Item_WeaponCategory&); // weapon category
  static const RPG_Item_WeaponClass stringToWeaponClass(const std::string&); // string
  static const std::string          weaponClassToString(const RPG_Item_WeaponClass&); // weapon class
  static const RPG_Item_WeaponType stringToWeaponType(const std::string&); // string
  static const std::string         weaponTypeToString(const RPG_Item_WeaponType&); // weapon type
  static const RPG_Item_WeaponDamageType stringToWeaponDamageType(const std::string&); // string
  static const std::string               weaponDamageToString(const RPG_Item_WeaponDamage&); // weapon damage
  static const std::string               damageToString(const RPG_Item_Damage&); // damage
  static const RPG_Item_ArmorCategory stringToArmorCategory(const std::string&); // string
  static const std::string            armorCategoryToString(const RPG_Item_ArmorCategory&); // armor category
  static const RPG_Item_ArmorType stringToArmorType(const std::string&); // string
  static const std::string        armorTypeToString(const RPG_Item_ArmorType&); // armor type

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools(const RPG_Item_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools& operator=(const RPG_Item_Common_Tools&));

  static RPG_String2ItemTypeTable_t         myString2ItemTypeTable;
  static RPG_String2MoneyTypeTable_t        myString2MoneyTypeTable;
  static RPG_String2WeaponCategoryTable_t   myString2WeaponCategoryTable;
  static RPG_String2WeaponClassTable_t      myString2WeaponClassTable;
  static RPG_String2WeaponTypeTable_t       myString2WeaponTypeTable;
  static RPG_String2WeaponDamageTypeTable_t myString2WeaponDamageTypeTable;
  static RPG_String2ArmorCategoryTable_t    myString2ArmorCategoryTable;
  static RPG_String2ArmorTypeTable_t        myString2ArmorTypeTable;
};

#endif
