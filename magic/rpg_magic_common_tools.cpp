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
#include "stdafx.h"

// *NOTE*: workaround quirky MSVC...
#if defined (_MSC_VER)
#define NOMINMAX
#endif // _MSC_VER

#include "rpg_magic_common_tools.h"

#include <string>
#include <sstream>

#include "ace/Log_Msg.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_dice_common_tools.h"

#include "rpg_character_common_tools.h"

#include "rpg_magic_defines.h"
#include "rpg_magic_dictionary.h"

// initialize statics
RPG_Magic_SchoolToStringTable_t RPG_Magic_SchoolHelper::myRPG_Magic_SchoolToStringTable;
RPG_Magic_SubSchoolToStringTable_t RPG_Magic_SubSchoolHelper::myRPG_Magic_SubSchoolToStringTable;
RPG_Magic_DescriptorToStringTable_t RPG_Magic_DescriptorHelper::myRPG_Magic_DescriptorToStringTable;
RPG_Magic_DomainToStringTable_t RPG_Magic_DomainHelper::myRPG_Magic_DomainToStringTable;
RPG_Magic_SpellTypeToStringTable_t RPG_Magic_SpellTypeHelper::myRPG_Magic_SpellTypeToStringTable;
RPG_Magic_AbilityClassToStringTable_t RPG_Magic_AbilityClassHelper::myRPG_Magic_AbilityClassToStringTable;
RPG_Magic_AbilityTypeToStringTable_t RPG_Magic_AbilityTypeHelper::myRPG_Magic_AbilityTypeToStringTable;
RPG_Magic_Spell_RangeEffectToStringTable_t RPG_Magic_Spell_RangeEffectHelper::myRPG_Magic_Spell_RangeEffectToStringTable;
RPG_Magic_Spell_TargetToStringTable_t RPG_Magic_Spell_TargetHelper::myRPG_Magic_Spell_TargetToStringTable;
RPG_Magic_Spell_AreaOfEffectToStringTable_t RPG_Magic_Spell_AreaOfEffectHelper::myRPG_Magic_Spell_AreaOfEffectToStringTable;
RPG_Magic_Spell_DurationToStringTable_t RPG_Magic_Spell_DurationHelper::myRPG_Magic_Spell_DurationToStringTable;
RPG_Magic_Spell_PreconditionToStringTable_t RPG_Magic_Spell_PreconditionHelper::myRPG_Magic_Spell_PreconditionToStringTable;
RPG_Magic_Spell_EffectToStringTable_t RPG_Magic_Spell_EffectHelper::myRPG_Magic_Spell_EffectToStringTable;

RPG_Magic_Common_Tools::RPG_Magic_NumSpellsTable_t RPG_Magic_Common_Tools::myNumSpellsTable;
RPG_Magic_Common_Tools::RPG_Magic_NumSpellsTable_t RPG_Magic_Common_Tools::myNumSpellsKnownTable;

void
RPG_Magic_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::initializeStringConversionTables"));

  RPG_Magic_SchoolHelper::init ();
  RPG_Magic_SubSchoolHelper::init ();
  RPG_Magic_DescriptorHelper::init ();
  RPG_Magic_DomainHelper::init ();
  RPG_Magic_SpellTypeHelper::init ();
  RPG_Magic_AbilityClassHelper::init ();
  RPG_Magic_AbilityTypeHelper::init ();
  RPG_Magic_Spell_RangeEffectHelper::init ();
  RPG_Magic_Spell_TargetHelper::init ();
  RPG_Magic_Spell_AreaOfEffectHelper::init ();
  RPG_Magic_Spell_DurationHelper::init ();
  RPG_Magic_Spell_PreconditionHelper::init ();
  RPG_Magic_Spell_EffectHelper::init ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Magic_Common_Tools: initialized string conversion tables...\n")));
}

