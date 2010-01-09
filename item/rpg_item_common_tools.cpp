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
#include "rpg_item_common_tools.h"

#include <rpg_magic_incl.h>

#include "rpg_item_incl.h"
#include "rpg_item_dictionary.h"

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
RPG_Item_TypeToStringTable_t RPG_Item_TypeHelper::myRPG_Item_TypeToStringTable;
RPG_Item_MoneyToStringTable_t RPG_Item_MoneyHelper::myRPG_Item_MoneyToStringTable;
RPG_Item_WeaponCategoryToStringTable_t RPG_Item_WeaponCategoryHelper::myRPG_Item_WeaponCategoryToStringTable;
RPG_Item_WeaponClassToStringTable_t RPG_Item_WeaponClassHelper::myRPG_Item_WeaponClassToStringTable;
RPG_Item_WeaponTypeToStringTable_t RPG_Item_WeaponTypeHelper::myRPG_Item_WeaponTypeToStringTable;
RPG_Item_ArmorCategoryToStringTable_t RPG_Item_ArmorCategoryHelper::myRPG_Item_ArmorCategoryToStringTable;
RPG_Item_ArmorTypeToStringTable_t RPG_Item_ArmorTypeHelper::myRPG_Item_ArmorTypeToStringTable;

void RPG_Item_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::initStringConversionTables"));

  RPG_Item_TypeHelper::init();
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

const std::string RPG_Item_Common_Tools::weaponDamageToString(const RPG_Item_WeaponDamage& weaponDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageToString"));

  std::string result;

  // sanity check
  if (weaponDamage_in.none())
  {
    return RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString(PHYSICALDAMAGE_NONE);
  } // end IF

  int damageType = PHYSICALDAMAGE_NONE + 1;
  for (int i = 0;
       i < weaponDamage_in.size();
       i++, damageType++)
  {
    if (weaponDamage_in.test(i))
    {
      result += RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString(ACE_static_cast(RPG_Common_PhysicalDamageType, damageType));
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

const RPG_Item_WeaponDamageList_t RPG_Item_Common_Tools::weaponDamageToPhysicalDamageType(const RPG_Item_WeaponDamage& weaponDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageToPhysicalDamageType"));

  RPG_Item_WeaponDamageList_t result;

  // sanity check
  if (weaponDamage_in.none())
  {
    result.insert(PHYSICALDAMAGE_NONE);

    // finished !
    return result;
  } // end IF

  int damageType = PHYSICALDAMAGE_NONE;
  for (int i = 0;
       i < weaponDamage_in.size();
       i++, damageType++)
  {
    if (weaponDamage_in.test(i))
    {
      result.insert(ACE_static_cast(RPG_Common_PhysicalDamageType, damageType));
    } // end IF
  } // end FOR

  return result;
}

const std::string RPG_Item_Common_Tools::damageToString(const RPG_Item_Damage& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::damageToString"));

  std::string result;
  std::stringstream str;

  if (damage_in.typeDice != D_0)
  {
    str << damage_in.numDice;
    result += str.str();
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
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  str << damage_in.modifier;
  result += str.str();

  return result;
}

const bool RPG_Item_Common_Tools::isShield(const RPG_Item_ArmorType& armorType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isShield"));

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

const bool RPG_Item_Common_Tools::isThrownWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isThrownWeapon"));

  RPG_Item_WeaponProperties weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weaponType_in);

  return (weapon_properties.rangeIncrement && !isProjectileWeapon(weaponType_in));
}

const bool RPG_Item_Common_Tools::isProjectileWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isProjectileWeapon"));

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

const bool RPG_Item_Common_Tools::isTwoHandedWeapon(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::isTwoHandedWeapon"));

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
      break;
    }
  } // end SWITCH

  return false;
}
