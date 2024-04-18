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

#include <sstream>

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

#include "rpg_common_environment_incl.h"
#include "rpg_character_incl.h"
#include "rpg_magic_incl.h"

#include "rpg_item_incl.h"
#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_weapon.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_XML_parser.h"

// initialize statics
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
RPG_Item_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::initializeStringConversionTables"));

  RPG_Item_TypeHelper::init ();
  RPG_Item_CommodityTypeHelper::init ();
  RPG_Item_CommodityBeverageHelper::init ();
  RPG_Item_CommodityLightHelper::init ();
  RPG_Item_MoneyHelper::init ();
  RPG_Item_WeaponCategoryHelper::init ();
  RPG_Item_WeaponClassHelper::init ();
  RPG_Item_WeaponTypeHelper::init ();
  RPG_Item_ArmorCategoryHelper::init ();
  RPG_Item_ArmorTypeHelper::init ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Item_Common_Tools: initialized string conversion tables...\n")));
}

std::string
RPG_Item_Common_Tools::toString (const RPG_Item_WeaponDamageType& weaponDamageType_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::toString"));

  std::string result;

  // sanity check
  if (weaponDamageType_in.none ())
    return RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString (PHYSICALDAMAGE_NONE);

  int damageType = PHYSICALDAMAGE_NONE + 1;
  for (unsigned int i = 0;
       i < weaponDamageType_in.size ();
       i++, damageType++)
    if (weaponDamageType_in.test (i))
    {
      result +=
        RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString (static_cast<RPG_Common_PhysicalDamageType> (damageType));
      result += ACE_TEXT_ALWAYS_CHAR ("|");
    } // end IF

  // crop last character
  std::string::iterator position = result.end ();
  position--;
  result.erase (position);

  return result;
}

RPG_Common_PhysicalDamageList_t
RPG_Item_Common_Tools::weaponDamageTypeToPhysicalDamageType (const RPG_Item_WeaponDamageType& weaponDamageType_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::weaponDamageTypeToPhysicalDamageType"));

  RPG_Common_PhysicalDamageList_t result;

  // sanity check
  if (weaponDamageType_in.none ())
  {
    result.insert (PHYSICALDAMAGE_NONE);
    return result;
  } // end IF

  int damageType = PHYSICALDAMAGE_NONE;
  for (unsigned int i = 0;
       i < weaponDamageType_in.size ();
       i++, damageType++)
    if (weaponDamageType_in.test (i))
      result.insert (static_cast<RPG_Common_PhysicalDamageType> (damageType));

  return result;
}

std::string
RPG_Item_Common_Tools::toString (const RPG_Item_Damage& damage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::toString"));

  std::string result;

  std::ostringstream converter;
  if (damage_in.typeDice != D_0)
  {
    converter << damage_in.numDice;
    result += converter.str ();
    result += RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString (damage_in.typeDice);
  } // end IF

  if (damage_in.modifier == 0)
    return result;
  if ((damage_in.modifier > 0) &&
      (damage_in.typeDice != D_0))
    result += ACE_TEXT_ALWAYS_CHAR ("+");
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << damage_in.modifier;
  result += converter.str ();

  return result;
}

std::string
RPG_Item_Common_Tools::commoditySubTypeToXMLString (const struct RPG_Item_CommodityUnion& commoditySubType_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::commoditySubTypeToXMLString"));

  std::string result;

  switch (commoditySubType_in.discriminator)
  {
    case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
      result = RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString (commoditySubType_in.commoditybeverage); break;
    case RPG_Item_CommodityUnion::COMMODITYLIGHT:
      result = RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString (commoditySubType_in.commoditylight); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid commodity type (was: %d), aborting\n"),
                  commoditySubType_in.discriminator));
      ACE_ASSERT (false);
      break;
    }
  } // end SWITCH

  return result;
}

RPG_Item_CommodityUnion
RPG_Item_Common_Tools::XMLStringToCommoditySubType (const std::string& string_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::XMLStringToCommoditySubType"));

  // *NOTE*: re-use the cxx-parser functionality...
  RPG_Item_CommodityUnion_Type union_type_xml;
  union_type_xml._characters(string_in);
  
  return union_type_xml.post_RPG_Item_CommodityUnion_Type();
}

