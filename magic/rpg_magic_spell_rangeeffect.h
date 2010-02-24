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

#ifndef RPG_MAGIC_SPELL_RANGEEFFECT_H
#define RPG_MAGIC_SPELL_RANGEEFFECT_H

enum RPG_Magic_Spell_RangeEffect
{
  RANGEEFFECT_PERSONAL = 0,
  RANGEEFFECT_TOUCH,
  RANGEEFFECT_CLOSE,
  RANGEEFFECT_MEDIUM,
  RANGEEFFECT_LONG,
  RANGEEFFECT_UNLIMITED,
  RANGEEFFECT_RANGED,
  //
  RPG_MAGIC_SPELL_RANGEEFFECT_MAX,
  RPG_MAGIC_SPELL_RANGEEFFECT_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_Spell_RangeEffect, std::string> RPG_Magic_Spell_RangeEffectToStringTable_t;
typedef RPG_Magic_Spell_RangeEffectToStringTable_t::const_iterator RPG_Magic_Spell_RangeEffectToStringTableIterator_t;

class RPG_Magic_Spell_RangeEffectHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_Spell_RangeEffectToStringTable.clear();
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_PERSONAL, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_PERSONAL")));
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_TOUCH, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_TOUCH")));
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_CLOSE, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_CLOSE")));
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_MEDIUM, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_MEDIUM")));
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_LONG, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_LONG")));
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_UNLIMITED, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_UNLIMITED")));
    myRPG_Magic_Spell_RangeEffectToStringTable.insert(std::make_pair(RANGEEFFECT_RANGED, ACE_TEXT_ALWAYS_CHAR("RANGEEFFECT_RANGED")));
  };

  inline static std::string RPG_Magic_Spell_RangeEffectToString(const RPG_Magic_Spell_RangeEffect& element_in)
  {
    std::string result;
    RPG_Magic_Spell_RangeEffectToStringTableIterator_t iterator = myRPG_Magic_Spell_RangeEffectToStringTable.find(element_in);
    if (iterator != myRPG_Magic_Spell_RangeEffectToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPELL_RANGEEFFECT_INVALID");

    return result;
  };

  inline static RPG_Magic_Spell_RangeEffect stringToRPG_Magic_Spell_RangeEffect(const std::string& string_in)
  {
    RPG_Magic_Spell_RangeEffectToStringTableIterator_t iterator = myRPG_Magic_Spell_RangeEffectToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_Spell_RangeEffectToStringTable.end());

    return RPG_MAGIC_SPELL_RANGEEFFECT_INVALID;
  };

  static RPG_Magic_Spell_RangeEffectToStringTable_t myRPG_Magic_Spell_RangeEffectToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_RangeEffectHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_RangeEffectHelper(const RPG_Magic_Spell_RangeEffectHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_RangeEffectHelper& operator=(const RPG_Magic_Spell_RangeEffectHelper&));
};

#endif
