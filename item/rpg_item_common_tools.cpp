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

#include <rpg_chance_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
RPG_Item_Common_Tools::RPG_String2ItemTypeTable_t         RPG_Item_Common_Tools::myString2ItemTypeTable;
RPG_Item_Common_Tools::RPG_String2MoneyTypeTable_t        RPG_Item_Common_Tools::myString2MoneyTypeTable;
RPG_Item_Common_Tools::RPG_String2WeaponCategoryTable_t   RPG_Item_Common_Tools::myString2WeaponCategoryTable;
RPG_Item_Common_Tools::RPG_String2WeaponClassTable_t      RPG_Item_Common_Tools::myString2WeaponClassTable;
RPG_Item_Common_Tools::RPG_String2WeaponTypeTable_t       RPG_Item_Common_Tools::myString2WeaponTypeTable;
RPG_Item_Common_Tools::RPG_String2WeaponDamageTypeTable_t RPG_Item_Common_Tools::myString2WeaponDamageTypeTable;
RPG_Item_Common_Tools::RPG_String2ArmorCategoryTable_t    RPG_Item_Common_Tools::myString2ArmorCategoryTable;
RPG_Item_Common_Tools::RPG_String2ArmorTypeTable_t        RPG_Item_Common_Tools::myString2ArmorTypeTable;

