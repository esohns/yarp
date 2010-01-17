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
#ifndef RPG_COMBAT_XML_PARSER_H
#define RPG_COMBAT_XML_PARSER_H

#include "rpg_combat_XML_types.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Combat_AttackForm_Type
 : public RPG_Combat_AttackForm_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_AttackForm post_RPG_Combat_AttackForm_Type();
};

class RPG_Combat_AreaOfEffect_Type
 : public RPG_Combat_AreaOfEffect_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_AreaOfEffect post_RPG_Combat_AreaOfEffect_Type();
};

class RPG_Combat_RangedEffectUnion_Type
 : public RPG_Combat_RangedEffectUnion_Type_pskel
{
 public:
  RPG_Combat_RangedEffectUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Combat_RangedEffectUnion post_RPG_Combat_RangedEffectUnion_Type();

 private:
  RPG_Combat_RangedEffectUnion myCurrentRangedEffect;
};

class RPG_Combat_RangedAttackProperties_Type
 : public RPG_Combat_RangedAttackProperties_Type_pskel
{
 public:
  RPG_Combat_RangedAttackProperties_Type();

//   virtual void pre();
  virtual void maxRange(unsigned char);
  virtual void increment(unsigned char);
  virtual void effect(const RPG_Combat_RangedEffectUnion&);
  virtual RPG_Combat_RangedAttackProperties post_RPG_Combat_RangedAttackProperties_Type();

 private:
  RPG_Combat_RangedAttackProperties myCurrentRangedProperties;
};

class RPG_Combat_AttackSituation_Type
 : public RPG_Combat_AttackSituation_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_AttackSituation post_RPG_Combat_AttackSituation_Type();
};

class RPG_Combat_DefenseSituation_Type
 : public RPG_Combat_DefenseSituation_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_DefenseSituation post_RPG_Combat_DefenseSituation_Type();
};

class RPG_Combat_SpecialAttack_Type
 : public RPG_Combat_SpecialAttack_Type_pskel,
   public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
   virtual RPG_Combat_SpecialAttack post_RPG_Combat_SpecialAttack_Type();
};

class RPG_Combat_SpecialDamageType_Type
 : public RPG_Combat_SpecialDamageType_Type_pskel,
   public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
   virtual RPG_Combat_SpecialDamageType post_RPG_Combat_SpecialDamageType_Type();
};

class RPG_Combat_DamageTypeUnion_Type
 : public RPG_Combat_DamageTypeUnion_Type_pskel
{
 public:
  RPG_Combat_DamageTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Combat_DamageTypeUnion post_RPG_Combat_DamageTypeUnion_Type();

 private:
  RPG_Combat_DamageTypeUnion myCurrentDamageType;
};

class RPG_Combat_DamageDuration_Type
 : public RPG_Combat_DamageDuration_Type_pskel
{
 public:
  RPG_Combat_DamageDuration_Type();

//   virtual void pre();
  virtual void incubationPeriod(const RPG_Dice_Roll&);
  virtual void interval(unsigned short);
  virtual void totalDuration(unsigned short);
  virtual RPG_Combat_DamageDuration post_RPG_Combat_DamageDuration_Type();

 private:
  RPG_Combat_DamageDuration myCurrentDuration;
};

class RPG_Combat_DamageBonusType_Type
 : public RPG_Combat_DamageBonusType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_DamageBonusType post_RPG_Combat_DamageBonusType_Type();
};

class RPG_Combat_DamageBonus_Type
 : public RPG_Combat_DamageBonus_Type_pskel
{
 public:
  RPG_Combat_DamageBonus_Type();

//   virtual void pre();
  virtual void type(const RPG_Combat_DamageBonusType&);
  virtual void modifier(signed char);
  virtual RPG_Combat_DamageBonus post_RPG_Combat_DamageBonus_Type();

 private:
  RPG_Combat_DamageBonus myCurrentDamageBonus;
};

class RPG_Combat_DamageCounterMeasureType_Type
 : public RPG_Combat_DamageCounterMeasureType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_DamageCounterMeasureType post_RPG_Combat_DamageCounterMeasureType_Type();
};

class RPG_Combat_CheckTypeUnion_Type
 : public RPG_Combat_CheckTypeUnion_Type_pskel
{
 public:
  RPG_Combat_CheckTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Combat_CheckTypeUnion post_RPG_Combat_CheckTypeUnion_Type();

 private:
  RPG_Combat_CheckTypeUnion myCurrentCheckTypeUnion;
};

class RPG_Combat_Check_Type
 : public RPG_Combat_Check_Type_pskel
{
 public:
  RPG_Combat_Check_Type();

//   virtual void pre();
  virtual void type(const RPG_Combat_CheckTypeUnion&);
  virtual void attribute(const RPG_Common_Attribute&);
  virtual void difficultyClass(unsigned char);
  virtual RPG_Combat_Check post_RPG_Combat_Check_Type();

 private:
  RPG_Combat_Check myCurrentCheck;
};

class RPG_Combat_DamageReductionType_Type
 : public RPG_Combat_DamageReductionType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_DamageReductionType post_RPG_Combat_DamageReductionType_Type();
};

class RPG_Combat_DamageCounterMeasure_Type
 : public RPG_Combat_DamageCounterMeasure_Type_pskel
{
  public:
   RPG_Combat_DamageCounterMeasure_Type();

//    virtual void pre();
   virtual void type(const RPG_Combat_DamageCounterMeasureType&);
   virtual void check(const RPG_Combat_Check&);
   virtual void spell(const RPG_Magic_Spell&);
   virtual void reduction(const RPG_Combat_DamageReductionType&);
   virtual RPG_Combat_DamageCounterMeasure post_RPG_Combat_DamageCounterMeasure_Type();

 private:
  RPG_Combat_DamageCounterMeasure myCurrentCounterMeasure;
};

class RPG_Combat_DamageElement_Type
 : public RPG_Combat_DamageElement_Type_pskel
{
 public:
  RPG_Combat_DamageElement_Type();

//   virtual void pre();
  virtual void type(const RPG_Combat_DamageTypeUnion&);
  virtual void amount(const RPG_Dice_Roll&);
  virtual void secondary(const RPG_Dice_Roll&);
  virtual void duration(const RPG_Combat_DamageDuration&);
  virtual void other(const RPG_Combat_DamageBonus&);
  virtual void attribute(const RPG_Common_Attribute&);
  virtual void counterMeasure(const RPG_Combat_DamageCounterMeasure&);
  virtual void effect(const RPG_Common_EffectType&);
  virtual RPG_Combat_DamageElement post_RPG_Combat_DamageElement_Type();

 private:
  RPG_Combat_DamageElement myCurrentDamageElement;
};

class RPG_Combat_Damage_Type
 : public RPG_Combat_Damage_Type_pskel
{
 public:
  RPG_Combat_Damage_Type();

//   virtual void pre();
  virtual void element(const RPG_Combat_DamageElement&);
  virtual RPG_Combat_Damage post_RPG_Combat_Damage_Type();

 private:
  RPG_Combat_Damage myCurrentDamage;
};

class RPG_Combat_ActionType_Type
 : public RPG_Combat_ActionType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Combat_ActionType post_RPG_Combat_ActionType_Type();
};

#endif
