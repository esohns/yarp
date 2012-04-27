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
#include "stdafx.h"

#include "rpg_monster_dictionary.h"

#include "rpg_monster_common_tools.h"

#include <rpg_dice_XML_parser.h>
#include <rpg_common_XML_parser.h>
#include <rpg_character_XML_parser.h>
#include <rpg_magic_XML_parser.h>
#include <rpg_combat_XML_parser.h>
#include "rpg_monster_XML_parser.h"

#include <rpg_character_common_tools.h>
#include <rpg_character_skills_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_xsderrorhandler.h>
#include <rpg_common_tools.h>

#include <rpg_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <iostream>
#include <algorithm>
#include <sstream>

RPG_Monster_Dictionary::RPG_Monster_Dictionary()
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::RPG_Monster_Dictionary"));

}

RPG_Monster_Dictionary::~RPG_Monster_Dictionary()
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::~RPG_Monster_Dictionary"));

}

void
RPG_Monster_Dictionary::init(const std::string& filename_in,
                             const bool& validateXML_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::init"));

  // Construct the parser.
  //
  ::xml_schema::string_pimpl                     string_p;

  RPG_Common_Size_Type                           size_p;
  ::xml_schema::boolean_pimpl                    bool_p;
  RPG_Monster_Size_Type                          size_p2;
  size_p2.parsers(size_p,
                  bool_p);

  RPG_Common_CreatureMetaType_Type               metaType_p;
  RPG_Common_CreatureSubType_Type                subType_p;
  RPG_Common_CreatureType_Type                   type_p;
  type_p.parsers(metaType_p,
                 subType_p);

  ::xml_schema::unsigned_int_pimpl               unsigned_int_p;
  RPG_Dice_DieType_Type                          dieType_p;
  ::xml_schema::int_pimpl                        int_p;
  RPG_Dice_Roll_Type                             roll_p;
  roll_p.parsers(unsigned_int_p,
                 dieType_p,
                 int_p);

  ::xml_schema::byte_pimpl                       byte_p;
  RPG_Monster_NaturalArmorClass_Type             naturalArmorClass_p;
  naturalArmorClass_p.parsers(byte_p,
                              byte_p,
                              byte_p);

  RPG_Magic_AbilityClass_Type                    abilityClass_p;
  RPG_Monster_SpecialPropertyTypeUnion_Type      specialPropertyTypeUnion_p;
  RPG_Common_ActionType_Type                     actionType_p;
  RPG_Common_Usage_Type                          usage_p;
  RPG_Monster_SpecialBaseProperties_Type         specialBaseProperties_p;
  specialBaseProperties_p.parsers(abilityClass_p,
                                  specialPropertyTypeUnion_p,
                                  actionType_p,
                                  usage_p);

  RPG_Monster_WeaponTypeUnion_Type               weaponUnion_p;
  RPG_Common_Condition_Type                      condition_p;
  RPG_Monster_SpecialDefensePreCondition_Type    specialDefensePreCondition_p;
  specialDefensePreCondition_p.parsers(weaponUnion_p,
                                       condition_p,
                                       condition_p,
                                       bool_p);

  RPG_Combat_DamageTypeUnion_Type                damageType_p;
  ::xml_schema::unsigned_short_pimpl             unsigned_short_p;
  RPG_Combat_DamageDuration_Type                 duration_p;
  duration_p.parsers(roll_p,
                     roll_p,
                     unsigned_short_p,
                     unsigned_short_p,
                     bool_p);
  RPG_Common_CounterMeasure_Type                 counterMeasureType_p;
  RPG_Combat_CheckTypeUnion_Type                 checkTypeUnion_p;
  RPG_Common_Attribute_Type                      attribute_p;
  ::xml_schema::unsigned_byte_pimpl              unsigned_byte_p;
  RPG_Combat_Check_Type                          check_p;
  check_p.parsers(checkTypeUnion_p,
                  attribute_p,
                  unsigned_byte_p);
  RPG_Magic_SpellType_Type                       spell_p;
  RPG_Common_Duration_Type                       commonDuration_p;
  commonDuration_p.parsers(unsigned_int_p,
                           unsigned_int_p,
                           unsigned_int_p);
//   RPG_Character_Condition_Type                   condition_p;
  RPG_Combat_DamageReductionType_Type            reduction_p;
  RPG_Combat_DamageCounterMeasure_Type           counterMeasure_p;
  counterMeasure_p.parsers(counterMeasureType_p,
                           check_p,
                           spell_p,
                           commonDuration_p,
                           condition_p,
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
                          condition_p,
                          duration_p,
                          counterMeasure_p,
                          others_p,
                          effectType_p);
  RPG_Combat_Damage_Type                         damage_p;
  damage_p.parsers(damageElement_p,
                   bool_p);
  RPG_Combat_RangedEffectUnion_Type              rangedEffectUnion_p;
  RPG_Combat_RangedAttackProperties_Type         rangedProperties_p;
  rangedProperties_p.parsers(unsigned_byte_p,
                             unsigned_byte_p,
                             rangedEffectUnion_p);
  RPG_Monster_DefenseAction_Type                 defenseAction_p;
  defenseAction_p.parsers(specialDefensePreCondition_p,
                          damage_p,
                          rangedProperties_p);
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
  RPG_Magic_SpellLikeProperties_Type             spellProperties_p;
  spellProperties_p.parsers(spell_p,
                            unsigned_byte_p,
                            commonDuration_p,
                            savingThrowCheck_p);
//   ::xml_schema::unsigned_byte_pimpl         unsigned_byte_p;
//   RPG_Combat_RangedAttackProperties_Type         rangedProperties_p;
  RPG_Character_Ability_Type                     ability_p;
//   ::xml_schema::boolean_pimpl                    bool_p;
  RPG_Monster_AttackAction_Type                  attackAction_p;
  attackAction_p.parsers(weaponUnion_p,
                         byte_p,
                         attackForm_p,
                         damage_p,
                         spellProperties_p,
                         unsigned_byte_p,
                         rangedProperties_p,
                         ability_p,
                         bool_p,
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
  RPG_Monster_ActionTrigger_Type                 trigger_p;
  trigger_p.parsers(weaponUnion_p,
                    unsigned_byte_p);
  RPG_Character_Alignment_Type                   alignment_p;
//   RPG_Character_Condition_Type                   condition_p;
//   RPG_Character_Size_Type                        size_p;
//   RPG_Combat_Check_Type                          check_p;
  RPG_Monster_SpecialAttackPreCondition_Type     specialAttackPreCondition_p;
  specialAttackPreCondition_p.parsers(trigger_p,
                                      alignment_p,
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

  RPG_Monster_SpecialAbilityPreCondition_Type    specialAbilityPreCondition_p;
  specialAbilityPreCondition_p.parsers(condition_p,
                                       size_p,
                                       check_p);
//   RPG_Dice_Roll_Type                             roll_p;
  ::xml_schema::float_pimpl                      float_p;
  RPG_Monster_SummonMonster_Type                 summonMonster_p;
  summonMonster_p.parsers(string_p,
                          roll_p,
                          float_p,
                          bool_p);
//   RPG_Magic_SpellProperties_Type                 spellProperties_p;
  RPG_Monster_SpecialAbilityProperties_Type      specialAbilityProperties_p;
  specialAbilityProperties_p.parsers(abilityClass_p,
                                     specialPropertyTypeUnion_p,
                                     actionType_p,
                                     usage_p,
                                     specialAbilityPreCondition_p,
                                     summonMonster_p,
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
  RPG_Common_Skill_Type                          skill_p;
  RPG_Character_SkillValue_Type                  skillvalue_p;
  skillvalue_p.parsers(skill_p,
                       byte_p);
  RPG_Character_Skills_Type                      skills_p;
  skills_p.parsers(skillvalue_p);
  RPG_Character_Feat_Type                        feat_p;
  RPG_Character_Feats_Type                       feats_p;
  feats_p.parsers(feat_p);
  RPG_Common_Plane_Type                          plane_p;
  RPG_Common_Terrain_Type                        terrain_p;
  RPG_Common_Climate_Type                        climate_p;
  RPG_Common_TimeOfDay_Type                      time_p;
  RPG_Common_AmbientLighting_Type                lighting_p;
  RPG_Common_Environment_Type                    environment_p;
  environment_p.parsers(plane_p,
                        terrain_p,
                        climate_p,
                        time_p,
                        lighting_p,
                        bool_p);
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
                          size_p2,
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
                RPG_XSDErrorHandler,
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

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing character dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Monster_Properties&
RPG_Monster_Dictionary::getProperties(const std::string& name_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::getProperties"));

  RPG_Monster_DictionaryConstIterator_t iterator = myMonsterDictionary.find(name_in);
  if (iterator == myMonsterDictionary.end())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid monster \"%s\", aborting\n"),
               name_in.c_str()));

    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}

unsigned int
RPG_Monster_Dictionary::numEntries() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::numEntries"));

  return myMonsterDictionary.size();
}

