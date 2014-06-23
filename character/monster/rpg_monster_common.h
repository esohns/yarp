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

#ifndef RPG_MONSTER_COMMON_H
#define RPG_MONSTER_COMMON_H

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include <rpg_common_environment_incl.h>
#include <rpg_character_incl.h>
#include <rpg_magic_incl.h>
#include <rpg_item_incl.h>
#include <rpg_combat_incl.h>
#include "rpg_monster_incl.h"

#include "rpg_monster.h"

#include "rpg_character_skills_common.h"

#include <string>
#include <vector>
#include <set>
#include <map>

typedef std::set<RPG_Common_CreatureSubType> RPG_Monster_SubTypes_t;
typedef RPG_Monster_SubTypes_t::const_iterator RPG_Monster_SubTypesIterator_t;

typedef RPG_Dice_Roll RPG_Monster_HitDice;

typedef std::vector<RPG_Monster_AttackAction> RPG_Monster_AttackActions_t;
typedef RPG_Monster_AttackActions_t::const_iterator RPG_Monster_AttackActionsIterator_t;
typedef std::vector<RPG_Monster_SpecialAttackProperties> RPG_Monster_SpecialAttackActions_t;
typedef RPG_Monster_SpecialAttackActions_t::const_iterator RPG_Monster_SpecialAttackActionsIterator_t;

typedef std::vector<RPG_Monster_OrganizationStep> RPG_Monster_Organizations_t;
typedef RPG_Monster_Organizations_t::const_iterator RPG_Monster_OrganizationsIterator_t;
typedef std::set<RPG_Monster_Organization> RPG_Monster_OrganizationSet_t;
typedef RPG_Monster_OrganizationSet_t::const_iterator RPG_Monster_OrganizationSetIterator_t;

typedef std::vector<RPG_Monster_AdvancementStep> RPG_Monster_Advancement_t;
typedef RPG_Monster_Advancement_t::const_iterator RPG_Monster_AdvancementIterator_t;

struct RPG_Monster_Properties
{
//   std::string name;
  RPG_Monster_Size size;
  RPG_Common_CreatureType type;
  RPG_Monster_HitDice hitDice;
  char initiative;
  unsigned char speed;
  RPG_Monster_NaturalArmorClass armorClass;
  RPG_Monster_Attack attack;
  std::vector<RPG_Monster_SpecialAttackProperties> specialAttacks;
  unsigned char space; // feet
  unsigned short reach; // feet
  RPG_Monster_SavingThrowModifiers saves;
  RPG_Character_Attributes attributes;
  RPG_Character_Skills_t skills;
  RPG_Character_Feats_t feats;
  RPG_Common_Environment environment;
  RPG_Monster_Organizations_t organizations;
  unsigned char challengeRating;
  unsigned char treasureModifier; // standard times x
  RPG_Character_Alignment alignment;
  RPG_Monster_Advancement_t advancements;
  unsigned char levelAdjustment;
};

typedef std::vector<RPG_Monster_Spawn> RPG_Monster_Spawns_t;
typedef RPG_Monster_Spawns_t::const_iterator RPG_Monster_SpawnsConstIterator_t;

typedef std::vector<RPG_Monster*> RPG_Monster_Group_t;
typedef RPG_Monster_Group_t::const_iterator RPG_Monster_GroupIterator_t;
typedef std::vector<RPG_Monster_Group_t> RPG_Monster_Groups_t;
typedef RPG_Monster_Groups_t::const_iterator RPG_Monster_GroupsIterator_t;

// some more useful types
typedef std::map<std::string, RPG_Monster_Properties> RPG_Monster_Dictionary_t;
typedef std::vector<std::string> RPG_Monster_List_t;
typedef RPG_Monster_List_t::const_iterator RPG_Monster_ListConstIterator_t;
typedef std::map<std::string, unsigned short int> RPG_Monster_Encounter_t;
typedef RPG_Monster_Encounter_t::const_iterator RPG_Monster_EncounterConstIterator_t;
typedef RPG_Monster_Encounter_t::iterator RPG_Monster_EncounterIterator_t;

#endif
