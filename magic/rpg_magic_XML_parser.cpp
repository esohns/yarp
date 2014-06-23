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

#include "rpg_magic_XML_parser.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Magic_School RPG_Magic_School_Type::post_RPG_Magic_School_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_School_Type::post_RPG_Magic_School_Type"));

  return RPG_Magic_SchoolHelper::stringToRPG_Magic_School(post_string());
}

RPG_Magic_SubSchool RPG_Magic_SubSchool_Type::post_RPG_Magic_SubSchool_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SubSchool_Type::post_RPG_Magic_SubSchool_Type"));

  return RPG_Magic_SubSchoolHelper::stringToRPG_Magic_SubSchool(post_string());
}

RPG_Magic_Descriptor RPG_Magic_Descriptor_Type::post_RPG_Magic_Descriptor_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Descriptor_Type::post_RPG_Magic_Descriptor_Type"));

  return RPG_Magic_DescriptorHelper::stringToRPG_Magic_Descriptor(post_string());
}

RPG_Magic_Domain RPG_Magic_Domain_Type::post_RPG_Magic_Domain_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Domain_Type::post_RPG_Magic_Domain_Type"));

  return RPG_Magic_DomainHelper::stringToRPG_Magic_Domain(post_string());
}

RPG_Magic_SpellType RPG_Magic_SpellType_Type::post_RPG_Magic_SpellType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellType_Type::post_RPG_Magic_SpellType_Type"));

  return RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(post_string());
}

RPG_Magic_AbilityClass RPG_Magic_AbilityClass_Type::post_RPG_Magic_AbilityClass_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_AbilityClass_Type::post_RPG_Magic_AbilityClass_Type"));

  return RPG_Magic_AbilityClassHelper::stringToRPG_Magic_AbilityClass(post_string());
}

RPG_Magic_AbilityType RPG_Magic_AbilityType_Type::post_RPG_Magic_AbilityType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_AbilityType_Type::post_RPG_Magic_AbilityType_Type"));

  return RPG_Magic_AbilityTypeHelper::stringToRPG_Magic_AbilityType(post_string());
}

RPG_Magic_Spell_Type_Type::RPG_Magic_Spell_Type_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::RPG_Magic_Spell_Type_Type"));

  myCurrentType.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentType.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentType.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentType.descriptors.clear();
  myCurrentType.counterSpell = RPG_MAGIC_SPELLTYPE_INVALID;
}

void RPG_Magic_Spell_Type_Type::type(const RPG_Magic_SpellType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::type"));

  myCurrentType.type = type_in;
}

void RPG_Magic_Spell_Type_Type::school(const RPG_Magic_School& school_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::school"));

  myCurrentType.school = school_in;
}

void RPG_Magic_Spell_Type_Type::subSchool(const RPG_Magic_SubSchool& subSchool_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::subSchool"));

  myCurrentType.subSchool = subSchool_in;
}

void RPG_Magic_Spell_Type_Type::descriptor(const RPG_Magic_Descriptor& descriptor_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::descriptor"));

  myCurrentType.descriptors.push_back(descriptor_in);
}

void RPG_Magic_Spell_Type_Type::counterSpell(const RPG_Magic_SpellType& counterSpell_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::counterSpell"));

  myCurrentType.counterSpell = counterSpell_in;
}

RPG_Magic_Spell_Type RPG_Magic_Spell_Type_Type::post_RPG_Magic_Spell_Type_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::post_RPG_Magic_Spell_Type_Type"));

  RPG_Magic_Spell_Type result = myCurrentType;

  // clear structure
  myCurrentType.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentType.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentType.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentType.descriptors.clear();
  myCurrentType.counterSpell = RPG_MAGIC_SPELLTYPE_INVALID;

  return result;
}

RPG_Magic_CasterClassUnion_Type::RPG_Magic_CasterClassUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CasterClassUnion_Type::RPG_Magic_CasterClassUnion_Type"));

  myCurrentCasterClassUnion.discriminator = RPG_Magic_CasterClassUnion::INVALID;
  myCurrentCasterClassUnion.subclass = RPG_COMMON_SUBCLASS_INVALID;
}

void RPG_Magic_CasterClassUnion_Type::_characters(const ::xml_schema::ro_string& casterClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CasterClassUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_SubClass --> "SUBCLASS_xxx"
  // - RPG_Magic_Domain --> "DOMAIN_xxx"
  std::string type = casterClass_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("SUBCLASS_")) == 0)
  {
    myCurrentCasterClassUnion.subclass = RPG_Common_SubClassHelper::stringToRPG_Common_SubClass(casterClass_in);
    myCurrentCasterClassUnion.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  } // end IF
  else
  {
    myCurrentCasterClassUnion.domain = RPG_Magic_DomainHelper::stringToRPG_Magic_Domain(casterClass_in);
    // *PORTABILITY*: "DOMAIN" seems to be a constant (see math.h)
    // --> provide a (temporary) workaround here...
#if defined __GNUC__ || defined _MSC_VER
#pragma message("applying quirk code for this compiler...")
    myCurrentCasterClassUnion.discriminator = RPG_Magic_CasterClassUnion::__QUIRK__DOMAIN;
#else
#pragma message("re-check code for this compiler")
    myCurrentCasterClassUnion.discriminator = RPG_Magic_CasterClassUnion::DOMAIN;
#endif
  } // end ELSE
}

