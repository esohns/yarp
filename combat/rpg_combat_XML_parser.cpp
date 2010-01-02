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

#include <ace/Log_Msg.h>

RPG_Combat_AttackForm RPG_Combat_AttackForm_Type::post_RPG_Combat_AttackForm_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_AttackForm_Type::post_RPG_Combat_AttackForm_Type"));

  return RPG_Combat_AttackFormHelper::stringToRPG_Combat_AttackForm(post_string());
}

RPG_Combat_AreaOfEffect RPG_Combat_AreaOfEffect_Type::post_RPG_Combat_AreaOfEffect_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_AreaOfEffect_Type::post_RPG_Combat_AreaOfEffect_Type"));

  return RPG_Combat_AreaOfEffectHelper::stringToRPG_Combat_AreaOfEffect(post_string());
}

RPG_Combat_RangedEffectUnion_Type::RPG_Combat_RangedEffectUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedEffectUnion_Type::RPG_Combat_RangedEffectUnion_Type"));

  myCurrentRangedEffect.size = RPG_CHARACTER_SIZE_INVALID;
  myCurrentRangedEffect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;
}

void RPG_Combat_RangedEffectUnion_Type::_characters(const ::xml_schema::ro_string& effectType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedEffectUnion_Type::_characters"));

  // can be either:
  // - RPG_Character_Size --> "SIZE_xxx"
  // - RPG_Combat_AreaOfEffect --> "AREA_xxx"
  std::string type = effectType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("SIZE_")) == 0)
    myCurrentRangedEffect.size = RPG_Character_SizeHelper::stringToRPG_Character_Size(effectType_in);
  else
    myCurrentRangedEffect.areaofeffect = RPG_Combat_AreaOfEffectHelper::stringToRPG_Combat_AreaOfEffect(effectType_in);
}

RPG_Combat_RangedEffectUnion RPG_Combat_RangedEffectUnion_Type::post_RPG_Combat_RangedEffectUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedEffectUnion_Type::post_RPG_Combat_RangedEffectUnion_Type"));

  RPG_Combat_RangedEffectUnion result = myCurrentRangedEffect;

  // clear structure
  myCurrentRangedEffect.size = RPG_CHARACTER_SIZE_INVALID;
  myCurrentRangedEffect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;

  return result;
}

RPG_Combat_RangedAttackProperties_Type::RPG_Combat_RangedAttackProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::RPG_Combat_RangedAttackProperties_Type"));

  myCurrentRangedProperties.maxRange = 0;
  myCurrentRangedProperties.increment = 0;
  myCurrentRangedProperties.effect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;
}

void RPG_Combat_RangedAttackProperties_Type::maxRange(unsigned char maxRange_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::maxRange"));

  myCurrentRangedProperties.maxRange = maxRange_in;
}

void RPG_Combat_RangedAttackProperties_Type::increment(unsigned char increment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::increment"));

  myCurrentRangedProperties.increment = increment_in;
}

void RPG_Combat_RangedAttackProperties_Type::effect(const RPG_Combat_RangedEffectUnion& effect_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::effect"));

  myCurrentRangedProperties.effect = effect_in;
}

RPG_Combat_RangedAttackProperties RPG_Combat_RangedAttackProperties_Type::post_RPG_Combat_RangedAttackProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_RangedAttackProperties_Type::post_RPG_Combat_RangedAttackProperties_Type"));

  RPG_Combat_RangedAttackProperties result = myCurrentRangedProperties;

  // clear structure
  myCurrentRangedProperties.maxRange = 0;
  myCurrentRangedProperties.increment = 0;
  myCurrentRangedProperties.effect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;

  return result;
}

RPG_Combat_AttackSituation RPG_Combat_AttackSituation_Type::post_RPG_Combat_AttackSituation_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_AttackSituation_Type::post_RPG_Combat_AttackSituation_Type"));

  return RPG_Combat_AttackSituationHelper::stringToRPG_Combat_AttackSituation(post_string());
}

RPG_Combat_DefenseSituation RPG_Combat_DefenseSituation_Type::post_RPG_Combat_DefenseSituation_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DefenseSituation_Type::post_RPG_Combat_DefenseSituation_Type"));

  return RPG_Combat_DefenseSituationHelper::stringToRPG_Combat_DefenseSituation(post_string());
}

RPG_Combat_SpecialAttack RPG_Combat_SpecialAttack_Type::post_RPG_Combat_SpecialAttack_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_SpecialAttack_Type::post_RPG_Combat_SpecialAttack_Type"));

  return RPG_Combat_SpecialAttackHelper::stringToRPG_Combat_SpecialAttack(post_string());
}

