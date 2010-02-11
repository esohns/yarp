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

#ifndef RPG_MAGIC_SCHOOL_H
#define RPG_MAGIC_SCHOOL_H

enum RPG_Magic_School
{
  SCHOOL_NONE = 0,
  SCHOOL_ABJURATION,
  SCHOOL_CONJURATION,
  SCHOOL_DIVINATION,
  SCHOOL_ENCHANTMENT,
  SCHOOL_EVOCATION,
  SCHOOL_ILLUSION,
  SCHOOL_NECROMANCY,
  SCHOOL_TRANSMUTATION,
  //
  RPG_MAGIC_SCHOOL_MAX,
  RPG_MAGIC_SCHOOL_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_School, std::string> RPG_Magic_SchoolToStringTable_t;
typedef RPG_Magic_SchoolToStringTable_t::const_iterator RPG_Magic_SchoolToStringTableIterator_t;

class RPG_Magic_SchoolHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_SchoolToStringTable.clear();
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_NONE, ACE_TEXT_ALWAYS_CHAR("SCHOOL_NONE")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_ABJURATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_ABJURATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_CONJURATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_CONJURATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_DIVINATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_DIVINATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_ENCHANTMENT, ACE_TEXT_ALWAYS_CHAR("SCHOOL_ENCHANTMENT")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_EVOCATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_EVOCATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_ILLUSION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_ILLUSION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_NECROMANCY, ACE_TEXT_ALWAYS_CHAR("SCHOOL_NECROMANCY")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_TRANSMUTATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_TRANSMUTATION")));
  };

  inline static std::string RPG_Magic_SchoolToString(const RPG_Magic_School& element_in)
  {
    std::string result;
    RPG_Magic_SchoolToStringTableIterator_t iterator = myRPG_Magic_SchoolToStringTable.find(element_in);
    if (iterator != myRPG_Magic_SchoolToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SCHOOL_INVALID");

    return result;
  };

  inline static RPG_Magic_School stringToRPG_Magic_School(const std::string& string_in)
  {
    RPG_Magic_SchoolToStringTableIterator_t iterator = myRPG_Magic_SchoolToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_SchoolToStringTable.end());

    return RPG_MAGIC_SCHOOL_INVALID;
  };

  static RPG_Magic_SchoolToStringTable_t myRPG_Magic_SchoolToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SchoolHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SchoolHelper(const RPG_Magic_SchoolHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SchoolHelper& operator=(const RPG_Magic_SchoolHelper&));
};

#endif