RPG_Magic_CasterClassUnion RPG_Magic_CasterClassUnion_Type::post_RPG_Magic_CasterClassUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CasterClassUnion_Type::post_RPG_Magic_CasterClassUnion_Type"));

  RPG_Magic_CasterClassUnion result = myCurrentCasterClassUnion;

  // clear structure
  myCurrentCasterClassUnion.discriminator = RPG_Magic_CasterClassUnion::INVALID;
  myCurrentCasterClassUnion.subclass = RPG_COMMON_SUBCLASS_INVALID;

  return result;
}

RPG_Magic_Spell_Level_Type::RPG_Magic_Spell_Level_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Level_Type::RPG_Magic_Spell_Level_Type"));

  myCurrentSpellLevel.casterClass.discriminator = RPG_Magic_CasterClassUnion::INVALID;
  myCurrentSpellLevel.casterClass.subclass = RPG_COMMON_SUBCLASS_INVALID;
  myCurrentSpellLevel.level = 0;
}

void RPG_Magic_Spell_Level_Type::casterClass(const RPG_Magic_CasterClassUnion& casterClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Level_Type::casterClass"));

  myCurrentSpellLevel.casterClass = casterClass_in;
}

void RPG_Magic_Spell_Level_Type::level(unsigned char level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Level_Type::level"));

  myCurrentSpellLevel.level = level_in;
}

RPG_Magic_Spell_Level RPG_Magic_Spell_Level_Type::post_RPG_Magic_Spell_Level_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Level_Type::post_RPG_Magic_Spell_Level_Type"));

  RPG_Magic_Spell_Level result = myCurrentSpellLevel;

  // clear structure
  myCurrentSpellLevel.casterClass.discriminator = RPG_Magic_CasterClassUnion::INVALID;
  myCurrentSpellLevel.casterClass.subclass = RPG_COMMON_SUBCLASS_INVALID;
  myCurrentSpellLevel.level = 0;

  return result;
}

RPG_Magic_Spell_CastingTime_Type::RPG_Magic_Spell_CastingTime_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_CastingTime_Type::RPG_Magic_Spell_CastingTime_Type"));

  myCurrentCastingTime.rounds = 0;
  myCurrentCastingTime.action = ACTION_STANDARD;
}

void RPG_Magic_Spell_CastingTime_Type::rounds(unsigned int rounds_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_CastingTime_Type::rounds"));

  myCurrentCastingTime.rounds = rounds_in;
}

void RPG_Magic_Spell_CastingTime_Type::action(const RPG_Common_ActionType& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_CastingTime_Type::action"));

  myCurrentCastingTime.action = action_in;
}

RPG_Magic_Spell_CastingTime RPG_Magic_Spell_CastingTime_Type::post_RPG_Magic_Spell_CastingTime_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_CastingTime_Type::post_RPG_Magic_Spell_CastingTime_Type"));

  RPG_Magic_Spell_CastingTime result = myCurrentCastingTime;

  // clear structure
  myCurrentCastingTime.rounds = 0;
  myCurrentCastingTime.action = ACTION_STANDARD;

  return result;
}

RPG_Magic_Spell_RangeEffect RPG_Magic_Spell_RangeEffect_Type::post_RPG_Magic_Spell_RangeEffect_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_RangeEffect_Type::post_RPG_Magic_Spell_RangeEffect_Type"));

  return RPG_Magic_Spell_RangeEffectHelper::stringToRPG_Magic_Spell_RangeEffect(post_string());
}

RPG_Magic_Spell_RangeProperties_Type::RPG_Magic_Spell_RangeProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_RangeProperties_Type::RPG_Magic_Spell_RangeProperties_Type"));

  myCurrentProperties.maximum = 0;
  myCurrentProperties.increment = 0;
  myCurrentProperties.effect = RPG_MAGIC_SPELL_RANGEEFFECT_INVALID;
}

void RPG_Magic_Spell_RangeProperties_Type::maximum(unsigned int max_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_RangeProperties_Type::maximum"));

  myCurrentProperties.maximum = max_in;
}

void RPG_Magic_Spell_RangeProperties_Type::increment(unsigned int increment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_RangeProperties_Type::increment"));

  myCurrentProperties.increment = increment_in;
}

void RPG_Magic_Spell_RangeProperties_Type::effect(const RPG_Magic_Spell_RangeEffect& effect_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_RangeProperties_Type::effect"));

  myCurrentProperties.effect = effect_in;
}

RPG_Magic_Spell_RangeProperties RPG_Magic_Spell_RangeProperties_Type::post_RPG_Magic_Spell_RangeProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_RangeProperties_Type::post_RPG_Magic_Spell_RangeProperties_Type"));

  RPG_Magic_Spell_RangeProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.maximum = 0;
  myCurrentProperties.increment = 0;
  myCurrentProperties.effect = RPG_MAGIC_SPELL_RANGEEFFECT_INVALID;

  return result;
}

RPG_Magic_Spell_Target RPG_Magic_Spell_Target_Type::post_RPG_Magic_Spell_Target_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Target_Type::post_RPG_Magic_Spell_Target_Type"));

  return RPG_Magic_Spell_TargetHelper::stringToRPG_Magic_Spell_Target(post_string());
}

