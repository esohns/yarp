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

#include <vector>

enum RPG_Item_Type
{
  ITEM_ARMOR,
  ITEM_GOODS,
  ITEM_OTHER,
  ITEM_VALUABLE,
  ITEM_WEAPON
};

// enum RPG_Item_Magic_Item
// {
//   MAGIC_IS_CURSED,
//   MAGIC_IS_IMBUED
// };

enum RPG_Item_Wealth_Type
{
  MONEY_COIN_COPPER,
  MONEY_COIN_SILVER,
  MONEY_COIN_GOLD,
  MONEY_COIN_PLATINUM,
  MONEY_GEM,
  MONEY_PRECIOUS
};

struct RPG_Item_Store_Price
{
  unsigned int numGoldPieces;
  unsigned int numSilverPieces;
};

enum RPG_Item_Weapon_Category
{
  WEAPON_CATEGORY_SIMPLE,
  WEAPON_CATEGORY_MARTIAL,
  WEAPON_CATEGORY_EXOTIC
};

enum RPG_Item_Weapon_Class
{
  WEAPON_CLASS_UNARMED,
  WEAPON_CLASS_LIGHT_MELEE,
  WEAPON_CLASS_ONE_HANDED_MELEE,
  WEAPON_CLASS_TWO_HANDED_MELEE,
  WEAPON_CLASS_RANGED
};

enum RPG_Item_Weapon_Type
{
  // simple weapons
  UNARMED_WEAPON_GAUNTLET = 0,
  UNARMED_WEAPON_STRIKE,
  LIGHT_MELEE_WEAPON_DAGGER,
  LIGHT_MELEE_WEAPON_SPIKED_GAUNTLET,
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
  LIGHT_MELEE_WEAPON_SPIKED_ARMOR,
  LIGHT_MELEE_WEAPON_SPIKED_SHIELD_LIGHT,
  LIGHT_MELEE_WEAPON_SWORD_SHORT,
  ONE_HANDED_MELEE_WEAPON_AXE_BATTLE,
  ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT,
  ONE_HANDED_MELEE_WEAPON_SWORD_LONG,
  ONE_HANDED_MELEE_WEAPON_PICK_HEAVY,
  ONE_HANDED_MELEE_WEAPON_RAPIER,
  ONE_HANDED_MELEE_WEAPON_SCIMITAR,
  ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY,
  ONE_HANDED_MELEE_WEAPON_SPIKED_SHIELD_HEAVY,
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
  RANGED_WEAPON_SHURIKEN
};

enum RPG_Item_Weapon_Damage_Type
{
  WEAPON_DAMAGE_TYPE_BLUDGEONING = 1,
  WEAPON_DAMAGE_TYPE_PIERCING = 2,
  WEAPON_DAMAGE_TYPE_SLASHING = 4,
};

enum RPG_Item_CriticalHit_Modifier
{
  CRITICALHIT_MODIFIER_X2 = 0,
  CRITICALHIT_MODIFIER_X3,
  CRITICALHIT_MODIFIER_X3_X4, // double edged
  CRITICALHIT_MODIFIER_X4,
  CRITICALHIT_MODIFIER_19_20_X2, // threat range: 19-20, critical x2
  CRITICALHIT_MODIFIER_18_20_X2 // threat range: 18-20, critical x2
};

typedef RPG_Chance_Roll RPG_Item_Damage;

struct RPG_Item_Weapon_Properties
{
  RPG_Item_Weapon_Category      weaponCategory;
  RPG_Item_Weapon_Class         weaponClass;
  RPG_Item_Weapon_Type          weaponType;
  RPG_Item_Store_Price          baseStorePrice;
  RPG_Item_Damage               baseDamage;
  RPG_Item_CriticalHit_Modifier criticalModifier;
  unsigned int                  rangeIncrement; // feet
  unsigned int                  baseWeight; // pounds
  RPG_Item_Weapon_Damage_Type   typeOfDamage;
};

struct RPG_Item_Magic_Weapon_Properties
{
  int toHitModifier;
};

enum RPG_Item_Armor_Category
{
  ARMOR_CATEGORY_LIGHT,
  ARMOR_CATEGORY_MEDIUM,
  ARMOR_CATEGORY_HEAVY,
  ARMOR_CATEGORY_SHIELD
};

enum RPG_Item_Armor_Type
{
  // light armor
  ARMOR_PADDED,
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
  ARMOR_SHIELD_TOWER
};

struct RPG_Item_Armor_Properties
{
  RPG_Item_Armor_Category armorCategory;
  RPG_Item_Armor_Type     armorType;
  RPG_Item_Store_Price    baseStorePrice;
  unsigned int            baseArmorBonus;
  unsigned int            maxDexterityBonus;
  int                     armorCheckPenalty;
  unsigned int            arcaneSpellFailure; // percentage
  unsigned int            baseSpeed; // feet/round
  unsigned int            baseWeight; // pounds
};

struct RPG_Item_Magic_Armor_Properties
{
  int defenseModifier;
};

// useful types
typedef std::vector<unsigned int> RPG_ITEM_LIST_T;
typedef RPG_ITEM_LIST_T::const_iterator RPG_ITEM_LIST_ITERATOR_T;

#endif
