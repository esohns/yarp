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

#ifndef RPG_MAGIC_COMMON_H
#define RPG_MAGIC_COMMON_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "rpg_common_incl.h"

#include "rpg_magic_incl.h"

typedef std::set<enum RPG_Magic_SpellType> RPG_Magic_SpellTypes_t;
typedef RPG_Magic_SpellTypes_t::const_iterator RPG_Magic_SpellTypesIterator_t;
typedef std::vector<enum RPG_Magic_SpellType> RPG_Magic_Spells_t;
typedef RPG_Magic_Spells_t::const_iterator RPG_Magic_SpellsIterator_t;

typedef std::vector<struct RPG_Magic_Spell_Level> RPG_Magic_SpellLevels_t;
typedef RPG_Magic_SpellLevels_t::const_iterator RPG_Magic_SpellLevelsIterator_t;
typedef std::vector<struct RPG_Magic_Spell_TargetProperties> RPG_Magic_Spell_Targets_t;
typedef RPG_Magic_Spell_Targets_t::const_iterator RPG_Magic_Spell_TargetsIterator_t;
typedef std::vector<struct RPG_Magic_Spell_PreconditionProperties> RPG_Magic_Spell_Preconditions_t;
typedef RPG_Magic_Spell_Preconditions_t::const_iterator RPG_Magic_Spell_PreconditionsIterator_t;
typedef std::vector<struct RPG_Magic_Spell_EffectProperties> RPG_Magic_Spell_Effects_t;
typedef RPG_Magic_Spell_Effects_t::const_iterator RPG_Magic_Spell_EffectsIterator_t;
typedef std::vector<struct RPG_Magic_CounterMeasure> RPG_Magic_Spell_CounterMeasures_t;
typedef RPG_Magic_Spell_CounterMeasures_t::const_iterator RPG_Magic_Spell_CounterMeasuresIterator_t;

struct RPG_Magic_Spell_Properties
{
  RPG_Magic_Spell_Properties ()
   : type ()
   , levels ()
   , cost (0)
   , time ()
   , range ()
   , targets ()
   , duration ()
   , preconditions ()
   , effects ()
   , counterMeasures ()
   , saveable (RPG_COMMON_SAVINGTHROW_INVALID)
   , resistible (false)
  {}

  struct RPG_Magic_Spell_Type               type;
  RPG_Magic_SpellLevels_t                   levels;
  ACE_UINT8                                 cost;
  struct RPG_Magic_Spell_CastingTime        time;
  struct RPG_Magic_Spell_RangeProperties    range;
  RPG_Magic_Spell_Targets_t                 targets;
  struct RPG_Magic_Spell_DurationProperties duration;
  RPG_Magic_Spell_Preconditions_t           preconditions;
  RPG_Magic_Spell_Effects_t                 effects;
  RPG_Magic_Spell_CounterMeasures_t         counterMeasures;
  enum RPG_Common_SavingThrow               saveable;
  bool                                      resistible;
};
typedef std::map<std::string, struct RPG_Magic_Spell_Properties> RPG_Magic_Dictionary_t;
typedef RPG_Magic_Dictionary_t::const_iterator RPG_Magic_DictionaryIterator_t;

#endif
