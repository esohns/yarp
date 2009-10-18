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
#include "rpg_character_skills_common_tools.h"

#include <ace/Log_Msg.h>

// init statics
RPG_Character_Skills_Common_Tools::RPG_Character_ClassSkillsTable_t RPG_Character_Skills_Common_Tools::myClassSkillsTable;

void RPG_Character_Skills_Common_Tools::initClassSkillsTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initClassSkillsTable"));

  // clean tables
  myClassSkillsTable.clear();

  // RPG_Character_SubClass / RPG_Character_Skill
  RPG_Character_ClassSkillsSet_t skillSet;
  // SUBCLASS_FIGHTER
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_FIGHTER, skillSet));

  skillSet.clear();
  // SUBCLASS_PALADIN
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_PALADIN, skillSet));

  skillSet.clear();
  // SUBCLASS_RANGER
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SEARCH);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SURVIVAL);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_USE_ROPE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_RANGER, skillSet));

  skillSet.clear();
  // SUBCLASS_BARBARIAN
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SURVIVAL);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_BARBARIAN, skillSet));

  skillSet.clear();
  // SUBCLASS_WIZARD
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_WIZARD, skillSet));

  skillSet.clear();
  // SUBCLASS_SORCERER
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_SORCERER, skillSet));

  skillSet.clear();
  // SUBCLASS_CLERIC
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_CLERIC, skillSet));

  skillSet.clear();
  // SUBCLASS_DRUID
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SPELLCRAFT);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SURVIVAL);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_DRUID, skillSet));

  skillSet.clear();
  // SUBCLASS_MONK
  skillSet.insert(SKILL_BALANCE);
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_PERFORM);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_TUMBLE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_MONK, skillSet));

  skillSet.clear();
  // SUBCLASS_THIEF
  skillSet.insert(SKILL_APPRAISE);
  skillSet.insert(SKILL_BALANCE);
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_DISABLE_DEVICE);
  skillSet.insert(SKILL_DISGUISE);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_FORGERY);
  skillSet.insert(SKILL_GATHER_INFORMATION);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_OPEN_LOCK);
  skillSet.insert(SKILL_PERFORM);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SEARCH);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  skillSet.insert(SKILL_SLEIGHT_OF_HAND);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_TUMBLE);
  skillSet.insert(SKILL_USE_MAGIC_DEVICE);
  skillSet.insert(SKILL_USE_ROPE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_THIEF, skillSet));

  skillSet.clear();
  // SUBCLASS_BARD
  skillSet.insert(SKILL_APPRAISE);
  skillSet.insert(SKILL_BALANCE);
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_DISGUISE);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_GATHER_INFORMATION);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE);
  skillSet.insert(SKILL_LISTEN);
  skillSet.insert(SKILL_MOVE_SILENTLY);
  skillSet.insert(SKILL_PERFORM);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  skillSet.insert(SKILL_SLEIGHT_OF_HAND);
  skillSet.insert(SKILL_SPEAK_LANGUAGE);
  skillSet.insert(SKILL_SPELLCRAFT);
  skillSet.insert(SKILL_SPOT);
  skillSet.insert(SKILL_SWIM);
  skillSet.insert(SKILL_TUMBLE);
  skillSet.insert(SKILL_USE_MAGIC_DEVICE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_BARD, skillSet));
}

const bool RPG_Character_Skills_Common_Tools::isClassSkill(const RPG_Character_SubClass& subClass_in,
                                                           const RPG_Character_Skill& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::isClassSkill"));

  RPG_Character_ClassSkillsTableIterator_t iterator = myClassSkillsTable.find(subClass_in);
  if (iterator == myClassSkillsTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid subclass: %d --> check implementation !, aborting\n"),
               subClass_in));

    return false;
  } // end IF

  return (iterator->second.find(skill_in) != iterator->second.end());
}