void
RPG_Magic_Common_Tools::initializeSpellsTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::initializeSpellsTables"));

  // SUBCLASS_BARD
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(1, 0)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(2, 0)), 3));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(2, 1)), 0));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(3, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(3, 1)), 1));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(4, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(4, 1)), 2));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(4, 2)), 0));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(5, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(5, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(5, 2)), 1));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(6, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(6, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(6, 2)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 2)), 2));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 3)), 0));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 3)), 1));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 3)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 3)), 2));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 4)), 0));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 3)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 4)), 1));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 3)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 4)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 0)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 3)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 4)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 5)), 0));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 1)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 3)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 4)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 5)), 1));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 1)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 2)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 3)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 4)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 5)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 1)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 2)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 3)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 4)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 5)), 2));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 6)), 0));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 1)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 2)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 3)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 4)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 5)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 6)), 1));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 1)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 2)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 3)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 4)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 5)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 6)), 2));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 1)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 2)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 3)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 4)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 5)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 6)), 3));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 0)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 1)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 2)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 3)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 4)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 5)), 4));
  myNumSpellsTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 6)), 4));

  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(1, 0)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(2, 0)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(2, 1)), 2));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(3, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(3, 1)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(4, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(4, 1)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(4, 2)), 2));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(5, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(5, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(5, 2)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(6, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(6, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(6, 2)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(7, 3)), 2));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(8, 3)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(9, 3)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(10, 4)), 2));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(11, 4)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(12, 4)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 4)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(13, 5)), 2));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 4)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(14, 5)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 1)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 4)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(15, 5)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 1)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 2)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 3)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 4)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 5)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(16, 6)), 2));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 1)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 2)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 3)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 4)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 5)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(17, 6)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 1)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 2)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 3)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 4)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 5)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(18, 6)), 3));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 1)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 2)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 3)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 4)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 5)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(19, 6)), 4));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 0)), 6));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 1)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 2)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 3)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 4)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 5)), 5));
  myNumSpellsKnownTable.insert (std::make_pair(std::make_pair(SUBCLASS_BARD, std::make_pair(20, 6)), 4));

  // SUBCLASS_CLERIC
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(1, 0)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(1, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(2, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(2, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(3, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(3, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(3, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(4, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(4, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(4, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(5, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(5, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(5, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(5, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(6, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(6, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(6, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(6, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(7, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(7, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(7, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(7, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(7, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(8, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(8, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(8, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(8, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(8, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(9, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(9, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(9, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(9, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(9, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(9, 5)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(10, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(10, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(10, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(10, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(10, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(10, 5)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 5)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(11, 6)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(12, 6)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 6)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(13, 7)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(14, 7)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 7)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(15, 8)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(16, 8)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 8)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(17, 9)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(18, 9)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 5)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(19, 9)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 5)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 8)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_CLERIC, std::make_pair(20, 9)), 4));

  // SUBCLASS_DRUID
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(1, 0)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(1, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(2, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(2, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(3, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(3, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(3, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(4, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(4, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(4, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(5, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(5, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(5, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(5, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(6, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(6, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(6, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(6, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(7, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(7, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(7, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(7, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(7, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(8, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(8, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(8, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(8, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(8, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(9, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(9, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(9, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(9, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(9, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(9, 5)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(10, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(10, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(10, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(10, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(10, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(10, 5)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 5)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(11, 6)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(12, 6)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 6)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(13, 7)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(14, 7)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 7)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(15, 8)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(16, 8)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 8)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(17, 9)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(18, 9)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 5)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(19, 9)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 5)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 8)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_DRUID, std::make_pair(20, 9)), 4));

  // SUBCLASS_PALADIN
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(4, 1)), 0));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(5, 1)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(6, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(7, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(8, 1)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(8, 2)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(9, 1)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(9, 2)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(10, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(10, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(11, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(11, 2)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(11, 3)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(12, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(12, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(12, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(13, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(13, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(13, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(14, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(14, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(14, 3)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(14, 4)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(15, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(15, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(15, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(15, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(16, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(16, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(16, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(16, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(17, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(17, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(17, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(17, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(18, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(18, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(18, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(18, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(19, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(19, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(19, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(19, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(20, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(20, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(20, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_PALADIN, std::make_pair(20, 4)), 3));

  // SUBCLASS_RANGER
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(4, 1)), 0));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(5, 1)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(6, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(7, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(8, 1)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(8, 2)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(9, 1)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(9, 2)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(10, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(10, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(11, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(11, 2)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(11, 3)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(12, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(12, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(12, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(13, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(13, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(13, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(14, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(14, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(14, 3)), 1));
//   myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(14, 4)), 0));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(15, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(15, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(15, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(15, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(16, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(16, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(16, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(16, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(17, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(17, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(17, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(17, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(18, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(18, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(18, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(18, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(19, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(19, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(19, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(19, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(20, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(20, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(20, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_RANGER, std::make_pair(20, 4)), 3));

  // SUBCLASS_SORCERER
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(1, 0)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(1, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(2, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(2, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(3, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(3, 1)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(4, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(4, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(4, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(5, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(5, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(5, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 2)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 3)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 4)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 5)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 6)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 6)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 6)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 7)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 6)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 7)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 8)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 6)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 7)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 8)), 5));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 9)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 6)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 7)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 8)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 9)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 0)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 1)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 2)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 3)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 4)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 5)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 6)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 7)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 8)), 6));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 9)), 6));

  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(1, 0)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(1, 1)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(2, 0)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(2, 1)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(3, 0)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(3, 1)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(4, 0)), 6));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(4, 1)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(4, 2)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(5, 0)), 6));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(5, 1)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(5, 2)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 0)), 7));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 1)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 2)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(6, 3)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 0)), 7));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 2)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(7, 3)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 0)), 8));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 2)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 3)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(8, 4)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 0)), 8));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 2)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 3)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(9, 4)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 2)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 3)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 4)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(10, 5)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 4)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(11, 5)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 4)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 5)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(12, 6)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 5)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(13, 6)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 5)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 6)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(14, 7)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 5)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 6)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(15, 7)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 5)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 6)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 7)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(16, 8)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 5)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 6)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 7)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(17, 8)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 3)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 5)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 6)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 7)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 8)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(18, 9)), 1));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 5)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 6)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 7)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 8)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(19, 9)), 2));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 0)), 9));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 1)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 2)), 5));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 3)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 4)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 5)), 4));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 6)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 4)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 5)), 3));
  myNumSpellsKnownTable.insert(std::make_pair(std::make_pair(SUBCLASS_SORCERER, std::make_pair(20, 6)), 3));

  // SUBCLASS_WIZARD
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(1, 0)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(1, 1)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(2, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(2, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(3, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(3, 1)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(3, 2)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(4, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(4, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(4, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(5, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(5, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(5, 2)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(5, 3)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(6, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(6, 1)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(6, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(6, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(7, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(7, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(7, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(7, 3)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(7, 4)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(8, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(8, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(8, 2)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(8, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(8, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(9, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(9, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(9, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(9, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(9, 4)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(9, 5)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(10, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(10, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(10, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(10, 3)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(10, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(10, 5)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 5)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(11, 6)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 4)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(12, 6)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 6)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(13, 7)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 5)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(14, 7)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 7)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(15, 8)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 6)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(16, 8)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 8)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(17, 9)), 1));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 7)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(18, 9)), 2));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 8)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(19, 9)), 3));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 0)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 1)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 2)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 3)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 4)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 5)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 6)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 7)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 8)), 4));
  myNumSpellsTable.insert(std::make_pair(std::make_pair(SUBCLASS_WIZARD, std::make_pair(20, 9)), 4));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Magic_Common_Tools: initialized spells tables...\n")));
}