void
RPG_Monster_Dictionary::find(const RPG_Character_Alignment& alignment_in,
                             const RPG_Common_Environment& environment_in,
                             const RPG_Monster_OrganizationSet_t& organizations_in,
                             const RPG_Monster_HitDice& hitDice_in,
                             RPG_Monster_List_t& list_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::find"));

  // init return value(s)
  list_out.clear();

  RPG_Monster_OrganizationSet_t possible_organizations;
  std::vector<RPG_Monster_Organization> intersection;
  for (RPG_Monster_DictionaryConstIterator_t iterator = myMonsterDictionary.begin();
       iterator != myMonsterDictionary.end();
       iterator++)
  {
    // check if environment is compatible
    if (!RPG_Common_Tools::match(environment_in,
                                 iterator->second.environment))
      continue;

    // check if alignment is compatible
    if (!RPG_Character_Common_Tools::match(alignment_in,
                                           iterator->second.alignment))
      continue;

    // check if organizations are compatible
    if (organizations_in.find(ORGANIZATION_ANY) == organizations_in.end())
    {
      possible_organizations.clear();
      for (RPG_Monster_OrganizationsIterator_t iterator2 = iterator->second.organizations.begin();
           iterator2 != iterator->second.organizations.end();
           iterator2++)
        possible_organizations.insert((*iterator2).type);
      intersection.clear();
      std::set_intersection(organizations_in.begin(),
                            organizations_in.end(),
                            possible_organizations.begin(),
                            possible_organizations.end(),
                            intersection.begin());
      if (intersection.empty())
        continue;
    } // end IF

    // check if HD is compatible
    if (!RPG_Monster_Common_Tools::match((*iterator).second.hitDice,
                                         hitDice_in))
      continue;

    list_out.push_back(iterator->first);
  } // end FOR
}

void
RPG_Monster_Dictionary::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster_Dictionary::dump"));

  // simply dump the current content of our dictionary
  for (RPG_Monster_DictionaryConstIterator_t iterator = myMonsterDictionary.begin();
       iterator != myMonsterDictionary.end();
       iterator++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Monster (\"%s\"):\nSize: %s\nType: %s\nHit Dice: %s\nInitiative: %d\nSpeed: %d ft/round\nArmor Class (normal/touch/flat-footed): %d / %d / %d\nAttacks:\n-------\n%sSpace: %d ft\nReach: %d ft\nSaves (fortitude/reflex/will): %d / %d / %d\nAttributes:\n-----------\n%sSkills:\n-------\n%sFeats:\n------\n%sEnvironment: %s\nOrganizations:\n--------------\n%sChallenge Rating: %d\nTreasure Modifier: %d\nAlignment: %s\nAdvancement:\n------------\n%sLevel Adjustment: %d\n"),
               (iterator->first).c_str(),
               RPG_Common_SizeHelper::RPG_Common_SizeToString((iterator->second).size.size).c_str(),
               RPG_Common_Tools::creatureTypeToString((iterator->second).type).c_str(),
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
               RPG_Common_Tools::environmentToString((iterator->second).environment).c_str(),
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
