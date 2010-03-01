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

#ifndef RPG_CHARACTER_PLANE_H
#define RPG_CHARACTER_PLANE_H

enum RPG_Character_Plane
{
  PLANE_INNER = 0,
  PLANE_MATERIAL,
  PLANE_OUTER,
  PLANE_TRANSITIVE,
  PLANE_ANY,
  //
  RPG_CHARACTER_PLANE_MAX,
  RPG_CHARACTER_PLANE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Plane, std::string> RPG_Character_PlaneToStringTable_t;
typedef RPG_Character_PlaneToStringTable_t::const_iterator RPG_Character_PlaneToStringTableIterator_t;

class RPG_Character_PlaneHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_PlaneToStringTable.clear();
    myRPG_Character_PlaneToStringTable.insert(std::make_pair(PLANE_INNER, ACE_TEXT_ALWAYS_CHAR("PLANE_INNER")));
    myRPG_Character_PlaneToStringTable.insert(std::make_pair(PLANE_MATERIAL, ACE_TEXT_ALWAYS_CHAR("PLANE_MATERIAL")));
    myRPG_Character_PlaneToStringTable.insert(std::make_pair(PLANE_OUTER, ACE_TEXT_ALWAYS_CHAR("PLANE_OUTER")));
    myRPG_Character_PlaneToStringTable.insert(std::make_pair(PLANE_TRANSITIVE, ACE_TEXT_ALWAYS_CHAR("PLANE_TRANSITIVE")));
    myRPG_Character_PlaneToStringTable.insert(std::make_pair(PLANE_ANY, ACE_TEXT_ALWAYS_CHAR("PLANE_ANY")));
  };

  inline static std::string RPG_Character_PlaneToString(const RPG_Character_Plane& element_in)
  {
    std::string result;
    RPG_Character_PlaneToStringTableIterator_t iterator = myRPG_Character_PlaneToStringTable.find(element_in);
    if (iterator != myRPG_Character_PlaneToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_PLANE_INVALID");

    return result;
  };

  inline static RPG_Character_Plane stringToRPG_Character_Plane(const std::string& string_in)
  {
    RPG_Character_PlaneToStringTableIterator_t iterator = myRPG_Character_PlaneToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_PlaneToStringTable.end());

    return RPG_CHARACTER_PLANE_INVALID;
  };

  static RPG_Character_PlaneToStringTable_t myRPG_Character_PlaneToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_PlaneHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_PlaneHelper(const RPG_Character_PlaneHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_PlaneHelper& operator=(const RPG_Character_PlaneHelper&));
};

#endif
