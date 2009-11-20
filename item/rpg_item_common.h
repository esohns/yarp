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

struct RPG_Item_StorePrice
{
  unsigned int numGoldPieces;
  unsigned int numSilverPieces;
};

enum RPG_Item_WeaponCategory
{
  WEAPONCATEGORY_SIMPLE = 0,
  WEAPONCATEGORY_MARTIAL,
  WEAPONCATEGORY_EXOTIC,
  //
  WEAPONCATEGORY_MAX,
  WEAPONCATEGORY_INVALID
};

enum RPG_Item_WeaponClass
{
  WEAPONCLASS_UNARMED = 0,
  WEAPONCLASS_LIGHT_MELEE,
  WEAPONCLASS_ONE_HANDED_MELEE,
  WEAPONCLASS_TWO_HANDED_MELEE,
  WEAPONCLASS_RANGED,
  //
  WEAPONCLASS_MAX,
  WEAPONCLASS_INVALID
};

enum RPG_Item_WeaponType
{
  // simple weapons
  UNARMED_WEAPON_GAUNTLET = 0,
  UNARMED_WEAPON_STRIKE,
  LIGHT_MELEE_WEAPON_DAGGER,
  LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED,
  LIGHT_MELEE_WEAPON_MACE_LIGHT,
  LIGHT_MELEE_WEAPON_SICKLE,
  ONE_HANDED_MELEE_WEAPON_CLUB,
  ONE_HANDED_MELEE_WEAPON_MACE_HEAVY,
  ONE_HANDED_MELEE_WEAPON_MORNINGSTAR,
  ONE_HANDED_MELEE_WEAPON_SHORTSPEAR,
  TWO_HANDED_MELEE_WEAPON_LONGSPEAR,
  TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF,
  TWO_HANDED_MELEE_WEAPON_SPEAR,
  RANGED_WEAPON_CROSSBOW_LIGHT,
  RANGED_WEAPON_CROSSBOW_HEAVY,
  RANGED_WEAPON_DART,
  RANGED_WEAPON_JAVELIN,
  RANGED_WEAPON_SLING,
  // martial weapons
  LIGHT_MELEE_WEAPON_AXE_THROWING,
  LIGHT_MELEE_WEAPON_HAMMER_LIGHT,
  LIGHT_MELEE_WEAPON_AXE_HAND,
  LIGHT_MELEE_WEAPON_KUKRI,
  LIGHT_MELEE_WEAPON_PICK_LIGHT,
  LIGHT_MELEE_WEAPON_SAP,
  LIGHT_MELEE_WEAPON_SHIELD_LIGHT,
  LIGHT_MELEE_WEAPON_ARMOR_SPIKED,
  LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED,
  LIGHT_MELEE_WEAPON_SWORD_SHORT,
  ONE_HANDED_MELEE_WEAPON_AXE_BATTLE,
  ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT,
  ONE_HANDED_MELEE_WEAPON_SWORD_LONG,
  ONE_HANDED_MELEE_WEAPON_PICK_HEAVY,
  ONE_HANDED_MELEE_WEAPON_RAPIER,
  ONE_HANDED_MELEE_WEAPON_SCIMITAR,
  ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY,
  ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED,
  ONE_HANDED_MELEE_WEAPON_TRIDENT,
  ONE_HANDED_MELEE_WEAPON_HAMMER_WAR,
  TWO_HANDED_MELEE_WEAPON_FALCHION,
  TWO_HANDED_MELEE_WEAPON_GLAIVE,
  TWO_HANDED_MELEE_WEAPON_AXE_GREAT,
  TWO_HANDED_MELEE_WEAPON_CLUB_GREAT,
  TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY,
  TWO_HANDED_MELEE_WEAPON_SWORD_GREAT,
  TWO_HANDED_MELEE_WEAPON_GUISARME,
  TWO_HANDED_MELEE_WEAPON_HALBERD,
  TWO_HANDED_MELEE_WEAPON_LANCE,
  TWO_HANDED_MELEE_WEAPON_RANSEUR,
  TWO_HANDED_MELEE_WEAPON_SCYTHE,
  RANGED_WEAPON_BOW_SHORT,
  RANGED_WEAPON_BOW_SHORT_COMPOSITE,
  RANGED_WEAPON_BOW_LONG,
  RANGED_WEAPON_BOW_LONG_COMPOSITE,
  // exotic weapons
  LIGHT_MELEE_WEAPON_KAMA,
  LIGHT_MELEE_WEAPON_NUNCHAKU,
  LIGHT_MELEE_WEAPON_SAI,
  LIGHT_MELEE_WEAPON_SIANGHAM,
  ONE_HANDED_MELEE_WEAPON_SWORD_BASTARD,
  ONE_HANDED_MELEE_WEAPON_AXE_WAR_DWARVEN,
  ONE_HANDED_MELEE_WEAPON_WHIP,
  TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE,
  TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED,
  TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE,
  TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED,
  TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED,
  TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN,
  RANGED_WEAPON_BOLAS,
  RANGED_WEAPON_CROSSBOW_HAND,
  RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT,
  RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY,
  RANGED_WEAPON_NET,
  RANGED_WEAPON_SHURIKEN,
  //
  WEAPON_TYPE_MAX,
  WEAPON_TYPE_INVALID
};

