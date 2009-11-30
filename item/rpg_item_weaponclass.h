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

#ifndef RPG_ITEM_WEAPONCLASS_H
#define RPG_ITEM_WEAPONCLASS_H

enum RPG_Item_WeaponClass
{
  WEAPONCLASS_UNARMED = 0,
  WEAPONCLASS_LIGHT_MELEE,
  WEAPONCLASS_ONE_HANDED_MELEE,
  WEAPONCLASS_TWO_HANDED_MELEE,
  WEAPONCLASS_RANGED,
  //
  RPG_ITEM_WEAPONCLASS_MAX,
  RPG_ITEM_WEAPONCLASS_INVALID
};

#include <map>
#include <string>

typedef std::map<RPG_Item_WeaponClass, std::string> RPG_Item_WeaponClassToStringTable_t;
typedef RPG_Item_WeaponClassToStringTable_t::const_iterator RPG_Item_WeaponClassToStringTableIterator_t;

class RPG_Item_WeaponClassHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_WeaponClassToStringTable.clear();
    myRPG_Item_WeaponClassToStringTable.insert(std::make_pair(WEAPONCLASS_UNARMED, ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_UNARMED")));
    myRPG_Item_WeaponClassToStringTable.insert(std::make_pair(WEAPONCLASS_LIGHT_MELEE, ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_LIGHT_MELEE")));
    myRPG_Item_WeaponClassToStringTable.insert(std::make_pair(WEAPONCLASS_ONE_HANDED_MELEE, ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_ONE_HANDED_MELEE")));
    myRPG_Item_WeaponClassToStringTable.insert(std::make_pair(WEAPONCLASS_TWO_HANDED_MELEE, ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_TWO_HANDED_MELEE")));
    myRPG_Item_WeaponClassToStringTable.insert(std::make_pair(WEAPONCLASS_RANGED, ACE_TEXT_ALWAYS_CHAR("WEAPONCLASS_RANGED")));
  };

  inline static std::string RPG_Item_WeaponClassToString(const RPG_Item_WeaponClass& element_in)
  {
    std::string result;
    RPG_Item_WeaponClassToStringTableIterator_t iterator = myRPG_Item_WeaponClassToStringTable.find(element_in);
    if (iterator != myRPG_Item_WeaponClassToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_WEAPONCLASS_INVALID");

    return result;
  };

  inline static RPG_Item_WeaponClass stringToRPG_Item_WeaponClass(const std::string& string_in)
  {
    RPG_Item_WeaponClassToStringTableIterator_t iterator = myRPG_Item_WeaponClassToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_WeaponClassToStringTable.end());

    return RPG_ITEM_WEAPONCLASS_INVALID;
  };

 private:
  static RPG_Item_WeaponClassToStringTable_t myRPG_Item_WeaponClassToStringTable;
};

#endif