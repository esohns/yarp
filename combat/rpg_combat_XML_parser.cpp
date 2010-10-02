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
#include "rpg_combat_XML_parser.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Combat_AttackForm RPG_Combat_AttackForm_Type::post_RPG_Combat_AttackForm_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_AttackForm_Type::post_RPG_Combat_AttackForm_Type"));

  return RPG_Combat_AttackFormHelper::stringToRPG_Combat_AttackForm(post_string());
}

RPG_Combat_RangedEffectUnion_Type::RPG_Combat_RangedEffectUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedEffectUnion_Type::RPG_Combat_RangedEffectUnion_Type"));

  myCurrentRangedEffect.discriminator = RPG_Combat_RangedEffectUnion::INVALID;
  myCurrentRangedEffect.size = RPG_COMMON_SIZE_INVALID;
}

void RPG_Combat_RangedEffectUnion_Type::_characters(const ::xml_schema::ro_string& effectType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedEffectUnion_Type::_characters"));

  // can be either:
  // - RPG_Character_Size --> "SIZE_xxx"
  // - RPG_Common_AreaOfEffect --> "AREA_xxx"
  std::string type = effectType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("SIZE_")) == 0)
  {
    myCurrentRangedEffect.discriminator = RPG_Combat_RangedEffectUnion::SIZE;
    myCurrentRangedEffect.size = RPG_Common_SizeHelper::stringToRPG_Common_Size(effectType_in);
  } // end IF
  else
  {
    myCurrentRangedEffect.discriminator = RPG_Combat_RangedEffectUnion::AREAOFEFFECT;
    myCurrentRangedEffect.areaofeffect = RPG_Common_AreaOfEffectHelper::stringToRPG_Common_AreaOfEffect(effectType_in);
  } // end ELSE
}

RPG_Combat_RangedEffectUnion RPG_Combat_RangedEffectUnion_Type::post_RPG_Combat_RangedEffectUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedEffectUnion_Type::post_RPG_Combat_RangedEffectUnion_Type"));

  RPG_Combat_RangedEffectUnion result = myCurrentRangedEffect;

  // clear structure
  myCurrentRangedEffect.discriminator = RPG_Combat_RangedEffectUnion::INVALID;
  myCurrentRangedEffect.size = RPG_COMMON_SIZE_INVALID;

  return result;
}

RPG_Combat_RangedAttackProperties_Type::RPG_Combat_RangedAttackProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::RPG_Combat_RangedAttackProperties_Type"));

  myCurrentRangedProperties.maxRange = 0;
  myCurrentRangedProperties.increment = 0;
  myCurrentRangedProperties.effect.areaofeffect = RPG_COMMON_AREAOFEFFECT_INVALID;
}

void RPG_Combat_RangedAttackProperties_Type::maxRange(unsigned char maxRange_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::maxRange"));

  myCurrentRangedProperties.maxRange = maxRange_in;
}

void RPG_Combat_RangedAttackProperties_Type::increment(unsigned char increment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::increment"));

  myCurrentRangedProperties.increment = increment_in;
}

void RPG_Combat_RangedAttackProperties_Type::effect(const RPG_Combat_RangedEffectUnion& effect_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::effect"));

  myCurrentRangedProperties.effect = effect_in;
}

RPG_Combat_RangedAttackProperties RPG_Combat_RangedAttackProperties_Type::post_RPG_Combat_RangedAttackProperties_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::post_RPG_Combat_RangedAttackProperties_Type"));

  RPG_Combat_RangedAttackProperties result = myCurrentRangedProperties;

  // clear structure
  myCurrentRangedProperties.maxRange = 0;
  myCurrentRangedProperties.increment = 0;
  myCurrentRangedProperties.effect.areaofeffect = RPG_COMMON_AREAOFEFFECT_INVALID;

  return result;
}

