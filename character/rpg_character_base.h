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

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_character_incl.h"

#include "rpg_character_inventory_common.h"
#include "rpg_character_inventory.h"
#include "rpg_character_equipment.h"
#include "rpg_character_common.h"
#include "rpg_character_skills_common.h"

#include <rpg_magic_common.h>

#include <rpg_combat_incl.h>

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

  const RPG_Character_Conditions_t getCondition() const;

  // retrieve base attributes
  const unsigned char getAttribute(const RPG_Common_Attribute&) const;

  const RPG_Character_Feats_t getFeats() const;
  const RPG_Character_Abilities_t getAbilities() const;
  const RPG_Character_Skills_t getSkills() const;

  // retrieve skill value (if any)
  void getSkillRank(const RPG_Common_Skill&, // skill
                    unsigned char&) const;   // result: value (0: doesn't exist)

  const bool hasFeat(const RPG_Character_Feat&) const; // feat
  const bool hasAbility(const RPG_Character_Ability&) const; // ability
  const bool hasCondition(const RPG_Common_Condition&) const; // condition

  const unsigned short int getNumTotalHitPoints() const;
  const short int getNumHitPoints() const;

  const unsigned int getWealth() const;
  const RPG_Common_Size getSize() const;

  const RPG_Magic_Spells_t getKnownSpells() const;
  const RPG_Magic_SpellList_t getSpells() const;

  virtual const RPG_Character_BaseAttackBonus_t getAttackBonus(const RPG_Common_Attribute&, // modifier
                                                               const RPG_Combat_AttackSituation&) const = 0;
  virtual const signed char getArmorClass(const RPG_Combat_DefenseSituation&) const = 0;

  // get a hint if this is a PC/NPC
  virtual const bool isPlayerCharacter() const = 0;
  // sustain some damage (melee, magic, ...)
  void sustainDamage(const RPG_Combat_Damage&); // damage
  // we just got wiser...
  virtual void gainExperience(const unsigned int&) = 0; // XP

  virtual void status() const;

  virtual void dump() const;

 protected:
  RPG_Character_Base(// base attributes
                     const std::string&,                // name
                     const RPG_Character_Alignment&,    // alignment
                     const RPG_Character_Attributes&,   // base attributes
                     const RPG_Character_Skills_t&,     // skills
                     const RPG_Character_Feats_t&,      // base feats
                     const RPG_Character_Abilities_t&,  // base abilities
                     const RPG_Common_Size&,            // (default) size
                     const unsigned short int&,         // max HP
                     const RPG_Magic_Spells_t&,         // set of known spells (bard / sorcerer)
                     // current status
                     const RPG_Character_Conditions_t&, // condition
                     const unsigned short int&,         // HP
                     const unsigned int&,               // wealth (GP)
                     const RPG_Magic_SpellList_t&,      // set of memorized/prepared spells (!bard)
                     const RPG_Item_List_t&);           // list of (carried) items
  RPG_Character_Base(const RPG_Character_Base&);

//   RPG_Character_Base& operator=(const RPG_Character_Base&);
  void init(// base attributes
            const std::string&,                // name
            const RPG_Character_Alignment&,    // alignment
            const RPG_Character_Attributes&,   // base attributes
            const RPG_Character_Skills_t&,     // skills
            const RPG_Character_Feats_t&,      // base feats
            const RPG_Character_Abilities_t&,  // base abilities
            const RPG_Common_Size&,            // (default) size
            const unsigned short int&,         // max HP
            const RPG_Magic_Spells_t&,         // set of known spells (bard / sorcerer)
            // current status
            const RPG_Character_Conditions_t&, // condition
            const unsigned short int&,         // HP
            const unsigned int&,               // wealth (GP)
            const RPG_Magic_SpellList_t&,      // set of memorized/prepared spells (!bard)
            const RPG_Item_List_t&);           // list of (carried) items

  virtual const signed char getShieldBonus() const = 0;

  unsigned int               myWealth;
  RPG_Common_Size            mySize;

  RPG_Magic_Spells_t         myKnownSpells;
  RPG_Magic_SpellList_t      mySpells;

  RPG_Character_Inventory    myInventory;
  RPG_Character_Equipment    myEquipment;

  short int                  myNumHitPoints;
  RPG_Character_Conditions_t myCondition;

  RPG_Character_Attributes   myAttributes;
  RPG_Character_Feats_t      myFeats;
  RPG_Character_Abilities_t  myAbilities;
  RPG_Character_Skills_t     mySkills;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Base());

  std::string                myName;
  RPG_Character_Alignment    myAlignment;
  unsigned short int         myNumTotalHitPoints;
};

#endif
