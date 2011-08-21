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
#include "rpg_common_XML_parser.h"

#include "rpg_common_macros.h"
#include "rpg_common_incl.h"

RPG_Common_CreatureMetaType RPG_Common_CreatureMetaType_Type::post_RPG_Common_CreatureMetaType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CreatureMetaType_Type::post_RPG_Common_CreatureMetaType_Type"));

  return RPG_Common_CreatureMetaTypeHelper::stringToRPG_Common_CreatureMetaType(post_string());
}

RPG_Common_CreatureSubType RPG_Common_CreatureSubType_Type::post_RPG_Common_CreatureSubType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CreatureSubType_Type::post_RPG_Common_CreatureSubType_Type"));

  return RPG_Common_CreatureSubTypeHelper::stringToRPG_Common_CreatureSubType(post_string());
}

RPG_Common_CreatureType_Type::RPG_Common_CreatureType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CreatureType_Type::RPG_Common_CreatureType_Type"));

  myCurrentType.metaType = RPG_COMMON_CREATUREMETATYPE_INVALID;
  myCurrentType.subTypes.clear();
}

void RPG_Common_CreatureType_Type::metaType(const RPG_Common_CreatureMetaType& metaType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CreatureType_Type::metaType"));

  myCurrentType.metaType = metaType_in;
}

void RPG_Common_CreatureType_Type::subType(const RPG_Common_CreatureSubType& subType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CreatureType_Type::subType"));

  myCurrentType.subTypes.push_back(subType_in);
}

RPG_Common_CreatureType RPG_Common_CreatureType_Type::post_RPG_Common_CreatureType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CreatureType_Type::post_RPG_Common_CreatureType_Type"));

  RPG_Common_CreatureType result = myCurrentType;

  // clear structure
  myCurrentType.metaType = RPG_COMMON_CREATUREMETATYPE_INVALID;
  myCurrentType.subTypes.clear();

  return result;
}

RPG_Common_SubClass RPG_Common_SubClass_Type::post_RPG_Common_SubClass_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SubClass_Type::post_RPG_Common_SubClass_Type"));

  return RPG_Common_SubClassHelper::stringToRPG_Common_SubClass(post_string());
}

RPG_Common_Attribute RPG_Common_Attribute_Type::post_RPG_Common_Attribute_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Attribute_Type::post_RPG_Common_Attribute_Type"));

  return RPG_Common_AttributeHelper::stringToRPG_Common_Attribute(post_string());
}

RPG_Common_Condition RPG_Common_Condition_Type::post_RPG_Common_Condition_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Condition_Type::post_RPG_Common_Condition_Type"));

  return RPG_Common_ConditionHelper::stringToRPG_Common_Condition(post_string());
}

RPG_Common_Size RPG_Common_Size_Type::post_RPG_Common_Size_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Size_Type::post_RPG_Common_Size_Type"));

  return RPG_Common_SizeHelper::stringToRPG_Common_Size(post_string());
}

RPG_Common_Skill RPG_Common_Skill_Type::post_RPG_Common_Skill_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Skill_Type::post_RPG_Common_Skill_Type"));

  return RPG_Common_SkillHelper::stringToRPG_Common_Skill(post_string());
}

RPG_Common_PhysicalDamageType RPG_Common_PhysicalDamageType_Type::post_RPG_Common_PhysicalDamageType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_PhysicalDamageType_Type::post_RPG_Common_PhysicalDamageType_Type"));

  return RPG_Common_PhysicalDamageTypeHelper::stringToRPG_Common_PhysicalDamageType(post_string());
}

RPG_Common_ActionType RPG_Common_ActionType_Type::post_RPG_Common_ActionType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ActionType_Type::post_RPG_Common_ActionType_Type"));

  return RPG_Common_ActionTypeHelper::stringToRPG_Common_ActionType(post_string());
}

RPG_Common_AreaOfEffect RPG_Common_AreaOfEffect_Type::post_RPG_Common_AreaOfEffect_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_AreaOfEffect_Type::post_RPG_Common_AreaOfEffect_Type"));

  return RPG_Common_AreaOfEffectHelper::stringToRPG_Common_AreaOfEffect(post_string());
}

RPG_Common_EffectType RPG_Common_EffectType_Type::post_RPG_Common_EffectType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectType_Type::post_RPG_Common_EffectType_Type"));

  return RPG_Common_EffectTypeHelper::stringToRPG_Common_EffectType(post_string());
}

