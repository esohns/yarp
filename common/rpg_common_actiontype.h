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

#ifndef RPG_COMMON_ACTIONTYPE_H
#define RPG_COMMON_ACTIONTYPE_H

enum RPG_Common_ActionType
{
  ACTION_FREE = 0,
  ACTION_FULLROUND,
  ACTION_MOVE,
  ACTION_NOTANACTION,
  ACTION_RESTRICTED,
  ACTION_STANDARD,
  ACTION_TIME,
  //
  RPG_COMMON_ACTIONTYPE_MAX,
  RPG_COMMON_ACTIONTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_ActionType, std::string> RPG_Common_ActionTypeToStringTable_t;
typedef RPG_Common_ActionTypeToStringTable_t::const_iterator RPG_Common_ActionTypeToStringTableIterator_t;

class RPG_Common_ActionTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_ActionTypeToStringTable.clear();
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_FREE, ACE_TEXT_ALWAYS_CHAR("ACTION_FREE")));
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_FULLROUND, ACE_TEXT_ALWAYS_CHAR("ACTION_FULLROUND")));
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_MOVE, ACE_TEXT_ALWAYS_CHAR("ACTION_MOVE")));
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_NOTANACTION, ACE_TEXT_ALWAYS_CHAR("ACTION_NOTANACTION")));
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_RESTRICTED, ACE_TEXT_ALWAYS_CHAR("ACTION_RESTRICTED")));
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_STANDARD, ACE_TEXT_ALWAYS_CHAR("ACTION_STANDARD")));
    myRPG_Common_ActionTypeToStringTable.insert(std::make_pair(ACTION_TIME, ACE_TEXT_ALWAYS_CHAR("ACTION_TIME")));
  };

  inline static std::string RPG_Common_ActionTypeToString(const RPG_Common_ActionType& element_in)
  {
    std::string result;
    RPG_Common_ActionTypeToStringTableIterator_t iterator = myRPG_Common_ActionTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_ActionTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_ACTIONTYPE_INVALID");

    return result;
  };

  inline static RPG_Common_ActionType stringToRPG_Common_ActionType(const std::string& string_in)
  {
    RPG_Common_ActionTypeToStringTableIterator_t iterator = myRPG_Common_ActionTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_ActionTypeToStringTable.end());

    return RPG_COMMON_ACTIONTYPE_INVALID;
  };

  static RPG_Common_ActionTypeToStringTable_t myRPG_Common_ActionTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ActionTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ActionTypeHelper(const RPG_Common_ActionTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ActionTypeHelper& operator=(const RPG_Common_ActionTypeHelper&));
};

#endif