void RPG_Item_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::initStringConversionTables"));

  // clean tables
  myString2ItemTypeTable.clear();
  myString2MoneyTypeTable.clear();
  myString2WeaponCategoryTable.clear();
  myString2WeaponClassTable.clear();
  myString2WeaponTypeTable.clear();
  myString2WeaponDamageTypeTable.clear();
  myString2ArmorCategoryTable.clear();
  myString2ArmorTypeTable.clear();

  // RPG_Item_Type
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Armor"), ITEM_ARMOR));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Goods"), ITEM_GOODS));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Other"), ITEM_OTHER));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Valuable"), ITEM_VALUABLE));
  myString2ItemTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Weapon"), ITEM_WEAPON));

  // RPG_Item_Wealth_Type
  myString2MoneyTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Copper"), MONEY_COIN_COPPER));
  myString2MoneyTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Silver"), MONEY_COIN_SILVER));
  myString2MoneyTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Gold"), MONEY_COIN_GOLD));
  myString2MoneyTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Platinum"), MONEY_COIN_PLATINUM));
  myString2MoneyTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Gem"), MONEY_GEM));
  myString2MoneyTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("Precious"), MONEY_PRECIOUS));

  // RPG_Item_Weapon_Category
  myString2WeaponCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_SIMPLE"), WEAPONCATEGORY_SIMPLE));
  myString2WeaponCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_MARTIAL"), WEAPONCATEGORY_MARTIAL));
  myString2WeaponCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_EXOTIC"), WEAPONCATEGORY_EXOTIC));

  // RPG_Item_Weapon_Class
  myString2WeaponClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_UNARMED"), WEAPONCLASS_UNARMED));
  myString2WeaponClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_LIGHT_MELEE"), WEAPONCLASS_LIGHT_MELEE));
  myString2WeaponClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_ONE_HANDED_MELEE"), WEAPONCLASS_ONE_HANDED_MELEE));
  myString2WeaponClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_TWO_HANDED_MELEE"), WEAPONCLASS_TWO_HANDED_MELEE));
  myString2WeaponClassTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_RANGED"), WEAPONCLASS_RANGED));

  // RPG_Item_Weapon_Type
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("UNARMED_WEAPON_GAUNTLET"), UNARMED_WEAPON_GAUNTLET));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("UNARMED_WEAPON_STRIKE"), UNARMED_WEAPON_STRIKE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_DAGGER"), LIGHT_MELEE_WEAPON_DAGGER));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED"), LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_MACE_LIGHT"), LIGHT_MELEE_WEAPON_MACE_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SICKLE"), LIGHT_MELEE_WEAPON_SICKLE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_CLUB"), ONE_HANDED_MELEE_WEAPON_CLUB));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_MACE_HEAVY"), ONE_HANDED_MELEE_WEAPON_MACE_HEAVY));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_MORNINGSTAR"), ONE_HANDED_MELEE_WEAPON_MORNINGSTAR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SHORTSPEAR"), ONE_HANDED_MELEE_WEAPON_SHORTSPEAR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_LONGSPEAR"), TWO_HANDED_MELEE_WEAPON_LONGSPEAR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF"), TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SPEAR"), TWO_HANDED_MELEE_WEAPON_SPEAR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_LIGHT"), RANGED_WEAPON_CROSSBOW_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_HEAVY"), RANGED_WEAPON_CROSSBOW_HEAVY));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_DART"), RANGED_WEAPON_DART));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_JAVELIN"), RANGED_WEAPON_JAVELIN));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_SLING"), RANGED_WEAPON_SLING));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_AXE_THROWING"), LIGHT_MELEE_WEAPON_AXE_THROWING));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_HAMMER_LIGHT"), LIGHT_MELEE_WEAPON_HAMMER_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_AXE_HAND"), LIGHT_MELEE_WEAPON_AXE_HAND));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_KUKRI"), LIGHT_MELEE_WEAPON_KUKRI));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_PICK_LIGHT"), LIGHT_MELEE_WEAPON_PICK_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SAP"), LIGHT_MELEE_WEAPON_SAP));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SHIELD_LIGHT"), LIGHT_MELEE_WEAPON_SHIELD_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_ARMOR_SPIKED"), LIGHT_MELEE_WEAPON_ARMOR_SPIKED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED"), LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SWORD_SHORT"), LIGHT_MELEE_WEAPON_SWORD_SHORT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_AXE_BATTLE"), ONE_HANDED_MELEE_WEAPON_AXE_BATTLE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT"), ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SWORD_LONG"), ONE_HANDED_MELEE_WEAPON_SWORD_LONG));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_PICK_HEAVY"), ONE_HANDED_MELEE_WEAPON_PICK_HEAVY));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_RAPIER"), ONE_HANDED_MELEE_WEAPON_RAPIER));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SCIMITAR"), ONE_HANDED_MELEE_WEAPON_SCIMITAR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY"), ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED"), ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_TRIDENT"), ONE_HANDED_MELEE_WEAPON_TRIDENT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_HAMMER_WAR"), ONE_HANDED_MELEE_WEAPON_HAMMER_WAR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_FALCHION"), TWO_HANDED_MELEE_WEAPON_FALCHION));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_GLAIVE"), TWO_HANDED_MELEE_WEAPON_GLAIVE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_AXE_GREAT"), TWO_HANDED_MELEE_WEAPON_AXE_GREAT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_CLUB_GREAT"), TWO_HANDED_MELEE_WEAPON_CLUB_GREAT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY"), TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SWORD_GREAT"), TWO_HANDED_MELEE_WEAPON_SWORD_GREAT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_GUISARME"), TWO_HANDED_MELEE_WEAPON_GUISARME));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_HALBERD"), TWO_HANDED_MELEE_WEAPON_HALBERD));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_LANCE"), TWO_HANDED_MELEE_WEAPON_LANCE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_RANSEUR"), TWO_HANDED_MELEE_WEAPON_RANSEUR));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SCYTHE"), TWO_HANDED_MELEE_WEAPON_SCYTHE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_SHORT"), RANGED_WEAPON_BOW_SHORT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_SHORT_COMPOSITE"), RANGED_WEAPON_BOW_SHORT_COMPOSITE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_LONG"), RANGED_WEAPON_BOW_LONG));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_LONG_COMPOSITE"), RANGED_WEAPON_BOW_LONG_COMPOSITE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_KAMA"), LIGHT_MELEE_WEAPON_KAMA));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_NUNCHAKU"), LIGHT_MELEE_WEAPON_NUNCHAKU));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SAI"), LIGHT_MELEE_WEAPON_SAI));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SIANGHAM"), LIGHT_MELEE_WEAPON_SIANGHAM));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SWORD_BASTARD"), ONE_HANDED_MELEE_WEAPON_SWORD_BASTARD));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_AXE_WAR_DWARVEN"), ONE_HANDED_MELEE_WEAPON_AXE_WAR_DWARVEN));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_WHIP"), ONE_HANDED_MELEE_WEAPON_WHIP));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE"), TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED"), TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE"), TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED"), TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED"), TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN"), TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOLAS"), RANGED_WEAPON_BOLAS));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_HAND"), RANGED_WEAPON_CROSSBOW_HAND));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT"), RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY"), RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_NET"), RANGED_WEAPON_NET));
  myString2WeaponTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_SHURIKEN"), RANGED_WEAPON_SHURIKEN));

  // RPG_Item_Weapon_Damage_Type
  myString2WeaponDamageTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_NONE"), WEAPONDAMAGE_NONE));
  myString2WeaponDamageTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_BLUDGEONING"), WEAPONDAMAGE_BLUDGEONING));
  myString2WeaponDamageTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_PIERCING"), WEAPONDAMAGE_PIERCING));
  myString2WeaponDamageTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_SLASHING"), WEAPONDAMAGE_SLASHING));

  // RPG_Item_Armor_Category
  myString2ArmorCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_LIGHT"), ARMORCATEGORY_LIGHT));
  myString2ArmorCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_MEDIUM"), ARMORCATEGORY_MEDIUM));
  myString2ArmorCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_HEAVY"), ARMORCATEGORY_HEAVY));
  myString2ArmorCategoryTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_SHIELD"), ARMORCATEGORY_SHIELD));

  // RPG_Item_Armor_Type
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_PADDED"), ARMOR_PADDED));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_LEATHER"), ARMOR_LEATHER));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_LEATHER_STUDDED"), ARMOR_LEATHER_STUDDED));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_CHAIN_SHIRT"), ARMOR_CHAIN_SHIRT));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_HIDE"), ARMOR_HIDE));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_SCALE"), ARMOR_MAIL_SCALE));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_CHAIN"), ARMOR_MAIL_CHAIN));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_PLATE_BREAST"), ARMOR_PLATE_BREAST));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_SPLINT"), ARMOR_MAIL_SPLINT));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_BANDED"), ARMOR_MAIL_BANDED));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_PLATE_HALF"), ARMOR_PLATE_HALF));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_PLATE_FULL"), ARMOR_PLATE_FULL));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_BUCKLER"), ARMOR_BUCKLER));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_LIGHT_WOODEN"), ARMOR_SHIELD_LIGHT_WOODEN));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_LIGHT_STEEL"), ARMOR_SHIELD_LIGHT_STEEL));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_HEAVY_WOODEN"), ARMOR_SHIELD_HEAVY_WOODEN));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_HEAVY_STEEL"), ARMOR_SHIELD_HEAVY_STEEL));
  myString2ArmorTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_TOWER"), ARMOR_SHIELD_TOWER));
}

