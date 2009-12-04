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

#ifndef RPG_ITEM_PHYSICALDAMAGETYPE_H
#define RPG_ITEM_PHYSICALDAMAGETYPE_H

enum RPG_Item_PhysicalDamageType
{
  PHYSICALDAMAGE_NONE = 0,
  PHYSICALDAMAGE_BLUDGEONING,
  PHYSICALDAMAGE_PIERCING,
  PHYSICALDAMAGE_SLASHING,
  //
  RPG_ITEM_PHYSICALDAMAGETYPE_MAX,
  RPG_ITEM_PHYSICALDAMAGETYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Item_PhysicalDamageType, std::string> RPG_Item_PhysicalDamageTypeToStringTable_t;
typedef RPG_Item_PhysicalDamageTypeToStringTable_t::const_iterator RPG_Item_PhysicalDamageTypeToStringTableIterator_t;

class RPG_Item_PhysicalDamageTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_PhysicalDamageTypeToStringTable.clear();
    myRPG_Item_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_NONE, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_NONE")));
    myRPG_Item_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_BLUDGEONING, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_BLUDGEONING")));
    myRPG_Item_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_PIERCING, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_PIERCING")));
    myRPG_Item_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_SLASHING, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_SLASHING")));
  };

  inline static std::string RPG_Item_PhysicalDamageTypeToString(const RPG_Item_PhysicalDamageType& element_in)
  {
    std::string result;
    RPG_Item_PhysicalDamageTypeToStringTableIterator_t iterator = myRPG_Item_PhysicalDamageTypeToStringTable.find(element_in);
    if (iterator != myRPG_Item_PhysicalDamageTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_PHYSICALDAMAGETYPE_INVALID");

    return result;
  };

  inline static RPG_Item_PhysicalDamageType stringToRPG_Item_PhysicalDamageType(const std::string& string_in)
  {
    RPG_Item_PhysicalDamageTypeToStringTableIterator_t iterator = myRPG_Item_PhysicalDamageTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_PhysicalDamageTypeToStringTable.end());

    return RPG_ITEM_PHYSICALDAMAGETYPE_INVALID;
  };

  static RPG_Item_PhysicalDamageTypeToStringTable_t myRPG_Item_PhysicalDamageTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_PhysicalDamageTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_PhysicalDamageTypeHelper(const RPG_Item_PhysicalDamageTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_PhysicalDamageTypeHelper& operator=(const RPG_Item_PhysicalDamageTypeHelper&));
};

#endif
