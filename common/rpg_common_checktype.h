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

#ifndef RPG_COMMON_CHECKTYPE_H
#define RPG_COMMON_CHECKTYPE_H

enum RPG_Common_CheckType
{
  CHECK_GRAPPLE = 0,
  //
  RPG_COMMON_CHECKTYPE_MAX,
  RPG_COMMON_CHECKTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_CheckType, std::string> RPG_Common_CheckTypeToStringTable_t;
typedef RPG_Common_CheckTypeToStringTable_t::const_iterator RPG_Common_CheckTypeToStringTableIterator_t;

class RPG_Common_CheckTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_CheckTypeToStringTable.clear();
    myRPG_Common_CheckTypeToStringTable.insert(std::make_pair(CHECK_GRAPPLE, ACE_TEXT_ALWAYS_CHAR("CHECK_GRAPPLE")));
  };

  inline static std::string RPG_Common_CheckTypeToString(const RPG_Common_CheckType& element_in)
  {
    std::string result;
    RPG_Common_CheckTypeToStringTableIterator_t iterator = myRPG_Common_CheckTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_CheckTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_CHECKTYPE_INVALID");

    return result;
  };

  inline static RPG_Common_CheckType stringToRPG_Common_CheckType(const std::string& string_in)
  {
    RPG_Common_CheckTypeToStringTableIterator_t iterator = myRPG_Common_CheckTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_CheckTypeToStringTable.end());

    return RPG_COMMON_CHECKTYPE_INVALID;
  };

  static RPG_Common_CheckTypeToStringTable_t myRPG_Common_CheckTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CheckTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CheckTypeHelper(const RPG_Common_CheckTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CheckTypeHelper& operator=(const RPG_Common_CheckTypeHelper&));
};

#endif
