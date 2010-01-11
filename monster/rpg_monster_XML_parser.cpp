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
#include "rpg_monster_XML_parser.h"

#include "rpg_monster_metatype.h"
#include "rpg_monster_naturalweapon.h"
#include "rpg_monster_advancement.h"

#include "rpg_character_incl.h"

#include "rpg_combat_incl.h"

#include <ace/Log_Msg.h>

RPG_Monster_Dictionary_Type::RPG_Monster_Dictionary_Type(RPG_Monster_Dictionary_t* monsterDictionary_in)
 : myMonsterDictionary(monsterDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary_Type::RPG_Monster_Dictionary_Type"));

}

RPG_Monster_Dictionary_Type::~RPG_Monster_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary_Type::~RPG_Monster_Dictionary_Type"));

}

// void RPG_Monster_Dictionary_Parser::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary_Parser::pre"));
//
// }

void RPG_Monster_Dictionary_Type::post_RPG_Monster_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary_Type::post_RPG_Monster_Dictionary_Type"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing monster dictionary, retrieved %d monster types...\n"),
             myMonsterDictionary->size()));
}

void RPG_Monster_Dictionary_Type::monster(const RPG_Monster_PropertiesXML& monster_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Dictionary_Type::monster"));

  RPG_Monster_Properties properties;
  properties.size             = monster_in.size;
  properties.type             = monster_in.type;
  properties.hitDice          = monster_in.hitDice;
  properties.initiative       = monster_in.initiative;
  properties.speed            = monster_in.speed;
  properties.armorClass       = monster_in.armorClass;
  properties.attack           = monster_in.attack;
  properties.specialAttacks   = monster_in.specialAttacks;
  properties.space            = monster_in.space;
  properties.reach            = monster_in.reach;
  properties.saves            = monster_in.saves;
  properties.attributes       = monster_in.attributes;
  for (std::vector<RPG_Character_SkillValue>::const_iterator iterator = monster_in.skills.skills.begin();
       iterator != monster_in.skills.skills.end();
       iterator++)
  {
    properties.skills.insert(std::make_pair((*iterator).skill, (*iterator).rank));
  } // end FOR
  for (std::vector<RPG_Character_Feat>::const_iterator iterator = monster_in.feats.feats.begin();
       iterator != monster_in.feats.feats.end();
       iterator++)
  {
    properties.feats.insert(*iterator);
  } // end FOR
  properties.environment      = monster_in.environment;
  for (std::vector<RPG_Monster_OrganizationStep>::const_iterator iterator = monster_in.organizations.steps.begin();
       iterator != monster_in.organizations.steps.end();
       iterator++)
  {
    properties.organizations.push_back(*iterator);
  } // end FOR
  properties.challengeRating  = monster_in.challengeRating;
  properties.treasureModifier = monster_in.treasureModifier;
  properties.alignment        = monster_in.alignment;
  for (std::vector<RPG_Monster_AdvancementStep>::const_iterator iterator = monster_in.advancements.steps.begin();
       iterator != monster_in.advancements.steps.end();
       iterator++)
  {
    properties.advancements.push_back(*iterator);
  } // end FOR
  properties.levelAdjustment = monster_in.levelAdjustment;

  myMonsterDictionary->insert(std::make_pair(monster_in.name, properties));
}

RPG_Monster_Type_Type::RPG_Monster_Type_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Type_Type::RPG_Monster_Type_Type"));

  myCurrentType.metaType = RPG_MONSTER_METATYPE_INVALID;
  myCurrentType.subTypes.clear();
}

void RPG_Monster_Type_Type::metaType(const RPG_Monster_MetaType& metaType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Type_Type::metaType"));

  myCurrentType.metaType = metaType_in;
}

void RPG_Monster_Type_Type::subType(const RPG_Monster_SubType& subType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Type_Type::subType"));

  myCurrentType.subTypes.push_back(subType_in);
}

RPG_Monster_Type RPG_Monster_Type_Type::post_RPG_Monster_Type_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Type_Type::post_RPG_Monster_Type_Type"));

  RPG_Monster_Type result = myCurrentType;

  // clear structure
  myCurrentType.metaType = RPG_MONSTER_METATYPE_INVALID;
  myCurrentType.subTypes.clear();

  return result;
}

RPG_Monster_MetaType RPG_Monster_MetaType_Type::post_RPG_Monster_MetaType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_MetaType_Type::post_RPG_Monster_MetaType_Type"));

  return RPG_Monster_MetaTypeHelper::stringToRPG_Monster_MetaType(post_string());
}

RPG_Monster_SubType RPG_Monster_SubType_Type::post_RPG_Monster_SubType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SubType_Type::post_RPG_Monster_SubType_Type"));

  return RPG_Monster_SubTypeHelper::stringToRPG_Monster_SubType(post_string());
}

RPG_Monster_NaturalArmorClass_Type::RPG_Monster_NaturalArmorClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_NaturalArmorClass_Type::RPG_Monster_NaturalArmorClass_Type"));

  myCurrentNaturalArmorClass.normal = 0;
  myCurrentNaturalArmorClass.touch = 0;
  myCurrentNaturalArmorClass.flatFooted = 0;
}

