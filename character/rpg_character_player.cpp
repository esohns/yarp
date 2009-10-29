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
#include "rpg_character_player.h"

#include <ace/Log_Msg.h>

RPG_Character_Player::RPG_Character_Player(const std::string& name_in,
                                           const RPG_Character_Gender& gender_in,
                                           const RPG_Character_Race& race_in,
                                           const RPG_Character_Class& class_in,
                                           const RPG_Character_Alignment& alignment_in,
                                           const RPG_Character_Attributes& attributes_in,
                                           const RPG_Character_Skills_t& skills_in,
                                           const unsigned int& experience_in,
                                           const unsigned short int& hitpoints_in,
                                           const unsigned int& wealth_in,
                                           const RPG_ITEM_LIST_T& inventory_in)
 : inherited(name_in,
             gender_in,
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
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::RPG_Character_Player"));

}

RPG_Character_Player::~RPG_Character_Player()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player::~RPG_Character_Player"));

}
