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
  static void initStringConversionTables();

  static const std::string             itemTypeToString(const RPG_Item_Type&); // item type
//   static const RPG_Item_WeaponCategory stringToWeaponCategory(const std::string&); // string
//   static const std::string             weaponCategoryToString(const RPG_Item_WeaponCategory&); // weapon category
//   static const RPG_Item_WeaponClass stringToWeaponClass(const std::string&); // string
//   static const std::string          weaponClassToString(const RPG_Item_WeaponClass&); // weapon class
//   static const RPG_Item_WeaponType stringToWeaponType(const std::string&); // string
//   static const std::string         weaponTypeToString(const RPG_Item_WeaponType&); // weapon type
//   static const RPG_Item_WeaponDamageType stringToWeaponDamageType(const std::string&); // string
//   static const std::string               weaponDamageTypeToString(const RPG_Item_WeaponDamageType&); // weapon damage type
  static const std::string               weaponDamageToString(const RPG_Item_WeaponDamage&); // weapon damage
  static const std::string               damageToString(const RPG_Item_Damage&); // damage
//   static const RPG_Item_ArmorCategory stringToArmorCategory(const std::string&); // string
//   static const std::string            armorCategoryToString(const RPG_Item_ArmorCategory&); // armor category
//   static const RPG_Item_ArmorType stringToArmorType(const std::string&); // string
//   static const std::string        armorTypeToString(const RPG_Item_ArmorType&); // armor type

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Item_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools(const RPG_Item_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_Tools& operator=(const RPG_Item_Common_Tools&));

  // some useful types
  typedef std::map<std::string, RPG_Item_Type> RPG_StringToItemTypeTable_t;
  typedef RPG_StringToItemTypeTable_t::const_iterator RPG_StringToItemTypeTableIterator_t;
  typedef std::map<std::string, RPG_Item_MoneyType> RPG_StringToMoneyTypeTable_t;
  typedef RPG_StringToMoneyTypeTable_t::const_iterator RPG_StringToMoneyTypeTableIterator_t;
//   typedef std::map<std::string, RPG_Item_WeaponCategory> RPG_StringToWeaponCategoryTable_t;
//   typedef RPG_StringToWeaponCategoryTable_t::const_iterator RPG_StringToWeaponCategoryTableIterator_t;
//   typedef std::map<std::string, RPG_Item_WeaponClass> RPG_StringToWeaponClassTable_t;
//   typedef RPG_StringToWeaponClassTable_t::const_iterator RPG_StringToWeaponClassTableIterator_t;
//   typedef std::map<std::string, RPG_Item_WeaponType> RPG_StringToWeaponTypeTable_t;
//   typedef RPG_StringToWeaponTypeTable_t::const_iterator RPG_StringToWeaponTypeTableIterator_t;
//   typedef std::map<std::string, RPG_Item_WeaponDamageType> RPG_StringToWeaponDamageTypeTable_t;
//   typedef RPG_StringToWeaponDamageTypeTable_t::const_iterator RPG_StringToWeaponDamageTypeTableIterator_t;
//   typedef std::map<std::string, RPG_Item_ArmorCategory> RPG_StringToArmorCategoryTable_t;
//   typedef RPG_StringToArmorCategoryTable_t::const_iterator RPG_StringToArmorCategoryTableIterator_t;
//   typedef std::map<std::string, RPG_Item_ArmorType> RPG_StringToArmorTypeTable_t;
//   typedef RPG_StringToArmorTypeTable_t::const_iterator RPG_StringToArmorTypeTableIterator_t;

  static RPG_StringToItemTypeTable_t         myStringToItemTypeTable;
  static RPG_StringToMoneyTypeTable_t        myStringToMoneyTypeTable;
//   static RPG_StringToWeaponCategoryTable_t   myStringToWeaponCategoryTable;
//   static RPG_StringToWeaponClassTable_t      myStringToWeaponClassTable;
//   static RPG_StringToWeaponTypeTable_t       myStringToWeaponTypeTable;
//   static RPG_StringToWeaponDamageTypeTable_t myStringToWeaponDamageTypeTable;
//   static RPG_StringToArmorCategoryTable_t    myStringToArmorCategoryTable;
//   static RPG_StringToArmorTypeTable_t        myStringToArmorTypeTable;
};

#endif
