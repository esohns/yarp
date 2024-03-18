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

#ifndef RPG_IPLAYER_H
#define RPG_IPLAYER_H

#include "rpg_common_attribute.h"
#include "rpg_common_terrain.h"
#include "rpg_common_track.h"
#include "rpg_common_ambientlighting.h"

#include "rpg_character_common.h"

#include "rpg_combat_attacksituation.h"
#include "rpg_combat_defensesituation.h"

class RPG_IPlayer
{
 public:
  virtual RPG_Character_BaseAttackBonus_t getAttackBonus (enum RPG_Common_Attribute, // modifier-
                                                          enum RPG_Combat_AttackSituation) const = 0;
  virtual ACE_INT8 getArmorClass (enum RPG_Combat_DefenseSituation) const = 0;
  virtual ACE_INT8 getShieldBonus () const = 0;

  virtual struct RPG_Dice_Roll getHitDicePerLevel (enum RPG_Common_SubClass) const = 0;
  virtual ACE_UINT8 getFeatsPerLevel (enum RPG_Common_SubClass) const = 0;
  virtual ACE_UINT8 getSkillsPerLevel (enum RPG_Common_SubClass) const = 0;
  virtual ACE_UINT16 getKnownSpellsPerLevel (enum RPG_Common_SubClass, // (spellcaster-) subclass
                                             ACE_UINT8) const = 0;     // spell level

  virtual ACE_UINT16 getReach (ACE_UINT16&,      // return value: base range (if any)
                               bool&) const = 0; // return value: reach is absolute ?
  virtual ACE_UINT8 getSpeed (bool = false,                                      // running ?
                              enum RPG_Common_AmbientLighting = AMBIENCE_BRIGHT, // environment
                              enum RPG_Common_Terrain = TERRAIN_ANY,             // terrain
                              enum RPG_Common_Track = TRACK_NONE) const = 0;     // track

  // get a hint if this is a PC/NPC
  virtual bool isPlayerCharacter () const = 0;

  virtual enum RPG_Common_SubClass gainExperience (ACE_UINT64) = 0; // XP

  virtual void status () const = 0;
  virtual void dump () const = 0;
};

#endif
