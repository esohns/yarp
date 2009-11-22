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

#include <rpg_chance_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Character_Monster_Common_Tools::RPG_StringToMonsterMetaTypeTable_t   RPG_Character_Monster_Common_Tools::myStringToMonsterMetaTypeTable;
RPG_Character_Monster_Common_Tools::RPG_StringToMonsterSubTypeTable_t    RPG_Character_Monster_Common_Tools::myStringToMonsterSubTypeTable;
RPG_Character_Monster_Common_Tools::RPG_StringToMonsterWeaponTable_t     RPG_Character_Monster_Common_Tools::myStringToMonsterWeaponTable;
RPG_Character_Monster_Common_Tools::RPG_StringToAttackFormTable_t RPG_Character_Monster_Common_Tools::myStringToAttackFormTable;
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

  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ANY"), MONSTERMETATYPE_ANY));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ABERRATION"), MONSTERMETATYPE_ABERRATION));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ANIMAL"), MONSTERMETATYPE_ANIMAL));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_CONSTRUCT"), MONSTERMETATYPE_CONSTRUCT));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_DRAGON"), MONSTERMETATYPE_DRAGON));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_ELEMENTAL"), MONSTERMETATYPE_ELEMENTAL));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_FEY"), MONSTERMETATYPE_FEY));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_GIANT"), MONSTERMETATYPE_GIANT));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_HUMANOID"), MONSTERMETATYPE_HUMANOID));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_MAGICAL_BEAST"), MONSTERMETATYPE_MAGICAL_BEAST));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_MONSTROUS_HUMANOID"), MONSTERMETATYPE_MONSTROUS_HUMANOID));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_OOZE"), MONSTERMETATYPE_OOZE));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_OUTSIDER"), MONSTERMETATYPE_OUTSIDER));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_PLANT"), MONSTERMETATYPE_PLANT));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_UNDEAD"), MONSTERMETATYPE_UNDEAD));
  myStringToMonsterMetaTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_VERMIN"), MONSTERMETATYPE_VERMIN));

  // clean table
  myStringToMonsterSubTypeTable.clear();

  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_ANY"), MONSTERSUBTYPE_ANY));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AIR"), MONSTERSUBTYPE_AIR));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_ANGEL"), MONSTERSUBTYPE_ANGEL));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AQUATIC"), MONSTERSUBTYPE_AQUATIC));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_ARCHON"), MONSTERSUBTYPE_ARCHON));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_AUGMENTED"), MONSTERSUBTYPE_AUGMENTED));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_CHAOTIC"), MONSTERSUBTYPE_CHAOTIC));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_COLD"), MONSTERSUBTYPE_COLD));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_EARTH"), MONSTERSUBTYPE_EARTH));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_EVIL"), MONSTERSUBTYPE_EVIL));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_EXTRAPLANAR"), MONSTERSUBTYPE_EXTRAPLANAR));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_FIRE"), MONSTERSUBTYPE_FIRE));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_GOBLINOID"), MONSTERSUBTYPE_GOBLINOID));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_GOOD"), MONSTERSUBTYPE_GOOD));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_INCORPOREAL"), MONSTERSUBTYPE_INCORPOREAL));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_LAWFUL"), MONSTERSUBTYPE_LAWFUL));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_NATIVE"), MONSTERSUBTYPE_NATIVE));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_REPTILIAN"), MONSTERSUBTYPE_REPTILIAN));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_SHAPECHANGER"), MONSTERSUBTYPE_SHAPECHANGER));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_SWARM"), MONSTERSUBTYPE_SWARM));
  myStringToMonsterSubTypeTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_WATER"), MONSTERSUBTYPE_WATER));

  // clean table
  myStringToMonsterWeaponTable.clear();

  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("INCORPOREAL_TOUCH"), INCORPOREAL_TOUCH));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_BITE"), NATURALWEAPON_BITE));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_CLAW_TALON"), NATURALWEAPON_CLAW_TALON));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_GORE"), NATURALWEAPON_GORE));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_SLAP_SLAM"), NATURALWEAPON_SLAP_SLAM));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_STING"), NATURALWEAPON_STING));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_TENTACLE"), NATURALWEAPON_TENTACLE));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_SPIT_ACID"), NATURALWEAPON_SPIT_ACID));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_WEB"), NATURALWEAPON_WEB));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_ROCK_STONE"), NATURALWEAPON_ROCK_STONE));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("STANDARDWEAPON_BLUDGEONING"), STANDARDWEAPON_BLUDGEONING));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("STANDARDWEAPON_PIERCING"), STANDARDWEAPON_PIERCING));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("STANDARDWEAPON_SLASHING"), STANDARDWEAPON_SLASHING));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RAY_ELECTRICITY"), RAY_ELECTRICITY));
  myStringToMonsterWeaponTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("RAY_LIGHT"), RAY_LIGHT));

  // clean table
  myStringToAttackFormTable.clear();

  myStringToAttackFormTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ATTACK_MELEE"), ATTACK_MELEE));
  myStringToAttackFormTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ATTACK_TOUCH"), ATTACK_TOUCH));
  myStringToAttackFormTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ATTACK_RANGED"), ATTACK_RANGED));

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

  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_ANY"), ENVIRONMENT_ANY));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_UNDERGROUND"), ENVIRONMENT_UNDERGROUND));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLAINS_TEMPERATE"), ENVIRONMENT_PLAINS_TEMPERATE));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLAINS_WARM"), ENVIRONMENT_PLAINS_WARM));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_FORESTS_TEMPERATE"), ENVIRONMENT_FORESTS_TEMPERATE));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_HILLS_TEMPERATE"), ENVIRONMENT_HILLS_TEMPERATE));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_HILLS_WARM"), ENVIRONMENT_HILLS_WARM));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_MOUNTAINS_TEMPERATE"), ENVIRONMENT_MOUNTAINS_TEMPERATE));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_DESERTS_WARM"), ENVIRONMENT_DESERTS_WARM));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_ANY_LAWFUL"), ENVIRONMENT_PLANE_ANY_LAWFUL));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_LAWFUL_GOOD"), ENVIRONMENT_PLANE_LAWFUL_GOOD));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_ANY_GOOD"), ENVIRONMENT_PLANE_ANY_GOOD));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_CHAOTIC_GOOD"), ENVIRONMENT_PLANE_CHAOTIC_GOOD));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_ANY_EVIL"), ENVIRONMENT_PLANE_ANY_EVIL));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_CHAOTIC_EVIL"), ENVIRONMENT_PLANE_CHAOTIC_EVIL));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_AIR"), ENVIRONMENT_PLANE_AIR));
  myStringToEnvironmentTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_PLANE_FIRE"), ENVIRONMENT_PLANE_FIRE));

  // clean table
  myStringToOrganizationTable.clear();

  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_ANY"), ORGANIZATION_ANY));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_SOLITARY"), ORGANIZATION_SOLITARY));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_PAIR"), ORGANIZATION_PAIR));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_BROOD"), ORGANIZATION_BROOD));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_CLUSTER"), ORGANIZATION_CLUSTER));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_CLUTCH"), ORGANIZATION_CLUTCH));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_PATCH"), ORGANIZATION_PATCH));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_GANG"), ORGANIZATION_GANG));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_TEAM"), ORGANIZATION_TEAM));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_SQUAD"), ORGANIZATION_SQUAD));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_PACK"), ORGANIZATION_PACK));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_COLONY"), ORGANIZATION_COLONY));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_FLOCK"), ORGANIZATION_FLOCK));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_TRIBE"), ORGANIZATION_TRIBE));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_SLAVER"), ORGANIZATION_SLAVER));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_BAND"), ORGANIZATION_BAND));
  myStringToOrganizationTable.insert(std::make_pair(ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_CLAN"), ORGANIZATION_CLAN));

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

const std::string RPG_Character_Monster_Common_Tools::sizeToString(const RPG_Character_Size& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::sizeToString"));

  for (RPG_StringToSizeTableIterator_t iterator = myStringToSizeTable.begin();
       iterator != myStringToSizeTable.end();
       iterator++)
  {
    if (iterator->second == size_in)
    {
      return iterator->first;
    } // end IF
  } // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid size: %d --> check implementation !, returning\n"),
             size_in));

  return ACE_TEXT_ALWAYS_CHAR("SIZE_INVALID");
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

const std::string RPG_Character_Monster_Common_Tools::monsterMetaTypeToString(const RPG_Character_MonsterMetaType& metaType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterMetaTypeToString"));

  for (RPG_StringToMonsterMetaTypeTableIterator_t iterator = myStringToMonsterMetaTypeTable.begin();
       iterator != myStringToMonsterMetaTypeTable.end();
       iterator++)
  {
    if (iterator->second == metaType_in)
    {
      return iterator->first;
    } // end IF
  } // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid monster (meta)type: %d --> check implementation !, returning\n"),
             metaType_in));

  return ACE_TEXT_ALWAYS_CHAR("MONSTERMETATYPE_INVALID");
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

