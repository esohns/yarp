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

#include "rpg_character_common.h"
#include "rpg_character_skills_common.h"

#include <rpg_chance_dice_common.h>

#include <string>
#include <vector>
#include <set>
#include <map>

typedef std::set<RPG_Character_MonsterSubType> RPG_Character_MonsterSubTypes_t;
typedef RPG_Character_MonsterSubTypes_t::const_iterator RPG_Character_MonsterSubTypesIterator_t;

typedef RPG_Chance_DiceRoll RPG_Character_Damage;
typedef RPG_Chance_DiceRoll RPG_Character_HitDice;

typedef std::pair<RPG_Character_Size, RPG_Chance_ValueRange> RPG_Character_MonsterAdvancementStep_t;
typedef std::vector<RPG_Character_MonsterAdvancementStep_t> RPG_Character_MonsterAdvancement_t;
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
  RPG_Character_MonsterAdvancement_t advancement;
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