bool
RPG_Item_Common_Tools::isShield (enum RPG_Item_ArmorType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::isShield"));

  switch (type_in)
  {
    case ARMOR_BUCKLER:
    case ARMOR_SHIELD_LIGHT_WOODEN:
    case ARMOR_SHIELD_LIGHT_STEEL:
    case ARMOR_SHIELD_HEAVY_WOODEN:
    case ARMOR_SHIELD_HEAVY_STEEL:
    case ARMOR_SHIELD_TOWER:
      return true;
    default:
      break;
  } // end SWITCH

  return false;
}

ACE_UINT8
RPG_Item_Common_Tools::lightingItemToRadius (enum RPG_Item_CommodityLight type_in,
                                             bool ambienceIsBright_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::lightingItemToRadius"));

  ACE_UINT8 result = 0;

  switch (type_in)
  {
    case COMMODITY_LIGHT_CANDLE:
      break; // 0
    case COMMODITY_LIGHT_LAMP:
      result = 15; break;
    case COMMODITY_LIGHT_LANTERN_BULLSEYE:
      result = 60; break;
    case COMMODITY_LIGHT_LANTERN_HOODED:
      result = 30; break;
    case COMMODITY_LIGHT_TORCH:
      result = 20; break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid (lighting) item type (was: \"%s\"), aborting\n"),
                  ACE_TEXT (RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString (type_in).c_str ())));
      return 0;
    }
  } // end SWITCH

  // *TODO*: consider ambient lighting as well...
  //return (ambienceIsBright_in ? result : (result * 2));
  return result;
}

void
RPG_Item_Common_Tools::itemToSlot (RPG_Item_ID_t id_in,
                                   enum RPG_Character_OffHand offHand_in,
                                   struct RPG_Character_EquipmentSlots& slots_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::itemToSlot"));

  // init return value(s)
  slots_out.slots.clear ();
  slots_out.is_inclusive = false;

  // retrieve properties
  RPG_Item_Base* item_base = NULL;
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->get (id_in,
                                                              item_base))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid item ID (was: %u), returning\n"),
                id_in));
    return;
  } // end IF
  ACE_ASSERT (item_base);

  switch (item_base->type ())
  {
    case ITEM_ARMOR:
    {
      const RPG_Item_Armor* armor = static_cast<const RPG_Item_Armor*> (item_base);
      enum RPG_Item_ArmorType armor_type_e = armor->armorType_;
      const RPG_Item_ArmorProperties& properties =
        RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties (armor_type_e);
      switch (properties.category)
      {
        case ARMORCATEGORY_GLOVES:
        {
          slots_out.slots.push_back (EQUIPMENTSLOT_HANDS);
          break;
        }
        case ARMORCATEGORY_LIGHT:
        case ARMORCATEGORY_MEDIUM:
        {
          // *TODO*: is this correct ?
          slots_out.slots.push_back (EQUIPMENTSLOT_TORSO);
          break;
        }
        case ARMORCATEGORY_HEAVY:
        {
          slots_out.slots.push_back (EQUIPMENTSLOT_BODY);
          break;
        }
        case ARMORCATEGORY_HELMET:
        {
          slots_out.slots.push_back (EQUIPMENTSLOT_HEAD);
          break;
        }
        case ARMORCATEGORY_SHIELD:
        {
          // *NOTE*: secondary, then primary
          slots_out.slots.push_back ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                                  : EQUIPMENTSLOT_HAND_RIGHT);
          slots_out.slots.push_back ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_RIGHT
                                                                  : EQUIPMENTSLOT_HAND_LEFT);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid armor category (was: \"%s\"), returning\n"),
                      ACE_TEXT (RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString (properties.category).c_str ())));
          return;
        }
      } // end SWITCH

      break;
    }
    case ITEM_COMMODITY:
    {
      const RPG_Item_Commodity* commodity = static_cast<const RPG_Item_Commodity*> (item_base);
      switch (commodity->subtype_.discriminator)
      {
        case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
          break;
        case RPG_Item_CommodityUnion::COMMODITYLIGHT:
        {
          // *NOTE*: secondary, then primary
          slots_out.slots.push_back ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                                  : EQUIPMENTSLOT_HAND_RIGHT);
          slots_out.slots.push_back ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_RIGHT
                                                                  : EQUIPMENTSLOT_HAND_LEFT);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid commodity type (was: %d), returning\n"),
                      commodity->subtype_.discriminator));
          return;
        }
      } // end SWITCH

      break;
    }
    case ITEM_OTHER:
    case ITEM_VALUABLE:
    {
      // *TODO*
      ACE_ASSERT (false);
      break;
    }
    case ITEM_WEAPON:
    {
      const RPG_Item_Weapon* weapon = static_cast<const RPG_Item_Weapon*> (item_base);
      const RPG_Item_WeaponProperties& properties =
        RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (weapon->weaponType_);

      // *TODO*: consider single-handed use of double-handed weapons...
      if (properties.isDoubleWeapon ||
          RPG_Item_Common_Tools::isTwoHandedWeapon (weapon->weaponType_))
        slots_out.is_inclusive = true;
      // *NOTE*: primary, then secondary
      slots_out.slots.push_back ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_RIGHT
                                                              : EQUIPMENTSLOT_HAND_LEFT);
      slots_out.slots.push_back ((offHand_in == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                              : EQUIPMENTSLOT_HAND_RIGHT);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("invalid item type (was: \"%s\"), returning\n"),
                  ACE_TEXT (RPG_Item_TypeHelper::RPG_Item_TypeToString (item_base->type ()).c_str ())));
      break;
    }
  } // end SWITCH
}

