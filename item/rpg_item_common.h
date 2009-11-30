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
#ifndef RPG_ITEM_COMMON_H
#define RPG_ITEM_COMMON_H

#include "rpg_chance_dice_common.h"

#include <bitset>
#include <map>

enum RPG_Item_Type
{
  ITEM_ARMOR = 0,
  ITEM_GOODS,
  ITEM_OTHER,
  ITEM_VALUABLE,
  ITEM_WEAPON,
  //
  ITEM_TYPE_MAX,
  ITEM_TYPE_INVALID
};

// enum RPG_Item_Magic_Item
// {
//   MAGIC_IS_CURSED,
//   MAGIC_IS_IMBUED
// };

enum RPG_Item_MoneyType
{
  MONEY_COIN_COPPER = 0,
  MONEY_COIN_SILVER,
  MONEY_COIN_GOLD,
  MONEY_COIN_PLATINUM,
  MONEY_GEM,
  MONEY_PRECIOUS,
  //
  MONEY_TYPE_MAX,
  MONEY_TYPE_INVALID
};

typedef std::bitset<3> RPG_Item_WeaponDamage;

typedef RPG_Chance_DiceRoll RPG_Item_Damage;

struct RPG_Item_WeaponProperties
{
//   RPG_Item_WeaponType          weaponType;
  RPG_Item_WeaponCategory      weaponCategory;
  RPG_Item_WeaponClass         weaponClass;
  RPG_Item_StorePrice          baseStorePrice;
  RPG_Item_Damage              baseDamage;
  RPG_Item_CriticalHitModifier criticalHitModifier;
  unsigned int                 rangeIncrement; // feet
  unsigned int                 baseWeight; // pounds
  RPG_Item_WeaponDamage        typeOfDamage; // bitfield
};

struct RPG_Item_MagicWeaponProperties
{
  int toHitModifier;
};

struct RPG_Item_ArmorProperties
{
//  RPG_Item_ArmorType     armorType;
  RPG_Item_ArmorCategory armorCategory;
  RPG_Item_StorePrice    baseStorePrice;
  unsigned int           baseArmorBonus;
  unsigned int           maxDexterityBonus;
  int                    armorCheckPenalty;
  unsigned int           arcaneSpellFailure; // percentage
  unsigned int           baseSpeed; // feet/round
  unsigned int           baseWeight; // pounds
};

struct RPG_Item_MagicArmorProperties
{
  int defenseModifier;
};

// useful types
// typedef std::pair<RPG_Item_WeaponType,
//                   RPG_Item_WeaponProperties> RPG_ITEM_WEAPONDICTIONARY_ITEM_T;
typedef std::map<RPG_Item_WeaponType,
                 RPG_Item_WeaponProperties> RPG_Item_WeaponDictionary_t;
typedef RPG_Item_WeaponDictionary_t::const_iterator RPG_Item_WeaponDictionaryIterator_t;
// typedef std::pair<RPG_Item_ArmorType,
//                   RPG_Item_ArmorProperties> RPG_ITEM_ARMORDICTIONARY_ITEM_T;
typedef std::map<RPG_Item_ArmorType,
                 RPG_Item_ArmorProperties> RPG_Item_ArmorDictionary_t;
typedef RPG_Item_ArmorDictionary_t::const_iterator RPG_Item_ArmorDictionaryIterator_t;

#endif
