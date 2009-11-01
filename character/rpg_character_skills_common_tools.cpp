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
#include "rpg_character_skills_common_tools.h"

#include <ace/Log_Msg.h>

// init statics
RPG_Character_Skills_Common_Tools::RPG_Character_Skill2StringTable_t RPG_Character_Skills_Common_Tools::mySkill2StringTable;
RPG_Character_Skills_Common_Tools::RPG_Character_ClassSkillsTable_t RPG_Character_Skills_Common_Tools::myClassSkillsTable;
RPG_Character_Skills_Common_Tools::RPG_Character_FeatPrerequisitesTable_t RPG_Character_Skills_Common_Tools::myFeatPrerequisitesTable;

void RPG_Character_Skills_Common_Tools::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::init"));

  initStringConversionTable();
  initClassSkillsTable();
  initFeatPrerequisitesTable();
}

void RPG_Character_Skills_Common_Tools::initFeatPrerequisitesTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initFeatPrerequisitesTable"));

  // clean tables
  myFeatPrerequisitesTable.clear();

  RPG_Character_Feat_Prerequisite prerequisite;
  prerequisite.minValue = 0;
  prerequisite.requiredOtherFeat = FEAT_INVALID;
  prerequisite.skill = SKILL_INVALID;
  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_INVALID;
  prerequisite.requiredAbility = ABILITY_INVALID;

  RPG_Character_Feat_Prerequisites_t prerequisites;

  // FEAT_ARMOR_PROFICIENCY_HEAVY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_ARMOR_PROFICIENCY_MEDIUM;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_ARMOR_PROFICIENCY_LIGHT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_ARMOR_PROFICIENCY_HEAVY, prerequisites));
  prerequisites.clear();

  // FEAT_ARMOR_PROFICIENCY_MEDIUM
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_ARMOR_PROFICIENCY_LIGHT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_ARMOR_PROFICIENCY_MEDIUM, prerequisites));
  prerequisites.clear();

  // FEAT_AUGMENT_SUMMONING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_SPELL_FOCUS;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_AUGMENT_SUMMONING, prerequisites));
  prerequisites.clear();

  // FEAT_BREW_POTION
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 3;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_BREW_POTION, prerequisites));
  prerequisites.clear();

  // FEAT_CLEAVE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CLEAVE, prerequisites));
  prerequisites.clear();

  // FEAT_COMBAT_EXPERTISE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_COMBAT_EXPERTISE, prerequisites));
  prerequisites.clear();

  // FEAT_CRAFT_MAGIC_ARMS_AND_ARMOR
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 5;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CRAFT_MAGIC_ARMS_AND_ARMOR, prerequisites));
  prerequisites.clear();

  // FEAT_CRAFT_ROD
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 9;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CRAFT_ROD, prerequisites));
  prerequisites.clear();

  // FEAT_CRAFT_STAFF
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 12;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CRAFT_STAFF, prerequisites));
  prerequisites.clear();

  // FEAT_CRAFT_WAND
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 5;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CRAFT_WAND, prerequisites));
  prerequisites.clear();

  // FEAT_CRAFT_WONDROUS_ITEM
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 3;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CRAFT_WONDROUS_ITEM, prerequisites));
  prerequisites.clear();

  // FEAT_DEFLECT_ARROWS
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_DEFLECT_ARROWS, prerequisites));
  prerequisites.clear();

  // FEAT_DODGE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_DODGE, prerequisites));
  prerequisites.clear();

  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_EXOTIC_WEAPONS, prerequisites));
  prerequisites.clear();

  // FEAT_EXTRA_TURNING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  // ability to turn or rebuke creatures
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_PALADIN);
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_CLERIC);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_EXTRA_TURNING, prerequisites));
  prerequisites.clear();

  // FEAT_FAR_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_FAR_SHOT, prerequisites));
  prerequisite.requiredOtherFeat = FEAT_INVALID;
  prerequisites.clear();

  // FEAT_FORGE_RING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 12;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_FORGE_RING, prerequisites));
  prerequisites.clear();

  // FEAT_GREAT_CLEAVE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_CLEAVE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 4;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREAT_CLEAVE, prerequisites));
  prerequisites.clear();

  // FEAT_GREATER_SPELL_PENETRATION
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_SPELL_PENETRATION;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREATER_SPELL_PENETRATION, prerequisites));
  prerequisites.clear();

  // FEAT_GREATER_TWO_WEAPON_FIGHTING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 19;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 11;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREATER_TWO_WEAPON_FIGHTING, prerequisites));
  prerequisites.clear();

  // FEAT_GREATER_WEAPON_FOCUS
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO requires proficiency for a SPECIFIC weapon !
  // FEAT_PROFICIENCY_SIMPLE_WEAPONS
  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_WEAPON_FOCUS;
  // TODO requires focus for a SPECIFIC weapon !
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 8;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_FIGHTER);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREATER_WEAPON_FOCUS, prerequisites));
  prerequisites.clear();

  // FEAT_GREATER_WEAPON_SPECIALIZATION
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO requires proficiency for a SPECIFIC weapon !
  // FEAT_PROFICIENCY_SIMPLE_WEAPONS
  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_GREATER_WEAPON_FOCUS;
  // TODO requires greater focus for a SPECIFIC weapon !
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_WEAPON_FOCUS;
  // TODO requires focus for a SPECIFIC weapon !
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_WEAPON_SPECIALIZATION;
  // TODO requires specialization for a SPECIFIC weapon !
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 12;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_FIGHTER);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREATER_WEAPON_SPECIALIZATION, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_BULL_RUSH
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_BULL_RUSH, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_CRITICAL
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO requires proficiency for a SPECIFIC weapon !
  // FEAT_PROFICIENCY_SIMPLE_WEAPONS
  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 8;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_CRITICAL, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_DISARM
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_DISARM, prerequisites));
  prerequisites.clear();

  // TODO: FEAT_IMPROVED_FAMILIAR

  // FEAT_IMPROVED_FEINT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_FEINT, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_GRAPPLE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_GRAPPLE, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_OVERRUN
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_OVERRUN, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_PRECISE_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 19;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.requiredOtherFeat = FEAT_PRECISE_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 11;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_PRECISE_SHOT, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_SHIELD_BASH
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_SHIELD;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_SHIELD_BASH, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_SUNDER
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_SUNDER, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_TRIP
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_TRIP, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_TURNING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  // ability to turn or rebuke creatures
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_PALADIN);
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_CLERIC);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_TURNING, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_TWO_WEAPON_FIGHTING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 17;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 6;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_TWO_WEAPON_FIGHTING, prerequisites));
  prerequisites.clear();

  // FEAT_LEADERSHIP
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 6;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_LEADERSHIP, prerequisites));
  prerequisites.clear();

  // FEAT_MANY_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 17;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.requiredOtherFeat = FEAT_RAPID_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 6;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MANY_SHOT, prerequisites));
  prerequisites.clear();

  // FEAT_MOBILITY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_DODGE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MOBILITY, prerequisites));
  prerequisites.clear();

  // FEAT_MOUNTED_ARCHERY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MOUNTED_ARCHERY, prerequisites));
  prerequisites.clear();

  // FEAT_MOUNTED_COMBAT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = SKILL_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MOUNTED_COMBAT, prerequisites));
  prerequisites.clear();

  // FEAT_NATURAL_SPELL
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_WISDOM;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_ABILITY;
  prerequisite.requiredAbility = ABILITY_POLYMORPH;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredAbility = ABILITY_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_NATURAL_SPELL, prerequisites));
  prerequisites.clear();

  // FEAT_POWER_ATTACK
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_POWER_ATTACK, prerequisites));
  prerequisites.clear();

  // FEAT_PRECISE_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PRECISE_SHOT, prerequisites));
  prerequisites.clear();

  // FEAT_QUICK_DRAW
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_QUICK_DRAW, prerequisites));
  prerequisites.clear();

  // FEAT_RAPID_RELOAD
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO: requires proficiency with SPECIAL weapon (crossbow) !
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_RAPID_RELOAD, prerequisites));
  prerequisites.clear();

  // FEAT_RAPID_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_RAPID_SHOT, prerequisites));
  prerequisites.clear();

  // FEAT_RIDE_BY_ATTACK
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_RIDE_BY_ATTACK, prerequisites));
  prerequisites.clear();

  // FEAT_SCRIBE_SCROLL
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SCRIBE_SCROLL, prerequisites));
  prerequisites.clear();

  // FEAT_SHOT_ON_THE_RUN
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_DODGE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_MOBILITY;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 4;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SHOT_ON_THE_RUN, prerequisites));
  prerequisites.clear();

  // FEAT_SNATCH_ARROWS
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 15;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_DEFLECT_ARROWS;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SNATCH_ARROWS, prerequisites));
  prerequisites.clear();

  // FEAT_SPELL_MASTERY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_WIZARD);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SPELL_MASTERY, prerequisites));
  prerequisites.clear();

  // FEAT_SPIRITED_CHARGE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_RIDE_BY_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SPIRITED_CHARGE, prerequisites));
  prerequisites.clear();

  // FEAT_STUNNING_FIST
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.attribute = ATTRIBUTE_WISDOM;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 8;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_STUNNING_FIST, prerequisites));
  prerequisites.clear();