RPG_Magic_Spell_AreaOfEffect RPG_Magic_Spell_AreaOfEffect_Type::post_RPG_Magic_Spell_AreaOfEffect_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_AreaOfEffect_Type::post_RPG_Magic_Spell_AreaOfEffect_Type"));

  return RPG_Magic_Spell_AreaOfEffectHelper::stringToRPG_Magic_Spell_AreaOfEffect(post_string());
}

RPG_Magic_Spell_TargetProperties_Type::RPG_Magic_Spell_TargetProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::RPG_Magic_Spell_TargetProperties_Type"));

  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.effect = RPG_MAGIC_SPELL_AREAOFEFFECT_INVALID;
  myCurrentProperties.shape = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentProperties.radius = 0;
  myCurrentProperties.height = 0;
  myCurrentProperties.target = RPG_MAGIC_SPELL_TARGET_INVALID;
  myCurrentProperties.shapeable = false;
  myCurrentProperties.rangeIsInHD = false;
  myCurrentProperties.incrementIsReciprocal = false;
}

void RPG_Magic_Spell_TargetProperties_Type::base(const RPG_Common_Amount& amount_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::base"));

  myCurrentProperties.base = amount_in;
}

void RPG_Magic_Spell_TargetProperties_Type::levelIncrement(unsigned char levelIncrement_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::levelIncrement"));

  myCurrentProperties.levelIncrement = levelIncrement_in;
}

void RPG_Magic_Spell_TargetProperties_Type::levelIncrementMax(unsigned char levelIncrementMax_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::levelIncrementMax"));

  myCurrentProperties.levelIncrementMax = levelIncrementMax_in;
}

void RPG_Magic_Spell_TargetProperties_Type::radius(unsigned int radius_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::radius"));

  myCurrentProperties.radius = radius_in;
}

void RPG_Magic_Spell_TargetProperties_Type::height(unsigned int height_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::height"));

  myCurrentProperties.height = height_in;
}

void RPG_Magic_Spell_TargetProperties_Type::effect(const RPG_Magic_Spell_AreaOfEffect& effect_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::effect"));

  myCurrentProperties.effect = effect_in;
}

void RPG_Magic_Spell_TargetProperties_Type::shape(const RPG_Common_AreaOfEffect& shape_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::shape"));

  myCurrentProperties.shape = shape_in;
}

void RPG_Magic_Spell_TargetProperties_Type::target(const RPG_Magic_Spell_Target& target_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::target"));

  myCurrentProperties.target = target_in;
}

void RPG_Magic_Spell_TargetProperties_Type::shapeable(bool shapeable_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::shapeable"));

  myCurrentProperties.shapeable = shapeable_in;
}

void RPG_Magic_Spell_TargetProperties_Type::rangeIsInHD(bool rangeIsInHD_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::rangeIsInHD"));

  myCurrentProperties.rangeIsInHD = rangeIsInHD_in;
}

void RPG_Magic_Spell_TargetProperties_Type::incrementIsReciprocal(bool incrementIsReciprocal_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::incrementIsReciprocal"));

  myCurrentProperties.incrementIsReciprocal = incrementIsReciprocal_in;
}

RPG_Magic_Spell_TargetProperties RPG_Magic_Spell_TargetProperties_Type::post_RPG_Magic_Spell_TargetProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_TargetProperties_Type::post_RPG_Magic_Spell_TargetProperties_Type"));

  RPG_Magic_Spell_TargetProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.effect = RPG_MAGIC_SPELL_AREAOFEFFECT_INVALID;
  myCurrentProperties.shape = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentProperties.radius = 0;
  myCurrentProperties.height = 0;
  myCurrentProperties.target = RPG_MAGIC_SPELL_TARGET_INVALID;
  myCurrentProperties.shapeable = false;
  myCurrentProperties.rangeIsInHD = false;
  myCurrentProperties.incrementIsReciprocal = false;

  return result;
}

RPG_Magic_Spell_Duration RPG_Magic_Spell_Duration_Type::post_RPG_Magic_Spell_Duration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Duration_Type::post_RPG_Magic_Spell_Duration_Type"));

  return RPG_Magic_Spell_DurationHelper::stringToRPG_Magic_Spell_Duration(post_string());
}

RPG_Magic_Spell_DurationProperties_Type::RPG_Magic_Spell_DurationProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::RPG_Magic_Spell_DurationProperties_Type"));

  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.reciprocalIncrement = 0;
  myCurrentProperties.duration = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.dismissible = false;
}

void RPG_Magic_Spell_DurationProperties_Type::base(const RPG_Common_Amount& amount_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::base"));

  myCurrentProperties.base = amount_in;
}

void RPG_Magic_Spell_DurationProperties_Type::levelIncrement(signed char levelIncrement_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::levelIncrement"));

  myCurrentProperties.levelIncrement = levelIncrement_in;
}

void RPG_Magic_Spell_DurationProperties_Type::levelIncrementMax(unsigned char levelIncrementMax_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::levelIncrementMax"));

  myCurrentProperties.levelIncrementMax = levelIncrementMax_in;
}

void RPG_Magic_Spell_DurationProperties_Type::reciprocalIncrement(unsigned char increment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::reciprocalIncrement"));

  myCurrentProperties.reciprocalIncrement = increment_in;
}

