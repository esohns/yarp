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

#include "rpg_character_XML_parser.h"

#include "rpg_character_incl.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Character_Gender RPG_Character_Gender_Type::post_RPG_Character_Gender_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Gender_Type::post_RPG_Character_Gender_Type"));

  return RPG_Character_GenderHelper::stringToRPG_Character_Gender(post_string());
}

RPG_Character_Race RPG_Character_Race_Type::post_RPG_Character_Race_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Type::post_RPG_Character_Race_Type"));

  return RPG_Character_RaceHelper::stringToRPG_Character_Race(post_string());
}

RPG_Character_MetaClass RPG_Character_MetaClass_Type::post_RPG_Character_MetaClass_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_MetaClass_Type::post_RPG_Character_MetaClass_Type"));

  return RPG_Character_MetaClassHelper::stringToRPG_Character_MetaClass(post_string());
}

RPG_Character_ClassXML_Type::RPG_Character_ClassXML_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_ClassXML_Type::RPG_Character_ClassXML_Type"));

  myCurrentClass.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  myCurrentClass.subClasses.clear();
}

void RPG_Character_ClassXML_Type::metaClass(const RPG_Character_MetaClass& metaClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_ClassXML_Type::metaClass"));

  myCurrentClass.metaClass = metaClass_in;
}

void RPG_Character_ClassXML_Type::subClass(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_ClassXML_Type::subClass"));

  myCurrentClass.subClasses.push_back(subClass_in);
}

RPG_Character_ClassXML RPG_Character_ClassXML_Type::post_RPG_Character_ClassXML_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_ClassXML_Type::post_RPG_Character_ClassXML_Type"));

  RPG_Character_ClassXML result = myCurrentClass;

  // clear structure
  myCurrentClass.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  myCurrentClass.subClasses.clear();

  return result;
}

RPG_Character_Ability RPG_Character_Ability_Type::post_RPG_Character_Ability_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Ability_Type::post_RPG_Character_Ability_Type"));

  return RPG_Character_AbilityHelper::stringToRPG_Character_Ability(post_string());
}

// RPG_Character_Abilities_Type::RPG_Character_Abilities_Type()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_Abilities_Type::RPG_Character_Abilities_Type"));
//
//   myCurrentAbilities.abilities.clear();
// }
//
// void RPG_Character_Abilities_Type::ability(const RPG_Character_Ability& ability_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_Abilities_Type::ability"));
//
//   myCurrentAbilities.abilities.push_back(ability_in);
// }
//
// RPG_Character_Abilities RPG_Character_Abilities_Type::post_RPG_Character_Abilities_Type()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_Abilities_Type::post_RPG_Character_Abilities_Type"));
//
//   RPG_Character_Abilities result = myCurrentAbilities;
//
//   // clear structure
//   myCurrentAbilities.abilities.clear();
//
//   return result;
// }

RPG_Character_Attributes_Type::RPG_Character_Attributes_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::RPG_Character_Attributes_Type"));

  myCurrentAttributes.strength = 0;
  myCurrentAttributes.dexterity = 0;
  myCurrentAttributes.constitution = 0;
  myCurrentAttributes.intelligence = 0;
  myCurrentAttributes.wisdom = 0;
  myCurrentAttributes.charisma = 0;
}

void RPG_Character_Attributes_Type::strength(unsigned char strength_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::strength"));

  myCurrentAttributes.strength = strength_in;
}

void RPG_Character_Attributes_Type::dexterity(unsigned char dexterity_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::dexterity"));

  myCurrentAttributes.dexterity = dexterity_in;
}

void RPG_Character_Attributes_Type::constitution(unsigned char constitution_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::constitution"));

  myCurrentAttributes.constitution = constitution_in;
}

void RPG_Character_Attributes_Type::intelligence(unsigned char intelligence_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::intelligence"));

  myCurrentAttributes.intelligence = intelligence_in;
}

void RPG_Character_Attributes_Type::wisdom(unsigned char wisdom_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::wisdom"));

  myCurrentAttributes.wisdom = wisdom_in;
}

void RPG_Character_Attributes_Type::charisma(unsigned char charisma_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::charisma"));

  myCurrentAttributes.charisma = charisma_in;
}

RPG_Character_Attributes RPG_Character_Attributes_Type::post_RPG_Character_Attributes_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::post_RPG_Character_Attributes_Type"));

  RPG_Character_Attributes result = myCurrentAttributes;

  // clear structure
  myCurrentAttributes.strength = 0;
  myCurrentAttributes.dexterity = 0;
  myCurrentAttributes.constitution = 0;
  myCurrentAttributes.intelligence = 0;
  myCurrentAttributes.wisdom = 0;
  myCurrentAttributes.charisma = 0;

  return result;
}

