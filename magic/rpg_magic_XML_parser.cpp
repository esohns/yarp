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

  myCurrentType.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentType.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentType.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentType.descriptors.clear();
}

void RPG_Magic_Spell_Type_Type::type(const RPG_Magic_SpellType& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type_Type::type"));

  myCurrentType.type = type_in;
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
  myCurrentType.type = RPG_MAGIC_SPELLTYPE_INVALID;
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

  myCurrentRange.max = 0;
  myCurrentRange.increment = 0;
  myCurrentRange.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentRange.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;
}

void RPG_Magic_Spell_Range_Type::max(unsigned int max_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Range_Type::max"));

  myCurrentRange.max = max_in;
}

void RPG_Magic_Spell_Range_Type::increment(unsigned int increment_in)
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
  myCurrentRange.max = 0;
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
  myCurrentProperties.levelIncrement = 0;
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

void RPG_Magic_Spell_DurationProperties_Type::levelIncrement(unsigned char levelIncrement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_DurationProperties_Type::levelIncrement"));

  myCurrentProperties.levelIncrement = levelIncrement_in;
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
  myCurrentProperties.levelIncrement = 0;
  myCurrentProperties.period.numDice = 0;
  myCurrentProperties.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.period.modifier = 0;
  myCurrentProperties.dismissible = false;

  return result;
}

RPG_Magic_Spell_Precondition RPG_Magic_Spell_Precondition_Type::post_RPG_Magic_Spell_Precondition_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Precondition_Type::post_RPG_Magic_Spell_Precondition_Type"));

  return RPG_Magic_Spell_PreconditionHelper::stringToRPG_Magic_Spell_Precondition(post_string());
}

RPG_Magic_Spell_PropertiesXML_Type::RPG_Magic_Spell_PropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::RPG_Magic_Spell_PropertiesXML_Type"));

  myCurrentProperties.name.resize(0);
  myCurrentProperties.type.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.type.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.type.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentProperties.type.descriptors.clear();
  myCurrentProperties.level = 0;
  myCurrentProperties.casterClasses.clear();
  myCurrentProperties.domain = DOMAIN_NONE;
  myCurrentProperties.domainLevel = 0;
  myCurrentProperties.cost = 0;
  myCurrentProperties.action = RPG_COMMON_ACTIONTYPE_INVALID;
  myCurrentProperties.range.max = 0;
  myCurrentProperties.range.increment = 0;
  myCurrentProperties.range.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentProperties.range.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;
  myCurrentProperties.duration.type = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration.duration = 0;
  myCurrentProperties.duration.period.numDice = 0;
  myCurrentProperties.duration.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.period.modifier = 0;
  myCurrentProperties.duration.dismissible = false;
  myCurrentProperties.preconditions.clear();
  myCurrentProperties.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.save.difficultyClass = 0;
  myCurrentProperties.save.reduction = RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;
  myCurrentProperties.damage.numDice = 0;
  myCurrentProperties.damage.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.damage.modifier = 0;
  myCurrentProperties.saveable = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.resistible = false;
}

