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
#ifndef RPG_MONSTER_XML_PARSER_H
#define RPG_MONSTER_XML_PARSER_H

#include "rpg_monster_XML_parser_base.h"

#include "rpg_monster_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Monster_Dictionary_Type
 : public RPG_Monster_Dictionary_Type_pimpl
{
 public:
  RPG_Monster_Dictionary_Type(RPG_Monster_Dictionary_t*); // monster dictionary
  virtual ~RPG_Monster_Dictionary_Type();

//   virtual void pre();
  virtual void monster(const RPG_Monster_PropertiesXML&);
  virtual void post_RPG_Monster_Dictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_Dictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_Dictionary_Type(const RPG_Monster_Dictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_Dictionary_Type& operator=(const RPG_Monster_Dictionary_Type&));

  RPG_Monster_Dictionary_t* myMonsterDictionary;
};

class RPG_Monster_Type_Type
 : public RPG_Monster_Type_Type_pimpl
{
 public:
  RPG_Monster_Type_Type();

//   virtual void pre();
  virtual void metaType(const RPG_Monster_MetaType&);
  virtual void subType(const RPG_Monster_SubType&);
  virtual RPG_Monster_Type post_RPG_Monster_Type_Type();

 private:
  RPG_Monster_Type myCurrentType;
};

class RPG_Monster_MetaType_Type
 : public RPG_Monster_MetaType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Monster_MetaType post_RPG_Monster_MetaType_Type();
};

class RPG_Monster_SubType_Type
 : public RPG_Monster_SubType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Monster_SubType post_RPG_Monster_SubType_Type();
};

class RPG_Monster_NaturalArmorClass_Type
 : public RPG_Monster_NaturalArmorClass_Type_pimpl
{
 public:
  RPG_Monster_NaturalArmorClass_Type();

//   virtual void pre();
  virtual void normal(signed char);
  virtual void touch(signed char);
  virtual void flatFooted(signed char);
  virtual RPG_Monster_NaturalArmorClass post_RPG_Monster_NaturalArmorClass_Type();

 private:
  RPG_Monster_NaturalArmorClass myCurrentNaturalArmorClass;
};

class RPG_Monster_NaturalWeapon_Type
 : public RPG_Monster_NaturalWeapon_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Monster_NaturalWeapon post_RPG_Monster_NaturalWeapon_Type();
};

class RPG_Monster_WeaponTypeUnion_Type
 : public RPG_Monster_WeaponTypeUnion_Type_pimpl
{
 public:
  RPG_Monster_WeaponTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Monster_WeaponTypeUnion post_RPG_Monster_WeaponTypeUnion_Type();

 private:
  RPG_Monster_WeaponTypeUnion myCurrentWeaponType;
};

class RPG_Monster_AttackAction_Type
 : public RPG_Monster_AttackAction_Type_pimpl
{
 public:
  RPG_Monster_AttackAction_Type();

//   virtual void pre();
  virtual void weapon(const RPG_Monster_WeaponTypeUnion&);
  virtual void attackBonus(signed char);
  virtual void attackForm(const RPG_Combat_AttackForm&);
  virtual void damage(const RPG_Combat_Damage&);
  virtual void numAttacksPerRound(unsigned char);
  virtual RPG_Monster_AttackAction post_RPG_Monster_AttackAction_Type();

 private:
  RPG_Monster_AttackAction myCurrentAttackAction;
};

class RPG_Monster_Attack_Type
 : public RPG_Monster_Attack_Type_pimpl
{
 public:
  RPG_Monster_Attack_Type();

//   virtual void pre();
  virtual void baseAttackBonus(signed char);
  virtual void grappleBonus(signed char);
  virtual void standardAttackAction(const RPG_Monster_AttackAction&);
  virtual void fullAttackAction(const RPG_Monster_AttackAction&);
  virtual RPG_Monster_Attack post_RPG_Monster_Attack_Type();

 private:
  RPG_Monster_Attack myCurrentAttack;
};

class RPG_Monster_SavingThrowModifiers_Type
 : public RPG_Monster_SavingThrowModifiers_Type_pimpl
{
 public:
  RPG_Monster_SavingThrowModifiers_Type();

//   virtual void pre();
  virtual void fortitude(signed char);
  virtual void reflex(signed char);
  virtual void will(signed char);
  virtual RPG_Monster_SavingThrowModifiers post_RPG_Monster_SavingThrowModifiers_Type();

 private:
  RPG_Monster_SavingThrowModifiers myCurrentSavingThrowModifiers;
};