RPG_Combat_SpecialDamageType RPG_Combat_SpecialDamageType_Type::post_RPG_Combat_SpecialDamageType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_SpecialDamageType_Type::post_RPG_Combat_SpecialDamageType_Type"));

  return RPG_Combat_SpecialDamageTypeHelper::stringToRPG_Combat_SpecialDamageType(post_string());
}

RPG_Combat_DamageTypeUnion_Type::RPG_Combat_DamageTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::RPG_Combat_DamageTypeUnion_Type"));

  myCurrentDamageType.physicaldamagetype = RPG_ITEM_PHYSICALDAMAGETYPE_INVALID;
  myCurrentDamageType.specialdamagetype = RPG_COMBAT_SPECIALDAMAGETYPE_INVALID;
}

void RPG_Combat_DamageTypeUnion_Type::_characters(const ::xml_schema::ro_string& damageType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Item_PhysicalDamageType --> "PHYSICALDAMAGE_xxx"
  // - RPG_Combat_SpecialDamageType --> "DAMAGE_xxx"
  std::string type = damageType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("PHYSICALDAMAGE_")) == 0)
    myCurrentDamageType.physicaldamagetype = RPG_Item_PhysicalDamageTypeHelper::stringToRPG_Item_PhysicalDamageType(damageType_in);
  else
    myCurrentDamageType.specialdamagetype = RPG_Combat_SpecialDamageTypeHelper::stringToRPG_Combat_SpecialDamageType(damageType_in);
}

RPG_Combat_DamageTypeUnion RPG_Combat_DamageTypeUnion_Type::post_RPG_Combat_DamageTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::post_RPG_Combat_DamageTypeUnion_Type"));

  RPG_Combat_DamageTypeUnion result = myCurrentDamageType;

  // clear structure
  myCurrentDamageType.physicaldamagetype = RPG_ITEM_PHYSICALDAMAGETYPE_INVALID;
  myCurrentDamageType.specialdamagetype = RPG_COMBAT_SPECIALDAMAGETYPE_INVALID;

  return result;
}

RPG_Combat_DamageDuration_Type::RPG_Combat_DamageDuration_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::RPG_Combat_DamageDuration_Type"));

  myCurrentDuration.incubationPeriod.numDice = 0;
  myCurrentDuration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDuration.incubationPeriod.modifier = 0;
  myCurrentDuration.interval = 0;
  myCurrentDuration.totalDuration = 0;
}

void RPG_Combat_DamageDuration_Type::incubationPeriod(const RPG_Dice_Roll& incubationPeriod_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::incubationPeriod"));

  myCurrentDuration.incubationPeriod = incubationPeriod_in;
}

void RPG_Combat_DamageDuration_Type::interval(unsigned short interval_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::interval"));

  myCurrentDuration.interval = interval_in;
}

void RPG_Combat_DamageDuration_Type::totalDuration(unsigned short duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::totalDuration"));

  myCurrentDuration.totalDuration = duration_in;
}

RPG_Combat_DamageDuration RPG_Combat_DamageDuration_Type::post_RPG_Combat_DamageDuration_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageDuration_Type::post_RPG_Combat_DamageDuration_Type"));

  RPG_Combat_DamageDuration result = myCurrentDuration;

  // clear structure
  myCurrentDuration.incubationPeriod.numDice = 0;
  myCurrentDuration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDuration.incubationPeriod.modifier = 0;
  myCurrentDuration.interval = 0;
  myCurrentDuration.totalDuration = 0;

  return result;
}

RPG_Combat_DamageBonusType RPG_Combat_DamageBonusType_Type::post_RPG_Combat_DamageBonusType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageBonusType_Type::post_RPG_Combat_DamageBonusType_Type"));

  return RPG_Combat_DamageBonusTypeHelper::stringToRPG_Combat_DamageBonusType(post_string());
}

RPG_Combat_DamageBonus_Type::RPG_Combat_DamageBonus_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageBonus_Type::RPG_Combat_DamageBonus_Type"));

  myCurrentDamageBonus.type = RPG_COMBAT_DAMAGEBONUSTYPE_INVALID;
  myCurrentDamageBonus.modifier = 0;
}

void RPG_Combat_DamageBonus_Type::type(const RPG_Combat_DamageBonusType& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageBonus_Type::type"));

  myCurrentDamageBonus.type = type_in;
}

void RPG_Combat_DamageBonus_Type::modifier(signed char modifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageBonus_Type::modifier"));

  myCurrentDamageBonus.modifier = modifier_in;
}

