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

#ifndef RPG_ITEM_WEAPONDAMAGETYPE_H
#define RPG_ITEM_WEAPONDAMAGETYPE_H

enum RPG_Item_WeaponDamageType
{
  WEAPONDAMAGE_NONE = 0,
  WEAPONDAMAGE_BLUDGEONING,
  WEAPONDAMAGE_PIERCING,
  WEAPONDAMAGE_SLASHING,
  //
  RPG_ITEM_WEAPONDAMAGETYPE_MAX,
  RPG_ITEM_WEAPONDAMAGETYPE_INVALID
};

#include <map>
#include <string>

typedef std::map<RPG_Item_WeaponDamageType, std::string> RPG_Item_WeaponDamageTypeToStringTable_t;
typedef RPG_Item_WeaponDamageTypeToStringTable_t::const_iterator RPG_Item_WeaponDamageTypeToStringTableIterator_t;

class RPG_Item_WeaponDamageTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_WeaponDamageTypeToStringTable.clear();
    myRPG_Item_WeaponDamageTypeToStringTable.insert(std::make_pair(WEAPONDAMAGE_NONE, ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_NONE")));
    myRPG_Item_WeaponDamageTypeToStringTable.insert(std::make_pair(WEAPONDAMAGE_BLUDGEONING, ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_BLUDGEONING")));
    myRPG_Item_WeaponDamageTypeToStringTable.insert(std::make_pair(WEAPONDAMAGE_PIERCING, ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_PIERCING")));
    myRPG_Item_WeaponDamageTypeToStringTable.insert(std::make_pair(WEAPONDAMAGE_SLASHING, ACE_TEXT_ALWAYS_CHAR("WEAPONDAMAGE_SLASHING")));
  };

  inline static std::string RPG_Item_WeaponDamageTypeToString(const RPG_Item_WeaponDamageType& element_in)
  {
    std::string result;
    RPG_Item_WeaponDamageTypeToStringTableIterator_t iterator = myRPG_Item_WeaponDamageTypeToStringTable.find(element_in);
    if (iterator != myRPG_Item_WeaponDamageTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_WEAPONDAMAGETYPE_INVALID");

    return result;
  };

  inline static RPG_Item_WeaponDamageType stringToRPG_Item_WeaponDamageType(const std::string& string_in)
  {
    RPG_Item_WeaponDamageTypeToStringTableIterator_t iterator = myRPG_Item_WeaponDamageTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_WeaponDamageTypeToStringTable.end());

    return RPG_ITEM_WEAPONDAMAGETYPE_INVALID;
  };

 private:
  static RPG_Item_WeaponDamageTypeToStringTable_t myRPG_Item_WeaponDamageTypeToStringTable;
};

#endif
