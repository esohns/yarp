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

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>

#include <rpg_magic_school.h>

#include "rpg_item_weapontype.h"
#include "rpg_item_weaponcategory.h"
#include "rpg_item_weaponclass.h"
#include "rpg_item_storeprice.h"
#include "rpg_item_criticalhitproperties.h"
#include "rpg_item_creationcost.h"
#include "rpg_item_baseproperties.h"
#include "rpg_item_magicalprerequisites.h"
#include "rpg_item_propertiesbase.h"
#include <vector>
#include "rpg_item_weaponpropertiesbase.h"
#include "rpg_item_weaponpropertiesxml.h"
#include "rpg_item_armortype.h"
#include "rpg_item_armorcategory.h"
#include "rpg_item_armorpropertiesbase.h"
#include "rpg_item_armorpropertiesxml.h"

#include <bitset>
#include <set>
#include <map>

// typedef std::bitset<(RPG_COMMON_PHYSICALDAMAGETYPE_MAX - 1)> RPG_Item_WeaponDamage;
typedef std::bitset<5> RPG_Item_WeaponDamageType;

typedef RPG_Dice_Roll RPG_Item_Damage;

// typedef RPG_Item_WeaponPropertiesXML RPG_Item_WeaponProperties;
struct RPG_Item_WeaponProperties
{
  signed char toHitModifier;
  // RPG_Item_WeaponPropertiesBase //
//   RPG_Item_WeaponType weaponType;
  RPG_Item_WeaponCategory weaponCategory;
  RPG_Item_WeaponClass weaponClass;
  RPG_Dice_Roll baseDamage;
  RPG_Item_CriticalHitProperties criticalHit;
  unsigned char rangeIncrement;
  RPG_Item_WeaponDamageType typeOfDamage;
  bool isNonLethal;
  bool isReachWeapon;
  bool isDoubleWeapon;
  // RPG_Item_PropertiesBase //
  RPG_Magic_School aura;
  RPG_Item_MagicalPrerequisites prerequisites;
  // RPG_Item_BaseProperties //
  unsigned short int baseWeight;
  RPG_Item_StorePrice baseStorePrice;
  RPG_Item_CreationCost costToCreate;
};

// typedef RPG_Item_ArmorPropertiesXML RPG_Item_ArmorProperties;
struct RPG_Item_ArmorProperties
{
  signed char defenseModifier;
  // RPG_Item_ArmorPropertiesBase //
//   RPG_Item_ArmorType armorType;
  RPG_Item_ArmorCategory armorCategory;
  unsigned char baseArmorBonus;
  unsigned char maxDexterityBonus;
  signed char armorCheckPenalty;
  unsigned char arcaneSpellFailure;
  unsigned short int baseSpeed;
  // RPG_Item_PropertiesBase //
  RPG_Magic_School aura;
  RPG_Item_MagicalPrerequisites prerequisites;
  // RPG_Item_BaseProperties //
  unsigned short int baseWeight;
  RPG_Item_StorePrice baseStorePrice;
  RPG_Item_CreationCost costToCreate;
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
