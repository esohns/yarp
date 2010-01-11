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

#ifndef RPG_COMMON_EFFECTTYPE_H
#define RPG_COMMON_EFFECTTYPE_H

enum RPG_Common_EffectType
{
  EFFECT_IMMEDIATE = 0,
  EFFECT_BATTLE,
  EFFECT_TEMPORARY,
  EFFECT_PERMANENT,
  //
  RPG_COMMON_EFFECTTYPE_MAX,
  RPG_COMMON_EFFECTTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_EffectType, std::string> RPG_Common_EffectTypeToStringTable_t;
typedef RPG_Common_EffectTypeToStringTable_t::const_iterator RPG_Common_EffectTypeToStringTableIterator_t;

class RPG_Common_EffectTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_EffectTypeToStringTable.clear();
    myRPG_Common_EffectTypeToStringTable.insert(std::make_pair(EFFECT_IMMEDIATE, ACE_TEXT_ALWAYS_CHAR("EFFECT_IMMEDIATE")));
    myRPG_Common_EffectTypeToStringTable.insert(std::make_pair(EFFECT_BATTLE, ACE_TEXT_ALWAYS_CHAR("EFFECT_BATTLE")));
    myRPG_Common_EffectTypeToStringTable.insert(std::make_pair(EFFECT_TEMPORARY, ACE_TEXT_ALWAYS_CHAR("EFFECT_TEMPORARY")));
    myRPG_Common_EffectTypeToStringTable.insert(std::make_pair(EFFECT_PERMANENT, ACE_TEXT_ALWAYS_CHAR("EFFECT_PERMANENT")));
  };

  inline static std::string RPG_Common_EffectTypeToString(const RPG_Common_EffectType& element_in)
  {
    std::string result;
    RPG_Common_EffectTypeToStringTableIterator_t iterator = myRPG_Common_EffectTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_EffectTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_EFFECTTYPE_INVALID");

    return result;
  };

  inline static RPG_Common_EffectType stringToRPG_Common_EffectType(const std::string& string_in)
  {
    RPG_Common_EffectTypeToStringTableIterator_t iterator = myRPG_Common_EffectTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_EffectTypeToStringTable.end());

    return RPG_COMMON_EFFECTTYPE_INVALID;
  };

  static RPG_Common_EffectTypeToStringTable_t myRPG_Common_EffectTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_EffectTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_EffectTypeHelper(const RPG_Common_EffectTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_EffectTypeHelper& operator=(const RPG_Common_EffectTypeHelper&));
};

#endif
