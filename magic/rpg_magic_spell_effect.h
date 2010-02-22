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

#ifndef RPG_MAGIC_SPELL_EFFECT_H
#define RPG_MAGIC_SPELL_EFFECT_H

enum RPG_Magic_Spell_Effect
{
  SPELLEFFECT_DAMAGE = 0,
  SPELLEFFECT_BONUS_HP,
  SPELLEFFECT_BONUS_MORALE,
  SPELLEFFECT_RESISTANCE_FEAR,
  //
  RPG_MAGIC_SPELL_EFFECT_MAX,
  RPG_MAGIC_SPELL_EFFECT_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_Spell_Effect, std::string> RPG_Magic_Spell_EffectToStringTable_t;
typedef RPG_Magic_Spell_EffectToStringTable_t::const_iterator RPG_Magic_Spell_EffectToStringTableIterator_t;

class RPG_Magic_Spell_EffectHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_Spell_EffectToStringTable.clear();
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DAMAGE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DAMAGE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BONUS_HP, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BONUS_HP")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BONUS_MORALE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BONUS_MORALE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_RESISTANCE_FEAR, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_RESISTANCE_FEAR")));
  };

  inline static std::string RPG_Magic_Spell_EffectToString(const RPG_Magic_Spell_Effect& element_in)
  {
    std::string result;
    RPG_Magic_Spell_EffectToStringTableIterator_t iterator = myRPG_Magic_Spell_EffectToStringTable.find(element_in);
    if (iterator != myRPG_Magic_Spell_EffectToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPELL_EFFECT_INVALID");

    return result;
  };

  inline static RPG_Magic_Spell_Effect stringToRPG_Magic_Spell_Effect(const std::string& string_in)
  {
    RPG_Magic_Spell_EffectToStringTableIterator_t iterator = myRPG_Magic_Spell_EffectToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_Spell_EffectToStringTable.end());

    return RPG_MAGIC_SPELL_EFFECT_INVALID;
  };

  static RPG_Magic_Spell_EffectToStringTable_t myRPG_Magic_Spell_EffectToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_EffectHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_EffectHelper(const RPG_Magic_Spell_EffectHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_EffectHelper& operator=(const RPG_Magic_Spell_EffectHelper&));
};

#endif
