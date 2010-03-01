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

#include "rpg_character_common_tools.h"

#include <rpg_dice_common.h>
#include <rpg_dice.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Character_Feats_t RPG_Character_Skills_Common_Tools::myFighterBonusFeatsTable;
RPG_Character_Feats_t RPG_Character_Skills_Common_Tools::myWizardBonusFeatsTable;
RPG_Character_Skills_Common_Tools::RPG_Character_ClassSkillsTable_t RPG_Character_Skills_Common_Tools::myClassSkillsTable;
RPG_Character_Skills_Common_Tools::RPG_Character_FeatPrerequisitesTable_t RPG_Character_Skills_Common_Tools::myFeatPrerequisitesTable;

void RPG_Character_Skills_Common_Tools::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::init"));

  initClassSkillsTable();
  initFeatPrerequisitesTable();
  initBonusFeatsTables();
}

void RPG_Character_Skills_Common_Tools::initBonusFeatsTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initBonusFeatsTables"));

  // clean table
  myFighterBonusFeatsTable.clear();

  myFighterBonusFeatsTable.insert(FEAT_BLIND_FIGHT);
  myFighterBonusFeatsTable.insert(FEAT_COMBAT_EXPERTISE);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_DISARM);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_FEINT);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_TRIP);
  myFighterBonusFeatsTable.insert(FEAT_WHIRLWIND_ATTACK);
  myFighterBonusFeatsTable.insert(FEAT_COMBAT_REFLEXES);
  myFighterBonusFeatsTable.insert(FEAT_DODGE);
  myFighterBonusFeatsTable.insert(FEAT_MOBILITY);
  myFighterBonusFeatsTable.insert(FEAT_SPRING_ATTACK);
  myFighterBonusFeatsTable.insert(FEAT_PROFICIENCY_EXOTIC_WEAPONS);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_CRITICAL);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_INITIATIVE);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_SHIELD_BASH);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_UNARMED_STRIKE);
  myFighterBonusFeatsTable.insert(FEAT_DEFLECT_ARROWS);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_GRAPPLE);
  myFighterBonusFeatsTable.insert(FEAT_SNATCH_ARROWS);
  myFighterBonusFeatsTable.insert(FEAT_STUNNING_FIST);
  myFighterBonusFeatsTable.insert(FEAT_MOUNTED_COMBAT);
  myFighterBonusFeatsTable.insert(FEAT_MOUNTED_ARCHERY);
  myFighterBonusFeatsTable.insert(FEAT_RIDE_BY_ATTACK);
  myFighterBonusFeatsTable.insert(FEAT_SPIRITED_CHARGE);
  myFighterBonusFeatsTable.insert(FEAT_TRAMPLE);
  myFighterBonusFeatsTable.insert(FEAT_POINT_BLANK_SHOT);
  myFighterBonusFeatsTable.insert(FEAT_FAR_SHOT);
  myFighterBonusFeatsTable.insert(FEAT_PRECISE_SHOT);
  myFighterBonusFeatsTable.insert(FEAT_RAPID_SHOT);
  myFighterBonusFeatsTable.insert(FEAT_MANY_SHOT);
  myFighterBonusFeatsTable.insert(FEAT_SHOT_ON_THE_RUN);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_PRECISE_SHOT);
  myFighterBonusFeatsTable.insert(FEAT_POWER_ATTACK);
  myFighterBonusFeatsTable.insert(FEAT_CLEAVE);
  myFighterBonusFeatsTable.insert(FEAT_GREAT_CLEAVE);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_BULL_RUSH);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_OVERRUN);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_SUNDER);
  myFighterBonusFeatsTable.insert(FEAT_QUICK_DRAW);
  myFighterBonusFeatsTable.insert(FEAT_RAPID_RELOAD);
  myFighterBonusFeatsTable.insert(FEAT_TWO_WEAPON_FIGHTING);
  myFighterBonusFeatsTable.insert(FEAT_TWO_WEAPON_DEFENSE);
  myFighterBonusFeatsTable.insert(FEAT_IMPROVED_TWO_WEAPON_FIGHTING);
  myFighterBonusFeatsTable.insert(FEAT_GREATER_TWO_WEAPON_FIGHTING);
  myFighterBonusFeatsTable.insert(FEAT_WEAPON_FINESSE);
  myFighterBonusFeatsTable.insert(FEAT_WEAPON_FOCUS);
  myFighterBonusFeatsTable.insert(FEAT_WEAPON_SPECIALIZATION);
  myFighterBonusFeatsTable.insert(FEAT_GREATER_WEAPON_FOCUS);
  myFighterBonusFeatsTable.insert(FEAT_GREATER_WEAPON_SPECIALIZATION);

  // clean table
  myWizardBonusFeatsTable.clear();

  myWizardBonusFeatsTable.insert(FEAT_EMPOWER_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_ENLARGE_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_EXTEND_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_HEIGHTEN_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_MAXIMIZE_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_QUICKEN_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_SILENT_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_WIDEN_SPELL);
  myWizardBonusFeatsTable.insert(FEAT_BREW_POTION);
  myWizardBonusFeatsTable.insert(FEAT_CRAFT_MAGIC_ARMS_AND_ARMOR);
  myWizardBonusFeatsTable.insert(FEAT_CRAFT_ROD);
  myWizardBonusFeatsTable.insert(FEAT_CRAFT_STAFF);
  myWizardBonusFeatsTable.insert(FEAT_CRAFT_WAND);
  myWizardBonusFeatsTable.insert(FEAT_CRAFT_WONDROUS_ITEM);
  myWizardBonusFeatsTable.insert(FEAT_FORGE_RING);
  myWizardBonusFeatsTable.insert(FEAT_SCRIBE_SCROLL);
  myWizardBonusFeatsTable.insert(FEAT_SPELL_MASTERY);
}

