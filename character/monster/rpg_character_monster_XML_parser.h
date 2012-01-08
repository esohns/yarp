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

#ifndef RPG_CHARACTER_MONSTER_XML_PARSER_H
#define RPG_CHARACTER_MONSTER_XML_PARSER_H

#include "rpg_character_monster_XML_types.h"

#include "rpg_character_monster_exports.h"
#include "rpg_character_monster_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Monster_Export RPG_Character_Monster_NaturalArmorClass_Type
 : public RPG_Character_Monster_NaturalArmorClass_Type_pskel
{
 public:
  RPG_Character_Monster_NaturalArmorClass_Type();

//   virtual void pre();
  virtual void normal(signed char);
  virtual void touch(signed char);
  virtual void flatFooted(signed char);
  virtual RPG_Character_Monster_NaturalArmorClass post_RPG_Character_Monster_NaturalArmorClass_Type();

 private:
  RPG_Character_Monster_NaturalArmorClass myCurrentNaturalArmorClass;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialPropertyTypeUnion_Type
  : public RPG_Character_Monster_SpecialPropertyTypeUnion_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialPropertyTypeUnion_Type();

//   virtual void pre();
    virtual void _characters(const ::xml_schema::ro_string&);
    virtual RPG_Character_Monster_SpecialPropertyTypeUnion post_RPG_Character_Monster_SpecialPropertyTypeUnion_Type();

  private:
    RPG_Character_Monster_SpecialPropertyTypeUnion myCurrentUnionType;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialBaseProperties_Type
  : public RPG_Character_Monster_SpecialBaseProperties_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialBaseProperties_Type();

//     virtual void pre();
    virtual void abilityClass(const RPG_Magic_AbilityClass&);
    virtual void type(const RPG_Character_Monster_SpecialPropertyTypeUnion&);
    virtual void actionType(const RPG_Common_ActionType&);
    virtual void usage(const RPG_Common_Usage&);
    virtual RPG_Character_Monster_SpecialBaseProperties post_RPG_Character_Monster_SpecialBaseProperties_Type();

  private:
    RPG_Character_Monster_SpecialBaseProperties myCurrentProperties;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialDefensePreCondition_Type
  : public RPG_Character_Monster_SpecialDefensePreCondition_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialDefensePreCondition_Type();

//   virtual void pre();
    virtual void weaponType(const RPG_Character_Monster_WeaponTypeUnion&);
    virtual void ownCondition(const RPG_Common_Condition&);
    virtual void targetCondition(const RPG_Common_Condition&);
    virtual void isMagicalWeapon(bool);
    virtual RPG_Character_Monster_SpecialDefensePreCondition post_RPG_Character_Monster_SpecialDefensePreCondition_Type();

  private:
    RPG_Character_Monster_SpecialDefensePreCondition myCurrentPreCondition;
};

class RPG_Monster_Export RPG_Character_Monster_DefenseAction_Type
  : public RPG_Character_Monster_DefenseAction_Type_pskel
{
  public:
    RPG_Character_Monster_DefenseAction_Type();

//   virtual void pre();
    virtual void preCondition(const RPG_Character_Monster_SpecialDefensePreCondition&);
    virtual void damage(const RPG_Combat_Damage&);
    virtual void ranged(const RPG_Combat_RangedAttackProperties&);
    virtual RPG_Character_Monster_DefenseAction post_RPG_Character_Monster_DefenseAction_Type();

  private:
    RPG_Character_Monster_DefenseAction myCurrentAction;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialDefenseProperties_Type
  : public RPG_Character_Monster_SpecialDefenseProperties_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialDefenseProperties_Type();

//     virtual void pre();
    virtual void abilityClass(const RPG_Magic_AbilityClass&);
    virtual void type(const RPG_Character_Monster_SpecialPropertyTypeUnion&);
    virtual void actionType(const RPG_Common_ActionType&);
    virtual void usage(const RPG_Common_Usage&);
    virtual RPG_Character_Monster_SpecialBaseProperties post_RPG_Character_Monster_SpecialBaseProperties_Type();
    // ------------------------------------------------------------
    virtual void action(const RPG_Character_Monster_DefenseAction&);
    virtual RPG_Character_Monster_SpecialDefenseProperties post_RPG_Character_Monster_SpecialDefenseProperties_Type();

  private:
    RPG_Character_Monster_SpecialDefenseProperties myCurrentProperties;
};

class RPG_Monster_Export RPG_Character_Monster_NaturalWeapon_Type
 : public RPG_Character_Monster_NaturalWeapon_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Monster_NaturalWeapon post_RPG_Character_Monster_NaturalWeapon_Type();
};

