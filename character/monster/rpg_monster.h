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

#ifndef RPG_MONSTER_H
#define RPG_MONSTER_H

#include "ace/Global_Macros.h"

#include "rpg_dice_incl.h"

#include "rpg_common_incl.h"

#include "rpg_character_common.h"
#include "rpg_character_incl.h"
#include "rpg_character_skills_common.h"

#include "rpg_magic_common.h"

#include "rpg_item_instance_common.h"

#include "rpg_combat_defensesituation.h"
#include "rpg_combat_attacksituation.h"

#include "rpg_player_base.h"

#include "rpg_monster_size.h"

class RPG_Monster
 : public RPG_Player_Base
{
  typedef RPG_Player_Base inherited;

 public:
  RPG_Monster (// base attributes
               const std::string&,                     // name
               const struct RPG_Common_CreatureType&,  // type
               const struct RPG_Character_Alignment&,  // alignment
               const struct RPG_Character_Attributes&, // base attributes
               const RPG_Character_Skills_t&,          // skills
               const RPG_Character_Feats_t&,           // base feats
               const RPG_Character_Abilities_t&,       // base abilities
               const struct RPG_Monster_Size&,         // (default) size
               unsigned short,                         // max HP
               const RPG_Magic_SpellTypes_t&,          // set of known spells (if any)
               // extended data
               unsigned int,                           // wealth (GP)
               const RPG_Magic_Spells_t&,              // set of memorized/prepared spells (if any)
               const RPG_Item_List_t&,                 // list of (carried) items
               // current status
               const RPG_Character_Conditions_t&,      // condition
               unsigned short,                         // HP
               // ...more extended data
               bool = false);                          // summoned ?
  RPG_Monster (const RPG_Monster&);
  inline virtual ~RPG_Monster () {}
//   RPG_Monster& operator=(const RPG_Monster&);

  inline const struct RPG_Common_CreatureType& getType () const { return myType; }
  inline const struct RPG_Monster_Size& getSize () const { return mySize; }
  inline bool isSummoned () const { return myIsSummoned; }

  // access current equipment (if any)
  inline RPG_Player_Equipment& getEquipment () { return inherited::myEquipment; }

  // implement (part of) RPG_IPlayer
  virtual ACE_INT8 getArmorClass (enum RPG_Combat_DefenseSituation) const;

  virtual unsigned short getReach (unsigned short&, // return value: base range (if any)
                                   bool&) const;    // return value: reach is absolute ?
  virtual ACE_UINT8 getSpeed (bool = false,                                      // running ?
                              enum RPG_Common_AmbientLighting = AMBIENCE_BRIGHT, // environment
                              enum RPG_Common_Terrain = TERRAIN_ANY,             // terrain
                              enum RPG_Common_Track = TRACK_NONE) const;         // track

  virtual enum RPG_Common_SubClass gainExperience (unsigned int); // XP

  inline virtual bool isPlayerCharacter () const { return false; }

  virtual void dump () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Monster ())

  // helper methods
  // implement (part of) RPG_IPlayer
  virtual RPG_Character_BaseAttackBonus_t getAttackBonus (enum RPG_Common_Attribute, // modifier
                                                          enum RPG_Combat_AttackSituation) const;
  virtual ACE_INT8 getShieldBonus () const;
  virtual struct RPG_Dice_Roll getHitDicePerLevel (enum RPG_Common_SubClass) const;
  virtual ACE_UINT8 getFeatsPerLevel (enum RPG_Common_SubClass) const;
  virtual ACE_UINT8 getSkillsPerLevel (enum RPG_Common_SubClass) const;
  virtual unsigned short getKnownSpellsPerLevel (enum RPG_Common_SubClass, // (spellcaster-) subclass
                                                 ACE_UINT8) const;         // spell level

  struct RPG_Common_CreatureType myType;
  struct RPG_Monster_Size        mySize;
  bool                           myIsSummoned;
};

#endif