const std::string RPG_Item_Common_Tools::itemTypeToString(const RPG_Item_Type& itemType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::itemTypeToString"));

  RPG_String2ItemTypeTableIterator_t iterator = myString2ItemTypeTable.begin();
  do
  {
    if (iterator->second == itemType_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2ItemTypeTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid item type: %d --> check implementation !, aborting\n"),
             itemType_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("ITEM_TYPE_INVALID"));
}

const RPG_Item_WeaponCategory RPG_Item_Common_Tools::stringToWeaponCategory(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToWeaponCategory"));

  RPG_String2WeaponCategoryTableIterator_t iterator = myString2WeaponCategoryTable.find(string_in);
  if (iterator == myString2WeaponCategoryTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid weapon category: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return WEAPONCATEGORY_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Item_Common_Tools::weaponCategoryToString(const RPG_Item_WeaponCategory& weaponCategory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponCategoryToString"));

  RPG_String2WeaponCategoryTableIterator_t iterator = myString2WeaponCategoryTable.begin();
  do
  {
    if (iterator->second == weaponCategory_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2WeaponCategoryTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid weapon category: %d --> check implementation !, aborting\n"),
             weaponCategory_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_INVALID"));
}

const RPG_Item_WeaponClass RPG_Item_Common_Tools::stringToWeaponClass(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToWeaponClass"));

  RPG_String2WeaponClassTableIterator_t iterator = myString2WeaponClassTable.find(string_in);
  if (iterator == myString2WeaponClassTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid weapon class: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return WEAPONCLASS_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Item_Common_Tools::weaponClassToString(const RPG_Item_WeaponClass& weaponClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponClassToString"));

  RPG_String2WeaponClassTableIterator_t iterator = myString2WeaponClassTable.begin();
  do
  {
    if (iterator->second == weaponClass_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2WeaponClassTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid weapon class: %d --> check implementation !, aborting\n"),
             weaponClass_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_INVALID"));
}

const RPG_Item_WeaponType RPG_Item_Common_Tools::stringToWeaponType(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToWeaponType"));

  RPG_String2WeaponTypeTableIterator_t iterator = myString2WeaponTypeTable.find(string_in);
  if (iterator == myString2WeaponTypeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid weapon type: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return WEAPON_TYPE_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Item_Common_Tools::weaponTypeToString(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponTypeToString"));

  RPG_String2WeaponTypeTableIterator_t iterator = myString2WeaponTypeTable.begin();
  do
  {
    if (iterator->second == weaponType_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2WeaponTypeTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid weapon type: %d --> check implementation !, aborting\n"),
             weaponType_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("WEAPON_TYPE_INVALID"));
}

const RPG_Item_WeaponDamageType RPG_Item_Common_Tools::stringToWeaponDamageType(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToWeaponDamageType"));

  RPG_String2WeaponDamageTypeTableIterator_t iterator = myString2WeaponDamageTypeTable.find(string_in);
  if (iterator == myString2WeaponDamageTypeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid weapon damage type: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return WEAPONDAMAGE_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Item_Common_Tools::weaponDamageTypeToString(const RPG_Item_WeaponDamageType& weaponDamageType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageTypeToString"));

  RPG_String2WeaponDamageTypeTableIterator_t iterator = myString2WeaponDamageTypeTable.begin();
  do
  {
    if (iterator->second == weaponDamageType_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2WeaponDamageTypeTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid weapon damage type: %d --> check implementation !, aborting\n"),
             weaponDamageType_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_INVALID"));
}

const std::string RPG_Item_Common_Tools::weaponDamageToString(const RPG_Item_WeaponDamage& weaponDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageToString"));

  std::string result;

  // sanity check
  if (weaponDamage_in.none())
  {
    return RPG_Item_Common_Tools::weaponDamageTypeToString(WEAPONDAMAGE_NONE);
  } // end IF

  for (int i = 0;
       i < weaponDamage_in.size();
       i++)
  {
    if (weaponDamage_in.test(i))
    {
      // *TODO*: this is a nasty hack !
      RPG_String2WeaponDamageTypeTableIterator_t iterator = myString2WeaponDamageTypeTable.begin();
      do
      {
        if (iterator->second == ACE_static_cast(RPG_Item_WeaponDamageType, (1 << i)))
        {
          // done
          break;
        } // end IF

        iterator++;
      } while (iterator != myString2WeaponDamageTypeTable.end());

      result += iterator->first;
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

const std::string RPG_Item_Common_Tools::damageToString(const RPG_Item_Damage& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::damageToString"));

  std::string result;
  std::stringstream str;

  if (damage_in.typeDice != D_0)
  {
    str << damage_in.numDice;
    result += str.str();
    result += RPG_Chance_Dice_Common_Tools::diceType2String(damage_in.typeDice);
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

const RPG_Item_ArmorCategory RPG_Item_Common_Tools::stringToArmorCategory(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToArmorCategory"));

  RPG_String2ArmorCategoryTableIterator_t iterator = myString2ArmorCategoryTable.find(string_in);
  if (iterator == myString2ArmorCategoryTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid armor category: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ARMORCATEGORY_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Item_Common_Tools::armorCategoryToString(const RPG_Item_ArmorCategory& armorCategory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::armorCategoryToString"));

  RPG_String2ArmorCategoryTableIterator_t iterator = myString2ArmorCategoryTable.begin();
  do
  {
    if (iterator->second == armorCategory_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2ArmorCategoryTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid armor category: %d --> check implementation !, aborting\n"),
             armorCategory_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_INVALID"));
}

const RPG_Item_ArmorType RPG_Item_Common_Tools::stringToArmorType(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::stringToArmorType"));

  RPG_String2ArmorTypeTableIterator_t iterator = myString2ArmorTypeTable.find(string_in);
  if (iterator == myString2ArmorTypeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid armor type: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ARMOR_TYPE_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Item_Common_Tools::armorTypeToString(const RPG_Item_ArmorType& armorType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::armorTypeToString"));

  RPG_String2ArmorTypeTableIterator_t iterator = myString2ArmorTypeTable.begin();
  do
  {
    if (iterator->second == armorType_in)
    {
      // done
      return iterator->first;
    } // end IF

    iterator++;
  } while (iterator != myString2ArmorTypeTable.end());

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid armor type: %d --> check implementation !, aborting\n"),
             armorType_in));

  return std::string(ACE_TEXT_ALWAYS_CHAR("ARMOR_TYPE_INVALID"));
}
