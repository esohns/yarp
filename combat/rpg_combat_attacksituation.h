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

#ifndef RPG_COMBAT_ATTACKSITUATION_H
#define RPG_COMBAT_ATTACKSITUATION_H

enum RPG_Combat_AttackSituation
{
  ATTACK_NORMAL = 0,
  ATTACK_BACKSTAB,
  ATTACK_SURPRISE,
  //
  RPG_COMBAT_ATTACKSITUATION_MAX,
  RPG_COMBAT_ATTACKSITUATION_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_AttackSituation, std::string> RPG_Combat_AttackSituationToStringTable_t;
typedef RPG_Combat_AttackSituationToStringTable_t::const_iterator RPG_Combat_AttackSituationToStringTableIterator_t;

class RPG_Combat_AttackSituationHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_AttackSituationToStringTable.clear();
    myRPG_Combat_AttackSituationToStringTable.insert(std::make_pair(ATTACK_NORMAL, ACE_TEXT_ALWAYS_CHAR("ATTACK_NORMAL")));
    myRPG_Combat_AttackSituationToStringTable.insert(std::make_pair(ATTACK_BACKSTAB, ACE_TEXT_ALWAYS_CHAR("ATTACK_BACKSTAB")));
    myRPG_Combat_AttackSituationToStringTable.insert(std::make_pair(ATTACK_SURPRISE, ACE_TEXT_ALWAYS_CHAR("ATTACK_SURPRISE")));
  };

  inline static std::string RPG_Combat_AttackSituationToString(const RPG_Combat_AttackSituation& element_in)
  {
    std::string result;
    RPG_Combat_AttackSituationToStringTableIterator_t iterator = myRPG_Combat_AttackSituationToStringTable.find(element_in);
    if (iterator != myRPG_Combat_AttackSituationToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_ATTACKSITUATION_INVALID");

    return result;
  };

  inline static RPG_Combat_AttackSituation stringToRPG_Combat_AttackSituation(const std::string& string_in)
  {
    RPG_Combat_AttackSituationToStringTableIterator_t iterator = myRPG_Combat_AttackSituationToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_AttackSituationToStringTable.end());

    return RPG_COMBAT_ATTACKSITUATION_INVALID;
  };

  static RPG_Combat_AttackSituationToStringTable_t myRPG_Combat_AttackSituationToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AttackSituationHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AttackSituationHelper(const RPG_Combat_AttackSituationHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AttackSituationHelper& operator=(const RPG_Combat_AttackSituationHelper&));
};

#endif