RPG_Combat_AttackSituation RPG_Combat_AttackSituation_Type::post_RPG_Combat_AttackSituation_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_AttackSituation_Type::post_RPG_Combat_AttackSituation_Type"));

  return RPG_Combat_AttackSituationHelper::stringToRPG_Combat_AttackSituation(post_string());
}

RPG_Combat_DefenseSituation RPG_Combat_DefenseSituation_Type::post_RPG_Combat_DefenseSituation_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DefenseSituation_Type::post_RPG_Combat_DefenseSituation_Type"));

  return RPG_Combat_DefenseSituationHelper::stringToRPG_Combat_DefenseSituation(post_string());
}

RPG_Combat_SpecialAttack RPG_Combat_SpecialAttack_Type::post_RPG_Combat_SpecialAttack_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_SpecialAttack_Type::post_RPG_Combat_SpecialAttack_Type"));

  return RPG_Combat_SpecialAttackHelper::stringToRPG_Combat_SpecialAttack(post_string());
}

RPG_Combat_SpecialDamageType RPG_Combat_SpecialDamageType_Type::post_RPG_Combat_SpecialDamageType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_SpecialDamageType_Type::post_RPG_Combat_SpecialDamageType_Type"));

  return RPG_Combat_SpecialDamageTypeHelper::stringToRPG_Combat_SpecialDamageType(post_string());
}

RPG_Combat_DamageTypeUnion_Type::RPG_Combat_DamageTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::RPG_Combat_DamageTypeUnion_Type"));

  myCurrentDamageType.discriminator = RPG_Combat_DamageTypeUnion::INVALID;
  myCurrentDamageType.physicaldamagetype = RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;
}

void RPG_Combat_DamageTypeUnion_Type::_characters(const ::xml_schema::ro_string& damageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Item_PhysicalDamageType --> "PHYSICALDAMAGE_xxx"
  // - RPG_Combat_SpecialDamageType --> "DAMAGE_xxx"
  std::string type = damageType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_")) == 0)
  {
    myCurrentDamageType.physicaldamagetype = RPG_Common_PhysicalDamageTypeHelper::stringToRPG_Common_PhysicalDamageType(damageType_in);
    myCurrentDamageType.discriminator = RPG_Combat_DamageTypeUnion::PHYSICALDAMAGETYPE;
  } // end IF
  else
  {
    myCurrentDamageType.specialdamagetype = RPG_Combat_SpecialDamageTypeHelper::stringToRPG_Combat_SpecialDamageType(damageType_in);
    myCurrentDamageType.discriminator = RPG_Combat_DamageTypeUnion::SPECIALDAMAGETYPE;
  } // end ELSE
}

RPG_Combat_DamageTypeUnion RPG_Combat_DamageTypeUnion_Type::post_RPG_Combat_DamageTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::post_RPG_Combat_DamageTypeUnion_Type"));

  RPG_Combat_DamageTypeUnion result = myCurrentDamageType;

  // clear structure
  myCurrentDamageType.discriminator = RPG_Combat_DamageTypeUnion::INVALID;
  myCurrentDamageType.physicaldamagetype = RPG_COMMON_PHYSICALDAMAGETYPE_INVALID;

  return result;
}

RPG_Combat_DamageDuration_Type::RPG_Combat_DamageDuration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::RPG_Combat_DamageDuration_Type"));

  myCurrentDuration.incubationPeriod.numDice = 0;
  myCurrentDuration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDuration.incubationPeriod.modifier = 0;
  myCurrentDuration.totalPeriod.numDice = 0;
  myCurrentDuration.totalPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDuration.totalPeriod.modifier = 0;
  myCurrentDuration.interval = 0;
  myCurrentDuration.totalDuration = 0;
  myCurrentDuration.vicinity = false;
}

void RPG_Combat_DamageDuration_Type::incubationPeriod(const RPG_Dice_Roll& incubationPeriod_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::incubationPeriod"));

  myCurrentDuration.incubationPeriod = incubationPeriod_in;
}

