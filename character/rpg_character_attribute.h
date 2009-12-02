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

#ifndef RPG_CHARACTER_ATTRIBUTE_H
#define RPG_CHARACTER_ATTRIBUTE_H

enum RPG_Character_Attribute
{
  ATTRIBUTE_STRENGTH = 0,
  ATTRIBUTE_DEXTERITY,
  ATTRIBUTE_CONSTITUTION,
  ATTRIBUTE_INTELLIGENCE,
  ATTRIBUTE_WISDOM,
  ATTRIBUTE_CHARISMA,
  //
  RPG_CHARACTER_ATTRIBUTE_MAX,
  RPG_CHARACTER_ATTRIBUTE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Attribute, std::string> RPG_Character_AttributeToStringTable_t;
typedef RPG_Character_AttributeToStringTable_t::const_iterator RPG_Character_AttributeToStringTableIterator_t;

class RPG_Character_AttributeHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_AttributeToStringTable.clear();
    myRPG_Character_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_STRENGTH, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_STRENGTH")));
    myRPG_Character_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_DEXTERITY, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_DEXTERITY")));
    myRPG_Character_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_CONSTITUTION, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_CONSTITUTION")));
    myRPG_Character_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_INTELLIGENCE, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_INTELLIGENCE")));
    myRPG_Character_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_WISDOM, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_WISDOM")));
    myRPG_Character_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_CHARISMA, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_CHARISMA")));
  };

  inline static std::string RPG_Character_AttributeToString(const RPG_Character_Attribute& element_in)
  {
    std::string result;
    RPG_Character_AttributeToStringTableIterator_t iterator = myRPG_Character_AttributeToStringTable.find(element_in);
    if (iterator != myRPG_Character_AttributeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_ATTRIBUTE_INVALID");

    return result;
  };

  inline static RPG_Character_Attribute stringToRPG_Character_Attribute(const std::string& string_in)
  {
    RPG_Character_AttributeToStringTableIterator_t iterator = myRPG_Character_AttributeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_AttributeToStringTable.end());

    return RPG_CHARACTER_ATTRIBUTE_INVALID;
  };

  static RPG_Character_AttributeToStringTable_t myRPG_Character_AttributeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AttributeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AttributeHelper(const RPG_Character_AttributeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AttributeHelper& operator=(const RPG_Character_AttributeHelper&));
};

#endif
