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

#include "ace/Global_Macros.h"

#include "rpg_dice_incl.h"

#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"

#include "rpg_character_class_common.h"
#include "rpg_character_common.h"
#include "rpg_character_incl.h"

#include "rpg_magic_common.h"
#include "rpg_magic_dictionary.h"
#include "rpg_magic_incl.h"

class RPG_Magic_Common_Tools
{
  // allow access to some specific helper methods
  friend class RPG_Magic_Dictionary;

 public:
  // init string conversion (and other) tables
  static void initialize ();

  static std::string spellToName (enum RPG_Magic_SpellType); // type

  // debug info
  static std::string toString (const RPG_Magic_Spells_t&); // spells
  static std::string toString (const RPG_Magic_SpellTypes_t&); // spells

  static bool isCasterClass (enum RPG_Common_SubClass); // subClass
  static bool isDivineCasterClass (enum RPG_Common_SubClass); // subClass
  static bool hasCasterClass (const RPG_Character_Class&); // class(es)
  static bool hasDivineCasterClass (const RPG_Character_Class&); // class(es)
  static bool hasArcaneCasterClass (const RPG_Character_Class&); // class(es)
  static ACE_UINT16 getNumKnownSpells (enum RPG_Common_SubClass, // subclass
                                       RPG_Character_Level_t,    // class level
                                       ACE_UINT8);               // spell level (Bard/Sorcerer)
  static unsigned int getNumSpells (enum RPG_Common_SubClass, // subclass
                                    RPG_Character_Level_t,    // class level
                                    ACE_UINT8);               // spell level (== 0xFF ? total # spell slots)

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC (RPG_Magic_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Magic_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Magic_Common_Tools (const RPG_Magic_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Magic_Common_Tools& operator= (const RPG_Magic_Common_Tools&))

  // helper methods
  static std::string toString (const RPG_Magic_Spell_Type&); // type
  static std::string toString (const RPG_Magic_SpellLevels_t&); // levels
  static std::string toString (const RPG_Magic_Spell_RangeProperties&); // range
  static std::string toString (const RPG_Magic_Spell_Targets_t&); // targets
  static std::string toString (const RPG_Magic_Spell_DurationProperties&); // duration
  static std::string toString (const RPG_Magic_Spell_Preconditions_t&); // preconditions
  static std::string toString (const RPG_Magic_Spell_Effects_t&); // effects
  static std::string toString (const RPG_Magic_Spell_CounterMeasures_t&); // counterMeasures

  // (internal) helper methods
  static void initializeStringConversionTables ();
  static void initializeSpellsTables ();

  static void updateSpellRange (RPG_Magic_Spell_RangeProperties&); // range

  typedef std::pair<RPG_Character_Level_t, ACE_UINT8> RPG_Magic_ClassLevelSpellLevelPair_t;
  typedef std::pair<RPG_Common_SubClass, RPG_Magic_ClassLevelSpellLevelPair_t> RPG_Magic_SubClassLevelPair_t;
  typedef std::map<RPG_Magic_SubClassLevelPair_t, ACE_UINT8> RPG_Magic_NumSpellsTable_t;
  typedef RPG_Magic_NumSpellsTable_t::const_iterator RPG_Magic_NumSpellsTableIterator_t;
  static RPG_Magic_NumSpellsTable_t myNumSpellsTable;
  static RPG_Magic_NumSpellsTable_t myNumSpellsKnownTable;
};

#endif
