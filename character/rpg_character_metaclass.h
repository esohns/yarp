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

#ifndef RPG_CHARACTER_METACLASS_H
#define RPG_CHARACTER_METACLASS_H

enum RPG_Character_MetaClass
{
  METACLASS_NONE = 0,
  METACLASS_PRIEST,
  METACLASS_ROGUE,
  METACLASS_WARRIOR,
  METACLASS_WIZARD,
  //
  RPG_CHARACTER_METACLASS_MAX,
  RPG_CHARACTER_METACLASS_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_MetaClass, std::string> RPG_Character_MetaClassToStringTable_t;
typedef RPG_Character_MetaClassToStringTable_t::const_iterator RPG_Character_MetaClassToStringTableIterator_t;

class RPG_Character_MetaClassHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_MetaClassToStringTable.clear();
    myRPG_Character_MetaClassToStringTable.insert(std::make_pair(METACLASS_NONE, ACE_TEXT_ALWAYS_CHAR("METACLASS_NONE")));
    myRPG_Character_MetaClassToStringTable.insert(std::make_pair(METACLASS_PRIEST, ACE_TEXT_ALWAYS_CHAR("METACLASS_PRIEST")));
    myRPG_Character_MetaClassToStringTable.insert(std::make_pair(METACLASS_ROGUE, ACE_TEXT_ALWAYS_CHAR("METACLASS_ROGUE")));
    myRPG_Character_MetaClassToStringTable.insert(std::make_pair(METACLASS_WARRIOR, ACE_TEXT_ALWAYS_CHAR("METACLASS_WARRIOR")));
    myRPG_Character_MetaClassToStringTable.insert(std::make_pair(METACLASS_WIZARD, ACE_TEXT_ALWAYS_CHAR("METACLASS_WIZARD")));
  };

  inline static std::string RPG_Character_MetaClassToString(const RPG_Character_MetaClass& element_in)
  {
    std::string result;
    RPG_Character_MetaClassToStringTableIterator_t iterator = myRPG_Character_MetaClassToStringTable.find(element_in);
    if (iterator != myRPG_Character_MetaClassToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_METACLASS_INVALID");

    return result;
  };

  inline static RPG_Character_MetaClass stringToRPG_Character_MetaClass(const std::string& string_in)
  {
    RPG_Character_MetaClassToStringTableIterator_t iterator = myRPG_Character_MetaClassToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_MetaClassToStringTable.end());

    return RPG_CHARACTER_METACLASS_INVALID;
  };

  static RPG_Character_MetaClassToStringTable_t myRPG_Character_MetaClassToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MetaClassHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MetaClassHelper(const RPG_Character_MetaClassHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MetaClassHelper& operator=(const RPG_Character_MetaClassHelper&));
};

#endif