const std::string RPG_Character_Monster_Common_Tools::monsterSubTypeToString(const RPG_Character_MonsterSubType& subType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterSubTypeToString"));

  for (RPG_StringToMonsterSubTypeTableIterator_t iterator = myStringToMonsterSubTypeTable.begin();
       iterator != myStringToMonsterSubTypeTable.end();
       iterator++)
  {
    if (iterator->second == subType_in)
    {
      return iterator->first;
    } // end IF
  } // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid monster (sub)type: %d --> check implementation !, returning\n"),
             subType_in));

  return ACE_TEXT_ALWAYS_CHAR("MONSTERSUBTYPE_INVALID");
}

const std::string RPG_Character_Monster_Common_Tools::monsterTypeToString(const RPG_Character_MonsterType& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterTypeToString"));

  std::string result = monsterMetaTypeToString(type_in.metaType);
  if (!type_in.subTypes.empty())
  {
    result += ACE_TEXT_ALWAYS_CHAR(" / (");
    for (RPG_Character_MonsterSubTypesIterator_t iterator = type_in.subTypes.begin();
        iterator != type_in.subTypes.end();
        iterator++)
    {
      result += monsterSubTypeToString(*iterator);
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end FOR
    result.erase(--(result.end()));
    result += ACE_TEXT_ALWAYS_CHAR(")");
  } // end IF

  return result;
}

const RPG_Character_MonsterWeapon RPG_Character_Monster_Common_Tools::stringToMonsterWeapon(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToMonsterWeapon"));

  RPG_StringToMonsterWeaponTableIterator_t iterator = myStringToMonsterWeaponTable.find(string_in);
  if (iterator == myStringToMonsterWeaponTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid monster weapon: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return MONSTERWEAPON_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Character_Monster_Common_Tools::monsterWeaponToString(const RPG_Character_MonsterWeapon& monsterWeapon_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterWeaponToString"));

  for (RPG_StringToMonsterWeaponTableIterator_t iterator = myStringToMonsterWeaponTable.begin();
       iterator != myStringToMonsterWeaponTable.end();
       iterator++)
  {
    if (iterator->second == monsterWeapon_in)
    {
      return iterator->first;
    } // end IF
  } // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid monster weapon: %d --> check implementation !, returning\n"),
                      monsterWeapon_in));

  return ACE_TEXT_ALWAYS_CHAR("MONSTERWEAPON_INVALID");
}

const RPG_Character_AttackForm RPG_Character_Monster_Common_Tools::stringToAttackForm(const std::string& string_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::stringToAttackForm"));

  RPG_StringToAttackFormTableIterator_t iterator = myStringToAttackFormTable.find(string_in);
  if (iterator == myStringToAttackFormTable.end())
  {
    // debug info
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid attack form: \"%s\" --> check implementation !, returning\n"),
               string_in.c_str()));

    return ATTACK_INVALID;
  } // end IF

  return iterator->second;
}