void
RPG_Magic_Common_Tools::initialize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::initialize"));

  initializeStringConversionTables ();
  initializeSpellsTables ();
}

std::string
RPG_Magic_Common_Tools::spellToName (enum RPG_Magic_SpellType type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::spellToName"));

  std::string result;

  struct RPG_Magic_Spell_Properties properties =
    RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpellProperties (type_in,
                                                                     result);

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const struct RPG_Magic_Spell_Type& type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result =
    RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (type_in.type);
  result += ACE_TEXT_ALWAYS_CHAR (" (");
  result += RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString (type_in.school);
  if (type_in.subSchool != RPG_MAGIC_SUBSCHOOL_INVALID)
  {
    result += ACE_TEXT_ALWAYS_CHAR (" / ");
    result += RPG_Magic_SubSchoolHelper::RPG_Magic_SubSchoolToString (type_in.subSchool);
  } // end IF
  result += ACE_TEXT_ALWAYS_CHAR (")");
  if (!type_in.descriptors.empty ())
  {
    result += ACE_TEXT_ALWAYS_CHAR (": ");
    for (std::vector<RPG_Magic_Descriptor>::const_iterator iterator = type_in.descriptors.begin ();
         iterator != type_in.descriptors.end ();
         iterator++)
    {
      result += RPG_Magic_DescriptorHelper::RPG_Magic_DescriptorToString (*iterator);
      result += ACE_TEXT_ALWAYS_CHAR (",");
    } // end FOR
    result.erase (--(result.end ()));
  } // end IF

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_SpellLevels_t& levels_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;
  std::ostringstream converter;
  for (RPG_Magic_SpellLevelsIterator_t iterator = levels_in.begin ();
       iterator != levels_in.end ();
       iterator++)
  {
    switch ((*iterator).casterClass.discriminator)
    {
      case RPG_Magic_CasterClassUnion::SUBCLASS:
      {
        result += RPG_Common_SubClassHelper::RPG_Common_SubClassToString ((*iterator).casterClass.subclass);
        break;
      }
      // *PORTABILITY*: "DOMAIN" seems to be a constant (see math.h)
      // --> provide a (temporary) workaround here...
#if defined (__GNUC__) || defined (_MSC_VER)
#pragma message("applying quirk code for this compiler...")
      case RPG_Magic_CasterClassUnion::__QUIRK__DOMAIN:
#else
#pragma message("re-check code for this compiler")
      case RPG_Magic_CasterClassUnion::DOMAIN:
#endif // __GNUC__ || _MSC_VER
      {
        result += RPG_Magic_DomainHelper::RPG_Magic_DomainToString ((*iterator).casterClass.domain);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid caster class (was: %d), continuing\n"),
                    (*iterator).casterClass.discriminator));
        break;
      }
    } // end SWITCH

    result += ACE_TEXT_ALWAYS_CHAR (": ");
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << static_cast<unsigned int> ((*iterator).level);
    result += converter.str ();
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end FOR

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const struct RPG_Magic_Spell_RangeProperties& range_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;

  std::ostringstream converter;
  RPG_Magic_Spell_RangeProperties temp = range_in;
  if ((range_in.maximum == 0) || (range_in.increment == 0))
    updateSpellRange (temp);

  switch (temp.effect)
  {
    case RANGEEFFECT_PERSONAL:
    case RANGEEFFECT_TOUCH:
    {
      result += ACE_TEXT_ALWAYS_CHAR ("effect: ");
      result += RPG_Magic_Spell_RangeEffectHelper::RPG_Magic_Spell_RangeEffectToString (temp.effect);
      result += ACE_TEXT_ALWAYS_CHAR ("\n");
      break;
    }
    case RANGEEFFECT_CLOSE:
    case RANGEEFFECT_MEDIUM:
    case RANGEEFFECT_LONG:
    case RANGEEFFECT_UNLIMITED:
    case RANGEEFFECT_RANGED:
    {
      result += ACE_TEXT_ALWAYS_CHAR ("maximum: ");
      converter << temp.maximum;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" ft\n");
      result += ACE_TEXT_ALWAYS_CHAR ("increment: ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << temp.increment;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" ft\n");
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid effect \"%s\" --> check implementation !, returning\n"),
                  ACE_TEXT (RPG_Magic_Spell_RangeEffectHelper::RPG_Magic_Spell_RangeEffectToString (temp.effect).c_str ())));
      break;
    }
  } // end SWITCH

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_Spell_Targets_t& targets_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;
  std::ostringstream converter;

  for (RPG_Magic_Spell_TargetsIterator_t iterator = targets_in.begin ();
       iterator != targets_in.end ();
       iterator++)
  {
    result += ACE_TEXT_ALWAYS_CHAR ("target: ");
    result += RPG_Magic_Spell_TargetHelper::RPG_Magic_Spell_TargetToString ((*iterator).target);
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
    switch ((*iterator).target)
    {
      case TARGET_SELF:
      case TARGET_SINGLE:
      case TARGET_LOCATION:
        break;
      case TARGET_SEVERAL:
      {
        result += ACE_TEXT_ALWAYS_CHAR ("#: ");
        if ((*iterator).base.range.typeDice == RPG_DICE_DIETYPE_INVALID)
        {
          converter << (*iterator).base.value;
          result += converter.str ();
        } // end IF
        else
          result += RPG_Dice_Common_Tools::toString ((*iterator).base.range).c_str ();
        if ((*iterator).rangeIsInHD)
          result += ACE_TEXT_ALWAYS_CHAR (" HD");
        if ((*iterator).levelIncrement)
        {
          result += ACE_TEXT_ALWAYS_CHAR (" + ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<unsigned int> ((*iterator).levelIncrement);
          result += converter.str ();
          result += ACE_TEXT_ALWAYS_CHAR (" / [casterLevel]");
        } // end IF
        result += ACE_TEXT_ALWAYS_CHAR ("\n");
        break;
      }
      case TARGET_AREA:
      {
        if ((*iterator).effect != RPG_MAGIC_SPELL_AREAOFEFFECT_INVALID)
        {
          result += ACE_TEXT_ALWAYS_CHAR ("effect: ");
          result +=
            RPG_Magic_Spell_AreaOfEffectHelper::RPG_Magic_Spell_AreaOfEffectToString ((*iterator).effect);
          result += ACE_TEXT_ALWAYS_CHAR ("\n");
        } // end IF
        if ((*iterator).shape != RPG_COMMON_AREAOFEFFECT_INVALID)
        {
          result += ACE_TEXT_ALWAYS_CHAR ("shape: ");
          result +=
            RPG_Common_AreaOfEffectHelper::RPG_Common_AreaOfEffectToString ((*iterator).shape);
          result += ACE_TEXT_ALWAYS_CHAR ("\n");
        } // end IF
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid target \"%s\", returning\n"),
                    ACE_TEXT (RPG_Magic_Spell_TargetHelper::RPG_Magic_Spell_TargetToString ((*iterator).target).c_str ())));
        break;
      }
    } // end SWITCH
    if ((*iterator).radius)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("radius: ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << static_cast<unsigned int> ((*iterator).radius);
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" ft\n");
      if ((*iterator).height)
      {
        result += ACE_TEXT_ALWAYS_CHAR ("height: ");
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << static_cast<unsigned int> ((*iterator).height);
        result += converter.str ();
        result += ACE_TEXT_ALWAYS_CHAR (" ft\n");
      } // end IF
    } // end IF
  } // end FOR

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const struct RPG_Magic_Spell_DurationProperties& duration_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result = RPG_Magic_Spell_DurationHelper::RPG_Magic_Spell_DurationToString (duration_in.duration);

  switch (duration_in.duration)
  {
    case DURATION_CONCENTRATION:
    case DURATION_DISCHARGE:
    case DURATION_INSTANTANEOUS:
    case DURATION_PERMANENT:
      break;
    case DURATION_TIMED:
    {
      ACE_ASSERT ((duration_in.base.range.typeDice != RPG_DICE_DIETYPE_INVALID) ||
                  (duration_in.base.value != 0));

      std::ostringstream converter;
      result += ACE_TEXT_ALWAYS_CHAR (", ");
      if (duration_in.base.range.typeDice != RPG_DICE_DIETYPE_INVALID)
        result += RPG_Dice_Common_Tools::toString (duration_in.base.range);
      else
      {
        converter << duration_in.base.value;
        result += converter.str ();
      } // end ELSE
      result += ACE_TEXT_ALWAYS_CHAR (" rd(s)");

      if (duration_in.levelIncrement)
      {
        result += ACE_TEXT_ALWAYS_CHAR (" ");

        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        // show sign in output
        converter.setf (ios::showpos);
        if (duration_in.reciprocalIncrement)
        {
          // show fixed point notation, single digit precision
          converter.setf (ios::fixed);
          converter.precision (1);
          converter << duration_in.levelIncrement / static_cast<float> (duration_in.reciprocalIncrement);
        } // end IF
        else
          converter << static_cast<int> (duration_in.levelIncrement);
        result += converter.str ();
        result += ACE_TEXT_ALWAYS_CHAR (" rd(s) / casterLevel");
        if (duration_in.levelIncrementMax)
        {
          result += ACE_TEXT_ALWAYS_CHAR (" (max: ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<unsigned int> (duration_in.levelIncrementMax);
          result += converter.str ();
          result += ACE_TEXT_ALWAYS_CHAR ("th)");
        } // end IF
      } // end IF
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid duration type (was: \"%s\"), continuing\n"),
                  ACE_TEXT (RPG_Magic_Spell_DurationHelper::RPG_Magic_Spell_DurationToString (duration_in.duration).c_str ())));
      break;
    }
  } // end SWITCH
  if (duration_in.dismissible)
    result += ACE_TEXT_ALWAYS_CHAR (", dismissible");

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_Spell_Preconditions_t& preconditions_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;

  std::ostringstream converter;
  for (RPG_Magic_Spell_PreconditionsIterator_t iterator = preconditions_in.begin ();
       iterator != preconditions_in.end ();
       iterator++)
  {
    if ((*iterator).reverse)
      result += ACE_TEXT_ALWAYS_CHAR ("!");
    result += RPG_Magic_Spell_PreconditionHelper::RPG_Magic_Spell_PreconditionToString ((*iterator).type);
    switch ((*iterator).type)
    {
      case PRECONDITION_ALLY:
      case PRECONDITION_ATTACK_MELEE_ANY:
      case PRECONDITION_ATTACK_MELEE_RANGED_TOUCH:
      case PRECONDITION_CASTER_PLANE:
      case PRECONDITION_IMMUNITY_POISON:
      case PRECONDITION_MANUFACTURED:
      case PRECONDITION_NONMAGICAL:
      case PRECONDITION_OBJECT:
      case PRECONDITION_SIGHTLESS:
      {
        break;
      }
      case PRECONDITION_ALIGNMENT:
      case PRECONDITION_ATTRIBUTE_MAX:
      case PRECONDITION_CONDITION:
      case PRECONDITION_ENVIRONMENT:
      case PRECONDITION_HD_MAX:
      case PRECONDITION_HD_MIN:
      case PRECONDITION_SPELLLEVEL_MAX:
      case PRECONDITION_SIZE_MAX:
      case PRECONDITION_SIZE_RELATIVE:
      {
        result += ACE_TEXT_ALWAYS_CHAR (": ");

        if ((*iterator).type == PRECONDITION_ALIGNMENT)
          result += RPG_Character_Common_Tools::toString ((*iterator).alignment);
        else if ((*iterator).type == PRECONDITION_ATTRIBUTE_MAX)
        {
          result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator).attribute);
          result += ACE_TEXT_ALWAYS_CHAR (" <= ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << (*iterator).value;
          result += converter.str ();
        } // end IF
        else if ((*iterator).type == PRECONDITION_CONDITION)
          result += RPG_Common_ConditionHelper::RPG_Common_ConditionToString ((*iterator).condition);
        else if ((*iterator).type == PRECONDITION_ENVIRONMENT)
          result += RPG_Common_Tools::toString ((*iterator).environment);
        else if (((*iterator).type == PRECONDITION_SIZE_MAX) ||
                 ((*iterator).type == PRECONDITION_SIZE_RELATIVE))
          result += RPG_Common_SizeHelper::RPG_Common_SizeToString ((*iterator).size);
        else
        {
          result += ACE_TEXT_ALWAYS_CHAR (" ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << (*iterator).value;
          result += converter.str ();

          if (((*iterator).type == PRECONDITION_HD_MAX) ||
              ((*iterator).type == PRECONDITION_HD_MIN))
            result += ACE_TEXT_ALWAYS_CHAR (" HD ");
        } // end ELSE

        if ((*iterator).levelIncrement)
        {
          result += ACE_TEXT_ALWAYS_CHAR (" / [casterLevel");
          if ((*iterator).levelIncrement != 1)
          {
            result += ACE_TEXT_ALWAYS_CHAR ("/");
            converter.clear ();
            converter.str (ACE_TEXT_ALWAYS_CHAR (""));
            converter << static_cast<unsigned int> ((*iterator).levelIncrement);
            result += converter.str ();
          } // end IF
          result += ACE_TEXT_ALWAYS_CHAR ("]");

          if ((*iterator).levelIncrementMax)
          {
            result += ACE_TEXT_ALWAYS_CHAR (" (max: ");
            converter.clear ();
            converter.str (ACE_TEXT_ALWAYS_CHAR (""));
            converter << static_cast<unsigned int> ((*iterator).levelIncrementMax);
            result += converter.str ();
            result += ACE_TEXT_ALWAYS_CHAR ("th)");
          } // end IF
        } // end IF
        break;
      }
      case PRECONDITION_TYPE:
      {
        for (std::vector<RPG_Common_CreatureType>::const_iterator iterator2 = (*iterator).creatures.begin ();
             iterator2 != (*iterator).creatures.end ();
             iterator2++)
        {
          result += ACE_TEXT_ALWAYS_CHAR ("\n");
          result += RPG_Common_Tools::toString (*iterator2);
        } // end FOR
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid precondition \"%s\" --> check implementation !, continuing\n"),
                    ACE_TEXT (RPG_Magic_Spell_PreconditionHelper::RPG_Magic_Spell_PreconditionToString ((*iterator).type).c_str ())));
        break;
      }
    } // end SWITCH
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end FOR

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_Spell_Effects_t& effects_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;

  std::ostringstream converter;
  for (RPG_Magic_Spell_EffectsIterator_t iterator = effects_in.begin ();
       iterator != effects_in.end ();
       iterator++)
  {
    if ((*iterator).type == SPELLEFFECT_MODIFIER_ATTRIBUTE)
      result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator).attribute);
    else
      result += RPG_Magic_Spell_EffectHelper::RPG_Magic_Spell_EffectToString ((*iterator).type);
    if ((*iterator).base.value ||
        ((*iterator).base.range.typeDice != RPG_DICE_DIETYPE_INVALID))
    {
      result += ACE_TEXT_ALWAYS_CHAR (": ");
      if ((*iterator).base.range.typeDice == RPG_DICE_DIETYPE_INVALID)
      {
        if ((*iterator).base.value > 0)
          result += ACE_TEXT_ALWAYS_CHAR ("+");
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << static_cast<int> ((*iterator).base.value);
        result += converter.str ();
      } // end IF
      else
      {
        result += RPG_Dice_Common_Tools::toString ((*iterator).base.range);
      } // end ELSE
      if (((*iterator).levelIncrement.value) ||
          ((*iterator).levelIncrement.range.typeDice != RPG_DICE_DIETYPE_INVALID))
      {
        result += ACE_TEXT_ALWAYS_CHAR (" [+ ");
        if ((*iterator).levelIncrement.range.typeDice == RPG_DICE_DIETYPE_INVALID)
        {
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<int> ((*iterator).levelIncrement.value);
          result += converter.str ();
        } // end IF
        else
          result += RPG_Dice_Common_Tools::toString ((*iterator).levelIncrement.range);

        if ((*iterator).levelIncrementMax)
        {
          result += ACE_TEXT_ALWAYS_CHAR (" / casterLevel (max: ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<unsigned int> ((*iterator).levelIncrementMax);
          result += converter.str ();
          result += ACE_TEXT_ALWAYS_CHAR ("th)]");
        } // end IF
        else
          result += ACE_TEXT_ALWAYS_CHAR (" / casterLevel]");
      } // end IF
    } // end IF
    if ((*iterator).type == SPELLEFFECT_DAMAGE)
    {
      result += ACE_TEXT_ALWAYS_CHAR (" [");
      switch ((*iterator).damage.discriminator)
      {
        case RPG_Magic_Spell_DamageTypeUnion::DESCRIPTOR:
        {
          result += RPG_Magic_DescriptorHelper::RPG_Magic_DescriptorToString ((*iterator).damage.descriptor);
          break;
        }
        case RPG_Magic_Spell_DamageTypeUnion::PHYSICALDAMAGETYPE:
        {
          result += RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString ((*iterator).damage.physicaldamagetype);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid damage type (was: %d, continuing\n"),
                      (*iterator).damage.discriminator));
          break;
        }
      } // end SWITCH
      result += ACE_TEXT_ALWAYS_CHAR ("]");
    } // end IF
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
    if ((*iterator).maxRange)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("range: ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << static_cast<unsigned int> ((*iterator).maxRange);
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" ft\n");
    } // end IF
    for (std::vector<RPG_Magic_CounterMeasure>::const_iterator iterator2 = (*iterator).counterMeasures.begin ();
         iterator2 != (*iterator).counterMeasures.end ();
         iterator2++)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("counterMeasure: ");
