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
#include "rpg_monster_common_tools.h"

#include "rpg_monster_dictionary.h"

#include <rpg_combat_common_tools.h>
#include <rpg_character_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Monster_NaturalWeaponToStringTable_t RPG_Monster_NaturalWeaponHelper::myRPG_Monster_NaturalWeaponToStringTable;
RPG_Monster_OrganizationToStringTable_t RPG_Monster_OrganizationHelper::myRPG_Monster_OrganizationToStringTable;

void
RPG_Monster_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::initStringConversionTables"));

  RPG_Monster_NaturalWeaponHelper::init();
  RPG_Monster_OrganizationHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Monster_Common_Tools: initialized string conversion tables...\n")));
}

const std::string
RPG_Monster_Common_Tools::weaponTypeToString(const RPG_Monster_WeaponTypeUnion& weaponType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::weaponTypeToString"));

  std::string result;

  switch (weaponType_in.discriminator)
  {
    case RPG_Monster_WeaponTypeUnion::NATURALWEAPON:
    {
      return RPG_Monster_NaturalWeaponHelper::RPG_Monster_NaturalWeaponToString(weaponType_in.naturalweapon);
    }
    case RPG_Monster_WeaponTypeUnion::ABILITY:
    {
      return RPG_Character_AbilityHelper::RPG_Character_AbilityToString(weaponType_in.ability);
    }
    case RPG_Monster_WeaponTypeUnion::WEAPONTYPE:
    {
      return RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weaponType_in.weapontype);
    }
    case RPG_Monster_WeaponTypeUnion::SPECIALATTACK:
    {
      return RPG_Combat_SpecialAttackHelper::RPG_Combat_SpecialAttackToString(weaponType_in.specialattack);
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid weapon type: %d, aborting\n"),
                 weaponType_in.discriminator));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string
RPG_Monster_Common_Tools::attackActionToString(const RPG_Monster_AttackAction& attackAction_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::attackActionToString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR("weapon: ");
  result += RPG_Monster_Common_Tools::weaponTypeToString(attackAction_in.weapon);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attackBonus(es): ");

  std::ostringstream converter;
  for (std::vector<char>::const_iterator iterator = attackAction_in.attackBonuses.begin();
       iterator != attackAction_in.attackBonuses.end();
       iterator++)
  {
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << ACE_static_cast(int, *iterator);
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("/");
  } // end FOR
  if (!attackAction_in.attackBonuses.empty())
  {
    result.erase(--(result.end()));
  } // end IF

  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attackForm: ");
  result += RPG_Combat_Common_Tools::attackFormsToString(attackAction_in.attackForms);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += RPG_Combat_Common_Tools::damageToString(attackAction_in.damage);
  result += ACE_TEXT_ALWAYS_CHAR("numAttacksPerRound: ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(int, attackAction_in.numAttacksPerRound);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string
RPG_Monster_Common_Tools::attackToString(const RPG_Monster_Attack& attack_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::attackToString"));

  std::string result;
  std::ostringstream converter;

  result += ACE_TEXT_ALWAYS_CHAR("baseAttackBonus: ");
  converter << ACE_static_cast(int, attack_in.baseAttackBonus);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("grappleBonus: ");
  converter.clear();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << ACE_static_cast(int, attack_in.grappleBonus);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("Standard Attack Action(s):\n-----------------------\n");
  int i = 1;
  for (std::vector<RPG_Monster_AttackAction>::const_iterator iterator = attack_in.standardAttackActions.begin();
       iterator != attack_in.standardAttackActions.end();
       iterator++, i++)
  {
    result += ACE_TEXT_ALWAYS_CHAR("action #");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << i;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("\n");
    result += attackActionToString(*iterator);
  } // end FOR
  result += ACE_TEXT_ALWAYS_CHAR("Full Attack Action(s):\n---------------\n");
  i = 1;
  for (std::vector<RPG_Monster_AttackAction>::const_iterator iterator = attack_in.fullAttackActions.begin();
       iterator != attack_in.fullAttackActions.end();
       iterator++, i++)
  {
    result += ACE_TEXT_ALWAYS_CHAR("action #");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << i;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("\n");
    result += attackActionToString(*iterator);
  } // end FOR

  return result;
}

const std::string
RPG_Monster_Common_Tools::organizationsToString(const RPG_Monster_OrganizationSet_t& organizations_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::organizationsToString"));

  std::string result;
  for (RPG_Monster_OrganizationSetIterator_t iterator = organizations_in.begin();
       iterator != organizations_in.end();
       iterator++)
  {
    result += RPG_Monster_OrganizationHelper::RPG_Monster_OrganizationToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  } // end FOR

  if (!result.empty())
  {
    result.erase(--(result.end()));
  } // end IF

  return result;
}

const std::string
RPG_Monster_Common_Tools::organizationsToString(const RPG_Monster_Organizations_t& organizations_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::organizationsToString"));

  std::string result;
  std::string range_string;
  for (RPG_Monster_OrganizationsIterator_t iterator = organizations_in.begin();
       iterator != organizations_in.end();
       iterator++)
  {
    result += RPG_Monster_OrganizationHelper::RPG_Monster_OrganizationToString((*iterator).type);
    range_string = RPG_Dice_Common_Tools::rangeToString((*iterator).range);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    result += range_string;
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  return result;
}

const std::string
RPG_Monster_Common_Tools::advancementToString(const RPG_Monster_Advancement_t& advancement_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::advancementToString"));

  std::string result;
  for (RPG_Monster_AdvancementIterator_t iterator = advancement_in.begin();
       iterator != advancement_in.end();
       iterator++)
  {
    result += RPG_Common_SizeHelper::RPG_Common_SizeToString((*iterator).size);
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    result += RPG_Dice_Common_Tools::rangeToString((*iterator).range);
    result += ACE_TEXT_ALWAYS_CHAR(" HD\n");
  } // end FOR

  return result;
}

const RPG_Common_PhysicalDamageList_t
RPG_Monster_Common_Tools::naturalWeaponToPhysicalDamageType(const RPG_Monster_NaturalWeapon& naturalWeapon_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::naturalWeaponToPhysicalDamageType"));

  RPG_Common_PhysicalDamageList_t result;

  switch (naturalWeapon_in)
  {
    case NATURALWEAPON_BITE:
    case NATURALWEAPON_GORE:
    case NATURALWEAPON_STING:
    {
      result.insert(PHYSICALDAMAGE_PIERCING);

      break;
    }
    case NATURALWEAPON_CLAW_TALON:
    case NATURALWEAPON_PINCER:
    {
//       result.push_back(PHYSICALDAMAGE_PIERCING);
      result.insert(PHYSICALDAMAGE_SLASHING);

      break;
    }
    case NATURALWEAPON_BLAST:
    case NATURALWEAPON_CONSTRICT:
    case NATURALWEAPON_HOOF:
    case NATURALWEAPON_ROCK_STONE:
    case NATURALWEAPON_SLAP_SLAM:
    case NATURALWEAPON_TAIL:
    case NATURALWEAPON_TENTACLE:
    {
      result.insert(PHYSICALDAMAGE_BLUDGEONING);

      break;
    }
    case NATURALWEAPON_SPIT:
    case NATURALWEAPON_WEB:
    {
      result.insert(PHYSICALDAMAGE_NONE);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid natural weapon: \"%s\", aborting\n"),
                 RPG_Monster_NaturalWeaponHelper::RPG_Monster_NaturalWeaponToString(naturalWeapon_in).c_str()));

      break;
    }
  } // end SWITCH

  return result;
}