void RPG_Character_Skills_Common_Tools::initFeatPrerequisitesTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initFeatPrerequisitesTable"));

  // clean tables
  myFeatPrerequisitesTable.clear();

  RPG_Character_Feat_Prerequisite prerequisite;
  prerequisite.minValue = 0;
  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;
  prerequisite.skill = RPG_COMMON_SKILL_INVALID;
  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_INVALID;
  prerequisite.requiredAbility = RPG_CHARACTER_ABILITY_INVALID;

  RPG_Character_Feat_Prerequisites_t prerequisites;

  // FEAT_ARMOR_PROFICIENCY_HEAVY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_ARMOR_MEDIUM;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_ARMOR_LIGHT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_HEAVY, prerequisites));
  prerequisites.clear();

  // FEAT_ARMOR_PROFICIENCY_MEDIUM
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_ARMOR_LIGHT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_MEDIUM, prerequisites));
  prerequisites.clear();

  // FEAT_AUGMENT_SUMMONING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_SPELL_FOCUS;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_CLEAVE, prerequisites));
  prerequisites.clear();

  // FEAT_COMBAT_EXPERTISE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_DEFLECT_ARROWS, prerequisites));
  prerequisites.clear();

  // FEAT_DODGE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
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
  prerequisite.restrictedSubClasses.insert(SUBCLASS_PALADIN);
  prerequisite.restrictedSubClasses.insert(SUBCLASS_CLERIC);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_EXTRA_TURNING, prerequisites));
  prerequisites.clear();

  // FEAT_FAR_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_FAR_SHOT, prerequisites));
  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;
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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_CLEAVE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREATER_SPELL_PENETRATION, prerequisites));
  prerequisites.clear();

  // FEAT_GREATER_TWO_WEAPON_FIGHTING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 19;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 8;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.insert(SUBCLASS_FIGHTER);
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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 12;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.insert(SUBCLASS_FIGHTER);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_GREATER_WEAPON_SPECIALIZATION, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_BULL_RUSH
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_BULL_RUSH, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_CRITICAL
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_MARTIAL_WEAPONS;
  // TODO requires proficiency for a SPECIFIC weapon !
  // FEAT_PROFICIENCY_SIMPLE_WEAPONS
  // FEAT_PROFICIENCY_EXOTIC_WEAPONS
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_DISARM, prerequisites));
  prerequisites.clear();

  // TODO: FEAT_IMPROVED_FAMILIAR

  // FEAT_IMPROVED_FEINT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_FEINT, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_GRAPPLE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_GRAPPLE, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_OVERRUN
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_OVERRUN, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_PRECISE_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 19;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  prerequisite.requiredOtherFeat = FEAT_PRECISE_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_SHIELD_BASH, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_SUNDER
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POWER_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_SUNDER, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_TRIP
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_INTELLIGENCE;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_COMBAT_EXPERTISE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_TRIP, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_TURNING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  // ability to turn or rebuke creatures
  prerequisite.restrictedSubClasses.insert(SUBCLASS_PALADIN);
  prerequisite.restrictedSubClasses.insert(SUBCLASS_CLERIC);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_IMPROVED_TURNING, prerequisites));
  prerequisites.clear();

  // FEAT_IMPROVED_TWO_WEAPON_FIGHTING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 17;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  prerequisite.requiredOtherFeat = FEAT_RAPID_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_DODGE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MOBILITY, prerequisites));
  prerequisites.clear();

  // FEAT_MOUNTED_ARCHERY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = RPG_COMMON_SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MOUNTED_ARCHERY, prerequisites));
  prerequisites.clear();

  // FEAT_MOUNTED_COMBAT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = RPG_COMMON_SKILL_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_MOUNTED_COMBAT, prerequisites));
  prerequisites.clear();

  // FEAT_NATURAL_SPELL
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_WISDOM;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_ABILITY;
  prerequisite.requiredAbility = ABILITY_ALTERNATE_FORM;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredAbility = RPG_CHARACTER_ABILITY_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_NATURAL_SPELL, prerequisites));
  prerequisites.clear();

  // FEAT_POWER_ATTACK
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_STRENGTH;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_POWER_ATTACK, prerequisites));
  prerequisites.clear();

  // FEAT_PRECISE_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_RAPID_RELOAD, prerequisites));
  prerequisites.clear();

  // FEAT_RAPID_SHOT
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_RAPID_SHOT, prerequisites));
  prerequisites.clear();

  // FEAT_RIDE_BY_ATTACK
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = RPG_COMMON_SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_DODGE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_MOBILITY;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_POINT_BLANK_SHOT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_DEFLECT_ARROWS;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SNATCH_ARROWS, prerequisites));
  prerequisites.clear();

  // FEAT_SPELL_MASTERY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCASTERLEVEL;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.insert(SUBCLASS_WIZARD);
  prerequisites.push_back(prerequisite);

  prerequisite.restrictedSubClasses.clear();

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SPELL_MASTERY, prerequisites));
  prerequisites.clear();

  // FEAT_SPIRITED_CHARGE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = RPG_COMMON_SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_RIDE_BY_ATTACK;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_SPIRITED_CHARGE, prerequisites));
  prerequisites.clear();

  // FEAT_STUNNING_FIST
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.attribute = ATTRIBUTE_WISDOM;
  prerequisite.minValue = 13;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_IMPROVED_UNARMED_STRIKE;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS;
  prerequisite.minValue = 8;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_STUNNING_FIST, prerequisites));
  prerequisites.clear();

  // FEAT_PROFICIENCY_TOWER_SHIELD
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_SHIELD;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_TOWER_SHIELD, prerequisites));
  prerequisites.clear();

  // FEAT_TRAMPLE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINSKILLRANK;
  prerequisite.skill = SKILL_RIDE;
  prerequisite.minValue = 1;
  prerequisites.push_back(prerequisite);

  prerequisite.skill = RPG_COMMON_SKILL_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_MOUNTED_COMBAT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_TRAMPLE, prerequisites));
  prerequisites.clear();

  // FEAT_TWO_WEAPON_DEFENSE
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 15;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_TWO_WEAPON_FIGHTING;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_TWO_WEAPON_DEFENSE, prerequisites));
  prerequisites.clear();

  // FEAT_TWO_WEAPON_FIGHTING
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE;
  prerequisite.attribute = ATTRIBUTE_DEXTERITY;
  prerequisite.minValue = 15;
  prerequisites.push_back(prerequisite);

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_MINCLASSLEVEL;
  prerequisite.minValue = 4;
  prerequisites.push_back(prerequisite);

  prerequisite.minValue = 0;

  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES;
  prerequisite.restrictedSubClasses.insert(SUBCLASS_FIGHTER);
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

  prerequisite.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
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

  prerequisite.requiredOtherFeat = RPG_CHARACTER_FEAT_INVALID;

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
  skillSet.insert(SKILL_KNOWLEDGE_NOBILITY_ROYALTY);
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
  skillSet.insert(SKILL_KNOWLEDGE_ARCHITECTURE_ENGINEERING);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_KNOWLEDGE_NOBILITY_ROYALTY);
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
  skillSet.insert(SKILL_KNOWLEDGE_ARCHITECTURE_ENGINEERING);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_KNOWLEDGE_NOBILITY_ROYALTY);
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

