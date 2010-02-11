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
#include "rpg_magic_XML_parser.h"

#include <ace/Log_Msg.h>

RPG_Magic_School RPG_Magic_School_Type::post_RPG_Magic_School_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_School_Type::post_RPG_Magic_School_Type"));

  return RPG_Magic_SchoolHelper::stringToRPG_Magic_School(post_string());
}

RPG_Magic_SubSchool RPG_Magic_SubSchool_Type::post_RPG_Magic_SubSchool_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SubSchool_Type::post_RPG_Magic_SubSchool_Type"));

  return RPG_Magic_SubSchoolHelper::stringToRPG_Magic_SubSchool(post_string());
}

RPG_Magic_Descriptor RPG_Magic_Descriptor_Type::post_RPG_Magic_Descriptor_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Descriptor_Type::post_RPG_Magic_Descriptor_Type"));

  return RPG_Magic_DescriptorHelper::stringToRPG_Magic_Descriptor(post_string());
}

RPG_Magic_Domain RPG_Magic_Domain_Type::post_RPG_Magic_Domain_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Domain_Type::post_RPG_Magic_Domain_Type"));

  return RPG_Magic_DomainHelper::stringToRPG_Magic_Domain(post_string());
}

RPG_Magic_SpellType RPG_Magic_SpellType_Type::post_RPG_Magic_SpellType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellType_Type::post_RPG_Magic_SpellType_Type"));

  return RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(post_string());
}

RPG_Magic_AbilityClass RPG_Magic_AbilityClass_Type::post_RPG_Magic_AbilityClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_AbilityClass_Type::post_RPG_Magic_AbilityClass_Type"));

  return RPG_Magic_AbilityClassHelper::stringToRPG_Magic_AbilityClass(post_string());
}

RPG_Magic_AbilityType RPG_Magic_AbilityType_Type::post_RPG_Magic_AbilityType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_AbilityType_Type::post_RPG_Magic_AbilityType_Type"));

  return RPG_Magic_AbilityTypeHelper::stringToRPG_Magic_AbilityType(post_string());
}

RPG_Magic_Spell_Type_Type::RPG_Magic_Spell_Type_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::RPG_Magic_Spell_Type_Type"));

  myCurrentType.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentType.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentType.descriptors.clear();
}

void RPG_Magic_Spell_Type_Type::school(const RPG_Magic_School& school_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::school"));

  myCurrentType.school = school_in;
}

void RPG_Magic_Spell_Type_Type::subSchool(const RPG_Magic_SubSchool& subSchool_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::subSchool"));

  myCurrentType.subSchool = subSchool_in;
}

void RPG_Magic_Spell_Type_Type::descriptor(const RPG_Magic_Descriptor& descriptor_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::descriptor"));

  myCurrentType.descriptors.push_back(descriptor_in);
}

RPG_Magic_Spell_Type RPG_Magic_Spell_Type_Type::post_RPG_Magic_Spell_Type_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::post_RPG_Magic_Spell_Type_Type"));

  RPG_Magic_Spell_Type result = myCurrentType;

  // clear structure
  myCurrentType.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentType.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentType.descriptors.clear();

  return result;
}

RPG_Magic_Spell_Effect RPG_Magic_Spell_Effect_Type::post_RPG_Magic_Spell_Effect_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Effect_Type::post_RPG_Magic_Spell_Effect_Type"));

  return RPG_Magic_Spell_EffectHelper::stringToRPG_Magic_Spell_Effect(post_string());
}

RPG_Magic_Spell_Range_Type::RPG_Magic_Spell_Range_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::RPG_Magic_Spell_Range_Type"));

  myCurrentRange.maxRange = 0;
  myCurrentRange.increment = 0;
  myCurrentRange.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentRange.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;
}

void RPG_Magic_Spell_Range_Type::maxRange(unsigned char maxRange_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::maxRange"));

  myCurrentRange.maxRange = maxRange_in;
}

void RPG_Magic_Spell_Range_Type::increment(unsigned char increment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::increment"));

  myCurrentRange.increment = increment_in;
}

void RPG_Magic_Spell_Range_Type::area(const RPG_Common_AreaOfEffect& area_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::area"));

  myCurrentRange.area = area_in;
}

void RPG_Magic_Spell_Range_Type::effect(const RPG_Magic_Spell_Effect& effect_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::effect"));

  myCurrentRange.effect = effect_in;
}

RPG_Magic_Spell_Range RPG_Magic_Spell_Range_Type::post_RPG_Magic_Spell_Range_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::post_RPG_Magic_Spell_Range_Type"));

  RPG_Magic_Spell_Range result = myCurrentRange;

  // clear structure
  myCurrentRange.maxRange = 0;
  myCurrentRange.increment = 0;
  myCurrentRange.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentRange.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;

  return result;
}

RPG_Magic_Spell_Duration RPG_Magic_Spell_Duration_Type::post_RPG_Magic_Spell_Duration_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Duration_Type::post_RPG_Magic_Spell_Duration_Type"));

  return RPG_Magic_Spell_DurationHelper::stringToRPG_Magic_Spell_Duration(post_string());
}

RPG_Magic_Spell_DurationProperties_Type::RPG_Magic_Spell_DurationProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::RPG_Magic_Spell_DurationProperties_Type"));

  myCurrentProperties.type = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration = 0;
  myCurrentProperties.period.numDice = 0;
  myCurrentProperties.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.period.modifier = 0;
  myCurrentProperties.dismissible = false;
}

