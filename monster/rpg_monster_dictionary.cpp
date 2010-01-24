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
#include "rpg_monster_dictionary.h"

#include "rpg_monster_XML_parser.h"
#include "rpg_monster_common_tools.h"

#include <rpg_common_XML_parser.h>

#include <rpg_character_common_tools.h>
#include <rpg_character_skills_common_tools.h>
#include <rpg_character_XML_parser.h>

#include <rpg_magic_XML_parser.h>

#include <rpg_combat_XML_parser.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>
#include <rpg_dice_XML_parser.h>

#include <ace/Log_Msg.h>

#include <iostream>
#include <algorithm>
#include <sstream>

RPG_Monster_Dictionary::RPG_Monster_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::RPG_Monster_Dictionary"));

}

RPG_Monster_Dictionary::~RPG_Monster_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::~RPG_Monster_Dictionary"));

}

void RPG_Monster_Dictionary::initMonsterDictionary(const std::string& filename_in,
                                                   const bool& validateXML_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::initMonsterDictionary"));

  // Construct the parser.
  //
  ::xml_schema::string_pimpl                     string_p;
  RPG_Character_Size_Type                        size_p;

  RPG_Monster_MetaType_Type                      metaType_p;
  RPG_Monster_SubType_Type                       subType_p;
  RPG_Monster_Type_Type                          type_p;
  type_p.parsers(metaType_p,
                 subType_p);

  ::xml_schema::unsigned_int_pimpl               unsigned_int_p;
  RPG_Dice_DieType_Type                          dieType_p;
  ::xml_schema::integer_pimpl                    int_p;
  RPG_Dice_Roll_Type                             roll_p;
  roll_p.parsers(unsigned_int_p,
                 dieType_p,
                 int_p);
//   int_pimpl                               initiative_p;
//   unsigned_int_pimpl                      speed_p;
  ::xml_schema::byte_pimpl                       byte_p;
  RPG_Monster_NaturalArmorClass_Type             naturalArmorClass_p;
  naturalArmorClass_p.parsers(byte_p,
                              byte_p,
                              byte_p);

  RPG_Magic_AbilityClass_Type                    abilityClass_p;
  RPG_Monster_SpecialPropertyTypeUnion_Type      specialPropertyTypeUnion_p;
  RPG_Combat_ActionType_Type                     actionType_p;
  RPG_Common_Usage_Type                          usage_p;
  RPG_Monster_SpecialBaseProperties_Type         specialBaseProperties_p;
  specialBaseProperties_p.parsers(abilityClass_p,
                                  specialPropertyTypeUnion_p,
                                  actionType_p,
                                  usage_p);

  RPG_Monster_WeaponTypeUnion_Type               weaponUnion_p;
  RPG_Monster_SpecialDefensePreCondition_Type    specialDefensePreCondition_p;
  specialDefensePreCondition_p.parsers(weaponUnion_p);

  RPG_Combat_DamageTypeUnion_Type                damageType_p;
  ::xml_schema::unsigned_short_pimpl             unsigned_short_p;
  RPG_Combat_DamageDuration_Type                 duration_p;
  duration_p.parsers(roll_p,
                     unsigned_short_p,
                     unsigned_short_p);
  RPG_Combat_DamageCounterMeasureType_Type       counterMeasureType_p;
  RPG_Combat_CheckTypeUnion_Type                 checkTypeUnion_p;
  RPG_Common_Attribute_Type                      attribute_p;
  ::xml_schema::unsigned_byte_pimpl              unsigned_byte_p;
  RPG_Combat_Check_Type                          check_p;
  check_p.parsers(checkTypeUnion_p,
                  attribute_p,
                  unsigned_byte_p);
  RPG_Magic_Spell_Type                           spell_p;
  RPG_Common_Duration_Type                       commonDuration_p;
  commonDuration_p.parsers(unsigned_int_p,
                           unsigned_int_p,
                           unsigned_int_p);
  RPG_Combat_DamageReductionType_Type            reduction_p;
  RPG_Combat_DamageCounterMeasure_Type           counterMeasure_p;
  counterMeasure_p.parsers(counterMeasureType_p,
                           check_p,
                           spell_p,
                           commonDuration_p,
                           reduction_p);

  RPG_Combat_OtherDamageType_Type                otherDamageType_p;
  RPG_Combat_OtherDamage_Type                    others_p;
  others_p.parsers(otherDamageType_p,
                   byte_p,
                   counterMeasure_p);
//   RPG_Common_Attribute_Type                      attribute_p;
  RPG_Common_EffectType_Type                     effectType_p;
  RPG_Combat_DamageElement_Type                  damageElement_p;
  damageElement_p.parsers(damageType_p,
                          roll_p,
                          roll_p,
                          attribute_p,
                          duration_p,
                          counterMeasure_p,
                          others_p,
                          effectType_p);
  RPG_Combat_Damage_Type                         damage_p;
  damage_p.parsers(damageElement_p);
  RPG_Monster_DefenseAction_Type                 defenseAction_p;
  defenseAction_p.parsers(specialDefensePreCondition_p,
                          damage_p);
  RPG_Monster_SpecialDefenseProperties_Type      specialDefenseProperties_p;
  specialDefenseProperties_p.parsers(abilityClass_p,
                                     specialPropertyTypeUnion_p,
                                     actionType_p,
                                     usage_p,
                                     defenseAction_p);

//   RPG_Monster_NaturalWeapon_Type            naturalWeapon_p;
//   RPG_Monster_WeaponTypeUnion_Type               weaponUnion_p;
  RPG_Combat_AttackForm_Type                     attackForm_p;

  //   RPG_Magic_Spell_Type                           spell_p;
  //   ::xml_schema::unsigned_byte_pimpl         unsigned_byte_p;
//   RPG_Common_Duration_Type                       commonDuration_p;
  RPG_Common_SavingThrow_Type                    savingThrow_p;
//   RPG_Common_Attribute_Type                      attribute_p;
  //   ::xml_schema::unsigned_byte_pimpl         unsigned_byte_p;
  RPG_Common_SaveReductionType_Type              saveReduction_p;
  RPG_Common_SavingThrowCheck_Type               savingThrowCheck_p;
  savingThrowCheck_p.parsers(savingThrow_p,
                             attribute_p,
                             unsigned_byte_p,
                             saveReduction_p);
  RPG_Magic_SpellProperties_Type                 spellProperties_p;
  spellProperties_p.parsers(spell_p,
                            unsigned_byte_p,
                            commonDuration_p,
                            savingThrowCheck_p);
//   ::xml_schema::unsigned_byte_pimpl         unsigned_byte_p;
  RPG_Combat_RangedEffectUnion_Type              rangedEffectUnion_p;
  RPG_Combat_RangedAttackProperties_Type         rangedProperties_p;
  rangedProperties_p.parsers(unsigned_byte_p,
                             unsigned_byte_p,
                             rangedEffectUnion_p);
  ::xml_schema::boolean_pimpl                    bool_p;
  RPG_Monster_AttackAction_Type                  attackAction_p;
  attackAction_p.parsers(weaponUnion_p,
                         byte_p,
                         attackForm_p,
                         damage_p,
                         spellProperties_p,
                         unsigned_byte_p,
                         rangedProperties_p,
                         bool_p);
  RPG_Monster_Attack_Type                        attack_p;
  attack_p.parsers(byte_p,
                   byte_p,
                   attackAction_p,
                   attackAction_p,
                   bool_p);

//   RPG_Magic_AbilityClass_Type                    abilityClass_p;
//   RPG_Monster_SpecialPropertyTypeUnion_Type      specialPropertyTypeUnion_p;
//   RPG_Combat_ActionType_Type                     actionType_p;
//   RPG_Common_Usage_Type                          usage_p;
  RPG_Character_Alignment_Type                   alignment_p;
  RPG_Character_Condition_Type                   condition_p;
//   RPG_Character_Size_Type                        size_p;
//   RPG_Combat_Check_Type                          check_p;
  RPG_Monster_SpecialAttackPreCondition_Type     specialAttackPreCondition_p;
  specialAttackPreCondition_p.parsers(alignment_p,
                                      condition_p,
                                      condition_p,
                                      size_p,
                                      size_p,
                                      check_p);
//   RPG_Monster_AttackAction_Type                  attackAction_p;
//   RPG_Dice_Roll_Type                             roll_p;
  RPG_Monster_SpecialAttackProperties_Type       specialAttack_p;
  specialAttack_p.parsers(abilityClass_p,
                          specialPropertyTypeUnion_p,
                          actionType_p,
                          usage_p,
                          specialAttackPreCondition_p,
                          attackAction_p//,
//                           roll_p,
                          );

//   RPG_Magic_SpellProperties_Type                 spellProperties_p;
  RPG_Monster_SpecialAbilityProperties_Type      specialAbilityProperties_p;
  specialAbilityProperties_p.parsers(abilityClass_p,
                                     specialPropertyTypeUnion_p,
                                     actionType_p,
                                     usage_p,
                                     spellProperties_p,
                                     rangedProperties_p);

//   unsigned_int_pimpl                      space_p;
//   unsigned_int_pimpl                      reach_p;
  RPG_Monster_SavingThrowModifiers_Type          savingThrowModifiers_p;
  savingThrowModifiers_p.parsers(byte_p,
                                 byte_p,
                                 byte_p);
  RPG_Character_Attributes_Type                  attributes_p;
  attributes_p.parsers(unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p);
  RPG_Character_Skill_Type                       skill_p;
  RPG_Character_SkillValue_Type                  skillvalue_p;
  skillvalue_p.parsers(skill_p,
                       byte_p);
  RPG_Character_Skills_Type                      skills_p;
  skills_p.parsers(skillvalue_p);
  RPG_Character_Feat_Type                        feat_p;
  RPG_Character_Feats_Type                       feats_p;
  feats_p.parsers(feat_p);
  RPG_Character_Terrain_Type                     terrain_p;
  RPG_Character_Climate_Type                     climate_p;
  RPG_Character_Environment_Type                 environment_p;
  environment_p.parsers(terrain_p,
                        climate_p);
  RPG_Monster_Organization_Type                  organization_p;
  RPG_Dice_ValueRange_Type                       range_p;
  range_p.parsers(int_p,
                  int_p);
  RPG_Monster_OrganizationSlaverStep_Type        organizationSlaverStep_p;
  organizationSlaverStep_p.parsers(string_p,
                                   range_p);
  RPG_Monster_OrganizationStep_Type              organizationStep_p;
  organizationStep_p.parsers(organization_p,
                             range_p,
                             organizationSlaverStep_p);
  RPG_Monster_Organizations_Type                 organizations_p;
  organizations_p.parsers(organizationStep_p);
//   unsigned_int_pimpl                      challengeRating_p;
//   unsigned_int_pimpl                      treasureModifier_p;
  RPG_Character_AlignmentCivic_Type              alignmentCivic_p;
  RPG_Character_AlignmentEthic_Type              alignmentEthic_p;
//   RPG_Character_Alignment_Type                   alignment_p;
  alignment_p.parsers(alignmentCivic_p,
                      alignmentEthic_p);

  RPG_Monster_AdvancementStep_Type               advancementStep_p;
  advancementStep_p.parsers(size_p,
                            range_p);
  RPG_Monster_Advancement_Type                   advancement_p;
  advancement_p.parsers(advancementStep_p);
//   unsigned_int_pimpl                      levelAdjustment_p;
  RPG_Monster_PropertiesXML_Type                 propertiesXML_p;
  propertiesXML_p.parsers(string_p,
                          size_p,
                          type_p,
                          roll_p,
                          byte_p,
                          unsigned_byte_p,
                          naturalArmorClass_p,
                          specialDefenseProperties_p,
                          attack_p,
                          specialAttack_p,
                          specialAbilityProperties_p,
                          unsigned_byte_p,
                          unsigned_byte_p,
                          savingThrowModifiers_p,
                          attributes_p,
                          skills_p,
                          feats_p,
                          environment_p,
                          organizations_p,
                          unsigned_byte_p,
                          unsigned_byte_p,
                          alignment_p,
                          advancement_p,
                          unsigned_byte_p);
  RPG_Monster_Dictionary_Type                    dictionary_p(&myMonsterDictionary);
  dictionary_p.parsers(propertiesXML_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(dictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("urn:rpg"),
                               ACE_TEXT_ALWAYS_CHAR("monsterDictionary"));

  dictionary_p.pre();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
  try
  {
    doc_p.parse(filename_in,
                myXSDErrorHandler,
                flags);
  }
  catch (const ::xml_schema::parsing& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Monster_Dictionary::initMonsterDictionary(): exception occurred: \"%s\", returning\n"),
               text.c_str()));

    throw(exception);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Monster_Dictionary::initMonsterDictionary(): exception occurred, returning\n")));

    throw;
  }

  dictionary_p.post_RPG_Monster_Dictionary_Type();

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing character dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Monster_Properties RPG_Monster_Dictionary::getMonsterProperties(const std::string& monsterName_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::getMonsterProperties"));

  RPG_Monster_DictionaryIterator_t iterator = myMonsterDictionary.find(monsterName_in);
  if (iterator == myMonsterDictionary.end())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid monster name \"%s\" --> check implementation !, aborting\n"),
               monsterName_in.c_str()));

    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}

