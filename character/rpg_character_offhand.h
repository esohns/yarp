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

#ifndef RPG_CHARACTER_OFFHAND_H
#define RPG_CHARACTER_OFFHAND_H

enum RPG_Character_OffHand
{
  OFFHAND_NONE = 0,
  OFFHAND_LEFT,
  OFFHAND_RIGHT,
  //
  RPG_CHARACTER_OFFHAND_MAX,
  RPG_CHARACTER_OFFHAND_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_OffHand, std::string> RPG_Character_OffHandToStringTable_t;
typedef RPG_Character_OffHandToStringTable_t::const_iterator RPG_Character_OffHandToStringTableIterator_t;

class RPG_Character_OffHandHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_OffHandToStringTable.clear();
    myRPG_Character_OffHandToStringTable.insert(std::make_pair(OFFHAND_NONE, ACE_TEXT_ALWAYS_CHAR("OFFHAND_NONE")));
    myRPG_Character_OffHandToStringTable.insert(std::make_pair(OFFHAND_LEFT, ACE_TEXT_ALWAYS_CHAR("OFFHAND_LEFT")));
    myRPG_Character_OffHandToStringTable.insert(std::make_pair(OFFHAND_RIGHT, ACE_TEXT_ALWAYS_CHAR("OFFHAND_RIGHT")));
  };

  inline static std::string RPG_Character_OffHandToString(const RPG_Character_OffHand& element_in)
  {
    std::string result;
    RPG_Character_OffHandToStringTableIterator_t iterator = myRPG_Character_OffHandToStringTable.find(element_in);
    if (iterator != myRPG_Character_OffHandToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_OFFHAND_INVALID");

    return result;
  };

  inline static RPG_Character_OffHand stringToRPG_Character_OffHand(const std::string& string_in)
  {
    RPG_Character_OffHandToStringTableIterator_t iterator = myRPG_Character_OffHandToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_OffHandToStringTable.end());

    return RPG_CHARACTER_OFFHAND_INVALID;
  };

  static RPG_Character_OffHandToStringTable_t myRPG_Character_OffHandToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_OffHandHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_OffHandHelper(const RPG_Character_OffHandHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_OffHandHelper& operator=(const RPG_Character_OffHandHelper&));
};

#endif