RPG_Common_CounterMeasure RPG_Common_CounterMeasure_Type::post_RPG_Common_CounterMeasure_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CounterMeasure_Type::post_RPG_Common_CounterMeasure_Type"));

  return RPG_Common_CounterMeasureHelper::stringToRPG_Common_CounterMeasure(post_string());
}

RPG_Common_CheckType RPG_Common_CheckType_Type::post_RPG_Common_CheckType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_CheckType_Type::post_RPG_Common_CheckType_Type"));

  return RPG_Common_CheckTypeHelper::stringToRPG_Common_CheckType(post_string());
}

RPG_Common_SavingThrow RPG_Common_SavingThrow_Type::post_RPG_Common_SavingThrow_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrow_Type::post_RPG_Common_SavingThrow_Type"));

  return RPG_Common_SavingThrowHelper::stringToRPG_Common_SavingThrow(post_string());
}

RPG_Common_BaseCheckTypeUnion_Type::RPG_Common_BaseCheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_BaseCheckTypeUnion_Type::RPG_Common_BaseCheckTypeUnion_Type"));

  myBaseCheckTypeUnion.discriminator = RPG_Common_BaseCheckTypeUnion::INVALID;
  myBaseCheckTypeUnion.checktype = RPG_COMMON_CHECKTYPE_INVALID;
  myBaseCheckTypeUnion.savingthrow = RPG_COMMON_SAVINGTHROW_INVALID;
}

void RPG_Common_BaseCheckTypeUnion_Type::_characters(const ::xml_schema::ro_string& checkType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_BaseCheckTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_CheckType_Type --> "CHECK_xxx"
  // - RPG_Common_SavingThrow_Type --> "SAVE_xxx"
  std::string type = checkType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("CHECK_")) == 0)
  {
    myBaseCheckTypeUnion.checktype = RPG_Common_CheckTypeHelper::stringToRPG_Common_CheckType(checkType_in);
    myBaseCheckTypeUnion.discriminator = RPG_Common_BaseCheckTypeUnion::CHECKTYPE;
  } // end IF
  else
  {
    myBaseCheckTypeUnion.savingthrow = RPG_Common_SavingThrowHelper::stringToRPG_Common_SavingThrow(checkType_in);
    myBaseCheckTypeUnion.discriminator = RPG_Common_BaseCheckTypeUnion::SAVINGTHROW;
  } // end ELSE
}

RPG_Common_BaseCheckTypeUnion RPG_Common_BaseCheckTypeUnion_Type::post_RPG_Common_BaseCheckTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_BaseCheckTypeUnion_Type::post_RPG_Common_BaseCheckTypeUnion_Type"));

  RPG_Common_BaseCheckTypeUnion result = myBaseCheckTypeUnion;

  // clear structure
  myBaseCheckTypeUnion.discriminator = RPG_Common_BaseCheckTypeUnion::INVALID;
  myBaseCheckTypeUnion.checktype = RPG_COMMON_CHECKTYPE_INVALID;
  myBaseCheckTypeUnion.savingthrow = RPG_COMMON_SAVINGTHROW_INVALID;

  return result;
}

RPG_Common_SaveReductionType RPG_Common_SaveReductionType_Type::post_RPG_Common_SaveReductionType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SaveReductionType_Type::post_RPG_Common_SaveReductionType_Type"));

  return RPG_Common_SaveReductionTypeHelper::stringToRPG_Common_SaveReductionType(post_string());
}

RPG_Common_SavingThrowCheck_Type::RPG_Common_SavingThrowCheck_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrowCheck_Type::RPG_Common_SavingThrowCheck_Type"));

  myCurrentCheck.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentCheck.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheck.difficultyClass = 0;
  myCurrentCheck.reduction = RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;
}

void RPG_Common_SavingThrowCheck_Type::type(const RPG_Common_SavingThrow& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrowCheck_Type::type"));

  myCurrentCheck.type = type_in;
}

void RPG_Common_SavingThrowCheck_Type::attribute(const RPG_Common_Attribute& attribute_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrowCheck_Type::attribute"));

  myCurrentCheck.attribute = attribute_in;
}

void RPG_Common_SavingThrowCheck_Type::difficultyClass(unsigned char difficultyClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrowCheck_Type::difficultyClass"));

  myCurrentCheck.difficultyClass = difficultyClass_in;
}

void RPG_Common_SavingThrowCheck_Type::reduction(const RPG_Common_SaveReductionType& reduction_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrowCheck_Type::reduction"));

  myCurrentCheck.reduction = reduction_in;
}

