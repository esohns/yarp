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
#ifndef RPG_CHARACTER_PLAYER_BASE_H
#define RPG_CHARACTER_PLAYER_BASE_H

#include "rpg_character_base.h"

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Player_Base
 : public RPG_Character_Base
{
 public:
  virtual ~RPG_Character_Player_Base();

  // retrieve basic player character data
  const std::string getName() const;

//   // retrieve basic character data
//   using RPG_Character_Base::getGender;
//   using RPG_Character_Base::getRace;
//   using RPG_Character_Base::getClass;
//   using RPG_Character_Base::getAlignment;
// 
//   // retrieve base attributes
//   using RPG_Character_Base::getStrength;
//   using RPG_Character_Base::getDexterity;
//   using RPG_Character_Base::getConstitution;
//   using RPG_Character_Base::getIntelligence;
//   using RPG_Character_Base::getWisdom;
//   using RPG_Character_Base::getCharisma;
//   // retrieve skill value (if any)
//   using RPG_Character_Base::getSkill;
// 
//   using RPG_Character_Base::getExperience;
//   // compute dynamically from class/XP
//   using RPG_Character_Base::getLevel;
//   using RPG_Character_Base::getNumTotalHitPoints;
//   using RPG_Character_Base::getNumCurrentHitPoints;
//   using RPG_Character_Base::getCurrentWealth;

 protected:
  RPG_Character_Player_Base(const std::string&,              // name
                            const RPG_Character_Gender&,     // gender
                            const RPG_Character_Race&,       // race
                            const RPG_Character_Class&,      // (starting) class
                            const RPG_Character_Alignment&,  // (starting) alignment
                            const RPG_Character_Attributes&, // base attributes
                            const RPG_Character_Skills_t&,   // (starting) skills
                            const unsigned int&,             // (starting) XP
                            const unsigned short int&,       // (starting) HP
                            const unsigned int&,             // (starting) wealth (GP)
                            const RPG_Item_List_t&);         // (starting) list of (carried) items

  std::string myName;
//  unsigned short int       mySize; // cm
//  unsigned short int       myWeight; // kg
//  unsigned int             myAge; // years

 private:
  typedef RPG_Character_Base inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player_Base());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player_Base(const RPG_Character_Player_Base&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player_Base& operator=(const RPG_Character_Player_Base&));
};

#endif