RPG_Common_EffectDuration RPG_Magic_Spell_DurationProperties_Type::post_RPG_Common_EffectDuration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::post_RPG_Common_EffectDuration_Type"));

  RPG_Common_EffectDuration result = myCurrentProperties;

  return result;
}

void RPG_Magic_Spell_DurationProperties_Type::duration(const RPG_Magic_Spell_Duration& duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_Spell_DurationProperties_Type::dismissible(bool dismissible_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::dismissible"));

  myCurrentProperties.dismissible = dismissible_in;
}

RPG_Magic_Spell_DurationProperties RPG_Magic_Spell_DurationProperties_Type::post_RPG_Magic_Spell_DurationProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::post_RPG_Magic_Spell_DurationProperties_Type"));

  RPG_Magic_Spell_DurationProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.reciprocalIncrement = 0;
  myCurrentProperties.duration = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.dismissible = false;

  return result;
}

RPG_Magic_Spell_Precondition RPG_Magic_Spell_Precondition_Type::post_RPG_Magic_Spell_Precondition_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Precondition_Type::post_RPG_Magic_Spell_Precondition_Type"));

  return RPG_Magic_Spell_PreconditionHelper::stringToRPG_Magic_Spell_Precondition(post_string());
}

RPG_Magic_Spell_PreconditionProperties_Type::RPG_Magic_Spell_PreconditionProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::RPG_Magic_Spell_PreconditionProperties_Type"));

  myCurrentProperties.type = RPG_MAGIC_SPELL_PRECONDITION_INVALID;
  myCurrentProperties.value = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentProperties.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  myCurrentProperties.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.condition = RPG_COMMON_CONDITION_INVALID;
  myCurrentProperties.creatures.clear();
  myCurrentProperties.size = RPG_COMMON_SIZE_INVALID;
  myCurrentProperties.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  myCurrentProperties.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  myCurrentProperties.reverse = false;
  myCurrentProperties.baseIsCasterLevel = false;
}