//       result += RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString((*iterator2).type);
      switch ((*iterator2).type)
      {
        case COUNTERMEASURE_CHECK:
        {
//           result += ACE_TEXT_ALWAYS_CHAR(": ");
          switch ((*iterator2).check.type.discriminator)
          {
            case RPG_Magic_CheckTypeUnion::SKILL:
            {
              result += RPG_Common_SkillHelper::RPG_Common_SkillToString ((*iterator2).check.type.skill);
              break;
            }
            case RPG_Magic_CheckTypeUnion::ATTRIBUTE:
            {
              result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator2).check.type.attribute);
              break;
            }
            case RPG_Magic_CheckTypeUnion::BASECHECKTYPEUNION:
            {
              switch ((*iterator2).check.type.basechecktypeunion.discriminator)
              {
                case RPG_Common_BaseCheckTypeUnion::CHECKTYPE:
                {
                  result += RPG_Common_CheckTypeHelper::RPG_Common_CheckTypeToString ((*iterator2).check.type.basechecktypeunion.checktype);
                  break;
                }
                case RPG_Common_BaseCheckTypeUnion::SAVINGTHROW:
                {
                  result += RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString ((*iterator2).check.type.basechecktypeunion.savingthrow);
                  break;
                }
                default:
                {
                  ACE_DEBUG ((LM_ERROR,
                              ACE_TEXT ("invalid RPG_Common_BaseCheckTypeUnion type: %d, continuing\n"),
                              (*iterator2).check.type.basechecktypeunion.discriminator));
                  break;
                }
              } // end SWITCH

              break;
            }
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid RPG_Magic_CheckTypeUnion type: %d, continuing\n"),
                          (*iterator2).check.type.discriminator));
              break;
            }
          } // end SWITCH
          if ((*iterator2).check.difficultyClass)
          {
            result += ACE_TEXT_ALWAYS_CHAR (" (DC: ");
            converter.clear ();
            converter.str (ACE_TEXT_ALWAYS_CHAR (""));
            converter << static_cast<unsigned int> ((*iterator2).check.difficultyClass);
            result += converter.str ();
            result += ACE_TEXT_ALWAYS_CHAR (")");
          } // end IF

          break;
        }
        case COUNTERMEASURE_SPELL:
        {
          for (std::vector<RPG_Magic_SpellType>::const_iterator iterator3 = (*iterator2).spells.begin ();
               iterator3 != (*iterator2).spells.end ();
               iterator3++)
          {
            result += RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator3);
            result += ACE_TEXT_ALWAYS_CHAR ("|");
          } // end FOR
          if (!(*iterator2).spells.empty ())
            result.erase (--(result.end ()));
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid counterMeasure \"%s\" --> check implementation !, continuing\n"),
                      ACE_TEXT (RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString ((*iterator2).type).c_str ())));
          break;
        }
      } // end SWITCH
      if ((*iterator2).reduction != SAVEREDUCTION_NEGATES)
      {
        result += ACE_TEXT_ALWAYS_CHAR (" [");
        result += RPG_Common_SaveReductionTypeHelper::RPG_Common_SaveReductionTypeToString ((*iterator2).reduction);
        result += ACE_TEXT_ALWAYS_CHAR ("]");
      } // end IF
      result += ACE_TEXT_ALWAYS_CHAR ("\n");
    } // end FOR
  } // end FOR

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_Spell_CounterMeasures_t& counterMeasures_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;

  std::ostringstream converter;
  for (RPG_Magic_Spell_CounterMeasuresIterator_t iterator = counterMeasures_in.begin ();
       iterator != counterMeasures_in.end ();
       iterator++)
  {
    result += ACE_TEXT_ALWAYS_CHAR ("counterMeasure: ");
    result += RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString ((*iterator).type);
    switch ((*iterator).type)
    {
      case COUNTERMEASURE_CHECK:
      {
        result += ACE_TEXT_ALWAYS_CHAR (": ");
        switch ((*iterator).check.type.discriminator)
        {
          case RPG_Magic_CheckTypeUnion::SKILL:
          {
            result += RPG_Common_SkillHelper::RPG_Common_SkillToString ((*iterator).check.type.skill);
            break;
          }
          case RPG_Magic_CheckTypeUnion::ATTRIBUTE:
          {
            result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator).check.type.attribute);
            break;
          }
          case RPG_Magic_CheckTypeUnion::BASECHECKTYPEUNION:
          {
            switch ((*iterator).check.type.basechecktypeunion.discriminator)
            {
              case RPG_Common_BaseCheckTypeUnion::CHECKTYPE:
              {
                result += RPG_Common_CheckTypeHelper::RPG_Common_CheckTypeToString ((*iterator).check.type.basechecktypeunion.checktype);
                break;
              }
              case RPG_Common_BaseCheckTypeUnion::SAVINGTHROW:
              {
                result += RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString ((*iterator).check.type.basechecktypeunion.savingthrow);
                break;
              }
              default:
              {
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("invalid RPG_Common_BaseCheckTypeUnion type: %d, continuing\n"),
                            (*iterator).check.type.basechecktypeunion.discriminator));
                break;
              }
            } // end SWITCH

            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid RPG_Magic_CheckTypeUnion type: %d, continuing\n"),
                        (*iterator).check.type.discriminator));
            break;
          }
        } // end SWITCH
        if ((*iterator).check.difficultyClass)
        {
          result += ACE_TEXT_ALWAYS_CHAR(" (DC: ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<unsigned int> ((*iterator).check.difficultyClass);
          result += converter.str ();
          result += ACE_TEXT_ALWAYS_CHAR (")");
        } // end IF
        else if ((*iterator).check.modifier)
        {
          if ((*iterator).check.baseIsCasterLevel)
          {
            result += ACE_TEXT_ALWAYS_CHAR (" (casterLevel ");
            converter.setf (ios::showpos);
          } // end IF
          else
            result += ACE_TEXT_ALWAYS_CHAR (" (modifier: ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<int> ((*iterator).check.modifier);
          result += converter.str ();
          result += ACE_TEXT_ALWAYS_CHAR (")");
          if ((*iterator).check.baseIsCasterLevel)
            converter.unsetf (ios::showpos);
        } // end ELSE
        if ((*iterator).check.levelIncrementMax)
        {
          result += ACE_TEXT_ALWAYS_CHAR (" (maxLevel: ");
          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<unsigned int> ((*iterator).check.levelIncrementMax);
          result += converter.str ();
          result += ACE_TEXT_ALWAYS_CHAR (")");
        } // end IF

        break;
      }
      case COUNTERMEASURE_SPELL:
      {
        for (RPG_Magic_SpellsIterator_t iterator2 = (*iterator).spells.begin ();
             iterator2 != (*iterator).spells.end ();
             iterator2++)
        {
          result += RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator2);
          result += ACE_TEXT_ALWAYS_CHAR ("|");
        } // end FOR
        if (!(*iterator).spells.empty ())
          result.erase (--(result.end ()));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid counterMeasure \"%s\", continuing\n"),
                    ACE_TEXT (RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString ((*iterator).type).c_str ())));
        break;
      }
    } // end SWITCH
    if ((*iterator).reduction != SAVEREDUCTION_NEGATES)
    {
      result += ACE_TEXT_ALWAYS_CHAR (" [");
      result += RPG_Common_SaveReductionTypeHelper::RPG_Common_SaveReductionTypeToString ((*iterator).reduction);
      result += ACE_TEXT_ALWAYS_CHAR ("]");
    } // end IF
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end FOR

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_SpellTypes_t& knownSpells_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;

  for (RPG_Magic_SpellTypesIterator_t iterator = knownSpells_in.begin ();
       iterator != knownSpells_in.end ();
       iterator++)
  {
    result += RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator);
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end FOR

  return result;
}

