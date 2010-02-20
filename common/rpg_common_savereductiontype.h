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

#ifndef RPG_COMMON_SAVEREDUCTIONTYPE_H
#define RPG_COMMON_SAVEREDUCTIONTYPE_H

enum RPG_Common_SaveReductionType
{
  SAVEREDUCTION_NEGATES = 0,
  SAVEREDUCTION_PARTIAL,
  SAVEREDUCTION_HALF,
  //
  RPG_COMMON_SAVEREDUCTIONTYPE_MAX,
  RPG_COMMON_SAVEREDUCTIONTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_SaveReductionType, std::string> RPG_Common_SaveReductionTypeToStringTable_t;
typedef RPG_Common_SaveReductionTypeToStringTable_t::const_iterator RPG_Common_SaveReductionTypeToStringTableIterator_t;

class RPG_Common_SaveReductionTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_SaveReductionTypeToStringTable.clear();
    myRPG_Common_SaveReductionTypeToStringTable.insert(std::make_pair(SAVEREDUCTION_NEGATES, ACE_TEXT_ALWAYS_CHAR("SAVEREDUCTION_NEGATES")));
    myRPG_Common_SaveReductionTypeToStringTable.insert(std::make_pair(SAVEREDUCTION_PARTIAL, ACE_TEXT_ALWAYS_CHAR("SAVEREDUCTION_PARTIAL")));
    myRPG_Common_SaveReductionTypeToStringTable.insert(std::make_pair(SAVEREDUCTION_HALF, ACE_TEXT_ALWAYS_CHAR("SAVEREDUCTION_HALF")));
  };

  inline static std::string RPG_Common_SaveReductionTypeToString(const RPG_Common_SaveReductionType& element_in)
  {
    std::string result;
    RPG_Common_SaveReductionTypeToStringTableIterator_t iterator = myRPG_Common_SaveReductionTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_SaveReductionTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_SAVEREDUCTIONTYPE_INVALID");

    return result;
  };

  inline static RPG_Common_SaveReductionType stringToRPG_Common_SaveReductionType(const std::string& string_in)
  {
    RPG_Common_SaveReductionTypeToStringTableIterator_t iterator = myRPG_Common_SaveReductionTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_SaveReductionTypeToStringTable.end());

    return RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;
  };

  static RPG_Common_SaveReductionTypeToStringTable_t myRPG_Common_SaveReductionTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SaveReductionTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SaveReductionTypeHelper(const RPG_Common_SaveReductionTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SaveReductionTypeHelper& operator=(const RPG_Common_SaveReductionTypeHelper&));
};

#endif
