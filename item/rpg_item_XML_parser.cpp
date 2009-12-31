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

#include "rpg_item_weapontype.h"
#include "rpg_item_weaponcategory.h"
#include "rpg_item_weaponclass.h"
#include "rpg_item_physicaldamagetype.h"
#include "rpg_item_armorcategory.h"

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

void RPG_Item_WeaponDictionary_Type::weapon(const RPG_Item_WeaponPropertiesXML& weapon_in)
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
  prop.typeOfDamage = PHYSICALDAMAGE_NONE;
  for (std::vector<RPG_Item_PhysicalDamageType>::const_iterator iterator = weapon_in.typeOfDamages.begin();
       iterator != weapon_in.typeOfDamages.end();
       iterator++)
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("weapon %s: damage: %s\n"),
//                RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon_in.weaponType).c_str(),
//                RPG_Item_PhysicalDamageTypeHelper::RPG_Item_PhysicalDamageTypeToString(*iterator).c_str()));

    // *IMPORTANT NOTE*: consider that some weapons don't do any damage...
    // --> in that case, the element could be empty OR set to this
    if ((*iterator) != PHYSICALDAMAGE_NONE)
      prop.typeOfDamage.set((*iterator - 1));
  } // end FOR

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

void RPG_Item_ArmorDictionary_Type::armor(const RPG_Item_ArmorPropertiesXML& armor_in)
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

  return RPG_Item_WeaponCategoryHelper::stringToRPG_Item_WeaponCategory(post_string());
}

// void RPG_Item_WeaponClass_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponClass_Type::pre"));
//
// }

RPG_Item_WeaponClass RPG_Item_WeaponClass_Type::post_RPG_Item_WeaponClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponClass_Type::post_RPG_Item_WeaponClass_Type"));

  return RPG_Item_WeaponClassHelper::stringToRPG_Item_WeaponClass(post_string());
}

// void RPG_Item_WeaponType_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponType_Type::pre"));
//
// }

RPG_Item_WeaponType RPG_Item_WeaponType_Type::post_RPG_Item_WeaponType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponType_Type::post_RPG_Item_WeaponType_Type"));

  return RPG_Item_WeaponTypeHelper::stringToRPG_Item_WeaponType(post_string());
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

RPG_Item_CriticalHitModifier RPG_Item_CriticalHitModifier_Type::post_RPG_Item_CriticalHitModifier_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CriticalHitModifier_Type::post_RPG_Item_CriticalHitModifier_Type"));

  return myCurrentCriticalHitModifier;
}

RPG_Item_PhysicalDamageType RPG_Item_PhysicalDamageType_Type::post_RPG_Item_PhysicalDamageType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_PhysicalDamageType_Type::post_RPG_Item_PhysicalDamageType_Type"));

  return RPG_Item_PhysicalDamageTypeHelper::stringToRPG_Item_PhysicalDamageType(post_string());
}

RPG_Item_CreationCost_Type::RPG_Item_CreationCost_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CreationCost_Type::RPG_Item_CreationCost_Type"));

  myCurrentCreationCost.numGoldPieces = 0;
  myCurrentCreationCost.numExperiencePoints = 0;
}

void RPG_Item_CreationCost_Type::numGoldPieces(unsigned int goldPieces_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CreationCost_Type::numGoldPieces"));

  myCurrentCreationCost.numGoldPieces = goldPieces_in;
}

void RPG_Item_CreationCost_Type::numExperiencePoints(unsigned int experiencePoints_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CreationCost_Type::numExperiencePoints"));

  myCurrentCreationCost.numExperiencePoints = experiencePoints_in;
}

RPG_Item_CreationCost RPG_Item_CreationCost_Type::post_RPG_Item_CreationCost_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_CreationCost_Type::post_RPG_Item_CreationCost_Type"));

  RPG_Item_CreationCost result = myCurrentCreationCost;

  myCurrentCreationCost.numGoldPieces = 0;
  myCurrentCreationCost.numExperiencePoints = 0;

  return result;
}

RPG_Item_BaseProperties_Type::RPG_Item_BaseProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_BaseProperties_Type::RPG_Item_BaseProperties_Type"));

  myBaseProperties.baseWeight = 0;
  myBaseProperties.baseStorePrice.numGoldPieces = 0;
  myBaseProperties.baseStorePrice.numSilverPieces = 0;
  myBaseProperties.costToCreate.numGoldPieces = 0;
  myBaseProperties.costToCreate.numExperiencePoints = 0;
}