void RPG_Magic_Spell_PropertiesXML_Type::name(const ::std::string& name_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::name"));

  myCurrentProperties.name = name_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::type(const RPG_Magic_Spell_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::level(unsigned char level_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::level"));

  myCurrentProperties.level = level_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::casterClass(const RPG_Common_SubClass& casterClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::casterClass"));

  myCurrentProperties.casterClasses.push_back(casterClass_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::domain(const RPG_Magic_Domain& domain_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::domain"));

  myCurrentProperties.domain = domain_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::domainLevel(unsigned char domainLevel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::domainLevel"));

  myCurrentProperties.domainLevel = domainLevel_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::cost(unsigned int cost_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::cost"));

  myCurrentProperties.cost = cost_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::action(const RPG_Common_ActionType& action_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::action"));

  myCurrentProperties.action = action_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::range(const RPG_Magic_Spell_Range& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::range"));

  myCurrentProperties.range = range_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::duration(const RPG_Magic_Spell_DurationProperties& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::precondition(const RPG_Magic_Spell_Precondition& precondition_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::precondition"));

  myCurrentProperties.preconditions.push_back(precondition_in);
}

void RPG_Magic_Spell_PropertiesXML_Type::save(const RPG_Common_SavingThrowCheck& save_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::save"));

  myCurrentProperties.save = save_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::damage(const RPG_Dice_Roll& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::damage"));

  myCurrentProperties.damage = damage_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::saveable(const RPG_Common_SavingThrow& saveable_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::saveable"));

  myCurrentProperties.saveable = saveable_in;
}

void RPG_Magic_Spell_PropertiesXML_Type::resistible(bool resistible_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::resistible"));

  myCurrentProperties.resistible = resistible_in;
}

RPG_Magic_Spell_PropertiesXML RPG_Magic_Spell_PropertiesXML_Type::post_RPG_Magic_Spell_PropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_PropertiesXML_Type::post_RPG_Magic_Spell_PropertiesXML_Type"));

  RPG_Magic_Spell_PropertiesXML result = myCurrentProperties;

  // clear structure
  myCurrentProperties.name.resize(0);
  myCurrentProperties.type.type = RPG_MAGIC_SPELLTYPE_INVALID;
  myCurrentProperties.type.school = RPG_MAGIC_SCHOOL_INVALID;
  myCurrentProperties.type.subSchool = RPG_MAGIC_SUBSCHOOL_INVALID;
  myCurrentProperties.type.descriptors.clear();
  myCurrentProperties.level = 0;
  myCurrentProperties.casterClasses.clear();
  myCurrentProperties.domain = DOMAIN_NONE;
  myCurrentProperties.domainLevel = 0;
  myCurrentProperties.cost = 0;
  myCurrentProperties.action = RPG_COMMON_ACTIONTYPE_INVALID;
  myCurrentProperties.range.max = 0;
  myCurrentProperties.range.increment = 0;
  myCurrentProperties.range.area = RPG_COMMON_AREAOFEFFECT_INVALID;
  myCurrentProperties.range.effect = RPG_MAGIC_SPELL_EFFECT_INVALID;
  myCurrentProperties.duration.type = RPG_MAGIC_SPELL_DURATION_INVALID;
  myCurrentProperties.duration.duration = 0;
  myCurrentProperties.duration.period.numDice = 0;
  myCurrentProperties.duration.period.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.duration.period.modifier = 0;
  myCurrentProperties.duration.dismissible = false;
  myCurrentProperties.preconditions.clear();
  myCurrentProperties.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.save.difficultyClass = 0;
  myCurrentProperties.save.reduction = RPG_COMMON_SAVEREDUCTIONTYPE_INVALID;
  myCurrentProperties.damage.numDice = 0;
  myCurrentProperties.damage.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.damage.modifier = 0;
  myCurrentProperties.saveable = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.resistible = false;

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

RPG_Magic_Dictionary_Type::RPG_Magic_Dictionary_Type(RPG_Magic_Dictionary_t* spellDictionary_in)
 : myDictionary(spellDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::RPG_Magic_Dictionary_Type"));

}

RPG_Magic_Dictionary_Type::~RPG_Magic_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::~RPG_Magic_Dictionary_Type"));

}

// void RPG_Magic_Dictionary_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::pre"));
//
// }

void RPG_Magic_Dictionary_Type::spell(const RPG_Magic_Spell_PropertiesXML& spell_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::spell"));

  RPG_Magic_Spell_Properties properties;
  properties.type = spell_in.type;
  properties.level = spell_in.level;
  for (std::vector<RPG_Common_SubClass>::const_iterator iterator = spell_in.casterClasses.begin();
       iterator != spell_in.casterClasses.end();
       iterator++)
  {
    properties.casterClasses.insert(*iterator);
  } // end FOR
  properties.domain = spell_in.domain;
  properties.domainLevel = spell_in.domainLevel;
  properties.cost = spell_in.cost;
  properties.action = spell_in.action;
  properties.range = spell_in.range;
  properties.duration = spell_in.duration;
  for (std::vector<RPG_Magic_Spell_Precondition>::const_iterator iterator = spell_in.preconditions.begin();
       iterator != spell_in.preconditions.end();
       iterator++)
  {
    properties.preconditions.insert(*iterator);
  } // end FOR
  properties.save = spell_in.save;
  properties.damage = spell_in.damage;
  properties.saveable = spell_in.saveable;
  properties.resistible = spell_in.resistible;

  myDictionary->insert(std::make_pair(spell_in.name, properties));
}

void RPG_Magic_Dictionary_Type::post_RPG_Magic_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary_Type::post_RPG_Magic_Dictionary_Type"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing spell dictionary, retrieved %d spell types...\n"),
             myDictionary->size()));
}