class RPG_Monster_Organization_Type
 : public RPG_Monster_Organization_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Monster_Organization post_RPG_Monster_Organization_Type();
};

class RPG_Monster_OrganizationSlaverStep_Type
 : public RPG_Monster_OrganizationSlaverStep_Type_pimpl
{
 public:
  RPG_Monster_OrganizationSlaverStep_Type();

//   virtual void pre();
  virtual void name(const std::string&);
  virtual void range(const RPG_Dice_ValueRange&);
  virtual RPG_Monster_OrganizationSlaverStep post_RPG_Monster_OrganizationSlaverStep_Type();

 private:
  RPG_Monster_OrganizationSlaverStep myCurrentOrganizationSlaverStep;
};

class RPG_Monster_OrganizationStep_Type
 : public RPG_Monster_OrganizationStep_Type_pimpl
{
 public:
  RPG_Monster_OrganizationStep_Type();

//   virtual void pre();
  virtual void type(const RPG_Monster_Organization&);
  virtual void range(const RPG_Dice_ValueRange&);
  virtual void slaves(const RPG_Monster_OrganizationSlaverStep&);
  virtual RPG_Monster_OrganizationStep post_RPG_Monster_OrganizationStep_Type();

 private:
  RPG_Monster_OrganizationStep myCurrentOrganizationStep;
};

class RPG_Monster_Organizations_Type
 : public RPG_Monster_Organizations_Type_pimpl
{
 public:
  RPG_Monster_Organizations_Type();

//   virtual void pre();
  virtual void step(const RPG_Monster_OrganizationStep&);
  virtual RPG_Monster_Organizations post_RPG_Monster_Organizations_Type();

 private:
  RPG_Monster_Organizations myCurrentOrganizations;
};

class RPG_Monster_AdvancementStep_Type
 : public RPG_Monster_AdvancementStep_Type_pimpl
{
 public:
  RPG_Monster_AdvancementStep_Type();

//   virtual void pre();
  virtual void size(const RPG_Character_Size&);
  virtual void range(const RPG_Dice_ValueRange&);
  virtual RPG_Monster_AdvancementStep post_RPG_Monster_AdvancementStep_Type();

 private:
  RPG_Monster_AdvancementStep myCurrentAdvancementStep;
};

class RPG_Monster_Advancement_Type
 : public RPG_Monster_Advancement_Type_pimpl
{
 public:
  RPG_Monster_Advancement_Type();

//   virtual void pre();
  virtual void step(const RPG_Monster_AdvancementStep&);
  virtual RPG_Monster_Advancement post_RPG_Monster_Advancement_Type();

 private:
  RPG_Monster_Advancement myCurrentAdvancement;
};

class RPG_Monster_PropertiesXML_Type
 : public RPG_Monster_PropertiesXML_Type_pimpl
{
 public:
   RPG_Monster_PropertiesXML_Type();

//   virtual void pre();
  virtual void name(const std::string&);
  virtual void size(const RPG_Character_Size&);
  virtual void type(const RPG_Monster_Type&);
  virtual void hitDice(const RPG_Dice_Roll&);
  virtual void initiative(signed char);
  virtual void speed(unsigned char);
  virtual void armorClass(const RPG_Monster_NaturalArmorClass&);
  virtual void attack(const RPG_Monster_Attack&);
  virtual void space(unsigned char);
  virtual void reach(unsigned char);
  virtual void saves(const RPG_Monster_SavingThrowModifiers&);
  virtual void attributes(const RPG_Character_Attributes&);
  virtual void skills(const RPG_Character_Skills&);
  virtual void feats(const RPG_Character_Feats&);
  virtual void environment(const RPG_Character_Environment&);
  virtual void organizations(const RPG_Monster_Organizations&);
  virtual void challengeRating(unsigned char);
  virtual void treasureModifier(unsigned char);
  virtual void alignment(const RPG_Character_Alignment&);
  virtual void advancement(const RPG_Monster_Advancement&);
  virtual void levelAdjustment(unsigned char);
  virtual RPG_Monster_PropertiesXML post_RPG_Monster_PropertiesXML_Type();

 private:
  RPG_Monster_PropertiesXML myCurrentProperties;
};

#endif
