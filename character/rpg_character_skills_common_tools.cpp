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

#include <ace/Log_Msg.h>

// init statics
RPG_Character_Skills_Common_Tools::RPG_Character_SkillToStringTable_t RPG_Character_Skills_Common_Tools::mySkillToStringTable;
RPG_Character_Skills_Common_Tools::RPG_Character_FeatToStringTable_t RPG_Character_Skills_Common_Tools::myFeatToStringTable;
RPG_Character_Skills_Common_Tools::RPG_Character_AbilityToStringTable_t RPG_Character_Skills_Common_Tools::myAbilityToStringTable;
RPG_Character_Feats_t RPG_Character_Skills_Common_Tools::myFighterBonusFeatsTable;
RPG_Character_Feats_t RPG_Character_Skills_Common_Tools::myWizardBonusFeatsTable;
RPG_Character_Skills_Common_Tools::RPG_Character_ClassSkillsTable_t RPG_Character_Skills_Common_Tools::myClassSkillsTable;
RPG_Character_Skills_Common_Tools::RPG_Character_FeatPrerequisitesTable_t RPG_Character_Skills_Common_Tools::myFeatPrerequisitesTable;

void RPG_Character_Skills_Common_Tools::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::init"));

  initStringConversionTables();
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
  prerequisite.requiredOtherFeat = FEAT_INVALID;
  prerequisite.skill = SKILL_INVALID;
  prerequisite.attribute = ATTRIBUTE_INVALID;
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_INVALID;
  prerequisite.requiredAbility = ABILITY_INVALID;

  RPG_Character_Feat_Prerequisites_t prerequisites;

  // FEAT_ARMOR_PROFICIENCY_HEAVY
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_ARMOR_MEDIUM;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_ARMOR_LIGHT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_HEAVY, prerequisites));
  prerequisites.clear();

  // FEAT_ARMOR_PROFICIENCY_MEDIUM
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_ARMOR_LIGHT;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_MEDIUM, prerequisites));
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

  prerequisite.requiredOtherFeat = FEAT_INVALID;

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
  prerequisite.requiredAbility = ABILITY_ALTERNATE_FORM;
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

  // FEAT_PROFICIENCY_TOWER_SHIELD
  prerequisite.prerequisiteType = FEAT_PREREQUISITETYPE_OTHERFEAT;
  prerequisite.requiredOtherFeat = FEAT_PROFICIENCY_SHIELD;
  prerequisites.push_back(prerequisite);

  prerequisite.requiredOtherFeat = FEAT_INVALID;

  myFeatPrerequisitesTable.insert(std::make_pair(FEAT_PROFICIENCY_TOWER_SHIELD, prerequisites));
  prerequisites.clear();

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

