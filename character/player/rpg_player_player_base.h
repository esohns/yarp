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

#include "rpg_player_exports.h"
#include "rpg_player_base.h"

#include <rpg_character_gender.h>
#include <rpg_character_common.h>
#include <rpg_character_race_common.h>
#include <rpg_character_class_common.h>
#include <rpg_character_offhand.h>
#include <rpg_character_alignmentcivic.h>
#include <rpg_character_alignmentethic.h>
#include <rpg_character_alignment.h>
#include <rpg_character_attributes.h>
#include <rpg_character_skills_common.h>

#include <rpg_combat_attacksituation.h>
#include <rpg_combat_defensesituation.h>

#include <rpg_item_instance_common.h>

#include <rpg_magic_common.h>

#include <rpg_common_attribute.h>
#include <rpg_common_subclass.h>
#include <rpg_common_camp.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Player_Export RPG_Player_Player_Base
 : public RPG_Player_Base
{
 public:
  virtual ~RPG_Player_Player_Base();

  // retrieve basic player character data
  const RPG_Character_Gender& getGender() const;
  const RPG_Character_Race_t& getRace() const;
  const RPG_Character_Class& getClass() const;
  const RPG_Character_OffHand& getOffHand() const;
  const RPG_Common_Size& getSize() const;

  unsigned int getExperience() const;
  // compute dynamically from class/XP
  unsigned char getLevel(const RPG_Common_SubClass& = SUBCLASS_NONE) const; // subclass

  // access current equipment
  RPG_Player_Equipment& getEquipment();

  // implement (part of) RPG_IPlayer
  virtual RPG_Character_BaseAttackBonus_t getAttackBonus(const RPG_Common_Attribute&, // modifier
                                                         const RPG_Combat_AttackSituation&) const;
  virtual signed char getArmorClass(const RPG_Combat_DefenseSituation&) const;

  virtual unsigned short getReach(unsigned short&, // return value: base range (if any)
                                  bool&) const;    // return value: reach is absolute ?
  virtual unsigned char getSpeed(const bool& = false,                                 // running ?
                                 const RPG_Common_AmbientLighting& = AMBIENCE_BRIGHT, // environment
                                 const RPG_Common_Terrain& = TERRAIN_ANY,             // terrain
                                 const RPG_Common_Track& = TRACK_NONE) const;         // track

  virtual bool isPlayerCharacter() const;

  virtual void gainExperience(const unsigned int&); // XP


  unsigned int rest(const RPG_Common_Camp&, // type of rest
                    const unsigned int&);   // hours
  void defaultEquip();

  virtual void status() const;

  virtual void dump() const;

 protected:
  RPG_Player_Player_Base(// base attributes
						             const std::string&,                // name
						             const RPG_Character_Gender&,       // gender
						             const RPG_Character_Race_t&,       // race
						             const RPG_Character_Class&,        // class(es)
						             const RPG_Character_Alignment&,    // alignment
						             const RPG_Character_Attributes&,   // base attributes
						             const RPG_Character_Skills_t&,     // skills
						             const RPG_Character_Feats_t&,      // base feats
						             const RPG_Character_Abilities_t&,  // base abilities
						             const RPG_Character_OffHand&,      // off-hand
						             const unsigned short int&,         // max HP
						             const RPG_Magic_SpellTypes_t&,     // set of known spells (if any)
						             // current status
						             const RPG_Character_Conditions_t&, // condition
						             const short int&,                  // HP
						             const unsigned int&,               // XP
						             const unsigned int&,               // wealth (GP)
						             const RPG_Magic_Spells_t&,         // list of prepared spells (if any)
						             const RPG_Item_List_t&);           // list of (carried) items
  RPG_Player_Player_Base(const RPG_Player_Player_Base&);

//   RPG_Character_Player_Base& operator=(const RPG_Character_Player_Base&);
  void init(// base attributes
            const std::string&,                // name
            const RPG_Character_Gender&,       // gender
            const RPG_Character_Race_t&,       // race
            const RPG_Character_Class&,        // class(es)
            const RPG_Character_Alignment&,    // alignment
            const RPG_Character_Attributes&,   // base attributes
            const RPG_Character_Skills_t&,     // skills
            const RPG_Character_Feats_t&,      // base feats
            const RPG_Character_Abilities_t&,  // base abilities
            const RPG_Character_OffHand&,      // off-hand
            const unsigned short int&,         // max HP
            const RPG_Magic_SpellTypes_t&,     // set of known spells (if any)
            // current status
            const RPG_Character_Conditions_t&, // condition
            const short int&,                  // HP
            const unsigned int&,               // XP
            const unsigned int&,               // wealth (GP)
            const RPG_Magic_Spells_t&,         // list of prepared spells (if any)
            const RPG_Item_List_t&);           // list of (carried) items

  // implement (part of) RPG_IPlayer
  virtual signed char getShieldBonus() const;

  RPG_Character_Gender  myGender;
  RPG_Character_Race_t  myRace;
  RPG_Character_Class   myClass;
  RPG_Character_OffHand myOffHand;

 private:
  typedef RPG_Player_Base inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Player_Player_Base());
  ACE_UNIMPLEMENTED_FUNC(RPG_Player_Player_Base& operator=(const RPG_Player_Player_Base&));

  unsigned int          myExperience;
  RPG_Common_Size       mySize;

//  unsigned short int       myWeight; // kg
//  unsigned int             myAge; // years
};

#endif