void
RPG_Monster_Common_Tools::generateRandomEncounter(const unsigned int& numDifferentMonsterTypes_in,
                                                  const unsigned int& numMonsters_in,
                                                  const RPG_Character_Alignment& alignment_in,
                                                  const RPG_Common_Environment& environment_in,
                                                  const RPG_Monster_OrganizationSet_t& organizations_in,
                                                  RPG_Monster_Encounter_t& encounter_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::generateRandomEncounter"));

  // init result
  encounter_out.clear();

  // sanity check(s)
  ACE_ASSERT(numDifferentMonsterTypes_in <= RPG_MONSTER_DICTIONARY_SINGLETON::instance()->numEntries());
  if (numMonsters_in)
  {
    ACE_ASSERT(numDifferentMonsterTypes_in <= numMonsters_in);
  } // end IF

  RPG_Monster_List_t list;
  RPG_MONSTER_DICTIONARY_SINGLETON::instance()->find(alignment_in,
                                                     environment_in,
                                                     organizations_in,
                                                     list);
  if (list.empty())
  {
    // nothing found in database...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("found no appropriate monster types (alignment: \"%s\", environment \"%s\", organizations: \"%s\" in dictionary (%u entries), returning\n"),
               RPG_Character_Common_Tools::alignmentToString(alignment_in).c_str(),
               RPG_Common_Tools::environmentToString(environment_in).c_str(),
               RPG_Monster_Common_Tools::organizationsToString(organizations_in).c_str(),
               RPG_MONSTER_DICTIONARY_SINGLETON::instance()->numEntries()));

    return;
  } // end IF

  RPG_Dice_RollResult_t result;
  RPG_Monster_Properties properties;
  RPG_Monster_OrganizationSet_t possible_organizations;
  for (unsigned int i = 0;
       i < numDifferentMonsterTypes_in;
       i++)
  {
    // step2a: choose new random foe (from the set of possibilities)
    RPG_Monster_EncounterConstIterator_t iterator;
    int choiceType = 0;
    do
    {
      result.clear();
      RPG_Dice::generateRandomNumbers(list.size(),
                                      1,
                                      result);
      choiceType = result.front() - 1; // list index
      // already used this type ?
      iterator = encounter_out.find(list[choiceType]);
    } while (iterator != encounter_out.end());

    // step2b: compute number of foes
    // step2ba: ...choose a (random) organization from the possible ones
    possible_organizations.clear();
    properties = RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getProperties(list[choiceType]);
    for (RPG_Monster_OrganizationsIterator_t iterator2 = properties.organizations.begin();
         iterator2 != properties.organizations.end();
         iterator2++)
    {
      possible_organizations.insert((*iterator2).type);
    } // end FOR
    if (organizations_in.find(ORGANIZATION_ANY) == organizations_in.end())
    {
      // compute intersection
      std::vector<RPG_Monster_Organization> intersection;
      std::set_intersection(organizations_in.begin(),
                            organizations_in.end(),
                            possible_organizations.begin(),
                            possible_organizations.end(),
                            intersection.begin());
      possible_organizations.clear();
      for (std::vector<RPG_Monster_Organization>::const_iterator iterator3 = intersection.begin();
           iterator3 != intersection.end();
           iterator3++)
      {
        possible_organizations.insert(*iterator3);
      } // end FOR
    } // end IF

    result.clear();
    RPG_Dice::generateRandomNumbers(possible_organizations.size(),
                                    1,
                                    result);
    int choiceOrganization = result.front() - 1; // list index
    RPG_Monster_OrganizationSetIterator_t iterator2 = possible_organizations.begin();
    std::advance(iterator2, choiceOrganization);
    RPG_Monster_OrganizationsIterator_t iterator3 = properties.organizations.begin();
    while ((*iterator3).type != *iterator2)
      iterator3++;
    ACE_ASSERT(iterator3 != properties.organizations.end());
    RPG_Dice_Roll roll;
    RPG_Monster_Common_Tools::organizationStepToRoll(*iterator3,
                                                     roll);
    result.clear();
    RPG_Dice::simulateRoll(roll,
                           1,
                           result);

    encounter_out.insert(std::make_pair(list[choiceType], result.front()));
  } // end FOR

  // reduce/increase total number of foes to adjust the result
  // *NOTE*: this means, however, that the final result doesn't necessarily
  // correspond to the numbers implied by the requested types of organizations...
  unsigned int numCurrentFoes = 0;
  if (numMonsters_in)
  {
    for (RPG_Monster_EncounterConstIterator_t iterator = encounter_out.begin();
         iterator != encounter_out.end();
         iterator++)
    {
      numCurrentFoes += (*iterator).second;
    } // end FOR
    RPG_Monster_EncounterIterator_t iterator;
    int diff = ::abs(numCurrentFoes - numMonsters_in);
    while (diff)
    {
      iterator = encounter_out.begin();
      result.clear();
      RPG_Dice::generateRandomNumbers(encounter_out.size(),
                                      1,
                                      result);
      std::advance(iterator, result.front() - 1);
      if ((*iterator).second) // don't go below 1...
      {
        (*iterator).second += ((numCurrentFoes > numMonsters_in) ? -1 : 1);
        diff--;
      } // end IF
    } // end WHILE
  } // end IF

  numCurrentFoes = 0;
  int index = 1;
  for (RPG_Monster_EncounterConstIterator_t iterator = encounter_out.begin();
       iterator != encounter_out.end();
       iterator++, index++)
  {
    numCurrentFoes += (*iterator).second;

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("group #%d: %d %s\n"),
               index,
               (*iterator).second,
               (*iterator).first.c_str()));
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("generated %d foes...\n"),
             numCurrentFoes));
}

