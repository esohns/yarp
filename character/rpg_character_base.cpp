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
#include "rpg_character_base.h"

#include <ace/OS.h>
#include <ace/Trace.h>

RPG_Character_Base::RPG_Character_Base(const RPG_Character_Gender& gender_in,
                                       const RPG_Character_Race& race_in,
                                       const RPG_Character_Class& class_in,
                                       const RPG_Character_Alignment& alignment_in,
                                       const RPG_Character_Attributes& attributes_in,
                                       const RPG_CHARACTER_SKILLS_T& skills_in,
                                       const unsigned int& experience_in,
                                       const unsigned short& hitpoints_in,
                                       const unsigned int& wealth_in,
                                       const RPG_CHARACTER_INVENTORY_T& inventory_in)
 : myGender(gender_in),
   myRace(race_in),
   myClass(class_in),
   myAlignment(alignment_in),
   myAttributes(attributes_in),
   mySkills(skills_in),
   myExperience(experience_in),
   myNumTotalHitPoints(hitpoints_in),
   myNumCurrentHitPoints(hitpoints_in), // we start out healthy, don't we ?
   myCurrentWealth(wealth_in),
   myCondition(CONDITION_NORMAL), // start normal
   myInventory(inventory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::RPG_Character_Base"));

}

RPG_Character_Base::~RPG_Character_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::~RPG_Character_Base"));

}

const RPG_Character_Gender RPG_Character_Base::getGender() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getGender"));

  return myGender;
}

const RPG_Character_Race RPG_Character_Base::getRace() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getRace"));

  return myRace;
}

const RPG_Character_Class RPG_Character_Base::getClass() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getClass"));

  return myClass;
}

const RPG_Character_Alignment RPG_Character_Base::getAlignment() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getAlignment"));

  return myAlignment;
}

const unsigned char RPG_Character_Base::getStrength() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getStrength"));

  return myAttributes.strength;
}

const unsigned char RPG_Character_Base::getDexterity() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getDexterity"));

  return myAttributes.dexterity;
}

const unsigned char RPG_Character_Base::getConstitution() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getConstitution"));

  return myAttributes.constitution;
}

const unsigned char RPG_Character_Base::getIntelligence() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getIntelligence"));

  return myAttributes.intelligence;
}

const unsigned char RPG_Character_Base::getWisdom() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getWisdom"));

  return myAttributes.wisdom;
}

const unsigned char RPG_Character_Base::getCharisma() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getCharisma"));

  return myAttributes.charisma;
}

void RPG_Character_Base::getSkill(const RPG_Character_Skill& skill_in,
                                  unsigned char& result_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getSkill"));

  // init return value
  result_out = 0;

  RPG_CHARACTER_SKILLS_ITERATOR_T iter = mySkills.find(skill_in);
  if (iter != mySkills.end())
  {
    result_out = iter->second;
  } // end IF
}

const unsigned char RPG_Character_Base::getLevel() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getLevel"));

  // *TODO*: consider implementing class-specific tables...
  return ACE_static_cast(unsigned int,
                         ACE_OS::floor((1.0 + ::sqrt((myExperience / 125) + 1)) / 2.0));
}

const unsigned int RPG_Character_Base::getExperience() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getExperience"));

  return myExperience;
}

const unsigned short RPG_Character_Base::getNumTotalHitPoints() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getNumTotalHitPoints"));

  return myNumTotalHitPoints;
}

const unsigned short RPG_Character_Base::getNumCurrentHitPoints() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getNumCurrentHitPoints"));

  return myNumCurrentHitPoints;
}

const unsigned int RPG_Character_Base::getCurrentWealth() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getCurrentWealth"));

  return myCurrentWealth;
}

const RPG_Character_Condition RPG_Character_Base::getCondition() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getCondition"));

  return myCondition;
}
