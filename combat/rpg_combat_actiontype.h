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

#ifndef RPG_COMBAT_ACTIONTYPE_H
#define RPG_COMBAT_ACTIONTYPE_H

enum RPG_Combat_ActionType
{
  ACTION_STANDARD = 0,
  ACTION_MOVE,
  ACTION_FULLROUND,
  ACTION_FREE,
  ACTION_NOTANACTION,
  ACTION_RESTRICTED,
  //
  RPG_COMBAT_ACTIONTYPE_MAX,
  RPG_COMBAT_ACTIONTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_ActionType, std::string> RPG_Combat_ActionTypeToStringTable_t;
typedef RPG_Combat_ActionTypeToStringTable_t::const_iterator RPG_Combat_ActionTypeToStringTableIterator_t;

class RPG_Combat_ActionTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_ActionTypeToStringTable.clear();
    myRPG_Combat_ActionTypeToStringTable.insert(std::make_pair(ACTION_STANDARD, ACE_TEXT_ALWAYS_CHAR("ACTION_STANDARD")));
    myRPG_Combat_ActionTypeToStringTable.insert(std::make_pair(ACTION_MOVE, ACE_TEXT_ALWAYS_CHAR("ACTION_MOVE")));
    myRPG_Combat_ActionTypeToStringTable.insert(std::make_pair(ACTION_FULLROUND, ACE_TEXT_ALWAYS_CHAR("ACTION_FULLROUND")));
    myRPG_Combat_ActionTypeToStringTable.insert(std::make_pair(ACTION_FREE, ACE_TEXT_ALWAYS_CHAR("ACTION_FREE")));
    myRPG_Combat_ActionTypeToStringTable.insert(std::make_pair(ACTION_NOTANACTION, ACE_TEXT_ALWAYS_CHAR("ACTION_NOTANACTION")));
    myRPG_Combat_ActionTypeToStringTable.insert(std::make_pair(ACTION_RESTRICTED, ACE_TEXT_ALWAYS_CHAR("ACTION_RESTRICTED")));
  };

  inline static std::string RPG_Combat_ActionTypeToString(const RPG_Combat_ActionType& element_in)
  {
    std::string result;
    RPG_Combat_ActionTypeToStringTableIterator_t iterator = myRPG_Combat_ActionTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_ActionTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_ACTIONTYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_ActionType stringToRPG_Combat_ActionType(const std::string& string_in)
  {
    RPG_Combat_ActionTypeToStringTableIterator_t iterator = myRPG_Combat_ActionTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_ActionTypeToStringTable.end());

    return RPG_COMBAT_ACTIONTYPE_INVALID;
  };

  static RPG_Combat_ActionTypeToStringTable_t myRPG_Combat_ActionTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_ActionTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_ActionTypeHelper(const RPG_Combat_ActionTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_ActionTypeHelper& operator=(const RPG_Combat_ActionTypeHelper&));
};

#endif