RPG_Common_SavingThrowCheck RPG_Common_SavingThrowCheck_Type::post_RPG_Common_SavingThrowCheck_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SavingThrowCheck_Type::post_RPG_Common_SavingThrowCheck_Type"));

  RPG_Common_SavingThrowCheck result = myCurrentCheck;

  // clear structure
  myCurrentCheck.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentCheck.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentCheck.difficultyClass = 0;
  myCurrentCheck.reduction = RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;

  return result;
}

RPG_Common_Amount_Type::RPG_Common_Amount_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Amount_Type::RPG_Common_Amount_Type"));

  myCurrentAmount.value = 0;
  myCurrentAmount.range.numDice = 0;
  myCurrentAmount.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentAmount.range.modifier = 0;
}

void RPG_Common_Amount_Type::value(short value_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Amount_Type::value"));

  myCurrentAmount.value = value_in;
}

void RPG_Common_Amount_Type::range(const RPG_Dice_Roll& range_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Amount_Type::range"));

  myCurrentAmount.range = range_in;
}

RPG_Common_Amount RPG_Common_Amount_Type::post_RPG_Common_Amount_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Amount_Type::post_RPG_Common_Amount_Type"));

  RPG_Common_Amount result = myCurrentAmount;

  // clear structure
  myCurrentAmount.value = 0;
  myCurrentAmount.range.numDice = 0;
  myCurrentAmount.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentAmount.range.modifier = 0;

  return result;
}

RPG_Common_Usage_Type::RPG_Common_Usage_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Usage_Type::RPG_Common_Usage_Type"));

  myCurrentUsage.numUses = 0;
  myCurrentUsage.period = 0;
  myCurrentUsage.interval.numDice = 0;
  myCurrentUsage.interval.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentUsage.interval.modifier = 0;
}

void RPG_Common_Usage_Type::numUses(unsigned char numUses_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Usage_Type::numUses"));

  myCurrentUsage.numUses = numUses_in;
}

void RPG_Common_Usage_Type::period(unsigned int period_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Usage_Type::period"));

  myCurrentUsage.period = period_in;
}

void RPG_Common_Usage_Type::interval(const RPG_Dice_Roll& interval_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Usage_Type::interval"));

  myCurrentUsage.interval = interval_in;
}

RPG_Common_Usage RPG_Common_Usage_Type::post_RPG_Common_Usage_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Usage_Type::post_RPG_Common_Usage_Type"));

  RPG_Common_Usage result = myCurrentUsage;

  // clear structure
  myCurrentUsage.numUses = 0;
  myCurrentUsage.period = 0;
  myCurrentUsage.interval.numDice = 0;
  myCurrentUsage.interval.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentUsage.interval.modifier = 0;

  return result;
}

RPG_Common_Duration_Type::RPG_Common_Duration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Duration_Type::RPG_Common_Duration_Type"));

  myCurrentDuration.activation = 0;
  myCurrentDuration.interval = 0;
  myCurrentDuration.total = 0;
//   myCurrentDuration.vicinity = false;
}

void RPG_Common_Duration_Type::activation(unsigned int activation_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Duration_Type::activation"));

  myCurrentDuration.activation = activation_in;
}

void RPG_Common_Duration_Type::interval(unsigned int interval_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Duration_Type::interval"));

  myCurrentDuration.interval = interval_in;
}

void RPG_Common_Duration_Type::total(unsigned int total_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Duration_Type::total"));

  myCurrentDuration.total = total_in;
}

// void RPG_Common_Duration_Type::vicinity(bool vicinity_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Common_Duration_Type::vicinity"));
//
//   myCurrentDuration.vicinity = vicinity_in;
// }

RPG_Common_Duration RPG_Common_Duration_Type::post_RPG_Common_Duration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Duration_Type::post_RPG_Common_Duration_Type"));

  RPG_Common_Duration result = myCurrentDuration;

  // clear structure
  myCurrentDuration.activation = 0;
  myCurrentDuration.interval = 0;
  myCurrentDuration.total = 0;
//   myCurrentDuration.vicinity = false;

  return result;
}

RPG_Common_EffectDuration_Type::RPG_Common_EffectDuration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectDuration_Type::RPG_Common_EffectDuration_Type"));

  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.reciprocalIncrement = 0;
}

void RPG_Common_EffectDuration_Type::base(const RPG_Common_Amount& amount_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectDuration_Type::base"));

  myCurrentProperties.base = amount_in;
}