const bool RPG_Character_Skills_Common_Tools::isClassSkill(const RPG_Common_SubClass& subClass_in,
                                                           const RPG_Common_Skill& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::isClassSkill"));

  RPG_Character_ClassSkillsTableIterator_t iterator = myClassSkillsTable.find(subClass_in);
  if (iterator == myClassSkillsTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("isClassSkill(\"%s\", \"%s\"): missing table for subclass, aborting\n"),
               RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str(),
               RPG_Common_SkillHelper::RPG_Common_SkillToString(skill_in).c_str()));

    return false;
  } // end IF

  return (iterator->second.find(skill_in) != iterator->second.end());
}

const unsigned int RPG_Character_Skills_Common_Tools::getSkillPoints(const RPG_Common_SubClass& subClass_in,
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
    // *TODO*: don't know if this makes sense...
    case SUBCLASS_WARLORD:
    case SUBCLASS_WARLOCK:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
    case SUBCLASS_SHAMAN:
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
    case SUBCLASS_THIEF:
    {
      baseValue += 8;

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: \"%s\" --> check implementation !, aborting\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

      return 0;
    }
  } // end SWITCH

  // at least 1/level...
  initialPoints_out = ((baseValue > 1) ? (baseValue * 4) : 1);

  return ((baseValue > 1) ? baseValue : 1);
}

