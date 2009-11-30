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
#include "rpg_item_XML_parser.h"

#include "rpg_item_common_tools.h"

#include <ace/Log_Msg.h>

RPG_Item_Dictionary_Type::RPG_Item_Dictionary_Type(RPG_Item_WeaponDictionary_t* weaponDictionary_in,
                                                   RPG_Item_ArmorDictionary_t* armorDictionary_in)
 : myWeaponDictionary(weaponDictionary_in),
   myArmorDictionary(armorDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Type::RPG_Item_Dictionary_Type"));

}

RPG_Item_Dictionary_Type::~RPG_Item_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Type::~RPG_Item_Dictionary_Type"));

}

// void RPG_Item_Dictionary_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Type::pre"));
//
// }

// void RPG_Item_Dictionary_Type::RPG_Item_WeaponDictionary()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Type::RPG_Item_WeaponDictionary"));
//
//   // TODO
//   //
// }
//
// void RPG_Item_Dictionary_Type::RPG_Item_ArmorDictionary()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Type::RPG_Item_ArmorDictionary"));
//
//   // TODO
//   //
// }

void RPG_Item_Dictionary_Type::post_RPG_Item_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Type::post_RPG_Item_Dictionary_Type"));

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing item dictionary, retrieved %d weapon and %d armor types...\n"),
//              myWeaponDictionary->size(),
//              myArmorDictionary->size()));
}

RPG_Item_WeaponDictionary_Type::RPG_Item_WeaponDictionary_Type(RPG_Item_WeaponDictionary_t* weaponDictionary_in)
 : myWeaponDictionary(weaponDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponDictionary_Type::RPG_Item_WeaponDictionary_Type"));

}

// void RPG_Item_WeaponDictionary_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponDictionary_Type::pre"));
//
// }

void RPG_Item_WeaponDictionary_Type::weapon(const RPG_Item_WeaponProperties_XML& weapon_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponDictionary_Type::weapon"));

  RPG_Item_WeaponProperties prop;
  prop.weaponCategory = weapon_in.weaponCategory;
  prop.weaponClass = weapon_in.weaponClass;
  prop.baseStorePrice = weapon_in.baseStorePrice;
  prop.baseDamage = weapon_in.baseDamage;
  prop.criticalHitModifier = weapon_in.criticalHitModifier;
  prop.rangeIncrement = weapon_in.rangeIncrement;
  prop.baseWeight = weapon_in.baseWeight;
  prop.typeOfDamage = weapon_in.typeOfDamage;

  myWeaponDictionary->insert(std::make_pair(weapon_in.weaponType, prop));
}

void RPG_Item_WeaponDictionary_Type::post_RPG_Item_WeaponDictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponDictionary_Type::post_RPG_Item_WeaponDictionary_Type"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing weapon dictionary, retrieved %d types...\n"),
             myWeaponDictionary->size()));
}

RPG_Item_ArmorDictionary_Type::RPG_Item_ArmorDictionary_Type(RPG_Item_ArmorDictionary_t* armorDictionary_in)
 : myArmorDictionary(armorDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorDictionary_Type::RPG_Item_ArmorDictionary_Type"));

}

// void RPG_Item_ArmorDictionary_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_ArmorDictionary_Type::pre"));
//
// }

void RPG_Item_ArmorDictionary_Type::armor(const RPG_Item_ArmorProperties_XML& armor_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorDictionary_Type::armor"));

  RPG_Item_ArmorProperties prop;
  prop.armorCategory = armor_in.armorCategory;
  prop.baseStorePrice = armor_in.baseStorePrice;
  prop.baseArmorBonus = armor_in.baseArmorBonus;
  prop.maxDexterityBonus = armor_in.maxDexterityBonus;
  prop.armorCheckPenalty = armor_in.armorCheckPenalty;
  prop.arcaneSpellFailure = armor_in.arcaneSpellFailure;
  prop.baseSpeed = armor_in.baseSpeed;
  prop.baseWeight = armor_in.baseWeight;

  myArmorDictionary->insert(std::make_pair(armor_in.armorType, prop));
}

void RPG_Item_ArmorDictionary_Type::post_RPG_Item_ArmorDictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorDictionary_Type::post_RPG_Item_ArmorDictionary_Type"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing armor dictionary, retrieved %d types...\n"),
             myArmorDictionary->size()));
}

