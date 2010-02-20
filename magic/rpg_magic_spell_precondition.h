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

#ifndef RPG_MAGIC_SPELL_PRECONDITION_H
#define RPG_MAGIC_SPELL_PRECONDITION_H

enum RPG_Magic_Spell_Precondition
{
  PRECONDITION_RANGED_TOUCH_ATTACK = 0,
  //
  RPG_MAGIC_SPELL_PRECONDITION_MAX,
  RPG_MAGIC_SPELL_PRECONDITION_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_Spell_Precondition, std::string> RPG_Magic_Spell_PreconditionToStringTable_t;
typedef RPG_Magic_Spell_PreconditionToStringTable_t::const_iterator RPG_Magic_Spell_PreconditionToStringTableIterator_t;

class RPG_Magic_Spell_PreconditionHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_Spell_PreconditionToStringTable.clear();
    myRPG_Magic_Spell_PreconditionToStringTable.insert(std::make_pair(PRECONDITION_RANGED_TOUCH_ATTACK, ACE_TEXT_ALWAYS_CHAR("PRECONDITION_RANGED_TOUCH_ATTACK")));
  };

  inline static std::string RPG_Magic_Spell_PreconditionToString(const RPG_Magic_Spell_Precondition& element_in)
  {
    std::string result;
    RPG_Magic_Spell_PreconditionToStringTableIterator_t iterator = myRPG_Magic_Spell_PreconditionToStringTable.find(element_in);
    if (iterator != myRPG_Magic_Spell_PreconditionToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPELL_PRECONDITION_INVALID");

    return result;
  };

  inline static RPG_Magic_Spell_Precondition stringToRPG_Magic_Spell_Precondition(const std::string& string_in)
  {
    RPG_Magic_Spell_PreconditionToStringTableIterator_t iterator = myRPG_Magic_Spell_PreconditionToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_Spell_PreconditionToStringTable.end());

    return RPG_MAGIC_SPELL_PRECONDITION_INVALID;
  };

  static RPG_Magic_Spell_PreconditionToStringTable_t myRPG_Magic_Spell_PreconditionToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_PreconditionHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_PreconditionHelper(const RPG_Magic_Spell_PreconditionHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_PreconditionHelper& operator=(const RPG_Magic_Spell_PreconditionHelper&));
};

#endif
