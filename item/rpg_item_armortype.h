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

#ifndef RPG_ITEM_ARMORTYPE_H
#define RPG_ITEM_ARMORTYPE_H

enum RPG_Item_ArmorType
{
  ARMOR_PADDED = 0,
  ARMOR_LEATHER,
  ARMOR_LEATHER_STUDDED,
  ARMOR_CHAIN_SHIRT,
  ARMOR_HIDE,
  ARMOR_MAIL_SCALE,
  ARMOR_MAIL_CHAIN,
  ARMOR_PLATE_BREAST,
  ARMOR_MAIL_SPLINT,
  ARMOR_MAIL_BANDED,
  ARMOR_PLATE_HALF,
  ARMOR_PLATE_FULL,
  ARMOR_BUCKLER,
  ARMOR_SHIELD_LIGHT_WOODEN,
  ARMOR_SHIELD_LIGHT_STEEL,
  ARMOR_SHIELD_HEAVY_WOODEN,
  ARMOR_SHIELD_HEAVY_STEEL,
  ARMOR_SHIELD_TOWER,
  //
  RPG_ITEM_ARMORTYPE_MAX,
  RPG_ITEM_ARMORTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Item_ArmorType, std::string> RPG_Item_ArmorTypeToStringTable_t;
typedef RPG_Item_ArmorTypeToStringTable_t::const_iterator RPG_Item_ArmorTypeToStringTableIterator_t;

class RPG_Item_ArmorTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_ArmorTypeToStringTable.clear();
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_PADDED, ACE_TEXT_ALWAYS_CHAR("ARMOR_PADDED")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_LEATHER, ACE_TEXT_ALWAYS_CHAR("ARMOR_LEATHER")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_LEATHER_STUDDED, ACE_TEXT_ALWAYS_CHAR("ARMOR_LEATHER_STUDDED")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_CHAIN_SHIRT, ACE_TEXT_ALWAYS_CHAR("ARMOR_CHAIN_SHIRT")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_HIDE, ACE_TEXT_ALWAYS_CHAR("ARMOR_HIDE")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_MAIL_SCALE, ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_SCALE")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_MAIL_CHAIN, ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_CHAIN")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_PLATE_BREAST, ACE_TEXT_ALWAYS_CHAR("ARMOR_PLATE_BREAST")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_MAIL_SPLINT, ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_SPLINT")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_MAIL_BANDED, ACE_TEXT_ALWAYS_CHAR("ARMOR_MAIL_BANDED")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_PLATE_HALF, ACE_TEXT_ALWAYS_CHAR("ARMOR_PLATE_HALF")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_PLATE_FULL, ACE_TEXT_ALWAYS_CHAR("ARMOR_PLATE_FULL")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_BUCKLER, ACE_TEXT_ALWAYS_CHAR("ARMOR_BUCKLER")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_SHIELD_LIGHT_WOODEN, ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_LIGHT_WOODEN")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_SHIELD_LIGHT_STEEL, ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_LIGHT_STEEL")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_SHIELD_HEAVY_WOODEN, ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_HEAVY_WOODEN")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_SHIELD_HEAVY_STEEL, ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_HEAVY_STEEL")));
    myRPG_Item_ArmorTypeToStringTable.insert(std::make_pair(ARMOR_SHIELD_TOWER, ACE_TEXT_ALWAYS_CHAR("ARMOR_SHIELD_TOWER")));
  };

  inline static std::string RPG_Item_ArmorTypeToString(const RPG_Item_ArmorType& element_in)
  {
    std::string result;
    RPG_Item_ArmorTypeToStringTableIterator_t iterator = myRPG_Item_ArmorTypeToStringTable.find(element_in);
    if (iterator != myRPG_Item_ArmorTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_ARMORTYPE_INVALID");

    return result;
  };

  inline static RPG_Item_ArmorType stringToRPG_Item_ArmorType(const std::string& string_in)
  {
    RPG_Item_ArmorTypeToStringTableIterator_t iterator = myRPG_Item_ArmorTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_ArmorTypeToStringTable.end());

    return RPG_ITEM_ARMORTYPE_INVALID;
  };

  static RPG_Item_ArmorTypeToStringTable_t myRPG_Item_ArmorTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorTypeHelper(const RPG_Item_ArmorTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorTypeHelper& operator=(const RPG_Item_ArmorTypeHelper&));
};

#endif
