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
#ifndef RPG_CHARACTER_PLAYER_H
#define RPG_CHARACTER_PLAYER_H

#include "rpg_character_player_base.h"

/**
player character

	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Player
 : protected RPG_Character_Player_Base
{
 public:
  RPG_Character_Player(const std::string&,              // name
                       const RPG_Character_Gender&,     // gender
                       const RPG_Character_Race&,       // race
                       const RPG_Character_Class&,      // (starting) class
                       const RPG_Character_Alignment&,  // (starting) alignment
                       const RPG_Character_Attributes&, // base attributes
                       const RPG_CHARACTER_SKILLS_T&,   // (starting) skills
                       const unsigned int&,             // (starting) XP
                       const unsigned short int&,       // (starting) HP
                       const unsigned int&,             // (starting) wealth (GP)
                       const RPG_ITEM_LIST_T&);         // (starting) list of (carried) items
  virtual ~RPG_Character_Player();

  using RPG_Character_Base::dump;

 private:
  typedef RPG_Character_Player_Base inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player(const RPG_Character_Player&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player& operator=(const RPG_Character_Player&));
};

#endif
