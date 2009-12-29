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

#include <ace/Log_Msg.h>

RPG_Character_Gender RPG_Character_Gender_Type::post_RPG_Character_Gender_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Gender_Type::post_RPG_Character_Gender_Type"));

  return RPG_Character_GenderHelper::stringToRPG_Character_Gender(post_string());
}

RPG_Character_Race RPG_Character_Race_Type::post_RPG_Character_Race_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Race_Type::post_RPG_Character_Race_Type"));

  return RPG_Character_RaceHelper::stringToRPG_Character_Race(post_string());
}

RPG_Character_MetaClass RPG_Character_MetaClass_Type::post_RPG_Character_MetaClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MetaClass_Type::post_RPG_Character_MetaClass_Type"));

  return RPG_Character_MetaClassHelper::stringToRPG_Character_MetaClass(post_string());
}

RPG_Character_SubClass RPG_Character_SubClass_Type::post_RPG_Character_SubClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SubClass_Type::post_RPG_Character_SubClass_Type"));

  return RPG_Character_SubClassHelper::stringToRPG_Character_SubClass(post_string());
}

RPG_Character_Class_Type::RPG_Character_Class_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Class_Type::RPG_Character_Class_Type"));

  myCurrentClass.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  myCurrentClass.subClass = RPG_CHARACTER_SUBCLASS_INVALID;
}

void RPG_Character_Class_Type::metaClass(const RPG_Character_MetaClass& metaClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Class_Type::metaClass"));

  myCurrentClass.metaClass = metaClass_in;
}

void RPG_Character_Class_Type::subClass(const RPG_Character_SubClass& subClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Class_Type::subClass"));

  myCurrentClass.subClass = subClass_in;
}

RPG_Character_Class RPG_Character_Class_Type::post_RPG_Character_Class_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Class_Type::post_RPG_Character_Class_Type"));

  RPG_Character_Class result = myCurrentClass;

  // clear structure
  myCurrentClass.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  myCurrentClass.subClass = RPG_CHARACTER_SUBCLASS_INVALID;

  return result;
}

RPG_Character_Condition RPG_Character_Condition_Type::post_RPG_Character_Condition_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Condition_Type::post_RPG_Character_Condition_Type"));

  return RPG_Character_ConditionHelper::stringToRPG_Character_Condition(post_string());
}

RPG_Character_Ability RPG_Character_Ability_Type::post_RPG_Character_Ability_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Ability_Type::post_RPG_Character_Ability_Type"));

  return RPG_Character_AbilityHelper::stringToRPG_Character_Ability(post_string());
}

RPG_Character_Size RPG_Character_Size_Type::post_RPG_Character_Size_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Size_Type::post_RPG_Character_Size_Type"));

  return RPG_Character_SizeHelper::stringToRPG_Character_Size(post_string());
}

RPG_Character_Attributes_Type::RPG_Character_Attributes_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::RPG_Character_Attributes_Type"));

  myCurrentAttributes.strength = 0;
  myCurrentAttributes.dexterity = 0;
  myCurrentAttributes.constitution = 0;
  myCurrentAttributes.intelligence = 0;
  myCurrentAttributes.wisdom = 0;
  myCurrentAttributes.charisma = 0;
}

void RPG_Character_Attributes_Type::strength(unsigned char strength_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::strength"));

  myCurrentAttributes.strength = strength_in;
}

void RPG_Character_Attributes_Type::dexterity(unsigned char dexterity_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::dexterity"));

  myCurrentAttributes.dexterity = dexterity_in;
}

void RPG_Character_Attributes_Type::constitution(unsigned char constitution_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::constitution"));

  myCurrentAttributes.constitution = constitution_in;
}

void RPG_Character_Attributes_Type::intelligence(unsigned char intelligence_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::intelligence"));

  myCurrentAttributes.intelligence = intelligence_in;
}

void RPG_Character_Attributes_Type::wisdom(unsigned char wisdom_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::wisdom"));

  myCurrentAttributes.wisdom = wisdom_in;
}

void RPG_Character_Attributes_Type::charisma(unsigned char charisma_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::charisma"));

  myCurrentAttributes.charisma = charisma_in;
}

RPG_Character_Attributes RPG_Character_Attributes_Type::post_RPG_Character_Attributes_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::post_RPG_Character_Attributes_Type"));

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

RPG_Character_Skill RPG_Character_Skill_Type::post_RPG_Character_Skill_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skill_Type::post_RPG_Character_Skill_Type"));

  return RPG_Character_SkillHelper::stringToRPG_Character_Skill(post_string());
}

RPG_Character_SkillValue_Type::RPG_Character_SkillValue_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::RPG_Character_SkillValue_Type"));

  myCurrentSkill.skill = RPG_CHARACTER_SKILL_INVALID;
  myCurrentSkill.rank = 0;
}

void RPG_Character_SkillValue_Type::skill(const RPG_Character_Skill& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::skill"));

  myCurrentSkill.skill = skill_in;
}

void RPG_Character_SkillValue_Type::rank(signed char rank_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::rank"));

  myCurrentSkill.rank = rank_in;
}

