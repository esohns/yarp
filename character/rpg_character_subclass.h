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

#ifndef RPG_CHARACTER_SUBCLASS_H
#define RPG_CHARACTER_SUBCLASS_H

enum RPG_Character_SubClass
{
  SUBCLASS_NONE = 0,
  SUBCLASS_FIGHTER,
  SUBCLASS_PALADIN,
  SUBCLASS_RANGER,
  SUBCLASS_BARBARIAN,
  SUBCLASS_WARLORD,
  SUBCLASS_WIZARD,
  SUBCLASS_SORCERER,
  SUBCLASS_WARLOCK,
  SUBCLASS_CLERIC,
  SUBCLASS_DRUID,
  SUBCLASS_MONK,
  SUBCLASS_AVENGER,
  SUBCLASS_INVOKER,
  SUBCLASS_SHAMAN,
  SUBCLASS_THIEF,
  SUBCLASS_BARD,
  //
  RPG_CHARACTER_SUBCLASS_MAX,
  RPG_CHARACTER_SUBCLASS_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_SubClass, std::string> RPG_Character_SubClassToStringTable_t;
typedef RPG_Character_SubClassToStringTable_t::const_iterator RPG_Character_SubClassToStringTableIterator_t;

class RPG_Character_SubClassHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_SubClassToStringTable.clear();
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_NONE, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_NONE")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_FIGHTER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_FIGHTER")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_PALADIN, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_PALADIN")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_RANGER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_RANGER")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_BARBARIAN, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_BARBARIAN")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_WARLORD, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_WARLORD")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_WIZARD, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_WIZARD")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_SORCERER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_SORCERER")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_WARLOCK, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_WARLOCK")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_CLERIC, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_CLERIC")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_DRUID, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_DRUID")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_MONK, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_MONK")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_AVENGER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_AVENGER")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_INVOKER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_INVOKER")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_SHAMAN, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_SHAMAN")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_THIEF, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_THIEF")));
    myRPG_Character_SubClassToStringTable.insert(std::make_pair(SUBCLASS_BARD, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_BARD")));
  };

  inline static std::string RPG_Character_SubClassToString(const RPG_Character_SubClass& element_in)
  {
    std::string result;
    RPG_Character_SubClassToStringTableIterator_t iterator = myRPG_Character_SubClassToStringTable.find(element_in);
    if (iterator != myRPG_Character_SubClassToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_SUBCLASS_INVALID");

    return result;
  };

  inline static RPG_Character_SubClass stringToRPG_Character_SubClass(const std::string& string_in)
  {
    RPG_Character_SubClassToStringTableIterator_t iterator = myRPG_Character_SubClassToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_SubClassToStringTable.end());

    return RPG_CHARACTER_SUBCLASS_INVALID;
  };

  static RPG_Character_SubClassToStringTable_t myRPG_Character_SubClassToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SubClassHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SubClassHelper(const RPG_Character_SubClassHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SubClassHelper& operator=(const RPG_Character_SubClassHelper&));
};

#endif