void RPG_Magic_Spell_PreconditionProperties_Type::type(const RPG_Magic_Spell_Precondition& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::value(int value_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::value"));

  myCurrentProperties.value = value_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::levelIncrement(unsigned char levelIncrement_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::levelIncrement"));

  myCurrentProperties.levelIncrement = levelIncrement_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::levelIncrementMax(unsigned char levelIncrementMax_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::levelIncrementMax"));

  myCurrentProperties.levelIncrementMax = levelIncrementMax_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::alignment(const RPG_Character_Alignment& alignment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::alignment"));

  myCurrentProperties.alignment = alignment_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::attribute(const RPG_Common_Attribute& attribute_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::attribute"));

  myCurrentProperties.attribute = attribute_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::condition(const RPG_Common_Condition& condition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::condition"));

  myCurrentProperties.condition = condition_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::creature(const RPG_Common_CreatureType& creature_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::creature"));

  myCurrentProperties.creatures.push_back(creature_in);
}

void RPG_Magic_Spell_PreconditionProperties_Type::size(const RPG_Common_Size& size_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::size"));

  myCurrentProperties.size = size_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::environment(const RPG_Common_Environment& environment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::environment"));

  myCurrentProperties.environment = environment_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::reverse(bool reverse_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::reverse"));

  myCurrentProperties.reverse = reverse_in;
}

void RPG_Magic_Spell_PreconditionProperties_Type::baseIsCasterLevel(bool baseIsCasterLevel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::baseIsCasterLevel"));

  myCurrentProperties.baseIsCasterLevel = baseIsCasterLevel_in;
}

RPG_Magic_Spell_PreconditionProperties RPG_Magic_Spell_PreconditionProperties_Type::post_RPG_Magic_Spell_PreconditionProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PreconditionProperties_Type::post_RPG_Magic_Spell_PreconditionProperties_Type"));

  RPG_Magic_Spell_PreconditionProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.type = RPG_MAGIC_SPELL_PRECONDITION_INVALID;
  myCurrentProperties.value = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentProperties.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  myCurrentProperties.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.condition = RPG_COMMON_CONDITION_INVALID;
  myCurrentProperties.creatures.clear();
  myCurrentProperties.size = RPG_COMMON_SIZE_INVALID;
  myCurrentProperties.environment.terrain = RPG_COMMON_TERRAIN_INVALID;
  myCurrentProperties.environment.climate = RPG_COMMON_CLIMATE_INVALID;
  myCurrentProperties.reverse = false;
  myCurrentProperties.baseIsCasterLevel = false;

  return result;
}

RPG_Magic_Spell_Effect RPG_Magic_Spell_Effect_Type::post_RPG_Magic_Spell_Effect_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_Effect_Type::post_RPG_Magic_Spell_Effect_Type"));

  return RPG_Magic_Spell_EffectHelper::stringToRPG_Magic_Spell_Effect(post_string());
}

RPG_Magic_Spell_DamageTypeUnion_Type::RPG_Magic_Spell_DamageTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DamageTypeUnion_Type::RPG_Magic_Spell_DamageTypeUnion_Type"));

  myCurrentDamageTypeUnion.discriminator = RPG_Magic_Spell_DamageTypeUnion::INVALID;
  myCurrentDamageTypeUnion.physicaldamagetype = RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;
}

void RPG_Magic_Spell_DamageTypeUnion_Type::_characters(const ::xml_schema::ro_string& damageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DamageTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_PhysicalDamageType --> "PHYSICALDAMAGE_xxx"
  // - RPG_Magic_Descriptor_Type --> "DESCRIPTOR_xxx"
  std::string type = damageType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_")) == 0)
  {
    myCurrentDamageTypeUnion.physicaldamagetype = RPG_Common_PhysicalDamageTypeHelper::stringToRPG_Common_PhysicalDamageType(type);
    myCurrentDamageTypeUnion.discriminator = RPG_Magic_Spell_DamageTypeUnion::PHYSICALDAMAGETYPE;
  } // end IF
  else
  {
    myCurrentDamageTypeUnion.descriptor = RPG_Magic_DescriptorHelper::stringToRPG_Magic_Descriptor(type);
    myCurrentDamageTypeUnion.discriminator = RPG_Magic_Spell_DamageTypeUnion::DESCRIPTOR;
  } // end IF
}

RPG_Magic_Spell_DamageTypeUnion RPG_Magic_Spell_DamageTypeUnion_Type::post_RPG_Magic_Spell_DamageTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_DamageTypeUnion_Type::post_RPG_Magic_Spell_DamageTypeUnion_Type"));

  RPG_Magic_Spell_DamageTypeUnion result = myCurrentDamageTypeUnion;

  // clear structure
  myCurrentDamageTypeUnion.discriminator = RPG_Magic_Spell_DamageTypeUnion::INVALID;
  myCurrentDamageTypeUnion.physicaldamagetype = RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;

  return result;
}

RPG_Magic_CheckTypeUnion_Type::RPG_Magic_CheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CheckTypeUnion_Type::RPG_Magic_CheckTypeUnion_Type"));

  myCurrentCheckTypeUnion.discriminator = RPG_Magic_CheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.checktype = RPG_COMMON_CHECKTYPE_INVALID;
}

void RPG_Magic_CheckTypeUnion_Type::_characters(const ::xml_schema::ro_string& checkType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CheckTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_Skill_Type --> "SKILL_xxx"
  // - RPG_Common_Attribute_Type --> "ATTRIBUTE_xxx"
  // - RPG_Common_BaseCheckTypeUnion --> "CHECK_xxx" || "SAVINGTHROW_xxx"
  std::string type = checkType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("SKILL_")) == 0)
  {
    myCurrentCheckTypeUnion.skill = RPG_Common_SkillHelper::stringToRPG_Common_Skill(checkType_in);
    myCurrentCheckTypeUnion.discriminator = RPG_Magic_CheckTypeUnion::SKILL;
  } // end IF
  else if (type.find(ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_")) == 0)
  {
    myCurrentCheckTypeUnion.attribute = RPG_Common_AttributeHelper::stringToRPG_Common_Attribute(checkType_in);
    myCurrentCheckTypeUnion.discriminator = RPG_Magic_CheckTypeUnion::ATTRIBUTE;
  } // end IF
  else if (type.find(ACE_TEXT_ALWAYS_CHAR("CHECK_")) == 0)
  {
    myCurrentCheckTypeUnion.discriminator = RPG_Magic_CheckTypeUnion::BASECHECKTYPEUNION;
    myCurrentCheckTypeUnion.basechecktypeunion.checktype = RPG_Common_CheckTypeHelper::stringToRPG_Common_CheckType(checkType_in);
    myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::CHECKTYPE;
  } // end IF
  else
  {
    myCurrentCheckTypeUnion.discriminator = RPG_Magic_CheckTypeUnion::BASECHECKTYPEUNION;
    myCurrentCheckTypeUnion.basechecktypeunion.savingthrow = RPG_Common_SavingThrowHelper::stringToRPG_Common_SavingThrow(checkType_in);
    myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::SAVINGTHROW;
  } // end ELSE
}

RPG_Magic_CheckTypeUnion RPG_Magic_CheckTypeUnion_Type::post_RPG_Magic_CheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CheckTypeUnion_Type::post_RPG_Magic_CheckTypeUnion_Type"));

  RPG_Magic_CheckTypeUnion result = myCurrentCheckTypeUnion;

  // clear structure
  myCurrentCheckTypeUnion.discriminator = RPG_Magic_CheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.checktype = RPG_COMMON_CHECKTYPE_INVALID;

  return result;
}

RPG_Magic_Check_Type::RPG_Magic_Check_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::RPG_Magic_Check_Type"));

  myCurrentCheck.type.discriminator = RPG_Magic_CheckTypeUnion::INVALID;
  myCurrentCheck.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheck.difficultyClass = 0;
  myCurrentCheck.modifier = 0;
  myCurrentCheck.levelIncrementMax = 0;
  myCurrentCheck.baseIsCasterLevel = false;
}

void RPG_Magic_Check_Type::type(const RPG_Magic_CheckTypeUnion& checkUnion_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::type"));

  myCurrentCheck.type = checkUnion_in;
}

void RPG_Magic_Check_Type::difficultyClass(unsigned char difficultyClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::difficultyClass"));

  myCurrentCheck.difficultyClass = difficultyClass_in;
}

void RPG_Magic_Check_Type::modifier(signed char modifier_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::modifier"));

  myCurrentCheck.modifier = modifier_in;
}

void RPG_Magic_Check_Type::levelIncrementMax(unsigned char incrementMax_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::levelIncrementMax"));

  myCurrentCheck.levelIncrementMax = incrementMax_in;
}

void RPG_Magic_Check_Type::baseIsCasterLevel(bool isCasterLevel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::baseIsCasterLevel"));

  myCurrentCheck.baseIsCasterLevel = isCasterLevel_in;
}

RPG_Magic_Check RPG_Magic_Check_Type::post_RPG_Magic_Check_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Check_Type::post_RPG_Magic_Check_Type"));

  RPG_Magic_Check result = myCurrentCheck;

  // clear structure
  myCurrentCheck.type.discriminator = RPG_Magic_CheckTypeUnion::INVALID;
  myCurrentCheck.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheck.difficultyClass = 0;
  myCurrentCheck.modifier = 0;
  myCurrentCheck.levelIncrementMax = 0;
  myCurrentCheck.baseIsCasterLevel = false;

  return result;
}

RPG_Magic_CounterMeasure_Type::RPG_Magic_CounterMeasure_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CounterMeasure_Type::RPG_Magic_CounterMeasure_Type"));

  myCurrentCounterMeasure.type = RPG_COMMON_COUNTERMEASURE_INVALID;
  myCurrentCounterMeasure.check.type.discriminator = RPG_Magic_CheckTypeUnion::INVALID;
  myCurrentCounterMeasure.check.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCounterMeasure.check.difficultyClass = 0;
  myCurrentCounterMeasure.spells.clear();
  myCurrentCounterMeasure.reduction = RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;
}

