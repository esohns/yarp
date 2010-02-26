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

#ifndef RPG_COMMON_CREATURESUBTYPE_H
#define RPG_COMMON_CREATURESUBTYPE_H

enum RPG_Common_CreatureSubType
{
  SUBTYPE_AIR = 0,
  SUBTYPE_ANGEL,
  SUBTYPE_AQUATIC,
  SUBTYPE_ARCHON,
  SUBTYPE_AUGMENTED,
  SUBTYPE_CHAOTIC,
  SUBTYPE_COLD,
  SUBTYPE_EARTH,
  SUBTYPE_EVIL,
  SUBTYPE_EXTRAPLANAR,
  SUBTYPE_FIRE,
  SUBTYPE_GOBLINOID,
  SUBTYPE_GOOD,
  SUBTYPE_INCORPOREAL,
  SUBTYPE_LAWFUL,
  SUBTYPE_NATIVE,
  SUBTYPE_REPTILIAN,
  SUBTYPE_SHAPECHANGER,
  SUBTYPE_SWARM,
  SUBTYPE_WATER,
  //
  RPG_COMMON_CREATURESUBTYPE_MAX,
  RPG_COMMON_CREATURESUBTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_CreatureSubType, std::string> RPG_Common_CreatureSubTypeToStringTable_t;
typedef RPG_Common_CreatureSubTypeToStringTable_t::const_iterator RPG_Common_CreatureSubTypeToStringTableIterator_t;

class RPG_Common_CreatureSubTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_CreatureSubTypeToStringTable.clear();
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_AIR, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_AIR")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_ANGEL, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_ANGEL")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_AQUATIC, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_AQUATIC")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_ARCHON, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_ARCHON")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_AUGMENTED, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_AUGMENTED")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_CHAOTIC, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_CHAOTIC")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_COLD, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_COLD")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_EARTH, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_EARTH")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_EVIL, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_EVIL")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_EXTRAPLANAR, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_EXTRAPLANAR")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_FIRE, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_FIRE")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_GOBLINOID, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_GOBLINOID")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_GOOD, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_GOOD")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_INCORPOREAL, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_INCORPOREAL")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_LAWFUL, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_LAWFUL")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_NATIVE, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_NATIVE")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_REPTILIAN, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_REPTILIAN")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_SHAPECHANGER, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_SHAPECHANGER")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_SWARM, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_SWARM")));
    myRPG_Common_CreatureSubTypeToStringTable.insert(std::make_pair(SUBTYPE_WATER, ACE_TEXT_ALWAYS_CHAR("SUBTYPE_WATER")));
  };

  inline static std::string RPG_Common_CreatureSubTypeToString(const RPG_Common_CreatureSubType& element_in)
  {
    std::string result;
    RPG_Common_CreatureSubTypeToStringTableIterator_t iterator = myRPG_Common_CreatureSubTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_CreatureSubTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_CREATURESUBTYPE_INVALID");

    return result;
  };

  inline static RPG_Common_CreatureSubType stringToRPG_Common_CreatureSubType(const std::string& string_in)
  {
    RPG_Common_CreatureSubTypeToStringTableIterator_t iterator = myRPG_Common_CreatureSubTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_CreatureSubTypeToStringTable.end());

    return RPG_COMMON_CREATURESUBTYPE_INVALID;
  };

  static RPG_Common_CreatureSubTypeToStringTable_t myRPG_Common_CreatureSubTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CreatureSubTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CreatureSubTypeHelper(const RPG_Common_CreatureSubTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CreatureSubTypeHelper& operator=(const RPG_Common_CreatureSubTypeHelper&));
};

#endif
