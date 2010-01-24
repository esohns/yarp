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

#ifndef RPG_CHARACTER_CLIMATE_H
#define RPG_CHARACTER_CLIMATE_H

enum RPG_Character_Climate
{
  CLIMATE_COLD = 0,
  CLIMATE_TEMPERATE,
  CLIMATE_WARM,
  CLIMATE_ANY,
  //
  RPG_CHARACTER_CLIMATE_MAX,
  RPG_CHARACTER_CLIMATE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Climate, std::string> RPG_Character_ClimateToStringTable_t;
typedef RPG_Character_ClimateToStringTable_t::const_iterator RPG_Character_ClimateToStringTableIterator_t;

class RPG_Character_ClimateHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_ClimateToStringTable.clear();
    myRPG_Character_ClimateToStringTable.insert(std::make_pair(CLIMATE_COLD, ACE_TEXT_ALWAYS_CHAR("CLIMATE_COLD")));
    myRPG_Character_ClimateToStringTable.insert(std::make_pair(CLIMATE_TEMPERATE, ACE_TEXT_ALWAYS_CHAR("CLIMATE_TEMPERATE")));
    myRPG_Character_ClimateToStringTable.insert(std::make_pair(CLIMATE_WARM, ACE_TEXT_ALWAYS_CHAR("CLIMATE_WARM")));
    myRPG_Character_ClimateToStringTable.insert(std::make_pair(CLIMATE_ANY, ACE_TEXT_ALWAYS_CHAR("CLIMATE_ANY")));
  };

  inline static std::string RPG_Character_ClimateToString(const RPG_Character_Climate& element_in)
  {
    std::string result;
    RPG_Character_ClimateToStringTableIterator_t iterator = myRPG_Character_ClimateToStringTable.find(element_in);
    if (iterator != myRPG_Character_ClimateToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_CLIMATE_INVALID");

    return result;
  };

  inline static RPG_Character_Climate stringToRPG_Character_Climate(const std::string& string_in)
  {
    RPG_Character_ClimateToStringTableIterator_t iterator = myRPG_Character_ClimateToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_ClimateToStringTable.end());

    return RPG_CHARACTER_CLIMATE_INVALID;
  };

  static RPG_Character_ClimateToStringTable_t myRPG_Character_ClimateToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_ClimateHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_ClimateHelper(const RPG_Character_ClimateHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_ClimateHelper& operator=(const RPG_Character_ClimateHelper&));
};

#endif