void RPG_Monster_NaturalArmorClass_Type::normal(signed char normal_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_NaturalArmorClass_Type::normal"));

  myCurrentNaturalArmorClass.normal = normal_in;
}

void RPG_Monster_NaturalArmorClass_Type::touch(signed char touch_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_NaturalArmorClass_Type::touch"));

  myCurrentNaturalArmorClass.touch = touch_in;
}

void RPG_Monster_NaturalArmorClass_Type::flatFooted(signed char flatFooted_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_NaturalArmorClass_Type::flatFooted"));

  myCurrentNaturalArmorClass.flatFooted = flatFooted_in;
}

RPG_Monster_NaturalArmorClass RPG_Monster_NaturalArmorClass_Type::post_RPG_Monster_NaturalArmorClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_NaturalArmorClass_Type::post_RPG_Monster_NaturalArmorClass_Type"));

  RPG_Monster_NaturalArmorClass result = myCurrentNaturalArmorClass;

  // clear structure
  myCurrentNaturalArmorClass.normal = 0;
  myCurrentNaturalArmorClass.touch = 0;
  myCurrentNaturalArmorClass.flatFooted = 0;

  return result;
}

RPG_Monster_NaturalWeapon RPG_Monster_NaturalWeapon_Type::post_RPG_Monster_NaturalWeapon_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_NaturalWeapon_Type::post_RPG_Monster_NaturalWeapon_Type"));

  return RPG_Monster_NaturalWeaponHelper::stringToRPG_Monster_NaturalWeapon(post_string());
}

RPG_Monster_WeaponTypeUnion_Type::RPG_Monster_WeaponTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_WeaponTypeUnion_Type::RPG_Monster_WeaponTypeUnion_Type"));

  myCurrentWeaponType.specialattack = RPG_COMBAT_SPECIALATTACK_INVALID;
  myCurrentWeaponType.naturalweapon = RPG_MONSTER_NATURALWEAPON_INVALID;
  myCurrentWeaponType.weapontype = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentWeaponType.discriminator = RPG_Monster_WeaponTypeUnion::INVALID;
}

void RPG_Monster_WeaponTypeUnion_Type::_characters(const ::xml_schema::ro_string& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_WeaponTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Combat_SpecialAttack_Type --> "SPECIALATTACK_xxx"
  // - RPG_Monster_NaturalWeapon_Type --> "NATURALWEAPON_xxx"
  // - RPG_Item_WeaponType_Type --> "WEAPONTYPE_xxx"
  std::string type = weaponType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("SPECIALATTACK_")) == 0)
  {
    myCurrentWeaponType.specialattack = RPG_Combat_SpecialAttackHelper::stringToRPG_Combat_SpecialAttack(weaponType_in);
    myCurrentWeaponType.discriminator = RPG_Monster_WeaponTypeUnion::SPECIALATTACK;
  } // end IF
  else if (type.find(ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_")) == 0)
  {
    myCurrentWeaponType.naturalweapon = RPG_Monster_NaturalWeaponHelper::stringToRPG_Monster_NaturalWeapon(weaponType_in);
    myCurrentWeaponType.discriminator = RPG_Monster_WeaponTypeUnion::NATURALWEAPON;
  } // end IF
  else
  {
    myCurrentWeaponType.weapontype = RPG_Item_WeaponTypeHelper::stringToRPG_Item_WeaponType(weaponType_in);
    myCurrentWeaponType.discriminator = RPG_Monster_WeaponTypeUnion::WEAPONTYPE;
  } // end ELSE
}

RPG_Monster_WeaponTypeUnion RPG_Monster_WeaponTypeUnion_Type::post_RPG_Monster_WeaponTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_WeaponTypeUnion_Type::post_RPG_Monster_WeaponTypeUnion_Type"));

  RPG_Monster_WeaponTypeUnion result = myCurrentWeaponType;

  // clear structure
  myCurrentWeaponType.specialattack = RPG_COMBAT_SPECIALATTACK_INVALID;
  myCurrentWeaponType.naturalweapon = RPG_MONSTER_NATURALWEAPON_INVALID;
  myCurrentWeaponType.weapontype = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentWeaponType.discriminator = RPG_Monster_WeaponTypeUnion::INVALID;

  return result;
}

RPG_Monster_AttackAction_Type::RPG_Monster_AttackAction_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::RPG_Monster_AttackAction_Type"));

  myCurrentAttackAction.weapon.weapontype = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentAttackAction.attackBonus.clear();
  myCurrentAttackAction.attackForms.clear();
  myCurrentAttackAction.damage.elements.clear();
  myCurrentAttackAction.effect.type = RPG_MAGIC_SPELL_INVALID;
  myCurrentAttackAction.effect.casterLevel = 0;
  myCurrentAttackAction.effect.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentAttackAction.effect.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentAttackAction.effect.save.difficultyClass = 0;
  myCurrentAttackAction.numAttacksPerRound = 0;
  myCurrentAttackAction.ranged.maxRange = 0;
  myCurrentAttackAction.ranged.increment = 0;
  myCurrentAttackAction.ranged.effect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;
  myCurrentAttackAction.fullAttackIncludesNextAction = false;
}