bool
RPG_Item_Common_Tools::isThrownWeapon (enum RPG_Item_WeaponType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::isThrownWeapon"));

  RPG_Item_WeaponProperties weapon_properties =
    RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (type_in);

  return (weapon_properties.rangeIncrement &&
          !RPG_Item_Common_Tools::isProjectileWeapon (type_in));
}

bool
RPG_Item_Common_Tools::isProjectileWeapon (enum RPG_Item_WeaponType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::isProjectileWeapon"));

  switch (type_in)
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
      return true;
    default:
      break;
  } // end SWITCH

  return false;
}

bool
RPG_Item_Common_Tools::isRangedWeapon (enum RPG_Item_WeaponType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::isRangedWeapon"));

  return (RPG_Item_Common_Tools::isThrownWeapon (type_in) ||
          RPG_Item_Common_Tools::isProjectileWeapon (type_in));
}

bool
RPG_Item_Common_Tools::isTwoHandedWeapon (enum RPG_Item_WeaponType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::isTwoHandedWeapon"));

  switch (type_in)
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
      return true;
    default:
    {
      // ALL projectile weapons are two-handed...
      if (RPG_Item_Common_Tools::isProjectileWeapon (type_in))
        return true;
      break;
    }
  } // end SWITCH

  return false;
}

bool
RPG_Item_Common_Tools::isMeleeWeapon (enum RPG_Item_WeaponType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::isMeleeWeapon"));

  if (!RPG_Item_Common_Tools::isRangedWeapon (type_in))
    return true;

  // *NOTE*: some thrown weapons CAN be effectively used in melee
  if (!RPG_Item_Common_Tools::isThrownWeapon (type_in))
    return false;

  switch (type_in)
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
      return true;
    default:
      break;
  } // end SWITCH

  return false;
}

bool
RPG_Item_Common_Tools::hasAbsoluteReach (enum RPG_Item_WeaponType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Common_Tools::hasAbsoluteReach"));
 
  // sanity check
  RPG_Item_WeaponProperties properties =
    RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (type_in);
  if (!properties.isReachWeapon)
    return false;

  switch (type_in)
  {
    case TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED:
      return false;
    default:
      break;
  } // end SWITCH

  return true;
}
