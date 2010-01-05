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

#ifndef RPG_COMBAT_SPECIALDAMAGETYPE_H
#define RPG_COMBAT_SPECIALDAMAGETYPE_H

enum RPG_Combat_SpecialDamageType
{
  DAMAGE_ACID = 0,
  DAMAGE_COLD,
  DAMAGE_ELECTRICITY,
  DAMAGE_FIRE,
  DAMAGE_SONIC,
  DAMAGE_POISON,
  DAMAGE_STUN,
  DAMAGE_SLAY,
  DAMAGE_ABILITY_REDUCTION,
  DAMAGE_ABILITY_LOSS,
  DAMAGE_ABILITY_DRAIN,
  DAMAGE_LEVEL_LOSS,
  DAMAGE_LEVEL_DRAIN,
  //
  RPG_COMBAT_SPECIALDAMAGETYPE_MAX,
  RPG_COMBAT_SPECIALDAMAGETYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_SpecialDamageType, std::string> RPG_Combat_SpecialDamageTypeToStringTable_t;
typedef RPG_Combat_SpecialDamageTypeToStringTable_t::const_iterator RPG_Combat_SpecialDamageTypeToStringTableIterator_t;

class RPG_Combat_SpecialDamageTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_SpecialDamageTypeToStringTable.clear();
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_ACID, ACE_TEXT_ALWAYS_CHAR("DAMAGE_ACID")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_COLD, ACE_TEXT_ALWAYS_CHAR("DAMAGE_COLD")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_ELECTRICITY, ACE_TEXT_ALWAYS_CHAR("DAMAGE_ELECTRICITY")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_FIRE, ACE_TEXT_ALWAYS_CHAR("DAMAGE_FIRE")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_SONIC, ACE_TEXT_ALWAYS_CHAR("DAMAGE_SONIC")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_POISON, ACE_TEXT_ALWAYS_CHAR("DAMAGE_POISON")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_STUN, ACE_TEXT_ALWAYS_CHAR("DAMAGE_STUN")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_SLAY, ACE_TEXT_ALWAYS_CHAR("DAMAGE_SLAY")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_ABILITY_REDUCTION, ACE_TEXT_ALWAYS_CHAR("DAMAGE_ABILITY_REDUCTION")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_ABILITY_LOSS, ACE_TEXT_ALWAYS_CHAR("DAMAGE_ABILITY_LOSS")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_ABILITY_DRAIN, ACE_TEXT_ALWAYS_CHAR("DAMAGE_ABILITY_DRAIN")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_LEVEL_LOSS, ACE_TEXT_ALWAYS_CHAR("DAMAGE_LEVEL_LOSS")));
    myRPG_Combat_SpecialDamageTypeToStringTable.insert(std::make_pair(DAMAGE_LEVEL_DRAIN, ACE_TEXT_ALWAYS_CHAR("DAMAGE_LEVEL_DRAIN")));
  };

  inline static std::string RPG_Combat_SpecialDamageTypeToString(const RPG_Combat_SpecialDamageType& element_in)
  {
    std::string result;
    RPG_Combat_SpecialDamageTypeToStringTableIterator_t iterator = myRPG_Combat_SpecialDamageTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_SpecialDamageTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_SPECIALDAMAGETYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_SpecialDamageType stringToRPG_Combat_SpecialDamageType(const std::string& string_in)
  {
    RPG_Combat_SpecialDamageTypeToStringTableIterator_t iterator = myRPG_Combat_SpecialDamageTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_SpecialDamageTypeToStringTable.end());

    return RPG_COMBAT_SPECIALDAMAGETYPE_INVALID;
  };

  static RPG_Combat_SpecialDamageTypeToStringTable_t myRPG_Combat_SpecialDamageTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_SpecialDamageTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_SpecialDamageTypeHelper(const RPG_Combat_SpecialDamageTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_SpecialDamageTypeHelper& operator=(const RPG_Combat_SpecialDamageTypeHelper&));
};

#endif