void RPG_Item_BaseProperties_Type::baseWeight(unsigned short baseWeight_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_BaseProperties_Type::baseWeight"));

  myBaseProperties.baseWeight = baseWeight_in;
}

void RPG_Item_BaseProperties_Type::baseStorePrice(const RPG_Item_StorePrice& baseStorePrice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_BaseProperties_Type::baseStorePrice"));

  myBaseProperties.baseStorePrice = baseStorePrice_in;
}

void RPG_Item_BaseProperties_Type::costToCreate(const RPG_Item_CreationCost& creationCost_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_BaseProperties_Type::costToCreate"));

  myBaseProperties.costToCreate = creationCost_in;
}

RPG_Item_BaseProperties RPG_Item_BaseProperties_Type::post_RPG_Item_BaseProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_BaseProperties_Type::post_RPG_Item_BaseProperties_Type"));

  RPG_Item_BaseProperties result = myBaseProperties;

  myBaseProperties.baseWeight = 0;
  myBaseProperties.baseStorePrice.numGoldPieces = 0;
  myBaseProperties.baseStorePrice.numSilverPieces = 0;
  myBaseProperties.costToCreate.numGoldPieces = 0;
  myBaseProperties.costToCreate.numExperiencePoints = 0;

  return result;
}

RPG_Item_MagicalPrerequisites_Type::RPG_Item_MagicalPrerequisites_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalPrerequisites_Type::RPG_Item_MagicalPrerequisites_Type"));

  myCurrentPrerequisites.minCasterLevel = 0;
}

void RPG_Item_MagicalPrerequisites_Type::minCasterLevel(unsigned char minCasterLevel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalPrerequisites_Type::minCasterLevel"));

  myCurrentPrerequisites.minCasterLevel = minCasterLevel_in;
}

RPG_Item_MagicalPrerequisites RPG_Item_MagicalPrerequisites_Type::post_RPG_Item_MagicalPrerequisites_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalPrerequisites_Type::post_RPG_Item_MagicalPrerequisites_Type"));

  RPG_Item_MagicalPrerequisites result = myCurrentPrerequisites;

  myCurrentPrerequisites.minCasterLevel = 0;

  return result;
}

RPG_Item_MagicalProperties_Type::RPG_Item_MagicalProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalProperties_Type::RPG_Item_MagicalProperties_Type"));

  myCurrentProperties.Aura = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.Prerequisites.minCasterLevel = 0;
}

void RPG_Item_MagicalProperties_Type::Aura(const RPG_Magic_School& school_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalProperties_Type::Aura"));

  myCurrentProperties.Aura = school_in;
}

void RPG_Item_MagicalProperties_Type::Prerequisites(const RPG_Item_MagicalPrerequisites& prerequisites_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalProperties_Type::Prerequisites"));

  myCurrentProperties.Prerequisites = prerequisites_in;
}

RPG_Item_MagicalProperties RPG_Item_MagicalProperties_Type::post_RPG_Item_MagicalProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_MagicalProperties_Type::post_RPG_Item_MagicalProperties_Type"));

  RPG_Item_MagicalProperties result = myCurrentProperties;

  myCurrentProperties.Aura = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.Prerequisites.minCasterLevel = 0;

  return result;
}

RPG_Item_WeaponPropertiesXML_Type::RPG_Item_WeaponPropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::RPG_Item_WeaponPropertiesXML_Type"));

  // reset current properties
  myCurrentWeaponProperties.weaponType = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentWeaponProperties.weaponCategory = RPG_ITEM_WEAPONCATEGORY_INVALID;
  myCurrentWeaponProperties.weaponClass = RPG_ITEM_WEAPONCLASS_INVALID;
  myCurrentWeaponProperties.baseStorePrice.numGoldPieces = 0;
  myCurrentWeaponProperties.baseStorePrice.numSilverPieces = 0;
  myCurrentWeaponProperties.baseDamage.numDice = 0;
  myCurrentWeaponProperties.baseDamage.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentWeaponProperties.baseDamage.modifier = 0;
  myCurrentWeaponProperties.criticalHitModifier.minToHitRoll = 20;
  myCurrentWeaponProperties.criticalHitModifier.damageModifier = 1;
  myCurrentWeaponProperties.rangeIncrement = 0;
  myCurrentWeaponProperties.baseWeight = 0;
  myCurrentWeaponProperties.typeOfDamages.clear();
}

