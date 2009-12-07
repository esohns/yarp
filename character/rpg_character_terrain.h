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

#ifndef RPG_CHARACTER_TERRAIN_H
#define RPG_CHARACTER_TERRAIN_H

enum RPG_Character_Terrain
{
  TERRAIN_UNDERGROUND = 0,
  TERRAIN_PLAINS,
  TERRAIN_FORESTS,
  TERRAIN_HILLS,
  TERRAIN_MOUNTAINS,
  TERRAIN_DESERTS,
  TERRAIN_MATERIALPLANE_ANY,
  TERRAIN_TRANSITIVEPLANE_ASTRAL,
  TERRAIN_TRANSITIVEPLANE_ETHERAL,
  TERRAIN_TRANSITIVEPLANE_SHADOW,
  TERRAIN_TRANSITIVEPLANE_ANY,
  TERRAIN_INNERPLANE_AIR,
  TERRAIN_INNERPLANE_EARTH,
  TERRAIN_INNERPLANE_FIRE,
  TERRAIN_INNERPLANE_WATER,
  TERRAIN_INNERPLANE_POSITIVE,
  TERRAIN_INNERPLANE_NEGATIVE,
  TERRAIN_INNERPLANE_ANY,
  TERRAIN_OUTERPLANE_LAWFUL_ANY,
  TERRAIN_OUTERPLANE_CHAOTIC_ANY,
  TERRAIN_OUTERPLANE_GOOD_ANY,
  TERRAIN_OUTERPLANE_EVIL_ANY,
  TERRAIN_OUTERPLANE_LAWFUL_GOOD,
  TERRAIN_OUTERPLANE_LAWFUL_EVIL,
  TERRAIN_OUTERPLANE_CHAOTIC_GOOD,
  TERRAIN_OUTERPLANE_CHAOTIC_EVIL,
  TERRAIN_OUTERPLANE_NEUTRAL,
  TERRAIN_OUTERPLANE_MILD_ANY,
  TERRAIN_OUTERPLANE_STRONG_ANY,
  TERRAIN_OUTERPLANE_ANY,
  TERRAIN_ANY,
  //
  RPG_CHARACTER_TERRAIN_MAX,
  RPG_CHARACTER_TERRAIN_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Terrain, std::string> RPG_Character_TerrainToStringTable_t;
typedef RPG_Character_TerrainToStringTable_t::const_iterator RPG_Character_TerrainToStringTableIterator_t;

class RPG_Character_TerrainHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_TerrainToStringTable.clear();
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_UNDERGROUND, ACE_TEXT_ALWAYS_CHAR("TERRAIN_UNDERGROUND")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_PLAINS, ACE_TEXT_ALWAYS_CHAR("TERRAIN_PLAINS")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_FORESTS, ACE_TEXT_ALWAYS_CHAR("TERRAIN_FORESTS")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_HILLS, ACE_TEXT_ALWAYS_CHAR("TERRAIN_HILLS")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_MOUNTAINS, ACE_TEXT_ALWAYS_CHAR("TERRAIN_MOUNTAINS")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_DESERTS, ACE_TEXT_ALWAYS_CHAR("TERRAIN_DESERTS")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_MATERIALPLANE_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_MATERIALPLANE_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_TRANSITIVEPLANE_ASTRAL, ACE_TEXT_ALWAYS_CHAR("TERRAIN_TRANSITIVEPLANE_ASTRAL")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_TRANSITIVEPLANE_ETHERAL, ACE_TEXT_ALWAYS_CHAR("TERRAIN_TRANSITIVEPLANE_ETHERAL")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_TRANSITIVEPLANE_SHADOW, ACE_TEXT_ALWAYS_CHAR("TERRAIN_TRANSITIVEPLANE_SHADOW")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_TRANSITIVEPLANE_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_TRANSITIVEPLANE_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_AIR, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_AIR")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_EARTH, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_EARTH")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_FIRE, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_FIRE")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_WATER, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_WATER")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_POSITIVE, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_POSITIVE")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_NEGATIVE, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_NEGATIVE")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_INNERPLANE_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_INNERPLANE_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_LAWFUL_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_LAWFUL_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_CHAOTIC_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_CHAOTIC_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_GOOD_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_GOOD_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_EVIL_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_EVIL_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_LAWFUL_GOOD, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_LAWFUL_GOOD")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_LAWFUL_EVIL, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_LAWFUL_EVIL")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_CHAOTIC_GOOD, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_CHAOTIC_GOOD")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_CHAOTIC_EVIL, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_CHAOTIC_EVIL")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_NEUTRAL, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_NEUTRAL")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_MILD_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_MILD_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_STRONG_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_STRONG_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_OUTERPLANE_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_OUTERPLANE_ANY")));
    myRPG_Character_TerrainToStringTable.insert(std::make_pair(TERRAIN_ANY, ACE_TEXT_ALWAYS_CHAR("TERRAIN_ANY")));
  };

  inline static std::string RPG_Character_TerrainToString(const RPG_Character_Terrain& element_in)
  {
    std::string result;
    RPG_Character_TerrainToStringTableIterator_t iterator = myRPG_Character_TerrainToStringTable.find(element_in);
    if (iterator != myRPG_Character_TerrainToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_TERRAIN_INVALID");

    return result;
  };

  inline static RPG_Character_Terrain stringToRPG_Character_Terrain(const std::string& string_in)
  {
    RPG_Character_TerrainToStringTableIterator_t iterator = myRPG_Character_TerrainToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_TerrainToStringTable.end());

    return RPG_CHARACTER_TERRAIN_INVALID;
  };

  static RPG_Character_TerrainToStringTable_t myRPG_Character_TerrainToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_TerrainHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_TerrainHelper(const RPG_Character_TerrainHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_TerrainHelper& operator=(const RPG_Character_TerrainHelper&));
};

#endif