void RPG_Monster_Dictionary::generateRandomEncounter(const unsigned int& numDifferentMonsterTypes_in,
                                                     const unsigned int& numMonsters_in,
                                                     const RPG_Character_Alignment& alignment_in,
                                                     const RPG_Character_Environment& environment_in,
                                                     const RPG_Monster_OrganizationList_t& organizations_in,
                                                     RPG_Monster_Encounter_t& encounter_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::generateRandomEncounter"));

  // init result
  encounter_out.clear();

  // sanity check(s)
  ACE_ASSERT(numDifferentMonsterTypes_in <= myMonsterDictionary.size());
  if (numMonsters_in)
  {
    ACE_ASSERT(numDifferentMonsterTypes_in <= numMonsters_in);
  } // end IF

  RPG_Monster_List_t list;
  for (RPG_Monster_DictionaryIterator_t iterator = myMonsterDictionary.begin();
       iterator != myMonsterDictionary.end();
       iterator++)
  {
    // check if environment is appropriate
    if (environmentMatches(environment_in,
                           iterator->second.environment))
    {
      // check if alignment is appropriate
      if (alignmentMatches(alignment_in,
                           iterator->second.alignment))
      {
        // check if organizations are appropriate
        RPG_Monster_OrganizationList_t possible_organizations;
        for (RPG_Monster_OrganizationsIterator_t iterator2 = iterator->second.organizations.begin();
             iterator2 != iterator->second.organizations.end();
             iterator2++)
        {
          possible_organizations.insert((*iterator2).type);
        } // end FOR
        std::vector<RPG_Monster_Organization> intersection;
        std::set_intersection(organizations_in.begin(),
                              organizations_in.end(),
                              possible_organizations.begin(),
                              possible_organizations.end(),
                              intersection.begin());
        if ((organizations_in.find(ORGANIZATION_ANY) != organizations_in.end()) ||
            !intersection.empty())
        {
          list.push_back(iterator->first);
        } // end IF
      } // end IF
    } // end IF
  } // end FOR

  if (list.empty())
  {
    // nothing found in database...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("found no appropriate monster types (alignment: \"%s\", environment \"%s\", organizations: \"%s\" in dictionary (%d items) --> check implementation !, returning\n"),
               RPG_Character_Common_Tools::alignmentToString(alignment_in).c_str(),
               RPG_Character_Common_Tools::environmentToString(environment_in).c_str(),
               RPG_Monster_Common_Tools::organizationsToString(organizations_in).c_str(),
               myMonsterDictionary.size()));

    return;
  } // end IF

  RPG_Dice_RollResult_t result;
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
    // step2ba: ...choose a random organization from the intersection
    RPG_Monster_DictionaryIterator_t iterator2 = myMonsterDictionary.find(list[choiceType]);
    RPG_Monster_OrganizationList_t possible_organizations;
    for (RPG_Monster_OrganizationsIterator_t iterator3 = (*iterator2).second.organizations.begin();
         iterator3 != (*iterator2).second.organizations.end();
         iterator3++)
    {
      possible_organizations.insert((*iterator3).type);
    } // end FOR
    if (organizations_in.find(ORGANIZATION_ANY) == organizations_in.end())
    {
      std::vector<RPG_Monster_Organization> intersection;
      std::set_intersection(organizations_in.begin(),
                            organizations_in.end(),
                            possible_organizations.begin(),
                            possible_organizations.end(),
                            intersection.begin());
      possible_organizations.clear();
      for (std::vector<RPG_Monster_Organization>::const_iterator iterator4 = intersection.begin();
           iterator4 != intersection.end();
           iterator4++)
      {
        possible_organizations.insert(*iterator4);
      } // end FOR
    } // end IF

    result.clear();
    RPG_Dice::generateRandomNumbers(possible_organizations.size(),
                                    1,
                                    result);
    int choiceOrganization = result.front() - 1; // list index
    RPG_Monster_OrganizationListIterator_t iterator3 = possible_organizations.begin();
    std::advance(iterator3, choiceOrganization);
    RPG_Monster_OrganizationsIterator_t iterator4 = (*iterator2).second.organizations.begin();
    while ((*iterator4).type != *iterator3)
      iterator4++;
    ACE_ASSERT(iterator4 != (*iterator2).second.organizations.end());
    RPG_Dice_Roll roll;
    organizationStepToRoll(*iterator4,
                           roll);
    result.clear();
    RPG_Dice::simulateRoll(roll,
                           1,
                           result);

    encounter_out.insert(std::make_pair(list[choiceType], result.front()));
  } // end FOR

  // reduce/increase total number of foes to adjust the result
  // *IMPORTANT NOTE*: this means, however, that the final result doesn't necessarily
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

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("group #%d: %d %s\n"),
               index,
               (*iterator).second,
               (*iterator).first.c_str()));
  } // end FOR

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("generated %d foes...\n"),
             numCurrentFoes));
}