const unsigned int RPG_Character_Skills_Common_Tools::getNumFeatsAbilities(const RPG_Character_Race& race_in,
                                                                           const RPG_Common_SubClass& subClass_in,
                                                                           const unsigned char& currentLevel_in,
                                                                           RPG_Character_Feats_t& baseFeats_out,
                                                                           unsigned int& numInitialFeats_out,
                                                                           RPG_Character_Abilities_t& baseAbilities_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::getNumFeatsAbilities"));

  // init defaults
  baseFeats_out.clear();
  baseAbilities_out.clear();
  // everybody starts with one feat
  numInitialFeats_out = 1;
  // everybody gets one new feat per level
  unsigned int numFeats = 1;

  switch (subClass_in)
  {
    case SUBCLASS_FIGHTER:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_MARTIAL_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_MEDIUM);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_HEAVY);
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);
      baseFeats_out.insert(FEAT_PROFICIENCY_TOWER_SHIELD);

      // fighters get a bonus (combat-oriented) feat at levels 1, 2, 4, 6, 8, 10, ...
      if (currentLevel_in == 1)
      {
        numInitialFeats_out += 1;
      } // end IF
      else if ((currentLevel_in % 2) == 0)
      {
        numFeats += 1;
      } // end ELSE

      break;
    }
    case SUBCLASS_PALADIN:
    case SUBCLASS_WARLORD:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_MARTIAL_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_MEDIUM);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_HEAVY);
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);

      baseAbilities_out.insert(ABILITY_SPELLS);

      break;
    }
    case SUBCLASS_WIZARD:
    {
      // TODO: proficient with dagger, club, crossbow (heavy, light) and quarterstaff ONLY
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      baseFeats_out.insert(FEAT_SCRIBE_SCROLL);

      baseAbilities_out.insert(ABILITY_SPELLS);

      // wizards get a bonus (magic-oriented) feat at levels 5, 10, 15, 20, ...
      if ((currentLevel_in % 5) == 0)
      {
        numFeats += 1;
      } // end ELSE

      break;
    }
    case SUBCLASS_SORCERER:
    case SUBCLASS_WARLOCK:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);

      baseAbilities_out.insert(ABILITY_SPELLS);

      break;
    }
    case SUBCLASS_CLERIC:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      // TODO: a cleric with the War domain receives the appropriate WEAPON_FOCUS and MARTIAL_WEAPON proficiency...
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_MEDIUM);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_HEAVY);
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);

      baseAbilities_out.insert(ABILITY_SPELLS);

      break;
    }
    case SUBCLASS_BARBARIAN:
    case SUBCLASS_SHAMAN:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_MARTIAL_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_MEDIUM);
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);

      break;
    }
    case SUBCLASS_DRUID:
    {
      // *TODO*: proficient with dagger, club, dart, quarterstaff, scimitar, sickle shortspear, sling and spear ONLY (+ natural attacks)
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      // *TODO*: prohibited from wearing metal armor !
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_MEDIUM);
      // *TODO*: prohibited from using metal shields !
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);

      baseAbilities_out.insert(ABILITY_SPELLS);
      baseAbilities_out.insert(ABILITY_NATURAL_WEAPONS);
      baseAbilities_out.insert(ABILITY_ANIMAL_COMPANION);
      if (currentLevel_in >= 2)
      {
        baseAbilities_out.insert(ABILITY_WOODLAND_STRIDE);
      } // end IF
      if (currentLevel_in >= 5)
      {
        baseAbilities_out.insert(ABILITY_ALTERNATE_FORM);
      } // end IF
      if (currentLevel_in >= 9)
      {
        baseAbilities_out.insert(ABILITY_IMMUNITY_POISON);
      } // end IF

      break;
    }
    case SUBCLASS_MONK:
    {
      // *TODO*: proficient with dagger, club, crossbow (light and heavy), handaxe, javelin,
      //         kama, nunchaku, quarterstaff, sai, shuriken, siangham and sling ONLY
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      baseFeats_out.insert(FEAT_IMPROVED_UNARMED_STRIKE);
      // *TODO*: choose EITHER of these two at first level
      baseFeats_out.insert(FEAT_IMPROVED_GRAPPLE);
      baseFeats_out.insert(FEAT_STUNNING_FIST);
      if (currentLevel_in == 2)
      {
        // *TODO*: choose EITHER of these two
        baseFeats_out.insert(FEAT_COMBAT_REFLEXES);
        baseFeats_out.insert(FEAT_DEFLECT_ARROWS);
      } // end IF
      if (currentLevel_in == 6)
      {
        // *TODO*: choose EITHER of these two
        baseFeats_out.insert(FEAT_IMPROVED_DISARM);
        baseFeats_out.insert(FEAT_IMPROVED_TRIP);
      } // end IF

      if (currentLevel_in >= 2)
      {
        baseAbilities_out.insert(ABILITY_EVASION);
      } // end IF
      if (currentLevel_in >= 5)
      {
        baseAbilities_out.insert(ABILITY_IMMUNITY_DISEASE);
      } // end IF
      if (currentLevel_in >= 11)
      {
        baseAbilities_out.insert(ABILITY_IMMUNITY_POISON);
      } // end IF
      if (currentLevel_in >= 13)
      {
        baseAbilities_out.insert(ABILITY_RESISTANCE_TO_SPELL);
      } // end IF
      if (currentLevel_in >= 19)
      {
        baseAbilities_out.insert(ABILITY_ETHEREALNESS);
      } // end IF
      if (currentLevel_in >= 20)
      {
        baseAbilities_out.insert(ABILITY_RESISTANCE_TO_DAMAGE);
      } // end IF

      break;
    }
    case SUBCLASS_RANGER:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_MARTIAL_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);
      baseFeats_out.insert(FEAT_TRACK);
      if (currentLevel_in >= 2)
      {
        // *TODO*: choose EITHER of these two
        baseFeats_out.insert(FEAT_RAPID_SHOT);
        baseFeats_out.insert(FEAT_TWO_WEAPON_FIGHTING);
      } // end IF
      if (currentLevel_in >= 3)
      {
        baseFeats_out.insert(FEAT_ENDURANCE);
      } // end IF
      if (currentLevel_in >= 6)
      {
        // *TODO*: choose EITHER of these two
        baseFeats_out.insert(FEAT_MANY_SHOT);
        baseFeats_out.insert(FEAT_IMPROVED_TWO_WEAPON_FIGHTING);
      } // end IF
      if (currentLevel_in >= 11)
      {
        // *TODO*: choose EITHER of these two
        baseFeats_out.insert(FEAT_IMPROVED_PRECISE_SHOT);
        baseFeats_out.insert(FEAT_GREATER_TWO_WEAPON_FIGHTING);
      } // end IF

      if (currentLevel_in >= 4)
      {
        baseAbilities_out.insert(ABILITY_ANIMAL_COMPANION);
        baseAbilities_out.insert(ABILITY_SPELLS);
      } // end IF
      if (currentLevel_in >= 7)
      {
        baseAbilities_out.insert(ABILITY_WOODLAND_STRIDE);
      } // end IF
      if (currentLevel_in >= 9)
      {
        baseAbilities_out.insert(ABILITY_EVASION);
      } // end IF

      break;
    }
    case SUBCLASS_BARD:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      // *TODO*: proficient with longsword, rapier, sap, short sword, shortbow and whip ONLY
      baseFeats_out.insert(FEAT_PROFICIENCY_MARTIAL_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);
      baseFeats_out.insert(FEAT_PROFICIENCY_SHIELD);

      baseAbilities_out.insert(ABILITY_SPELLS);

      break;
    }
    case SUBCLASS_THIEF:
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);
      // *TODO*: proficient with hand crossbow, rapier, sap, short sword and shortbow ONLY
      baseFeats_out.insert(FEAT_PROFICIENCY_MARTIAL_WEAPONS);
      baseFeats_out.insert(FEAT_PROFICIENCY_ARMOR_LIGHT);

      if (currentLevel_in >= 2)
      {
        baseAbilities_out.insert(ABILITY_EVASION);
      } // end IF
      if (currentLevel_in >= 3)
      {
        baseAbilities_out.insert(ABILITY_SENSE_TRAPS);
      } // end IF
      if (currentLevel_in >= 4)
      {
        baseAbilities_out.insert(ABILITY_UNCANNY_DODGE);
      } // end IF
      if (currentLevel_in >= 8)
      {
        baseAbilities_out.insert(ABILITY_IMPROVED_UNCANNY_DODGE);
      } // end IF
      if (currentLevel_in >= 10)
      {
        unsigned int numChoices = 1 + ((currentLevel_in - 10) / 3);
        // choose numChoices among these...
        RPG_Character_Abilities_t choiceList;
        choiceList.insert(ABILITY_CRIPPLING_STRIKE);
        choiceList.insert(ABILITY_DEFENSIVE_ROLL);
        choiceList.insert(ABILITY_IMPROVED_EVASION);
        choiceList.insert(ABILITY_OPPORTUNIST);
        choiceList.insert(ABILITY_SKILL_MASTERY);
        choiceList.insert(ABILITY_SLIPPERY_MIND);
        choiceList.insert(ABILITY_BONUS_FEAT);

        // sanity check: make sure we can satisfy this requirement !!!
        unsigned int options = 0;
        for (RPG_Character_AbilitiesConstIterator_t iterator = choiceList.begin();
             iterator != choiceList.end();
             iterator++)
        {
          if (baseAbilities_out.find(*iterator) == baseAbilities_out.end())
            options++;
        } // end FOR
        if (options < numChoices)
        {
          // debug info
          ACE_DEBUG((LM_WARNING,
                     ACE_TEXT("%d/%d abilities already acquired --> cannot choose %d among them\n"),
                     choiceList.size() - options,
                     choiceList.size(),
                     numChoices));

          // what else can we do ?
          numChoices = options;
        } // end IF

        RPG_Character_AbilitiesConstIterator_t iterator;
        RPG_Dice_RollResult_t result;
        std::pair<RPG_Character_AbilitiesIterator_t, bool> position;
        while (numChoices)
        {
          // choose random ability
          iterator = choiceList.begin();
          result.clear();
          RPG_Dice::generateRandomNumbers(choiceList.size(),
                                          1,
                                          result);
          std::advance(iterator, result.front() - 1);
          position = baseAbilities_out.insert(*iterator);

          if (position.second == false)
            continue; // already have this ability --> try again...

          numChoices--;
        } // end WHILE
      } // end IF

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: \"%s\" --> check implementation !, aborting\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

      return 0;
    }
  } // end SWITCH

  // humans get one additional initial feat
  if (race_in == RACE_HUMAN)
  {
    numInitialFeats_out += 1;
  } // end IF

  return numFeats;
}

