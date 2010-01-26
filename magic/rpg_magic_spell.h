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
  SPELL_BLASPHEMY = 0,
  SPELL_BLINK,
  SPELL_CANTRIP,
  SPELL_DARKNESS,
  SPELL_DETECT_CHAOS,
  SPELL_DETECT_EVIL,
  SPELL_DETECT_GOOD,
  SPELL_DETECT_LAW,
  SPELL_DETECT_THOUGHTS,
  SPELL_DIMENSION_DOOR,
  SPELL_DISPEL_MAGIC,
  SPELL_DISPEL_MAGIC_GREATER,
  SPELL_DOMINATE_MONSTER,
  SPELL_ENTANGLE,
  SPELL_ETHEREAL_JAUNT,
  SPELL_FEAR,
  SPELL_FIRE_STORM,
  SPELL_GASEOUS_FORM,
  SPELL_HEAL,
  SPELL_HEAL_MASS,
  SPELL_HOLD_MONSTER,
  SPELL_IMPLOSION,
  SPELL_INSANITY,
  SPELL_INVISIBILITY,
  SPELL_MIRROR_IMAGE,
  SPELL_PLANE_SHIFT,
  SPELL_POLYMORPH,
  SPELL_POWER_WORD_STUN,
  SPELL_REMOVE_DISEASE,
  SPELL_RESTORATION,
  SPELL_RESTORATION_GREATER,
  SPELL_SEE_INVISIBILITY,
  SPELL_SILENT_IMAGE,
  SPELL_STONE_SHAPE,
  SPELL_SUMMON_MONSTER_III,
  SPELL_SUMMON_MONSTER_IX,
  SPELL_TELEKINESIS,
  SPELL_TELEPATHY,
  SPELL_TELEPORT_GREATER,
  SPELL_TRUE_SEEING,
  SPELL_UNHOLY_AURA,
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
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_BLASPHEMY, ACE_TEXT_ALWAYS_CHAR("SPELL_BLASPHEMY")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_BLINK, ACE_TEXT_ALWAYS_CHAR("SPELL_BLINK")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_CANTRIP, ACE_TEXT_ALWAYS_CHAR("SPELL_CANTRIP")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DARKNESS, ACE_TEXT_ALWAYS_CHAR("SPELL_DARKNESS")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DETECT_CHAOS, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_CHAOS")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DETECT_EVIL, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_EVIL")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DETECT_GOOD, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_GOOD")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DETECT_LAW, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_LAW")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DETECT_THOUGHTS, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_THOUGHTS")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DIMENSION_DOOR, ACE_TEXT_ALWAYS_CHAR("SPELL_DIMENSION_DOOR")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DISPEL_MAGIC, ACE_TEXT_ALWAYS_CHAR("SPELL_DISPEL_MAGIC")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DISPEL_MAGIC_GREATER, ACE_TEXT_ALWAYS_CHAR("SPELL_DISPEL_MAGIC_GREATER")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_DOMINATE_MONSTER, ACE_TEXT_ALWAYS_CHAR("SPELL_DOMINATE_MONSTER")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_ENTANGLE, ACE_TEXT_ALWAYS_CHAR("SPELL_ENTANGLE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_ETHEREAL_JAUNT, ACE_TEXT_ALWAYS_CHAR("SPELL_ETHEREAL_JAUNT")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_FEAR, ACE_TEXT_ALWAYS_CHAR("SPELL_FEAR")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_FIRE_STORM, ACE_TEXT_ALWAYS_CHAR("SPELL_FIRE_STORM")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_GASEOUS_FORM, ACE_TEXT_ALWAYS_CHAR("SPELL_GASEOUS_FORM")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_HEAL, ACE_TEXT_ALWAYS_CHAR("SPELL_HEAL")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_HEAL_MASS, ACE_TEXT_ALWAYS_CHAR("SPELL_HEAL_MASS")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_HOLD_MONSTER, ACE_TEXT_ALWAYS_CHAR("SPELL_HOLD_MONSTER")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_IMPLOSION, ACE_TEXT_ALWAYS_CHAR("SPELL_IMPLOSION")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_INSANITY, ACE_TEXT_ALWAYS_CHAR("SPELL_INSANITY")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_INVISIBILITY, ACE_TEXT_ALWAYS_CHAR("SPELL_INVISIBILITY")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_MIRROR_IMAGE, ACE_TEXT_ALWAYS_CHAR("SPELL_MIRROR_IMAGE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_PLANE_SHIFT, ACE_TEXT_ALWAYS_CHAR("SPELL_PLANE_SHIFT")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_POLYMORPH, ACE_TEXT_ALWAYS_CHAR("SPELL_POLYMORPH")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_POWER_WORD_STUN, ACE_TEXT_ALWAYS_CHAR("SPELL_POWER_WORD_STUN")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_REMOVE_DISEASE, ACE_TEXT_ALWAYS_CHAR("SPELL_REMOVE_DISEASE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_RESTORATION, ACE_TEXT_ALWAYS_CHAR("SPELL_RESTORATION")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_RESTORATION_GREATER, ACE_TEXT_ALWAYS_CHAR("SPELL_RESTORATION_GREATER")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_SEE_INVISIBILITY, ACE_TEXT_ALWAYS_CHAR("SPELL_SEE_INVISIBILITY")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_SILENT_IMAGE, ACE_TEXT_ALWAYS_CHAR("SPELL_SILENT_IMAGE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_STONE_SHAPE, ACE_TEXT_ALWAYS_CHAR("SPELL_STONE_SHAPE")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_III, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_III")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_IX, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_IX")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_TELEKINESIS, ACE_TEXT_ALWAYS_CHAR("SPELL_TELEKINESIS")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_TELEPATHY, ACE_TEXT_ALWAYS_CHAR("SPELL_TELEPATHY")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_TELEPORT_GREATER, ACE_TEXT_ALWAYS_CHAR("SPELL_TELEPORT_GREATER")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_TRUE_SEEING, ACE_TEXT_ALWAYS_CHAR("SPELL_TRUE_SEEING")));
    myRPG_Magic_SpellToStringTable.insert(std::make_pair(SPELL_UNHOLY_AURA, ACE_TEXT_ALWAYS_CHAR("SPELL_UNHOLY_AURA")));
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
