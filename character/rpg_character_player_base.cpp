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

#include "rpg_character_common_tools.h"

#include <ace/OS.h>
#include <ace/Log_Msg.h>

RPG_Character_Player_Base::RPG_Character_Player_Base(const std::string& name_in,
                                                     const RPG_Character_Gender& gender_in,
                                                     const RPG_Character_Race& race_in,
                                                     const RPG_Character_Class& class_in,
                                                     const RPG_Character_Alignment& alignment_in,
                                                     const RPG_Character_Attributes& attributes_in,
                                                     const RPG_Character_Skills_t& skills_in,
                                                     const RPG_Character_Feats_t& feats_in,
                                                     const RPG_Character_Abilities_t& abilities_in,
                                                     const unsigned int& experience_in,
                                                     const unsigned short int& hitpoints_in,
                                                     const unsigned int& wealth_in,
                                                     const RPG_Item_List_t& inventory_in)
 : myGender(gender_in),
   myRace(race_in),
   myClass(class_in),
   myExperience(experience_in),
   inherited(name_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
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

const RPG_Character_Gender RPG_Character_Player_Base::getGender() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getGender"));

  return myGender;
}

const RPG_Character_Race RPG_Character_Player_Base::getRace() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getRace"));

  return myRace;
}

const RPG_Character_Class RPG_Character_Player_Base::getClass() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getClass"));

  return myClass;
}

const unsigned char RPG_Character_Player_Base::getLevel() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getLevel"));

  // *TODO*: consider implementing class-specific tables...
  return ACE_static_cast(unsigned int,
                         ACE_OS::floor((1.0 + ::sqrt((myExperience / 125) + 1)) / 2.0));
}

const unsigned int RPG_Character_Player_Base::getExperience() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getExperience"));

  return myExperience;
}

void RPG_Character_Player_Base::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::dump"));

  std::string gender = RPG_Character_Common_Tools::genderToString(myGender);
  std::string race = RPG_Character_Common_Tools::raceToString(myRace);
  std::string class_string = RPG_Character_Common_Tools::subClassToString(myClass.subClass);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Player: \nGender: %s\nRace: %s\nClass: %s\nXP: %d\n"),
             gender.c_str(),
             race.c_str(),
             class_string.c_str(),
             myExperience));

  inherited::dump();
}