class RPG_Monster_Export RPG_Character_Monster_WeaponTypeUnion_Type
 : public RPG_Character_Monster_WeaponTypeUnion_Type_pskel
{
 public:
  RPG_Character_Monster_WeaponTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Character_Monster_WeaponTypeUnion post_RPG_Character_Monster_WeaponTypeUnion_Type();

 private:
  RPG_Character_Monster_WeaponTypeUnion myCurrentWeaponType;
};

class RPG_Monster_Export RPG_Character_Monster_AttackAction_Type
 : public RPG_Character_Monster_AttackAction_Type_pskel
{
 public:
  RPG_Character_Monster_AttackAction_Type();

//   virtual void pre();
  virtual void weapon(const RPG_Character_Monster_WeaponTypeUnion&);
  virtual void attackBonus(signed char);
  virtual void attackForm(const RPG_Combat_AttackForm&);
  virtual void damage(const RPG_Combat_Damage&);
  virtual void effect(const RPG_Magic_SpellLikeProperties&);
  virtual void numAttacksPerRound(unsigned char);
  virtual void ranged(const RPG_Combat_RangedAttackProperties&);
  virtual void trigger(const RPG_Character_Ability&);
  virtual void allAdjacent(bool);
  virtual void fullAttackIncludesNextAction(bool);
  virtual RPG_Character_Monster_AttackAction post_RPG_Character_Monster_AttackAction_Type();

 private:
  RPG_Character_Monster_AttackAction myCurrentAttackAction;
};

class RPG_Monster_Export RPG_Character_Monster_Attack_Type
 : public RPG_Character_Monster_Attack_Type_pskel
{
 public:
  RPG_Character_Monster_Attack_Type();

//   virtual void pre();
  virtual void baseAttackBonus(signed char);
  virtual void grappleBonus(signed char);
  virtual void standardAttackAction(const RPG_Character_Monster_AttackAction&);
  virtual void fullAttackAction(const RPG_Character_Monster_AttackAction&);
  virtual void actionsAreInclusive(bool);
  virtual RPG_Character_Monster_Attack post_RPG_Character_Monster_Attack_Type();

 private:
  RPG_Character_Monster_Attack myCurrentAttack;
};

class RPG_Monster_Export RPG_Character_Monster_ActionTrigger_Type
  : public RPG_Character_Monster_ActionTrigger_Type_pskel
{
  public:
    RPG_Character_Monster_ActionTrigger_Type();

//   virtual void pre();
    virtual void weapon(const RPG_Character_Monster_WeaponTypeUnion&);
    virtual void numHits(unsigned char);
    virtual RPG_Character_Monster_ActionTrigger post_RPG_Character_Monster_ActionTrigger_Type();

  private:
    RPG_Character_Monster_ActionTrigger myCurrentTrigger;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialAttackPreCondition_Type
  : public RPG_Character_Monster_SpecialAttackPreCondition_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialAttackPreCondition_Type();

//   virtual void pre();
    virtual void targetAlignment(const RPG_Character_Alignment&);
    virtual void ownCondition(const RPG_Common_Condition&);
    virtual void targetCondition(const RPG_Common_Condition&);
    virtual void minTargetSize(const RPG_Common_Size&);
    virtual void maxTargetSize(const RPG_Common_Size&);
    virtual void check(const RPG_Combat_Check&);
    virtual RPG_Character_Monster_SpecialAttackPreCondition post_RPG_Character_Monster_SpecialAttackPreCondition_Type();

  private:
    RPG_Character_Monster_SpecialAttackPreCondition myCurrentPreCondition;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialAttackProperties_Type
  : public RPG_Character_Monster_SpecialAttackProperties_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialAttackProperties_Type();

//     virtual void pre();
    virtual void abilityClass(const RPG_Magic_AbilityClass&);
    virtual void type(const RPG_Character_Monster_SpecialPropertyTypeUnion&);
    virtual void actionType(const RPG_Common_ActionType&);
    virtual void usage(const RPG_Common_Usage&);
    virtual RPG_Character_Monster_SpecialBaseProperties post_RPG_Character_Monster_SpecialBaseProperties_Type();
    // ------------------------------------------------------------
    virtual void preCondition(const RPG_Character_Monster_SpecialAttackPreCondition&);
    virtual void action(const RPG_Character_Monster_AttackAction&);
//     virtual void amount(const RPG_Dice_Roll&);
    virtual RPG_Character_Monster_SpecialAttackProperties post_RPG_Character_Monster_SpecialAttackProperties_Type();

  private:
    RPG_Character_Monster_SpecialAttackProperties myCurrentProperties;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialAbilityPreCondition_Type
  : public RPG_Character_Monster_SpecialAbilityPreCondition_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialAbilityPreCondition_Type();

//   virtual void pre();
    virtual void targetCondition(const RPG_Common_Condition&);
    virtual void maxTargetSize(const RPG_Common_Size&);
    virtual void check(const RPG_Combat_Check&);
    virtual RPG_Character_Monster_SpecialAbilityPreCondition post_RPG_Character_Monster_SpecialAbilityPreCondition_Type();

  private:
    RPG_Character_Monster_SpecialAbilityPreCondition myCurrentPreCondition;
};

