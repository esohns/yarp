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

#ifndef RPG_COMBAT_DAMAGECOUNTERMEASURETYPE_H
#define RPG_COMBAT_DAMAGECOUNTERMEASURETYPE_H

enum RPG_Combat_DamageCounterMeasureType
{
  COUNTERMEASURE_CHECK = 0,
  COUNTERMEASURE_SPELL,
  //
  RPG_COMBAT_DAMAGECOUNTERMEASURETYPE_MAX,
  RPG_COMBAT_DAMAGECOUNTERMEASURETYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_DamageCounterMeasureType, std::string> RPG_Combat_DamageCounterMeasureTypeToStringTable_t;
typedef RPG_Combat_DamageCounterMeasureTypeToStringTable_t::const_iterator RPG_Combat_DamageCounterMeasureTypeToStringTableIterator_t;

class RPG_Combat_DamageCounterMeasureTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_DamageCounterMeasureTypeToStringTable.clear();
    myRPG_Combat_DamageCounterMeasureTypeToStringTable.insert(std::make_pair(COUNTERMEASURE_CHECK, ACE_TEXT_ALWAYS_CHAR("COUNTERMEASURE_CHECK")));
    myRPG_Combat_DamageCounterMeasureTypeToStringTable.insert(std::make_pair(COUNTERMEASURE_SPELL, ACE_TEXT_ALWAYS_CHAR("COUNTERMEASURE_SPELL")));
  };

  inline static std::string RPG_Combat_DamageCounterMeasureTypeToString(const RPG_Combat_DamageCounterMeasureType& element_in)
  {
    std::string result;
    RPG_Combat_DamageCounterMeasureTypeToStringTableIterator_t iterator = myRPG_Combat_DamageCounterMeasureTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_DamageCounterMeasureTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_DAMAGECOUNTERMEASURETYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_DamageCounterMeasureType stringToRPG_Combat_DamageCounterMeasureType(const std::string& string_in)
  {
    RPG_Combat_DamageCounterMeasureTypeToStringTableIterator_t iterator = myRPG_Combat_DamageCounterMeasureTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_DamageCounterMeasureTypeToStringTable.end());

    return RPG_COMBAT_DAMAGECOUNTERMEASURETYPE_INVALID;
  };

  static RPG_Combat_DamageCounterMeasureTypeToStringTable_t myRPG_Combat_DamageCounterMeasureTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageCounterMeasureTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageCounterMeasureTypeHelper(const RPG_Combat_DamageCounterMeasureTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageCounterMeasureTypeHelper& operator=(const RPG_Combat_DamageCounterMeasureTypeHelper&));
};

#endif
