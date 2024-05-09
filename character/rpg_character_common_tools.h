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

#include <map>
#include <string>

#include "ace/Global_Macros.h"

#include "rpg_dice_incl.h"

#include "rpg_common_incl.h"

#include "rpg_character_common.h"
#include "rpg_character_encumbrance.h"
#include "rpg_character_incl.h"
#include "rpg_character_class_common.h"
#include "rpg_character_race_common.h"

class RPG_Character_Common_Tools
{
 public:
  // initialize string conversion- (and other) tables
  static void initialize ();

  static std::string toString (const RPG_Character_Race_t&); // race(es)
  static std::string toString (const struct RPG_Character_Class&); // class(es)
  static std::string toString (const struct RPG_Character_Alignment&); // alignment
  static std::string toString (const struct RPG_Character_Attributes&); // attributes
  static std::string toString (const RPG_Character_Conditions_t&); // condition

  static bool match (const struct RPG_Character_Alignment&,  // a
                     const struct RPG_Character_Alignment&); // b

  static ACE_INT8 getAttributeAbilityModifier (ACE_UINT8); // attribute ability score
  static bool getAttributeCheck (ACE_UINT8); // attribute ability score
  static bool getSkillCheck (ACE_UINT8,      // skill rank
                             ACE_INT8,       // ability modifier,
                             ACE_INT8,       // miscellaneous modifiers
                             ACE_INT8);      // difficulty class (DC)
  static enum RPG_Dice_DieType getHitDie (enum RPG_Common_SubClass); // subclass
  static RPG_Character_BaseAttackBonus_t getBaseAttackBonus (enum RPG_Common_SubClass, // subClass
                                                             ACE_UINT8);               // class level
  static enum RPG_Character_Encumbrance getEncumbrance (ACE_UINT8,            // strength
                                                        enum RPG_Common_Size, // size
                                                        ACE_UINT16,           // (carried) weight
                                                        bool = true);         // is biped ?
  static void getLoadModifiers (enum RPG_Character_Encumbrance, // encumbrance
                                ACE_UINT8,                      // (base) speed
                                ACE_INT8&,                      // return value: max Dex modifier (AC)
                                ACE_INT8&,                      // return value: (armor) check penalty
                                ACE_UINT8&,                     // return value: (reduced) speed
                                ACE_UINT8&);                    // return value: run modifier
  static ACE_UINT8 getReducedSpeed (ACE_UINT8); // (base) speed

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Character_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Character_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Character_Common_Tools (const RPG_Character_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Character_Common_Tools& operator= (const RPG_Character_Common_Tools&))

  static void initializeStringConversionTables ();
  static void initializeEncumbranceTable ();

  struct RPG_Character_EncumbranceEntry
  {
    unsigned short light;
    unsigned short medium;
    unsigned short heavy;
  };
  typedef std::map<ACE_UINT8, struct RPG_Character_EncumbranceEntry> RPG_Character_EncumbranceTable_t;
  typedef RPG_Character_EncumbranceTable_t::const_iterator RPG_Character_EncumbranceTableConstIterator_t;

  static RPG_Character_EncumbranceTable_t myEncumbranceTable;
};

#endif
