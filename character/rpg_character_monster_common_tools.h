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

#include <vector>

#include <rpg_chance_dicetype.h>
#include <rpg_chance_diceroll.h>
#include <rpg_chance_valuerange.h>

#include "rpg_character_monstermetatype.h"
#include "rpg_character_monstersubtype.h"
#include "rpg_character_monstertype.h"

#include "rpg_character_monsterweapon.h"
#include "rpg_character_attackform.h"
#include "rpg_character_monsterattackaction.h"
#include "rpg_character_monsterattack.h"
#include "rpg_character_organization.h"
#include "rpg_character_size.h"
#include "rpg_character_monsteradvancementstep.h"
#include "rpg_character_monsteradvancement.h"
#include "rpg_character_monster_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Monster_Common_Tools
{
 public:
  static const std::string monsterTypeToString(const RPG_Character_MonsterType&); // type
  static const std::string monsterAttackActionToString(const RPG_Character_MonsterAttackAction&); // attack action
  static const std::string monsterAttackToString(const RPG_Character_MonsterAttack&); // attack
  static const std::string organizationsToString(const RPG_Character_Organizations_t&); // organizations
  static const std::string monsterAdvancementToString(const RPG_Character_MonsterAdvancement_t&); // advancement

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Monster_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools(const RPG_Character_Monster_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools& operator=(const RPG_Character_Monster_Common_Tools&));
};

#endif