RPG_Combat_DamageBonus RPG_Combat_DamageBonus_Type::post_RPG_Combat_DamageBonus_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageBonus_Type::post_RPG_Combat_DamageBonus_Type"));

  RPG_Combat_DamageBonus result = myCurrentDamageBonus;

  // clear structure
  myCurrentDamageBonus.type = RPG_COMBAT_DAMAGEBONUSTYPE_INVALID;
  myCurrentDamageBonus.modifier = 0;

  return result;
}

RPG_Combat_DamageEffectType RPG_Combat_DamageEffectType_Type::post_RPG_Combat_DamageEffectType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageEffectType_Type::post_RPG_Combat_DamageEffectType_Type"));

  return RPG_Combat_DamageEffectTypeHelper::stringToRPG_Combat_DamageEffectType(post_string());
}

RPG_Combat_DamageElement_Type::RPG_Combat_DamageElement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::RPG_Combat_DamageElement_Type"));

  myCurrentDamageElement.types.clear();
  myCurrentDamageElement.amount.numDice = 0;
  myCurrentDamageElement.amount.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.amount.modifier = 0;
  myCurrentDamageElement.duration.incubationPeriod.numDice = 0;
  myCurrentDamageElement.duration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.duration.incubationPeriod.modifier = 0;
  myCurrentDamageElement.duration.interval = 0;
  myCurrentDamageElement.duration.totalDuration = 0;
  myCurrentDamageElement.others.clear();
  myCurrentDamageElement.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentDamageElement.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentDamageElement.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentDamageElement.save.difficultyClass = 0;
  myCurrentDamageElement.counterMeasure = false;
  myCurrentDamageElement.effect = RPG_COMBAT_DAMAGEEFFECTTYPE_INVALID;
}

void RPG_Combat_DamageElement_Type::type(const RPG_Combat_DamageTypeUnion& damageType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::type"));

  myCurrentDamageElement.types.push_back(damageType_in);
}

void RPG_Combat_DamageElement_Type::amount(const RPG_Dice_Roll& amount_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::amount"));

  myCurrentDamageElement.amount = amount_in;
}

void RPG_Combat_DamageElement_Type::duration(const RPG_Combat_DamageDuration& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::duration"));

  myCurrentDamageElement.duration = duration_in;
}

void RPG_Combat_DamageElement_Type::other(const RPG_Combat_DamageBonus& bonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::other"));

  myCurrentDamageElement.others.push_back(bonus_in);
}

void RPG_Combat_DamageElement_Type::attribute(const RPG_Common_Attribute& attitude_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::attribute"));

  myCurrentDamageElement.attribute = attitude_in;
}

void RPG_Combat_DamageElement_Type::save(const RPG_Common_SavingThrowModifier& savingThrow_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::save"));

  myCurrentDamageElement.save = savingThrow_in;
}

void RPG_Combat_DamageElement_Type::counterMeasure(bool counterMeasure_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::counterMeasure"));

  myCurrentDamageElement.counterMeasure = counterMeasure_in;
}

void RPG_Combat_DamageElement_Type::effect(const RPG_Combat_DamageEffectType& effect_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::effect"));

  myCurrentDamageElement.effect = effect_in;
}

RPG_Combat_DamageElement RPG_Combat_DamageElement_Type::post_RPG_Combat_DamageElement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::post_RPG_Combat_DamageElement_Type"));

  RPG_Combat_DamageElement result = myCurrentDamageElement;

  // clear structure
  myCurrentDamageElement.types.clear();
  myCurrentDamageElement.amount.numDice = 0;
  myCurrentDamageElement.amount.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.amount.modifier = 0;
  myCurrentDamageElement.duration.incubationPeriod.numDice = 0;
  myCurrentDamageElement.duration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.duration.incubationPeriod.modifier = 0;
  myCurrentDamageElement.duration.interval = 0;
  myCurrentDamageElement.duration.totalDuration = 0;
  myCurrentDamageElement.others.clear();
  myCurrentDamageElement.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentDamageElement.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentDamageElement.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentDamageElement.save.difficultyClass = 0;
  myCurrentDamageElement.counterMeasure = false;
  myCurrentDamageElement.effect = RPG_COMBAT_DAMAGEEFFECTTYPE_INVALID;

  return result;
}

RPG_Combat_Damage_Type::RPG_Combat_Damage_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::RPG_Combat_Damage_Type"));

  myCurrentDamage.elements.clear();
}

void RPG_Combat_Damage_Type::element(const RPG_Combat_DamageElement& element_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::element"));

  myCurrentDamage.elements.push_back(element_in);
}

RPG_Combat_Damage RPG_Combat_Damage_Type::post_RPG_Combat_Damage_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Damage_Type::post_RPG_Combat_Damage_Type"));

  RPG_Combat_Damage result = myCurrentDamage;

  // clear structure
  myCurrentDamage.elements.clear();

  return result;
}
