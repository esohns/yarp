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

#include "rpg_character_exports.h"
#include "rpg_character_common.h"
#include "rpg_character_encumbrance.h"
#include "rpg_character_race_common.h"
#include "rpg_character_class_common.h"

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Export RPG_Character_Common_Tools
{
 public:
  // init string conversion (and other) tables
  static void init();

  static std::string raceToString(const RPG_Character_Race_t&); // race(es)
  static std::string classToString(const RPG_Character_Class&); // class(es)
  static std::string alignmentToString(const RPG_Character_Alignment&); // alignment
  static std::string attributesToString(const RPG_Character_Attributes&); // attributes
  static std::string conditionToString(const RPG_Character_Conditions_t&); // condition

  static bool match(const RPG_Character_Alignment&,  // a
                    const RPG_Character_Alignment&); // b

  static signed char getAttributeAbilityModifier(const unsigned char&); // attribute ability score
  static bool getAttributeCheck(const unsigned char&); // attribute ability score
  static RPG_Dice_DieType getHitDie(const RPG_Common_SubClass&); // subclass
  static RPG_Character_BaseAttackBonus_t getBaseAttackBonus(const RPG_Common_SubClass&, // subClass
                                                            const unsigned char&);      // class level
  static RPG_Character_Encumbrance getEncumbrance(const unsigned char&,   // strength
                                                  const RPG_Common_Size&, // size
                                                  const unsigned short&,  // (carried) weight
                                                  const bool& = true);    // is biped ?
  static void getLoadModifiers(const RPG_Character_Encumbrance&, // encumbrance
                               const unsigned char&,             // (base) speed
                               signed char&,                     // max Dex modifier (AC)
                               signed char&,                     // (armor) check penalty
                               unsigned char&,                   // (reduced) speed
                               unsigned char&);                  // run modifier
  static unsigned char getReducedSpeed(const unsigned char&); // (base) speed

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools(const RPG_Character_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools& operator=(const RPG_Character_Common_Tools&));

  static void initStringConversionTables();
  static void initEncumbranceTable();

  struct RPG_Character_EncumbranceEntry_t
  {
    unsigned short light;
    unsigned short medium;
    unsigned short heavy;
  };
  typedef std::map<unsigned char, RPG_Character_EncumbranceEntry_t> RPG_Character_EncumbranceTable_t;
  typedef RPG_Character_EncumbranceTable_t::const_iterator RPG_Character_EncumbranceTableConstIterator_t;

  static RPG_Character_EncumbranceTable_t myEncumbranceTable;
};

#endif
