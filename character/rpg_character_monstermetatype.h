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

#ifndef RPG_CHARACTER_MONSTERMETATYPE_H
#define RPG_CHARACTER_MONSTERMETATYPE_H

enum RPG_Character_MonsterMetaType
{
  MONSTERMETATYPE_ABERRATION = 0,
  MONSTERMETATYPE_ANIMAL,
  MONSTERMETATYPE_CONSTRUCT,
  MONSTERMETATYPE_DRAGON,
  MONSTERMETATYPE_ELEMENTAL,
  MONSTERMETATYPE_FEY,
  MONSTERMETATYPE_GIANT,
  MONSTERMETATYPE_HUMANOID,
  MONSTERMETATYPE_MAGICAL_BEAST,
  MONSTERMETATYPE_MONSTROUS_HUMANOID,
  MONSTERMETATYPE_OOZE,
  MONSTERMETATYPE_OUTSIDER,
  MONSTERMETATYPE_PLANT,
  MONSTERMETATYPE_UNDEAD,
  MONSTERMETATYPE_VERMIN,
  //
  RPG_CHARACTER_MONSTERMETATYPE_MAX,
  RPG_CHARACTER_MONSTERMETATYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_MonsterMetaType, std::string> RPG_Character_MonsterMetaTypeToStringTable_t;
typedef RPG_Character_MonsterMetaTypeToStringTable_t::const_iterator RPG_Character_MonsterMetaTypeToStringTableIterator_t;

class RPG_Character_MonsterMetaTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_MonsterMetaTypeToStringTable.clear();
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_ABERRATION, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ABERRATION")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ANIMAL")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_CONSTRUCT, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_CONSTRUCT")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_DRAGON, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_DRAGON")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_ELEMENTAL, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ELEMENTAL")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_FEY, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_FEY")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_GIANT, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_GIANT")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_HUMANOID, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_HUMANOID")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_MAGICAL_BEAST, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_MAGICAL_BEAST")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_MONSTROUS_HUMANOID, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_MONSTROUS_HUMANOID")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_OOZE, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_OOZE")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_OUTSIDER, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_OUTSIDER")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_PLANT, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_PLANT")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_UNDEAD, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_UNDEAD")));
    myRPG_Character_MonsterMetaTypeToStringTable.insert(std::make_pair(MONSTERMETATYPE_VERMIN, ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_VERMIN")));
  };

  inline static std::string RPG_Character_MonsterMetaTypeToString(const RPG_Character_MonsterMetaType& element_in)
  {
    std::string result;
    RPG_Character_MonsterMetaTypeToStringTableIterator_t iterator = myRPG_Character_MonsterMetaTypeToStringTable.find(element_in);
    if (iterator != myRPG_Character_MonsterMetaTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_MONSTERMETATYPE_INVALID");

    return result;
  };

  inline static RPG_Character_MonsterMetaType stringToRPG_Character_MonsterMetaType(const std::string& string_in)
  {
    RPG_Character_MonsterMetaTypeToStringTableIterator_t iterator = myRPG_Character_MonsterMetaTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_MonsterMetaTypeToStringTable.end());

    return RPG_CHARACTER_MONSTERMETATYPE_INVALID;
  };

  static RPG_Character_MonsterMetaTypeToStringTable_t myRPG_Character_MonsterMetaTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterMetaTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterMetaTypeHelper(const RPG_Character_MonsterMetaTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterMetaTypeHelper& operator=(const RPG_Character_MonsterMetaTypeHelper&));
};

#endif
