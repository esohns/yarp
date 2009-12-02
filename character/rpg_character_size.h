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

#ifndef RPG_CHARACTER_SIZE_H
#define RPG_CHARACTER_SIZE_H

enum RPG_Character_Size
{
  SIZE_FINE = 0,
  SIZE_DIMINUTIVE,
  SIZE_TINY,
  SIZE_SMALL,
  SIZE_MEDIUM,
  SIZE_LARGE,
  SIZE_HUGE,
  SIZE_GARGANTUAN,
  SIZE_COLOSSAL,
  //
  RPG_CHARACTER_SIZE_MAX,
  RPG_CHARACTER_SIZE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Size, std::string> RPG_Character_SizeToStringTable_t;
typedef RPG_Character_SizeToStringTable_t::const_iterator RPG_Character_SizeToStringTableIterator_t;

class RPG_Character_SizeHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_SizeToStringTable.clear();
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_FINE, ACE_TEXT_ALWAYS_CHAR("SIZE_FINE")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_DIMINUTIVE, ACE_TEXT_ALWAYS_CHAR("SIZE_DIMINUTIVE")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_TINY, ACE_TEXT_ALWAYS_CHAR("SIZE_TINY")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_SMALL, ACE_TEXT_ALWAYS_CHAR("SIZE_SMALL")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_MEDIUM, ACE_TEXT_ALWAYS_CHAR("SIZE_MEDIUM")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_LARGE, ACE_TEXT_ALWAYS_CHAR("SIZE_LARGE")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_HUGE, ACE_TEXT_ALWAYS_CHAR("SIZE_HUGE")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_GARGANTUAN, ACE_TEXT_ALWAYS_CHAR("SIZE_GARGANTUAN")));
    myRPG_Character_SizeToStringTable.insert(std::make_pair(SIZE_COLOSSAL, ACE_TEXT_ALWAYS_CHAR("SIZE_COLOSSAL")));
  };

  inline static std::string RPG_Character_SizeToString(const RPG_Character_Size& element_in)
  {
    std::string result;
    RPG_Character_SizeToStringTableIterator_t iterator = myRPG_Character_SizeToStringTable.find(element_in);
    if (iterator != myRPG_Character_SizeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_SIZE_INVALID");

    return result;
  };

  inline static RPG_Character_Size stringToRPG_Character_Size(const std::string& string_in)
  {
    RPG_Character_SizeToStringTableIterator_t iterator = myRPG_Character_SizeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_SizeToStringTable.end());

    return RPG_CHARACTER_SIZE_INVALID;
  };

  static RPG_Character_SizeToStringTable_t myRPG_Character_SizeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SizeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SizeHelper(const RPG_Character_SizeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SizeHelper& operator=(const RPG_Character_SizeHelper&));
};

#endif
