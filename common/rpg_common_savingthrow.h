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

#ifndef RPG_COMMON_SAVINGTHROW_H
#define RPG_COMMON_SAVINGTHROW_H

enum RPG_Common_SavingThrow
{
  SAVE_FORTITUDE = 0,
  SAVE_REFLEX,
  SAVE_WILL,
  //
  RPG_COMMON_SAVINGTHROW_MAX,
  RPG_COMMON_SAVINGTHROW_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_SavingThrow, std::string> RPG_Common_SavingThrowToStringTable_t;
typedef RPG_Common_SavingThrowToStringTable_t::const_iterator RPG_Common_SavingThrowToStringTableIterator_t;

class RPG_Common_SavingThrowHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_SavingThrowToStringTable.clear();
    myRPG_Common_SavingThrowToStringTable.insert(std::make_pair(SAVE_FORTITUDE, ACE_TEXT_ALWAYS_CHAR("SAVE_FORTITUDE")));
    myRPG_Common_SavingThrowToStringTable.insert(std::make_pair(SAVE_REFLEX, ACE_TEXT_ALWAYS_CHAR("SAVE_REFLEX")));
    myRPG_Common_SavingThrowToStringTable.insert(std::make_pair(SAVE_WILL, ACE_TEXT_ALWAYS_CHAR("SAVE_WILL")));
  };

  inline static std::string RPG_Common_SavingThrowToString(const RPG_Common_SavingThrow& element_in)
  {
    std::string result;
    RPG_Common_SavingThrowToStringTableIterator_t iterator = myRPG_Common_SavingThrowToStringTable.find(element_in);
    if (iterator != myRPG_Common_SavingThrowToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_SAVINGTHROW_INVALID");

    return result;
  };

  inline static RPG_Common_SavingThrow stringToRPG_Common_SavingThrow(const std::string& string_in)
  {
    RPG_Common_SavingThrowToStringTableIterator_t iterator = myRPG_Common_SavingThrowToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_SavingThrowToStringTable.end());

    return RPG_COMMON_SAVINGTHROW_INVALID;
  };

  static RPG_Common_SavingThrowToStringTable_t myRPG_Common_SavingThrowToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SavingThrowHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SavingThrowHelper(const RPG_Common_SavingThrowHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SavingThrowHelper& operator=(const RPG_Common_SavingThrowHelper&));
};

#endif
