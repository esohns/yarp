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

#ifndef RPG_CHARACTER_MONSTERWEAPON_H
#define RPG_CHARACTER_MONSTERWEAPON_H

enum RPG_Character_MonsterWeapon
{
  INCORPOREAL_TOUCH = 0,
  NATURALWEAPON_BITE,
  NATURALWEAPON_CLAW_TALON,
  NATURALWEAPON_GORE,
  NATURALWEAPON_SLAP_SLAM,
  NATURALWEAPON_STING,
  NATURALWEAPON_TENTACLE,
  NATURALWEAPON_SPIT_ACID,
  NATURALWEAPON_WEB,
  NATURALWEAPON_ROCK_STONE,
  STANDARDWEAPON_BLUDGEONING,
  STANDARDWEAPON_PIERCING,
  STANDARDWEAPON_SLASHING,
  RAY_ELECTRICITY,
  RAY_LIGHT,
  //
  RPG_CHARACTER_MONSTERWEAPON_MAX,
  RPG_CHARACTER_MONSTERWEAPON_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_MonsterWeapon, std::string> RPG_Character_MonsterWeaponToStringTable_t;
typedef RPG_Character_MonsterWeaponToStringTable_t::const_iterator RPG_Character_MonsterWeaponToStringTableIterator_t;

class RPG_Character_MonsterWeaponHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_MonsterWeaponToStringTable.clear();
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(INCORPOREAL_TOUCH, ACE_TEXT_ALWAYS_CHAR("INCORPOREAL_TOUCH")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_BITE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_BITE")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_CLAW_TALON, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_CLAW_TALON")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_GORE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_GORE")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_SLAP_SLAM, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_SLAP_SLAM")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_STING, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_STING")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_TENTACLE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_TENTACLE")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_SPIT_ACID, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_SPIT_ACID")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_WEB, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_WEB")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_ROCK_STONE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_ROCK_STONE")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(STANDARDWEAPON_BLUDGEONING, ACE_TEXT_ALWAYS_CHAR("STANDARDWEAPON_BLUDGEONING")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(STANDARDWEAPON_PIERCING, ACE_TEXT_ALWAYS_CHAR("STANDARDWEAPON_PIERCING")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(STANDARDWEAPON_SLASHING, ACE_TEXT_ALWAYS_CHAR("STANDARDWEAPON_SLASHING")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(RAY_ELECTRICITY, ACE_TEXT_ALWAYS_CHAR("RAY_ELECTRICITY")));
    myRPG_Character_MonsterWeaponToStringTable.insert(std::make_pair(RAY_LIGHT, ACE_TEXT_ALWAYS_CHAR("RAY_LIGHT")));
  };

  inline static std::string RPG_Character_MonsterWeaponToString(const RPG_Character_MonsterWeapon& element_in)
  {
    std::string result;
    RPG_Character_MonsterWeaponToStringTableIterator_t iterator = myRPG_Character_MonsterWeaponToStringTable.find(element_in);
    if (iterator != myRPG_Character_MonsterWeaponToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_MONSTERWEAPON_INVALID");

    return result;
  };

  inline static RPG_Character_MonsterWeapon stringToRPG_Character_MonsterWeapon(const std::string& string_in)
  {
    RPG_Character_MonsterWeaponToStringTableIterator_t iterator = myRPG_Character_MonsterWeaponToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_MonsterWeaponToStringTable.end());

    return RPG_CHARACTER_MONSTERWEAPON_INVALID;
  };

  static RPG_Character_MonsterWeaponToStringTable_t myRPG_Character_MonsterWeaponToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterWeaponHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterWeaponHelper(const RPG_Character_MonsterWeaponHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterWeaponHelper& operator=(const RPG_Character_MonsterWeaponHelper&));
};

#endif
