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

#include "rpg_character_common.h"
#include "rpg_character_race_common.h"
#include "rpg_character_class_common.h"
#include "rpg_character_skills_common.h"
#include "rpg_character_inventory_common.h"
#include "rpg_character_inventory.h"
#include "rpg_character_equipment.h"

#include <ace/Global_Macros.h>

/**
base class of all PCs, NPCs and monsters

	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Base
{
 public:
  virtual ~RPG_Character_Base();

  // retrieve basic character data
  const RPG_Character_Gender getGender() const;
  const RPG_Character_Race getRace() const;
  const RPG_Character_Class getClass() const;
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

  const unsigned int getExperience() const;
  // compute dynamically from class/XP
  const unsigned char getLevel() const;
  const unsigned short int getNumTotalHitPoints() const;
  const unsigned short int getNumCurrentHitPoints() const;
  const unsigned int getCurrentWealth() const;

  const RPG_Character_Condition getCondition() const;

  virtual void dump() const;

 protected:
  RPG_Character_Base(// base attributes
                     const RPG_Character_Gender&,       // gender
                     const RPG_Character_Race&,         // race
                     const RPG_Character_Class&,        // (starting) class
                     const RPG_Character_Alignment&,    // (starting) alignment
                     const RPG_Character_Attributes&,   // base attributes
                     const RPG_Character_Skills_t&,     // (starting) skills
                     const unsigned int&,               // (starting) XP
                     const unsigned short int&,         // (starting) HP
                     const unsigned int&,               // (starting) wealth (GP)
                     // base items
                     const RPG_Item_List_t&);           // (starting) list of (carried) items

  RPG_Character_Gender     myGender;
  RPG_Character_Race       myRace;
  RPG_Character_Class      myClass;
  RPG_Character_Alignment  myAlignment;
  RPG_Character_Attributes myAttributes;
  RPG_Character_Skills_t   mySkills;
  unsigned int             myExperience;

  unsigned short int       myNumTotalHitPoints;
  unsigned short int       myNumCurrentHitPoints;

  unsigned int             myCurrentWealth;

  RPG_Character_Condition  myCondition;

  RPG_Character_Inventory  myInventory;
  RPG_Character_Equipment  myEquipment;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base(const RPG_Character_Base&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base& operator=(const RPG_Character_Base&));
};

#endif
