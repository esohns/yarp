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
RPG_Character_Skills_Common_Tools::RPG_Character_Skill2StringTable_t RPG_Character_Skills_Common_Tools::mySkill2StringTable;

void RPG_Character_Skills_Common_Tools::initClassSkillsTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initClassSkillsTable"));

  // clean tables
  myClassSkillsTable.clear();

  // RPG_Character_SubClass / RPG_Character_Skill
  RPG_Character_ClassSkillsSet_t skillSet;
  // SUBCLASS_FIGHTER
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_INTIMIDATE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SWIM);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_FIGHTER, skillSet));

  skillSet.clear();
  // SUBCLASS_PALADIN
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE_NOB_ROY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_RIDE);
  skillSet.insert(SKILL_SENSE_MOTIVE);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_PALADIN, skillSet));

  skillSet.clear();
  // SUBCLASS_RANGER
  skillSet.insert(SKILL_CLIMB);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
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
  skillSet.insert(SKILL_CRAFT_FLETCHER);
  skillSet.insert(SKILL_CRAFT_BOWYER);
  skillSet.insert(SKILL_CRAFT_SMITH_ARMOR);
  skillSet.insert(SKILL_CRAFT_SMITH_BLACK);
  skillSet.insert(SKILL_CRAFT_SMITH_WEAPON);
  skillSet.insert(SKILL_CRAFT_OTHER);
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
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_ARCH_ENG);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_KNOWLEDGE_NOB_ROY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_KNOWLEDGE_PLANES);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_WIZARD, skillSet));

  skillSet.clear();
  // SUBCLASS_SORCERER
  skillSet.insert(SKILL_BLUFF);
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_SORCERER, skillSet));

  skillSet.clear();
  // SUBCLASS_CLERIC
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_KNOWLEDGE_PLANES);
  skillSet.insert(SKILL_PROFESSION);
  skillSet.insert(SKILL_SPELLCRAFT);
  myClassSkillsTable.insert(std::make_pair(SUBCLASS_CLERIC, skillSet));

  skillSet.clear();
  // SUBCLASS_DRUID
  skillSet.insert(SKILL_CONCENTRATION);
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_HANDLE_ANIMAL);
  skillSet.insert(SKILL_HEAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
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
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
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
  skillSet.insert(SKILL_CRAFT_TRAP);
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
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
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
  skillSet.insert(SKILL_CRAFT_ALCHEMY);
  skillSet.insert(SKILL_DECIPHER_SCRIPT);
  skillSet.insert(SKILL_DIPLOMACY);
  skillSet.insert(SKILL_DISGUISE);
  skillSet.insert(SKILL_ESCAPE_ARTIST);
  skillSet.insert(SKILL_GATHER_INFORMATION);
  skillSet.insert(SKILL_HIDE);
  skillSet.insert(SKILL_JUMP);
  skillSet.insert(SKILL_KNOWLEDGE_ARCANA);
  skillSet.insert(SKILL_KNOWLEDGE_ARCH_ENG);
  skillSet.insert(SKILL_KNOWLEDGE_DUNGEONS);
  skillSet.insert(SKILL_KNOWLEDGE_GEOGRAPHY);
  skillSet.insert(SKILL_KNOWLEDGE_HISTORY);
  skillSet.insert(SKILL_KNOWLEDGE_LOCAL);
  skillSet.insert(SKILL_KNOWLEDGE_NATURE);
  skillSet.insert(SKILL_KNOWLEDGE_NOB_ROY);
  skillSet.insert(SKILL_KNOWLEDGE_RELIGION);
  skillSet.insert(SKILL_KNOWLEDGE_PLANES);
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

void RPG_Character_Skills_Common_Tools::initStringConversionTable()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::initStringConversionTable"));

  // clean tables
  mySkill2StringTable.clear();

  mySkill2StringTable.insert(std::make_pair(SKILL_APPRAISE, ACE_TEXT_ALWAYS_CHAR("Appraise")));
  mySkill2StringTable.insert(std::make_pair(SKILL_BALANCE, ACE_TEXT_ALWAYS_CHAR("Balance")));
  mySkill2StringTable.insert(std::make_pair(SKILL_BLUFF, ACE_TEXT_ALWAYS_CHAR("Bluff")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CLIMB, ACE_TEXT_ALWAYS_CHAR("Climb")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CONCENTRATION, ACE_TEXT_ALWAYS_CHAR("Concentration")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_ALCHEMY, ACE_TEXT_ALWAYS_CHAR("Craft: Alchemy")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_FLETCHER, ACE_TEXT_ALWAYS_CHAR("Craft: Fletcher")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_BOWYER, ACE_TEXT_ALWAYS_CHAR("Craft: Bowyer")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_ARMOR, ACE_TEXT_ALWAYS_CHAR("Craft: Armorsmith")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_BLACK, ACE_TEXT_ALWAYS_CHAR("Craft: Blacksmith")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_WEAPON, ACE_TEXT_ALWAYS_CHAR("Craft: Weaponsmith")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_TRAP, ACE_TEXT_ALWAYS_CHAR("Craft: Make Traps")));
  mySkill2StringTable.insert(std::make_pair(SKILL_CRAFT_OTHER, ACE_TEXT_ALWAYS_CHAR("Craft: General")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DECIPHER_SCRIPT, ACE_TEXT_ALWAYS_CHAR("Decipher Script")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DIPLOMACY, ACE_TEXT_ALWAYS_CHAR("Diplomacy")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DISABLE_DEVICE, ACE_TEXT_ALWAYS_CHAR("Disable Device")));
  mySkill2StringTable.insert(std::make_pair(SKILL_DISGUISE, ACE_TEXT_ALWAYS_CHAR("Disguise")));
  mySkill2StringTable.insert(std::make_pair(SKILL_ESCAPE_ARTIST, ACE_TEXT_ALWAYS_CHAR("Escape Artist")));
  mySkill2StringTable.insert(std::make_pair(SKILL_FORGERY, ACE_TEXT_ALWAYS_CHAR("Forgery")));
  mySkill2StringTable.insert(std::make_pair(SKILL_GATHER_INFORMATION, ACE_TEXT_ALWAYS_CHAR("Gather Information")));
  mySkill2StringTable.insert(std::make_pair(SKILL_HANDLE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("Handle Animal")));
  mySkill2StringTable.insert(std::make_pair(SKILL_HEAL, ACE_TEXT_ALWAYS_CHAR("Heal")));
  mySkill2StringTable.insert(std::make_pair(SKILL_HIDE, ACE_TEXT_ALWAYS_CHAR("Hide")));
  mySkill2StringTable.insert(std::make_pair(SKILL_INTIMIDATE, ACE_TEXT_ALWAYS_CHAR("Intimidate")));
  mySkill2StringTable.insert(std::make_pair(SKILL_JUMP, ACE_TEXT_ALWAYS_CHAR("Jump")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCANA, ACE_TEXT_ALWAYS_CHAR("Knowledge: Arcana")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCH_ENG, ACE_TEXT_ALWAYS_CHAR("Knowledge: Achitecture & Engineering")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_DUNGEONS, ACE_TEXT_ALWAYS_CHAR("Knowledge: Dungeoneering")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_GEOGRAPHY, ACE_TEXT_ALWAYS_CHAR("Knowledge: Geography")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_HISTORY, ACE_TEXT_ALWAYS_CHAR("Knowledge: History")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_LOCAL, ACE_TEXT_ALWAYS_CHAR("Knowledge: Local")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NATURE, ACE_TEXT_ALWAYS_CHAR("Knowledge: Nature")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NOB_ROY, ACE_TEXT_ALWAYS_CHAR("Knowledge: Nobility & Royalty")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_RELIGION, ACE_TEXT_ALWAYS_CHAR("Knowledge: Religion")));
  mySkill2StringTable.insert(std::make_pair(SKILL_KNOWLEDGE_PLANES, ACE_TEXT_ALWAYS_CHAR("Knowledge: The Planes")));
  mySkill2StringTable.insert(std::make_pair(SKILL_LISTEN, ACE_TEXT_ALWAYS_CHAR("Listen")));
  mySkill2StringTable.insert(std::make_pair(SKILL_MOVE_SILENTLY, ACE_TEXT_ALWAYS_CHAR("Move Silently")));
  mySkill2StringTable.insert(std::make_pair(SKILL_OPEN_LOCK, ACE_TEXT_ALWAYS_CHAR("Open Lock")));
  mySkill2StringTable.insert(std::make_pair(SKILL_PERFORM, ACE_TEXT_ALWAYS_CHAR("Perform")));
  mySkill2StringTable.insert(std::make_pair(SKILL_PROFESSION, ACE_TEXT_ALWAYS_CHAR("Profession")));
  mySkill2StringTable.insert(std::make_pair(SKILL_RIDE, ACE_TEXT_ALWAYS_CHAR("Ride")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SEARCH, ACE_TEXT_ALWAYS_CHAR("Search")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SENSE_MOTIVE, ACE_TEXT_ALWAYS_CHAR("Sense Motive")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SLEIGHT_OF_HAND, ACE_TEXT_ALWAYS_CHAR("Sleight Of Hand")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SPEAK_LANGUAGE, ACE_TEXT_ALWAYS_CHAR("Speak Language")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SPELLCRAFT, ACE_TEXT_ALWAYS_CHAR("Spellcraft")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SPOT, ACE_TEXT_ALWAYS_CHAR("Spot")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SURVIVAL, ACE_TEXT_ALWAYS_CHAR("Survival")));
  mySkill2StringTable.insert(std::make_pair(SKILL_SWIM, ACE_TEXT_ALWAYS_CHAR("Swim")));
  mySkill2StringTable.insert(std::make_pair(SKILL_TUMBLE, ACE_TEXT_ALWAYS_CHAR("Tumble")));
  mySkill2StringTable.insert(std::make_pair(SKILL_USE_MAGIC_DEVICE, ACE_TEXT_ALWAYS_CHAR("Use Magic Device")));
  mySkill2StringTable.insert(std::make_pair(SKILL_USE_ROPE, ACE_TEXT_ALWAYS_CHAR("Use Rope")));
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

const unsigned int RPG_Character_Skills_Common_Tools::getSkillPoints(const RPG_Character_SubClass& subClass_in,
                                                                     const short int& INTModifier_in,
                                                                     unsigned int& initialPoints_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Common_Tools::getSkillPoints"));

  // init defaults
  initialPoints_out = 0;

  short int baseValue = INTModifier_in;
  switch (subClass_in)
  {
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    case SUBCLASS_CLERIC:
    {
      baseValue += 2;

      break;
    }
    case SUBCLASS_BARBARIAN:
    case SUBCLASS_DRUID:
    case SUBCLASS_MONK:
    {
      baseValue += 4;

      break;
    }
    case SUBCLASS_RANGER:
    case SUBCLASS_BARD:
    {
      baseValue += 6;

      break;
    }
//     case SUBCLASS_WARLORD:
//     case SUBCLASS_WARLOCK:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
    case SUBCLASS_THIEF:
    {
      baseValue += 8;

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: %d --> check implementation !, aborting\n"),
                 subClass_in));

      return 0;
    }
  } // end SWITCH

  // at least 1/level...
  initialPoints_out = ((baseValue > 1) ? (baseValue * 4) : 1);

  return ((baseValue > 1) ? baseValue : 1);
}