void RPG_Monster_AttackAction_Type::weapon(const RPG_Monster_WeaponTypeUnion& weapon_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::weapon"));

  myCurrentAttackAction.weapon = weapon_in;
}

void RPG_Monster_AttackAction_Type::attackBonus(signed char attackBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::attackBonus"));

  myCurrentAttackAction.attackBonus.push_back(attackBonus_in);
}

void RPG_Monster_AttackAction_Type::attackForm(const RPG_Combat_AttackForm& attackForm_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::attackForm"));

  myCurrentAttackAction.attackForms.push_back(attackForm_in);
}

void RPG_Monster_AttackAction_Type::damage(const RPG_Combat_Damage& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::damage"));

  myCurrentAttackAction.damage = damage_in;
}

void RPG_Monster_AttackAction_Type::effect(const RPG_Magic_SpellProperties& effect_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::effect"));

  myCurrentAttackAction.effect = effect_in;
}

void RPG_Monster_AttackAction_Type::numAttacksPerRound(unsigned char numAttacksPerRound_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::numAttacksPerRound"));

  myCurrentAttackAction.numAttacksPerRound = numAttacksPerRound_in;
}

void RPG_Monster_AttackAction_Type::ranged(const RPG_Combat_RangedAttackProperties& rangedProperties_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::ranged"));

  myCurrentAttackAction.ranged = rangedProperties_in;
}

void RPG_Monster_AttackAction_Type::fullAttackIncludesNextAction(bool includeNextAction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::fullAttackIncludesNextAction"));

  myCurrentAttackAction.fullAttackIncludesNextAction = includeNextAction_in;
}

RPG_Monster_AttackAction RPG_Monster_AttackAction_Type::post_RPG_Monster_AttackAction_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AttackAction_Type::post_RPG_Monster_AttackAction_Type"));

  RPG_Monster_AttackAction result = myCurrentAttackAction;

  // clear structure
  myCurrentAttackAction.weapon.weapontype = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentAttackAction.attackBonus.clear();
  myCurrentAttackAction.attackForms.clear();
  myCurrentAttackAction.damage.elements.clear();
  myCurrentAttackAction.effect.type = RPG_MAGIC_SPELL_INVALID;
  myCurrentAttackAction.effect.casterLevel = 0;
  myCurrentAttackAction.effect.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentAttackAction.effect.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentAttackAction.effect.save.difficultyClass = 0;
  myCurrentAttackAction.numAttacksPerRound = 0;
  myCurrentAttackAction.ranged.maxRange = 0;
  myCurrentAttackAction.ranged.increment = 0;
  myCurrentAttackAction.ranged.effect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;
  myCurrentAttackAction.fullAttackIncludesNextAction = false;

  return result;
}

RPG_Monster_Attack_Type::RPG_Monster_Attack_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::RPG_Monster_Attack_Type"));

  myCurrentAttack.baseAttackBonus = 0;
  myCurrentAttack.grappleBonus = 0;
  myCurrentAttack.standardAttackActions.clear();
  myCurrentAttack.fullAttackActions.clear();
  myCurrentAttack.attackActionsAreInclusive = true;
}

void RPG_Monster_Attack_Type::baseAttackBonus(signed char baseAttackBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::baseAttackBonus"));

  myCurrentAttack.baseAttackBonus = baseAttackBonus_in;
}

void RPG_Monster_Attack_Type::grappleBonus(signed char grappleBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::grappleBonus"));

  myCurrentAttack.grappleBonus = grappleBonus_in;
}

void RPG_Monster_Attack_Type::standardAttackAction(const RPG_Monster_AttackAction& attackAction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::standardAttackAction"));

  myCurrentAttack.standardAttackActions.push_back(attackAction_in);
}

void RPG_Monster_Attack_Type::fullAttackAction(const RPG_Monster_AttackAction& attackAction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::fullAttackAction"));

  myCurrentAttack.fullAttackActions.push_back(attackAction_in);
}

void RPG_Monster_Attack_Type::attackActionsAreInclusive(bool actionsAreInclusive_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::fullAttackAction"));

  myCurrentAttack.attackActionsAreInclusive = actionsAreInclusive_in;
}

RPG_Monster_Attack RPG_Monster_Attack_Type::post_RPG_Monster_Attack_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Attack_Type::post_RPG_Monster_Attack_Type"));

  RPG_Monster_Attack result = myCurrentAttack;

  // clear structure
  myCurrentAttack.baseAttackBonus = 0;
  myCurrentAttack.grappleBonus = 0;
  myCurrentAttack.standardAttackActions.clear();
  myCurrentAttack.fullAttackActions.clear();
  myCurrentAttack.attackActionsAreInclusive = true;

  return result;
}

RPG_Monster_SpecialAttackTypeUnion_Type::RPG_Monster_SpecialAttackTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackTypeUnion_Type::RPG_Monster_SpecialAttackTypeUnion_Type"));

  myCurrentUnionType.ability = RPG_CHARACTER_ABILITY_INVALID;
  myCurrentUnionType.abilitytype = RPG_MAGIC_ABILITYTYPE_INVALID;
  myCurrentUnionType.discriminator = RPG_Monster_SpecialAttackTypeUnion::INVALID;
}

