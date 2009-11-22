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

#include "rpg_character_common_tools.h"
#include "rpg_character_skills_common_tools.h"

#include <ace/Log_Msg.h>

#include <string>

RPG_Character_Base::RPG_Character_Base(const std::string& name_in,
                                       const RPG_Character_Alignment& alignment_in,
                                       const RPG_Character_Attributes& attributes_in,
                                       const RPG_Character_Skills_t& skills_in,
                                       const RPG_Character_Feats_t& feats_in,
                                       const RPG_Character_Abilities_t& abilities_in,
                                       const unsigned short int& hitpoints_in,
                                       const unsigned int& wealth_in,
                                       const RPG_Item_List_t& inventory_in)
 : myName(name_in),
   myAlignment(alignment_in),
   myAttributes(attributes_in),
   mySkills(skills_in),
   myFeats(feats_in),
   myAbilities(abilities_in),
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

const std::string RPG_Character_Base::getName() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getName"));

  return myName;
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

  RPG_Character_SkillsConstIterator_t iter = mySkills.find(skill_in);
  if (iter != mySkills.end())
  {
    result_out = iter->second;
  } // end IF
}

const bool RPG_Character_Base::hasFeat(const RPG_Character_Feat& feat_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::hasFeat"));

  return (myFeats.find(feat_in) != myFeats.end());
}

const bool RPG_Character_Base::hasAbility(const RPG_Character_Ability& ability_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::hasAbility"));

  return (myAbilities.find(ability_in) != myAbilities.end());
}

const unsigned short int RPG_Character_Base::getNumTotalHitPoints() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::getNumTotalHitPoints"));

  return myNumTotalHitPoints;
}

const unsigned short int RPG_Character_Base::getNumCurrentHitPoints() const
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

void RPG_Character_Base::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Base::dump"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Name: \"%s\"\nAlignment: \"%s\"\nAttributes:\n===========\n%sSkills:\n=======\n%sFeats:\n======\n%sAbilities:\n==========\n%sItems:\n======\n"),
             myName.c_str(),
             RPG_Character_Common_Tools::alignmentToString(myAlignment).c_str(),
             RPG_Character_Common_Tools::attributesToString(myAttributes).c_str(),
             RPG_Character_Skills_Common_Tools::skillsToString(mySkills).c_str(),
             RPG_Character_Skills_Common_Tools::featsToString(myFeats).c_str(),
             RPG_Character_Skills_Common_Tools::abilitiesToString(myAbilities).c_str()));

  // dump items
  myInventory.dump();
}