// void RPG_Item_WeaponProperties_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_WeaponProperties_Type::pre"));
//
// }

void RPG_Item_WeaponPropertiesXML_Type::weaponType(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::weaponType"));

  myCurrentWeaponProperties.weaponType = weaponType_in;
}

void RPG_Item_WeaponPropertiesXML_Type::weaponCategory(const RPG_Item_WeaponCategory& weaponCategory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::weaponCategory"));

  myCurrentWeaponProperties.weaponCategory = weaponCategory_in;
}

void RPG_Item_WeaponPropertiesXML_Type::weaponClass(const RPG_Item_WeaponClass& weaponClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::weaponClass"));

  myCurrentWeaponProperties.weaponClass = weaponClass_in;
}

void RPG_Item_WeaponPropertiesXML_Type::baseStorePrice(const RPG_Item_StorePrice& baseStorePrice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::baseStorePrice"));

  myCurrentWeaponProperties.baseStorePrice = baseStorePrice_in;
}

void RPG_Item_WeaponPropertiesXML_Type::baseDamage(const RPG_Item_Damage& baseDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::baseDamage"));

  myCurrentWeaponProperties.baseDamage = baseDamage_in;
}

void RPG_Item_WeaponPropertiesXML_Type::criticalHitModifier(const RPG_Item_CriticalHitModifier& criticalHitModifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::criticalHitModifier"));

  myCurrentWeaponProperties.criticalHitModifier = criticalHitModifier_in;
}

void RPG_Item_WeaponPropertiesXML_Type::rangeIncrement(unsigned char rangeIncrement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::rangeIncrement"));

  myCurrentWeaponProperties.rangeIncrement = rangeIncrement_in;
}

void RPG_Item_WeaponPropertiesXML_Type::baseWeight(unsigned short baseWeight_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::baseWeight"));

  myCurrentWeaponProperties.baseWeight = baseWeight_in;
}

void RPG_Item_WeaponPropertiesXML_Type::typeOfDamage(const RPG_Item_PhysicalDamageType& typeOfDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::typeOfDamage"));

  myCurrentWeaponProperties.typeOfDamages.push_back(typeOfDamage_in);
}

RPG_Item_WeaponPropertiesXML RPG_Item_WeaponPropertiesXML_Type::post_RPG_Item_WeaponPropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_WeaponPropertiesXML_Type::post_RPG_Item_WeaponPropertiesXML_Type"));

  RPG_Item_WeaponPropertiesXML result = myCurrentWeaponProperties;

  // reset current properties
  myCurrentWeaponProperties.weaponType = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentWeaponProperties.weaponCategory = RPG_ITEM_WEAPONCATEGORY_INVALID;
  myCurrentWeaponProperties.weaponClass = RPG_ITEM_WEAPONCLASS_INVALID;
  myCurrentWeaponProperties.baseStorePrice.numGoldPieces = 0;
  myCurrentWeaponProperties.baseStorePrice.numSilverPieces = 0;
  myCurrentWeaponProperties.baseDamage.numDice = 0;
  myCurrentWeaponProperties.baseDamage.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentWeaponProperties.baseDamage.modifier = 0;
  myCurrentWeaponProperties.criticalHitModifier.minToHitRoll = 20;
  myCurrentWeaponProperties.criticalHitModifier.damageModifier = 1;
  myCurrentWeaponProperties.rangeIncrement = 0;
  myCurrentWeaponProperties.baseWeight = 0;
  myCurrentWeaponProperties.typeOfDamages.clear();

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

  return RPG_Item_ArmorCategoryHelper::stringToRPG_Item_ArmorCategory(post_string());
}

// void RPG_Item_ArmorType_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_ArmorType_Type::pre"));
//
// }

RPG_Item_ArmorType RPG_Item_ArmorType_Type::post_RPG_Item_ArmorType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorType_Type::post_RPG_Item_ArmorType_Type"));

  return RPG_Item_ArmorTypeHelper::stringToRPG_Item_ArmorType(post_string());
}