void RPG_Monster_SpecialAttackTypeUnion_Type::_characters(const ::xml_schema::ro_string& abilityType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Character_Ability --> "ABILITY_xxx"
  // - RPG_Magic_AbilityType --> "ABILITYTYPE_xxx"
  std::string type = abilityType_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("ABILITYTYPE_")) == 0)
  {
    myCurrentUnionType.abilitytype = RPG_Magic_AbilityTypeHelper::stringToRPG_Magic_AbilityType(type);
    myCurrentUnionType.discriminator = RPG_Monster_SpecialAttackTypeUnion::ABILITYTYPE;
  } // end IF
  else
  {
    myCurrentUnionType.ability = RPG_Character_AbilityHelper::stringToRPG_Character_Ability(type);
    myCurrentUnionType.discriminator = RPG_Monster_SpecialAttackTypeUnion::ABILITY;
  } // end ELSE
}

RPG_Monster_SpecialAttackTypeUnion RPG_Monster_SpecialAttackTypeUnion_Type::post_RPG_Monster_SpecialAttackTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackTypeUnion_Type::post_RPG_Monster_SpecialAttackTypeUnion_Type"));

  RPG_Monster_SpecialAttackTypeUnion result = myCurrentUnionType;

  // clear structure
  myCurrentUnionType.ability = RPG_CHARACTER_ABILITY_INVALID;
  myCurrentUnionType.abilitytype = RPG_MAGIC_ABILITYTYPE_INVALID;
  myCurrentUnionType.discriminator = RPG_Monster_SpecialAttackTypeUnion::INVALID;

  return result;
}

RPG_Monster_SpecialAttackPreCondition_Type::RPG_Monster_SpecialAttackPreCondition_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::RPG_Monster_SpecialAttackPreCondition_Type"));

  myCurrentPreCondition.targetAlignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentPreCondition.targetAlignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  myCurrentPreCondition.ownConditions.clear();
  myCurrentPreCondition.targetConditions.clear();
  myCurrentPreCondition.minTargetSize = RPG_CHARACTER_SIZE_INVALID;
  myCurrentPreCondition.maxTargetSize = RPG_CHARACTER_SIZE_INVALID;
  myCurrentPreCondition.check.type.skill = RPG_CHARACTER_SKILL_INVALID;
  myCurrentPreCondition.check.type.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentPreCondition.check.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentPreCondition.check.difficultyClass = 0;
}

void RPG_Monster_SpecialAttackPreCondition_Type::targetAlignment(const RPG_Character_Alignment& targetAlignment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::targetAlignment"));

  myCurrentPreCondition.targetAlignment = targetAlignment_in;
}

void RPG_Monster_SpecialAttackPreCondition_Type::ownCondition(const RPG_Character_Condition& condition_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::ownCondition"));

  myCurrentPreCondition.ownConditions.push_back(condition_in);
}

void RPG_Monster_SpecialAttackPreCondition_Type::targetCondition(const RPG_Character_Condition& condition_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::targetCondition"));

  myCurrentPreCondition.targetConditions.push_back(condition_in);
}

void RPG_Monster_SpecialAttackPreCondition_Type::minTargetSize(const RPG_Character_Size& minTargetSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::minTargetSize"));

  myCurrentPreCondition.minTargetSize = minTargetSize_in;
}

void RPG_Monster_SpecialAttackPreCondition_Type::maxTargetSize(const RPG_Character_Size& maxTargetSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::maxTargetSize"));

  myCurrentPreCondition.maxTargetSize = maxTargetSize_in;
}

void RPG_Monster_SpecialAttackPreCondition_Type::check(const RPG_Combat_Check& check_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::check"));

  myCurrentPreCondition.check = check_in;
}

RPG_Monster_SpecialAttackPreCondition RPG_Monster_SpecialAttackPreCondition_Type::post_RPG_Monster_SpecialAttackPreCondition_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackPreCondition_Type::post_RPG_Monster_SpecialAttackPreCondition_Type"));

  RPG_Monster_SpecialAttackPreCondition result = myCurrentPreCondition;

  // clear structure
  myCurrentPreCondition.targetAlignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentPreCondition.targetAlignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  myCurrentPreCondition.ownConditions.clear();
  myCurrentPreCondition.targetConditions.clear();
  myCurrentPreCondition.minTargetSize = RPG_CHARACTER_SIZE_INVALID;
  myCurrentPreCondition.maxTargetSize = RPG_CHARACTER_SIZE_INVALID;
  myCurrentPreCondition.check.type.skill = RPG_CHARACTER_SKILL_INVALID;
  myCurrentPreCondition.check.type.discriminator = RPG_Combat_CheckTypeUnion::INVALID;
  myCurrentPreCondition.check.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentPreCondition.check.difficultyClass = 0;

  return result;
}