void RPG_Magic_CounterMeasure_Type::type(const RPG_Common_CounterMeasure& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CounterMeasure_Type::type"));

  myCurrentCounterMeasure.type = type_in;
}

void RPG_Magic_CounterMeasure_Type::check(const RPG_Magic_Check& check_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CounterMeasure_Type::check"));

  myCurrentCounterMeasure.check = check_in;
}

void RPG_Magic_CounterMeasure_Type::spell(const RPG_Magic_SpellType& spell_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CounterMeasure_Type::spell"));

  myCurrentCounterMeasure.spells.push_back(spell_in);
}

void RPG_Magic_CounterMeasure_Type::reduction(const RPG_Common_SaveReductionType& reduction_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CounterMeasure_Type::reduction"));

  myCurrentCounterMeasure.reduction = reduction_in;
}

RPG_Magic_CounterMeasure RPG_Magic_CounterMeasure_Type::post_RPG_Magic_CounterMeasure_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_CounterMeasure_Type::post_RPG_Magic_CounterMeasure_Type"));

  RPG_Magic_CounterMeasure result = myCurrentCounterMeasure;

  // clear structure
  myCurrentCounterMeasure.type = RPG_COMMON_COUNTERMEASURE_INVALID;
  myCurrentCounterMeasure.check.type.discriminator = RPG_Magic_CheckTypeUnion::INVALID;
  myCurrentCounterMeasure.check.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCounterMeasure.check.difficultyClass = 0;
  myCurrentCounterMeasure.spells.clear();
  myCurrentCounterMeasure.reduction = RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;

  return result;
}

RPG_Magic_Spell_EffectProperties_Type::RPG_Magic_Spell_EffectProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::RPG_Magic_Spell_EffectProperties_Type"));

  myCurrentProperties.type = RPG_MAGIC_SPELL_EFFECT_INVALID;
  myCurrentProperties.damage.discriminator = RPG_Magic_Spell_DamageTypeUnion::INVALID;
  myCurrentProperties.damage.physicaldamagetype = RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;
  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.increment = 0;
  myCurrentProperties.levelIncrement.value = 0;
  myCurrentProperties.levelIncrement.range.numDice = 0;
  myCurrentProperties.levelIncrement.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.levelIncrement.range.modifier = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.duration.base.value = 0;
  myCurrentProperties.duration.base.range.numDice = 0;
  myCurrentProperties.duration.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.base.range.modifier = 0;
  myCurrentProperties.duration.levelIncrement = 0;
  myCurrentProperties.duration.levelIncrementMax = 0;
  myCurrentProperties.duration.reciprocalIncrement = 0;
  myCurrentProperties.preconditions.clear();
  myCurrentProperties.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.domain = RPG_MAGIC_DOMAIN_INVALID;
  myCurrentProperties.creature.metaType = RPG_COMMON_CREATUREMETATYPE_INVALID;
  myCurrentProperties.creature.subTypes.clear();
  myCurrentProperties.maxRange = 0;
  myCurrentProperties.counterMeasures.clear();
  myCurrentProperties.includeAdjacent = false;
  myCurrentProperties.incrementIsReciprocal = false;
}

