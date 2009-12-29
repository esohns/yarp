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
#ifndef RPG_COMBAT_COMMON_TOOLS_H
#define RPG_COMBAT_COMMON_TOOLS_H

#include <vector>

#include <rpg_dice_dietype.h>
#include <rpg_dice_roll.h>

#include <rpg_item_physicaldamagetype.h>
#include <rpg_item_weapontype.h>

#include <rpg_common_attribute.h>
#include <rpg_common_savingthrow.h>
#include <rpg_common_savingthrowunion.h>

#include "rpg_combat_attackform.h"
#include "rpg_combat_attacksituation.h"
#include "rpg_combat_defensesituation.h"
#include "rpg_combat_specialdamagetype.h"
#include "rpg_combat_damagetypeunion.h"
#include "rpg_combat_attacksavingthrow.h"
#include "rpg_combat_damageeffecttype.h"
#include "rpg_combat_damageelement.h"
#include "rpg_combat_damage.h"
#include "rpg_combat_common.h"

#include <rpg_character_player_common.h>
#include <rpg_character_size.h>

#include <ace/Global_Macros.h>

#include <deque>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Combat_Common_Tools
{
 public:
  static void initStringConversionTables();

  static const std::string attackFormsToString(const RPG_Combat_AttackForms_t&); // attack forms
  static const std::string damageToString(const RPG_Combat_Damage&); // damage

  static const bool isPartyHelpless(const RPG_Character_Party_t&); // party
  static const bool areMonstersHelpless(const RPG_Character_Monsters_t&); // monsters

  static void getCombatantSequence(const RPG_Character_Party_t&,     // party
                                   const RPG_Character_Monsters_t&,  // monsters
                                   RPG_Combat_CombatantSequence_t&); // battle sequence
  static void performCombatRound(const RPG_Combat_AttackSituation&,      // attack situation
                                 const RPG_Combat_DefenseSituation&,     // defense situation
                                 const RPG_Combat_CombatantSequence_t&); // battle sequence

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Combat_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_Common_Tools(const RPG_Combat_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_Common_Tools& operator=(const RPG_Combat_Common_Tools&));

  // helper types
  typedef std::deque<RPG_Combat_CombatantSequenceElement> RPG_Combat_CombatSequenceList_t;
  typedef RPG_Combat_CombatSequenceList_t::iterator RPG_Combat_CombatSequenceListIterator_t;

  static const bool isMonsterGroupHelpless(const RPG_Character_MonsterGroupInstance_t&); // group instance
  static const bool isCharacterHelpless(const RPG_Character_Base* const); // character handle
  static const bool isCharacterDeadOrDying(const RPG_Character_Base* const); // character handle

  static RPG_Combat_AttackForm weaponTypeToAttackForm(const RPG_Item_WeaponType&); // weapon type
  static const signed char getSizeModifier(const RPG_Character_Size&);
  static void attackFoe(const RPG_Character_Base* const,     // attacker
                        RPG_Character_Base* const,           // target
                        const RPG_Combat_AttackSituation&,   // attacker situation
                        const RPG_Combat_DefenseSituation&); // defender situation
};

#endif
