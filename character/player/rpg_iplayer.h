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

#include <rpg_combat_attacksituation.h>
#include <rpg_combat_defensesituation.h>

#include <rpg_character_common.h>

#include <rpg_common_attribute.h>
#include <rpg_common_terrain.h>
#include <rpg_common_track.h>
#include <rpg_common_ambientlighting.h>

class RPG_IPlayer
{
 public:
  virtual RPG_Character_BaseAttackBonus_t getAttackBonus(const RPG_Common_Attribute&, // modifier
                                                         const RPG_Combat_AttackSituation&) const = 0;
  virtual signed char getArmorClass(const RPG_Combat_DefenseSituation&) const = 0;
  virtual signed char getShieldBonus() const = 0;

  virtual unsigned short getReach(unsigned short&,  // return value: base range (if any)
                                  bool&) const = 0; // return value: reach is absolute ?
  virtual unsigned char getSpeed(const bool& = false,                                 // running ?
                                 const RPG_Common_AmbientLighting& = AMBIENCE_BRIGHT, // environment
                                 const RPG_Common_Terrain& = TERRAIN_ANY,             // terrain
                                 const RPG_Common_Track& = TRACK_NONE) const = 0;     // track

  // get a hint if this is a PC/NPC
  virtual bool isPlayerCharacter() const = 0;

  virtual void gainExperience(const unsigned int&) = 0; // XP

  virtual void status() const = 0;
  virtual void dump() const = 0;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_IPlayer());
};

#endif
