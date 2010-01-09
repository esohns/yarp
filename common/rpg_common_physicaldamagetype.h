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

#ifndef RPG_COMMON_PHYSICALDAMAGETYPE_H
#define RPG_COMMON_PHYSICALDAMAGETYPE_H

enum RPG_Common_PhysicalDamageType
{
  PHYSICALDAMAGE_NONE = 0,
  PHYSICALDAMAGE_BLUDGEONING,
  PHYSICALDAMAGE_PIERCING,
  PHYSICALDAMAGE_SLASHING,
  PHYSICALDAMAGE_CONSTRICT,
  //
  RPG_COMMON_PHYSICALDAMAGETYPE_MAX,
  RPG_COMMON_PHYSICALDAMAGETYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_PhysicalDamageType, std::string> RPG_Common_PhysicalDamageTypeToStringTable_t;
typedef RPG_Common_PhysicalDamageTypeToStringTable_t::const_iterator RPG_Common_PhysicalDamageTypeToStringTableIterator_t;

class RPG_Common_PhysicalDamageTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_PhysicalDamageTypeToStringTable.clear();
    myRPG_Common_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_NONE, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_NONE")));
    myRPG_Common_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_BLUDGEONING, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_BLUDGEONING")));
    myRPG_Common_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_PIERCING, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_PIERCING")));
    myRPG_Common_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_SLASHING, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_SLASHING")));
    myRPG_Common_PhysicalDamageTypeToStringTable.insert(std::make_pair(PHYSICALDAMAGE_CONSTRICT, ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_CONSTRICT")));
  };

  inline static std::string RPG_Common_PhysicalDamageTypeToString(const RPG_Common_PhysicalDamageType& element_in)
  {
    std::string result;
    RPG_Common_PhysicalDamageTypeToStringTableIterator_t iterator = myRPG_Common_PhysicalDamageTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_PhysicalDamageTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_PHYSICALDAMAGETYPE_INVALID");

    return result;
  };

  inline static RPG_Common_PhysicalDamageType stringToRPG_Common_PhysicalDamageType(const std::string& string_in)
  {
    RPG_Common_PhysicalDamageTypeToStringTableIterator_t iterator = myRPG_Common_PhysicalDamageTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_PhysicalDamageTypeToStringTable.end());

    return RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;
  };

  static RPG_Common_PhysicalDamageTypeToStringTable_t myRPG_Common_PhysicalDamageTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_PhysicalDamageTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_PhysicalDamageTypeHelper(const RPG_Common_PhysicalDamageTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_PhysicalDamageTypeHelper& operator=(const RPG_Common_PhysicalDamageTypeHelper&));
};

#endif
