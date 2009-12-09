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

#ifndef RPG_COMBAT_ATTACKFORM_H
#define RPG_COMBAT_ATTACKFORM_H

enum RPG_Combat_AttackForm
{
  ATTACK_MELEE = 0,
  ATTACK_TOUCH,
  ATTACK_RANGED,
  //
  RPG_COMBAT_ATTACKFORM_MAX,
  RPG_COMBAT_ATTACKFORM_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Combat_AttackForm, std::string> RPG_Combat_AttackFormToStringTable_t;
typedef RPG_Combat_AttackFormToStringTable_t::const_iterator RPG_Combat_AttackFormToStringTableIterator_t;

class RPG_Combat_AttackFormHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_AttackFormToStringTable.clear();
    myRPG_Combat_AttackFormToStringTable.insert(std::make_pair(ATTACK_MELEE, ACE_TEXT_ALWAYS_CHAR("ATTACK_MELEE")));
    myRPG_Combat_AttackFormToStringTable.insert(std::make_pair(ATTACK_TOUCH, ACE_TEXT_ALWAYS_CHAR("ATTACK_TOUCH")));
    myRPG_Combat_AttackFormToStringTable.insert(std::make_pair(ATTACK_RANGED, ACE_TEXT_ALWAYS_CHAR("ATTACK_RANGED")));
  };

  inline static std::string RPG_Combat_AttackFormToString(const RPG_Combat_AttackForm& element_in)
  {
    std::string result;
    RPG_Combat_AttackFormToStringTableIterator_t iterator = myRPG_Combat_AttackFormToStringTable.find(element_in);
    if (iterator != myRPG_Combat_AttackFormToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_ATTACKFORM_INVALID");

    return result;
  };

  inline static RPG_Combat_AttackForm stringToRPG_Combat_AttackForm(const std::string& string_in)
  {
    RPG_Combat_AttackFormToStringTableIterator_t iterator = myRPG_Combat_AttackFormToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_AttackFormToStringTable.end());

    return RPG_COMBAT_ATTACKFORM_INVALID;
  };

  static RPG_Combat_AttackFormToStringTable_t myRPG_Combat_AttackFormToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AttackFormHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AttackFormHelper(const RPG_Combat_AttackFormHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_AttackFormHelper& operator=(const RPG_Combat_AttackFormHelper&));
};

#endif