RPG_Character_SkillValue_Type::RPG_Character_SkillValue_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::RPG_Character_SkillValue_Type"));

  myCurrentSkill.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentSkill.rank = 0;
}

void RPG_Character_SkillValue_Type::skill(const RPG_Common_Skill& skill_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::skill"));

  myCurrentSkill.skill = skill_in;
}

void RPG_Character_SkillValue_Type::rank(signed char rank_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::rank"));

  myCurrentSkill.rank = rank_in;
}

RPG_Character_SkillValue RPG_Character_SkillValue_Type::post_RPG_Character_SkillValue_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::post_RPG_Character_SkillValue_Type"));

  RPG_Character_SkillValue result = myCurrentSkill;

  // clear structure
  myCurrentSkill.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentSkill.rank = 0;

  return result;
}

RPG_Character_Skills_Type::RPG_Character_Skills_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Skills_Type::RPG_Character_Skills_Type"));

  myCurrentSkills.skills.clear();
}

void RPG_Character_Skills_Type::skill(const RPG_Character_SkillValue& skill_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Skills_Type::skill"));

  myCurrentSkills.skills.push_back(skill_in);
}

RPG_Character_Skills RPG_Character_Skills_Type::post_RPG_Character_Skills_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Skills_Type::post_RPG_Character_Skills_Type"));

  RPG_Character_Skills result = myCurrentSkills;

  // clear structure
  myCurrentSkills.skills.clear();

  return result;
}

RPG_Character_CheckTypeUnion_Type::RPG_Character_CheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_CheckTypeUnion_Type::RPG_Character_CheckTypeUnion_Type"));

  myCurrentCheckType.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheckType.discriminator = RPG_Character_CheckTypeUnion::INVALID;
}

void RPG_Character_CheckTypeUnion_Type::_characters(const ::xml_schema::ro_string& checkType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_CheckTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_Attribute_Type --> "ATTRIBUTE_xxx"
  // - RPG_Character_Skill_Type --> "SKILL_xxx"
  std::string type = checkType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_")) == 0)
  {
    myCurrentCheckType.attribute = RPG_Common_AttributeHelper::stringToRPG_Common_Attribute(checkType_in);
    myCurrentCheckType.discriminator = RPG_Character_CheckTypeUnion::ATTRIBUTE;
  } // end IF
  else
  {
    myCurrentCheckType.skill = RPG_Common_SkillHelper::stringToRPG_Common_Skill(checkType_in);
    myCurrentCheckType.discriminator = RPG_Character_CheckTypeUnion::SKILL;
  } // end ELSE
}

RPG_Character_CheckTypeUnion RPG_Character_CheckTypeUnion_Type::post_RPG_Character_CheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_CheckTypeUnion_Type::post_RPG_Character_CheckTypeUnion_Type"));

  RPG_Character_CheckTypeUnion result = myCurrentCheckType;

  // clear structure
  myCurrentCheckType.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheckType.discriminator = RPG_Character_CheckTypeUnion::INVALID;

  return result;
}

RPG_Character_Check_Type::RPG_Character_Check_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Check_Type::RPG_Character_Check_Type"));

  myCurrentCheck.type.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheck.type.discriminator = RPG_Character_CheckTypeUnion::INVALID;
  myCurrentCheck.difficultyClass = 0;
}

void RPG_Character_Check_Type::type(const RPG_Character_CheckTypeUnion& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Check_Type::type"));

  myCurrentCheck.type = type_in;
}

void RPG_Character_Check_Type::difficultyClass(unsigned char difficultyClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Check_Type::difficultyClass"));

  myCurrentCheck.difficultyClass = difficultyClass_in;
}

RPG_Character_Check RPG_Character_Check_Type::post_RPG_Character_Check_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Check_Type::post_RPG_Character_Check_Type"));

  RPG_Character_Check result = myCurrentCheck;

  // clear structure
  myCurrentCheck.type.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheck.type.discriminator = RPG_Character_CheckTypeUnion::INVALID;
  myCurrentCheck.difficultyClass = 0;

  return result;
}

RPG_Character_Feat RPG_Character_Feat_Type::post_RPG_Character_Feat_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Feat_Type::post_RPG_Character_Feat_Type"));

  return RPG_Character_FeatHelper::stringToRPG_Character_Feat(post_string());
}

RPG_Character_Feats_Type::RPG_Character_Feats_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Feats_Type::RPG_Character_Feats_Type"));

  myCurrentFeats.feats.clear();
}