class RPG_Monster_Export RPG_Character_Monster_SummonMonster_Type
  : public RPG_Character_Monster_SummonMonster_Type_pskel
{
  public:
    RPG_Character_Monster_SummonMonster_Type();

//   virtual void pre();
    virtual void name(const ::std::string&);
    virtual void amount(const RPG_Dice_Roll&);
    virtual void successRate(float);
    virtual void actionsAreInclusive(bool);
    virtual RPG_Character_Monster_SummonMonster post_RPG_Character_Monster_SummonMonster_Type();

  private:
    RPG_Character_Monster_SummonMonster myCurrentSummonStep;
};

class RPG_Monster_Export RPG_Character_Monster_SpecialAbilityProperties_Type
  : public RPG_Character_Monster_SpecialAbilityProperties_Type_pskel
{
  public:
    RPG_Character_Monster_SpecialAbilityProperties_Type();

//     virtual void pre();
    virtual void abilityClass(const RPG_Magic_AbilityClass&);
    virtual void type(const RPG_Character_Monster_SpecialPropertyTypeUnion&);
    virtual void actionType(const RPG_Common_ActionType&);
    virtual void usage(const RPG_Common_Usage&);
    virtual RPG_Character_Monster_SpecialBaseProperties post_RPG_Character_Monster_SpecialBaseProperties_Type();
    // ------------------------------------------------------------
    virtual void preCondition(const RPG_Character_Monster_SpecialAbilityPreCondition&);
    virtual void summon(const RPG_Character_Monster_SummonMonster&);
    virtual void spell(const RPG_Magic_SpellLikeProperties&);
    virtual void ranged(const RPG_Combat_RangedAttackProperties&);
    virtual RPG_Character_Monster_SpecialAbilityProperties post_RPG_Character_Monster_SpecialAbilityProperties_Type();

  private:
    RPG_Character_Monster_SpecialAbilityProperties myCurrentProperties;
};

class RPG_Monster_Export RPG_Character_Monster_SavingThrowModifiers_Type
 : public RPG_Character_Monster_SavingThrowModifiers_Type_pskel
{
 public:
  RPG_Character_Monster_SavingThrowModifiers_Type();

//   virtual void pre();
  virtual void fortitude(signed char);
  virtual void reflex(signed char);
  virtual void will(signed char);
  virtual RPG_Character_Monster_SavingThrowModifiers post_RPG_Character_Monster_SavingThrowModifiers_Type();

 private:
  RPG_Character_Monster_SavingThrowModifiers myCurrentSavingThrowModifiers;
};

class RPG_Monster_Export RPG_Character_Monster_Organization_Type
 : public RPG_Character_Monster_Organization_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Monster_Organization post_RPG_Character_Monster_Organization_Type();
};

class RPG_Monster_Export RPG_Character_Monster_OrganizationSlaverStep_Type
 : public RPG_Character_Monster_OrganizationSlaverStep_Type_pskel
{
 public:
  RPG_Character_Monster_OrganizationSlaverStep_Type();

//   virtual void pre();
  virtual void name(const std::string&);
  virtual void range(const RPG_Dice_ValueRange&);
  virtual RPG_Character_Monster_OrganizationSlaverStep post_RPG_Character_Monster_OrganizationSlaverStep_Type();

 private:
  RPG_Character_Monster_OrganizationSlaverStep myCurrentOrganizationSlaverStep;
};

class RPG_Monster_Export RPG_Character_Monster_OrganizationStep_Type
 : public RPG_Character_Monster_OrganizationStep_Type_pskel
{
 public:
  RPG_Character_Monster_OrganizationStep_Type();

//   virtual void pre();
  virtual void type(const RPG_Character_Monster_Organization&);
  virtual void range(const RPG_Dice_ValueRange&);
  virtual void slaves(const RPG_Character_Monster_OrganizationSlaverStep&);
  virtual RPG_Character_Monster_OrganizationStep post_RPG_Character_Monster_OrganizationStep_Type();

 private:
  RPG_Character_Monster_OrganizationStep myCurrentOrganizationStep;
};

