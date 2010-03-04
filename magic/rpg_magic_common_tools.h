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
#ifndef RPG_MAGIC_COMMON_TOOLS_H
#define RPG_MAGIC_COMMON_TOOLS_H

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_magic_incl.h"

#include "rpg_magic_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Magic_Common_Tools
{
 public:
  static void init();

  static const std::string spellToName(const RPG_Magic_SpellType&); // type
  static const std::string spellTypeToString(const RPG_Magic_Spell_Type&); // type
  static const std::string spellLevelsToString(const RPG_Magic_SpellLevelList_t&); // levels
  static const std::string spellRangeToString(const RPG_Magic_Spell_RangeProperties&); // range
  static const std::string spellTargetToString(const RPG_Magic_Spell_TargetProperties&); // target
  static const std::string spellDurationToString(const RPG_Magic_Spell_DurationProperties&); // duration
  static const std::string preconditionsToString(const RPG_Magic_Spell_PreconditionList_t&); // preconditions
  static const std::string effectsToString(const RPG_Magic_Spell_EffectList_t&); // effects
  static const std::string spellsToString(const RPG_Magic_Spells_t&); // spells
  static const std::string spellsToString(const RPG_Magic_SpellList_t&); // spells

  static void getNumSpellsPerLevel(const RPG_Common_SubClass&, // subclass
                                   const unsigned char&,       // class level
                                   const unsigned char&,       // spell level
                                   unsigned char&,             // return value: #spells (/day)
                                   unsigned char&);            // return value: #spells known

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Magic_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Common_Tools(const RPG_Magic_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Common_Tools& operator=(const RPG_Magic_Common_Tools&));

  // helper methods
  static void initStringConversionTables();
  static void initSpellsTables();

  static void updateSpellRange(RPG_Magic_Spell_RangeProperties&); // range

  typedef std::pair<unsigned char, unsigned char> RPG_Magic_ClassLevelSpellLevelPair_t;
  typedef std::pair<RPG_Common_SubClass, RPG_Magic_ClassLevelSpellLevelPair_t> RPG_Magic_SubClassLevelPair_t;
  typedef std::map<RPG_Magic_SubClassLevelPair_t, unsigned char> RPG_Magic_NumSpellsTable_t;
  typedef RPG_Magic_NumSpellsTable_t::const_iterator RPG_Magic_NumSpellsTableIterator_t;
  static RPG_Magic_NumSpellsTable_t myNumSpellsTable;
  static RPG_Magic_NumSpellsTable_t myNumSpellsKnownTable;
};

#endif
