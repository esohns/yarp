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
#ifndef RPG_CHARACTER_COMMON_TOOLS_H
#define RPG_CHARACTER_COMMON_TOOLS_H

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_character_incl.h"
#include "rpg_character_player_common.h"
#include "rpg_character_player.h"

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Common_Tools
{
 public:
  // init string conversion (and other) tables
  static void init();

  static const std::string raceToString(const RPG_Character_Race_t&); // race(es)
  static const std::string classToString(const RPG_Character_Class&); // class(es)
  static const std::string alignmentToString(const RPG_Character_Alignment&); // alignment
  static const std::string attributesToString(const RPG_Character_Attributes&); // attributes
  static const std::string conditionToString(const RPG_Character_Conditions_t&); // condition

  static const bool match(const RPG_Character_Alignment&,  // a
                          const RPG_Character_Alignment&); // b

  static const signed char getAttributeAbilityModifier(const unsigned char&); // attribute ability score
  static const bool getAttributeCheck(const unsigned char&); // attribute ability score
  static const RPG_Dice_DieType getHitDie(const RPG_Common_SubClass&); // subclass
  static const RPG_Character_BaseAttackBonus_t getBaseAttackBonus(const RPG_Common_SubClass&, // subClass
                                                                  const unsigned char&);      // class level

  static RPG_Character_Player generatePlayerCharacter(); // return value: (random) player
  static const unsigned int restParty(RPG_Character_Party_t&); // party of players

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools(const RPG_Character_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools& operator=(const RPG_Character_Common_Tools&));

  static void initStringConversionTables();
};

#endif
