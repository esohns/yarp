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

#ifndef RPG_COMBAT_AREAOFEFFECT_H
#define RPG_COMBAT_AREAOFEFFECT_H

enum RPG_Combat_AreaOfEffect
{
  AREA_LINE = 0,
  AREA_CUBE,
  AREA_CONE,
  //
  RPG_COMBAT_AREAOFEFFECT_MAX,
  RPG_COMBAT_AREAOFEFFECT_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_AreaOfEffect, std::string> RPG_Combat_AreaOfEffectToStringTable_t;
typedef RPG_Combat_AreaOfEffectToStringTable_t::const_iterator RPG_Combat_AreaOfEffectToStringTableIterator_t;

class RPG_Combat_AreaOfEffectHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_AreaOfEffectToStringTable.clear();
    myRPG_Combat_AreaOfEffectToStringTable.insert(std::make_pair(AREA_LINE, ACE_TEXT_ALWAYS_CHAR("AREA_LINE")));
    myRPG_Combat_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CUBE, ACE_TEXT_ALWAYS_CHAR("AREA_CUBE")));
    myRPG_Combat_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CONE, ACE_TEXT_ALWAYS_CHAR("AREA_CONE")));
  };

  inline static std::string RPG_Combat_AreaOfEffectToString(const RPG_Combat_AreaOfEffect& element_in)
  {
    std::string result;
    RPG_Combat_AreaOfEffectToStringTableIterator_t iterator = myRPG_Combat_AreaOfEffectToStringTable.find(element_in);
    if (iterator != myRPG_Combat_AreaOfEffectToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_AREAOFEFFECT_INVALID");

    return result;
  };

  inline static RPG_Combat_AreaOfEffect stringToRPG_Combat_AreaOfEffect(const std::string& string_in)
  {
    RPG_Combat_AreaOfEffectToStringTableIterator_t iterator = myRPG_Combat_AreaOfEffectToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_AreaOfEffectToStringTable.end());

    return RPG_COMBAT_AREAOFEFFECT_INVALID;
  };

  static RPG_Combat_AreaOfEffectToStringTable_t myRPG_Combat_AreaOfEffectToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AreaOfEffectHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AreaOfEffectHelper(const RPG_Combat_AreaOfEffectHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AreaOfEffectHelper& operator=(const RPG_Combat_AreaOfEffectHelper&));
};

#endif
