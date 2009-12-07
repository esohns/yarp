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
  result += monsterAttackFormsToString(attackAction_in.attackForms);
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

const std::string RPG_Character_Monster_Common_Tools::monsterAttackFormsToString(const RPG_Character_AttackForms_t& attackForms_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterAttackFormsToString"));

  std::string result;

  for (RPG_Character_AttackFormsIterator_t iterator = attackForms_in.begin();
       iterator != attackForms_in.end();
       iterator++)
  {
    result += RPG_Character_AttackFormHelper::RPG_Character_AttackFormToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  } // end FOR

  if (!result.empty())
  {
    result.erase(--(result.end()));
  } // end IF

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

const std::string RPG_Character_Monster_Common_Tools::environmentToString(const RPG_Character_Environment& environment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::environmentToString"));

  std::string result;

  result += RPG_Character_TerrainHelper::RPG_Character_TerrainToString(environment_in.terrain);
  if (environment_in.climate != CLIMATE_NONE)
  {
    result += ACE_TEXT_ALWAYS_CHAR("|");
    result += RPG_Character_ClimateHelper::RPG_Character_ClimateToString(environment_in.climate);
  } // end IF

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::organizationsToString(const RPG_Character_OrganizationList_t& organizations_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::organizationsToString"));

  std::string result;

  std::string range_string;
  for (RPG_Character_OrganizationListIterator_t iterator = organizations_in.begin();
       iterator != organizations_in.end();
       iterator++)
  {
    result += RPG_Character_OrganizationHelper::RPG_Character_OrganizationToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  }; // end FOR

  if (!result.empty())
  {
    result.erase(--(result.end()));
  } // end IF

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::organizationsToString(const RPG_Character_Organizations_t& organizations_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::organizationsToString"));

  std::string result;

  std::string range_string;
  for (RPG_Character_OrganizationsIterator_t iterator = organizations_in.begin();
       iterator != organizations_in.end();
       iterator++)
  {
    result += RPG_Character_OrganizationHelper::RPG_Character_OrganizationToString((*iterator).type);
    range_string = RPG_Chance_Dice_Common_Tools::rangeToString((*iterator).range);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    result += range_string;
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  }; // end FOR

  return result;
}

const std::string RPG_Character_Monster_Common_Tools::monsterAdvancementsToString(const RPG_Character_MonsterAdvancement_t& advancement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::monsterAdvancementsToString"));

  std::string result;
  for (RPG_Character_MonsterAdvancementIterator_t iterator = advancement_in.begin();
       iterator != advancement_in.end();
       iterator++)
  {
    result += RPG_Character_SizeHelper::RPG_Character_SizeToString((*iterator).size);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    result += RPG_Chance_Dice_Common_Tools::rangeToString((*iterator).range);
    result += ACE_TEXT_ALWAYS_CHAR(" HD\n");
  }; // end FOR

  return result;
}

const RPG_Character_Plane RPG_Character_Monster_Common_Tools::terrainToPlane(const RPG_Character_Terrain& terrain_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster_Common_Tools::terrainToPlane"));

  switch (terrain_in)
  {
    case TERRAIN_UNDERGROUND:
    case TERRAIN_PLAINS:
    case TERRAIN_FORESTS:
    case TERRAIN_HILLS:
    case TERRAIN_MOUNTAINS:
    case TERRAIN_DESERTS:
    case TERRAIN_MATERIALPLANE_ANY:
    {
      return PLANE_MATERIAL;
    }
    case TERRAIN_TRANSITIVEPLANE_ASTRAL:
    case TERRAIN_TRANSITIVEPLANE_ETHERAL:
    case TERRAIN_TRANSITIVEPLANE_SHADOW:
    case TERRAIN_TRANSITIVEPLANE_ANY:
    {
      return PLANE_TRANSITIVE;
    }
    case TERRAIN_INNERPLANE_AIR:
    case TERRAIN_INNERPLANE_EARTH:
    case TERRAIN_INNERPLANE_FIRE:
    case TERRAIN_INNERPLANE_WATER:
    case TERRAIN_INNERPLANE_POSITIVE:
    case TERRAIN_INNERPLANE_NEGATIVE:
    case TERRAIN_INNERPLANE_ANY:
    {
      return PLANE_INNER;
    }
    case TERRAIN_OUTERPLANE_LAWFUL_ANY:
    case TERRAIN_OUTERPLANE_CHAOTIC_ANY:
    case TERRAIN_OUTERPLANE_GOOD_ANY:
    case TERRAIN_OUTERPLANE_EVIL_ANY:
    case TERRAIN_OUTERPLANE_LAWFUL_GOOD:
    case TERRAIN_OUTERPLANE_LAWFUL_EVIL:
    case TERRAIN_OUTERPLANE_CHAOTIC_GOOD:
    case TERRAIN_OUTERPLANE_CHAOTIC_EVIL:
    case TERRAIN_OUTERPLANE_NEUTRAL:
    case TERRAIN_OUTERPLANE_MILD_ANY:
    case TERRAIN_OUTERPLANE_STRONG_ANY:
    case TERRAIN_OUTERPLANE_ANY:
    {
      return PLANE_OUTER;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid terrain: \"%s\" --> check implementation !, aborting\n"),
                 RPG_Character_TerrainHelper::RPG_Character_TerrainToString(terrain_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_CHARACTER_PLANE_INVALID;
}
