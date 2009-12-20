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

RPG_Combat_DamageEffectType RPG_Combat_DamageEffectType_Type::post_RPG_Combat_DamageEffectType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageEffectType_Type::post_RPG_Combat_DamageEffectType_Type"));

  return RPG_Combat_DamageEffectTypeHelper::stringToRPG_Combat_DamageEffectType(post_string());
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
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageTypeUnion_Type::RPG_Combat_DamageTypeUnion_Type"));

  RPG_Combat_DamageTypeUnion result = myCurrentDamageType;

  // clear structure
  myCurrentDamageType.physicaldamagetype = RPG_ITEM_PHYSICALDAMAGETYPE_INVALID;
  myCurrentDamageType.specialdamagetype = RPG_COMBAT_SPECIALDAMAGETYPE_INVALID;

  return result;
}

RPG_Combat_DamageElement_Type::RPG_Combat_DamageElement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::RPG_Combat_DamageElement_Type"));

  myCurrentDamageElement.type.physicaldamagetype = RPG_ITEM_PHYSICALDAMAGETYPE_INVALID;
  myCurrentDamageElement.damage.numDice = 0;
  myCurrentDamageElement.damage.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.damage.modifier = 0;
  myCurrentDamageElement.duration = 0;
  myCurrentDamageElement.effect = RPG_COMBAT_DAMAGEEFFECTTYPE_INVALID;
}

void RPG_Combat_DamageElement_Type::type(const RPG_Combat_DamageTypeUnion& damageType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::type"));

  myCurrentDamageElement.type = damageType_in;
}

void RPG_Combat_DamageElement_Type::damage(const RPG_Dice_Roll& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::damage"));

  myCurrentDamageElement.damage = damage_in;
}

void RPG_Combat_DamageElement_Type::duration(unsigned short duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_DamageElement_Type::duration"));

  myCurrentDamageElement.duration = duration_in;
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
  myCurrentDamageElement.type.physicaldamagetype = RPG_ITEM_PHYSICALDAMAGETYPE_INVALID;
  myCurrentDamageElement.damage.numDice = 0;
  myCurrentDamageElement.damage.typeDice = RPG_DICE_DIETYPE_INVALID;
  myCurrentDamageElement.damage.modifier = 0;
  myCurrentDamageElement.duration = 0;
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
