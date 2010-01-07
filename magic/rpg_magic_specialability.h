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

#ifndef RPG_MAGIC_SPECIALABILITY_H
#define RPG_MAGIC_SPECIALABILITY_H

enum RPG_Magic_SpecialAbility
{
  SPECIALABILITY_SMITE_GOOD = 0,
  //
  RPG_MAGIC_SPECIALABILITY_MAX,
  RPG_MAGIC_SPECIALABILITY_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_SpecialAbility, std::string> RPG_Magic_SpecialAbilityToStringTable_t;
typedef RPG_Magic_SpecialAbilityToStringTable_t::const_iterator RPG_Magic_SpecialAbilityToStringTableIterator_t;

class RPG_Magic_SpecialAbilityHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_SpecialAbilityToStringTable.clear();
    myRPG_Magic_SpecialAbilityToStringTable.insert(std::make_pair(SPECIALABILITY_SMITE_GOOD, ACE_TEXT_ALWAYS_CHAR("SPECIALABILITY_SMITE_GOOD")));
  };

  inline static std::string RPG_Magic_SpecialAbilityToString(const RPG_Magic_SpecialAbility& element_in)
  {
    std::string result;
    RPG_Magic_SpecialAbilityToStringTableIterator_t iterator = myRPG_Magic_SpecialAbilityToStringTable.find(element_in);
    if (iterator != myRPG_Magic_SpecialAbilityToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPECIALABILITY_INVALID");

    return result;
  };

  inline static RPG_Magic_SpecialAbility stringToRPG_Magic_SpecialAbility(const std::string& string_in)
  {
    RPG_Magic_SpecialAbilityToStringTableIterator_t iterator = myRPG_Magic_SpecialAbilityToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_SpecialAbilityToStringTable.end());

    return RPG_MAGIC_SPECIALABILITY_INVALID;
  };

  static RPG_Magic_SpecialAbilityToStringTable_t myRPG_Magic_SpecialAbilityToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpecialAbilityHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpecialAbilityHelper(const RPG_Magic_SpecialAbilityHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpecialAbilityHelper& operator=(const RPG_Magic_SpecialAbilityHelper&));
};

#endif