void RPG_Character_Feats_Type::feat(const RPG_Character_Feat& feat_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Feats_Type::feat"));

  myCurrentFeats.feats.push_back(feat_in);
}

RPG_Character_Feats RPG_Character_Feats_Type::post_RPG_Character_Feats_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Feats_Type::post_RPG_Character_Feats_Type"));

  RPG_Character_Feats result = myCurrentFeats;

  // clear structure
  myCurrentFeats.feats.clear();

  return result;
}

RPG_Character_AlignmentCivic RPG_Character_AlignmentCivic_Type::post_RPG_Character_AlignmentCivic_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_AlignmentCivic_Type::post_RPG_Character_AlignmentCivic_Type"));

  return RPG_Character_AlignmentCivicHelper::stringToRPG_Character_AlignmentCivic(post_string());
}

RPG_Character_AlignmentEthic RPG_Character_AlignmentEthic_Type::post_RPG_Character_AlignmentEthic_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_AlignmentEthic_Type::post_RPG_Character_AlignmentEthic_Type"));

  return RPG_Character_AlignmentEthicHelper::stringToRPG_Character_AlignmentEthic(post_string());
}

RPG_Character_Alignment_Type::RPG_Character_Alignment_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::RPG_Character_Alignment_Type"));

  myCurrentAlignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentAlignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
}

void RPG_Character_Alignment_Type::civic(const RPG_Character_AlignmentCivic& alignmentCivic_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::civic"));

  myCurrentAlignment.civic = alignmentCivic_in;
}

void RPG_Character_Alignment_Type::ethic(const RPG_Character_AlignmentEthic& alignmentEthic_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::ethic"));

  myCurrentAlignment.ethic = alignmentEthic_in;
}

RPG_Character_Alignment RPG_Character_Alignment_Type::post_RPG_Character_Alignment_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::post_RPG_Character_Alignment_Type"));

  RPG_Character_Alignment result = myCurrentAlignment;

  // clear structure
  myCurrentAlignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentAlignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;

  return result;
}

RPG_Character_EquipmentSlot RPG_Character_EquipmentSlot_Type::post_RPG_Character_EquipmentSlot_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_EquipmentSlot_Type::post_RPG_Character_EquipmentSlot_Type"));

  return RPG_Character_EquipmentSlotHelper::stringToRPG_Character_EquipmentSlot(post_string());
}

RPG_Character_OffHand RPG_Character_OffHand_Type::post_RPG_Character_OffHand_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Character_OffHand_Type::post_RPG_Character_OffHand_Type"));

  return RPG_Character_OffHandHelper::stringToRPG_Character_OffHand(post_string());
}