const std::string RPG_Character_Monster_Common_Tools::attackFormToString(const RPG_Character_AttackForm& attackForm_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::attackFormToString"));

  for (RPG_StringToAttackFormTableIterator_t iterator = myStringToAttackFormTable.begin();
       iterator != myStringToAttackFormTable.end();
       iterator++)
  {
    if (iterator->second == attackForm_in)
    {
      return iterator->first;
    } // end IF
  } // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid attack form: %d --> check implementation !, returning\n"),
             attackForm_in));

  return ACE_TEXT_ALWAYS_CHAR("ATTACK_INVALID");
}

const std::string RPG_Character_Monster_Common_Tools::monsterAttackActionToString(const RPG_Character_MonsterAttackAction& attackAction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterAttackActionToString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR("weapon: ");
  result += monsterWeaponToString(attackAction_in.monsterWeapon);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attackBonus: ");
  std::stringstream str;
  str << attackAction_in.attackBonus;
  result += str.str();
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attackForm: ");
  result += attackFormToString(attackAction_in.attackForm);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("damage: ");
  result += RPG_Chance_Dice_Common_Tools::rollToString(attackAction_in.damage);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("numAttacksPerRound: ");
  str << attackAction_in.numAttacksPerRound;
  result += str.str();
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::monsterAttackToString(const RPG_Character_MonsterAttack& monsterAttack_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterAttackToString"));

  std::string result;

  std::stringstream str;
  result += ACE_TEXT_ALWAYS_CHAR("baseAttackBonus: ");
  str << monsterAttack_in.baseAttackBonus;
  result += str.str();
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("grappleBonus: ");
  str << monsterAttack_in.grappleBonus;
  result += str.str();
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("Action(s):\n----------\n");
  for (RPG_Character_MonsterAttackActionsIterator_t iterator = monsterAttack_in.attackActions.begin();
       iterator != monsterAttack_in.attackActions.end();
       iterator++)
  {
    result += monsterAttackActionToString(*iterator);
  } // end FOR

  return result;
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

const std::string RPG_Character_Monster_Common_Tools::environmentToString(const RPG_Character_Environment& environment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::environmentToString"));

  for (RPG_StringToEnvironmentTableIterator_t iterator = myStringToEnvironmentTable.begin();
       iterator != myStringToEnvironmentTable.end();
       iterator++)
  {
    if (iterator->second == environment_in)
    {
      return iterator->first;
    } // end IF
  }; // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid environment: %d --> check implementation !, returning\n"),
             environment_in));

  return ACE_TEXT_ALWAYS_CHAR("ENVIRONMENT_INVALID");
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

const std::string RPG_Character_Monster_Common_Tools::organizationToString(const RPG_Character_Organization& organization_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::organizationToString"));

  for (RPG_StringToOrganizationTableIterator_t iterator = myStringToOrganizationTable.begin();
       iterator != myStringToOrganizationTable.end();
       iterator++)
  {
    if (iterator->second == organization_in)
    {
      return iterator->first;
    } // end IF
  }; // end FOR

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid organization: %d --> check implementation !, returning\n"),
             organization_in));

  return ACE_TEXT_ALWAYS_CHAR("ORGANIZATION_INVALID");
}

const std::string RPG_Character_Monster_Common_Tools::organizationsToString(const RPG_Character_Organizations_t& organizations_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::organizationsToString"));

  std::string result;

  for (RPG_Character_OrganizationsIterator_t iterator = organizations_in.begin();
       iterator != organizations_in.end();
       iterator++)
  {
    result += organizationToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  }; // end FOR

  // sanity check
  if (!organizations_in.empty())
    result.erase(--(result.end()));

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::monsterAdvancementToString(const RPG_Character_MonsterAdvancement_t& advancement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterAdvancementToString"));

  std::string result;
  for (RPG_Character_MonsterAdvancementIterator_t iterator = advancement_in.begin();
       iterator != advancement_in.end();
       iterator++)
  {
    result += sizeToString((*iterator).first);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    result += RPG_Chance_Dice_Common_Tools::rangeToString((*iterator).second);
    result += ACE_TEXT_ALWAYS_CHAR(" HD\n");
  }; // end FOR

  return result;
}