const bool RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites(const RPG_Character_Feat& feat_in,
                                                                     const RPG_Common_SubClass& subClass_in,
                                                                     const unsigned char& currentLevel_in,
                                                                     const RPG_Character_Attributes& attributes_in,
                                                                     const RPG_Character_Skills_t& skills_in,
                                                                     const RPG_Character_Feats_t& feats_in,
                                                                     const RPG_Character_Abilities_t& abilities_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites"));

  // debug info
  if (RPG_Character_FeatHelper::RPG_Character_FeatToString(feat_in) == ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_FEAT_INVALID"))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid feat: %d --> check implementation !, aborting\n"),
               feat_in));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("checking feat: \"%s\"...\n"),
//              iterator->second.c_str()));

  // find feat prerequisites
  RPG_Character_FeatPrerequisitesTableIterator_t iterator2 = myFeatPrerequisitesTable.find(feat_in);
  if (iterator2 == myFeatPrerequisitesTable.end())
  {
    // apparently there are no prerequisites...
    return true;
  } // end IF

  // iterate over prerequisites
  for (RPG_Character_Feat_PrerequisitesIterator_t iterator3 = iterator2->second.begin();
       iterator3 != iterator2->second.end();
       iterator3++)
  {
    switch ((*iterator3).prerequisiteType)
    {
      case FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE:
      {
        switch ((*iterator3).attribute)
        {
          case ATTRIBUTE_STRENGTH:
          {
            if (attributes_in.strength >= (*iterator3).minValue)
            {
              // OK
              continue;
            } // end IF

            break;
          }
          case ATTRIBUTE_DEXTERITY:
          {
            if (attributes_in.dexterity >= (*iterator3).minValue)
            {
              // OK
              continue;
            } // end IF

            break;
          }
          case ATTRIBUTE_CONSTITUTION:
          {
            if (attributes_in.constitution >= (*iterator3).minValue)
            {
              // OK
              continue;
            } // end IF

            break;
          }
          case ATTRIBUTE_INTELLIGENCE:
          {
            if (attributes_in.intelligence >= (*iterator3).minValue)
            {
              // OK
              continue;
            } // end IF

            break;
          }
          case ATTRIBUTE_WISDOM:
          {
            if (attributes_in.wisdom >= (*iterator3).minValue)
            {
              // OK
              continue;
            } // end IF

            break;
          }
          case ATTRIBUTE_CHARISMA:
          {
            if (attributes_in.charisma >= (*iterator3).minValue)
            {
              // OK
              continue;
            } // end IF

            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid attribute: \"%s\" --> check implementation !, aborting\n"),
                       RPG_Common_AttributeHelper::RPG_Common_AttributeToString((*iterator3).attribute).c_str()));

            break;
          }
        } // end SWITCH