// RPG_Character_BaseXML_Type::RPG_Character_BaseXML_Type()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::RPG_Character_BaseXML_Type"));
//
//   myCurrentCharacterBaseXML.gender = RPG_CHARACTER_GENDER_INVALID;
//   myCurrentCharacterBaseXML.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
//   myCurrentCharacterBaseXML.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
//   myCurrentCharacterBaseXML.attributes.strength = 0;
//   myCurrentCharacterBaseXML.attributes.dexterity = 0;
//   myCurrentCharacterBaseXML.attributes.constitution = 0;
//   myCurrentCharacterBaseXML.attributes.intelligence = 0;
//   myCurrentCharacterBaseXML.attributes.wisdom = 0;
//   myCurrentCharacterBaseXML.attributes.charisma = 0;
//   myCurrentCharacterBaseXML.skills.skills.clear();
//   myCurrentCharacterBaseXML.feats.feats.clear();
//   myCurrentCharacterBaseXML.abilities.abilities.clear();
//   myCurrentCharacterBaseXML.XP = 0;
//   myCurrentCharacterBaseXML.HP = 0;
//   myCurrentCharacterBaseXML.maxHP = 0;
//   myCurrentCharacterBaseXML.gold = 0;
//   myCurrentCharacterBaseXML.conditions.clear();
// }
//
// void RPG_Character_BaseXML_Type::gender(const RPG_Character_Gender& gender_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::gender"));
//
//   myCurrentCharacterBaseXML.gender = gender_in;
// }
//
// void RPG_Character_BaseXML_Type::alignment(const RPG_Character_Alignment& alignment_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::alignment"));
//
//   myCurrentCharacterBaseXML.alignment = alignment_in;
// }
//
// void RPG_Character_BaseXML_Type::attributes(const RPG_Character_Attributes& attributes_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::attributes"));
//
//   myCurrentCharacterBaseXML.attributes = attributes_in;
// }
//
// void RPG_Character_BaseXML_Type::skills(const RPG_Character_Skills& skills_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::skills"));
//
//   myCurrentCharacterBaseXML.skills = skills_in;
// }
//
// void RPG_Character_BaseXML_Type::feats(const RPG_Character_Feats& feats_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::feats"));
//
//   myCurrentCharacterBaseXML.feats = feats_in;
// }
//
// void RPG_Character_BaseXML_Type::abilities(const RPG_Character_Abilities& abilities_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::abilities"));
//
//   myCurrentCharacterBaseXML.abilities = abilities_in;
// }
//
// void RPG_Character_BaseXML_Type::XP(unsigned int XP_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::XP"));
//
//   myCurrentCharacterBaseXML.XP = XP_in;
// }
//
// void RPG_Character_BaseXML_Type::HP(long long HP_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::HP"));
//
//   myCurrentCharacterBaseXML.HP = HP_in;
// }
//
// void RPG_Character_BaseXML_Type::maxHP(unsigned int maxHP_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::maxHP"));
//
//   myCurrentCharacterBaseXML.maxHP = maxHP_in;
// }
//
// void RPG_Character_BaseXML_Type::gold(unsigned int gold_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::gold"));
//
//   myCurrentCharacterBaseXML.gold = gold_in;
// }
//
// void RPG_Character_BaseXML_Type::condition(const RPG_Common_Condition& condition_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::condition"));
//
//   myCurrentCharacterBaseXML.conditions.push_back(condition_in);
// }
//
// RPG_Character_BaseXML RPG_Character_BaseXML_Type::post_RPG_Character_BaseXML_Type()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_BaseXML_Type::post_RPG_Character_BaseXML_Type"));
//
//   RPG_Character_BaseXML result = myCurrentCharacterBaseXML;
//
//   myCurrentCharacterBaseXML.gender = RPG_CHARACTER_GENDER_INVALID;
//   myCurrentCharacterBaseXML.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
//   myCurrentCharacterBaseXML.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
//   myCurrentCharacterBaseXML.attributes.strength = 0;
//   myCurrentCharacterBaseXML.attributes.dexterity = 0;
//   myCurrentCharacterBaseXML.attributes.constitution = 0;
//   myCurrentCharacterBaseXML.attributes.intelligence = 0;
//   myCurrentCharacterBaseXML.attributes.wisdom = 0;
//   myCurrentCharacterBaseXML.attributes.charisma = 0;
//   myCurrentCharacterBaseXML.skills.skills.clear();
//   myCurrentCharacterBaseXML.feats.feats.clear();
//   myCurrentCharacterBaseXML.abilities.abilities.clear();
//   myCurrentCharacterBaseXML.XP = 0;
//   myCurrentCharacterBaseXML.HP = 0;
//   myCurrentCharacterBaseXML.maxHP = 0;
//   myCurrentCharacterBaseXML.gold = 0;
//   myCurrentCharacterBaseXML.conditions.clear();
//
//   return result;
// }

