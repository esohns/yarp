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

#ifndef RPG_PLAYER_PLAYER_BASE_H
#define RPG_PLAYER_PLAYER_BASE_H

#include <string>

#include "ace/Global_Macros.h"

#include "rpg_common_attribute.h"
#include "rpg_common_camp.h"
#include "rpg_common_subclass.h"

#include "rpg_character_gender.h"
#include "rpg_character_common.h"
#include "rpg_character_race_common.h"
#include "rpg_character_class_common.h"
#include "rpg_character_offhand.h"
#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_alignment.h"
#include "rpg_character_attributes.h"
#include "rpg_character_skills_common.h"

#include "rpg_magic_common.h"

#include "rpg_item_instance_common.h"

#include "rpg_combat_attacksituation.h"
#include "rpg_combat_defensesituation.h"

#include "rpg_player_base.h"

class RPG_Player_Player_Base
 : public RPG_Player_Base
{
  typedef RPG_Player_Base inherited;

 public:
  inline virtual ~RPG_Player_Player_Base () {}

  // retrieve basic player character data
  inline enum RPG_Character_Gender getGender () const { return myGender; }
  inline RPG_Character_Race_t getRace () const { return myRace; }
  inline struct RPG_Character_Class getClass () const { return myClass; }
  inline enum RPG_Character_OffHand getOffHand () const { return myOffHand; }
  inline enum RPG_Common_Size getSize () const { return mySize; }

  inline ACE_UINT64 getExperience () const { return myExperience; }
  // compute dynamically from class/XP
  RPG_Character_Level_t getLevel (enum RPG_Common_SubClass = SUBCLASS_NONE) const; // subclass

  // access current equipment
  inline RPG_Player_Equipment& getEquipment () { return inherited::myEquipment; }

  // implement (part of) RPG_IPlayer
  virtual RPG_Character_BaseAttackBonus_t getAttackBonus (enum RPG_Common_Attribute, // modifier
                                                          enum RPG_Combat_AttackSituation) const;
  virtual ACE_INT8 getArmorClass (enum RPG_Combat_DefenseSituation) const;

  // *NOTE*: return value unit is feet
  virtual ACE_UINT16 getReach (ACE_UINT16&,  // return value: base range (if any)
                               bool&) const; // return value: reach is absolute ?
  virtual ACE_UINT8 getSpeed (bool = false,                                 // running ?
                              enum RPG_Common_AmbientLighting = AMBIENCE_BRIGHT, // environment
                              enum RPG_Common_Terrain = TERRAIN_ANY,             // terrain
                              enum RPG_Common_Track = TRACK_NONE) const;         // track

  inline virtual bool isPlayerCharacter () const { return true; }

  virtual enum RPG_Common_SubClass gainExperience (ACE_UINT64); // XP

  unsigned int rest (enum RPG_Common_Camp, // type of rest
                     unsigned int);        // hours
  void defaultEquip ();

  virtual void status () const;

  virtual void dump () const;

 protected:
  RPG_Player_Player_Base (// base attributes
                          const std::string&,                     // name
                          enum RPG_Character_Gender,              // gender
                          const RPG_Character_Race_t&,            // race
                          const struct RPG_Character_Class&,      // class(es)
                          const struct RPG_Character_Alignment&,  // alignment
                          const struct RPG_Character_Attributes&, // base attributes
                          const RPG_Character_Skills_t&,          // skills
                          const RPG_Character_Feats_t&,           // base feats
                          const RPG_Character_Abilities_t&,       // base abilities
                          enum RPG_Character_OffHand,             // off-hand
                          ACE_UINT16    ,                         // max HP
                          const RPG_Magic_SpellTypes_t&,          // set of known spells (if any)
                          // current status
                          const RPG_Character_Conditions_t&,      // condition
                          ACE_INT16,                              // HP
                          ACE_UINT64,                             // XP
                          ACE_UINT64,                             // wealth (GP)
                          const RPG_Magic_Spells_t&,              // list of prepared spells (if any)
                          const RPG_Item_List_t&);                // list of (carried) items
  RPG_Player_Player_Base (const RPG_Player_Player_Base&);

//   RPG_Character_Player_Base& operator=(const RPG_Character_Player_Base&);
  void initialize (// base attributes
                   const std::string&,                     // name
                   enum RPG_Character_Gender,              // gender
                   const RPG_Character_Race_t&,            // race
                   const struct RPG_Character_Class&,      // class(es)
                   const struct RPG_Character_Alignment&,  // alignment
                   const struct RPG_Character_Attributes&, // base attributes
                   const RPG_Character_Skills_t&,          // skills
                   const RPG_Character_Feats_t&,           // base feats
                   const RPG_Character_Abilities_t&,       // base abilities
                   enum RPG_Character_OffHand,             // off-hand
                   ACE_UINT16,                             // max HP
                   const RPG_Magic_SpellTypes_t&,          // set of known spells (if any)
                   // current status
                   const RPG_Character_Conditions_t&,      // condition
                   ACE_INT16,                              // HP
                   ACE_UINT64,                             // XP
                   ACE_UINT64,                             // wealth (GP)
                   const RPG_Magic_Spells_t&,              // list of prepared spells (if any)
                   const RPG_Item_List_t&);                // list of (carried) items

  // implement (part of) RPG_IPlayer
  virtual ACE_INT8 getShieldBonus () const;

  enum RPG_Character_Gender  myGender;
  RPG_Character_Race_t       myRace;
  struct RPG_Character_Class myClass;
  enum RPG_Character_OffHand myOffHand;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Player_Player_Base ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Player_Player_Base& operator= (const RPG_Player_Player_Base&))

  ACE_UINT64                 myExperience;
  enum RPG_Common_Size       mySize;

//  unsigned short int       myWeight; // kg
//  unsigned int             myAge; // years
};

#endif
