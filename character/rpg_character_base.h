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
#ifndef RPG_CHARACTER_BASE_H
#define RPG_CHARACTER_BASE_H

#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_alignment.h"
#include "rpg_character_skill.h"
#include "rpg_character_feat.h"
#include "rpg_character_ability.h"
#include "rpg_character_condition.h"
#include "rpg_character_attributes.h"
#include "rpg_character_skills_common.h"
#include "rpg_character_inventory_common.h"
#include "rpg_character_inventory.h"
#include "rpg_character_equipment.h"

#include <ace/Global_Macros.h>

#include <string>

/**
base class of all PCs, NPCs and monsters

	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Base
{
 public:
  virtual ~RPG_Character_Base();

  // retrieve basic character data
  const std::string getName() const;
  const RPG_Character_Alignment getAlignment() const;

  // retrieve base attributes
  const unsigned char getStrength() const;
  const unsigned char getDexterity() const;
  const unsigned char getConstitution() const;
  const unsigned char getIntelligence() const;
  const unsigned char getWisdom() const;
  const unsigned char getCharisma() const;

  // retrieve skill value (if any)
  void getSkill(const RPG_Character_Skill&, // skill
                unsigned char&) const;      // result: value (0: doesn't exist)

  const bool hasFeat(const RPG_Character_Feat&) const; // feat

  const bool hasAbility(const RPG_Character_Ability&) const; // ability

  const unsigned short int getNumTotalHitPoints() const;
  const unsigned short int getNumCurrentHitPoints() const;
  const RPG_Character_Condition getCondition() const;

  const unsigned int getCurrentWealth() const;

  virtual void dump() const;

 protected:
  RPG_Character_Base(// base attributes
                     const std::string&,                // name
                     const RPG_Character_Alignment&,    // (starting) alignment
                     const RPG_Character_Attributes&,   // base attributes
                     const RPG_Character_Skills_t&,     // (starting) skills
                     const RPG_Character_Feats_t&,      // base feats
                     const RPG_Character_Abilities_t&,  // base abilities
                     const unsigned short int&,         // (starting) HP
                     const unsigned int&,               // (starting) wealth (GP)
                     // base items
                     const RPG_Item_List_t&);           // (starting) list of (carried) items

  std::string               myName;
  RPG_Character_Alignment   myAlignment;

  RPG_Character_Attributes  myAttributes;

  RPG_Character_Skills_t    mySkills;

  RPG_Character_Feats_t     myFeats;

  RPG_Character_Abilities_t myAbilities;

  unsigned short int        myNumTotalHitPoints;
  unsigned short int        myNumCurrentHitPoints;
  RPG_Character_Condition   myCondition;

  unsigned int              myCurrentWealth;

  RPG_Character_Equipment   myEquipment;
  RPG_Character_Inventory   myInventory;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base(const RPG_Character_Base&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base& operator=(const RPG_Character_Base&));
};

#endif
