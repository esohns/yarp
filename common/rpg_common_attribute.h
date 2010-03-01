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

#ifndef RPG_COMMON_ATTRIBUTE_H
#define RPG_COMMON_ATTRIBUTE_H

enum RPG_Common_Attribute
{
  ATTRIBUTE_CHARISMA = 0,
  ATTRIBUTE_CONSTITUTION,
  ATTRIBUTE_DEXTERITY,
  ATTRIBUTE_INTELLIGENCE,
  ATTRIBUTE_STRENGTH,
  ATTRIBUTE_WISDOM,
  //
  RPG_COMMON_ATTRIBUTE_MAX,
  RPG_COMMON_ATTRIBUTE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_Attribute, std::string> RPG_Common_AttributeToStringTable_t;
typedef RPG_Common_AttributeToStringTable_t::const_iterator RPG_Common_AttributeToStringTableIterator_t;

class RPG_Common_AttributeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_AttributeToStringTable.clear();
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_CHARISMA, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_CHARISMA")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_CONSTITUTION, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_CONSTITUTION")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_DEXTERITY, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_DEXTERITY")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_INTELLIGENCE, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_INTELLIGENCE")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_STRENGTH, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_STRENGTH")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_WISDOM, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_WISDOM")));
  };

  inline static std::string RPG_Common_AttributeToString(const RPG_Common_Attribute& element_in)
  {
    std::string result;
    RPG_Common_AttributeToStringTableIterator_t iterator = myRPG_Common_AttributeToStringTable.find(element_in);
    if (iterator != myRPG_Common_AttributeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_ATTRIBUTE_INVALID");

    return result;
  };

  inline static RPG_Common_Attribute stringToRPG_Common_Attribute(const std::string& string_in)
  {
    RPG_Common_AttributeToStringTableIterator_t iterator = myRPG_Common_AttributeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_AttributeToStringTable.end());

    return RPG_COMMON_ATTRIBUTE_INVALID;
  };

  static RPG_Common_AttributeToStringTable_t myRPG_Common_AttributeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AttributeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AttributeHelper(const RPG_Common_AttributeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AttributeHelper& operator=(const RPG_Common_AttributeHelper&));
};

#endif
