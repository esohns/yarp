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
#include "rpg_character_monster_common_tools.h"

#include <ace/Log_Msg.h>

// init statics
RPG_Character_Monster_Common_Tools::RPG_StringToMonsterMetaTypeTable_t   RPG_Character_Monster_Common_Tools::myStringToMonsterMetaTypeTable;
RPG_Character_Monster_Common_Tools::RPG_StringToMonsterSubTypeTable_t    RPG_Character_Monster_Common_Tools::myStringToMonsterSubTypeTable;
RPG_Character_Monster_Common_Tools::RPG_StringToNaturalWeaponTable_t     RPG_Character_Monster_Common_Tools::myStringToNaturalWeaponTable;
RPG_Character_Monster_Common_Tools::RPG_StringToMonsterAttackFormTable_t RPG_Character_Monster_Common_Tools::myStringToMonsterAttackFormTable;
RPG_Character_Monster_Common_Tools::RPG_StringToSizeTable_t              RPG_Character_Monster_Common_Tools::myStringToSizeTable;
RPG_Character_Monster_Common_Tools::RPG_StringToEnvironmentTable_t       RPG_Character_Monster_Common_Tools::myStringToEnvironmentTable;
RPG_Character_Monster_Common_Tools::RPG_StringToOrganizationTable_t      RPG_Character_Monster_Common_Tools::myStringToOrganizationTable;

void RPG_Character_Monster_Common_Tools::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::init"));

  initStringConversionTables();
}

void RPG_Character_Monster_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::initStringConversionTables"));

  // clean table
  myStringToMonsterMetaTypeTable.clear();

  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ABERRATION"), MONSTERMETATYPE_ABERRATION));

  // clean table
  myStringToMonsterSubTypeTable.clear();

  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AQUATIC"), MONSTERSUBTYPE_AQUATIC));

  // clean table
  myStringToNaturalWeaponTable.clear();

  myStringToNaturalWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_TENTACLE"), NATURALWEAPON_TENTACLE));

  // clean table
  myStringToMonsterAttackFormTable.clear();

  myStringToMonsterAttackFormTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ATTACK_MELEE"), ATTACK_MELEE));
  myStringToMonsterAttackFormTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ATTACK_RANGED"), ATTACK_RANGED));

  // clean table
  myStringToSizeTable.clear();

  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_FINE"), SIZE_FINE));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_DIMINUTIVE"), SIZE_DIMINUTIVE));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_TINY"), SIZE_TINY));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_SMALL"), SIZE_SMALL));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_MEDIUM"), SIZE_MEDIUM));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_LARGE"), SIZE_LARGE));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_HUGE"), SIZE_HUGE));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_GARGANTUAN"), SIZE_GARGANTUAN));
  myStringToSizeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("SIZE_COLOSSAL"), SIZE_COLOSSAL));

  // clean table
  myStringToEnvironmentTable.clear();

  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_UNDERGROUND"), ENVIRONMENT_UNDERGROUND));

  // clean table
  myStringToOrganizationTable.clear();

  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_SOLITARY"), ORGANIZATION_SOLITARY));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_BROOD"), ORGANIZATION_BROOD));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_SLAVER_BROOD"), ORGANIZATION_SLAVER_BROOD));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Character_Monster_Common_Tools: initialized string conversion tables...\n")));
}

const RPG_Character_Size RPG_Character_Monster_Common_Tools::stringToSize(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToSize"));

  RPG_StringToSizeTableIterator_t iterator = myStringToSizeTable.find(string_in);
  if (iterator == myStringToSizeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid size: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return SIZE_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_MonsterMetaType RPG_Character_Monster_Common_Tools::stringToMonsterMetaType(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToMonsterMetaType"));

  RPG_StringToMonsterMetaTypeTableIterator_t iterator = myStringToMonsterMetaTypeTable.find(string_in);
  if (iterator == myStringToMonsterMetaTypeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid monster metatype: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return MONSTERMETATYPE_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_MonsterSubType RPG_Character_Monster_Common_Tools::stringToMonsterSubType(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToMonsterSubType"));

  RPG_StringToMonsterSubTypeTableIterator_t iterator = myStringToMonsterSubTypeTable.find(string_in);
  if (iterator == myStringToMonsterSubTypeTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid monster subtype: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return MONSTERSUBTYPE_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_NaturalWeapon RPG_Character_Monster_Common_Tools::stringToNaturalWeapon(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToNaturalWeapon"));

  RPG_StringToNaturalWeaponTableIterator_t iterator = myStringToNaturalWeaponTable.find(string_in);
  if (iterator == myStringToNaturalWeaponTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid natural weapon: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return NATURALWEAPON_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_MonsterAttackForm RPG_Character_Monster_Common_Tools::stringToMonsterAttackForm(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToMonsterAttackForm"));

  RPG_StringToMonsterAttackFormTableIterator_t iterator = myStringToMonsterAttackFormTable.find(string_in);
  if (iterator == myStringToMonsterAttackFormTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid monster attack form: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ATTACK_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_Environment RPG_Character_Monster_Common_Tools::stringToEnvironment(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToEnvironment"));

  RPG_StringToEnvironmentTableIterator_t iterator = myStringToEnvironmentTable.find(string_in);
  if (iterator == myStringToEnvironmentTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid environment: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ENVIRONMENT_INVALID;
  } // end IF

  return iterator->second;
}

const RPG_Character_Organization RPG_Character_Monster_Common_Tools::stringToOrganization(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToOrganization"));

  RPG_StringToOrganizationTableIterator_t iterator = myStringToOrganizationTable.find(string_in);
  if (iterator == myStringToOrganizationTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid organization: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ORGANIZATION_INVALID;
  } // end IF

  return iterator->second;
}
