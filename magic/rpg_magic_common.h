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

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include <rpg_character_incl.h>
#include "rpg_magic_incl.h"

#include <vector>
#include <set>
#include <string>
#include <map>

// some useful types
typedef std::set<RPG_Magic_SpellType> RPG_Magic_Spells_t;
typedef RPG_Magic_Spells_t::const_iterator RPG_Magic_SpellsIterator_t;

typedef std::vector<RPG_Magic_Spell_Level> RPG_Magic_SpellLevelList_t;
typedef RPG_Magic_SpellLevelList_t::const_iterator RPG_Magic_SpellLevelListIterator_t;
typedef std::vector<RPG_Magic_Spell_PreconditionProperties> RPG_Magic_Spell_PreconditionList_t;
typedef RPG_Magic_Spell_PreconditionList_t::const_iterator RPG_Magic_Spell_PreconditionListIterator_t;
typedef std::vector<RPG_Magic_Spell_EffectProperties> RPG_Magic_Spell_EffectList_t;
typedef RPG_Magic_Spell_EffectList_t::const_iterator RPG_Magic_Spell_EffectListIterator_t;

struct RPG_Magic_Spell_Properties
{
//   std::string name;
  RPG_Magic_Spell_Type type;
  RPG_Magic_SpellLevelList_t levels;
  unsigned char cost;
  RPG_Common_ActionType action;
  RPG_Magic_Spell_RangeProperties range;
  RPG_Magic_Spell_TargetProperties target;
  RPG_Magic_Spell_DurationProperties duration;
  RPG_Magic_Spell_PreconditionList_t preconditions;
  RPG_Common_SavingThrowCheck save;
  RPG_Magic_Spell_EffectList_t effects;
  RPG_Common_SavingThrow saveable;
  bool resistible;
};

typedef std::map<std::string, RPG_Magic_Spell_Properties> RPG_Magic_Dictionary_t;
typedef RPG_Magic_Dictionary_t::const_iterator RPG_Magic_DictionaryIterator_t;

#endif