RPG_Item_ArmorPropertiesXML_Type::RPG_Item_ArmorPropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::RPG_Item_ArmorPropertiesXML_Type"));

  // reset properties
  myCurrentArmorProperties.armorType = RPG_ITEM_ARMORTYPE_INVALID;
  myCurrentArmorProperties.armorCategory = RPG_ITEM_ARMORCATEGORY_INVALID;
  myCurrentArmorProperties.baseStorePrice.numGoldPieces = 0;
  myCurrentArmorProperties.baseStorePrice.numSilverPieces = 0;
  myCurrentArmorProperties.baseArmorBonus = 0;
  myCurrentArmorProperties.maxDexterityBonus = 0;
  myCurrentArmorProperties.armorCheckPenalty = 0;
  myCurrentArmorProperties.arcaneSpellFailure = 0;
  myCurrentArmorProperties.baseSpeed = 0;
  myCurrentArmorProperties.baseWeight = 0;
}

// void RPG_Item_ArmorProperties_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_ArmorProperties_Type::pre"));
//
// }

void RPG_Item_ArmorPropertiesXML_Type::armorType(const RPG_Item_ArmorType& armorType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::armorType"));

  myCurrentArmorProperties.armorType = armorType_in;
}

void RPG_Item_ArmorPropertiesXML_Type::armorCategory(const RPG_Item_ArmorCategory& armorCategory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::armorCategory"));

  myCurrentArmorProperties.armorCategory = armorCategory_in;
}

void RPG_Item_ArmorPropertiesXML_Type::baseStorePrice(const RPG_Item_StorePrice& baseStorePrice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::baseStorePrice"));

  myCurrentArmorProperties.baseStorePrice = baseStorePrice_in;
}

void RPG_Item_ArmorPropertiesXML_Type::baseArmorBonus(unsigned char baseArmorBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::baseArmorBonus"));

  myCurrentArmorProperties.baseArmorBonus = baseArmorBonus_in;
}

void RPG_Item_ArmorPropertiesXML_Type::maxDexterityBonus(unsigned char maxDexterityBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::maxDexterityBonus"));

  myCurrentArmorProperties.maxDexterityBonus = maxDexterityBonus_in;
}

void RPG_Item_ArmorPropertiesXML_Type::armorCheckPenalty(signed char armorCheckPenalty_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::armorCheckPenalty"));

  myCurrentArmorProperties.armorCheckPenalty = armorCheckPenalty_in;
}

void RPG_Item_ArmorPropertiesXML_Type::arcaneSpellFailure(unsigned char arcaneSpellFailure_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::arcaneSpellFailure"));

  myCurrentArmorProperties.arcaneSpellFailure = arcaneSpellFailure_in;
}

void RPG_Item_ArmorPropertiesXML_Type::baseSpeed(unsigned short baseSpeed_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::baseSpeed"));

  myCurrentArmorProperties.baseSpeed = baseSpeed_in;
}

void RPG_Item_ArmorPropertiesXML_Type::baseWeight(unsigned short baseWeight_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::baseWeight"));

  myCurrentArmorProperties.baseWeight = baseWeight_in;
}

RPG_Item_ArmorPropertiesXML RPG_Item_ArmorPropertiesXML_Type::post_RPG_Item_ArmorPropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_ArmorPropertiesXML_Type::post_RPG_Item_ArmorPropertiesXML_Type"));

  RPG_Item_ArmorPropertiesXML result = myCurrentArmorProperties;

  // reset properties
  myCurrentArmorProperties.armorType = RPG_ITEM_ARMORTYPE_INVALID;
  myCurrentArmorProperties.armorCategory = RPG_ITEM_ARMORCATEGORY_INVALID;
  myCurrentArmorProperties.baseStorePrice.numGoldPieces = 0;
  myCurrentArmorProperties.baseStorePrice.numSilverPieces = 0;
  myCurrentArmorProperties.baseArmorBonus = 0;
  myCurrentArmorProperties.maxDexterityBonus = 0;
  myCurrentArmorProperties.armorCheckPenalty = 0;
  myCurrentArmorProperties.arcaneSpellFailure = 0;
  myCurrentArmorProperties.baseSpeed = 0;
  myCurrentArmorProperties.baseWeight = 0;

  return result;
}
