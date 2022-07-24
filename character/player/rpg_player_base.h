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

#ifndef RPG_PLAYER_BASE_H
#define RPG_PLAYER_BASE_H

#include <string>

#include "ace/Global_Macros.h"

#include "rpg_dice_incl.h"

#include "rpg_common_ambientlighting.h"
#include "rpg_common_attribute.h"
#include "rpg_common_condition.h"
#include "rpg_common_environment_incl.h"
#include "rpg_common_incl.h"
#include "rpg_common_size.h"
#include "rpg_common_skill.h"

#include "rpg_character_ability.h"
#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_alignment.h"
#include "rpg_character_common.h"
#include "rpg_character_feat.h"
#include "rpg_character_incl.h"
#include "rpg_character_skills_common.h"

#include "rpg_magic_common.h"
#include "rpg_magic_incl.h"

#include "rpg_item_instance_common.h"

#include "rpg_combat_incl.h"

#include "rpg_iplayer.h"
#include "rpg_player_equipment.h"
#include "rpg_player_inventory.h"

/**
base class of all PCs, NPCs and monsters
 */
class RPG_Player_Base
 : public RPG_IPlayer
{
 public:
  inline virtual ~RPG_Player_Base () {}

  // retrieve basic character data
  inline const std::string& getName () const { return myName; }
  inline const RPG_Character_Alignment& getAlignment () const { return myAlignment; }

  inline const RPG_Character_Conditions_t& getCondition () const { return myCondition; }

  // retrieve base attributes
  ACE_UINT8 getAttribute (enum RPG_Common_Attribute) const;

  inline const RPG_Character_Feats_t& getFeats () const { return myFeats; }
  inline const RPG_Character_Abilities_t& getAbilities () const { return myAbilities; }
  inline const RPG_Character_Skills_t& getSkills () const { return mySkills; }

  // retrieve skill value (if any)
  void getSkillRank (enum RPG_Common_Skill, // skill
                     ACE_UINT8&) const;     // result: value (0: doesn't exist)

  bool hasFeat (enum RPG_Character_Feat) const; // feat
  bool hasAbility (enum RPG_Character_Ability) const; // ability
  bool hasCondition (enum RPG_Common_Condition) const; // condition

  inline unsigned short getNumTotalHitPoints () const { return myNumTotalHitPoints; }
  inline short getNumHitPoints () const { return myNumHitPoints; }

  inline unsigned int getWealth () const { return myWealth; }

  inline const RPG_Magic_SpellTypes_t& getKnownSpells () const { return myKnownSpells; }
  inline const RPG_Magic_Spells_t& getSpells () const { return mySpells; }

  inline const RPG_Player_Inventory& getInventory () const { return myInventory; }
//   const RPG_Character_Equipment getEquipment() const;

  // sustain some damage (melee, magic, ...)
  void sustainDamage (const RPG_Combat_Damage&); // damage

  // implement (part of) RPG_IPlayer
  virtual void status () const;
  virtual void dump () const;

 protected:
  RPG_Player_Base (// base attributes
                   const std::string&,                // name
                   const RPG_Character_Alignment&,    // alignment
                   const RPG_Character_Attributes&,   // base attributes
                   const RPG_Character_Skills_t&,     // skills
                   const RPG_Character_Feats_t&,      // base feats
                   const RPG_Character_Abilities_t&,  // base abilities
                   unsigned short,                    // max HP
                   const RPG_Magic_SpellTypes_t&,     // set of known spells (bard / sorcerer)
                   // current status
                   const RPG_Character_Conditions_t&, // condition
                   short,                             // HP
                   unsigned int,                      // wealth (GP)
                   const RPG_Magic_Spells_t&,         // list of memorized/prepared spells (!bard)
                   const RPG_Item_List_t&);           // list of (carried) items
  RPG_Player_Base (const RPG_Player_Base&);

//   RPG_Player_Base& operator= (const RPG_Player_Base&);
  void initialize (// base attributes
                   const std::string&,                // name
                   const RPG_Character_Alignment&,    // alignment
                   const RPG_Character_Attributes&,   // base attributes
                   const RPG_Character_Skills_t&,     // skills
                   const RPG_Character_Feats_t&,      // base feats
                   const RPG_Character_Abilities_t&,  // base abilities
                   unsigned short,                    // max HP
                   const RPG_Magic_SpellTypes_t&,     // set of known spells (bard / sorcerer)
                   // current status
                   const RPG_Character_Conditions_t&, // condition
                   short,                             // HP
                   unsigned int,                      // wealth (GP)
                   const RPG_Magic_Spells_t&,         // list of memorized/prepared spells (!bard)
                   const RPG_Item_List_t&);           // list of (carried) items

  unsigned int                    myWealth;

  RPG_Magic_SpellTypes_t          myKnownSpells;
  RPG_Magic_Spells_t              mySpells;

  RPG_Player_Inventory            myInventory;
  RPG_Player_Equipment            myEquipment;

  short                           myNumHitPoints;
  RPG_Character_Conditions_t      myCondition;

  struct RPG_Character_Attributes myAttributes;
  RPG_Character_Feats_t           myFeats;
  RPG_Character_Abilities_t       myAbilities;
  RPG_Character_Skills_t          mySkills;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Player_Base ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Player_Base& operator= (const RPG_Player_Base&))

  std::string                     myName;
  struct RPG_Character_Alignment  myAlignment;
  unsigned short                  myNumTotalHitPoints;
};

#endif
