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

#ifndef RPG_PLAYER_H
#define RPG_PLAYER_H

#include "rpg_player_player_base.h"

/**
player character
*/
class RPG_Player
 : public RPG_Player_Player_Base
{
  typedef RPG_Player_Player_Base inherited;

 public:
  RPG_Player (// base attributes
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
              // extended data
              ACE_UINT64,                             // XP
              ACE_UINT64,                             // wealth (GP)
              const RPG_Item_List_t&,                 // list of (carried) items
              // current status
              const RPG_Character_Conditions_t&,      // condition
              ACE_INT16,                              // HP
              const RPG_Magic_Spells_t&);             // list of prepared spells (if any)
  RPG_Player (const RPG_Player&);
  inline virtual ~RPG_Player () {}

  // implement (part of) RPG_IPlayer
  virtual struct RPG_Dice_Roll getHitDicePerLevel (enum RPG_Common_SubClass) const;
  virtual ACE_UINT8 getFeatsPerLevel (enum RPG_Common_SubClass) const;
  virtual ACE_UINT8 getSkillsPerLevel (enum RPG_Common_SubClass) const;
  virtual ACE_UINT16 getKnownSpellsPerLevel (enum RPG_Common_SubClass, // (spellcaster-) subclass
                                             ACE_UINT8) const;         // spell level

  // *WARNING*: result needs to be delete()d !
  static RPG_Player* random (); // return value: (random) player
  // *WARNING*: result needs to be deleted(d) !
  static RPG_Player* dummy ();
  // *WARNING*: result needs to be deleted(d) !
  static RPG_Player* load (const std::string&,                // FQ filename
                           const std::string&,                // schema repository (directory)
                           // current status
                           const RPG_Character_Conditions_t&, // condition
                           ACE_INT16,                         // HP
                           const RPG_Magic_Spells_t&);        // list of prepared spells (if any)
  bool save (const std::string&) const; // FQ filename

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Player ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Player& operator= (const RPG_Player&))
};

#endif