void
RPG_Monster_Common_Tools::organizationStepToRoll(const RPG_Monster_OrganizationStep& organizationStep_in,
                                                 RPG_Dice_Roll& roll_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Common_Tools::organizationStepToRoll"));

  // init result
  roll_out.numDice = 0;
  roll_out.typeDice = D_0;
  roll_out.modifier = 0;

  switch (organizationStep_in.type)
  {
    case ORGANIZATION_SOLITARY:
    {
      roll_out.modifier = 1;

      break;
    }
    case ORGANIZATION_PAIR:
    {
      roll_out.modifier = 2;

      break;
    }
    case ORGANIZATION_BAND:
    case ORGANIZATION_BROOD:
    case ORGANIZATION_COLONY:
    case ORGANIZATION_CLAN:
    case ORGANIZATION_CLUSTER:
    case ORGANIZATION_CLUTCH:
    case ORGANIZATION_COMPANY:
    case ORGANIZATION_CROWD:
    case ORGANIZATION_FLIGHT:
    case ORGANIZATION_FLOCK:
    case ORGANIZATION_GANG:
    case ORGANIZATION_GROUP:
    case ORGANIZATION_MOB:
    case ORGANIZATION_PACK:
    case ORGANIZATION_PATCH:
    case ORGANIZATION_PRIDE:
    case ORGANIZATION_SQUAD:
    case ORGANIZATION_SWARM:
    case ORGANIZATION_TEAM:
    case ORGANIZATION_TRIBE:
    case ORGANIZATION_TROOP:
    case ORGANIZATION_TROUPE:
    {
      RPG_Dice::rangeToRoll(organizationStep_in.range,
                            roll_out);

      break;
    }
    case ORGANIZATION_ANY:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid organization \"%s\", aborting\n"),
                 RPG_Monster_OrganizationHelper::RPG_Monster_OrganizationToString(organizationStep_in.type).c_str()));

      break;
    }
  } // end SWITCH
}
