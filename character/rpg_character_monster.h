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
#ifndef RPG_CHARACTER_MONSTER_H
#define RPG_CHARACTER_MONSTER_H

#include "rpg_character_base.h"
#include "rpg_character_common.h"

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Monster
 : public RPG_Character_Base
{
 public:
  RPG_Character_Monster(const std::string&,               // name
                        const RPG_Common_CreatureType&,   // type
                        const RPG_Character_Alignment&,   // (starting) alignment
                        const RPG_Character_Attributes&,  // base attributes
                        const RPG_Character_Skills_t&,    // (starting) skills
                        const RPG_Character_Feats_t&,     // base feats
                        const RPG_Character_Abilities_t&, // base abilities
                        const RPG_Common_Size&,           // (default) size
                        const unsigned short int&,        // (starting) HP
                        const unsigned int&,              // (starting) wealth (GP)
                        const RPG_Item_List_t&,           // (starting) list of (carried) items
                        const bool& = false);             // summoned ?
  RPG_Character_Monster(const RPG_Character_Monster&);
  virtual ~RPG_Character_Monster();
  RPG_Character_Monster& operator=(const RPG_Character_Monster&);

  const RPG_Common_CreatureType getType() const;
  const bool isSummoned() const;

  virtual const signed char getArmorClass(const RPG_Combat_DefenseSituation&) const;

  virtual void gainExperience(const unsigned int&); // XP

  virtual const bool isPlayerCharacter() const;

  virtual void dump() const;

 private:
  typedef RPG_Character_Base inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster());

  // helper methods
  virtual const RPG_Character_BaseAttackBonus_t getAttackBonus(const RPG_Common_Attribute&, // modifier
                                                               const RPG_Combat_AttackSituation&) const;
  virtual const signed char getShieldBonus() const;

  RPG_Common_CreatureType myType;
  bool                    myIsSummoned;
};

#endif
