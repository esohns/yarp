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
#ifndef RPG_CHARACTER_MONSTER_COMMON_TOOLS_H
#define RPG_CHARACTER_MONSTER_COMMON_TOOLS_H

#include <rpg_dice_incl.h>

#include <rpg_common.h>
#include <rpg_common_incl.h>
#include <rpg_common_environment_incl.h>
#include <rpg_item_weapontype.h>
#include <rpg_character_incl.h>
#include <rpg_magic_incl.h>
#include <rpg_combat_incl.h>

#include "rpg_character_monster_incl.h"

#include "rpg_character_monster_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Monster_Common_Tools
{
 public:
  static void initStringConversionTables();

  static const std::string weaponTypeToString(const RPG_Character_Monster_WeaponTypeUnion&); // weapon type
  static const std::string attackToString(const RPG_Character_Monster_Attack&); // attack
  static const std::string organizationsToString(const RPG_Character_Monster_Organizations_t&); // organizations
  static const std::string organizationsToString(const RPG_Character_Monster_OrganizationSet_t&); // organizations
  static const std::string advancementToString(const RPG_Character_Monster_Advancement_t&); // advancement
  static const RPG_Common_PhysicalDamageList_t naturalWeaponToPhysicalDamageType(const RPG_Character_Monster_NaturalWeapon&);

  static void generateRandomEncounter(const unsigned int&,                  // # of different monster types
                                      const unsigned int&,                  // total # of foes (0: random)
                                      const RPG_Character_Alignment&,       // alignment
                                      const RPG_Common_Environment&,        // environment
                                      const RPG_Character_Monster_OrganizationSet_t&, // organization(s)
                                      RPG_Character_Monster_Encounter_t&);            // return value: encounter

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Monster_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools(const RPG_Character_Monster_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools& operator=(const RPG_Character_Monster_Common_Tools&));

  // helper methods
  static const std::string attackActionToString(const RPG_Character_Monster_AttackAction&); // attack action
  static void organizationStepToRoll(const RPG_Character_Monster_OrganizationStep&, // organization step
                                     RPG_Dice_Roll&);                     // return value: roll
};

#endif