class RPG_Monster_Export RPG_Character_Monster_Organizations_Type
 : public RPG_Character_Monster_Organizations_Type_pskel
{
 public:
  RPG_Character_Monster_Organizations_Type();

//   virtual void pre();
  virtual void step(const RPG_Character_Monster_OrganizationStep&);
  virtual RPG_Character_Monster_Organizations post_RPG_Character_Monster_Organizations_Type();

 private:
  RPG_Character_Monster_Organizations myCurrentOrganizations;
};

class RPG_Monster_Export RPG_Character_Monster_AdvancementStep_Type
 : public RPG_Character_Monster_AdvancementStep_Type_pskel
{
 public:
  RPG_Character_Monster_AdvancementStep_Type();

//   virtual void pre();
  virtual void size(const RPG_Common_Size&);
  virtual void range(const RPG_Dice_ValueRange&);
  virtual RPG_Character_Monster_AdvancementStep post_RPG_Character_Monster_AdvancementStep_Type();

 private:
  RPG_Character_Monster_AdvancementStep myCurrentAdvancementStep;
};

class RPG_Monster_Export RPG_Character_Monster_Advancement_Type
 : public RPG_Character_Monster_Advancement_Type_pskel
{
 public:
  RPG_Character_Monster_Advancement_Type();

//   virtual void pre();
  virtual void step(const RPG_Character_Monster_AdvancementStep&);
  virtual RPG_Character_Monster_Advancement post_RPG_Character_Monster_Advancement_Type();

 private:
  RPG_Character_Monster_Advancement myCurrentAdvancement;
};

class RPG_Monster_Export RPG_Character_Monster_PropertiesXML_Type
 : public RPG_Character_Monster_PropertiesXML_Type_pskel
{
 public:
   RPG_Character_Monster_PropertiesXML_Type();

//   virtual void pre();
  virtual void name(const std::string&);
  virtual void size(const RPG_Common_Size&);
  virtual void type(const RPG_Common_CreatureType&);
  virtual void hitDice(const RPG_Dice_Roll&);
  virtual void initiative(signed char);
  virtual void speed(unsigned char);
  virtual void armorClass(const RPG_Character_Monster_NaturalArmorClass&);
  virtual void specialDefense(const RPG_Character_Monster_SpecialDefenseProperties&);
  virtual void attack(const RPG_Character_Monster_Attack&);
  virtual void specialAttack(const RPG_Character_Monster_SpecialAttackProperties&);
  virtual void specialAbility(const RPG_Character_Monster_SpecialAbilityProperties&);
  virtual void space(unsigned char);
  virtual void reach(unsigned char);
  virtual void saves(const RPG_Character_Monster_SavingThrowModifiers&);
  virtual void attributes(const RPG_Character_Attributes&);
  virtual void skills(const RPG_Character_Skills&);
  virtual void feats(const RPG_Character_Feats&);
  virtual void environment(const RPG_Common_Environment&);
  virtual void organizations(const RPG_Character_Monster_Organizations&);
  virtual void challengeRating(unsigned char);
  virtual void treasureModifier(unsigned char);
  virtual void alignment(const RPG_Character_Alignment&);
  virtual void advancement(const RPG_Character_Monster_Advancement&);
  virtual void levelAdjustment(unsigned char);
  virtual RPG_Character_Monster_PropertiesXML post_RPG_Character_Monster_PropertiesXML_Type();

 private:
  RPG_Character_Monster_PropertiesXML myCurrentProperties;
};

class RPG_Monster_Export RPG_Character_Monster_Dictionary_Type
  : public RPG_Character_Monster_Dictionary_Type_pskel
{
  public:
    RPG_Character_Monster_Dictionary_Type(RPG_Character_Monster_Dictionary_t*); // monster dictionary
    virtual ~RPG_Character_Monster_Dictionary_Type();

//   virtual void pre();
    virtual void monster(const RPG_Character_Monster_PropertiesXML&);
    virtual void post_RPG_Character_Monster_Dictionary_Type();

  private:
  // safety measures
    ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Dictionary_Type());
    ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Dictionary_Type(const RPG_Character_Monster_Dictionary_Type&));
    ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_Dictionary_Type& operator=(const RPG_Character_Monster_Dictionary_Type&));

    RPG_Character_Monster_Dictionary_t* myMonsterDictionary;
};

#endif