RPG_Monster_SpecialAttackProperties_Type::RPG_Monster_SpecialAttackProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::RPG_Monster_SpecialAttackProperties_Type"));

  myCurrentProperties.abilityClass = RPG_MAGIC_ABILITYCLASS_INVALID;
  myCurrentProperties.type.ability = RPG_CHARACTER_ABILITY_INVALID;
  myCurrentProperties.type.discriminator = RPG_Monster_SpecialAttackTypeUnion::INVALID;
  myCurrentProperties.actionType = RPG_COMBAT_ACTIONTYPE_INVALID;
  myCurrentProperties.preConditions.clear();
  myCurrentProperties.action.weapon.weapontype = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentProperties.action.attackBonus.clear();
  myCurrentProperties.action.attackForms.clear();
  myCurrentProperties.action.damage.elements.clear();
  myCurrentProperties.action.numAttacksPerRound = 0;
  myCurrentProperties.action.ranged.maxRange = 0;
  myCurrentProperties.action.ranged.increment = 0;
  myCurrentProperties.action.ranged.effect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;
  myCurrentProperties.action.fullAttackIncludesNextAction = false;
//   myCurrentProperties.amount.numDice = 0;
//   myCurrentProperties.amount.typeDice = RPG_DICE_DIETYPE_INVALID;
//   myCurrentProperties.amount.modifier = 0;
  myCurrentProperties.usage.numUses = 0;
  myCurrentProperties.usage.period = 0;
}

void RPG_Monster_SpecialAttackProperties_Type::abilityClass(const RPG_Magic_AbilityClass& abilityClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::abilityClass"));

  myCurrentProperties.abilityClass = abilityClass_in;
}

void RPG_Monster_SpecialAttackProperties_Type::type(const RPG_Monster_SpecialAttackTypeUnion& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Monster_SpecialAttackProperties_Type::actionType(const RPG_Combat_ActionType& actionType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::actionType"));

  myCurrentProperties.actionType = actionType_in;
}

void RPG_Monster_SpecialAttackProperties_Type::preCondition(const RPG_Monster_SpecialAttackPreCondition& preCondition_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::preCondition"));

  myCurrentProperties.preConditions.push_back(preCondition_in);
}

void RPG_Monster_SpecialAttackProperties_Type::action(const RPG_Monster_AttackAction& action_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::action"));

  myCurrentProperties.action = action_in;
}

// void RPG_Monster_SpecialAttackProperties_Type::amount(const RPG_Dice_Roll& amount_in)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::amount"));
//
//   myCurrentProperties.amount = amount_in;
// }

void RPG_Monster_SpecialAttackProperties_Type::usage(const RPG_Common_Usage& usage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::usage"));

  myCurrentProperties.usage = usage_in;
}

RPG_Monster_SpecialAttackProperties RPG_Monster_SpecialAttackProperties_Type::post_RPG_Monster_SpecialAttackProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SpecialAttackProperties_Type::post_RPG_Monster_SpecialAttackProperties_Type"));

  RPG_Monster_SpecialAttackProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.abilityClass = RPG_MAGIC_ABILITYCLASS_INVALID;
  myCurrentProperties.type.ability = RPG_CHARACTER_ABILITY_INVALID;
  myCurrentProperties.type.discriminator = RPG_Monster_SpecialAttackTypeUnion::INVALID;
  myCurrentProperties.actionType = RPG_COMBAT_ACTIONTYPE_INVALID;
  myCurrentProperties.preConditions.clear();
  myCurrentProperties.action.weapon.weapontype = RPG_ITEM_WEAPONTYPE_INVALID;
  myCurrentProperties.action.attackBonus.clear();
  myCurrentProperties.action.attackForms.clear();
  myCurrentProperties.action.damage.elements.clear();
  myCurrentProperties.action.numAttacksPerRound = 0;
  myCurrentProperties.action.ranged.maxRange = 0;
  myCurrentProperties.action.ranged.increment = 0;
  myCurrentProperties.action.ranged.effect.areaofeffect = RPG_COMBAT_AREAOFEFFECT_INVALID;
  myCurrentProperties.action.fullAttackIncludesNextAction = false;
//   myCurrentProperties.amount.numDice = 0;
//   myCurrentProperties.amount.typeDice = RPG_DICE_DIETYPE_INVALID;
//   myCurrentProperties.amount.modifier = 0;
  myCurrentProperties.usage.numUses = 0;
  myCurrentProperties.usage.period = 0;

  return result;
}

RPG_Monster_SavingThrowModifiers_Type::RPG_Monster_SavingThrowModifiers_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SavingThrowModifiers_Type::RPG_Monster_SavingThrowModifiers_Type"));

  myCurrentSavingThrowModifiers.fortitude = 0;
  myCurrentSavingThrowModifiers.reflex = 0;
  myCurrentSavingThrowModifiers.will = 0;
}

void RPG_Monster_SavingThrowModifiers_Type::fortitude(signed char fortitude_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SavingThrowModifiers_Type::fortitude"));

  myCurrentSavingThrowModifiers.fortitude = fortitude_in;
}

void RPG_Monster_SavingThrowModifiers_Type::reflex(signed char reflex_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SavingThrowModifiers_Type::reflex"));

  myCurrentSavingThrowModifiers.reflex = reflex_in;
}

void RPG_Monster_SavingThrowModifiers_Type::will(signed char will_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SavingThrowModifiers_Type::will"));

  myCurrentSavingThrowModifiers.will = will_in;
}

