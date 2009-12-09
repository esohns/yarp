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

#ifndef RPG_COMBAT_DAMAGEEFFECTTYPE_H
#define RPG_COMBAT_DAMAGEEFFECTTYPE_H

enum RPG_Combat_DamageEffectType
{
  EFFECT_IMMEDIATE = 0,
  EFFECT_BATTLE,
  EFFECT_TEMPORARY,
  EFFECT_PERMANENT,
  //
  RPG_COMBAT_DAMAGEEFFECTTYPE_MAX,
  RPG_COMBAT_DAMAGEEFFECTTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_DamageEffectType, std::string> RPG_Combat_DamageEffectTypeToStringTable_t;
typedef RPG_Combat_DamageEffectTypeToStringTable_t::const_iterator RPG_Combat_DamageEffectTypeToStringTableIterator_t;

class RPG_Combat_DamageEffectTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_DamageEffectTypeToStringTable.clear();
    myRPG_Combat_DamageEffectTypeToStringTable.insert(std::make_pair(EFFECT_IMMEDIATE, ACE_TEXT_ALWAYS_CHAR("EFFECT_IMMEDIATE")));
    myRPG_Combat_DamageEffectTypeToStringTable.insert(std::make_pair(EFFECT_BATTLE, ACE_TEXT_ALWAYS_CHAR("EFFECT_BATTLE")));
    myRPG_Combat_DamageEffectTypeToStringTable.insert(std::make_pair(EFFECT_TEMPORARY, ACE_TEXT_ALWAYS_CHAR("EFFECT_TEMPORARY")));
    myRPG_Combat_DamageEffectTypeToStringTable.insert(std::make_pair(EFFECT_PERMANENT, ACE_TEXT_ALWAYS_CHAR("EFFECT_PERMANENT")));
  };

  inline static std::string RPG_Combat_DamageEffectTypeToString(const RPG_Combat_DamageEffectType& element_in)
  {
    std::string result;
    RPG_Combat_DamageEffectTypeToStringTableIterator_t iterator = myRPG_Combat_DamageEffectTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_DamageEffectTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_DAMAGEEFFECTTYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_DamageEffectType stringToRPG_Combat_DamageEffectType(const std::string& string_in)
  {
    RPG_Combat_DamageEffectTypeToStringTableIterator_t iterator = myRPG_Combat_DamageEffectTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_DamageEffectTypeToStringTable.end());

    return RPG_COMBAT_DAMAGEEFFECTTYPE_INVALID;
  };

  static RPG_Combat_DamageEffectTypeToStringTable_t myRPG_Combat_DamageEffectTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageEffectTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageEffectTypeHelper(const RPG_Combat_DamageEffectTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageEffectTypeHelper& operator=(const RPG_Combat_DamageEffectTypeHelper&));
};

#endif