//         // attribute test failed
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: \"%s\" < %d\n"),
//                    iterator->second.c_str(),
//                    RPG_Character_Common_Tools::attributeToString((*iterator3).attribute).c_str(),
//                    (*iterator3).minValue));

        return false;
      }
      case FEAT_PREREQUISITETYPE_OTHERFEAT:
      {
        if (feats_in.find((*iterator3).requiredOtherFeat) != feats_in.end())
        {
          // OK
          continue;
        } // end IF

//         // other feat test failed
//         RPG_Character_Feat2StringTableIterator_t iterator4 = myFeatToStringTable.find((*iterator3).requiredOtherFeat);
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: missing feat \"%s\"\n"),
//                    iterator->second.c_str(),
//                    iterator4->second.c_str()));

        return false;
      }
      case FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS:
      {
        RPG_Character_BaseAttackBonus_t BAB = RPG_Character_Common_Tools::getBaseAttackBonus(subClass_in,
                                                                                             currentLevel_in);
        if (BAB.front() >= (*iterator3).minValue)
        {
          // OK
          continue;
        } // end IF

//         // min base attack bonus test failed
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: %d < %d\n"),
//                    iterator->second.c_str(),
//                    BAB.front(),
//                    (*iterator3).minValue));

        return false;
      }
      case FEAT_PREREQUISITETYPE_MINSKILLRANK:
      {
        RPG_Character_SkillsConstIterator_t iterator4 = skills_in.find((*iterator3).skill);
        if (iterator4 != skills_in.end())
        {
          if (iterator4->second >= (*iterator3).minValue)
          {
            // OK
            continue;
          } // end IF
        } // end IF

//         // min skill rank test failed
//         RPG_Character_Skill2StringTableIterator_t iterator5 = mySkillToStringTable.find((*iterator3).skill);
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: \"%s\" < %d\n"),
//                    iterator->second.c_str(),
//                    iterator5->second.c_str(),
//                    (*iterator3).minValue));

        return false;
      }
      case FEAT_PREREQUISITETYPE_MINCLASSLEVEL:
      {
        if (((*iterator3).restrictedSubClasses.find(subClass_in) != (*iterator3).restrictedSubClasses.end()) &&
            (currentLevel_in < (*iterator3).minValue))
        {
          // OK
          continue;
        } // end IF

//         // min class level failed
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: \"%s\" < %d\n"),
//                    iterator->second.c_str(),
//                    RPG_Character_Common_Tools::subClassToString(subClass_in).c_str(),
//                    (*iterator3).minValue));

        return false;
      }
      case FEAT_PREREQUISITETYPE_MINCASTERLEVEL:
      {
        if ((abilities_in.find(ABILITY_SPELLS) != abilities_in.end()) &&
            (currentLevel_in < (*iterator3).minValue))
        {
          // OK
          continue;
        } // end IF

//         // min caster level failed
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: \"%s\", OR %d < %d\n"),
//                    iterator->second.c_str(),
//                    RPG_Character_Common_Tools::subClassToString(subClass_in).c_str(),
//                    currentLevel_in,
//                    (*iterator3).minValue));

        return false;
      }
      case FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES:
      {
        if ((*iterator3).restrictedSubClasses.find(subClass_in) != (*iterator3).restrictedSubClasses.end())
        {
          // OK
          continue;
        } // end IF

//         // restricted subclasses test failed
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: invalid class \"%s\"\n"),
//                    iterator->second.c_str(),
//                    RPG_Character_Common_Tools::subClassToString(subClass_in).c_str()));

        return false;
      }
      case FEAT_PREREQUISITETYPE_ABILITY:
      {
        if (abilities_in.find((*iterator3).requiredAbility) != abilities_in.end())
        {
          // OK
          continue;
        } // end IF

//         // ability test failed
//         RPG_Character_Ability2StringTableIterator_t iterator4 = myAbilityToStringTable.find((*iterator3).requiredAbility);
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("prerequisite of feat \"%s\" not met: missing ability \"%s\"\n"),
//                    iterator->second.c_str(),
//                    iterator4->second.c_str()));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid prerequisite type: %d --> check implementation !, aborting\n"),
                   (*iterator3).prerequisiteType));

        return false;
      }
    } // end SWITCH
  } // end FOR

  return true;
}