// void RPG_Item_WeaponCategory_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponCategory_Type::pre"));
//
// }

RPG_Item_WeaponCategory RPG_Item_WeaponCategory_Type::post_RPG_Item_WeaponCategory_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponCategory_Type::post_RPG_Item_WeaponCategory_Type"));

  return RPG_Item_Common_Tools::stringToWeaponCategory(post_string());
}

// void RPG_Item_WeaponClass_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponClass_Type::pre"));
//
// }

RPG_Item_WeaponClass RPG_Item_WeaponClass_Type::post_RPG_Item_WeaponClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponClass_Type::post_RPG_Item_WeaponClass_Type"));

  return RPG_Item_Common_Tools::stringToWeaponClass(post_string());
}

// void RPG_Item_WeaponType_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponType_Type::pre"));
//
// }

RPG_Item_WeaponType RPG_Item_WeaponType_Type::post_RPG_Item_WeaponType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponType_Type::post_RPG_Item_WeaponType_Type"));

  return RPG_Item_Common_Tools::stringToWeaponType(post_string());
}

RPG_Item_StorePrice_Type::RPG_Item_StorePrice_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_StorePrice_Type::RPG_Item_StorePrice_Type"));

  myCurrentStorePrice.numGoldPieces = 0;
  myCurrentStorePrice.numSilverPieces = 0;
}

// void RPG_Item_StorePrice_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_StorePrice_Type::pre"));
//
// }

void RPG_Item_StorePrice_Type::numGoldPieces(unsigned int numGoldPieces_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_StorePrice_Type::numGoldPieces"));

  myCurrentStorePrice.numGoldPieces = numGoldPieces_in;
}

void RPG_Item_StorePrice_Type::numSilverPieces(unsigned int numSilverPieces_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_StorePrice_Type::numSilverPieces"));

  myCurrentStorePrice.numSilverPieces = numSilverPieces_in;
}

RPG_Item_StorePrice RPG_Item_StorePrice_Type::post_RPG_Item_StorePrice_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_StorePrice_Type::post_RPG_Item_StorePrice_Type"));

  return myCurrentStorePrice;
}

RPG_Item_CriticalHitModifier_Type::RPG_Item_CriticalHitModifier_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CriticalHitModifier_Type::RPG_Item_CriticalHitModifier_Type"));

  myCurrentCriticalHitModifier.minToHitRoll = 20;
  myCurrentCriticalHitModifier.damageModifier = 1;
}

void RPG_Item_CriticalHitModifier_Type::minToHitRoll(unsigned char minToHitRoll_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CriticalHitModifier_Type::minToHitRoll"));

  myCurrentCriticalHitModifier.minToHitRoll = minToHitRoll_in;
}

void RPG_Item_CriticalHitModifier_Type::damageModifier(unsigned char damageModifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CriticalHitModifier_Type::damageModifier"));

  myCurrentCriticalHitModifier.damageModifier = damageModifier_in;
}

// void RPG_Item_CriticalHitModifier_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_CriticalHitModifier_Type::pre"));
//
// }

RPG_Item_CriticalHitModifier RPG_Item_CriticalHitModifier_Type::post_RPG_Item_CriticalHitModifier_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CriticalHitModifier_Type::post_RPG_Item_CriticalHitModifier_Type"));

  return myCurrentCriticalHitModifier;
}

// void RPG_Item_WeaponDamageType_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponDamageType_Type::pre"));
//
// }

RPG_Item_WeaponDamageType RPG_Item_WeaponDamageType_Type::post_RPG_Item_WeaponDamageType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponDamageType_Type::post_RPG_Item_WeaponDamageType_Type"));

  return RPG_Item_Common_Tools::stringToWeaponDamageType(post_string());
}

RPG_Item_WeaponProperties_Type::RPG_Item_WeaponProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::RPG_Item_WeaponProperties_Type"));

  // reset current properties
  myCurrentWeaponProperty.weaponCategory = WEAPONCATEGORY_INVALID;
  myCurrentWeaponProperty.weaponClass = WEAPONCLASS_INVALID;
  myCurrentWeaponProperty.baseStorePrice.numGoldPieces = 0;
  myCurrentWeaponProperty.baseStorePrice.numSilverPieces = 0;
  myCurrentWeaponProperty.baseDamage.numDice = 0;
  myCurrentWeaponProperty.baseDamage.typeDice = D_TYPE_INVALID;
  myCurrentWeaponProperty.baseDamage.modifier = 0;
  myCurrentWeaponProperty.criticalHitModifier.minToHitRoll = 20;
  myCurrentWeaponProperty.criticalHitModifier.damageModifier = 1;
  myCurrentWeaponProperty.rangeIncrement = 0;
  myCurrentWeaponProperty.baseWeight = 0;
  myCurrentWeaponProperty.typeOfDamage.reset();
}

