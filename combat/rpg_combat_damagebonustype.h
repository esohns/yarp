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

#ifndef RPG_COMBAT_DAMAGEBONUSTYPE_H
#define RPG_COMBAT_DAMAGEBONUSTYPE_H

enum RPG_Combat_DamageBonusType
{
  BONUS_HITPOINTS = 0,
  MALUS_NATURALARMOR,
  //
  RPG_COMBAT_DAMAGEBONUSTYPE_MAX,
  RPG_COMBAT_DAMAGEBONUSTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_DamageBonusType, std::string> RPG_Combat_DamageBonusTypeToStringTable_t;
typedef RPG_Combat_DamageBonusTypeToStringTable_t::const_iterator RPG_Combat_DamageBonusTypeToStringTableIterator_t;

class RPG_Combat_DamageBonusTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_DamageBonusTypeToStringTable.clear();
    myRPG_Combat_DamageBonusTypeToStringTable.insert(std::make_pair(BONUS_HITPOINTS, ACE_TEXT_ALWAYS_CHAR("BONUS_HITPOINTS")));
    myRPG_Combat_DamageBonusTypeToStringTable.insert(std::make_pair(MALUS_NATURALARMOR, ACE_TEXT_ALWAYS_CHAR("MALUS_NATURALARMOR")));
  };

  inline static std::string RPG_Combat_DamageBonusTypeToString(const RPG_Combat_DamageBonusType& element_in)
  {
    std::string result;
    RPG_Combat_DamageBonusTypeToStringTableIterator_t iterator = myRPG_Combat_DamageBonusTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_DamageBonusTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_DAMAGEBONUSTYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_DamageBonusType stringToRPG_Combat_DamageBonusType(const std::string& string_in)
  {
    RPG_Combat_DamageBonusTypeToStringTableIterator_t iterator = myRPG_Combat_DamageBonusTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_DamageBonusTypeToStringTable.end());

    return RPG_COMBAT_DAMAGEBONUSTYPE_INVALID;
  };

  static RPG_Combat_DamageBonusTypeToStringTable_t myRPG_Combat_DamageBonusTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageBonusTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageBonusTypeHelper(const RPG_Combat_DamageBonusTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageBonusTypeHelper& operator=(const RPG_Combat_DamageBonusTypeHelper&));
};

#endif