std::string
RPG_Magic_Common_Tools::toString (const RPG_Magic_Spells_t& spells_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::toString"));

  std::string result;

  RPG_Magic_SpellTypes_t completed;
  unsigned int count = 0;
  std::ostringstream converter;
  RPG_Magic_SpellsIterator_t iterator2;
  for (RPG_Magic_SpellsIterator_t iterator = spells_in.begin ();
       iterator != spells_in.end ();
       iterator++)
  {
    // already treated this type ?
    if (completed.find(*iterator) != completed.end ())
      continue;

    result += RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator);
    // look ahead if we have memorized it several times
    count = 1;
    iterator2 = iterator;
    std::advance (iterator2, 1);
    for (;
         iterator2 != spells_in.end ();
         iterator2++)
    {
      if (*iterator2 == *iterator)
        count++;
    } // end FOR
//     count = spells_in.count(*iterator);

    // remember this type
    completed.insert (*iterator);

    if (count > 1)
    {
      result += ACE_TEXT_ALWAYS_CHAR (": ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << count;
      result += converter.str ();
    } // end IF
    result += ACE_TEXT_ALWAYS_CHAR ("\n");
  } // end FOR

  return result;
}

void
RPG_Magic_Common_Tools::updateSpellRange (struct RPG_Magic_Spell_RangeProperties& range_inout)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::updateSpellRange"));

  switch (range_inout.effect)
  {
    case RANGEEFFECT_PERSONAL:
    case RANGEEFFECT_TOUCH:
    {
      range_inout.maximum = 0;
      range_inout.increment = 0;

      break;
    }
    case RANGEEFFECT_CLOSE:
    {
      range_inout.maximum = 25;
      range_inout.increment = 5; // per casterLevel/2

      break;
    }
    case RANGEEFFECT_MEDIUM:
    {
      range_inout.maximum = 100;
      range_inout.increment = 10; // per casterLevel

      break;
    }
    case RANGEEFFECT_LONG:
    {
      range_inout.maximum = 400;
      range_inout.increment = 40; // per casterLevel

      break;
    }
    case RANGEEFFECT_UNLIMITED:
    {
      range_inout.maximum = std::numeric_limits<unsigned int>::max ();
      range_inout.increment = std::numeric_limits<unsigned int>::max ();

      break;
    }
    case RANGEEFFECT_RANGED:
    {
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid effect \"%s\", returning\n"),
                  ACE_TEXT (RPG_Magic_Spell_RangeEffectHelper::RPG_Magic_Spell_RangeEffectToString (range_inout.effect).c_str ())));
      break;
    }
  } // end SWITCH
}

