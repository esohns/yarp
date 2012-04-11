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

#include "rpg_item_common_tools.h"

#include "rpg_item_incl.h"
#include "rpg_item_dictionary.h"

#include <rpg_common_environment_incl.h>
#include <rpg_character_incl.h>
#include <rpg_magic_incl.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
RPG_Item_TypeToStringTable_t RPG_Item_TypeHelper::myRPG_Item_TypeToStringTable;
RPG_Item_CommodityTypeToStringTable_t RPG_Item_CommodityTypeHelper::myRPG_Item_CommodityTypeToStringTable;
RPG_Item_CommodityBeverageToStringTable_t RPG_Item_CommodityBeverageHelper::myRPG_Item_CommodityBeverageToStringTable;
RPG_Item_CommodityLightToStringTable_t RPG_Item_CommodityLightHelper::myRPG_Item_CommodityLightToStringTable;
RPG_Item_MoneyToStringTable_t RPG_Item_MoneyHelper::myRPG_Item_MoneyToStringTable;
RPG_Item_WeaponCategoryToStringTable_t RPG_Item_WeaponCategoryHelper::myRPG_Item_WeaponCategoryToStringTable;
RPG_Item_WeaponClassToStringTable_t RPG_Item_WeaponClassHelper::myRPG_Item_WeaponClassToStringTable;
RPG_Item_WeaponTypeToStringTable_t RPG_Item_WeaponTypeHelper::myRPG_Item_WeaponTypeToStringTable;
RPG_Item_ArmorCategoryToStringTable_t RPG_Item_ArmorCategoryHelper::myRPG_Item_ArmorCategoryToStringTable;
RPG_Item_ArmorTypeToStringTable_t RPG_Item_ArmorTypeHelper::myRPG_Item_ArmorTypeToStringTable;

void
RPG_Item_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::initStringConversionTables"));

  RPG_Item_TypeHelper::init();
  RPG_Item_CommodityTypeHelper::init();
  RPG_Item_CommodityBeverageHelper::init();
  RPG_Item_CommodityLightHelper::init();
  RPG_Item_MoneyHelper::init();
  RPG_Item_WeaponCategoryHelper::init();
  RPG_Item_WeaponClassHelper::init();
  RPG_Item_WeaponTypeHelper::init();
  RPG_Item_ArmorCategoryHelper::init();
  RPG_Item_ArmorTypeHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Item_Common_Tools: initialized string conversion tables...\n")));
}

std::string
RPG_Item_Common_Tools::weaponDamageTypeToString(const RPG_Item_WeaponDamageType& weaponDamageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageTypeToString"));

  std::string result;

  // sanity check
  if (weaponDamageType_in.none())
  {
    return RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString(PHYSICALDAMAGE_NONE);
  } // end IF

  int damageType = PHYSICALDAMAGE_NONE + 1;
  for (unsigned int i = 0;
       i < weaponDamageType_in.size();
       i++, damageType++)
  {
    if (weaponDamageType_in.test(i))
    {
      result += RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString(static_cast<RPG_Common_PhysicalDamageType>(damageType));
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end IF
  } // end FOR

  // crop last character
  std::string::iterator position = result.end();
  position--;
  result.erase(position);

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("weapon damage: \"%s\" --> \"%s\"\n"),
//              weaponDamage_in.to_string().c_str(),
//              result.c_str()));

  return result;
}

RPG_Common_PhysicalDamageList_t
RPG_Item_Common_Tools::weaponDamageTypeToPhysicalDamageType(const RPG_Item_WeaponDamageType& weaponDamageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageTypeToPhysicalDamageType"));

  RPG_Common_PhysicalDamageList_t result;

  // sanity check
  if (weaponDamageType_in.none())
  {
    result.insert(PHYSICALDAMAGE_NONE);

    // finished !
    return result;
  } // end IF

  int damageType = PHYSICALDAMAGE_NONE;
  for (unsigned int i = 0;
       i < weaponDamageType_in.size();
       i++, damageType++)
  {
    if (weaponDamageType_in.test(i))
    {
      result.insert(static_cast<RPG_Common_PhysicalDamageType>(damageType));
    } // end IF
  } // end FOR

  return result;
}

std::string
RPG_Item_Common_Tools::damageToString(const RPG_Item_Damage& damage_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::damageToString"));

  std::string result;

  std::ostringstream converter;
  if (damage_in.typeDice != D_0)
  {
    converter << damage_in.numDice;
    result += converter.str();
    result += RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString(damage_in.typeDice);
  } // end IF

  if (damage_in.modifier == 0)
  {
    return result;
  } // end IF
  else if ((damage_in.modifier > 0) &&
           (damage_in.typeDice != D_0))
  {
    result += ACE_TEXT_ALWAYS_CHAR("+");
  } // end IF
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << damage_in.modifier;
  result += converter.str();

  return result;
}

std::string
RPG_Item_Common_Tools::commoditySubTypeToXMLString(const RPG_Item_CommodityUnion& commoditySubType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::commoditySubTypeToXMLString"));

  std::string result;

  switch (commoditySubType_in.discriminator)
  {
    case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
      result = RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString(commoditySubType_in.commoditybeverage); break;
    case RPG_Item_CommodityUnion::COMMODITYLIGHT:
      result = RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString(commoditySubType_in.commoditylight); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid commodity type (was: %d), aborting\n"),
                 commoditySubType_in.discriminator));

      ACE_ASSERT(false);

      break;
    }
  } // end SWITCH

  return result;
}