RPG_Monster_SavingThrowModifiers RPG_Monster_SavingThrowModifiers_Type::post_RPG_Monster_SavingThrowModifiers_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_SavingThrowModifiers_Type::post_RPG_Monster_SavingThrowModifiers_Type"));

  RPG_Monster_SavingThrowModifiers result = myCurrentSavingThrowModifiers;

  // clear structure
  myCurrentSavingThrowModifiers.fortitude = 0;
  myCurrentSavingThrowModifiers.reflex = 0;
  myCurrentSavingThrowModifiers.will = 0;

  return result;
}

RPG_Monster_Organization RPG_Monster_Organization_Type::post_RPG_Monster_Organization_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Organization_Type::post_RPG_Monster_Organization_Type"));

  return RPG_Monster_OrganizationHelper::stringToRPG_Monster_Organization(post_string());
}

RPG_Monster_OrganizationSlaverStep_Type::RPG_Monster_OrganizationSlaverStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationSlaverStep_Type::RPG_Monster_OrganizationSlaverStep_Type"));

  myCurrentOrganizationSlaverStep.name.clear();
  myCurrentOrganizationSlaverStep.range.begin = 0;
  myCurrentOrganizationSlaverStep.range.end = 0;
}

void RPG_Monster_OrganizationSlaverStep_Type::name(const std::string& name_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationSlaverStep_Type::name"));

  myCurrentOrganizationSlaverStep.name = name_in;
}

void RPG_Monster_OrganizationSlaverStep_Type::range(const RPG_Dice_ValueRange& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationSlaverStep_Type::range"));

  myCurrentOrganizationSlaverStep.range = range_in;
}

RPG_Monster_OrganizationSlaverStep RPG_Monster_OrganizationSlaverStep_Type::post_RPG_Monster_OrganizationSlaverStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationSlaverStep_Type::post_RPG_Monster_OrganizationSlaverStep_Type"));

  RPG_Monster_OrganizationSlaverStep result = myCurrentOrganizationSlaverStep;

  // clear structure
  myCurrentOrganizationSlaverStep.name.clear();
  myCurrentOrganizationSlaverStep.range.begin = 0;
  myCurrentOrganizationSlaverStep.range.end = 0;

  return result;
}

RPG_Monster_OrganizationStep_Type::RPG_Monster_OrganizationStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationStep_Type::RPG_Monster_OrganizationStep_Type"));

  myCurrentOrganizationStep.type = RPG_MONSTER_ORGANIZATION_INVALID;
  myCurrentOrganizationStep.range.begin = 0;
  myCurrentOrganizationStep.range.end = 0;
  myCurrentOrganizationStep.slaves.clear();
}

void RPG_Monster_OrganizationStep_Type::type(const RPG_Monster_Organization& organization_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationStep_Type::type"));

  myCurrentOrganizationStep.type = organization_in;
}

void RPG_Monster_OrganizationStep_Type::range(const RPG_Dice_ValueRange& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationStep_Type::range"));

  myCurrentOrganizationStep.range = range_in;
}

void RPG_Monster_OrganizationStep_Type::slaves(const RPG_Monster_OrganizationSlaverStep& organizationSlaverStep_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationStep_Type::slaves"));

  myCurrentOrganizationStep.slaves.push_back(organizationSlaverStep_in);
}

RPG_Monster_OrganizationStep RPG_Monster_OrganizationStep_Type::post_RPG_Monster_OrganizationStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_OrganizationStep_Type::post_RPG_Monster_OrganizationStep_Type"));

  RPG_Monster_OrganizationStep result = myCurrentOrganizationStep;

  // if necessary, adjust range
  if (result.type == ORGANIZATION_SOLITARY)
  {
    result.range.begin = 1;
    result.range.end = 1;
  } // end IF
  else if (result.type == ORGANIZATION_PAIR)
  {
    result.range.begin = 2;
    result.range.end = 2;
  } // end IF

  // clear structure
  myCurrentOrganizationStep.type = RPG_MONSTER_ORGANIZATION_INVALID;
  myCurrentOrganizationStep.range.begin = 0;
  myCurrentOrganizationStep.range.end = 0;
  myCurrentOrganizationStep.slaves.clear();

  return result;
}

RPG_Monster_Organizations_Type::RPG_Monster_Organizations_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Organizations_Type::RPG_Monster_Organizations_Type"));

  myCurrentOrganizations.steps.clear();
}

void RPG_Monster_Organizations_Type::step(const RPG_Monster_OrganizationStep& organizationStep_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Organizations_Type::step"));

  myCurrentOrganizations.steps.push_back(organizationStep_in);
}

RPG_Monster_Organizations RPG_Monster_Organizations_Type::post_RPG_Monster_Organizations_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Organizations_Type::post_RPG_Monster_Organizations_Type"));

  RPG_Monster_Organizations result = myCurrentOrganizations;

  // clear structure
  myCurrentOrganizations.steps.clear();

  return result;
}

RPG_Monster_AdvancementStep_Type::RPG_Monster_AdvancementStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AdvancementStep_Type::RPG_Monster_AdvancementStep_Type"));

  myCurrentAdvancementStep.size = RPG_CHARACTER_SIZE_INVALID;
  myCurrentAdvancementStep.range.begin = 0;
  myCurrentAdvancementStep.range.end = 0;
}

