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

#ifndef RPG_COMBAT_OTHERDAMAGETYPE_H
#define RPG_COMBAT_OTHERDAMAGETYPE_H

enum RPG_Combat_OtherDamageType
{
  BONUS_HITPOINTS = 0,
  MALUS_NATURALARMOR,
  //
  RPG_COMBAT_OTHERDAMAGETYPE_MAX,
  RPG_COMBAT_OTHERDAMAGETYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_OtherDamageType, std::string> RPG_Combat_OtherDamageTypeToStringTable_t;
typedef RPG_Combat_OtherDamageTypeToStringTable_t::const_iterator RPG_Combat_OtherDamageTypeToStringTableIterator_t;

class RPG_Combat_OtherDamageTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_OtherDamageTypeToStringTable.clear();
    myRPG_Combat_OtherDamageTypeToStringTable.insert(std::make_pair(BONUS_HITPOINTS, ACE_TEXT_ALWAYS_CHAR("BONUS_HITPOINTS")));
    myRPG_Combat_OtherDamageTypeToStringTable.insert(std::make_pair(MALUS_NATURALARMOR, ACE_TEXT_ALWAYS_CHAR("MALUS_NATURALARMOR")));
  };

  inline static std::string RPG_Combat_OtherDamageTypeToString(const RPG_Combat_OtherDamageType& element_in)
  {
    std::string result;
    RPG_Combat_OtherDamageTypeToStringTableIterator_t iterator = myRPG_Combat_OtherDamageTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_OtherDamageTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_OTHERDAMAGETYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_OtherDamageType stringToRPG_Combat_OtherDamageType(const std::string& string_in)
  {
    RPG_Combat_OtherDamageTypeToStringTableIterator_t iterator = myRPG_Combat_OtherDamageTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_OtherDamageTypeToStringTable.end());

    return RPG_COMBAT_OTHERDAMAGETYPE_INVALID;
  };

  static RPG_Combat_OtherDamageTypeToStringTable_t myRPG_Combat_OtherDamageTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_OtherDamageTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_OtherDamageTypeHelper(const RPG_Combat_OtherDamageTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_OtherDamageTypeHelper& operator=(const RPG_Combat_OtherDamageTypeHelper&));
};

#endif
