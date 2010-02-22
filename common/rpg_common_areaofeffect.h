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

#ifndef RPG_COMMON_AREAOFEFFECT_H
#define RPG_COMMON_AREAOFEFFECT_H

enum RPG_Common_AreaOfEffect
{
  AREA_LINE = 0,
  AREA_CUBE,
  AREA_CONE,
  AREA_CYLINDER,
  AREA_WALL,
  //
  RPG_COMMON_AREAOFEFFECT_MAX,
  RPG_COMMON_AREAOFEFFECT_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_AreaOfEffect, std::string> RPG_Common_AreaOfEffectToStringTable_t;
typedef RPG_Common_AreaOfEffectToStringTable_t::const_iterator RPG_Common_AreaOfEffectToStringTableIterator_t;

class RPG_Common_AreaOfEffectHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_AreaOfEffectToStringTable.clear();
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_LINE, ACE_TEXT_ALWAYS_CHAR("AREA_LINE")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CUBE, ACE_TEXT_ALWAYS_CHAR("AREA_CUBE")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CONE, ACE_TEXT_ALWAYS_CHAR("AREA_CONE")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CYLINDER, ACE_TEXT_ALWAYS_CHAR("AREA_CYLINDER")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_WALL, ACE_TEXT_ALWAYS_CHAR("AREA_WALL")));
  };

  inline static std::string RPG_Common_AreaOfEffectToString(const RPG_Common_AreaOfEffect& element_in)
  {
    std::string result;
    RPG_Common_AreaOfEffectToStringTableIterator_t iterator = myRPG_Common_AreaOfEffectToStringTable.find(element_in);
    if (iterator != myRPG_Common_AreaOfEffectToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_AREAOFEFFECT_INVALID");

    return result;
  };

  inline static RPG_Common_AreaOfEffect stringToRPG_Common_AreaOfEffect(const std::string& string_in)
  {
    RPG_Common_AreaOfEffectToStringTableIterator_t iterator = myRPG_Common_AreaOfEffectToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_AreaOfEffectToStringTable.end());

    return RPG_COMMON_AREAOFEFFECT_INVALID;
  };

  static RPG_Common_AreaOfEffectToStringTable_t myRPG_Common_AreaOfEffectToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AreaOfEffectHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AreaOfEffectHelper(const RPG_Common_AreaOfEffectHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AreaOfEffectHelper& operator=(const RPG_Common_AreaOfEffectHelper&));
};

#endif