void RPG_Character_Skills_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initStringConversionTables"));

  // clean table
  mySkillToStringTable.clear();

  mySkillToStringTable.insert(std::make_pair(SKILL_APPRAISE, ACE_TEXT_ALWAYS_CHAR("Appraise")));
  mySkillToStringTable.insert(std::make_pair(SKILL_BALANCE, ACE_TEXT_ALWAYS_CHAR("Balance")));
  mySkillToStringTable.insert(std::make_pair(SKILL_BLUFF, ACE_TEXT_ALWAYS_CHAR("Bluff")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CLIMB, ACE_TEXT_ALWAYS_CHAR("Climb")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CONCENTRATION, ACE_TEXT_ALWAYS_CHAR("Concentration")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_ALCHEMY, ACE_TEXT_ALWAYS_CHAR("Craft: Alchemy")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_FLETCHER, ACE_TEXT_ALWAYS_CHAR("Craft: Fletcher")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_BOWYER, ACE_TEXT_ALWAYS_CHAR("Craft: Bowyer")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_ARMOR, ACE_TEXT_ALWAYS_CHAR("Craft: Armorsmith")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_BLACK, ACE_TEXT_ALWAYS_CHAR("Craft: Blacksmith")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_WEAPON, ACE_TEXT_ALWAYS_CHAR("Craft: Weaponsmith")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_TRAP, ACE_TEXT_ALWAYS_CHAR("Craft: Make Traps")));
  mySkillToStringTable.insert(std::make_pair(SKILL_CRAFT_OTHER, ACE_TEXT_ALWAYS_CHAR("Craft: General")));
  mySkillToStringTable.insert(std::make_pair(SKILL_DECIPHER_SCRIPT, ACE_TEXT_ALWAYS_CHAR("Decipher Script")));
  mySkillToStringTable.insert(std::make_pair(SKILL_DIPLOMACY, ACE_TEXT_ALWAYS_CHAR("Diplomacy")));
  mySkillToStringTable.insert(std::make_pair(SKILL_DISABLE_DEVICE, ACE_TEXT_ALWAYS_CHAR("Disable Device")));
  mySkillToStringTable.insert(std::make_pair(SKILL_DISGUISE, ACE_TEXT_ALWAYS_CHAR("Disguise")));
  mySkillToStringTable.insert(std::make_pair(SKILL_ESCAPE_ARTIST, ACE_TEXT_ALWAYS_CHAR("Escape Artist")));
  mySkillToStringTable.insert(std::make_pair(SKILL_FORGERY, ACE_TEXT_ALWAYS_CHAR("Forgery")));
  mySkillToStringTable.insert(std::make_pair(SKILL_GATHER_INFORMATION, ACE_TEXT_ALWAYS_CHAR("Gather Information")));
  mySkillToStringTable.insert(std::make_pair(SKILL_HANDLE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("Handle Animal")));
  mySkillToStringTable.insert(std::make_pair(SKILL_HEAL, ACE_TEXT_ALWAYS_CHAR("Heal")));
  mySkillToStringTable.insert(std::make_pair(SKILL_HIDE, ACE_TEXT_ALWAYS_CHAR("Hide")));
  mySkillToStringTable.insert(std::make_pair(SKILL_INTIMIDATE, ACE_TEXT_ALWAYS_CHAR("Intimidate")));
  mySkillToStringTable.insert(std::make_pair(SKILL_JUMP, ACE_TEXT_ALWAYS_CHAR("Jump")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCANA, ACE_TEXT_ALWAYS_CHAR("Knowledge: Arcana")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCH_ENG, ACE_TEXT_ALWAYS_CHAR("Knowledge: Achitecture & Engineering")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_DUNGEONS, ACE_TEXT_ALWAYS_CHAR("Knowledge: Dungeoneering")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_GEOGRAPHY, ACE_TEXT_ALWAYS_CHAR("Knowledge: Geography")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_HISTORY, ACE_TEXT_ALWAYS_CHAR("Knowledge: History")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_LOCAL, ACE_TEXT_ALWAYS_CHAR("Knowledge: Local")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NATURE, ACE_TEXT_ALWAYS_CHAR("Knowledge: Nature")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NOB_ROY, ACE_TEXT_ALWAYS_CHAR("Knowledge: Nobility & Royalty")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_RELIGION, ACE_TEXT_ALWAYS_CHAR("Knowledge: Religion")));
  mySkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_PLANES, ACE_TEXT_ALWAYS_CHAR("Knowledge: The Planes")));
  mySkillToStringTable.insert(std::make_pair(SKILL_LISTEN, ACE_TEXT_ALWAYS_CHAR("Listen")));
  mySkillToStringTable.insert(std::make_pair(SKILL_MOVE_SILENTLY, ACE_TEXT_ALWAYS_CHAR("Move Silently")));
  mySkillToStringTable.insert(std::make_pair(SKILL_OPEN_LOCK, ACE_TEXT_ALWAYS_CHAR("Open Lock")));
  mySkillToStringTable.insert(std::make_pair(SKILL_PERFORM, ACE_TEXT_ALWAYS_CHAR("Perform")));
  mySkillToStringTable.insert(std::make_pair(SKILL_PROFESSION, ACE_TEXT_ALWAYS_CHAR("Profession")));
  mySkillToStringTable.insert(std::make_pair(SKILL_RIDE, ACE_TEXT_ALWAYS_CHAR("Ride")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SEARCH, ACE_TEXT_ALWAYS_CHAR("Search")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SENSE_MOTIVE, ACE_TEXT_ALWAYS_CHAR("Sense Motive")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SLEIGHT_OF_HAND, ACE_TEXT_ALWAYS_CHAR("Sleight Of Hand")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SPEAK_LANGUAGE, ACE_TEXT_ALWAYS_CHAR("Speak Language")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SPELLCRAFT, ACE_TEXT_ALWAYS_CHAR("Spellcraft")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SPOT, ACE_TEXT_ALWAYS_CHAR("Spot")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SURVIVAL, ACE_TEXT_ALWAYS_CHAR("Survival")));
  mySkillToStringTable.insert(std::make_pair(SKILL_SWIM, ACE_TEXT_ALWAYS_CHAR("Swim")));
  mySkillToStringTable.insert(std::make_pair(SKILL_TUMBLE, ACE_TEXT_ALWAYS_CHAR("Tumble")));
  mySkillToStringTable.insert(std::make_pair(SKILL_USE_MAGIC_DEVICE, ACE_TEXT_ALWAYS_CHAR("Use Magic Device")));
  mySkillToStringTable.insert(std::make_pair(SKILL_USE_ROPE, ACE_TEXT_ALWAYS_CHAR("Use Rope")));

  // clean table
  myFeatToStringTable.clear();

  myFeatToStringTable.insert(std::make_pair(FEAT_BLIND_FIGHT, ACE_TEXT_ALWAYS_CHAR("Blind Fight")));
  myFeatToStringTable.insert(std::make_pair(FEAT_COMBAT_EXPERTISE, ACE_TEXT_ALWAYS_CHAR("Combat Expertise")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_DISARM, ACE_TEXT_ALWAYS_CHAR("Improved Disarm")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_FEINT, ACE_TEXT_ALWAYS_CHAR("Improved Feint")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_TRIP, ACE_TEXT_ALWAYS_CHAR("Improved Trip")));
  myFeatToStringTable.insert(std::make_pair(FEAT_WHIRLWIND_ATTACK, ACE_TEXT_ALWAYS_CHAR("Whirlwind Attack")));
  myFeatToStringTable.insert(std::make_pair(FEAT_COMBAT_REFLEXES, ACE_TEXT_ALWAYS_CHAR("Combat Reflexes")));
  myFeatToStringTable.insert(std::make_pair(FEAT_DODGE, ACE_TEXT_ALWAYS_CHAR("Dodge")));
  myFeatToStringTable.insert(std::make_pair(FEAT_MOBILITY, ACE_TEXT_ALWAYS_CHAR("Mobility")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SPRING_ATTACK, ACE_TEXT_ALWAYS_CHAR("Spring Attack")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_EXOTIC_WEAPONS, ACE_TEXT_ALWAYS_CHAR("Proficiency: Exotic Weapons")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_CRITICAL, ACE_TEXT_ALWAYS_CHAR("Improved Critical")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_INITIATIVE, ACE_TEXT_ALWAYS_CHAR("Improved Initiative")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_SHIELD_BASH, ACE_TEXT_ALWAYS_CHAR("Improved Shield Bash")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_UNARMED_STRIKE, ACE_TEXT_ALWAYS_CHAR("Improved Unarmed Strike")));
  myFeatToStringTable.insert(std::make_pair(FEAT_DEFLECT_ARROWS, ACE_TEXT_ALWAYS_CHAR("Deflect Arrows")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_GRAPPLE, ACE_TEXT_ALWAYS_CHAR("Improved Grapple")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SNATCH_ARROWS, ACE_TEXT_ALWAYS_CHAR("Snatch Arrows")));
  myFeatToStringTable.insert(std::make_pair(FEAT_STUNNING_FIST, ACE_TEXT_ALWAYS_CHAR("Stunning Fist")));
  myFeatToStringTable.insert(std::make_pair(FEAT_MOUNTED_COMBAT, ACE_TEXT_ALWAYS_CHAR("Mounted Combat")));
  myFeatToStringTable.insert(std::make_pair(FEAT_MOUNTED_ARCHERY, ACE_TEXT_ALWAYS_CHAR("Mounted Archery")));
  myFeatToStringTable.insert(std::make_pair(FEAT_RIDE_BY_ATTACK, ACE_TEXT_ALWAYS_CHAR("Ride-By Attack")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SPIRITED_CHARGE, ACE_TEXT_ALWAYS_CHAR("Spirited Charge")));
  myFeatToStringTable.insert(std::make_pair(FEAT_TRAMPLE, ACE_TEXT_ALWAYS_CHAR("Trample")));
  myFeatToStringTable.insert(std::make_pair(FEAT_POINT_BLANK_SHOT, ACE_TEXT_ALWAYS_CHAR("Point-Blank Shot")));
  myFeatToStringTable.insert(std::make_pair(FEAT_FAR_SHOT, ACE_TEXT_ALWAYS_CHAR("Far Shot")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PRECISE_SHOT, ACE_TEXT_ALWAYS_CHAR("Precise Shot")));
  myFeatToStringTable.insert(std::make_pair(FEAT_RAPID_SHOT, ACE_TEXT_ALWAYS_CHAR("Rapid Shot")));
  myFeatToStringTable.insert(std::make_pair(FEAT_MANY_SHOT, ACE_TEXT_ALWAYS_CHAR("Many Shot")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SHOT_ON_THE_RUN, ACE_TEXT_ALWAYS_CHAR("Shot On-The-Run")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_PRECISE_SHOT, ACE_TEXT_ALWAYS_CHAR("Improved Precise Shot")));
  myFeatToStringTable.insert(std::make_pair(FEAT_POWER_ATTACK, ACE_TEXT_ALWAYS_CHAR("Power Attack")));
  myFeatToStringTable.insert(std::make_pair(FEAT_CLEAVE, ACE_TEXT_ALWAYS_CHAR("Cleave")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREAT_CLEAVE, ACE_TEXT_ALWAYS_CHAR("Great Cleave")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_BULL_RUSH, ACE_TEXT_ALWAYS_CHAR("Improved Bull Rush")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_OVERRUN, ACE_TEXT_ALWAYS_CHAR("Improved Overrun")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_SUNDER, ACE_TEXT_ALWAYS_CHAR("Improved Sunder")));
  myFeatToStringTable.insert(std::make_pair(FEAT_QUICK_DRAW, ACE_TEXT_ALWAYS_CHAR("Quick Draw")));
  myFeatToStringTable.insert(std::make_pair(FEAT_RAPID_RELOAD, ACE_TEXT_ALWAYS_CHAR("Rapid Reload")));
  myFeatToStringTable.insert(std::make_pair(FEAT_TWO_WEAPON_FIGHTING, ACE_TEXT_ALWAYS_CHAR("Two-Weapon Fighting")));
  myFeatToStringTable.insert(std::make_pair(FEAT_TWO_WEAPON_DEFENSE, ACE_TEXT_ALWAYS_CHAR("Two-Weapon Defense")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_TWO_WEAPON_FIGHTING, ACE_TEXT_ALWAYS_CHAR("Improved Two-Weapon Fighting")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREATER_TWO_WEAPON_FIGHTING, ACE_TEXT_ALWAYS_CHAR("Greater Two-Weapon Fighting")));
  myFeatToStringTable.insert(std::make_pair(FEAT_WEAPON_FINESSE, ACE_TEXT_ALWAYS_CHAR("Weapon Finesse")));
  myFeatToStringTable.insert(std::make_pair(FEAT_WEAPON_FOCUS, ACE_TEXT_ALWAYS_CHAR("Weapon Focus")));
  myFeatToStringTable.insert(std::make_pair(FEAT_WEAPON_SPECIALIZATION, ACE_TEXT_ALWAYS_CHAR("Weapon Specialization")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREATER_WEAPON_FOCUS, ACE_TEXT_ALWAYS_CHAR("Greater Weapon Focus")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREATER_WEAPON_SPECIALIZATION, ACE_TEXT_ALWAYS_CHAR("Greater Weapon Specialization")));
  myFeatToStringTable.insert(std::make_pair(FEAT_BREW_POTION, ACE_TEXT_ALWAYS_CHAR("Brew Potion")));
  myFeatToStringTable.insert(std::make_pair(FEAT_CRAFT_MAGIC_ARMS_AND_ARMOR, ACE_TEXT_ALWAYS_CHAR("Craft Magic Arms & Armor")));
  myFeatToStringTable.insert(std::make_pair(FEAT_CRAFT_ROD, ACE_TEXT_ALWAYS_CHAR("Craft Rod")));
  myFeatToStringTable.insert(std::make_pair(FEAT_CRAFT_STAFF, ACE_TEXT_ALWAYS_CHAR("Craft Staff")));
  myFeatToStringTable.insert(std::make_pair(FEAT_CRAFT_WAND, ACE_TEXT_ALWAYS_CHAR("Craft Wand")));
  myFeatToStringTable.insert(std::make_pair(FEAT_CRAFT_WONDROUS_ITEM, ACE_TEXT_ALWAYS_CHAR("Craft Wondrous Item")));
  myFeatToStringTable.insert(std::make_pair(FEAT_FORGE_RING, ACE_TEXT_ALWAYS_CHAR("Forge Ring")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SCRIBE_SCROLL, ACE_TEXT_ALWAYS_CHAR("Scribe Scroll")));
  myFeatToStringTable.insert(std::make_pair(FEAT_EMPOWER_SPELL, ACE_TEXT_ALWAYS_CHAR("Empower Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ENLARGE_SPELL, ACE_TEXT_ALWAYS_CHAR("Enlarge Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_EXTEND_SPELL, ACE_TEXT_ALWAYS_CHAR("Extend Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_HEIGHTEN_SPELL, ACE_TEXT_ALWAYS_CHAR("Heighten Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_MAXIMIZE_SPELL, ACE_TEXT_ALWAYS_CHAR("Maximize Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_QUICKEN_SPELL, ACE_TEXT_ALWAYS_CHAR("Quicken Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SILENT_SPELL, ACE_TEXT_ALWAYS_CHAR("Silent Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_WIDEN_SPELL, ACE_TEXT_ALWAYS_CHAR("Widen Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ACROBATIC, ACE_TEXT_ALWAYS_CHAR("Acrobatic")));
  myFeatToStringTable.insert(std::make_pair(FEAT_AGILE, ACE_TEXT_ALWAYS_CHAR("Agile")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ALERTNESS, ACE_TEXT_ALWAYS_CHAR("Alertness")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ANIMAL_AFFINITY, ACE_TEXT_ALWAYS_CHAR("Animal Affinity")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_LIGHT, ACE_TEXT_ALWAYS_CHAR("Proficiency: Light Armor")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_MEDIUM, ACE_TEXT_ALWAYS_CHAR("Proficiency: Medium Armor")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_ARMOR_HEAVY, ACE_TEXT_ALWAYS_CHAR("Proficiency: Heavy Armor")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ATHLETIC, ACE_TEXT_ALWAYS_CHAR("Athletic")));
  myFeatToStringTable.insert(std::make_pair(FEAT_AUGMENT_SUMMONING, ACE_TEXT_ALWAYS_CHAR("Augment Summoning")));
  myFeatToStringTable.insert(std::make_pair(FEAT_COMBAT_CASTING, ACE_TEXT_ALWAYS_CHAR("Combat Casting")));
  myFeatToStringTable.insert(std::make_pair(FEAT_DECEITFUL, ACE_TEXT_ALWAYS_CHAR("Deceitful")));
  myFeatToStringTable.insert(std::make_pair(FEAT_DEFT_HANDS, ACE_TEXT_ALWAYS_CHAR("Deft Hands")));
  myFeatToStringTable.insert(std::make_pair(FEAT_DILIGENT, ACE_TEXT_ALWAYS_CHAR("Diligent")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ENDURANCE, ACE_TEXT_ALWAYS_CHAR("Endurance")));
  myFeatToStringTable.insert(std::make_pair(FEAT_DIE_HARD, ACE_TEXT_ALWAYS_CHAR("Diehard")));
  myFeatToStringTable.insert(std::make_pair(FEAT_ESCHEW_MATERIALS, ACE_TEXT_ALWAYS_CHAR("Eschew Materials")));
  myFeatToStringTable.insert(std::make_pair(FEAT_EXTRA_TURNING, ACE_TEXT_ALWAYS_CHAR("Extra Turning")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREAT_FORTITUDE, ACE_TEXT_ALWAYS_CHAR("Great Fortitude")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_COUNTERSPELL, ACE_TEXT_ALWAYS_CHAR("Improved Counterspell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_FAMILIAR, ACE_TEXT_ALWAYS_CHAR("Improved Familiar")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IMPROVED_TURNING, ACE_TEXT_ALWAYS_CHAR("Improved Turning")));
  myFeatToStringTable.insert(std::make_pair(FEAT_INVESTIGATOR, ACE_TEXT_ALWAYS_CHAR("Investigator")));
  myFeatToStringTable.insert(std::make_pair(FEAT_IRON_WILL, ACE_TEXT_ALWAYS_CHAR("Iron Will")));
  myFeatToStringTable.insert(std::make_pair(FEAT_LEADERSHIP, ACE_TEXT_ALWAYS_CHAR("Leadership")));
  myFeatToStringTable.insert(std::make_pair(FEAT_LIGHTNING_REFLEXES, ACE_TEXT_ALWAYS_CHAR("Lightning Reflexes")));
  myFeatToStringTable.insert(std::make_pair(FEAT_MAGICAL_APTITUDE, ACE_TEXT_ALWAYS_CHAR("Magical Aptitude")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_MARTIAL_WEAPONS, ACE_TEXT_ALWAYS_CHAR("Proficiency: Martial Weapons")));
  myFeatToStringTable.insert(std::make_pair(FEAT_NATURAL_SPELL, ACE_TEXT_ALWAYS_CHAR("Natural Spell")));
  myFeatToStringTable.insert(std::make_pair(FEAT_NEGOTIATOR, ACE_TEXT_ALWAYS_CHAR("Negotiator")));
  myFeatToStringTable.insert(std::make_pair(FEAT_NIMBLE_FINGERS, ACE_TEXT_ALWAYS_CHAR("Nimble Fingers")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PERSUASIVE, ACE_TEXT_ALWAYS_CHAR("Persuasive")));
  myFeatToStringTable.insert(std::make_pair(FEAT_RUN, ACE_TEXT_ALWAYS_CHAR("Run")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SELF_SUFFICIENT, ACE_TEXT_ALWAYS_CHAR("Self-Sufficient")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_SHIELD, ACE_TEXT_ALWAYS_CHAR("Proficiency: Shields")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_TOWER_SHIELD, ACE_TEXT_ALWAYS_CHAR("Proficiency: Tower Shield")));
  myFeatToStringTable.insert(std::make_pair(FEAT_PROFICIENCY_SIMPLE_WEAPONS, ACE_TEXT_ALWAYS_CHAR("Proficiency: Simple Weapons")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SKILL_FOCUS, ACE_TEXT_ALWAYS_CHAR("Skill Focus")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SPELL_FOCUS, ACE_TEXT_ALWAYS_CHAR("Spell Focus")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREATER_SPELL_FOCUS, ACE_TEXT_ALWAYS_CHAR("Greater Spell Focus")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SPELL_MASTERY, ACE_TEXT_ALWAYS_CHAR("Spell Mastery")));
  myFeatToStringTable.insert(std::make_pair(FEAT_SPELL_PENETRATION, ACE_TEXT_ALWAYS_CHAR("Spell Penetration")));
  myFeatToStringTable.insert(std::make_pair(FEAT_GREATER_SPELL_PENETRATION, ACE_TEXT_ALWAYS_CHAR("Greater Spell Penetration")));
  myFeatToStringTable.insert(std::make_pair(FEAT_STEALTHY, ACE_TEXT_ALWAYS_CHAR("Stealthy")));
  myFeatToStringTable.insert(std::make_pair(FEAT_TOUGHNESS, ACE_TEXT_ALWAYS_CHAR("Toughness")));
  myFeatToStringTable.insert(std::make_pair(FEAT_TRACK, ACE_TEXT_ALWAYS_CHAR("Track")));

  // clean table
  myAbilityToStringTable.clear();

  myAbilityToStringTable.insert(std::make_pair(ABILITY_SCORE_LOSS, ACE_TEXT_ALWAYS_CHAR("Ability Score Loss")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_ALTERNATE_FORM, ACE_TEXT_ALWAYS_CHAR("Alternate Form")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_ANTIMAGIC, ACE_TEXT_ALWAYS_CHAR("Antimagic")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_BLINDSIGHT, ACE_TEXT_ALWAYS_CHAR("Blindsight")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_BLINDSENSE, ACE_TEXT_ALWAYS_CHAR("Blindsense")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_BREATH_WEAPON, ACE_TEXT_ALWAYS_CHAR("Breath Weapon")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_CHANGE_SHAPE, ACE_TEXT_ALWAYS_CHAR("Change Shape")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_CHARM, ACE_TEXT_ALWAYS_CHAR("Charm")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_COMPULSION, ACE_TEXT_ALWAYS_CHAR("Compulsion")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_COLD, ACE_TEXT_ALWAYS_CHAR("Cold Immunity")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_CONSTRICT, ACE_TEXT_ALWAYS_CHAR("Constrict")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_DAMAGE, ACE_TEXT_ALWAYS_CHAR("Damage Reduction")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_DARKVISION, ACE_TEXT_ALWAYS_CHAR("Darkvision")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_DEATH_ATTACK, ACE_TEXT_ALWAYS_CHAR("Death Attack")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_DISEASE, ACE_TEXT_ALWAYS_CHAR("Disease")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_ENERGY_DRAIN, ACE_TEXT_ALWAYS_CHAR("Energy Drain")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_ETHEREALNESS, ACE_TEXT_ALWAYS_CHAR("Etherealness")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_EVASION, ACE_TEXT_ALWAYS_CHAR("Evasion")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMPROVED_EVASION, ACE_TEXT_ALWAYS_CHAR("Improved Evasion")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_FAST_HEALING, ACE_TEXT_ALWAYS_CHAR("Fast Healing")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_FEAR, ACE_TEXT_ALWAYS_CHAR("Fear")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_FIRE, ACE_TEXT_ALWAYS_CHAR("Fire Immunity")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_GASEOUS_FORM, ACE_TEXT_ALWAYS_CHAR("Gaseous Form")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_GAZE_ATTACK, ACE_TEXT_ALWAYS_CHAR("Gaze Attack")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMPROVED_GRAB, ACE_TEXT_ALWAYS_CHAR("Improved Grab")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_INCORPOREALITY, ACE_TEXT_ALWAYS_CHAR("Incorporeality")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_INVISIBILITY, ACE_TEXT_ALWAYS_CHAR("Invisibility")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_LEVEL_LOSS, ACE_TEXT_ALWAYS_CHAR("Level Loss")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_LOWLIGHT_VISION, ACE_TEXT_ALWAYS_CHAR("Low-Light Vision")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_MANUFACTURED_WEAPONS, ACE_TEXT_ALWAYS_CHAR("Manufactured Weapons")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_MOVEMENT_MODES, ACE_TEXT_ALWAYS_CHAR("Movement Modes")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_NATURAL_WEAPONS, ACE_TEXT_ALWAYS_CHAR("Natural Weapons")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_NONABILITIES, ACE_TEXT_ALWAYS_CHAR("Nonabilities")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_PARALYZE_ATTACK, ACE_TEXT_ALWAYS_CHAR("Paralyze Attack")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_POISON_ATTACK, ACE_TEXT_ALWAYS_CHAR("Poison Attack")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_POISON, ACE_TEXT_ALWAYS_CHAR("Poison Immunity")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_POLYMORPH, ACE_TEXT_ALWAYS_CHAR("Polymorph")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_POUNCE, ACE_TEXT_ALWAYS_CHAR("Pounce")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_POWERFUL_CHARGE, ACE_TEXT_ALWAYS_CHAR("Powerful Charge")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_PSIONICS, ACE_TEXT_ALWAYS_CHAR("Psionics")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_RAKE, ACE_TEXT_ALWAYS_CHAR("Rake")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_RAY_ATTACK, ACE_TEXT_ALWAYS_CHAR("Ray Attack")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_REGENERATION, ACE_TEXT_ALWAYS_CHAR("Regeneration")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_ENERGY, ACE_TEXT_ALWAYS_CHAR("Resistance To Energy")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SCENT, ACE_TEXT_ALWAYS_CHAR("Scent")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SONIC_ATTACK, ACE_TEXT_ALWAYS_CHAR("Sonic Attack")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_SPELL, ACE_TEXT_ALWAYS_CHAR("Spell Immunity")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_SPELL, ACE_TEXT_ALWAYS_CHAR("Spell Resistance")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SPELLS, ACE_TEXT_ALWAYS_CHAR("Spells")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SUMMON, ACE_TEXT_ALWAYS_CHAR("Summon")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SWALLOW_WHOLE, ACE_TEXT_ALWAYS_CHAR("Swallow Whole")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_TELEPATHY, ACE_TEXT_ALWAYS_CHAR("Telepathy")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_TRAMPLE, ACE_TEXT_ALWAYS_CHAR("Trample")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_TREMOR_SENSE, ACE_TEXT_ALWAYS_CHAR("Sense Tremors")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_TURNING, ACE_TEXT_ALWAYS_CHAR("Turn Resistance")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_VULNERABILITY_TO_ENERGY, ACE_TEXT_ALWAYS_CHAR("Vulnerability to Energy")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_DISEASE, ACE_TEXT_ALWAYS_CHAR("Disease Immunity")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_ANIMAL_COMPANION, ACE_TEXT_ALWAYS_CHAR("Animal Companion")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_WOODLAND_STRIDE, ACE_TEXT_ALWAYS_CHAR("Woodland Stride")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SENSE_TRAPS, ACE_TEXT_ALWAYS_CHAR("Sense Traps")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_UNCANNY_DODGE, ACE_TEXT_ALWAYS_CHAR("Uncanny Dodge")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_IMPROVED_UNCANNY_DODGE, ACE_TEXT_ALWAYS_CHAR("Improved Uncanny Dodge")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_CRIPPLING_STRIKE, ACE_TEXT_ALWAYS_CHAR("Crippling Strike")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_DEFENSIVE_ROLL, ACE_TEXT_ALWAYS_CHAR("Defensive Roll")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_OPPORTUNIST, ACE_TEXT_ALWAYS_CHAR("Opportunist")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SKILL_MASTERY, ACE_TEXT_ALWAYS_CHAR("Skill Mastery")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_SLIPPERY_MIND, ACE_TEXT_ALWAYS_CHAR("Slippery Mind")));
  myAbilityToStringTable.insert(std::make_pair(ABILITY_BONUS_FEAT, ACE_TEXT_ALWAYS_CHAR("Bonus Feat")));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Skills_Common_Tools: initialized string conversion tables...\n")));
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

const unsigned int RPG_Character_Skills_Common_Tools::getNumFeatsAbilities(const RPG_Character_Race& race_in,
                                                                           const RPG_Character_SubClass& subClass_in,
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
    {
      baseFeats_out.insert(FEAT_PROFICIENCY_SIMPLE_WEAPONS);

      baseAbilities_out.insert(ABILITY_SPELLS);

      break;
    }
    case SUBCLASS_CLERIC:
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
//     case SUBCLASS_WARLORD:
//     case SUBCLASS_WARLOCK:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
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
        unsigned int x = 1 + ((currentLevel_in - 10) / 3);
        // *TODO*: choose x among these...
        baseAbilities_out.insert(ABILITY_CRIPPLING_STRIKE);
        baseAbilities_out.insert(ABILITY_DEFENSIVE_ROLL);
        baseAbilities_out.insert(ABILITY_IMPROVED_EVASION);
        baseAbilities_out.insert(ABILITY_OPPORTUNIST);
        baseAbilities_out.insert(ABILITY_SKILL_MASTERY);
        baseAbilities_out.insert(ABILITY_SLIPPERY_MIND);
        baseAbilities_out.insert(ABILITY_BONUS_FEAT);
      } // end IF

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

  // humans get one additional initial feat
  if (race_in == RACE_HUMAN)
  {
    numInitialFeats_out += 1;
  } // end IF

  return numFeats;
}

const bool RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites(const RPG_Character_Feat& feat_in,
                                                                     const RPG_Character_SubClass& subClass_in,
                                                                     const unsigned char& currentLevel_in,
                                                                     const RPG_Character_Attributes& attributes_in,
                                                                     const RPG_Character_Skills_t& skills_in,
                                                                     const RPG_Character_Feats_t& feats_in,
                                                                     const RPG_Character_Abilities_t& abilities_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites"));

  // debug info
  RPG_Character_FeatToStringTableIterator_t iterator = myFeatToStringTable.find(feat_in);
  if (iterator == myFeatToStringTable.end())
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
                       RPG_Character_Common_Tools::attributeToString((*iterator3).attribute).c_str()));

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