//   // FEAT_PROFICIENCY_TOWER_SHIELD
//   prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
//   prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_SHIELD;
//   prerequisites.push_back(prerequisite);
// 
//   prerequisite.requiredOtherFeat = FEAT_INVALID;
// 
//   myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_TOWER_SHIELD, prerequisites));
//   prerequisites.clear();

  // FEAT_TRAMPLE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_TRAMPLE, prerequisites));
  prerequisites.clear();

  // FEAT_TWO_WEAPON_DEFENSE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 15;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_TWO_WEAPON_DEFENSE, prerequisites));
  prerequisites.clear();

  // FEAT_TWO_WEAPON_FIGHTING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 15;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_TWO_WEAPON_FIGHTING, prerequisites));
  prerequisites.clear();

  // FEAT_WEAPON_FINESSE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_WEAPON_FINESSE, prerequisites));
  prerequisites.clear();

  // FEAT_WEAPON_FOCUS
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO requires proficiency for a SPECIFIC weapon !
  // FEAT_PROFICIENCY_SIMPLE_WEAPONS
  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_WEAPON_FOCUS, prerequisites));
  prerequisites.clear();

  // FEAT_WEAPON_SPECIALIZATION
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO requires proficiency for a SPECIFIC weapon !
  // FEAT_PROFICIENCY_SIMPLE_WEAPONS
  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_WEAPON_FOCUS;
  // TODO requires focus for a SPECIFIC weapon !
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 4;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.push_back(SUBCLASS_FIGHTER);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_WEAPON_SPECIALIZATION, prerequisites));
  prerequisites.clear();

  // FEAT_WHIRLWIND_ATTACK
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_DODGE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_MOBILITY;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_SPRING_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 4;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_WHIRLWIND_ATTACK, prerequisites));
  prerequisites.clear();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Skills_Common_Tools: initialized feat prerequisite table...\n")));
}

