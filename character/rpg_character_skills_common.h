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
#ifndef RPG_CHARACTER_SKILLS_COMMON_H
#define RPG_CHARACTER_SKILLS_COMMON_H

#include <map>
#include <set>

#include "rpg_common_attribute.h"
#include "rpg_common_skill.h"

#include "rpg_character_class_common.h"
#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_character_incl.h"

enum RPG_Character_Feat_Prerequisite_Type
{
  FEAT_PREREQUISITETYPE_MINATTRIBUTESCORE = 0,
  FEAT_PREREQUISITETYPE_OTHERFEAT,
  FEAT_PREREQUISITETYPE_MINBASEATTACKBONUS,
  FEAT_PREREQUISITETYPE_MINSKILLRANK,
  FEAT_PREREQUISITETYPE_MINCLASSLEVEL,
  FEAT_PREREQUISITETYPE_MINCASTERLEVEL,
  FEAT_PREREQUISITETYPE_RESTRICTEDCLASSES,
  FEAT_PREREQUISITETYPE_ABILITY,
  //
  FEAT_PREREQUISITETYPE_MAX,
  FEAT_PREREQUISITETYPE_INVALID
};

struct RPG_Character_Feat_Prerequisite
{
  enum RPG_Character_Feat_Prerequisite_Type prerequisiteType;

  // *CONSIDER*: use a union ?
  enum RPG_Character_Feat requiredOtherFeat;       // if applicable
  enum RPG_Common_Attribute attribute;             // if applicable
  ACE_UINT8 minValue;                              // if applicable
  enum RPG_Common_Skill skill;                     // if applicable
  RPG_Character_SubClasses_t restrictedSubClasses; // if applicable
  enum RPG_Character_Ability requiredAbility;      // if applicable
};

// some useful types
typedef std::pair<enum RPG_Common_Skill, ACE_INT8> RPG_Character_SkillsItem_t;
typedef std::map<enum RPG_Common_Skill, ACE_INT8> RPG_Character_Skills_t;
// typedef RPG_Character_Skills_t::value_type RPG_Character_SkillsItem_t;
typedef RPG_Character_Skills_t::iterator RPG_Character_SkillsIterator_t;
typedef RPG_Character_Skills_t::const_iterator RPG_Character_SkillsConstIterator_t;
typedef std::multiset<RPG_Character_Feat> RPG_Character_Feats_t;
typedef RPG_Character_Feats_t::iterator RPG_Character_FeatsIterator_t;
typedef RPG_Character_Feats_t::const_iterator RPG_Character_FeatsConstIterator_t;
typedef std::set<RPG_Character_Ability> RPG_Character_Abilities_t;
typedef RPG_Character_Abilities_t::iterator RPG_Character_AbilitiesIterator_t;
typedef RPG_Character_Abilities_t::const_iterator RPG_Character_AbilitiesConstIterator_t;

#endif
