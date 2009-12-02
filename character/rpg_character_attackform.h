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

#ifndef RPG_CHARACTER_ATTACKFORM_H
#define RPG_CHARACTER_ATTACKFORM_H

enum RPG_Character_AttackForm
{
  ATTACK_MELEE = 0,
  ATTACK_TOUCH,
  ATTACK_RANGED,
  //
  RPG_CHARACTER_ATTACKFORM_MAX,
  RPG_CHARACTER_ATTACKFORM_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_AttackForm, std::string> RPG_Character_AttackFormToStringTable_t;
typedef RPG_Character_AttackFormToStringTable_t::const_iterator RPG_Character_AttackFormToStringTableIterator_t;

class RPG_Character_AttackFormHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_AttackFormToStringTable.clear();
    myRPG_Character_AttackFormToStringTable.insert(std::make_pair(ATTACK_MELEE, ACE_TEXT_ALWAYS_CHAR("ATTACK_MELEE")));
    myRPG_Character_AttackFormToStringTable.insert(std::make_pair(ATTACK_TOUCH, ACE_TEXT_ALWAYS_CHAR("ATTACK_TOUCH")));
    myRPG_Character_AttackFormToStringTable.insert(std::make_pair(ATTACK_RANGED, ACE_TEXT_ALWAYS_CHAR("ATTACK_RANGED")));
  };

  inline static std::string RPG_Character_AttackFormToString(const RPG_Character_AttackForm& element_in)
  {
    std::string result;
    RPG_Character_AttackFormToStringTableIterator_t iterator = myRPG_Character_AttackFormToStringTable.find(element_in);
    if (iterator != myRPG_Character_AttackFormToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_ATTACKFORM_INVALID");

    return result;
  };

  inline static RPG_Character_AttackForm stringToRPG_Character_AttackForm(const std::string& string_in)
  {
    RPG_Character_AttackFormToStringTableIterator_t iterator = myRPG_Character_AttackFormToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_AttackFormToStringTable.end());

    return RPG_CHARACTER_ATTACKFORM_INVALID;
  };

  static RPG_Character_AttackFormToStringTable_t myRPG_Character_AttackFormToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AttackFormHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AttackFormHelper(const RPG_Character_AttackFormHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AttackFormHelper& operator=(const RPG_Character_AttackFormHelper&));
};

#endif
