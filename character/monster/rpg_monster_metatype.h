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

#ifndef RPG_MONSTER_METATYPE_H
#define RPG_MONSTER_METATYPE_H

enum RPG_Monster_MetaType
{
  METATYPE_ABERRATION = 0,
  METATYPE_ANIMAL,
  METATYPE_CONSTRUCT,
  METATYPE_DRAGON,
  METATYPE_ELEMENTAL,
  METATYPE_FEY,
  METATYPE_GIANT,
  METATYPE_HUMANOID,
  METATYPE_MAGICAL_BEAST,
  METATYPE_MONSTROUS_HUMANOID,
  METATYPE_OOZE,
  METATYPE_OUTSIDER,
  METATYPE_PLANT,
  METATYPE_UNDEAD,
  METATYPE_VERMIN,
  //
  RPG_MONSTER_METATYPE_MAX,
  RPG_MONSTER_METATYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Monster_MetaType, std::string> RPG_Monster_MetaTypeToStringTable_t;
typedef RPG_Monster_MetaTypeToStringTable_t::const_iterator RPG_Monster_MetaTypeToStringTableIterator_t;

class RPG_Monster_MetaTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Monster_MetaTypeToStringTable.clear();
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_ABERRATION, ACE_TEXT_ALWAYS_CHAR("METATYPE_ABERRATION")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("METATYPE_ANIMAL")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_CONSTRUCT, ACE_TEXT_ALWAYS_CHAR("METATYPE_CONSTRUCT")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_DRAGON, ACE_TEXT_ALWAYS_CHAR("METATYPE_DRAGON")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_ELEMENTAL, ACE_TEXT_ALWAYS_CHAR("METATYPE_ELEMENTAL")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_FEY, ACE_TEXT_ALWAYS_CHAR("METATYPE_FEY")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_GIANT, ACE_TEXT_ALWAYS_CHAR("METATYPE_GIANT")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_HUMANOID, ACE_TEXT_ALWAYS_CHAR("METATYPE_HUMANOID")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_MAGICAL_BEAST, ACE_TEXT_ALWAYS_CHAR("METATYPE_MAGICAL_BEAST")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_MONSTROUS_HUMANOID, ACE_TEXT_ALWAYS_CHAR("METATYPE_MONSTROUS_HUMANOID")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_OOZE, ACE_TEXT_ALWAYS_CHAR("METATYPE_OOZE")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_OUTSIDER, ACE_TEXT_ALWAYS_CHAR("METATYPE_OUTSIDER")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_PLANT, ACE_TEXT_ALWAYS_CHAR("METATYPE_PLANT")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_UNDEAD, ACE_TEXT_ALWAYS_CHAR("METATYPE_UNDEAD")));
    myRPG_Monster_MetaTypeToStringTable.insert(std::make_pair(METATYPE_VERMIN, ACE_TEXT_ALWAYS_CHAR("METATYPE_VERMIN")));
  };

  inline static std::string RPG_Monster_MetaTypeToString(const RPG_Monster_MetaType& element_in)
  {
    std::string result;
    RPG_Monster_MetaTypeToStringTableIterator_t iterator = myRPG_Monster_MetaTypeToStringTable.find(element_in);
    if (iterator != myRPG_Monster_MetaTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MONSTER_METATYPE_INVALID");

    return result;
  };

  inline static RPG_Monster_MetaType stringToRPG_Monster_MetaType(const std::string& string_in)
  {
    RPG_Monster_MetaTypeToStringTableIterator_t iterator = myRPG_Monster_MetaTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Monster_MetaTypeToStringTable.end());

    return RPG_MONSTER_METATYPE_INVALID;
  };

  static RPG_Monster_MetaTypeToStringTable_t myRPG_Monster_MetaTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_MetaTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_MetaTypeHelper(const RPG_Monster_MetaTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_MetaTypeHelper& operator=(const RPG_Monster_MetaTypeHelper&));
};

#endif
