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

#ifndef RPG_COMMON_SUBCLASS_H
#define RPG_COMMON_SUBCLASS_H

enum RPG_Common_SubClass
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
  RPG_COMMON_SUBCLASS_MAX,
  RPG_COMMON_SUBCLASS_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_SubClass, std::string> RPG_Common_SubClassToStringTable_t;
typedef RPG_Common_SubClassToStringTable_t::const_iterator RPG_Common_SubClassToStringTableIterator_t;

class RPG_Common_SubClassHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_SubClassToStringTable.clear();
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_NONE, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_NONE")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_FIGHTER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_FIGHTER")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_PALADIN, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_PALADIN")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_RANGER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_RANGER")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_BARBARIAN, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_BARBARIAN")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_WARLORD, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_WARLORD")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_WIZARD, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_WIZARD")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_SORCERER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_SORCERER")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_WARLOCK, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_WARLOCK")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_CLERIC, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_CLERIC")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_DRUID, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_DRUID")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_MONK, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_MONK")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_AVENGER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_AVENGER")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_INVOKER, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_INVOKER")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_SHAMAN, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_SHAMAN")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_THIEF, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_THIEF")));
    myRPG_Common_SubClassToStringTable.insert(std::make_pair(SUBCLASS_BARD, ACE_TEXT_ALWAYS_CHAR("SUBCLASS_BARD")));
  };

  inline static std::string RPG_Common_SubClassToString(const RPG_Common_SubClass& element_in)
  {
    std::string result;
    RPG_Common_SubClassToStringTableIterator_t iterator = myRPG_Common_SubClassToStringTable.find(element_in);
    if (iterator != myRPG_Common_SubClassToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_SUBCLASS_INVALID");

    return result;
  };

  inline static RPG_Common_SubClass stringToRPG_Common_SubClass(const std::string& string_in)
  {
    RPG_Common_SubClassToStringTableIterator_t iterator = myRPG_Common_SubClassToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_SubClassToStringTable.end());

    return RPG_COMMON_SUBCLASS_INVALID;
  };

  static RPG_Common_SubClassToStringTable_t myRPG_Common_SubClassToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SubClassHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SubClassHelper(const RPG_Common_SubClassHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SubClassHelper& operator=(const RPG_Common_SubClassHelper&));
};

#endif