void RPG_Combat_DamageDuration_Type::totalPeriod(const RPG_Dice_Roll& totalPeriod_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::totalPeriod"));

  myCurrentDuration.totalPeriod = totalPeriod_in;
}

void RPG_Combat_DamageDuration_Type::interval(unsigned short interval_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::interval"));

  myCurrentDuration.interval = interval_in;
}

void RPG_Combat_DamageDuration_Type::totalDuration(unsigned short duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::totalDuration"));

  myCurrentDuration.totalDuration = duration_in;
}

void RPG_Combat_DamageDuration_Type::vicinity(bool vicinity_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::vicinity"));

  myCurrentDuration.vicinity = vicinity_in;
}

RPG_Combat_DamageDuration RPG_Combat_DamageDuration_Type::post_RPG_Combat_DamageDuration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::post_RPG_Combat_DamageDuration_Type"));

  RPG_Combat_DamageDuration result = myCurrentDuration;

  // clear structure
  myCurrentDuration.incubationPeriod.numDice = 0;
  myCurrentDuration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDuration.incubationPeriod.modifier = 0;
  myCurrentDuration.totalPeriod.numDice = 0;
  myCurrentDuration.totalPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDuration.totalPeriod.modifier = 0;
  myCurrentDuration.interval = 0;
  myCurrentDuration.totalDuration = 0;
  myCurrentDuration.vicinity = false;

  return result;
}

RPG_Combat_OtherDamageType RPG_Combat_OtherDamageType_Type::post_RPG_Combat_OtherDamageType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_OtherDamageType_Type::post_RPG_Combat_OtherDamageType_Type"));

  return RPG_Combat_OtherDamageTypeHelper::stringToRPG_Combat_OtherDamageType(post_string());
}

RPG_Combat_OtherDamage_Type::RPG_Combat_OtherDamage_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_OtherDamage_Type::RPG_Combat_OtherDamage_Type"));

  myCurrentOtherDamage.type = RPG_COMBAT_OTHERDAMAGETYPE_INVALID;
  myCurrentOtherDamage.modifier = 0;
  myCurrentOtherDamage.counterMeasures.clear();
}

void RPG_Combat_OtherDamage_Type::type(const RPG_Combat_OtherDamageType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_OtherDamage_Type::type"));

  myCurrentOtherDamage.type = type_in;
}

void RPG_Combat_OtherDamage_Type::modifier(signed char modifier_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_OtherDamage_Type::modifier"));

  myCurrentOtherDamage.modifier = modifier_in;
}

void RPG_Combat_OtherDamage_Type::counterMeasure(const RPG_Combat_DamageCounterMeasure& counterMeasure_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_OtherDamage_Type::counterMeasure"));

  myCurrentOtherDamage.counterMeasures.push_back(counterMeasure_in);
}

RPG_Combat_OtherDamage RPG_Combat_OtherDamage_Type::post_RPG_Combat_OtherDamage_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_OtherDamage_Type::post_RPG_Combat_OtherDamage_Type"));

  RPG_Combat_OtherDamage result = myCurrentOtherDamage;

  // clear structure
  myCurrentOtherDamage.type = RPG_COMBAT_OTHERDAMAGETYPE_INVALID;
  myCurrentOtherDamage.modifier = 0;
  myCurrentOtherDamage.counterMeasures.clear();

  return result;
}

RPG_Combat_CheckTypeUnion_Type::RPG_Combat_CheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_CheckTypeUnion_Type::RPG_Combat_CheckTypeUnion_Type"));

  myCurrentCheckTypeUnion.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.checktype = RPG_COMMON_CHECKTYPE_INVALID;
}

