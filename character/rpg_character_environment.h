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

#ifndef RPG_CHARACTER_ENVIRONMENT_H
#define RPG_CHARACTER_ENVIRONMENT_H

enum RPG_Character_Environment
{
  ENVIRONMENT_ANY = 0,
  ENVIRONMENT_UNDERGROUND,
  ENVIRONMENT_PLAINS_TEMPERATE,
  ENVIRONMENT_PLAINS_WARM,
  ENVIRONMENT_FORESTS_TEMPERATE,
  ENVIRONMENT_HILLS_TEMPERATE,
  ENVIRONMENT_HILLS_WARM,
  ENVIRONMENT_MOUNTAINS_TEMPERATE,
  ENVIRONMENT_DESERTS_WARM,
  ENVIRONMENT_PLANE_ANY_LAWFUL,
  ENVIRONMENT_PLANE_LAWFUL_GOOD,
  ENVIRONMENT_PLANE_ANY_GOOD,
  ENVIRONMENT_PLANE_CHAOTIC_GOOD,
  ENVIRONMENT_PLANE_ANY_EVIL,
  ENVIRONMENT_PLANE_CHAOTIC_EVIL,
  ENVIRONMENT_PLANE_AIR,
  ENVIRONMENT_PLANE_FIRE,
  //
  RPG_CHARACTER_ENVIRONMENT_MAX,
  RPG_CHARACTER_ENVIRONMENT_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Environment, std::string> RPG_Character_EnvironmentToStringTable_t;
typedef RPG_Character_EnvironmentToStringTable_t::const_iterator RPG_Character_EnvironmentToStringTableIterator_t;

class RPG_Character_EnvironmentHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_EnvironmentToStringTable.clear();
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_ANY, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_ANY")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_UNDERGROUND, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_UNDERGROUND")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLAINS_TEMPERATE, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLAINS_TEMPERATE")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLAINS_WARM, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLAINS_WARM")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_FORESTS_TEMPERATE, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_FORESTS_TEMPERATE")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_HILLS_TEMPERATE, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_HILLS_TEMPERATE")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_HILLS_WARM, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_HILLS_WARM")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_MOUNTAINS_TEMPERATE, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_MOUNTAINS_TEMPERATE")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_DESERTS_WARM, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_DESERTS_WARM")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_ANY_LAWFUL, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_ANY_LAWFUL")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_LAWFUL_GOOD, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_LAWFUL_GOOD")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_ANY_GOOD, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_ANY_GOOD")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_CHAOTIC_GOOD, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_CHAOTIC_GOOD")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_ANY_EVIL, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_ANY_EVIL")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_CHAOTIC_EVIL, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_CHAOTIC_EVIL")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_AIR, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_AIR")));
    myRPG_Character_EnvironmentToStringTable.insert(std::make_pair(ENVIRONMENT_PLANE_FIRE, ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_FIRE")));
  };

  inline static std::string RPG_Character_EnvironmentToString(const RPG_Character_Environment& element_in)
  {
    std::string result;
    RPG_Character_EnvironmentToStringTableIterator_t iterator = myRPG_Character_EnvironmentToStringTable.find(element_in);
    if (iterator != myRPG_Character_EnvironmentToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_ENVIRONMENT_INVALID");

    return result;
  };

  inline static RPG_Character_Environment stringToRPG_Character_Environment(const std::string& string_in)
  {
    RPG_Character_EnvironmentToStringTableIterator_t iterator = myRPG_Character_EnvironmentToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_EnvironmentToStringTable.end());

    return RPG_CHARACTER_ENVIRONMENT_INVALID;
  };

  static RPG_Character_EnvironmentToStringTable_t myRPG_Character_EnvironmentToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_EnvironmentHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_EnvironmentHelper(const RPG_Character_EnvironmentHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_EnvironmentHelper& operator=(const RPG_Character_EnvironmentHelper&));
};

#endif