void RPG_Monster_Dictionary::organizationStepToRoll(const RPG_Monster_OrganizationStep& organizationStep_in,
                                                    RPG_Dice_Roll& roll_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::organizationStepToRoll"));

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
    case ORGANIZATION_BROOD:   // 2-4 --> 1d3+1
    case ORGANIZATION_CLUSTER:
    case ORGANIZATION_CLUTCH:
    case ORGANIZATION_PATCH:
    case ORGANIZATION_GANG:
    case ORGANIZATION_TEAM: // 3-4 --> 1d2+2
    case ORGANIZATION_SQUAD: // 3-5, 11-20 + leaders
    case ORGANIZATION_GROUP: // 4-4
    case ORGANIZATION_PACK: // 3-6 --> 1d4+2
    case ORGANIZATION_COLONY:
    case ORGANIZATION_FLOCK: // 5-8 --> 1d4+4
    case ORGANIZATION_TRIBE: // 7-12 --> 1d6+6
//     case ORGANIZATION_SLAVER: // 7-12 slaves
    case ORGANIZATION_BAND: // 11-20 + 2 sergeants + 1 leader + 150% noncombatants
    case ORGANIZATION_CLAN: // 30-100 + leaders + 50% noncombatants
    {
      RPG_Dice::rangeToRoll(organizationStep_in.range,
                            roll_out);

      break;
    }
    case ORGANIZATION_ANY:
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid organization \"%s\" --> check implementation !, returning\n"),
                 RPG_Monster_OrganizationHelper::RPG_Monster_OrganizationToString(organizationStep_in.type).c_str()));

      break;
    }
  } // end SWITCH
}