void RPG_Character_Skills_Common_Tools::initClassSkillsTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initClassSkillsTable"));

  // clean tables
  myClassSkillsTable.clear();

  // RPG_Character_SubClass / RPG_Character_Skill
  RPG_Character_ClassSkillsSet_t skillSet;
  // SUBCLASS_FIGHTER
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_FIGHTER, skillSet));

  skillSet.clear();
  // SUBCLASS_PALADIN
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE_NOB_ROY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_PALADIN, skillSet));

  skillSet.clear();
  // SUBCLASS_RANGER
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SEARCH);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SURVIVAL);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_USE_ROPE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_RANGER, skillSet));

  skillSet.clear();
  // SUBCLASS_BARBARIAN
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SURVIVAL);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_BARBARIAN, skillSet));

  skillSet.clear();
  // SUBCLASS_WIZARD
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_ARCH_ENG);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_KNOWLEDGE_NOB_ROY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_KNOWLEDGE_PLANES);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_WIZARD, skillSet));

  skillSet.clear();
  // SUBCLASS_SORCERER
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_SORCERER, skillSet));

  skillSet.clear();
  // SUBCLASS_CLERIC
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_KNOWLEDGE_PLANES);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_CLERIC, skillSet));

  skillSet.clear();
  // SUBCLASS_DRUID
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SPELLCRAFT);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SURVIVAL);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_DRUID, skillSet));

  skillSet.clear();
  // SUBCLASS_MONK
  skillSet.insert(SKILL_BALANCE);
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_PERFORM);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_TUMBLE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_MONK, skillSet));

  skillSet.clear();
  // SUBCLASS_THIEF
  skillSet.insert(SKILL_APPRAISE);
  skillSet.insert(SKILL_BALANCE);
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT_TRAP);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_DISABLE_DEVICE);
  skillSet.insert(SKILL_DISGUISE);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_FORGERY);
  skillSet.insert(SKILL_GATHER_INFORMATION);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_OPEN_LOCK);
  skillSet.insert(SKILL_PERFORM);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SEARCH);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  skillSet.insert(SKILL_SLEIGHT_OF_HAND);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_TUMBLE);
  skillSet.insert(SKILL_USE_MAGIC_DEVICE);
  skillSet.insert(SKILL_USE_ROPE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_THIEF, skillSet));

  skillSet.clear();
  // SUBCLASS_BARD
  skillSet.insert(SKILL_APPRAISE);
  skillSet.insert(SKILL_BALANCE);
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_DISGUISE);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_GATHER_INFORMATION);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_ARCH_ENG);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_KNOWLEDGE_NOB_ROY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_KNOWLEDGE_PLANES);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_PERFORM);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  skillSet.insert(SKILL_SLEIGHT_OF_HAND);
  skillSet.insert(SKILL_SPEAK_LANGUAGE);
  skillSet.insert(SKILL_SPELLCRAFT);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_TUMBLE);
  skillSet.insert(SKILL_USE_MAGIC_DEVICE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_BARD, skillSet));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Skills_Common_Tools: initialized class skill table...\n")));
}