RPG_Character_SkillValue RPG_Character_SkillValue_Type::post_RPG_Character_SkillValue_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::post_RPG_Character_SkillValue_Type"));

  RPG_Character_SkillValue result = myCurrentSkill;

  // clear structure
  myCurrentSkill.skill = RPG_CHARACTER_SKILL_INVALID;
  myCurrentSkill.rank = 0;

  return result;
}

RPG_Character_Skills_Type::RPG_Character_Skills_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Type::RPG_Character_Skills_Type"));

  myCurrentSkills.skills.clear();
}

void RPG_Character_Skills_Type::skill(const RPG_Character_SkillValue& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Type::skill"));

  myCurrentSkills.skills.push_back(skill_in);
}

RPG_Character_Skills RPG_Character_Skills_Type::post_RPG_Character_Skills_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Type::post_RPG_Character_Skills_Type"));

  RPG_Character_Skills result = myCurrentSkills;

  // clear structure
  myCurrentSkills.skills.clear();

  return result;
}

RPG_Character_Feat RPG_Character_Feat_Type::post_RPG_Character_Feat_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feat_Type::post_RPG_Character_Feat_Type"));

  return RPG_Character_FeatHelper::stringToRPG_Character_Feat(post_string());
}

RPG_Character_Feats_Type::RPG_Character_Feats_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feats_Type::RPG_Character_Feats_Type"));

  myCurrentFeats.feats.clear();
}

void RPG_Character_Feats_Type::feat(const RPG_Character_Feat& feat_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feats_Type::feat"));

  myCurrentFeats.feats.push_back(feat_in);
}

RPG_Character_Feats RPG_Character_Feats_Type::post_RPG_Character_Feats_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feats_Type::post_RPG_Character_Feats_Type"));

  RPG_Character_Feats result = myCurrentFeats;

  // clear structure
  myCurrentFeats.feats.clear();

  return result;
}

RPG_Character_Terrain RPG_Character_Terrain_Type::post_RPG_Character_Terrain_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Terrain_Type::post_RPG_Character_Terrain_Type"));

  return RPG_Character_TerrainHelper::stringToRPG_Character_Terrain(post_string());
}

RPG_Character_Climate RPG_Character_Climate_Type::post_RPG_Character_Climate_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Climate_Type::post_RPG_Character_Climate_Type"));

  return RPG_Character_ClimateHelper::stringToRPG_Character_Climate(post_string());
}

RPG_Character_Environment_Type::RPG_Character_Environment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Environment_Type::RPG_Character_Environment_Type"));

  myCurrentEnvironment.terrain = RPG_CHARACTER_TERRAIN_INVALID;
  myCurrentEnvironment.climate = RPG_CHARACTER_CLIMATE_INVALID;
}

void RPG_Character_Environment_Type::terrain(const RPG_Character_Terrain& terrain_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Environment_Type::terrain"));

  myCurrentEnvironment.terrain = terrain_in;
}

void RPG_Character_Environment_Type::climate(const RPG_Character_Climate& climate_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Environment_Type::climate"));

  myCurrentEnvironment.climate = climate_in;
}

RPG_Character_Environment RPG_Character_Environment_Type::post_RPG_Character_Environment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Environment_Type::post_RPG_Character_Environment_Type"));

  RPG_Character_Environment result = myCurrentEnvironment;

  // clear structure
  myCurrentEnvironment.terrain = RPG_CHARACTER_TERRAIN_INVALID;
  myCurrentEnvironment.climate = RPG_CHARACTER_CLIMATE_INVALID;

  return result;
}

RPG_Character_AlignmentCivic RPG_Character_AlignmentCivic_Type::post_RPG_Character_AlignmentCivic_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_AlignmentCivic_Type::post_RPG_Character_AlignmentCivic_Type"));

  return RPG_Character_AlignmentCivicHelper::stringToRPG_Character_AlignmentCivic(post_string());
}

RPG_Character_AlignmentEthic RPG_Character_AlignmentEthic_Type::post_RPG_Character_AlignmentEthic_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_AlignmentEthic_Type::post_RPG_Character_AlignmentEthic_Type"));

  return RPG_Character_AlignmentEthicHelper::stringToRPG_Character_AlignmentEthic(post_string());
}

RPG_Character_Alignment_Type::RPG_Character_Alignment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::RPG_Character_Alignment_Type"));

  myCurrentAlignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentAlignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
}

void RPG_Character_Alignment_Type::civic(const RPG_Character_AlignmentCivic& alignmentCivic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::civic"));

  myCurrentAlignment.civic = alignmentCivic_in;
}

void RPG_Character_Alignment_Type::ethic(const RPG_Character_AlignmentEthic& alignmentEthic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::ethic"));

  myCurrentAlignment.ethic = alignmentEthic_in;
}

RPG_Character_Alignment RPG_Character_Alignment_Type::post_RPG_Character_Alignment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::post_RPG_Character_Alignment_Type"));

  RPG_Character_Alignment result = myCurrentAlignment;

  // clear structure
  myCurrentAlignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentAlignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;

  return result;
}

RPG_Character_EquipmentSlot RPG_Character_EquipmentSlot_Type::post_RPG_Character_EquipmentSlot_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_EquipmentSlot_Type::post_RPG_Character_EquipmentSlot_Type"));

  return RPG_Character_EquipmentSlotHelper::stringToRPG_Character_EquipmentSlot(post_string());
}