const bool RPG_Monster_Dictionary::alignmentMatches(const RPG_Character_Alignment& alignmentA_in,
                                                    const RPG_Character_Alignment& alignmentB_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::alignmentMatches"));

  // check civics
  switch (alignmentA_in.civic)
  {
    case ALIGNMENTCIVIC_ANY:
    {
      // OK
      break;
    }
    default:
    {
      switch (alignmentB_in.civic)
      {
        case ALIGNMENTCIVIC_ANY:
        {
          // OK
          break;
        }
        default:
        {
          if (alignmentA_in.civic != alignmentB_in.civic)
            return false;

          // OK
          break;
        }
      } // end SWITCH
    }
  } // end SWITCH

  // check ethics
  switch (alignmentA_in.ethic)
  {
    case ALIGNMENTETHIC_ANY:
    {
      // OK
      break;
    }
    default:
    {
      switch (alignmentB_in.ethic)
      {
        case ALIGNMENTETHIC_ANY:
        {
          // OK
          break;
        }
        default:
        {
          if (alignmentA_in.ethic != alignmentB_in.ethic)
            return false;

          // OK
          break;
        }
      } // end SWITCH
    }
  } // end SWITCH

  return true;
}

const bool RPG_Monster_Dictionary::environmentMatches(const RPG_Character_Environment& environmentA_in,
                                                      const RPG_Character_Environment& environmentB_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::environmentMatches"));

  if ((environmentA_in.terrain == TERRAIN_ANY) ||
      (environmentB_in.terrain == TERRAIN_ANY))
    return true;

  // different planes don't match: determine planes
  RPG_Character_Plane planeA = RPG_Character_Common_Tools::terrainToPlane(environmentA_in.terrain);
  RPG_Character_Plane planeB = RPG_Character_Common_Tools::terrainToPlane(environmentB_in.terrain);
  if (planeA != planeB)
    return false;

  switch (planeA)
  {
    case PLANE_MATERIAL:
    {
      if ((environmentA_in.terrain == TERRAIN_MATERIALPLANE_ANY) ||
          (environmentB_in.terrain == TERRAIN_MATERIALPLANE_ANY))
        return true;

      // handle climate
      if ((environmentA_in.climate == CLIMATE_ANY) ||
          (environmentB_in.climate == CLIMATE_ANY))
        return true;

      return ((environmentA_in.terrain == environmentB_in.terrain) &&
              (environmentA_in.climate == environmentB_in.climate));
    }
    case PLANE_TRANSITIVE:
    {
      if ((environmentA_in.terrain == TERRAIN_TRANSITIVEPLANE_ANY) ||
          (environmentB_in.terrain == TERRAIN_TRANSITIVEPLANE_ANY))
        return true;

      return (environmentA_in.terrain == environmentB_in.terrain);
    }
    case PLANE_INNER:
    {
      if ((environmentA_in.terrain == TERRAIN_INNERPLANE_ANY) ||
          (environmentB_in.terrain == TERRAIN_INNERPLANE_ANY))
        return true;

      return (environmentA_in.terrain == environmentB_in.terrain);
    }
    case PLANE_OUTER:
    {
      if ((environmentA_in.terrain == TERRAIN_OUTERPLANE_ANY) ||
          (environmentB_in.terrain == TERRAIN_OUTERPLANE_ANY))
        return true;

      switch (environmentA_in.terrain)
      {
        case TERRAIN_OUTERPLANE_LAWFUL_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_OUTERPLANE_LAWFUL_GOOD) ||
                  (environmentB_in.terrain == TERRAIN_OUTERPLANE_LAWFUL_EVIL));
        }
        case TERRAIN_OUTERPLANE_CHAOTIC_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_OUTERPLANE_CHAOTIC_GOOD) ||
                  (environmentB_in.terrain == TERRAIN_OUTERPLANE_CHAOTIC_EVIL));
        }
        case TERRAIN_OUTERPLANE_GOOD_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_OUTERPLANE_LAWFUL_GOOD) ||
                  (environmentB_in.terrain == TERRAIN_OUTERPLANE_CHAOTIC_GOOD));
        }
        case TERRAIN_OUTERPLANE_EVIL_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_OUTERPLANE_LAWFUL_EVIL) ||
                  (environmentB_in.terrain == TERRAIN_OUTERPLANE_CHAOTIC_EVIL));
        }
        case TERRAIN_OUTERPLANE_NEUTRAL:
        {
          return ((environmentB_in.terrain == TERRAIN_OUTERPLANE_NEUTRAL) ||
                  (environmentB_in.terrain == TERRAIN_OUTERPLANE_MILD_ANY) ||
                  (environmentB_in.terrain == TERRAIN_OUTERPLANE_STRONG_ANY));
        }
        case TERRAIN_OUTERPLANE_MILD_ANY:
        case TERRAIN_OUTERPLANE_STRONG_ANY:
        default:
        {
          // debug info
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid terrain: \"%s\" --> check implementation !, aborting\n"),
                     RPG_Character_TerrainHelper::RPG_Character_TerrainToString(environmentA_in.terrain).c_str()));

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid plane: \"%s\" --> check implementation !, aborting\n"),
                 RPG_Character_PlaneHelper::RPG_Character_PlaneToString(planeA).c_str()));

      break;
    }
  } // end SWITCH

  return false;
}