void RPG_Character_Skills_Common_Tools::initStringConversionTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initStringConversionTable"));

  // clean tables
  mySkill2StringTable.clear();

  mySkill2StringTable.insert(std::make_pair(SKILL_APPRAISE, ACE_TEXT_ALWAYS_CHAR("Appraise")));
  mySkill2StringTable.insert(std::make_pair(SKILL_BALANCE, ACE_TEXT_ALWAYS_CHAR("Balance")));
  mySkill2StringTable.insert(std::make_pair(SKILL_BLUFF, ACE_TEXT_ALWAYS_CHAR("Bluff")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CLIMB, ACE_TEXT_ALWAYS_CHAR("Climb")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CONCENTRATION, ACE_TEXT_ALWAYS_CHAR("Concentration")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_ALCHEMY, ACE_TEXT_ALWAYS_CHAR("Craft: Alchemy")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_FLETCHER, ACE_TEXT_ALWAYS_CHAR("Craft: Fletcher")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_BOWYER, ACE_TEXT_ALWAYS_CHAR("Craft: Bowyer")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_ARMOR, ACE_TEXT_ALWAYS_CHAR("Craft: Armorsmith")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_BLACK, ACE_TEXT_ALWAYS_CHAR("Craft: Blacksmith")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_WEAPON, ACE_TEXT_ALWAYS_CHAR("Craft: Weaponsmith")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_TRAP, ACE_TEXT_ALWAYS_CHAR("Craft: Make Traps")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_OTHER, ACE_TEXT_ALWAYS_CHAR("Craft: General")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DECIPHER_SCRIPT, ACE_TEXT_ALWAYS_CHAR("Decipher Script")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DIPLOMACY, ACE_TEXT_ALWAYS_CHAR("Diplomacy")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DISABLE_DEVICE, ACE_TEXT_ALWAYS_CHAR("Disable Device")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DISGUISE, ACE_TEXT_ALWAYS_CHAR("Disguise")));
  mySkill2StringTable.insert(std::make_pair(SKILL_ESCAPE_ARTIST, ACE_TEXT_ALWAYS_CHAR("Escape Artist")));
  mySkill2StringTable.insert(std::make_pair(SKILL_FORGERY, ACE_TEXT_ALWAYS_CHAR("Forgery")));
  mySkill2StringTable.insert(std::make_pair(SKILL_GATHER_INFORMATION, ACE_TEXT_ALWAYS_CHAR("Gather Information")));
  mySkill2StringTable.insert(std::make_pair(SKILL_HANDLE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("Handle Animal")));
  mySkill2StringTable.insert(std::make_pair(SKILL_HEAL, ACE_TEXT_ALWAYS_CHAR("Heal")));
  mySkill2StringTable.insert(std::make_pair(SKILL_HIDE, ACE_TEXT_ALWAYS_CHAR("Hide")));
  mySkill2StringTable.insert(std::make_pair(SKILL_INTIMIDATE, ACE_TEXT_ALWAYS_CHAR("Intimidate")));
  mySkill2StringTable.insert(std::make_pair(SKILL_JUMP, ACE_TEXT_ALWAYS_CHAR("Jump")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCANA, ACE_TEXT_ALWAYS_CHAR("Knowledge: Arcana")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCH_ENG, ACE_TEXT_ALWAYS_CHAR("Knowledge: Achitecture & Engineering")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_DUNGEONS, ACE_TEXT_ALWAYS_CHAR("Knowledge: Dungeoneering")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_GEOGRAPHY, ACE_TEXT_ALWAYS_CHAR("Knowledge: Geography")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_HISTORY, ACE_TEXT_ALWAYS_CHAR("Knowledge: History")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_LOCAL, ACE_TEXT_ALWAYS_CHAR("Knowledge: Local")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NATURE, ACE_TEXT_ALWAYS_CHAR("Knowledge: Nature")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NOB_ROY, ACE_TEXT_ALWAYS_CHAR("Knowledge: Nobility & Royalty")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_RELIGION, ACE_TEXT_ALWAYS_CHAR("Knowledge: Religion")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_PLANES, ACE_TEXT_ALWAYS_CHAR("Knowledge: The Planes")));
  mySkill2StringTable.insert(std::make_pair(SKILL_LISTEN, ACE_TEXT_ALWAYS_CHAR("Listen")));
  mySkill2StringTable.insert(std::make_pair(SKILL_MOVE_SILENTLY, ACE_TEXT_ALWAYS_CHAR("Move Silently")));
  mySkill2StringTable.insert(std::make_pair(SKILL_OPEN_LOCK, ACE_TEXT_ALWAYS_CHAR("Open Lock")));
  mySkill2StringTable.insert(std::make_pair(SKILL_PERFORM, ACE_TEXT_ALWAYS_CHAR("Perform")));
  mySkill2StringTable.insert(std::make_pair(SKILL_PROFESSION, ACE_TEXT_ALWAYS_CHAR("Profession")));
  mySkill2StringTable.insert(std::make_pair(SKILL_RIDE, ACE_TEXT_ALWAYS_CHAR("Ride")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SEARCH, ACE_TEXT_ALWAYS_CHAR("Search")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SENSE_MOTIVE, ACE_TEXT_ALWAYS_CHAR("Sense Motive")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SLEIGHT_OF_HAND, ACE_TEXT_ALWAYS_CHAR("Sleight Of Hand")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SPEAK_LANGUAGE, ACE_TEXT_ALWAYS_CHAR("Speak Language")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SPELLCRAFT, ACE_TEXT_ALWAYS_CHAR("Spellcraft")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SPOT, ACE_TEXT_ALWAYS_CHAR("Spot")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SURVIVAL, ACE_TEXT_ALWAYS_CHAR("Survival")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SWIM, ACE_TEXT_ALWAYS_CHAR("Swim")));
  mySkill2StringTable.insert(std::make_pair(SKILL_TUMBLE, ACE_TEXT_ALWAYS_CHAR("Tumble")));
  mySkill2StringTable.insert(std::make_pair(SKILL_USE_MAGIC_DEVICE, ACE_TEXT_ALWAYS_CHAR("Use Magic Device")));
  mySkill2StringTable.insert(std::make_pair(SKILL_USE_ROPE, ACE_TEXT_ALWAYS_CHAR("Use Rope")));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Skills_Common_Tools: initialized string conversion table...\n")));
}