void RPG_Combat_CheckTypeUnion_Type::_characters(const ::xml_schema::ro_string& checkType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_CheckTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_Skill_Type --> "SKILL_xxx"
  // - RPG_Common_Attribute_Type --> "ATTRIBUTE_xxx"
  // - RPG_Common_BaseCheckTypeUnion --> "CHECK_xxx" || "SAVINGTHROW_xxx"
  std::string type = checkType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("SKILL_")) == 0)
  {
    myCurrentCheckTypeUnion.skill = RPG_Common_SkillHelper::stringToRPG_Common_Skill(checkType_in);
    myCurrentCheckTypeUnion.discriminator = RPG_Combat_CheckTypeUnion::SKILL;
  } // end IF
  else if (type.find(ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_")) == 0)
  {
    myCurrentCheckTypeUnion.attribute = RPG_Common_AttributeHelper::stringToRPG_Common_Attribute(checkType_in);
    myCurrentCheckTypeUnion.discriminator = RPG_Combat_CheckTypeUnion::ATTRIBUTE;
  } // end IF
  else if (type.find(ACE_TEXT_ALWAYS_CHAR("CHECK_")) == 0)
  {
    myCurrentCheckTypeUnion.discriminator = RPG_Combat_CheckTypeUnion::BASECHECKTYPEUNION;
    myCurrentCheckTypeUnion.basechecktypeunion.checktype = RPG_Common_CheckTypeHelper::stringToRPG_Common_CheckType(checkType_in);
    myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::CHECKTYPE;
  } // end IF
  else
  {
    myCurrentCheckTypeUnion.discriminator = RPG_Combat_CheckTypeUnion::BASECHECKTYPEUNION;
    myCurrentCheckTypeUnion.basechecktypeunion.savingthrow = RPG_Common_SavingThrowHelper::stringToRPG_Common_SavingThrow(checkType_in);
    myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::SAVINGTHROW;
  } // end ELSE
}

RPG_Combat_CheckTypeUnion RPG_Combat_CheckTypeUnion_Type::post_RPG_Combat_CheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_CheckTypeUnion_Type::post_RPG_Combat_CheckTypeUnion_Type"));

  RPG_Combat_CheckTypeUnion result = myCurrentCheckTypeUnion;

  // clear structure
  myCurrentCheckTypeUnion.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.discriminator = RPG_Common_BaseCheckTypeUnion::INVALID;
  myCurrentCheckTypeUnion.basechecktypeunion.checktype = RPG_COMMON_CHECKTYPE_INVALID;

  return result;
}

RPG_Combat_Check_Type::RPG_Combat_Check_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Check_Type::RPG_Combat_Check_Type"));

  myCurrentCheck.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheck.type.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentCheck.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheck.difficultyClass = 0;
}

void RPG_Combat_Check_Type::type(const RPG_Combat_CheckTypeUnion& checkUnion_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Check_Type::type"));

  myCurrentCheck.type = checkUnion_in;
}

void RPG_Combat_Check_Type::attribute(const RPG_Common_Attribute& attribute_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Check_Type::attribute"));

  myCurrentCheck.attribute = attribute_in;
}

void RPG_Combat_Check_Type::difficultyClass(unsigned char difficultyClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Check_Type::difficultyClass"));

  myCurrentCheck.difficultyClass = difficultyClass_in;
}

RPG_Combat_Check RPG_Combat_Check_Type::post_RPG_Combat_Check_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Check_Type::post_RPG_Combat_Check_Type"));

  RPG_Combat_Check result = myCurrentCheck;

  // clear structure
  myCurrentCheck.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCheck.type.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentCheck.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheck.difficultyClass = 0;

  return result;
}

RPG_Combat_DamageReductionType RPG_Combat_DamageReductionType_Type::post_RPG_Combat_DamageReductionType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageReductionType_Type::post_RPG_Combat_DamageReductionType_Type"));

  return RPG_Combat_DamageReductionTypeHelper::stringToRPG_Combat_DamageReductionType(post_string());
}

