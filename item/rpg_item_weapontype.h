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

#ifndef RPG_ITEM_WEAPONTYPE_H
#define RPG_ITEM_WEAPONTYPE_H

enum RPG_Item_WeaponType
{
  UNARMED_WEAPON_GAUNTLET = 0,
  UNARMED_WEAPON_STRIKE,
  LIGHT_MELEE_WEAPON_DAGGER,
  LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED,
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
  LIGHT_MELEE_WEAPON_AXE_THROWING,
  LIGHT_MELEE_WEAPON_HAMMER_LIGHT,
  LIGHT_MELEE_WEAPON_AXE_HAND,
  LIGHT_MELEE_WEAPON_KUKRI,
  LIGHT_MELEE_WEAPON_PICK_LIGHT,
  LIGHT_MELEE_WEAPON_SAP,
  LIGHT_MELEE_WEAPON_SHIELD_LIGHT,
  LIGHT_MELEE_WEAPON_ARMOR_SPIKED,
  LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED,
  LIGHT_MELEE_WEAPON_SWORD_SHORT,
  ONE_HANDED_MELEE_WEAPON_AXE_BATTLE,
  ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT,
  ONE_HANDED_MELEE_WEAPON_SWORD_LONG,
  ONE_HANDED_MELEE_WEAPON_PICK_HEAVY,
  ONE_HANDED_MELEE_WEAPON_RAPIER,
  ONE_HANDED_MELEE_WEAPON_SCIMITAR,
  ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY,
  ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED,
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
  RANGED_WEAPON_SHURIKEN,
  //
  RPG_ITEM_WEAPONTYPE_MAX,
  RPG_ITEM_WEAPONTYPE_INVALID
};

#include <map>
#include <string>

typedef std::map<RPG_Item_WeaponType, std::string> RPG_Item_WeaponTypeToStringTable_t;
typedef RPG_Item_WeaponTypeToStringTable_t::const_iterator RPG_Item_WeaponTypeToStringTableIterator_t;

class RPG_Item_WeaponTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_WeaponTypeToStringTable.clear();
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(UNARMED_WEAPON_GAUNTLET, ACE_TEXT_ALWAYS_CHAR("UNARMED_WEAPON_GAUNTLET")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(UNARMED_WEAPON_STRIKE, ACE_TEXT_ALWAYS_CHAR("UNARMED_WEAPON_STRIKE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_DAGGER, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_DAGGER")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_MACE_LIGHT, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_MACE_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SICKLE, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SICKLE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_CLUB, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_CLUB")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_MACE_HEAVY, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_MACE_HEAVY")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_MORNINGSTAR, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_MORNINGSTAR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_SHORTSPEAR, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SHORTSPEAR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_LONGSPEAR, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_LONGSPEAR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_SPEAR, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SPEAR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_CROSSBOW_LIGHT, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_CROSSBOW_HEAVY, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_HEAVY")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_DART, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_DART")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_JAVELIN, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_JAVELIN")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_SLING, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_SLING")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_AXE_THROWING, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_AXE_THROWING")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_HAMMER_LIGHT, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_HAMMER_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_AXE_HAND, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_AXE_HAND")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_KUKRI, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_KUKRI")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_PICK_LIGHT, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_PICK_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SAP, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SAP")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SHIELD_LIGHT, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SHIELD_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_ARMOR_SPIKED, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_ARMOR_SPIKED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SWORD_SHORT, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SWORD_SHORT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_AXE_BATTLE, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_AXE_BATTLE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_SWORD_LONG, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SWORD_LONG")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_PICK_HEAVY, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_PICK_HEAVY")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_RAPIER, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_RAPIER")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_SCIMITAR, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SCIMITAR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_TRIDENT, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_TRIDENT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_HAMMER_WAR, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_HAMMER_WAR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_FALCHION, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_FALCHION")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_GLAIVE, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_GLAIVE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_AXE_GREAT, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_AXE_GREAT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_CLUB_GREAT, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_CLUB_GREAT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_SWORD_GREAT, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SWORD_GREAT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_GUISARME, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_GUISARME")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_HALBERD, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_HALBERD")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_LANCE, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_LANCE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_RANSEUR, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_RANSEUR")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_SCYTHE, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SCYTHE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_BOW_SHORT, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_SHORT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_BOW_SHORT_COMPOSITE, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_SHORT_COMPOSITE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_BOW_LONG, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_LONG")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_BOW_LONG_COMPOSITE, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOW_LONG_COMPOSITE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_KAMA, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_KAMA")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_NUNCHAKU, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_NUNCHAKU")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SAI, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SAI")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(LIGHT_MELEE_WEAPON_SIANGHAM, ACE_TEXT_ALWAYS_CHAR("LIGHT_MELEE_WEAPON_SIANGHAM")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_SWORD_BASTARD, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_SWORD_BASTARD")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_AXE_WAR_DWARVEN, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_AXE_WAR_DWARVEN")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(ONE_HANDED_MELEE_WEAPON_WHIP, ACE_TEXT_ALWAYS_CHAR("ONE_HANDED_MELEE_WEAPON_WHIP")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN, ACE_TEXT_ALWAYS_CHAR("TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_BOLAS, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_BOLAS")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_CROSSBOW_HAND, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_HAND")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_NET, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_NET")));
    myRPG_Item_WeaponTypeToStringTable.insert(std::make_pair(RANGED_WEAPON_SHURIKEN, ACE_TEXT_ALWAYS_CHAR("RANGED_WEAPON_SHURIKEN")));
  };

  inline static std::string RPG_Item_WeaponTypeToString(const RPG_Item_WeaponType& element_in)
  {
    std::string result;
    RPG_Item_WeaponTypeToStringTableIterator_t iterator = myRPG_Item_WeaponTypeToStringTable.find(element_in);
    if (iterator != myRPG_Item_WeaponTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_WEAPONTYPE_INVALID");

    return result;
  };

  inline static RPG_Item_WeaponType stringToRPG_Item_WeaponType(const std::string& string_in)
  {
    RPG_Item_WeaponTypeToStringTableIterator_t iterator = myRPG_Item_WeaponTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_WeaponTypeToStringTable.end());

    return RPG_ITEM_WEAPONTYPE_INVALID;
  };

 private:
  static RPG_Item_WeaponTypeToStringTable_t myRPG_Item_WeaponTypeToStringTable;
};

#endif