const bool RPG_Character_Skills_Common_Tools::isClassSkill(const RPG_Character_SubClass& subClass_in,
                                                           const RPG_Character_Skill& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::isClassSkill"));

  RPG_Character_ClassSkillsTableIterator_t iterator = myClassSkillsTable.find(subClass_in);
  if (iterator == myClassSkillsTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid subclass: %d --> check implementation !, aborting\n"),
               subClass_in));

    return false;
  } // end IF

  return (iterator->second.find(skill_in) != iterator->second.end());
}

const unsigned int RPG_Character_Skills_Common_Tools::getSkillPoints(const RPG_Character_SubClass& subClass_in,
                                                                     const short int& INTModifier_in,
                                                                     unsigned int& initialPoints_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::getSkillPoints"));

  // init defaults
  initialPoints_out = 0;

  short int baseValue = INTModifier_in;
  switch (subClass_in)
  {
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    case SUBCLASS_CLERIC:
    {
      baseValue += 2;

      break;
    }
    case SUBCLASS_BARBARIAN:
    case SUBCLASS_DRUID:
    case SUBCLASS_MONK:
    {
      baseValue += 4;

      break;
    }
    case SUBCLASS_RANGER:
    case SUBCLASS_BARD:
    {
      baseValue += 6;

      break;
    }
//     case SUBCLASS_WARLORD:
//     case SUBCLASS_WARLOCK:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
    case SUBCLASS_THIEF:
    {
      baseValue += 8;

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: %d --> check implementation !, aborting\n"),
                 subClass_in));

      return 0;
    }
  } // end SWITCH

  // at least 1/level...
  initialPoints_out = ((baseValue > 1) ? (baseValue * 4) : 1);

  return ((baseValue > 1) ? baseValue : 1);
}