void RPG_Magic_Spell_EffectProperties_Type::type(const RPG_Magic_Spell_Effect& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_Spell_EffectProperties_Type::damage(const RPG_Magic_Spell_DamageTypeUnion& damageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::damage"));

  myCurrentProperties.damage = damageType_in;
}

void RPG_Magic_Spell_EffectProperties_Type::base(const RPG_Common_Amount& base_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::base"));

  myCurrentProperties.base = base_in;
}

void RPG_Magic_Spell_EffectProperties_Type::increment(unsigned int increment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::increment"));

  myCurrentProperties.increment = increment_in;
}

void RPG_Magic_Spell_EffectProperties_Type::levelIncrement(const RPG_Common_Amount& levelIncrement_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::levelIncrement"));

  myCurrentProperties.levelIncrement = levelIncrement_in;
}

void RPG_Magic_Spell_EffectProperties_Type::levelIncrementMax(unsigned char levelIncrementMax_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::levelIncrementMax"));

  myCurrentProperties.levelIncrementMax = levelIncrementMax_in;
}

void RPG_Magic_Spell_EffectProperties_Type::attribute(const RPG_Common_Attribute& attribute_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::attribute"));

  myCurrentProperties.attribute = attribute_in;
}

void RPG_Magic_Spell_EffectProperties_Type::domain(const RPG_Magic_Domain& domain_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::domain"));

  myCurrentProperties.domain = domain_in;
}

void RPG_Magic_Spell_EffectProperties_Type::creature(const RPG_Common_CreatureType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::creature"));

  myCurrentProperties.creature = type_in;
}

void RPG_Magic_Spell_EffectProperties_Type::duration(const RPG_Common_EffectDuration& duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_Spell_EffectProperties_Type::precondition(const RPG_Magic_Spell_PreconditionProperties& precondition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::precondition"));

  myCurrentProperties.preconditions.push_back(precondition_in);
}

void RPG_Magic_Spell_EffectProperties_Type::maxRange(unsigned char maxRange_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::maxRange"));

  myCurrentProperties.maxRange = maxRange_in;
}

void RPG_Magic_Spell_EffectProperties_Type::counterMeasure(const RPG_Magic_CounterMeasure& counterMeasure_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::counterMeasure"));

  myCurrentProperties.counterMeasures.push_back(counterMeasure_in);
}

void RPG_Magic_Spell_EffectProperties_Type::includeAdjacent(bool includeAdjacent_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::includeAdjacent"));

  myCurrentProperties.includeAdjacent = includeAdjacent_in;
}

void RPG_Magic_Spell_EffectProperties_Type::incrementIsReciprocal(bool incrementIsReciprocal_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::incrementIsReciprocal"));

  myCurrentProperties.incrementIsReciprocal = incrementIsReciprocal_in;
}

RPG_Magic_Spell_EffectProperties RPG_Magic_Spell_EffectProperties_Type::post_RPG_Magic_Spell_EffectProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_EffectProperties_Type::post_RPG_Magic_Spell_EffectProperties_Type"));

  RPG_Magic_Spell_EffectProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.type = RPG_MAGIC_SPELL_EFFECT_INVALID;
  myCurrentProperties.damage.discriminator = RPG_Magic_Spell_DamageTypeUnion::INVALID;
  myCurrentProperties.damage.physicaldamagetype = RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;
  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.increment = 0;
  myCurrentProperties.levelIncrement.value = 0;
  myCurrentProperties.levelIncrement.range.numDice = 0;
  myCurrentProperties.levelIncrement.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.levelIncrement.range.modifier = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.duration.base.value = 0;
  myCurrentProperties.duration.base.range.numDice = 0;
  myCurrentProperties.duration.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.base.range.modifier = 0;
  myCurrentProperties.duration.levelIncrement = 0;
  myCurrentProperties.duration.levelIncrementMax = 0;
  myCurrentProperties.duration.reciprocalIncrement = 0;
  myCurrentProperties.preconditions.clear();
  myCurrentProperties.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.domain = RPG_MAGIC_DOMAIN_INVALID;
  myCurrentProperties.creature.metaType = RPG_COMMON_CREATUREMETATYPE_INVALID;
  myCurrentProperties.creature.subTypes.clear();
  myCurrentProperties.maxRange = 0;
  myCurrentProperties.counterMeasures.clear();
  myCurrentProperties.includeAdjacent = false;
  myCurrentProperties.incrementIsReciprocal = false;

  return result;
}

RPG_Magic_Spell_PropertiesXML_Type::RPG_Magic_Spell_PropertiesXML_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::RPG_Magic_Spell_PropertiesXML_Type"));

  myCurrentProperties.name.resize(0);
  myCurrentProperties.type.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.type.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.type.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentProperties.type.descriptors.clear();
  myCurrentProperties.levels.clear();
  myCurrentProperties.cost = 0;
  myCurrentProperties.time.rounds = 0;
  myCurrentProperties.time.action = ACTION_STANDARD;
  myCurrentProperties.range.maximum = 0;
  myCurrentProperties.range.increment = 0;
  myCurrentProperties.range.effect = RPG_MAGIC_SPELL_RANGEEFFECT_INVALID;
  myCurrentProperties.targets.clear();
  myCurrentProperties.duration.base.value = 0;
  myCurrentProperties.duration.base.range.numDice = 0;
  myCurrentProperties.duration.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.base.range.modifier = 0;
  myCurrentProperties.duration.levelIncrement = 0;
  myCurrentProperties.duration.levelIncrementMax = 0;
  myCurrentProperties.duration.duration = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration.dismissible = false;
  myCurrentProperties.preconditions.clear();
  myCurrentProperties.effects.clear();
  myCurrentProperties.counterMeasures.clear();
  myCurrentProperties.saveable = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.resistible = false;
  myCurrentProperties.effectsAreInclusive = true;
}

