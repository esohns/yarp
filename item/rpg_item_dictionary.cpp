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
#include "stdafx.h"

#include "rpg_item_dictionary.h"

#include "rpg_dice_common_tools.h"
#include "rpg_dice_XML_parser.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
//#include "rpg_common_xsderrorhandler.h"
#include "rpg_common_XML_tools.h"
#include "rpg_common_XML_parser.h"

#include "rpg_magic_XML_parser.h"

#include "rpg_item_common.h"
#include "rpg_item_base.h"
#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_weapon.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_XML_parser.h"

#include <ace/Log_Msg.h>

#include <sstream>

RPG_Item_Dictionary::RPG_Item_Dictionary()
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::RPG_Item_Dictionary"));

}

RPG_Item_Dictionary::~RPG_Item_Dictionary()
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::~RPG_Item_Dictionary"));

}

void
RPG_Item_Dictionary::init(const std::string& filename_in,
                          const bool& validateXML_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::init"));

  // Construct the parser.
  //
  ::xml_schema::unsigned_short_pimpl      unsigned_short_p;

  ::xml_schema::unsigned_int_pimpl        unsigned_int_p;
  RPG_Item_StorePrice_Type                baseStorePrice_p;
  baseStorePrice_p.parsers(unsigned_int_p,
                           unsigned_int_p);

  RPG_Item_CreationCost_Type              creationCost_p;
  creationCost_p.parsers(unsigned_int_p,
                         unsigned_int_p);

  RPG_Magic_School_Type                   school_p;

  ::xml_schema::unsigned_byte_pimpl       unsigned_byte_p;
  RPG_Item_MagicalPrerequisites_Type      magicalPreRequisites_p;
  magicalPreRequisites_p.parsers(unsigned_byte_p);

  RPG_Item_ArmorType_Type                 armorType_p;
  RPG_Item_ArmorCategory_Type             armorCategory_p;
  ::xml_schema::byte_pimpl                byte_p;
  RPG_Item_ArmorPropertiesXML_Type        armorPropertiesXML_p;
  armorPropertiesXML_p.parsers(unsigned_short_p,
                               baseStorePrice_p,
                               creationCost_p,
                               school_p,
                               magicalPreRequisites_p,
                               armorType_p,
                               armorCategory_p,
                               unsigned_byte_p,
                               unsigned_byte_p,
                               byte_p,
                               unsigned_byte_p,
                               unsigned_byte_p,
                               byte_p);

  RPG_Item_CommodityType_Type           commodityType_p;
  RPG_Item_CommodityUnion_Type          commodityUnion_p;
  RPG_Item_CommodityPropertiesBase_Type commodityPropertiesBase_p;
  commodityPropertiesBase_p.parsers(unsigned_short_p,
                                    baseStorePrice_p,
                                    creationCost_p,
                                    school_p,
                                    magicalPreRequisites_p,
                                    commodityType_p,
                                    commodityUnion_p);

  RPG_Item_WeaponType_Type                weaponType_p;
  RPG_Item_WeaponCategory_Type            weaponCategory_p;
  RPG_Item_WeaponClass_Type               weaponClass_p;
  RPG_Dice_DieType_Type                   dieType_p;
  ::xml_schema::int_pimpl                 int_p;
  RPG_Dice_Roll_Type                      roll_p;
  roll_p.parsers(unsigned_int_p,
                 dieType_p,
                 int_p);
  RPG_Item_CriticalHitProperties_Type     criticalHitProperties_p;
  criticalHitProperties_p.parsers(unsigned_byte_p,
                                  unsigned_byte_p);
//   unsigned_byte_pimpl                       rangeIncrement_p;
  RPG_Common_PhysicalDamageType_Type      damageType_p;
  ::xml_schema::boolean_pimpl             bool_p;
  RPG_Item_WeaponPropertiesXML_Type       weaponPropertiesXML_p;
  weaponPropertiesXML_p.parsers(unsigned_short_p,
                                baseStorePrice_p,
                                creationCost_p,
                                school_p,
                                magicalPreRequisites_p,
                                weaponType_p,
                                weaponCategory_p,
                                weaponClass_p,
                                roll_p,
                                criticalHitProperties_p,
                                unsigned_byte_p,
                                damageType_p,
                                bool_p,
                                bool_p,
                                bool_p,
                                byte_p);
  RPG_Item_Dictionary_Type                itemDictionary_p(&myArmorDictionary,
                                                           &myCommodityBeverageDictionary,
                                                           &myCommodityLightDictionary,
                                                           &myWeaponDictionary);
  itemDictionary_p.parsers(armorPropertiesXML_p,
                           commodityPropertiesBase_p,
                           weaponPropertiesXML_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(itemDictionary_p,                                      // parser
                               ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE), // namespace
                               ACE_TEXT_ALWAYS_CHAR("itemDictionary"),                // root element name
															 false);                                                // polymorphic ?

  itemDictionary_p.pre();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
  ::xml_schema::properties properties;
  try
  {
    //doc_p.parse(filename_in,
    //            RPG_XSDErrorHandler,
    //            flags);
    doc_p.parse(filename_in,
                *RPG_Common_XML_Tools::parser(),
                flags,
                properties);
  }
  catch (const ::xml_schema::parsing& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::init(): exception occurred: \"%s\", returning\n"),
               text.c_str()));

    return;
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::init(): exception occurred, returning\n")));

    return;
  }

  itemDictionary_p.post_RPG_Item_Dictionary_Type();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing item dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Item_PropertiesBase&