// void RPG_Item_WeaponProperties_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::pre"));
//
// }

void RPG_Item_WeaponProperties_Type::weaponType(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::weaponType"));

  myCurrentWeaponProperty.weaponType = weaponType_in;
}

void RPG_Item_WeaponProperties_Type::weaponCategory(const RPG_Item_WeaponCategory& weaponCategory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::weaponCategory"));

  myCurrentWeaponProperty.weaponCategory = weaponCategory_in;
}

void RPG_Item_WeaponProperties_Type::weaponClass(const RPG_Item_WeaponClass& weaponClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::weaponClass"));

  myCurrentWeaponProperty.weaponClass = weaponClass_in;
}

void RPG_Item_WeaponProperties_Type::baseStorePrice(const RPG_Item_StorePrice& baseStorePrice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::baseStorePrice"));

  myCurrentWeaponProperty.baseStorePrice = baseStorePrice_in;
}

void RPG_Item_WeaponProperties_Type::baseDamage(const RPG_Item_Damage& baseDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::baseDamage"));

  myCurrentWeaponProperty.baseDamage = baseDamage_in;
}

void RPG_Item_WeaponProperties_Type::criticalHitModifier(const RPG_Item_CriticalHitModifier& criticalHitModifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::criticalHitModifier"));

  myCurrentWeaponProperty.criticalHitModifier = criticalHitModifier_in;
}

void RPG_Item_WeaponProperties_Type::rangeIncrement(unsigned char rangeIncrement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::rangeIncrement"));

  myCurrentWeaponProperty.rangeIncrement = rangeIncrement_in;
}

void RPG_Item_WeaponProperties_Type::baseWeight(unsigned short baseWeight_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::baseWeight"));

  myCurrentWeaponProperty.baseWeight = baseWeight_in;
}

void RPG_Item_WeaponProperties_Type::typeOfDamage(const RPG_Item_WeaponDamageType& typeOfDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::typeOfDamage"));

  // *IMPORTANT NOTE*: a weapon can exert more than one type of damage...
  myCurrentWeaponProperty.typeOfDamage |= RPG_Item_WeaponDamage(typeOfDamage_in);

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("current weapon damage: \"%s\"\n"),
//              myCurrentWeaponProperty.typeOfDamage.to_string().c_str()));
}

RPG_Item_WeaponProperties_XML RPG_Item_WeaponProperties_Type::post_RPG_Item_WeaponProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::post_RPG_Item_WeaponProperties_Type"));

  RPG_Item_WeaponProperties_XML result = myCurrentWeaponProperty;

  // reset current properties
  myCurrentWeaponProperty.weaponCategory = WEAPONCATEGORY_INVALID;
  myCurrentWeaponProperty.weaponClass = WEAPONCLASS_INVALID;
  myCurrentWeaponProperty.baseStorePrice.numGoldPieces = 0;
  myCurrentWeaponProperty.baseStorePrice.numSilverPieces = 0;
  myCurrentWeaponProperty.baseDamage.numDice = 0;
  myCurrentWeaponProperty.baseDamage.typeDice = D_TYPE_INVALID;
  myCurrentWeaponProperty.baseDamage.modifier = 0;
  myCurrentWeaponProperty.criticalHitModifier.minToHitRoll = 20;
  myCurrentWeaponProperty.criticalHitModifier.damageModifier = 1;
  myCurrentWeaponProperty.rangeIncrement = 0;
  myCurrentWeaponProperty.baseWeight = 0;
  myCurrentWeaponProperty.typeOfDamage.reset();

  return result;
}

// void RPG_Item_ArmorCategory_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_ArmorCategory_Type::pre"));
//
// }

RPG_Item_ArmorCategory RPG_Item_ArmorCategory_Type::post_RPG_Item_ArmorCategory_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorCategory_Type::post_RPG_Item_ArmorCategory_Type"));

  return RPG_Item_Common_Tools::stringToArmorCategory(post_string());
}