void RPG_Common_EffectDuration_Type::levelIncrement(signed char levelIncrement_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectDuration_Type::levelIncrement"));

  myCurrentProperties.levelIncrement = levelIncrement_in;
}

void RPG_Common_EffectDuration_Type::levelIncrementMax(unsigned char levelIncrementMax_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectDuration_Type::levelIncrementMax"));

  myCurrentProperties.levelIncrementMax = levelIncrementMax_in;
}

void RPG_Common_EffectDuration_Type::reciprocalIncrement(unsigned char increment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectDuration_Type::reciprocalIncrement"));

  myCurrentProperties.reciprocalIncrement = increment_in;
}

RPG_Common_EffectDuration RPG_Common_EffectDuration_Type::post_RPG_Common_EffectDuration_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_EffectDuration_Type::post_RPG_Common_EffectDuration_Type"));

  RPG_Common_EffectDuration result = myCurrentProperties;

  // clear structure
  myCurrentProperties.base.value = 0;
  myCurrentProperties.base.range.numDice = 0;
  myCurrentProperties.base.range.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.base.range.modifier = 0;
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.levelIncrementMax = 0;
  myCurrentProperties.reciprocalIncrement = 0;

  return result;
}

RPG_Common_Camp RPG_Common_Camp_Type::post_RPG_Common_Camp_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Camp_Type::post_RPG_Common_Camp_Type"));

  return RPG_Common_CampHelper::stringToRPG_Common_Camp(post_string());
}

RPG_Common_Plane RPG_Common_Plane_Type::post_RPG_Common_Plane_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Plane_Type::post_RPG_Common_Plane_Type"));

  return RPG_Common_PlaneHelper::stringToRPG_Common_Plane(post_string());
}

RPG_Common_Terrain RPG_Common_Terrain_Type::post_RPG_Common_Terrain_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Terrain_Type::post_RPG_Common_Terrain_Type"));

  return RPG_Common_TerrainHelper::stringToRPG_Common_Terrain(post_string());
}

RPG_Common_Climate RPG_Common_Climate_Type::post_RPG_Common_Climate_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Climate_Type::post_RPG_Common_Climate_Type"));

  return RPG_Common_ClimateHelper::stringToRPG_Common_Climate(post_string());
}

RPG_Common_TimeOfDay RPG_Common_TimeOfDay_Type::post_RPG_Common_TimeOfDay_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_TimeOfDay_Type::post_RPG_Common_TimeOfDay_Type"));

  return RPG_Common_TimeOfDayHelper::stringToRPG_Common_TimeOfDay(post_string());
}

RPG_Common_Environment_Type::RPG_Common_Environment_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Environment_Type::RPG_Common_Environment_Type"));

  myCurrentEnvironment.terrain = RPG_COMMON_TERRAIN_INVALID;
  myCurrentEnvironment.climate = RPG_COMMON_CLIMATE_INVALID;
  myCurrentEnvironment.time = RPG_COMMON_TIMEOFDAY_INVALID;
  myCurrentEnvironment.outdoors = true;
}

void RPG_Common_Environment_Type::terrain(const RPG_Common_Terrain& terrain_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Environment_Type::terrain"));

  myCurrentEnvironment.terrain = terrain_in;
}

void RPG_Common_Environment_Type::climate(const RPG_Common_Climate& climate_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Environment_Type::climate"));

  myCurrentEnvironment.climate = climate_in;
}

void RPG_Common_Environment_Type::time(const RPG_Common_TimeOfDay& timeOfDay_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Environment_Type::time"));

  myCurrentEnvironment.time = timeOfDay_in;
}

void RPG_Common_Environment_Type::outdoors(bool outdoors_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Environment_Type::outdoors"));

  myCurrentEnvironment.outdoors = outdoors_in;
}

RPG_Common_Environment RPG_Common_Environment_Type::post_RPG_Common_Environment_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Environment_Type::post_RPG_Common_Environment_Type"));

  RPG_Common_Environment result = myCurrentEnvironment;

  // clear structure
  myCurrentEnvironment.terrain = RPG_COMMON_TERRAIN_INVALID;
  myCurrentEnvironment.climate = RPG_COMMON_CLIMATE_INVALID;
  myCurrentEnvironment.time = RPG_COMMON_TIMEOFDAY_INVALID;
  myCurrentEnvironment.outdoors = true;

  return result;
}
