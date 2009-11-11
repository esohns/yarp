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

enum RPG_Character_MonsterMetaType
{
  MONSTERMETATYPE_ABERRATION = 0,
  //
  MONSTERMETATYPE_MAX,
  MONSTERMETATYPE_INVALID
};

enum RPG_Character_MonsterSubType
{
  MONSTERSUBTYPE_AQUATIC = 0,
  //
  MONSTERSUBTYPE_MAX,
  MONSTERSUBTYPE_INVALID
};

struct RPG_Character_MonsterType
{
  RPG_Character_MonsterMetaType metaType;
  RPG_Character_MonsterSubType  subType;
};

struct RPG_Character_MonsterArmorClass
{
  unsigned int normal;
  unsigned int touch;
  unsigned int flatFooted;
};

enum RPG_Character_NaturalWeapon
{
  NATURALWEAPON_TENTACLE = 0,
  //
  NATURALWEAPON_MAX,
  NATURALWEAPON_INVALID
};

enum RPG_Character_MonsterAttackForm
{
  ATTACK_MELEE = 0,
  ATTACK_RANGED,
  //
  ATTACK_MAX,
  ATTACK_INVALID
};

typedef RPG_Chance_Roll RPG_Character_Damage;

struct RPG_Character_MonsterAttack
{
  unsigned int                    baseAttackBonus;
  unsigned int                    grappleBonus;
  RPG_Character_NaturalWeapon     naturalWeapon;
  unsigned int                    attackBonus;
  RPG_Character_MonsterAttackForm attackForm;
  RPG_Character_Damage            damage;
  unsigned int                    numAttacksPerRound;
};

enum RPG_Character_Size
{
  SIZE_FINE = 0,
  SIZE_DIMINUTIVE,
  SIZE_TINY,
  SIZE_SMALL,
  SIZE_MEDIUM,
  SIZE_LARGE,
  SIZE_HUGE,
  SIZE_GARGANTUAN,
  SIZE_COLOSSAL,
  //
  SIZE_MAX,
  SIZE_INVALID
};

struct RPG_Character_SavingThrowModifiers
{
  unsigned int fortitude;
  unsigned int reflex;
  unsigned int will;
};

enum RPG_Character_Environment
{
  ENVIRONMENT_UNDERGROUND = 0,
  //
  ENVIRONMENT_MAX,
  ENVIRONMENT_INVALID
};

enum RPG_Character_Organization
{
  ORGANIZATION_SOLITARY = 0,
  ORGANIZATION_BROOD,
  ORGANIZATION_SLAVER_BROOD,
  //
  ORGANIZATION_MAX,
  ORGANIZATION_INVALID
};

typedef std::set<RPG_Character_Organization> RPG_Character_Organizations_t;
typedef RPG_Character_Organizations_t::const_iterator RPG_Character_OrganizationsIterator_t;

typedef RPG_Chance_Roll RPG_Character_HitDice;

typedef std::pair<RPG_Character_Size, RPG_Chance_Roll> RPG_Character_MonsterAdvancementStep_t;
typedef std::vector<RPG_Character_MonsterAdvancementStep_t> RPG_Character_MonsterAdvancement_t;
typedef RPG_Character_MonsterAdvancement_t::const_iterator RPG_Character_MonsterAdvancementIterator_t;

struct RPG_Character_MonsterProperties
{
//   std::string                        name;
  RPG_Character_Size                 size;
  RPG_Character_MonsterType          type;
  RPG_Character_HitDice              hitDice;
  int                                initiative;
  unsigned int                       speed;
  RPG_Character_MonsterArmorClass    armorClass;
  RPG_Character_MonsterAttack        attack;
  unsigned int                       space; // feet
  unsigned int                       reach; // feet
  RPG_Character_SavingThrowModifiers saves;
  RPG_Character_Abilities_t          abilities;
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

// need this for parsing purposes...
struct RPG_Character_MonsterProperties_XML
{
  std::string                        name;
  RPG_Character_Size                 size;
  RPG_Character_MonsterType          type;
  RPG_Character_HitDice              hitDice;
  int                                initiative;
  unsigned int                       speed;
  RPG_Character_MonsterArmorClass    armorClass;
  RPG_Character_MonsterAttack        attack;
  unsigned int                       space; // feet
  unsigned int                       reach; // feet
  RPG_Character_SavingThrowModifiers saves;
  RPG_Character_Abilities_t          abilities;
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

// some useful types
typedef std::map<std::string, RPG_Character_MonsterProperties> RPG_Character_MonsterDictionary_t;
typedef RPG_Character_MonsterDictionary_t::const_iterator RPG_Character_MonsterDictionaryIterator_t;
typedef std::map<std::string, unsigned short int> RPG_Character_Encounter_t;
typedef RPG_Character_Encounter_t::const_iterator RPG_Character_EncounterIterator_t;

#endif