// void RPG_Item_ArmorType_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_ArmorType_Type::pre"));
//
// }

RPG_Item_ArmorType RPG_Item_ArmorType_Type::post_RPG_Item_ArmorType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorType_Type::post_RPG_Item_ArmorType_Type"));

  return RPG_Item_Common_Tools::stringToArmorType(post_string());
}

RPG_Item_ArmorProperties_Type::RPG_Item_ArmorProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::RPG_Item_ArmorProperties_Type"));

  // reset properties
  myCurrentArmorProperty.armorType = ARMOR_TYPE_INVALID;
  myCurrentArmorProperty.armorCategory = ARMORCATEGORY_INVALID;
  myCurrentArmorProperty.baseStorePrice.numGoldPieces = 0;
  myCurrentArmorProperty.baseStorePrice.numSilverPieces = 0;
  myCurrentArmorProperty.baseArmorBonus = 0;
  myCurrentArmorProperty.maxDexterityBonus = 0;
  myCurrentArmorProperty.armorCheckPenalty = 0;
  myCurrentArmorProperty.arcaneSpellFailure = 0;
  myCurrentArmorProperty.baseSpeed = 0;
  myCurrentArmorProperty.baseWeight = 0;
}

// void RPG_Item_ArmorProperties_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::pre"));
//
// }

void RPG_Item_ArmorProperties_Type::armorType(const RPG_Item_ArmorType& armorType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::armorType"));

  myCurrentArmorProperty.armorType = armorType_in;
}

void RPG_Item_ArmorProperties_Type::armorCategory(const RPG_Item_ArmorCategory& armorCategory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::armorCategory"));

  myCurrentArmorProperty.armorCategory = armorCategory_in;
}

void RPG_Item_ArmorProperties_Type::baseStorePrice(const RPG_Item_StorePrice& baseStorePrice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::baseStorePrice"));

  myCurrentArmorProperty.baseStorePrice = baseStorePrice_in;
}

void RPG_Item_ArmorProperties_Type::baseArmorBonus(unsigned char baseArmorBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::baseArmorBonus"));

  myCurrentArmorProperty.baseArmorBonus = baseArmorBonus_in;
}

void RPG_Item_ArmorProperties_Type::maxDexterityBonus(unsigned char maxDexterityBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::maxDexterityBonus"));

  myCurrentArmorProperty.maxDexterityBonus = maxDexterityBonus_in;
}

void RPG_Item_ArmorProperties_Type::armorCheckPenalty(signed char armorCheckPenalty_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::armorCheckPenalty"));

  myCurrentArmorProperty.armorCheckPenalty = armorCheckPenalty_in;
}

void RPG_Item_ArmorProperties_Type::arcaneSpellFailure(unsigned char arcaneSpellFailure_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::arcaneSpellFailure"));

  myCurrentArmorProperty.arcaneSpellFailure = arcaneSpellFailure_in;
}

void RPG_Item_ArmorProperties_Type::baseSpeed(unsigned short baseSpeed_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::baseSpeed"));

  myCurrentArmorProperty.baseSpeed = baseSpeed_in;
}

void RPG_Item_ArmorProperties_Type::baseWeight(unsigned short baseWeight_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::baseWeight"));

  myCurrentArmorProperty.baseWeight = baseWeight_in;
}

RPG_Item_ArmorProperties_XML RPG_Item_ArmorProperties_Type::post_RPG_Item_ArmorProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::post_RPG_Item_ArmorProperties_Type"));

  RPG_Item_ArmorProperties_XML result = myCurrentArmorProperty;

  // reset properties
  myCurrentArmorProperty.armorType = ARMOR_TYPE_INVALID;
  myCurrentArmorProperty.armorCategory = ARMORCATEGORY_INVALID;
  myCurrentArmorProperty.baseStorePrice.numGoldPieces = 0;
  myCurrentArmorProperty.baseStorePrice.numSilverPieces = 0;
  myCurrentArmorProperty.baseArmorBonus = 0;
  myCurrentArmorProperty.maxDexterityBonus = 0;
  myCurrentArmorProperty.armorCheckPenalty = 0;
  myCurrentArmorProperty.arcaneSpellFailure = 0;
  myCurrentArmorProperty.baseSpeed = 0;
  myCurrentArmorProperty.baseWeight = 0;

  return result;
}
