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

#ifndef RPG_COMMON_CREATUREMETATYPE_H
#define RPG_COMMON_CREATUREMETATYPE_H

enum RPG_Common_CreatureMetaType
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
  RPG_COMMON_CREATUREMETATYPE_MAX,
  RPG_COMMON_CREATUREMETATYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_CreatureMetaType, std::string> RPG_Common_CreatureMetaTypeToStringTable_t;
typedef RPG_Common_CreatureMetaTypeToStringTable_t::const_iterator RPG_Common_CreatureMetaTypeToStringTableIterator_t;

class RPG_Common_CreatureMetaTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_CreatureMetaTypeToStringTable.clear();
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_ABERRATION, ACE_TEXT_ALWAYS_CHAR("METATYPE_ABERRATION")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_ANIMAL, ACE_TEXT_ALWAYS_CHAR("METATYPE_ANIMAL")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_CONSTRUCT, ACE_TEXT_ALWAYS_CHAR("METATYPE_CONSTRUCT")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_DRAGON, ACE_TEXT_ALWAYS_CHAR("METATYPE_DRAGON")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_ELEMENTAL, ACE_TEXT_ALWAYS_CHAR("METATYPE_ELEMENTAL")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_FEY, ACE_TEXT_ALWAYS_CHAR("METATYPE_FEY")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_GIANT, ACE_TEXT_ALWAYS_CHAR("METATYPE_GIANT")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_HUMANOID, ACE_TEXT_ALWAYS_CHAR("METATYPE_HUMANOID")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_MAGICAL_BEAST, ACE_TEXT_ALWAYS_CHAR("METATYPE_MAGICAL_BEAST")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_MONSTROUS_HUMANOID, ACE_TEXT_ALWAYS_CHAR("METATYPE_MONSTROUS_HUMANOID")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_OOZE, ACE_TEXT_ALWAYS_CHAR("METATYPE_OOZE")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_OUTSIDER, ACE_TEXT_ALWAYS_CHAR("METATYPE_OUTSIDER")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_PLANT, ACE_TEXT_ALWAYS_CHAR("METATYPE_PLANT")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_UNDEAD, ACE_TEXT_ALWAYS_CHAR("METATYPE_UNDEAD")));
    myRPG_Common_CreatureMetaTypeToStringTable.insert(std::make_pair(METATYPE_VERMIN, ACE_TEXT_ALWAYS_CHAR("METATYPE_VERMIN")));
  };

  inline static std::string RPG_Common_CreatureMetaTypeToString(const RPG_Common_CreatureMetaType& element_in)
  {
    std::string result;
    RPG_Common_CreatureMetaTypeToStringTableIterator_t iterator = myRPG_Common_CreatureMetaTypeToStringTable.find(element_in);
    if (iterator != myRPG_Common_CreatureMetaTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_CREATUREMETATYPE_INVALID");

    return result;
  };

  inline static RPG_Common_CreatureMetaType stringToRPG_Common_CreatureMetaType(const std::string& string_in)
  {
    RPG_Common_CreatureMetaTypeToStringTableIterator_t iterator = myRPG_Common_CreatureMetaTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_CreatureMetaTypeToStringTable.end());

    return RPG_COMMON_CREATUREMETATYPE_INVALID;
  };

  static RPG_Common_CreatureMetaTypeToStringTable_t myRPG_Common_CreatureMetaTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CreatureMetaTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CreatureMetaTypeHelper(const RPG_Common_CreatureMetaTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CreatureMetaTypeHelper& operator=(const RPG_Common_CreatureMetaTypeHelper&));
};

#endif
