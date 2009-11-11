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
#ifndef RPG_CHARACTER_MONSTER_COMMON_TOOLS_H
#define RPG_CHARACTER_MONSTER_COMMON_TOOLS_H

#include "rpg_character_monster_common.h"

#include <ace/Global_Macros.h>

#include <map>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Monster_Common_Tools
{
 public:
  // init static data
  static void init();

  static const RPG_Character_Size stringToSize(const std::string&); // string
  static const RPG_Character_MonsterMetaType stringToMonsterMetaType(const std::string&); // string
  static const RPG_Character_MonsterSubType stringToMonsterSubType(const std::string&); // string
  static const RPG_Character_NaturalWeapon stringToNaturalWeapon(const std::string&); // string
  static const RPG_Character_MonsterAttackForm stringToMonsterAttackForm(const std::string&); // string
  static const RPG_Character_Environment stringToEnvironment(const std::string&); // string
  static const RPG_Character_Organization stringToOrganization(const std::string&); // string

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Monster_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools(const RPG_Character_Monster_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Common_Tools& operator=(const RPG_Character_Monster_Common_Tools&));

  // helper methods
  static void initStringConversionTables();

  // some useful types
  typedef std::map<std::string, RPG_Character_MonsterMetaType> RPG_StringToMonsterMetaTypeTable_t;
  typedef RPG_StringToMonsterMetaTypeTable_t::const_iterator RPG_StringToMonsterMetaTypeTableIterator_t;
  typedef std::map<std::string, RPG_Character_MonsterSubType> RPG_StringToMonsterSubTypeTable_t;
  typedef RPG_StringToMonsterSubTypeTable_t::const_iterator RPG_StringToMonsterSubTypeTableIterator_t;
  typedef std::map<std::string, RPG_Character_NaturalWeapon> RPG_StringToNaturalWeaponTable_t;
  typedef RPG_StringToNaturalWeaponTable_t::const_iterator RPG_StringToNaturalWeaponTableIterator_t;
  typedef std::map<std::string, RPG_Character_MonsterAttackForm> RPG_StringToMonsterAttackFormTable_t;
  typedef RPG_StringToMonsterAttackFormTable_t::const_iterator RPG_StringToMonsterAttackFormTableIterator_t;
  typedef std::map<std::string, RPG_Character_Size> RPG_StringToSizeTable_t;
  typedef RPG_StringToSizeTable_t::const_iterator RPG_StringToSizeTableIterator_t;
  typedef std::map<std::string, RPG_Character_Environment> RPG_StringToEnvironmentTable_t;
  typedef RPG_StringToEnvironmentTable_t::const_iterator RPG_StringToEnvironmentTableIterator_t;
  typedef std::map<std::string, RPG_Character_Organization> RPG_StringToOrganizationTable_t;
  typedef RPG_StringToOrganizationTable_t::const_iterator RPG_StringToOrganizationTableIterator_t;

  static RPG_StringToMonsterMetaTypeTable_t   myStringToMonsterMetaTypeTable;
  static RPG_StringToMonsterSubTypeTable_t    myStringToMonsterSubTypeTable;
  static RPG_StringToNaturalWeaponTable_t     myStringToNaturalWeaponTable;
  static RPG_StringToMonsterAttackFormTable_t myStringToMonsterAttackFormTable;
  static RPG_StringToSizeTable_t              myStringToSizeTable;
  static RPG_StringToEnvironmentTable_t       myStringToEnvironmentTable;
  static RPG_StringToOrganizationTable_t      myStringToOrganizationTable;
};

#endif