RPG_Combat_DamageCounterMeasure_Type::RPG_Combat_DamageCounterMeasure_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::RPG_Combat_DamageCounterMeasure_Type"));

  myCurrentCounterMeasure.type = RPG_COMMON_COUNTERMEASURE_INVALID;
  myCurrentCounterMeasure.check.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCounterMeasure.check.type.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentCounterMeasure.check.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCounterMeasure.check.difficultyClass = 0;
  myCurrentCounterMeasure.spells.clear();
  myCurrentCounterMeasure.duration.activation = 0;
  myCurrentCounterMeasure.duration.interval = 0;
  myCurrentCounterMeasure.duration.total = 0;
  myCurrentCounterMeasure.condition = RPG_COMMON_CONDITION_INVALID;
  myCurrentCounterMeasure.reduction = RPG_COMBAT_DAMAGEREDUCTIONTYPE_INVALID;
}

void RPG_Combat_DamageCounterMeasure_Type::type(const RPG_Common_CounterMeasure& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::type"));

  myCurrentCounterMeasure.type = type_in;
}

void RPG_Combat_DamageCounterMeasure_Type::check(const RPG_Combat_Check& check_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::check"));

  myCurrentCounterMeasure.check = check_in;
}

void RPG_Combat_DamageCounterMeasure_Type::spell(const RPG_Magic_SpellType& spell_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::spell"));

  myCurrentCounterMeasure.spells.push_back(spell_in);
}

void RPG_Combat_DamageCounterMeasure_Type::duration(const RPG_Common_Duration& duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::duration"));

  myCurrentCounterMeasure.duration = duration_in;
}

void RPG_Combat_DamageCounterMeasure_Type::condition(const RPG_Common_Condition& condition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::condition"));

  myCurrentCounterMeasure.condition = condition_in;
}

void RPG_Combat_DamageCounterMeasure_Type::reduction(const RPG_Combat_DamageReductionType& reduction_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::reduction"));

  myCurrentCounterMeasure.reduction = reduction_in;
}

RPG_Combat_DamageCounterMeasure RPG_Combat_DamageCounterMeasure_Type::post_RPG_Combat_DamageCounterMeasure_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageCounterMeasure_Type::post_RPG_Combat_DamageCounterMeasure_Type"));

  RPG_Combat_DamageCounterMeasure result = myCurrentCounterMeasure;

  // clear structure
  myCurrentCounterMeasure.type = RPG_COMMON_COUNTERMEASURE_INVALID;
  myCurrentCounterMeasure.check.type.skill = RPG_COMMON_SKILL_INVALID;
  myCurrentCounterMeasure.check.type.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentCounterMeasure.check.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCounterMeasure.check.difficultyClass = 0;
  myCurrentCounterMeasure.spells.clear();
  myCurrentCounterMeasure.duration.activation = 0;
  myCurrentCounterMeasure.duration.interval = 0;
  myCurrentCounterMeasure.duration.total = 0;
  myCurrentCounterMeasure.condition = RPG_COMMON_CONDITION_INVALID;
  myCurrentCounterMeasure.reduction = RPG_COMBAT_DAMAGEREDUCTIONTYPE_INVALID;

  return result;
}

RPG_Combat_DamageElement_Type::RPG_Combat_DamageElement_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::RPG_Combat_DamageElement_Type"));

  myCurrentDamageElement.types.clear();
  myCurrentDamageElement.amount.numDice = 0;
  myCurrentDamageElement.amount.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.amount.modifier = 0;
  myCurrentDamageElement.secondary.numDice = 0;
  myCurrentDamageElement.secondary.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.secondary.modifier = 0;
  myCurrentDamageElement.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentDamageElement.condition = RPG_COMMON_CONDITION_INVALID;
  myCurrentDamageElement.duration.incubationPeriod.numDice = 0;
  myCurrentDamageElement.duration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.duration.incubationPeriod.modifier = 0;
  myCurrentDamageElement.duration.interval = 0;
  myCurrentDamageElement.duration.totalDuration = 0;
  myCurrentDamageElement.counterMeasures.clear();
  myCurrentDamageElement.others.clear();
  myCurrentDamageElement.effect = RPG_COMMON_EFFECTTYPE_INVALID;
}

