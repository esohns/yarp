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
#ifndef RPG_CHARACTER_MONSTER_COMMON_H
#define RPG_CHARACTER_MONSTER_COMMON_H

#include <string>
#include <vector>
#include <set>
#include <map>

#include <rpg_chance_dicetype.h>
#include <rpg_chance_diceroll.h>
#include <rpg_chance_valuerange.h>

#include "rpg_character_monstersubtype.h"
#include "rpg_character_size.h"
#include "rpg_character_monstermetatype.h"
#include "rpg_character_monstertype.h"
#include "rpg_character_monsterarmorclass.h"
#include "rpg_character_monsterweapon.h"
#include "rpg_character_attackform.h"
#include "rpg_character_monsterattackaction.h"
#include "rpg_character_monsterattack.h"
#include "rpg_character_savingthrowmodifiers.h"
#include "rpg_character_attributes.h"
#include "rpg_character_terrain.h"
#include "rpg_character_climate.h"
#include "rpg_character_environment.h"
#include "rpg_character_organization.h"
#include "rpg_character_organizationstep.h"
#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_alignment.h"
#include "rpg_character_monsteradvancementstep.h"
#include "rpg_character_common.h"
#include "rpg_character_skills_common.h"

typedef std::set<RPG_Character_MonsterSubType> RPG_Character_MonsterSubTypes_t;
typedef RPG_Character_MonsterSubTypes_t::const_iterator RPG_Character_MonsterSubTypesIterator_t;

typedef RPG_Chance_DiceRoll RPG_Character_Damage;
typedef RPG_Chance_DiceRoll RPG_Character_HitDice;

typedef std::vector<RPG_Character_OrganizationStep> RPG_Character_Organizations_t;
typedef RPG_Character_Organizations_t::const_iterator RPG_Character_OrganizationsIterator_t;
typedef std::set<RPG_Character_Organization> RPG_Character_OrganizationList_t;
typedef RPG_Character_OrganizationList_t::const_iterator RPG_Character_OrganizationListIterator_t;

typedef std::vector<RPG_Character_MonsterAdvancementStep> RPG_Character_MonsterAdvancement_t;
typedef RPG_Character_MonsterAdvancement_t::const_iterator RPG_Character_MonsterAdvancementIterator_t;

struct RPG_Character_MonsterProperties
{
//   std::string                        name;
  RPG_Character_Size                 size;
  RPG_Character_MonsterType          type;
  RPG_Chance_DiceRoll                hitDice;
  int                                initiative;
  unsigned int                       speed;
  RPG_Character_MonsterArmorClass    armorClass;
  RPG_Character_MonsterAttack        attack;
  unsigned int                       space; // feet
  unsigned int                       reach; // feet
  RPG_Character_SavingThrowModifiers saves;
  RPG_Character_Attributes           attributes;
  RPG_Character_Skills_t             skills;
  RPG_Character_Feats_t              feats;
  RPG_Character_Environment          environment;
  RPG_Character_Organizations_t      organizations;
  unsigned int                       challengeRating;
  unsigned int                       treasureModifier; // standard times x
  RPG_Character_Alignment            alignment;
  RPG_Character_MonsterAdvancement_t advancements;
  unsigned int                       levelAdjustment;
};

// some more useful types
typedef std::map<std::string, RPG_Character_MonsterProperties> RPG_Character_MonsterDictionary_t;
typedef RPG_Character_MonsterDictionary_t::const_iterator RPG_Character_MonsterDictionaryIterator_t;

typedef std::vector<std::string> RPG_Character_MonsterList_t;
typedef RPG_Character_MonsterList_t::const_iterator RPG_Character_MonsterListIterator_t;
typedef std::map<std::string, unsigned short int> RPG_Character_Encounter_t;
typedef RPG_Character_Encounter_t::const_iterator RPG_Character_EncounterIterator_t;

#endif