bool RPG_Monster_Dictionary::XSD_Error_Handler::handle(const std::string& id_in,
                                                       unsigned long line_in,
                                                       unsigned long column_in,
                                                       ::xml_schema::error_handler::severity severity_in,
                                                       const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::XSD_Error_Handler::handle"));

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
//              id_in.c_str(),
//              line_in,
//              column_in,
//              message_in.c_str()));

  switch (severity_in)
  {
    case ::xml_schema::error_handler::severity::warning:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("WARNING: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::error:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("ERROR: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::fatal:
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("FATAL: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("unkown error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
  } // end SWITCH

  // try to continue anyway...
  return true;
}

void RPG_Monster_Dictionary::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary::dump"));

  // simply dump the current content of our dictionary
  for (RPG_Monster_DictionaryIterator_t iterator = myMonsterDictionary.begin();
       iterator != myMonsterDictionary.end();
       iterator++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Monster (\"%s\"):\nSize: %s\nType: %s\nHit Dice: %s\nInitiative: %d\nSpeed: %d ft/round\nArmor Class (normal/touch/flat-footed): %d / %d / %d\nAttacks:\n-------\n%sSpace: %d ft\nReach: %d ft\nSaves (fortitude/reflex/will): %d / %d / %d\nAttributes:\n-----------\n%sSkills:\n-------\n%sFeats:\n------\n%sEnvironment: %s\nOrganizations:\n--------------\n%sChallenge Rating: %d\nTreasure Modifier: %d\nAlignment: %s\nAdvancement:\n------------\n%sLevel Adjustment: %d\n"),
               (iterator->first).c_str(),
               RPG_Character_SizeHelper::RPG_Character_SizeToString((iterator->second).size).c_str(),
               RPG_Monster_Common_Tools::typeToString((iterator->second).type).c_str(),
               RPG_Dice_Common_Tools::rollToString((iterator->second).hitDice).c_str(),
               (iterator->second).initiative,
               (iterator->second).speed,
               (iterator->second).armorClass.normal,
               (iterator->second).armorClass.touch,
               (iterator->second).armorClass.flatFooted,
               RPG_Monster_Common_Tools::attackToString((iterator->second).attack).c_str(),
               (iterator->second).space,
               (iterator->second).reach,
               (iterator->second).saves.fortitude,
               (iterator->second).saves.reflex,
               (iterator->second).saves.will,
               RPG_Character_Common_Tools::attributesToString((iterator->second).attributes).c_str(),
               RPG_Character_Skills_Common_Tools::skillsToString((iterator->second).skills).c_str(),
               RPG_Character_Skills_Common_Tools::featsToString((iterator->second).feats).c_str(),
               RPG_Character_Common_Tools::environmentToString((iterator->second).environment).c_str(),
               RPG_Monster_Common_Tools::organizationsToString((iterator->second).organizations).c_str(),
               (iterator->second).challengeRating,
               (iterator->second).treasureModifier,
               RPG_Character_Common_Tools::alignmentToString((iterator->second).alignment).c_str(),
               RPG_Monster_Common_Tools::advancementToString((iterator->second).advancements).c_str(),
               (iterator->second).levelAdjustment));
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("===========================\n")));
  } // end FOR
}