bool
RPG_Item_Common_Tools::isShield(const RPG_Item_ArmorType& armorType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isShield"));

  switch (armorType_in)
  {
    case ARMOR_BUCKLER:
    case ARMOR_SHIELD_LIGHT_WOODEN:
    case ARMOR_SHIELD_LIGHT_STEEL:
    case ARMOR_SHIELD_HEAVY_WOODEN:
    case ARMOR_SHIELD_HEAVY_STEEL:
    case ARMOR_SHIELD_TOWER:
    {
      return true;
    }
    default:
    {
      break;
    }
  } // end SWITCH

  return false;
}

unsigned char
RPG_Item_Common_Tools::lightingItem2Radius(const RPG_Item_CommodityLight& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::lightingItem2Radius"));

  switch (type_in)
  {
    case COMMODITY_LIGHT_CANDLE:
      break; // 0
    case COMMODITY_LIGHT_LAMP:
      return 15;
    case COMMODITY_LIGHT_LANTERN_BULLSEYE:
      return 60;
    case COMMODITY_LIGHT_LANTERN_HOODED:
      return 30;
    case COMMODITY_LIGHT_TORCH:
      return 20;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item type (was: \"%s\"), aborting\n"),
                 RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString(type_in).c_str()));

      break;
    }
  } // end SWITCH

  return 0;
}

bool
RPG_Item_Common_Tools::isThrownWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isThrownWeapon"));

  RPG_Item_WeaponProperties weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weaponType_in);

  return (weapon_properties.rangeIncrement && !isProjectileWeapon(weaponType_in));
}

bool
RPG_Item_Common_Tools::isProjectileWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isProjectileWeapon"));

  switch (weaponType_in)
  {
    case RANGED_WEAPON_SLING:
    case RANGED_WEAPON_CROSSBOW_LIGHT:
    case RANGED_WEAPON_CROSSBOW_HEAVY:
    case RANGED_WEAPON_BOW_SHORT:
    case RANGED_WEAPON_BOW_SHORT_COMPOSITE:
    case RANGED_WEAPON_BOW_LONG:
    case RANGED_WEAPON_BOW_LONG_COMPOSITE:
    case RANGED_WEAPON_CROSSBOW_HAND:
    case RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT:
    case RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY:
    {
      return true;
    }
    default:
    {
      break;
    }
  } // end SWITCH

  return false;
}

bool
RPG_Item_Common_Tools::isRangedWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isRangedWeapon"));

  return (isThrownWeapon(weaponType_in) || isProjectileWeapon(weaponType_in));
}

bool
RPG_Item_Common_Tools::isTwoHandedWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isTwoHandedWeapon"));

  switch (weaponType_in)
  {
    case TWO_HANDED_MELEE_WEAPON_LONGSPEAR:
    case TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF:
    case TWO_HANDED_MELEE_WEAPON_SPEAR:
    case TWO_HANDED_MELEE_WEAPON_FALCHION:
    case TWO_HANDED_MELEE_WEAPON_GLAIVE:
    case TWO_HANDED_MELEE_WEAPON_AXE_GREAT:
    case TWO_HANDED_MELEE_WEAPON_CLUB_GREAT:
    case TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY:
    case TWO_HANDED_MELEE_WEAPON_SWORD_GREAT:
    case TWO_HANDED_MELEE_WEAPON_GUISARME:
    case TWO_HANDED_MELEE_WEAPON_HALBERD:
    case TWO_HANDED_MELEE_WEAPON_LANCE:
    case TWO_HANDED_MELEE_WEAPON_RANSEUR:
    case TWO_HANDED_MELEE_WEAPON_SCYTHE:
    case TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE:
    case TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED:
    case TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE:
    case TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED:
    case TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED:
    case TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN:
    {
      return true;
    }
    default:
    {
      // ALL projectile weapons are two-handed...
      if (isProjectileWeapon(weaponType_in))
        return true;

      break;
    }
  } // end SWITCH

  return false;
}

bool
RPG_Item_Common_Tools::isMeleeWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isMeleeWeapon"));

  if (!isRangedWeapon(weaponType_in))
    return true;

  // some thrown weapons CAN be effectively used in melee
  if (!isThrownWeapon(weaponType_in))
    return false;

  switch (weaponType_in)
  {
    case LIGHT_MELEE_WEAPON_DAGGER:
    case ONE_HANDED_MELEE_WEAPON_CLUB:
    case ONE_HANDED_MELEE_WEAPON_SHORTSPEAR:
    case TWO_HANDED_MELEE_WEAPON_SPEAR:
//     case RANGED_WEAPON_JAVELIN:
    case LIGHT_MELEE_WEAPON_AXE_THROWING:
    case LIGHT_MELEE_WEAPON_HAMMER_LIGHT:
    case ONE_HANDED_MELEE_WEAPON_TRIDENT:
    case LIGHT_MELEE_WEAPON_SAI:
    {
      return true;
    }
    default:
    {
      break;
    }
  } // end SWITCH

  return false;
}