const std::string RPG_Character_Skills_Common_Tools::skillsToString(const RPG_Character_Skills_t& skills_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::skillsToString"));

  std::string result;

  std::stringstream str;
  for (RPG_Character_SkillsConstIterator_t iterator = skills_in.begin();
       iterator != skills_in.end();
       iterator++)
  {
    result += RPG_Common_SkillHelper::RPG_Common_SkillToString(iterator->first);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    str.str(ACE_TEXT_ALWAYS_CHAR(""));
    str << ACE_static_cast(int, iterator->second);
    result += str.str();
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  return result;
}

const std::string RPG_Character_Skills_Common_Tools::featsToString(const RPG_Character_Feats_t& feats_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::featsToString"));

  std::string result;
  for (RPG_Character_FeatsConstIterator_t iterator = feats_in.begin();
       iterator != feats_in.end();
       iterator++)
  {
    result += RPG_Character_FeatHelper::RPG_Character_FeatToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  return result;
}

const std::string RPG_Character_Skills_Common_Tools::abilitiesToString(const RPG_Character_Abilities_t& abilities_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::abilitiesToString"));

  std::string result;
  for (RPG_Character_AbilitiesConstIterator_t iterator = abilities_in.begin();
       iterator != abilities_in.end();
       iterator++)
  {
    result += RPG_Character_AbilityHelper::RPG_Character_AbilityToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  return result;
}
