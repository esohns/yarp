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

#ifndef RPG_CHARACTER_MONSTERSUBTYPE_H
#define RPG_CHARACTER_MONSTERSUBTYPE_H

enum RPG_Character_MonsterSubType
{
  MONSTERSUBTYPE_AIR = 0,
  MONSTERSUBTYPE_ANGEL,
  MONSTERSUBTYPE_AQUATIC,
  MONSTERSUBTYPE_ARCHON,
  MONSTERSUBTYPE_AUGMENTED,
  MONSTERSUBTYPE_CHAOTIC,
  MONSTERSUBTYPE_COLD,
  MONSTERSUBTYPE_EARTH,
  MONSTERSUBTYPE_EVIL,
  MONSTERSUBTYPE_EXTRAPLANAR,
  MONSTERSUBTYPE_FIRE,
  MONSTERSUBTYPE_GOBLINOID,
  MONSTERSUBTYPE_GOOD,
  MONSTERSUBTYPE_INCORPOREAL,
  MONSTERSUBTYPE_LAWFUL,
  MONSTERSUBTYPE_NATIVE,
  MONSTERSUBTYPE_REPTILIAN,
  MONSTERSUBTYPE_SHAPECHANGER,
  MONSTERSUBTYPE_SWARM,
  MONSTERSUBTYPE_WATER,
  //
  RPG_CHARACTER_MONSTERSUBTYPE_MAX,
  RPG_CHARACTER_MONSTERSUBTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_MonsterSubType, std::string> RPG_Character_MonsterSubTypeToStringTable_t;
typedef RPG_Character_MonsterSubTypeToStringTable_t::const_iterator RPG_Character_MonsterSubTypeToStringTableIterator_t;

class RPG_Character_MonsterSubTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_MonsterSubTypeToStringTable.clear();
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_AIR, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AIR")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_ANGEL, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_ANGEL")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_AQUATIC, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AQUATIC")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_ARCHON, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_ARCHON")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_AUGMENTED, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AUGMENTED")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_CHAOTIC, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_CHAOTIC")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_COLD, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_COLD")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_EARTH, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_EARTH")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_EVIL, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_EVIL")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_EXTRAPLANAR, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_EXTRAPLANAR")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_FIRE, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_FIRE")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_GOBLINOID, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_GOBLINOID")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_GOOD, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_GOOD")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_INCORPOREAL, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_INCORPOREAL")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_LAWFUL, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_LAWFUL")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_NATIVE, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_NATIVE")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_REPTILIAN, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_REPTILIAN")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_SHAPECHANGER, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_SHAPECHANGER")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_SWARM, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_SWARM")));
    myRPG_Character_MonsterSubTypeToStringTable.insert(std::make_pair(MONSTERSUBTYPE_WATER, ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_WATER")));
  };

  inline static std::string RPG_Character_MonsterSubTypeToString(const RPG_Character_MonsterSubType& element_in)
  {
    std::string result;
    RPG_Character_MonsterSubTypeToStringTableIterator_t iterator = myRPG_Character_MonsterSubTypeToStringTable.find(element_in);
    if (iterator != myRPG_Character_MonsterSubTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_MONSTERSUBTYPE_INVALID");

    return result;
  };

  inline static RPG_Character_MonsterSubType stringToRPG_Character_MonsterSubType(const std::string& string_in)
  {
    RPG_Character_MonsterSubTypeToStringTableIterator_t iterator = myRPG_Character_MonsterSubTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_MonsterSubTypeToStringTable.end());

    return RPG_CHARACTER_MONSTERSUBTYPE_INVALID;
  };

  static RPG_Character_MonsterSubTypeToStringTable_t myRPG_Character_MonsterSubTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterSubTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterSubTypeHelper(const RPG_Character_MonsterSubTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterSubTypeHelper& operator=(const RPG_Character_MonsterSubTypeHelper&));
};

#endif
