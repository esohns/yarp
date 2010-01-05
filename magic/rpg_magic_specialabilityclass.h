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

#ifndef RPG_MAGIC_SPECIALABILITYCLASS_H
#define RPG_MAGIC_SPECIALABILITYCLASS_H

enum RPG_Magic_SpecialAbilityClass
{
  ABILITYCLASS_EXTRAORDINARY = 0,
  ABILITYCLASS_SPELLLIKE,
  ABILITYCLASS_SUPERNATURAL,
  //
  RPG_MAGIC_SPECIALABILITYCLASS_MAX,
  RPG_MAGIC_SPECIALABILITYCLASS_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_SpecialAbilityClass, std::string> RPG_Magic_SpecialAbilityClassToStringTable_t;
typedef RPG_Magic_SpecialAbilityClassToStringTable_t::const_iterator RPG_Magic_SpecialAbilityClassToStringTableIterator_t;

class RPG_Magic_SpecialAbilityClassHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_SpecialAbilityClassToStringTable.clear();
    myRPG_Magic_SpecialAbilityClassToStringTable.insert(std::make_pair(ABILITYCLASS_EXTRAORDINARY, ACE_TEXT_ALWAYS_CHAR("ABILITYCLASS_EXTRAORDINARY")));
    myRPG_Magic_SpecialAbilityClassToStringTable.insert(std::make_pair(ABILITYCLASS_SPELLLIKE, ACE_TEXT_ALWAYS_CHAR("ABILITYCLASS_SPELLLIKE")));
    myRPG_Magic_SpecialAbilityClassToStringTable.insert(std::make_pair(ABILITYCLASS_SUPERNATURAL, ACE_TEXT_ALWAYS_CHAR("ABILITYCLASS_SUPERNATURAL")));
  };

  inline static std::string RPG_Magic_SpecialAbilityClassToString(const RPG_Magic_SpecialAbilityClass& element_in)
  {
    std::string result;
    RPG_Magic_SpecialAbilityClassToStringTableIterator_t iterator = myRPG_Magic_SpecialAbilityClassToStringTable.find(element_in);
    if (iterator != myRPG_Magic_SpecialAbilityClassToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPECIALABILITYCLASS_INVALID");

    return result;
  };

  inline static RPG_Magic_SpecialAbilityClass stringToRPG_Magic_SpecialAbilityClass(const std::string& string_in)
  {
    RPG_Magic_SpecialAbilityClassToStringTableIterator_t iterator = myRPG_Magic_SpecialAbilityClassToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_SpecialAbilityClassToStringTable.end());

    return RPG_MAGIC_SPECIALABILITYCLASS_INVALID;
  };

  static RPG_Magic_SpecialAbilityClassToStringTable_t myRPG_Magic_SpecialAbilityClassToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpecialAbilityClassHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpecialAbilityClassHelper(const RPG_Magic_SpecialAbilityClassHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpecialAbilityClassHelper& operator=(const RPG_Magic_SpecialAbilityClassHelper&));
};

#endif