enum RPG_Item_WeaponDamageType
{
  WEAPONDAMAGE_NONE = 0,
  //
  WEAPONDAMAGE_BLUDGEONING = 1,
  WEAPONDAMAGE_PIERCING = 2,
  WEAPONDAMAGE_SLASHING = 4,
  //
  WEAPONDAMAGE_MAX,
  WEAPONDAMAGE_INVALID
};

typedef std::bitset<3> RPG_Item_WeaponDamage;

struct RPG_Item_CriticalHitModifier
{
  unsigned int minToHitRoll; // usually a "20" on a D_20
  unsigned int damageModifier;
};

typedef RPG_Chance_DiceRoll RPG_Item_Damage;

struct RPG_Item_WeaponProperties
{
//   RPG_Item_WeaponType         weaponType;
  RPG_Item_WeaponCategory      weaponCategory;
  RPG_Item_WeaponClass         weaponClass;
  RPG_Item_StorePrice          baseStorePrice;
  RPG_Item_Damage              baseDamage;
  RPG_Item_CriticalHitModifier criticalHitModifier;
  unsigned int                 rangeIncrement; // feet
  unsigned int                 baseWeight; // pounds
  RPG_Item_WeaponDamage        typeOfDamage; // bitfield
};

// need this for parsing purposes...
struct RPG_Item_WeaponProperties_XML
{
  RPG_Item_WeaponType          weaponType;
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

enum RPG_Item_ArmorCategory
{
  ARMORCATEGORY_LIGHT = 0,
  ARMORCATEGORY_MEDIUM,
  ARMORCATEGORY_HEAVY,
  ARMORCATEGORY_SHIELD,
  //
  ARMORCATEGORY_MAX,
  ARMORCATEGORY_INVALID
};

enum RPG_Item_ArmorType
{
  // light armor
  ARMOR_PADDED = 0,
  ARMOR_LEATHER,
  ARMOR_LEATHER_STUDDED,
  ARMOR_CHAIN_SHIRT,
  // medium armor
  ARMOR_HIDE,
  ARMOR_MAIL_SCALE,
  ARMOR_MAIL_CHAIN,
  ARMOR_PLATE_BREAST,
  // heavy armor
  ARMOR_MAIL_SPLINT,
  ARMOR_MAIL_BANDED,
  ARMOR_PLATE_HALF,
  ARMOR_PLATE_FULL,
  // shields
  ARMOR_BUCKLER,
  ARMOR_SHIELD_LIGHT_WOODEN,
  ARMOR_SHIELD_LIGHT_STEEL,
  ARMOR_SHIELD_HEAVY_WOODEN,
  ARMOR_SHIELD_HEAVY_STEEL,
  ARMOR_SHIELD_TOWER,
  //
  ARMOR_TYPE_MAX,
  ARMOR_TYPE_INVALID
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

// need this for parsing purposes...
struct RPG_Item_ArmorProperties_XML
{
  RPG_Item_ArmorType     armorType;
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
