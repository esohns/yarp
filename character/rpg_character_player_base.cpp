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
#include "rpg_character_player_base.h"

#include <ace/Log_Msg.h>

RPG_Character_Player_Base::RPG_Character_Player_Base(const std::string& name_in,
                                                     const RPG_Character_Gender& gender_in,
                                                     const RPG_Character_Race& race_in,
                                                     const RPG_Character_Class& class_in,
                                                     const RPG_Character_Alignment& alignment_in,
                                                     const RPG_Character_Attributes& attributes_in,
                                                     const RPG_CHARACTER_SKILLS_T& skills_in,
                                                     const unsigned int& experience_in,
                                                     const unsigned short int& hitpoints_in,
                                                     const unsigned int& wealth_in,
                                                     const RPG_ITEM_LIST_T& inventory_in)
 : myName(name_in),
   inherited(gender_in,
             race_in,
             class_in,
             alignment_in,
             attributes_in,
             skills_in,
             experience_in,
             hitpoints_in,
             wealth_in,
             inventory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::RPG_Character_Player_Base"));

}

RPG_Character_Player_Base::~RPG_Character_Player_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::~RPG_Character_Player_Base"));

}

const std::string RPG_Character_Player_Base::getName() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getName"));

  return myName;
}