// RPG_Character_PlayerXML_Type::RPG_Character_PlayerXML_Type()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::RPG_Character_PlayerXML_Type"));
//
//   myCurrentCharacterXML.gender = RPG_CHARACTER_GENDER_INVALID;
//   myCurrentCharacterXML.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
//   myCurrentCharacterXML.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
//   myCurrentCharacterXML.attributes.strength = 0;
//   myCurrentCharacterXML.attributes.dexterity = 0;
//   myCurrentCharacterXML.attributes.constitution = 0;
//   myCurrentCharacterXML.attributes.intelligence = 0;
//   myCurrentCharacterXML.attributes.wisdom = 0;
//   myCurrentCharacterXML.attributes.charisma = 0;
//   myCurrentCharacterXML.skills.skills.clear();
//   myCurrentCharacterXML.feats.feats.clear();
//   myCurrentCharacterXML.abilities.abilities.clear();
//   myCurrentCharacterXML.XP = 0;
//   myCurrentCharacterXML.HP = 0;
//   myCurrentCharacterXML.maxHP = 0;
//   myCurrentCharacterXML.gold = 0;
//   myCurrentCharacterXML.conditions.clear();
//   // -----------------------------------
//   myCurrentCharacterXML.name.clear();
//   myCurrentCharacterXML.races.clear();
//   myCurrentCharacterXML.classXML.metaClass = RPG_CHARACTER_METACLASS_INVALID;
//   myCurrentCharacterXML.classXML.subClasses.clear();
//   myCurrentCharacterXML.offhand = RPG_CHARACTER_OFFHAND_INVALID;
// }
//
// void RPG_Character_PlayerXML_Type::gender(const RPG_Character_Gender& gender_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::gender"));
//
//   myCurrentCharacterXML.gender = gender_in;
// }
//
// void RPG_Character_PlayerXML_Type::alignment(const RPG_Character_Alignment& alignment_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::alignment"));
//
//   myCurrentCharacterXML.alignment = alignment_in;
// }
//
// void RPG_Character_PlayerXML_Type::attributes(const RPG_Character_Attributes& attributes_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::attributes"));
//
//   myCurrentCharacterXML.attributes = attributes_in;
// }
//
// void RPG_Character_PlayerXML_Type::skills(const RPG_Character_Skills& skills_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::skills"));
//
//   myCurrentCharacterXML.skills = skills_in;
// }
//
// void RPG_Character_PlayerXML_Type::feats(const RPG_Character_Feats& feats_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::feats"));
//
//   myCurrentCharacterXML.feats = feats_in;
// }
//
// void RPG_Character_PlayerXML_Type::abilities(const RPG_Character_Abilities& abilities_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::abilities"));
//
//   myCurrentCharacterXML.abilities = abilities_in;
// }
//
// void RPG_Character_PlayerXML_Type::XP(unsigned int XP_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::XP"));
//
//   myCurrentCharacterXML.XP = XP_in;
// }
//
// void RPG_Character_PlayerXML_Type::HP(long long HP_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::HP"));
//
//   myCurrentCharacterXML.HP = HP_in;
// }
//
// void RPG_Character_PlayerXML_Type::maxHP(unsigned int maxHP_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::maxHP"));
//
//   myCurrentCharacterXML.maxHP = maxHP_in;
// }
//
// void RPG_Character_PlayerXML_Type::gold(unsigned int gold_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::gold"));
//
//   myCurrentCharacterXML.gold = gold_in;
// }
//
// void RPG_Character_PlayerXML_Type::condition(const RPG_Common_Condition& condition_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::condition"));
//
//   myCurrentCharacterXML.conditions.push_back(condition_in);
// }
//
// void RPG_Character_PlayerXML_Type::name(const ::std::string& name_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::name"));
//
//   myCurrentCharacterXML.name = name_in;
// }
//
// void RPG_Character_PlayerXML_Type::race(const RPG_Character_Race& race_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::race"));
//
//   myCurrentCharacterXML.races.push_back(race_in);
// }
//
// void RPG_Character_PlayerXML_Type::classXML(const RPG_Character_ClassXML& classXML_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::classXML"));
//
//   myCurrentCharacterXML.classXML = classXML_in;
// }
//
// void RPG_Character_PlayerXML_Type::offhand(const RPG_Character_OffHand& offhand_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::offhand"));
//
//   myCurrentCharacterXML.offhand = offhand_in;
// }
//
// RPG_Character_PlayerXML RPG_Character_PlayerXML_Type::post_RPG_Character_PlayerXML_Type()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Character_PlayerXML_Type::post_RPG_Character_PlayerXML_Type"));
//
//   RPG_Character_PlayerXML result = myCurrentCharacterXML;
//
//   myCurrentCharacterXML.gender = RPG_CHARACTER_GENDER_INVALID;
//   myCurrentCharacterXML.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
//   myCurrentCharacterXML.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
//   myCurrentCharacterXML.attributes.strength = 0;
//   myCurrentCharacterXML.attributes.dexterity = 0;
//   myCurrentCharacterXML.attributes.constitution = 0;
//   myCurrentCharacterXML.attributes.intelligence = 0;
//   myCurrentCharacterXML.attributes.wisdom = 0;
//   myCurrentCharacterXML.attributes.charisma = 0;
//   myCurrentCharacterXML.skills.skills.clear();
//   myCurrentCharacterXML.feats.feats.clear();
//   myCurrentCharacterXML.abilities.abilities.clear();
//   myCurrentCharacterXML.XP = 0;
//   myCurrentCharacterXML.HP = 0;
//   myCurrentCharacterXML.maxHP = 0;
//   myCurrentCharacterXML.gold = 0;
//   myCurrentCharacterXML.conditions.clear();
//   // -----------------------------------
//   myCurrentCharacterXML.name.clear();
//   myCurrentCharacterXML.races.clear();
//   myCurrentCharacterXML.classXML.metaClass = RPG_CHARACTER_METACLASS_INVALID;
//   myCurrentCharacterXML.classXML.subClasses.clear();
//   myCurrentCharacterXML.offhand = RPG_CHARACTER_OFFHAND_INVALID;
//
//   return result;
// }