void RPG_Monster_AdvancementStep_Type::size(const RPG_Character_Size& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AdvancementStep_Type::size"));

  myCurrentAdvancementStep.size = size_in;
}

void RPG_Monster_AdvancementStep_Type::range(const RPG_Dice_ValueRange& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AdvancementStep_Type::range"));

  myCurrentAdvancementStep.range = range_in;
}

RPG_Monster_AdvancementStep RPG_Monster_AdvancementStep_Type::post_RPG_Monster_AdvancementStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_AdvancementStep_Type::post_RPG_Monster_AdvancementStep_Type"));

  RPG_Monster_AdvancementStep result = myCurrentAdvancementStep;

  // clear structure
  myCurrentAdvancementStep.size = RPG_CHARACTER_SIZE_INVALID;
  myCurrentAdvancementStep.range.begin = 0;
  myCurrentAdvancementStep.range.end = 0;

  return result;
}

RPG_Monster_Advancement_Type::RPG_Monster_Advancement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Advancement_Type::RPG_Monster_Advancement_Type"));

  myCurrentAdvancement.steps.clear();
}

void RPG_Monster_Advancement_Type::step(const RPG_Monster_AdvancementStep& advancementStep_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Advancement_Type::step"));

  myCurrentAdvancement.steps.push_back(advancementStep_in);
}

RPG_Monster_Advancement RPG_Monster_Advancement_Type::post_RPG_Monster_Advancement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_Advancement_Type::post_RPG_Monster_Advancement_Type"));

  RPG_Monster_Advancement result = myCurrentAdvancement;

  // clear container
  myCurrentAdvancement.steps.clear();

  return result;
}

RPG_Monster_PropertiesXML_Type::RPG_Monster_PropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::RPG_Monster_PropertiesXML_Type"));

  myCurrentProperties.name.resize(0);
  myCurrentProperties.size = RPG_CHARACTER_SIZE_INVALID;
  myCurrentProperties.type.metaType = RPG_MONSTER_METATYPE_INVALID;
  myCurrentProperties.type.subTypes.clear();
  myCurrentProperties.hitDice.numDice = 0;
  myCurrentProperties.hitDice.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.hitDice.modifier = 0;
  myCurrentProperties.initiative = 0;
  myCurrentProperties.speed = 0;
  myCurrentProperties.armorClass.normal = 0;
  myCurrentProperties.armorClass.touch = 0;
  myCurrentProperties.armorClass.flatFooted = 0;
  myCurrentProperties.attack.baseAttackBonus = 0;
  myCurrentProperties.attack.grappleBonus = 0;
  myCurrentProperties.attack.standardAttackActions.clear();
  myCurrentProperties.attack.fullAttackActions.clear();
  myCurrentProperties.attack.attackActionsAreInclusive = true;
  myCurrentProperties.specialAttacks.clear();
  myCurrentProperties.space = 0;
  myCurrentProperties.reach = 0;
  myCurrentProperties.saves.fortitude = 0;
  myCurrentProperties.saves.reflex = 0;
  myCurrentProperties.saves.will = 0;
  myCurrentProperties.attributes.strength = 0;
  myCurrentProperties.attributes.dexterity = 0;
  myCurrentProperties.attributes.constitution = 0;
  myCurrentProperties.attributes.intelligence = 0;
  myCurrentProperties.attributes.wisdom = 0;
  myCurrentProperties.attributes.charisma = 0;
  myCurrentProperties.skills.skills.clear();
  myCurrentProperties.feats.feats.clear();
  myCurrentProperties.environment.climate = RPG_CHARACTER_CLIMATE_INVALID;
  myCurrentProperties.environment.terrain = RPG_CHARACTER_TERRAIN_INVALID;
  myCurrentProperties.organizations.steps.clear();
  myCurrentProperties.challengeRating = 0;
  myCurrentProperties.treasureModifier = 0;
  myCurrentProperties.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentProperties.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  myCurrentProperties.advancements.steps.clear();
  myCurrentProperties.levelAdjustment = 0;
}

void RPG_Monster_PropertiesXML_Type::name(const std::string& name_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::name"));

  myCurrentProperties.name = name_in;
}

void RPG_Monster_PropertiesXML_Type::size(const RPG_Character_Size& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::size"));

  myCurrentProperties.size = size_in;
}