RPG_Item_Dictionary::getProperties(const RPG_Item_Base* item_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::getProperties"));

  ACE_ASSERT(item_in);

  switch (item_in->getType())
  {
    case ITEM_ARMOR:
    {
      const RPG_Item_Armor* armor = dynamic_cast<const RPG_Item_Armor*>(item_in);
      ACE_ASSERT(armor);

      RPG_Item_ArmorProperties properties = getArmorProperties(armor->getArmorType());
      return properties;
    }
    case ITEM_COMMODITY:
    {
      const RPG_Item_Commodity* commodity = dynamic_cast<const RPG_Item_Commodity*>(item_in);
      ACE_ASSERT(commodity);

      RPG_Item_CommodityProperties properties = getCommodityProperties(commodity->getCommoditySubType());
      return properties;
    }
    case ITEM_OTHER:
    case ITEM_VALUABLE:
    {
      // *TODO*
      ACE_ASSERT(false);

      break;
    }
    case ITEM_WEAPON:
    {
      const RPG_Item_Weapon* weapon = dynamic_cast<const RPG_Item_Weapon*>(item_in);
      ACE_ASSERT(weapon);

      RPG_Item_WeaponProperties properties = getWeaponProperties(weapon->getWeaponType());
      return properties;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid item type (was: \"%s\"), aborting\n"),
                 RPG_Item_TypeHelper::RPG_Item_TypeToString(item_in->getType()).c_str()));

      break;
    }
  } // end SWITCH

  ACE_ASSERT(false);
  // *TODO*: clean this up
  RPG_Item_PropertiesBase* dummy = new(std::nothrow) RPG_Item_PropertiesBase;
#if defined (_MSC_VER)
  return *dummy;
#else
  ACE_NOTREACHED(return *dummy;)
#endif
}

const RPG_Item_CommodityProperties
RPG_Item_Dictionary::getCommodityProperties(const RPG_Item_CommodityUnion& commoditySubType_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::getCommodityProperties"));

  switch (commoditySubType_in.discriminator)
  {
    case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
    {
      RPG_Item_CommodityBeverageDictionaryIterator_t iterator = myCommodityBeverageDictionary.find(commoditySubType_in.commoditybeverage);
      if (iterator == myCommodityBeverageDictionary.end())
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("invalid commodity beverage type \"%s\", aborting\n"),
                   RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString(commoditySubType_in.commoditybeverage).c_str()));

        break;
      } // end IF

      return (*iterator).second;
    }
    case RPG_Item_CommodityUnion::COMMODITYLIGHT:
    {
      RPG_Item_CommodityLightDictionaryIterator_t iterator = myCommodityLightDictionary.find(commoditySubType_in.commoditylight);
      if (iterator == myCommodityLightDictionary.end())
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("invalid commodity light type \"%s\", aborting\n"),
                   RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString(commoditySubType_in.commoditylight).c_str()));

        break;
      } // end IF

      return (*iterator).second;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid commodity type (was: %d), aborting\n"),
                 commoditySubType_in.discriminator));

      break;
    }
  } // end SWITCH

  ACE_ASSERT(false);
  RPG_Item_CommodityProperties dummy;
  dummy.aura = RPG_MAGIC_SCHOOL_INVALID;
  dummy.baseStorePrice.numGoldPieces = 0;
  dummy.baseStorePrice.numSilverPieces = 0;
  dummy.baseWeight = 0;
  dummy.costToCreate.numExperiencePoints = 0;
  dummy.costToCreate.numGoldPieces = 0;
  dummy.prerequisites.minCasterLevel = 0;
#if defined (_MSC_VER)
  return dummy;
#else
  ACE_NOTREACHED(return dummy;)
#endif
}

const RPG_Item_ArmorProperties
RPG_Item_Dictionary::getArmorProperties(const RPG_Item_ArmorType& armorType_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::getArmorProperties"));

  RPG_Item_ArmorDictionaryIterator_t iterator = myArmorDictionary.find(armorType_in);
  if (iterator == myArmorDictionary.end())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid armor type \"%s\", aborting\n"),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armorType_in).c_str()));

    ACE_ASSERT(false);
    RPG_Item_ArmorProperties dummy;
    dummy.aura = RPG_MAGIC_SCHOOL_INVALID;
    dummy.baseStorePrice.numGoldPieces = 0;
    dummy.baseStorePrice.numSilverPieces = 0;
    dummy.baseWeight = 0;
    dummy.costToCreate.numExperiencePoints = 0;
    dummy.costToCreate.numGoldPieces = 0;
    dummy.prerequisites.minCasterLevel = 0;
    dummy.arcaneSpellFailure = 0;
    dummy.baseBonus = 0;
    dummy.baseSpeed = 0;
    dummy.category = RPG_ITEM_ARMORCATEGORY_INVALID;
    dummy.checkPenalty = 0;
    dummy.defenseModifier = 0;
    dummy.maxDexterityBonus = 0;