bool
RPG_Magic_Common_Tools::isCasterClass (enum RPG_Common_SubClass subClass_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::isCasterClass"));

  switch (subClass_in)
  {
    case SUBCLASS_BARD:
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
//     case SUBCLASS_INVOKER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_RANGER:
//     case SUBCLASS_SHAMAN:
    case SUBCLASS_SORCERER:
//     case SUBCLASS_WARLOCK:
    case SUBCLASS_WIZARD:
      return true;
    default:
      break;
  } // end SWITCH

  return false;
}

bool
RPG_Magic_Common_Tools::isDivineCasterClass (enum RPG_Common_SubClass subClass_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::isDivineCasterClass"));

  switch (subClass_in)
  {
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
    case SUBCLASS_PALADIN:
    case SUBCLASS_RANGER:
      return true;
    default:
      break;
  } // end SWITCH

  return false;
}

bool
RPG_Magic_Common_Tools::hasCasterClass (const RPG_Character_Class& class_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::hasCasterClass"));

  for (RPG_Character_SubClassesIterator_t iterator = class_in.subClasses.begin ();
       iterator != class_in.subClasses.end ();
       iterator++)
    if (RPG_Magic_Common_Tools::isCasterClass (*iterator))
      return true;

  return false;
}

bool
RPG_Magic_Common_Tools::hasDivineCasterClass (const RPG_Character_Class& class_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::hasDivineCasterClass"));

  for (RPG_Character_SubClassesIterator_t iterator = class_in.subClasses.begin ();
       iterator != class_in.subClasses.end ();
       iterator++)
    if (RPG_Magic_Common_Tools::isDivineCasterClass (*iterator))
      return true;

  return false;
}