void RPG_Combat_DamageElement_Type::type(const RPG_Combat_DamageTypeUnion& damageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::type"));

  myCurrentDamageElement.types.push_back(damageType_in);
}

void RPG_Combat_DamageElement_Type::amount(const RPG_Dice_Roll& amount_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::amount"));

  myCurrentDamageElement.amount = amount_in;
}

void RPG_Combat_DamageElement_Type::secondary(const RPG_Dice_Roll& secondary_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::secondary"));

  myCurrentDamageElement.secondary = secondary_in;
}

void RPG_Combat_DamageElement_Type::condition(const RPG_Common_Condition& condition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::condition"));

  myCurrentDamageElement.condition = condition_in;
}

void RPG_Combat_DamageElement_Type::duration(const RPG_Combat_DamageDuration& duration_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::duration"));

  myCurrentDamageElement.duration = duration_in;
}

void RPG_Combat_DamageElement_Type::other(const RPG_Combat_OtherDamage& bonus_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::other"));

  myCurrentDamageElement.others.push_back(bonus_in);
}

void RPG_Combat_DamageElement_Type::attribute(const RPG_Common_Attribute& attitude_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::attribute"));

  myCurrentDamageElement.attribute = attitude_in;
}

void RPG_Combat_DamageElement_Type::counterMeasure(const RPG_Combat_DamageCounterMeasure& counterMeasure_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::counterMeasure"));

  myCurrentDamageElement.counterMeasures.push_back(counterMeasure_in);
}

void RPG_Combat_DamageElement_Type::effect(const RPG_Common_EffectType& effect_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::effect"));

  myCurrentDamageElement.effect = effect_in;
}

RPG_Combat_DamageElement RPG_Combat_DamageElement_Type::post_RPG_Combat_DamageElement_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::post_RPG_Combat_DamageElement_Type"));

  RPG_Combat_DamageElement result = myCurrentDamageElement;

  // clear structure
  myCurrentDamageElement.types.clear();
  myCurrentDamageElement.amount.numDice = 0;
  myCurrentDamageElement.amount.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.amount.modifier = 0;
  myCurrentDamageElement.secondary.numDice = 0;
  myCurrentDamageElement.secondary.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.secondary.modifier = 0;
  myCurrentDamageElement.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentDamageElement.condition = RPG_COMMON_CONDITION_INVALID;
  myCurrentDamageElement.duration.incubationPeriod.numDice = 0;
  myCurrentDamageElement.duration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.duration.incubationPeriod.modifier = 0;
  myCurrentDamageElement.duration.interval = 0;
  myCurrentDamageElement.duration.totalDuration = 0;
  myCurrentDamageElement.counterMeasures.clear();
  myCurrentDamageElement.others.clear();
  myCurrentDamageElement.effect = RPG_COMMON_EFFECTTYPE_INVALID;

  return result;
}

RPG_Combat_Damage_Type::RPG_Combat_Damage_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::RPG_Combat_Damage_Type"));

  myCurrentDamage.elements.clear();
  myCurrentDamage.elementsAreInclusive = true;
}

void RPG_Combat_Damage_Type::element(const RPG_Combat_DamageElement& element_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::element"));

  myCurrentDamage.elements.push_back(element_in);
}

void RPG_Combat_Damage_Type::elementsAreInclusive(bool elementsAreInclusive_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::elementsAreInclusive"));

  myCurrentDamage.elementsAreInclusive = elementsAreInclusive_in;
}

RPG_Combat_Damage RPG_Combat_Damage_Type::post_RPG_Combat_Damage_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::post_RPG_Combat_Damage_Type"));

  RPG_Combat_Damage result = myCurrentDamage;

  // clear structure
  myCurrentDamage.elements.clear();
  myCurrentDamage.elementsAreInclusive = true;

  return result;
}
