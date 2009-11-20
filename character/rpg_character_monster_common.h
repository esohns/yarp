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
  MONSTERMETATYPE_ANY = 0,
  MONSTERMETATYPE_ABERRATION,
  MONSTERMETATYPE_ANIMAL,
  MONSTERMETATYPE_CONSTRUCT,
  MONSTERMETATYPE_DRAGON,
  MONSTERMETATYPE_ELEMENTAL,
  MONSTERMETATYPE_FEY,
  MONSTERMETATYPE_GIANT,
  MONSTERMETATYPE_HUMANOID,
  MONSTERMETATYPE_MAGICAL_BEAST,
  MONSTERMETATYPE_MONSTROUS_HUMANOID,
  MONSTERMETATYPE_OOZE,
  MONSTERMETATYPE_OUTSIDER,
  MONSTERMETATYPE_PLANT,
  MONSTERMETATYPE_UNDEAD,
  MONSTERMETATYPE_VERMIN,
  //
  MONSTERMETATYPE_MAX,
  MONSTERMETATYPE_INVALID
};

enum RPG_Character_MonsterSubType
{
  MONSTERSUBTYPE_ANY = 0,
  MONSTERSUBTYPE_AIR,
  MONSTERSUBTYPE_ANGEL,
  MONSTERSUBTYPE_AQUATIC,
  MONSTERSUBTYPE_ARCHON,
  MONSTERSUBTYPE_AUGMENTED,
  MONSTERSUBTYPE_CHAOTIC,
  MONSTERSUBTYPE_COLD,
  MONSTERSUBTYPE_EARTH,
  MONSTERSUBTYPE_EVIL,
  MONSTERSUBTYPE_EXTRAPLANAR,
  MONSTERSUBTYPE_FIRE,
  MONSTERSUBTYPE_GOBLINOID,
  MONSTERSUBTYPE_GOOD,
  MONSTERSUBTYPE_INCORPOREAL,
  MONSTERSUBTYPE_LAWFUL,
  MONSTERSUBTYPE_NATIVE,
  MONSTERSUBTYPE_REPTILIAN,
  MONSTERSUBTYPE_SHAPECHANGER,
  MONSTERSUBTYPE_SWARM,
  MONSTERSUBTYPE_WATER,
  //
  MONSTERSUBTYPE_MAX,
  MONSTERSUBTYPE_INVALID
};

typedef std::set<RPG_Character_MonsterSubType> RPG_Character_MonsterSubTypes_t;
typedef RPG_Character_MonsterSubTypes_t::const_iterator RPG_Character_MonsterSubTypesIterator_t;

struct RPG_Character_MonsterType
{
  RPG_Character_MonsterMetaType metaType;
  RPG_Character_MonsterSubTypes_t subTypes;
};

struct RPG_Character_MonsterArmorClass
{
  unsigned int normal;
  unsigned int touch;
  unsigned int flatFooted;
};

enum RPG_Character_MonsterWeapon
{
  INCORPOREAL_TOUCH = 0,
  NATURALWEAPON_BITE,
  NATURALWEAPON_CLAW_TALON,
  NATURALWEAPON_GORE,
  NATURALWEAPON_SLAP_SLAM,
  NATURALWEAPON_STING,
  NATURALWEAPON_TENTACLE,
  NATURALWEAPON_SPIT_ACID,
  NATURALWEAPON_WEB,
  NATURALWEAPON_ROCK_STONE,
  STANDARDWEAPON_BLUDGEONING,
  STANDARDWEAPON_PIERCING,
  STANDARDWEAPON_SLASHING,
  RAY_ELECTRICITY,
  RAY_LIGHT,
  //
  MONSTERWEAPON_MAX,
  MONSTERWEAPON_INVALID
};

enum RPG_Character_MonsterAttackForm
{
  ATTACK_MELEE = 0,
  ATTACK_TOUCH,
  ATTACK_RANGED,
  //
  ATTACK_MAX,
  ATTACK_INVALID
};

typedef RPG_Chance_DiceRoll RPG_Character_Damage;

struct RPG_Character_MonsterAttackAction
{
  RPG_Character_MonsterWeapon     monsterWeapon;
  int                             attackBonus;
  RPG_Character_MonsterAttackForm attackForm;
  RPG_Character_Damage            damage;
  unsigned int                    numAttacksPerRound;
};

typedef std::vector<RPG_Character_MonsterAttackAction> RPG_Character_MonsterAttackActions_t;
typedef RPG_Character_MonsterAttackActions_t::const_iterator RPG_Character_MonsterAttackActionsIterator_t;

struct RPG_Character_MonsterAttack
{
  int                                  baseAttackBonus;
  int                                  grappleBonus;
  RPG_Character_MonsterAttackActions_t attackActions;
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
  int fortitude;
  int reflex;
  int will;
};

enum RPG_Character_Environment
{
  ENVIRONMENT_ANY = 0,
  ENVIRONMENT_UNDERGROUND,
  ENVIRONMENT_PLAINS_TEMPERATE,
  ENVIRONMENT_PLAINS_WARM,
  ENVIRONMENT_FORESTS_TEMPERATE,
  ENVIRONMENT_HILLS_TEMPERATE,
  ENVIRONMENT_HILLS_WARM,
  ENVIRONMENT_MOUNTAINS_TEMPERATE,
  ENVIRONMENT_DESERTS_WARM,
  ENVIRONMENT_PLANE_ANY_LAWFUL,
  ENVIRONMENT_PLANE_LAWFUL_GOOD,
  ENVIRONMENT_PLANE_ANY_GOOD,
  ENVIRONMENT_PLANE_CHAOTIC_GOOD,
  ENVIRONMENT_PLANE_ANY_EVIL,
  ENVIRONMENT_PLANE_CHAOTIC_EVIL,
  ENVIRONMENT_PLANE_AIR,
  ENVIRONMENT_PLANE_FIRE,
  //
  ENVIRONMENT_MAX,
  ENVIRONMENT_INVALID
};

enum RPG_Character_Organization
{
  ORGANIZATION_ANY = 0,
  ORGANIZATION_SOLITARY, // 1
  ORGANIZATION_PAIR,     // 2
  ORGANIZATION_BROOD,    // 2-4
  ORGANIZATION_CLUSTER,  // 2-4
  ORGANIZATION_CLUTCH,   // 2-4, 3-4
  ORGANIZATION_PATCH,    // 2-4
  ORGANIZATION_GANG,     // 2-4
  ORGANIZATION_TEAM,     // 3-4
  ORGANIZATION_SQUAD,    // 3-5, 11-20 + leaders
  ORGANIZATION_PACK,     // 3-6, 7-16
  ORGANIZATION_COLONY,   // 3-6
  ORGANIZATION_FLOCK,    // 5-8
  ORGANIZATION_TRIBE,    // 7-12
  ORGANIZATION_SLAVER,   // 7-12 slaves
  ORGANIZATION_BAND,     // 11-20 + 2 sergeants + 1 leader + 150% noncombatants
  ORGANIZATION_CLAN,     // 30-100 + leaders + 50% noncombatants
  //
  ORGANIZATION_MAX,
  ORGANIZATION_INVALID
};

typedef std::set<RPG_Character_Organization> RPG_Character_Organizations_t;
typedef RPG_Character_Organizations_t::const_iterator RPG_Character_OrganizationsIterator_t;

typedef RPG_Chance_DiceRoll RPG_Character_HitDice;

typedef std::pair<RPG_Character_Size, RPG_Chance_ValueRange> RPG_Character_MonsterAdvancementStep_t;
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