void RPG_Magic_Spell_PropertiesXML_Type::name(const ::std::string& name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::name"));

  myCurrentProperties.name = name_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::type(const RPG_Magic_Spell_Type& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::level(const RPG_Magic_Spell_Level& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::level"));

  myCurrentProperties.levels.push_back(level_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::cost(unsigned int cost_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::cost"));

  myCurrentProperties.cost = cost_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::time(const RPG_Magic_Spell_CastingTime& time_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::time"));

  myCurrentProperties.time = time_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::range(const RPG_Magic_Spell_RangeProperties& range_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::range"));

  myCurrentProperties.range = range_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::target(const RPG_Magic_Spell_TargetProperties& target_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::target"));

  myCurrentProperties.targets.push_back(target_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::duration(const RPG_Magic_Spell_DurationProperties& duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::precondition(const RPG_Magic_Spell_PreconditionProperties& precondition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::precondition"));

  myCurrentProperties.preconditions.push_back(precondition_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::effect(const RPG_Magic_Spell_EffectProperties& effect_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::effect"));

  myCurrentProperties.effects.push_back(effect_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::counterMeasure(const RPG_Magic_CounterMeasure& counterMeasure_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::counterMeasure"));

  myCurrentProperties.counterMeasures.push_back(counterMeasure_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::saveable(const RPG_Common_SavingThrow& saveable_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::saveable"));

  myCurrentProperties.saveable = saveable_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::resistible(bool resistible_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::resistible"));

  myCurrentProperties.resistible = resistible_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::effectsAreInclusive(bool effectsAreInclusive_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::effectsAreInclusive"));

  myCurrentProperties.effectsAreInclusive = effectsAreInclusive_in;
}

RPG_Magic_Spell_PropertiesXML RPG_Magic_Spell_PropertiesXML_Type::post_RPG_Magic_Spell_PropertiesXML_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::post_RPG_Magic_Spell_PropertiesXML_Type"));

  RPG_Magic_Spell_PropertiesXML result = myCurrentProperties;

  // clear structure
  myCurrentProperties.name.resize(0);
  myCurrentProperties.type.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.type.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.type.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentProperties.type.descriptors.clear();
  myCurrentProperties.levels.clear();
  myCurrentProperties.cost = 0;
  myCurrentProperties.time.rounds = 0;
  myCurrentProperties.time.action = ACTION_STANDARD;
  myCurrentProperties.range.maximum = 0;
  myCurrentProperties.range.increment = 0;
  myCurrentProperties.range.effect = RPG_MAGIC_SPELL_RANGEEFFECT_INVALID;
  myCurrentProperties.targets.clear();
  myCurrentProperties.duration.base.value = 0;
  myCurrentProperties.duration.base.range.numDice = 0;
  myCurrentProperties.duration.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.base.range.modifier = 0;
  myCurrentProperties.duration.levelIncrement = 0;
  myCurrentProperties.duration.levelIncrementMax = 0;
  myCurrentProperties.duration.duration = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration.dismissible = false;
  myCurrentProperties.preconditions.clear();
  myCurrentProperties.effects.clear();
  myCurrentProperties.counterMeasures.clear();
  myCurrentProperties.saveable = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.resistible = false;
  myCurrentProperties.effectsAreInclusive = true;

  return result;
}

RPG_Magic_SpellLikeProperties_Type::RPG_Magic_SpellLikeProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::RPG_Magic_SpellLikeProperties_Type"));

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
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_SpellLikeProperties_Type::casterLevel(unsigned char casterLevel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::casterLevel"));

  myCurrentProperties.casterLevel = casterLevel_in;
}

void RPG_Magic_SpellLikeProperties_Type::duration(const RPG_Common_Duration& duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_SpellLikeProperties_Type::save(const RPG_Common_SavingThrowCheck& save_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::save"));

  myCurrentProperties.save = save_in;
}

RPG_Magic_SpellLikeProperties RPG_Magic_SpellLikeProperties_Type::post_RPG_Magic_SpellLikeProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_SpellLikeProperties_Type::post_RPG_Magic_SpellLikeProperties_Type"));

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

RPG_Magic_Dictionary_Type::RPG_Magic_Dictionary_Type(RPG_Magic_Dictionary_t* spellDictionary_in)
 : myDictionary(spellDictionary_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::RPG_Magic_Dictionary_Type"));

}

RPG_Magic_Dictionary_Type::~RPG_Magic_Dictionary_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::~RPG_Magic_Dictionary_Type"));

}

// void RPG_Magic_Dictionary_Type::pre()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::pre"));
//
// }

void RPG_Magic_Dictionary_Type::spell(const RPG_Magic_Spell_PropertiesXML& spell_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::spell"));

  RPG_Magic_Spell_Properties properties;
  properties.type = spell_in.type;
  properties.levels = spell_in.levels;
  properties.cost = spell_in.cost;
  properties.time = spell_in.time;
  properties.range = spell_in.range;
  properties.targets = spell_in.targets;
  properties.duration = spell_in.duration;
  properties.preconditions = spell_in.preconditions;
  properties.effects = spell_in.effects;
  properties.saveable = spell_in.saveable;
  properties.resistible = spell_in.resistible;

  myDictionary->insert(std::make_pair(spell_in.name, properties));
}

void RPG_Magic_Dictionary_Type::post_RPG_Magic_Dictionary_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::post_RPG_Magic_Dictionary_Type"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing spell dictionary, retrieved %d spell types...\n"),
             myDictionary->size()));
}