bool
RPG_Magic_Common_Tools::hasArcaneCasterClass (const RPG_Character_Class& class_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::hasArcaneCasterClass"));

  for (RPG_Character_SubClassesIterator_t iterator = class_in.subClasses.begin ();
       iterator != class_in.subClasses.end ();
       iterator++)
    if (RPG_Magic_Common_Tools::isCasterClass (*iterator) &&
        !RPG_Magic_Common_Tools::isDivineCasterClass (*iterator))
      return true;

  return false;
}

ACE_UINT16
RPG_Magic_Common_Tools::getNumKnownSpells (enum RPG_Common_SubClass subClass_in,
                                           RPG_Character_Level_t classLevel_in,
                                           ACE_UINT8 spellLevel_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::getNumKnownSpells"));

  // sanity check(s)
  ACE_ASSERT (classLevel_in <= RPG_COMMON_MAX_CLASS_LEVEL);
  if (!RPG_Magic_Common_Tools::isCasterClass (subClass_in))
    return 0;

  switch (subClass_in)
  {
    case SUBCLASS_BARD:
    case SUBCLASS_SORCERER:
    { // sanity check(s)
      ACE_ASSERT (spellLevel_in <= RPG_COMMON_MAX_SPELL_LEVEL);

      RPG_Magic_ClassLevelSpellLevelPair_t levelPair =
          std::make_pair (classLevel_in, spellLevel_in);
      RPG_Magic_SubClassLevelPair_t combination =
          std::make_pair (subClass_in, levelPair);
      RPG_Magic_NumSpellsTableIterator_t iterator =
          myNumSpellsKnownTable.find (combination);

      return ((iterator != myNumSpellsKnownTable.end ()) ? iterator->second
                                                         : 0);
    }
    case SUBCLASS_WIZARD:
    default:
      return std::numeric_limits<ACE_UINT16>::max (); // ALL
  } // end SWITCH

  return (RPG_MAGIC_DEF_NUM_NEW_SPELLS_PER_LEVEL * classLevel_in);
}

