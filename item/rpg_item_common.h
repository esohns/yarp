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

#include "rpg_dice_incl.h"
#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"
#include "rpg_character_incl.h"
#include "rpg_magic_incl.h"

#include "rpg_item_incl.h"

#include <bitset>
#include <map>

// typedef std::bitset<(RPG_COMMON_PHYSICALDAMAGETYPE_MAX - 1)> RPG_Item_WeaponDamage;
typedef std::bitset<5> RPG_Item_WeaponDamageType;

typedef RPG_Dice_Roll RPG_Item_Damage;

// typedef RPG_Item_WeaponPropertiesXML RPG_Item_WeaponProperties;
struct RPG_Item_WeaponProperties
 : public RPG_Item_PropertiesBase
{
  signed char toHitModifier;
  // RPG_Item_WeaponPropertiesBase //
//   RPG_Item_WeaponType type;
  RPG_Item_WeaponCategory category;
  RPG_Item_WeaponClass weaponClass;
  RPG_Dice_Roll baseDamage;
  RPG_Item_CriticalHitProperties criticalHit;
  unsigned char rangeIncrement;
  RPG_Item_WeaponDamageType typeOfDamage;
  bool isNonLethal;
  bool isReachWeapon;
  bool isDoubleWeapon;
  //// RPG_Item_PropertiesBase //
  //RPG_Magic_School aura;
  //RPG_Item_MagicalPrerequisites prerequisites;
  //// RPG_Item_BaseProperties //
  //unsigned short int baseWeight;
  //RPG_Item_StorePrice baseStorePrice;
  //RPG_Item_CreationCost costToCreate;
};

// typedef RPG_Item_CommodityPropertiesBase RPG_Item_CommodityProperties;
struct RPG_Item_CommodityProperties
 : public RPG_Item_PropertiesBase
{
  // RPG_Item_CommodityPropertiesBase //
// RPG_Item_CommodityType type;
//  RPG_Item_CommodityUnion subtype;
  //// RPG_Item_PropertiesBase //
  //RPG_Magic_School aura;
  //RPG_Item_MagicalPrerequisites prerequisites;
  //// RPG_Item_BaseProperties //
  //unsigned short int baseWeight;
  //RPG_Item_StorePrice baseStorePrice;
  //RPG_Item_CreationCost costToCreate;
};

// typedef RPG_Item_ArmorPropertiesXML RPG_Item_ArmorProperties;
struct RPG_Item_ArmorProperties
 : public RPG_Item_PropertiesBase
{
  signed char defenseModifier;
  // RPG_Item_ArmorPropertiesBase //
//   RPG_Item_ArmorType type;
  RPG_Item_ArmorCategory category;
  unsigned char baseBonus;
  unsigned char maxDexterityBonus;
  signed char checkPenalty;
  unsigned char arcaneSpellFailure;
  unsigned char baseSpeed;
  //// RPG_Item_PropertiesBase //
  //RPG_Magic_School aura;
  //RPG_Item_MagicalPrerequisites prerequisites;
  //// RPG_Item_BaseProperties //
  //unsigned short int baseWeight;
  //RPG_Item_StorePrice baseStorePrice;
  //RPG_Item_CreationCost costToCreate;
};

// useful types
// typedef std::pair<RPG_Item_ArmorType,
//                   RPG_Item_ArmorProperties> RPG_ITEM_ARMORDICTIONARY_ITEM_T;
typedef std::map<RPG_Item_ArmorType,
                 RPG_Item_ArmorProperties> RPG_Item_ArmorDictionary_t;
typedef RPG_Item_ArmorDictionary_t::const_iterator RPG_Item_ArmorDictionaryIterator_t;
// typedef std::pair<RPG_Item_CommodityType,
//                   RPG_Item_CommodityProperties> RPG_ITEM_COMMODITYDICTIONARY_ITEM_T;
typedef std::map<RPG_Item_CommodityBeverage,
                 RPG_Item_CommodityProperties> RPG_Item_CommodityBeverageDictionary_t;
typedef RPG_Item_CommodityBeverageDictionary_t::const_iterator RPG_Item_CommodityBeverageDictionaryIterator_t;
typedef std::map<RPG_Item_CommodityLight,
                 RPG_Item_CommodityProperties> RPG_Item_CommodityLightDictionary_t;
typedef RPG_Item_CommodityLightDictionary_t::const_iterator RPG_Item_CommodityLightDictionaryIterator_t;
// typedef std::pair<RPG_Item_WeaponType,
//                   RPG_Item_WeaponProperties> RPG_ITEM_WEAPONDICTIONARY_ITEM_T;
typedef std::map<RPG_Item_WeaponType,
                 RPG_Item_WeaponProperties> RPG_Item_WeaponDictionary_t;
typedef RPG_Item_WeaponDictionary_t::const_iterator RPG_Item_WeaponDictionaryIterator_t;

#endif
