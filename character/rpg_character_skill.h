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

#ifndef RPG_CHARACTER_SKILL_H
#define RPG_CHARACTER_SKILL_H

enum RPG_Character_Skill
{
  SKILL_APPRAISE = 0,
  SKILL_BALANCE,
  SKILL_BLUFF,
  SKILL_CLIMB,
  SKILL_CONCENTRATION,
  SKILL_CRAFT_ANY,
  SKILL_CRAFT_ALL,
  SKILL_CRAFT_ALCHEMY,
  SKILL_CRAFT_FLETCHER,
  SKILL_CRAFT_BOWYER,
  SKILL_CRAFT_SMITH_ARMOR,
  SKILL_CRAFT_SMITH_BLACK,
  SKILL_CRAFT_SMITH_WEAPON,
  SKILL_CRAFT_TRAP,
  SKILL_CRAFT_OTHER,
  SKILL_DECIPHER_SCRIPT,
  SKILL_DIPLOMACY,
  SKILL_DISABLE_DEVICE,
  SKILL_DISGUISE,
  SKILL_ESCAPE_ARTIST,
  SKILL_FORGERY,
  SKILL_GATHER_INFORMATION,
  SKILL_HANDLE_ANIMAL,
  SKILL_HEAL,
  SKILL_HIDE,
  SKILL_INTIMIDATE,
  SKILL_JUMP,
  SKILL_KNOWLEDGE_ANY,
  SKILL_KNOWLEDGE_ALL,
  SKILL_KNOWLEDGE_ARCANA,
  SKILL_KNOWLEDGE_ARCH_ENG,
  SKILL_KNOWLEDGE_DUNGEONS,
  SKILL_KNOWLEDGE_GEOGRAPHY,
  SKILL_KNOWLEDGE_HISTORY,
  SKILL_KNOWLEDGE_LOCAL,
  SKILL_KNOWLEDGE_NATURE,
  SKILL_KNOWLEDGE_NOB_ROY,
  SKILL_KNOWLEDGE_RELIGION,
  SKILL_KNOWLEDGE_PLANES,
  SKILL_LISTEN,
  SKILL_MOVE_SILENTLY,
  SKILL_OPEN_LOCK,
  SKILL_PERFORM,
  SKILL_PROFESSION,
  SKILL_RIDE,
  SKILL_SEARCH,
  SKILL_SENSE_MOTIVE,
  SKILL_SLEIGHT_OF_HAND,
  SKILL_SPEAK_LANGUAGE,
  SKILL_SPELLCRAFT,
  SKILL_SPOT,
  SKILL_SURVIVAL,
  SKILL_SWIM,
  SKILL_TUMBLE,
  SKILL_USE_MAGIC_DEVICE,
  SKILL_USE_ROPE,
  //
  RPG_CHARACTER_SKILL_MAX,
  RPG_CHARACTER_SKILL_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Skill, std::string> RPG_Character_SkillToStringTable_t;
typedef RPG_Character_SkillToStringTable_t::const_iterator RPG_Character_SkillToStringTableIterator_t;

class RPG_Character_SkillHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_SkillToStringTable.clear();
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_APPRAISE, ACE_TEXT_ALWAYS_CHAR("SKILL_APPRAISE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_BALANCE, ACE_TEXT_ALWAYS_CHAR("SKILL_BALANCE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_BLUFF, ACE_TEXT_ALWAYS_CHAR("SKILL_BLUFF")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CLIMB, ACE_TEXT_ALWAYS_CHAR("SKILL_CLIMB")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CONCENTRATION, ACE_TEXT_ALWAYS_CHAR("SKILL_CONCENTRATION")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_ANY, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_ANY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_ALL, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_ALL")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_ALCHEMY, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_ALCHEMY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_FLETCHER, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_FLETCHER")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_BOWYER, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_BOWYER")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_ARMOR, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_SMITH_ARMOR")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_BLACK, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_SMITH_BLACK")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_SMITH_WEAPON, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_SMITH_WEAPON")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_TRAP, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_TRAP")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_CRAFT_OTHER, ACE_TEXT_ALWAYS_CHAR("SKILL_CRAFT_OTHER")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_DECIPHER_SCRIPT, ACE_TEXT_ALWAYS_CHAR("SKILL_DECIPHER_SCRIPT")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_DIPLOMACY, ACE_TEXT_ALWAYS_CHAR("SKILL_DIPLOMACY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_DISABLE_DEVICE, ACE_TEXT_ALWAYS_CHAR("SKILL_DISABLE_DEVICE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_DISGUISE, ACE_TEXT_ALWAYS_CHAR("SKILL_DISGUISE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_ESCAPE_ARTIST, ACE_TEXT_ALWAYS_CHAR("SKILL_ESCAPE_ARTIST")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_FORGERY, ACE_TEXT_ALWAYS_CHAR("SKILL_FORGERY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_GATHER_INFORMATION, ACE_TEXT_ALWAYS_CHAR("SKILL_GATHER_INFORMATION")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_HANDLE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("SKILL_HANDLE_ANIMAL")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_HEAL, ACE_TEXT_ALWAYS_CHAR("SKILL_HEAL")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_HIDE, ACE_TEXT_ALWAYS_CHAR("SKILL_HIDE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_INTIMIDATE, ACE_TEXT_ALWAYS_CHAR("SKILL_INTIMIDATE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_JUMP, ACE_TEXT_ALWAYS_CHAR("SKILL_JUMP")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ANY, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_ANY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ALL, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_ALL")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCANA, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_ARCANA")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_ARCH_ENG, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_ARCH_ENG")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_DUNGEONS, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_DUNGEONS")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_GEOGRAPHY, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_GEOGRAPHY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_HISTORY, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_HISTORY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_LOCAL, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_LOCAL")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NATURE, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_NATURE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_NOB_ROY, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_NOB_ROY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_RELIGION, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_RELIGION")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_KNOWLEDGE_PLANES, ACE_TEXT_ALWAYS_CHAR("SKILL_KNOWLEDGE_PLANES")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_LISTEN, ACE_TEXT_ALWAYS_CHAR("SKILL_LISTEN")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_MOVE_SILENTLY, ACE_TEXT_ALWAYS_CHAR("SKILL_MOVE_SILENTLY")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_OPEN_LOCK, ACE_TEXT_ALWAYS_CHAR("SKILL_OPEN_LOCK")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_PERFORM, ACE_TEXT_ALWAYS_CHAR("SKILL_PERFORM")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_PROFESSION, ACE_TEXT_ALWAYS_CHAR("SKILL_PROFESSION")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_RIDE, ACE_TEXT_ALWAYS_CHAR("SKILL_RIDE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SEARCH, ACE_TEXT_ALWAYS_CHAR("SKILL_SEARCH")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SENSE_MOTIVE, ACE_TEXT_ALWAYS_CHAR("SKILL_SENSE_MOTIVE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SLEIGHT_OF_HAND, ACE_TEXT_ALWAYS_CHAR("SKILL_SLEIGHT_OF_HAND")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SPEAK_LANGUAGE, ACE_TEXT_ALWAYS_CHAR("SKILL_SPEAK_LANGUAGE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SPELLCRAFT, ACE_TEXT_ALWAYS_CHAR("SKILL_SPELLCRAFT")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SPOT, ACE_TEXT_ALWAYS_CHAR("SKILL_SPOT")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SURVIVAL, ACE_TEXT_ALWAYS_CHAR("SKILL_SURVIVAL")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_SWIM, ACE_TEXT_ALWAYS_CHAR("SKILL_SWIM")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_TUMBLE, ACE_TEXT_ALWAYS_CHAR("SKILL_TUMBLE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_USE_MAGIC_DEVICE, ACE_TEXT_ALWAYS_CHAR("SKILL_USE_MAGIC_DEVICE")));
    myRPG_Character_SkillToStringTable.insert(std::make_pair(SKILL_USE_ROPE, ACE_TEXT_ALWAYS_CHAR("SKILL_USE_ROPE")));
  };

  inline static std::string RPG_Character_SkillToString(const RPG_Character_Skill& element_in)
  {
    std::string result;
    RPG_Character_SkillToStringTableIterator_t iterator = myRPG_Character_SkillToStringTable.find(element_in);
    if (iterator != myRPG_Character_SkillToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_SKILL_INVALID");

    return result;
  };

  inline static RPG_Character_Skill stringToRPG_Character_Skill(const std::string& string_in)
  {
    RPG_Character_SkillToStringTableIterator_t iterator = myRPG_Character_SkillToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_SkillToStringTable.end());

    return RPG_CHARACTER_SKILL_INVALID;
  };

  static RPG_Character_SkillToStringTable_t myRPG_Character_SkillToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SkillHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SkillHelper(const RPG_Character_SkillHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_SkillHelper& operator=(const RPG_Character_SkillHelper&));
};

#endif
