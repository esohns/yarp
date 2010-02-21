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

#ifndef RPG_MAGIC_SPELLTYPE_H
#define RPG_MAGIC_SPELLTYPE_H

enum RPG_Magic_SpellType
{
  SPELL_ACID_ARROW = 0,
  SPELL_ACID_FOG,
  SPELL_ALIGN_WEAPON,
  SPELL_BLADE_BARRIER,
  SPELL_BLASPHEMY,
  SPELL_BLESS,
  SPELL_BLINK,
  SPELL_CALL_LIGHTNING,
  SPELL_CANTRIP,
  SPELL_CHAOS_HAMMER,
  SPELL_CHARM_MONSTER,
  SPELL_COMMUNE,
  SPELL_CONFUSION,
  SPELL_DARKNESS,
  SPELL_DELAY_POISON,
  SPELL_DETECT_CHAOS,
  SPELL_DETECT_EVIL,
  SPELL_DETECT_GOOD,
  SPELL_DETECT_LAW,
  SPELL_DETECT_MAGIC,
  SPELL_DETECT_THOUGHTS,
  SPELL_DIMENSION_DOOR,
  SPELL_DISCERN_LOCATION,
  SPELL_DISPEL_MAGIC,
  SPELL_DISPEL_MAGIC_GREATER,
  SPELL_DOMINATE_MONSTER,
  SPELL_ENTANGLE,
  SPELL_ETHEREAL_JAUNT,
  SPELL_FEAR,
  SPELL_FEEBLEMIND,
  SPELL_FIRE_STORM,
  SPELL_GASEOUS_FORM,
  SPELL_HEAL,
  SPELL_HEAL_MASS,
  SPELL_HEROISM,
  SPELL_HOLD_MONSTER,
  SPELL_IMPLOSION,
  SPELL_INSANITY,
  SPELL_INVISIBILITY,
  SPELL_MAGIC_WEAPON,
  SPELL_MIRROR_IMAGE,
  SPELL_NEUTRALIZE_POISON,
  SPELL_PLANE_SHIFT,
  SPELL_POLYMORPH,
  SPELL_POWER_WORD_STUN,
  SPELL_PROJECT_IMAGE,
  SPELL_REMOVE_DISEASE,
  SPELL_RESTORATION,
  SPELL_RESTORATION_GREATER,
  SPELL_REVERSE_GRAVITY,
  SPELL_SCARE,
  SPELL_SEE_INVISIBILITY,
  SPELL_SILENT_IMAGE,
  SPELL_SLOW,
  SPELL_STINKING_CLOUD,
  SPELL_STONE_SHAPE,
  SPELL_SUGGESTION,
  SPELL_SUMMON_MONSTER_I,
  SPELL_SUMMON_MONSTER_III,
  SPELL_SUMMON_MONSTER_IV,
  SPELL_SUMMON_MONSTER_V,
  SPELL_SUMMON_MONSTER_IX,
  SPELL_TELEKINESIS,
  SPELL_TELEPATHY,
  SPELL_TELEPORT_GREATER,
  SPELL_TONGUES,
  SPELL_TRUE_SEEING,
  SPELL_UNHOLY_AURA,
  SPELL_UNHOLY_BLIGHT,
  //
  RPG_MAGIC_SPELLTYPE_MAX,
  RPG_MAGIC_SPELLTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_SpellType, std::string> RPG_Magic_SpellTypeToStringTable_t;
typedef RPG_Magic_SpellTypeToStringTable_t::const_iterator RPG_Magic_SpellTypeToStringTableIterator_t;

class RPG_Magic_SpellTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_SpellTypeToStringTable.clear();
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_ACID_ARROW, ACE_TEXT_ALWAYS_CHAR("SPELL_ACID_ARROW")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_ACID_FOG, ACE_TEXT_ALWAYS_CHAR("SPELL_ACID_FOG")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_ALIGN_WEAPON, ACE_TEXT_ALWAYS_CHAR("SPELL_ALIGN_WEAPON")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_BLADE_BARRIER, ACE_TEXT_ALWAYS_CHAR("SPELL_BLADE_BARRIER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_BLASPHEMY, ACE_TEXT_ALWAYS_CHAR("SPELL_BLASPHEMY")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_BLESS, ACE_TEXT_ALWAYS_CHAR("SPELL_BLESS")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_BLINK, ACE_TEXT_ALWAYS_CHAR("SPELL_BLINK")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_CALL_LIGHTNING, ACE_TEXT_ALWAYS_CHAR("SPELL_CALL_LIGHTNING")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_CANTRIP, ACE_TEXT_ALWAYS_CHAR("SPELL_CANTRIP")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_CHAOS_HAMMER, ACE_TEXT_ALWAYS_CHAR("SPELL_CHAOS_HAMMER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_CHARM_MONSTER, ACE_TEXT_ALWAYS_CHAR("SPELL_CHARM_MONSTER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_COMMUNE, ACE_TEXT_ALWAYS_CHAR("SPELL_COMMUNE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_CONFUSION, ACE_TEXT_ALWAYS_CHAR("SPELL_CONFUSION")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DARKNESS, ACE_TEXT_ALWAYS_CHAR("SPELL_DARKNESS")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DELAY_POISON, ACE_TEXT_ALWAYS_CHAR("SPELL_DELAY_POISON")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DETECT_CHAOS, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_CHAOS")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DETECT_EVIL, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_EVIL")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DETECT_GOOD, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_GOOD")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DETECT_LAW, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_LAW")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DETECT_MAGIC, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_MAGIC")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DETECT_THOUGHTS, ACE_TEXT_ALWAYS_CHAR("SPELL_DETECT_THOUGHTS")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DIMENSION_DOOR, ACE_TEXT_ALWAYS_CHAR("SPELL_DIMENSION_DOOR")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DISCERN_LOCATION, ACE_TEXT_ALWAYS_CHAR("SPELL_DISCERN_LOCATION")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DISPEL_MAGIC, ACE_TEXT_ALWAYS_CHAR("SPELL_DISPEL_MAGIC")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DISPEL_MAGIC_GREATER, ACE_TEXT_ALWAYS_CHAR("SPELL_DISPEL_MAGIC_GREATER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_DOMINATE_MONSTER, ACE_TEXT_ALWAYS_CHAR("SPELL_DOMINATE_MONSTER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_ENTANGLE, ACE_TEXT_ALWAYS_CHAR("SPELL_ENTANGLE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_ETHEREAL_JAUNT, ACE_TEXT_ALWAYS_CHAR("SPELL_ETHEREAL_JAUNT")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_FEAR, ACE_TEXT_ALWAYS_CHAR("SPELL_FEAR")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_FEEBLEMIND, ACE_TEXT_ALWAYS_CHAR("SPELL_FEEBLEMIND")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_FIRE_STORM, ACE_TEXT_ALWAYS_CHAR("SPELL_FIRE_STORM")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_GASEOUS_FORM, ACE_TEXT_ALWAYS_CHAR("SPELL_GASEOUS_FORM")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_HEAL, ACE_TEXT_ALWAYS_CHAR("SPELL_HEAL")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_HEAL_MASS, ACE_TEXT_ALWAYS_CHAR("SPELL_HEAL_MASS")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_HEROISM, ACE_TEXT_ALWAYS_CHAR("SPELL_HEROISM")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_HOLD_MONSTER, ACE_TEXT_ALWAYS_CHAR("SPELL_HOLD_MONSTER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_IMPLOSION, ACE_TEXT_ALWAYS_CHAR("SPELL_IMPLOSION")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_INSANITY, ACE_TEXT_ALWAYS_CHAR("SPELL_INSANITY")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_INVISIBILITY, ACE_TEXT_ALWAYS_CHAR("SPELL_INVISIBILITY")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_MAGIC_WEAPON, ACE_TEXT_ALWAYS_CHAR("SPELL_MAGIC_WEAPON")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_MIRROR_IMAGE, ACE_TEXT_ALWAYS_CHAR("SPELL_MIRROR_IMAGE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_NEUTRALIZE_POISON, ACE_TEXT_ALWAYS_CHAR("SPELL_NEUTRALIZE_POISON")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_PLANE_SHIFT, ACE_TEXT_ALWAYS_CHAR("SPELL_PLANE_SHIFT")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_POLYMORPH, ACE_TEXT_ALWAYS_CHAR("SPELL_POLYMORPH")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_POWER_WORD_STUN, ACE_TEXT_ALWAYS_CHAR("SPELL_POWER_WORD_STUN")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_PROJECT_IMAGE, ACE_TEXT_ALWAYS_CHAR("SPELL_PROJECT_IMAGE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_REMOVE_DISEASE, ACE_TEXT_ALWAYS_CHAR("SPELL_REMOVE_DISEASE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_RESTORATION, ACE_TEXT_ALWAYS_CHAR("SPELL_RESTORATION")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_RESTORATION_GREATER, ACE_TEXT_ALWAYS_CHAR("SPELL_RESTORATION_GREATER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_REVERSE_GRAVITY, ACE_TEXT_ALWAYS_CHAR("SPELL_REVERSE_GRAVITY")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SCARE, ACE_TEXT_ALWAYS_CHAR("SPELL_SCARE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SEE_INVISIBILITY, ACE_TEXT_ALWAYS_CHAR("SPELL_SEE_INVISIBILITY")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SILENT_IMAGE, ACE_TEXT_ALWAYS_CHAR("SPELL_SILENT_IMAGE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SLOW, ACE_TEXT_ALWAYS_CHAR("SPELL_SLOW")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_STINKING_CLOUD, ACE_TEXT_ALWAYS_CHAR("SPELL_STINKING_CLOUD")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_STONE_SHAPE, ACE_TEXT_ALWAYS_CHAR("SPELL_STONE_SHAPE")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SUGGESTION, ACE_TEXT_ALWAYS_CHAR("SPELL_SUGGESTION")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_I, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_I")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_III, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_III")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_IV, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_IV")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_V, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_V")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_SUMMON_MONSTER_IX, ACE_TEXT_ALWAYS_CHAR("SPELL_SUMMON_MONSTER_IX")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_TELEKINESIS, ACE_TEXT_ALWAYS_CHAR("SPELL_TELEKINESIS")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_TELEPATHY, ACE_TEXT_ALWAYS_CHAR("SPELL_TELEPATHY")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_TELEPORT_GREATER, ACE_TEXT_ALWAYS_CHAR("SPELL_TELEPORT_GREATER")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_TONGUES, ACE_TEXT_ALWAYS_CHAR("SPELL_TONGUES")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_TRUE_SEEING, ACE_TEXT_ALWAYS_CHAR("SPELL_TRUE_SEEING")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_UNHOLY_AURA, ACE_TEXT_ALWAYS_CHAR("SPELL_UNHOLY_AURA")));
    myRPG_Magic_SpellTypeToStringTable.insert(std::make_pair(SPELL_UNHOLY_BLIGHT, ACE_TEXT_ALWAYS_CHAR("SPELL_UNHOLY_BLIGHT")));
  };

  inline static std::string RPG_Magic_SpellTypeToString(const RPG_Magic_SpellType& element_in)
  {
    std::string result;
    RPG_Magic_SpellTypeToStringTableIterator_t iterator = myRPG_Magic_SpellTypeToStringTable.find(element_in);
    if (iterator != myRPG_Magic_SpellTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPELLTYPE_INVALID");

    return result;
  };

  inline static RPG_Magic_SpellType stringToRPG_Magic_SpellType(const std::string& string_in)
  {
    RPG_Magic_SpellTypeToStringTableIterator_t iterator = myRPG_Magic_SpellTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_SpellTypeToStringTable.end());

    return RPG_MAGIC_SPELLTYPE_INVALID;
  };

  static RPG_Magic_SpellTypeToStringTable_t myRPG_Magic_SpellTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpellTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpellTypeHelper(const RPG_Magic_SpellTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SpellTypeHelper& operator=(const RPG_Magic_SpellTypeHelper&));
};

#endif
