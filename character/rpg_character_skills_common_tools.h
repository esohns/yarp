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

#ifndef RPG_CHARACTER_SKILLS_COMMON_TOOLS_H
#define RPG_CHARACTER_SKILLS_COMMON_TOOLS_H

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_character_incl.h"

#include "rpg_character_exports.h"
#include "rpg_character_common.h"
#include "rpg_character_skills_common.h"
#include "rpg_character_common_tools.h"

#include <ace/Global_Macros.h>

#include <set>
#include <map>
#include <vector>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Export RPG_Character_Skills_Common_Tools
{
  // allow access to initialization methods
  friend class RPG_Character_Common_Tools;

 public:
  static const bool isClassSkill(const RPG_Common_SubClass&, // subclass
                                 const RPG_Common_Skill&);   // skill
  static const unsigned int getSkillPoints(const RPG_Common_SubClass&, // subclass
                                           const short int&,           // INT modifier
                                           unsigned int&);             // return value: initial points (level 1)
  static const unsigned int getNumFeatsAbilities(const RPG_Character_Race&,   // race
                                                 const RPG_Common_SubClass&,  // subclass
                                                 const unsigned char&,        // current level
                                                 RPG_Character_Feats_t&,      // return value: base feats
                                                 unsigned int&,               // return value: initial feats (level 1)
                                                 RPG_Character_Abilities_t&); // return value: base abilities

  static const bool meetsFeatPrerequisites(const RPG_Character_Feat&,         // feat
                                           const RPG_Common_SubClass&,        // subclass
                                           const unsigned char&,              // current level
                                           const RPG_Character_Attributes&,   // base attributes
                                           const RPG_Character_Skills_t&,     // skills
                                           const RPG_Character_Feats_t&,      // feats
                                           const RPG_Character_Abilities_t&); // abilities

  // debug info
  static const std::string skillsToString(const RPG_Character_Skills_t&); // skills
  static const std::string featsToString(const RPG_Character_Feats_t&); // feats
  static const std::string abilitiesToString(const RPG_Character_Abilities_t&); // abilities

  static RPG_Character_Feats_t                myFighterBonusFeatsTable;
  static RPG_Character_Feats_t                myWizardBonusFeatsTable;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Skills_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Skills_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Skills_Common_Tools(const RPG_Character_Skills_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Skills_Common_Tools& operator=(const RPG_Character_Skills_Common_Tools&));

  // some useful types
  typedef std::set<RPG_Common_Skill> RPG_Character_ClassSkillsSet_t;
  typedef RPG_Character_ClassSkillsSet_t::const_iterator RPG_Character_ClassSkillsSetIterator_t;
  typedef std::map<RPG_Common_SubClass, RPG_Character_ClassSkillsSet_t> RPG_Character_ClassSkillsTable_t;
  typedef RPG_Character_ClassSkillsTable_t::const_iterator RPG_Character_ClassSkillsTableIterator_t;
  typedef std::vector<RPG_Character_Feat_Prerequisite> RPG_Character_Feat_Prerequisites_t;
  typedef RPG_Character_Feat_Prerequisites_t::const_iterator RPG_Character_Feat_PrerequisitesIterator_t;
  typedef std::map<RPG_Character_Feat, RPG_Character_Feat_Prerequisites_t> RPG_Character_FeatPrerequisitesTable_t;
  typedef RPG_Character_FeatPrerequisitesTable_t::const_iterator RPG_Character_FeatPrerequisitesTableIterator_t;

  // init static data
  static void init();

  // helper methods
  static void initStringConversionTables();
  static void initClassSkillsTable();
  static void initFeatPrerequisitesTable();
  static void initBonusFeatsTables();

  static RPG_Character_ClassSkillsTable_t       myClassSkillsTable;
  static RPG_Character_FeatPrerequisitesTable_t myFeatPrerequisitesTable;
};

#endif
