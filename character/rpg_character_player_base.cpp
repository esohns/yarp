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

#include <rpg_item_common.h>
#include <rpg_item_dictionary.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

RPG_Character_Player_Base::RPG_Character_Player_Base(const std::string& name_in,
                                                     const RPG_Character_Gender& gender_in,
                                                     const RPG_Character_Race& race_in,
                                                     const RPG_Character_Classes_t& classes_in,
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
   myClasses(classes_in),
   myExperience(experience_in),
   inherited(name_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
             SIZE_MEDIUM, // standard PCs are all medium-sized
             hitpoints_in,
             wealth_in,
             inventory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::RPG_Character_Player_Base"));

}

RPG_Character_Player_Base::RPG_Character_Player_Base(const RPG_Character_Player_Base& playerBase_in)
 : myGender(playerBase_in.myGender),
   myRace(playerBase_in.myRace),
   myClasses(playerBase_in.myClasses),
   myExperience(playerBase_in.myExperience),
   inherited(playerBase_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::RPG_Character_Player_Base"));

}

RPG_Character_Player_Base::~RPG_Character_Player_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::~RPG_Character_Player_Base"));

}

RPG_Character_Player_Base& RPG_Character_Player_Base::operator=(const RPG_Character_Player_Base& playerBase_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::operator="));

  myGender = playerBase_in.myGender;
  myRace = playerBase_in.myRace;
  myClasses = playerBase_in.myClasses;
  myExperience = playerBase_in.myExperience;
  inherited::operator=(playerBase_in);

  return *this;
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

const RPG_Character_Classes_t RPG_Character_Player_Base::getClasses() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getClasses"));

  return myClasses;
}

const unsigned char RPG_Character_Player_Base::getLevel(const RPG_Character_SubClass& subClass_in) const
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

const RPG_Character_Equipment* RPG_Character_Player_Base::getEquipment() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getEquipment"));

  return &(inherited::myEquipment);
}

const signed char RPG_Character_Player_Base::getArmorClass(const RPG_Combat_DefenseSituation& defenseSituation_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getArmorClass"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getArmor();
  if (type == ARMOR_NONE)
    return 0;

  RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
  // *TODO*: consider defense situation
  return properties.baseArmorBonus;
}

const signed char RPG_Character_Player_Base::getShieldBonus() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getShieldBonus"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getShield();
  if (type == ARMOR_NONE)
    return 0;

  RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
  return properties.baseArmorBonus;
}

void RPG_Character_Player_Base::gainExperience(const unsigned int& XP_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::gainExperience"));

  // *TODO*
  ACE_ASSERT(false);
}

const bool RPG_Character_Player_Base::isPlayerCharacter() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::isPlayerCharacter"));

  return true;
}

void RPG_Character_Player_Base::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::dump"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Player: \nGender: %s\nRace: %s\nClass(es):\n%sXP: %d\n"),
             RPG_Character_GenderHelper::RPG_Character_GenderToString(myGender).c_str(),
             RPG_Character_RaceHelper::RPG_Character_RaceToString(myRace).c_str(),
             RPG_Character_Common_Tools::classesToString(myClasses).c_str(),
             myExperience));

  inherited::dump();
}
