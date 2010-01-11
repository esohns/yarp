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

#ifndef RPG_MAGIC_SPELL_H
#define RPG_MAGIC_SPELL_H

enum RPG_Magic_Spell
{
  SPELL_CANTRIP = 0,
  SPELL_REMOVE_DISEASE,
  SPELL_HEAL,
  SPELL_MASS_HEAL,
  SPELL_ENTANGLE,
  SPELL_FEAR,
  SPELL_GASEOUS_FORM,
  //
  RPG_MAGIC_SPELL_MAX,
  RPG_MAGIC_SPELL_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_Spell, std::string> RPG_Magic_SpellToStringTable_t;
typedef RPG_Magic_SpellToStringTable_t::const_iterator RPG_Magic_SpellToStringTableIterator_t;

class RPG_Magic_SpellHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_SpellToStringTable.clear();
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_CANTRIP, ACE_TEXT_ALWAYS_CHAR("SPELL_CANTRIP")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_REMOVE_DISEASE, ACE_TEXT_ALWAYS_CHAR("SPELL_REMOVE_DISEASE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_HEAL, ACE_TEXT_ALWAYS_CHAR("SPELL_HEAL")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_MASS_HEAL, ACE_TEXT_ALWAYS_CHAR("SPELL_MASS_HEAL")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_ENTANGLE, ACE_TEXT_ALWAYS_CHAR("SPELL_ENTANGLE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_FEAR, ACE_TEXT_ALWAYS_CHAR("SPELL_FEAR")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_GASEOUS_FORM, ACE_TEXT_ALWAYS_CHAR("SPELL_GASEOUS_FORM")));
  };

  inline static std::string RPG_Magic_SpellToString(const RPG_Magic_Spell& element_in)
  {
    std::string result;
    RPG_Magic_SpellToStringTableIterator_t iterator = myRPG_Magic_SpellToStringTable.find(element_in);
    if (iterator != myRPG_Magic_SpellToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPELL_INVALID");

    return result;
  };

  inline static RPG_Magic_Spell stringToRPG_Magic_Spell(const std::string& string_in)
  {
    RPG_Magic_SpellToStringTableIterator_t iterator = myRPG_Magic_SpellToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_SpellToStringTable.end());

    return RPG_MAGIC_SPELL_INVALID;
  };

  static RPG_Magic_SpellToStringTable_t myRPG_Magic_SpellToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpellHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpellHelper(const RPG_Magic_SpellHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpellHelper& operator=(const RPG_Magic_SpellHelper&));
};

#endif