void RPG_Magic_Spell_DurationProperties_Type::type(const RPG_Magic_Spell_Duration& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_Spell_DurationProperties_Type::duration(unsigned int duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_Spell_DurationProperties_Type::period(const RPG_Dice_Roll& period_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::period"));

  myCurrentProperties.period = period_in;
}

void RPG_Magic_Spell_DurationProperties_Type::dismissible(bool dismissible_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::dismissible"));

  myCurrentProperties.dismissible = dismissible_in;
}

RPG_Magic_Spell_DurationProperties RPG_Magic_Spell_DurationProperties_Type::post_RPG_Magic_Spell_DurationProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::post_RPG_Magic_Spell_DurationProperties_Type"));

  RPG_Magic_Spell_DurationProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.type = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration = 0;
  myCurrentProperties.period.numDice = 0;
  myCurrentProperties.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.period.modifier = 0;
  myCurrentProperties.dismissible = false;

  return result;
}

RPG_Magic_Spell_Properties_Type::RPG_Magic_Spell_Properties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::RPG_Magic_Spell_Properties_Type"));

  myCurrentProperties.type.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.type.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentProperties.type.descriptors.clear();
  myCurrentProperties.name = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.level = 0;
  myCurrentProperties.casterClasses.clear();
  myCurrentProperties.domain = RPG_MAGIC_DOMAIN_INVALID;
  myCurrentProperties.domainLevel = 0;
  myCurrentProperties.cost = 0;
  myCurrentProperties.action = RPG_COMMON_ACTIONTYPE_INVALID;
  myCurrentProperties.range.maxRange = 0;
  myCurrentProperties.range.increment = 0;
  myCurrentProperties.range.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentProperties.range.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;
  myCurrentProperties.duration.type = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration.duration = 0;
  myCurrentProperties.duration.period.numDice = 0;
  myCurrentProperties.duration.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.period.modifier = 0;
  myCurrentProperties.duration.dismissible = false;
}

void RPG_Magic_Spell_Properties_Type::type(const RPG_Magic_Spell_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_Spell_Properties_Type::name(const RPG_Magic_SpellType& name_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::name"));

  myCurrentProperties.name = name_in;
}

void RPG_Magic_Spell_Properties_Type::level(unsigned char level_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::level"));

  myCurrentProperties.level = level_in;
}

void RPG_Magic_Spell_Properties_Type::casterClass(const RPG_Common_SubClass& casterClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::casterClass"));

  myCurrentProperties.casterClasses.push_back(casterClass_in);
}

void RPG_Magic_Spell_Properties_Type::domain(const RPG_Magic_Domain& domain_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::domain"));

  myCurrentProperties.domain = domain_in;
}

void RPG_Magic_Spell_Properties_Type::domainLevel(unsigned char domainLevel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::domainLevel"));

  myCurrentProperties.domainLevel = domainLevel_in;
}

void RPG_Magic_Spell_Properties_Type::cost(unsigned char cost_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::cost"));

  myCurrentProperties.cost = cost_in;
}

void RPG_Magic_Spell_Properties_Type::action(const RPG_Common_ActionType& action_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::action"));

  myCurrentProperties.action = action_in;
}

void RPG_Magic_Spell_Properties_Type::range(const RPG_Magic_Spell_Range& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::range"));

  myCurrentProperties.range = range_in;
}

void RPG_Magic_Spell_Properties_Type::duration(const RPG_Magic_Spell_DurationProperties& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

RPG_Magic_Spell_Properties RPG_Magic_Spell_Properties_Type::post_RPG_Magic_Spell_Properties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Properties_Type::post_RPG_Magic_Spell_Properties_Type"));

  RPG_Magic_Spell_Properties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.type.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.type.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentProperties.type.descriptors.clear();
  myCurrentProperties.name = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.level = 0;
  myCurrentProperties.casterClasses.clear();
  myCurrentProperties.domain = RPG_MAGIC_DOMAIN_INVALID;
  myCurrentProperties.domainLevel = 0;
  myCurrentProperties.cost = 0;
  myCurrentProperties.action = RPG_COMMON_ACTIONTYPE_INVALID;
  myCurrentProperties.range.maxRange = 0;
  myCurrentProperties.range.increment = 0;
  myCurrentProperties.range.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentProperties.range.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;
  myCurrentProperties.duration.type = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration.duration = 0;
  myCurrentProperties.duration.period.numDice = 0;
  myCurrentProperties.duration.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.period.modifier = 0;
  myCurrentProperties.duration.dismissible = false;

  return result;
}

RPG_Magic_SpellLikeProperties_Type::RPG_Magic_SpellLikeProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::RPG_Magic_SpellLikeProperties_Type"));

  myCurrentProperties.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.casterLevel = 0;
  myCurrentProperties.duration.activation = 0;
  myCurrentProperties.duration.interval = 0;
  myCurrentProperties.duration.total = 0;
  myCurrentProperties.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.save.difficultyClass = 0;
}

void RPG_Magic_SpellLikeProperties_Type::type(const RPG_Magic_SpellType& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_SpellLikeProperties_Type::casterLevel(unsigned char casterLevel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::casterLevel"));

  myCurrentProperties.casterLevel = casterLevel_in;
}

void RPG_Magic_SpellLikeProperties_Type::duration(const RPG_Common_Duration& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_SpellLikeProperties_Type::save(const RPG_Common_SavingThrowCheck& save_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::save"));

  myCurrentProperties.save = save_in;
}

RPG_Magic_SpellLikeProperties RPG_Magic_SpellLikeProperties_Type::post_RPG_Magic_SpellLikeProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::post_RPG_Magic_SpellLikeProperties_Type"));

  RPG_Magic_SpellLikeProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.casterLevel = 0;
  myCurrentProperties.duration.activation = 0;
  myCurrentProperties.duration.interval = 0;
  myCurrentProperties.duration.total = 0;
  myCurrentProperties.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.save.difficultyClass = 0;

  return result;
}
