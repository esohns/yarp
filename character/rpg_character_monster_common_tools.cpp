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

const std::string RPG_Character_Monster_Common_Tools::monsterTypeToString(const RPG_Character_MonsterType& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterTypeToString"));

  std::string result = RPG_Character_MonsterMetaTypeHelper::RPG_Character_MonsterMetaTypeToString(type_in.metaType);
  if (!type_in.subTypes.empty())
  {
    result += ACE_TEXT_ALWAYS_CHAR(" / (");
    for (std::vector<RPG_Character_MonsterSubType>::const_iterator iterator = type_in.subTypes.begin();
        iterator != type_in.subTypes.end();
        iterator++)
    {
      result += RPG_Character_MonsterSubTypeHelper::RPG_Character_MonsterSubTypeToString(*iterator);
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end FOR
    result.erase(--(result.end()));
    result += ACE_TEXT_ALWAYS_CHAR(")");
  } // end IF

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::monsterAttackActionToString(const RPG_Character_MonsterAttackAction& attackAction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterAttackActionToString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR("weapon: ");
  result += RPG_Character_MonsterWeaponHelper::RPG_Character_MonsterWeaponToString(attackAction_in.monsterWeapon);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attackBonus: ");
  std::stringstream str;
  str << attackAction_in.attackBonus;
  result += str.str();
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attackForm: ");
  result += RPG_Character_AttackFormHelper::RPG_Character_AttackFormToString(attackAction_in.attackForm);
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
  for (std::vector<RPG_Character_MonsterAttackAction>::const_iterator iterator = monsterAttack_in.attackActions.begin();
       iterator != monsterAttack_in.attackActions.end();
       iterator++)
  {
    result += monsterAttackActionToString(*iterator);
  } // end FOR

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::organizationsToString(const RPG_Character_Organizations_t& organizations_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::organizationsToString"));

  std::string result;

  for (RPG_Character_OrganizationsIterator_t iterator = organizations_in.begin();
       iterator != organizations_in.end();
       iterator++)
  {
    result += RPG_Character_OrganizationHelper::RPG_Character_OrganizationToString(*iterator);
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
    result += RPG_Character_SizeHelper::RPG_Character_SizeToString((*iterator).first);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    result += RPG_Chance_Dice_Common_Tools::rangeToString((*iterator).second);
    result += ACE_TEXT_ALWAYS_CHAR(" HD\n");
  }; // end FOR

  return result;
}