#if defined (_MSC_VER)
    return dummy;
#else
    ACE_NOTREACHED(return dummy;)
#endif
  } // end IF

  return iterator->second;
}

const RPG_Item_WeaponProperties
RPG_Item_Dictionary::getWeaponProperties(const RPG_Item_WeaponType& weaponType_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::getWeaponProperties"));

  RPG_Item_WeaponDictionaryIterator_t iterator = myWeaponDictionary.find(weaponType_in);
  if (iterator == myWeaponDictionary.end())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid weapon type \"%s\", aborting\n"),
               RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weaponType_in).c_str()));

    ACE_ASSERT(false);
    RPG_Item_WeaponProperties dummy;
    dummy.aura = RPG_MAGIC_SCHOOL_INVALID;
    dummy.baseStorePrice.numGoldPieces = 0;
    dummy.baseStorePrice.numSilverPieces = 0;
    dummy.baseWeight = 0;
    dummy.costToCreate.numExperiencePoints = 0;
    dummy.costToCreate.numGoldPieces = 0;
    dummy.prerequisites.minCasterLevel = 0;
    dummy.baseDamage.modifier = 0;
    dummy.baseDamage.numDice = 0;
    dummy.baseDamage.typeDice = RPG_DICE_DIETYPE_INVALID;
    dummy.category = RPG_ITEM_WEAPONCATEGORY_INVALID;
    dummy.criticalHit.damageModifier = 0;
    dummy.criticalHit.minToHitRoll = 0;
    dummy.isDoubleWeapon = false;
    dummy.isNonLethal = false;
    dummy.isReachWeapon = false;
    dummy.rangeIncrement = 0;
    dummy.toHitModifier = 0;
    dummy.typeOfDamage = 0;
    dummy.weaponClass = RPG_ITEM_WEAPONCLASS_INVALID;
#if defined (_MSC_VER)
    return dummy;
#else
    ACE_NOTREACHED(return dummy;)
#endif
  } // end IF

  return iterator->second;
}

void
RPG_Item_Dictionary::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Dictionary::dump"));

  // simply dump the current content of our dictionaries
  for (RPG_Item_WeaponDictionaryIterator_t iterator = myWeaponDictionary.begin();
       iterator != myWeaponDictionary.end();
       iterator++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Item: Weapon\nType: %s\nCategory: %s\nClass: %s\nPrice: %d GP, %d SP\nDamage: %s\ncritical: %d, x%d\nRange: %d\nWeight: %d\nDamage Type: %s\n"),
               RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString((*iterator).first).c_str(),
               RPG_Item_WeaponCategoryHelper::RPG_Item_WeaponCategoryToString((*iterator).second.category).c_str(),
               RPG_Item_WeaponClassHelper::RPG_Item_WeaponClassToString((*iterator).second.weaponClass).c_str(),
               (*iterator).second.baseStorePrice.numGoldPieces,
               (*iterator).second.baseStorePrice.numSilverPieces,
               RPG_Dice_Common_Tools::rollToString((*iterator).second.baseDamage).c_str(),
               (*iterator).second.criticalHit.minToHitRoll,
               (*iterator).second.criticalHit.damageModifier,
               (*iterator).second.rangeIncrement,
               (*iterator).second.baseWeight,
               RPG_Item_Common_Tools::weaponDamageTypeToString((*iterator).second.typeOfDamage).c_str()));

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("==================================\n")));
  } // end FOR

  for (RPG_Item_ArmorDictionaryIterator_t iterator = myArmorDictionary.begin();
       iterator != myArmorDictionary.end();
       iterator++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Item: Armor\nType: %s\nCategory: %s\nPrice: %d GP, %d SP\nAC Bonus: %d\nmax Dex Bonus: %d\nPenalty: %d\nSpell Failure: %d%%\nSpeed: %d\nWeight: %d\n"),
               RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString((*iterator).first).c_str(),
               RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString((*iterator).second.category).c_str(),
               (*iterator).second.baseStorePrice.numGoldPieces,
               (*iterator).second.baseStorePrice.numSilverPieces,
               (*iterator).second.baseBonus,
               (*iterator).second.maxDexterityBonus,
               (*iterator).second.checkPenalty,
               (*iterator).second.arcaneSpellFailure,
               (*iterator).second.baseSpeed,
               (*iterator).second.baseWeight));

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("==================================\n")));
  } // end FOR
}