unsigned int
RPG_Magic_Common_Tools::getNumSpells (enum RPG_Common_SubClass subClass_in,
                                      RPG_Character_Level_t classLevel_in,
                                      ACE_UINT8 spellLevel_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Common_Tools::getNumSpells"));

  // sanity check(s)
  ACE_ASSERT (classLevel_in <= RPG_COMMON_MAX_CLASS_LEVEL);
  if (!RPG_Magic_Common_Tools::isCasterClass (subClass_in))
    return 0;

  unsigned int result = 0;
  RPG_Magic_NumSpellsTableIterator_t iterator;
  if (spellLevel_in == std::numeric_limits<ACE_UINT8>::max ())
  {
    for (iterator = myNumSpellsTable.begin ();
         iterator != myNumSpellsTable.end ();
         iterator++)
      if (((*iterator).first.first == subClass_in) &&
          ((*iterator).first.second.first <= classLevel_in) &&
          ((*iterator).first.second.second <= spellLevel_in))
        result += (*iterator).second;
  } // end IF
  else
  {
    RPG_Magic_ClassLevelSpellLevelPair_t levelPair =
        std::make_pair (classLevel_in, spellLevel_in);
    RPG_Magic_SubClassLevelPair_t combination =
        std::make_pair (subClass_in, levelPair);
    iterator = myNumSpellsTable.find (combination);
    if (iterator != myNumSpellsTable.end ())
      result = (*iterator).second;
  } // end ELSE

  return result;
}