void RPG_Monster_PropertiesXML_Type::type(const RPG_Monster_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Monster_PropertiesXML_Type::hitDice(const RPG_Dice_Roll& hitDice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::hitDice"));

  myCurrentProperties.hitDice = hitDice_in;
}

void RPG_Monster_PropertiesXML_Type::initiative(signed char initiative_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::initiative"));

  myCurrentProperties.initiative = initiative_in;
}

void RPG_Monster_PropertiesXML_Type::speed(unsigned char speed_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::speed"));

  myCurrentProperties.speed = speed_in;
}

void RPG_Monster_PropertiesXML_Type::armorClass(const RPG_Monster_NaturalArmorClass& armorClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::armorClass"));

  myCurrentProperties.armorClass = armorClass_in;
}

void RPG_Monster_PropertiesXML_Type::attack(const RPG_Monster_Attack& attack_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::attack"));

  myCurrentProperties.attack = attack_in;
}

void RPG_Monster_PropertiesXML_Type::specialAttack(const RPG_Monster_SpecialAttackProperties& property_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::specialAttack"));

  myCurrentProperties.specialAttacks.push_back(property_in);
}

void RPG_Monster_PropertiesXML_Type::space(unsigned char space_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::space"));

  myCurrentProperties.space = space_in;
}

void RPG_Monster_PropertiesXML_Type::reach(unsigned char reach_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::reach"));

  myCurrentProperties.reach = reach_in;
}

void RPG_Monster_PropertiesXML_Type::saves(const RPG_Monster_SavingThrowModifiers& saves_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::saves"));

  myCurrentProperties.saves = saves_in;
}

void RPG_Monster_PropertiesXML_Type::attributes(const RPG_Character_Attributes& attributes_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::attributes"));

  myCurrentProperties.attributes = attributes_in;
}

void RPG_Monster_PropertiesXML_Type::skills(const RPG_Character_Skills& skills_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::skills"));

  myCurrentProperties.skills = skills_in;
}

void RPG_Monster_PropertiesXML_Type::feats(const RPG_Character_Feats& feats_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::feats"));

  myCurrentProperties.feats = feats_in;
}

void RPG_Monster_PropertiesXML_Type::environment(const RPG_Character_Environment& environment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::environment"));

  myCurrentProperties.environment = environment_in;
}

void RPG_Monster_PropertiesXML_Type::organizations(const RPG_Monster_Organizations& organizations_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::organizations"));

  myCurrentProperties.organizations = organizations_in;
}

void RPG_Monster_PropertiesXML_Type::challengeRating(unsigned char challengeRating_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::challengeRating"));

  myCurrentProperties.challengeRating = challengeRating_in;
}

void RPG_Monster_PropertiesXML_Type::treasureModifier(unsigned char treasureModifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::treasureModifier"));

  myCurrentProperties.treasureModifier = treasureModifier_in;
}

void RPG_Monster_PropertiesXML_Type::alignment(const RPG_Character_Alignment& alignment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::alignment"));

  myCurrentProperties.alignment = alignment_in;
}

void RPG_Monster_PropertiesXML_Type::advancement(const RPG_Monster_Advancement& advancement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::advancement"));

  myCurrentProperties.advancements = advancement_in;
}

void RPG_Monster_PropertiesXML_Type::levelAdjustment(unsigned char levelAdjustment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::levelAdjustment"));

  myCurrentProperties.levelAdjustment = levelAdjustment_in;
}

RPG_Monster_PropertiesXML RPG_Monster_PropertiesXML_Type::post_RPG_Monster_PropertiesXML_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Monster_PropertiesXML_Type::post_RPG_Monster_PropertiesXML_Type"));

  RPG_Monster_PropertiesXML result = myCurrentProperties;

  // clear structure
  myCurrentProperties.name.resize(0);
  myCurrentProperties.size = RPG_CHARACTER_SIZE_INVALID;
  myCurrentProperties.type.metaType = RPG_MONSTER_METATYPE_INVALID;
  myCurrentProperties.type.subTypes.clear();
  myCurrentProperties.hitDice.numDice = 0;
  myCurrentProperties.hitDice.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentProperties.hitDice.modifier = 0;
  myCurrentProperties.initiative = 0;
  myCurrentProperties.speed = 0;
  myCurrentProperties.armorClass.normal = 0;
  myCurrentProperties.armorClass.touch = 0;
  myCurrentProperties.armorClass.flatFooted = 0;
  myCurrentProperties.attack.baseAttackBonus = 0;
  myCurrentProperties.attack.grappleBonus = 0;
  myCurrentProperties.attack.standardAttackActions.clear();
  myCurrentProperties.attack.fullAttackActions.clear();
  myCurrentProperties.attack.attackActionsAreInclusive = true;
  myCurrentProperties.specialAttacks.clear();
  myCurrentProperties.space = 0;
  myCurrentProperties.reach = 0;
  myCurrentProperties.saves.fortitude = 0;
  myCurrentProperties.saves.reflex = 0;
  myCurrentProperties.saves.will = 0;
  myCurrentProperties.attributes.strength = 0;
  myCurrentProperties.attributes.dexterity = 0;
  myCurrentProperties.attributes.constitution = 0;
  myCurrentProperties.attributes.intelligence = 0;
  myCurrentProperties.attributes.wisdom = 0;
  myCurrentProperties.attributes.charisma = 0;
  myCurrentProperties.skills.skills.clear();
  myCurrentProperties.feats.feats.clear();
  myCurrentProperties.environment.climate = RPG_CHARACTER_CLIMATE_INVALID;
  myCurrentProperties.environment.terrain = RPG_CHARACTER_TERRAIN_INVALID;
  myCurrentProperties.organizations.steps.clear();
  myCurrentProperties.challengeRating = 0;
  myCurrentProperties.treasureModifier = 0;
  myCurrentProperties.alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  myCurrentProperties.alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  myCurrentProperties.advancements.steps.clear();
  myCurrentProperties.levelAdjustment = 0;

  return result;
}
