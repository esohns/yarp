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
#ifndef RPG_CHARACTER_XML_PARSER_H
#define RPG_CHARACTER_XML_PARSER_H

#include "rpg_character_XML_parser_base.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Dictionary_Type
 : public RPG_Character_Dictionary_Type_pimpl
{
 public:
  RPG_Character_Dictionary_Type(RPG_Character_MonsterDictionary_t*); // monster dictionary
  virtual ~RPG_Character_Dictionary_Type();

//   virtual void pre();
//   virtual void RPG_Character_MonsterDictionary();
  virtual void post_RPG_Character_Dictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary_Type(const RPG_Character_Dictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary_Type& operator=(const RPG_Character_Dictionary_Type&));

  RPG_Character_MonsterDictionary_t* myMonsterDictionary;
};

class RPG_Character_MonsterDictionary_Type
 : public RPG_Character_MonsterDictionary_Type_pimpl
{
 public:
  RPG_Character_MonsterDictionary_Type(RPG_Character_MonsterDictionary_t*); // monster dictionary

//   virtual void pre();
  virtual void monster(const RPG_Character_MonsterPropertiesXML&);
  virtual void post_RPG_Character_MonsterDictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterDictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterDictionary_Type(const RPG_Character_MonsterDictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_MonsterDictionary_Type& operator=(const RPG_Character_MonsterDictionary_Type&));

  RPG_Character_MonsterDictionary_t* myMonsterDictionary;
};

class RPG_Character_Size_Type
 : public RPG_Character_Size_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Size post_RPG_Character_Size_Type();
};

class RPG_Character_MonsterType_Type
 : public RPG_Character_MonsterType_Type_pimpl
{
 public:
  RPG_Character_MonsterType_Type();

//   virtual void pre();
  virtual void metaType(const RPG_Character_MonsterMetaType&);
  virtual void subType(const RPG_Character_MonsterSubType&);
  virtual RPG_Character_MonsterType post_RPG_Character_MonsterType_Type();

 private:
  RPG_Character_MonsterType myCurrentMonsterType;
};

class RPG_Character_MonsterMetaType_Type
 : public RPG_Character_MonsterMetaType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_MonsterMetaType post_RPG_Character_MonsterMetaType_Type();
};

class RPG_Character_MonsterSubType_Type
 : public RPG_Character_MonsterSubType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_MonsterSubType post_RPG_Character_MonsterSubType_Type();
};

class RPG_Character_MonsterArmorClass_Type
 : public RPG_Character_MonsterArmorClass_Type_pimpl
{
 public:
  RPG_Character_MonsterArmorClass_Type();

//   virtual void pre();
  virtual void normal(signed char);
  virtual void touch(signed char);
  virtual void flatFooted(signed char);
  virtual RPG_Character_MonsterArmorClass post_RPG_Character_MonsterArmorClass_Type();

 private:
  RPG_Character_MonsterArmorClass myCurrentArmorClass;
};

class RPG_Character_MonsterWeapon_Type
 : public RPG_Character_MonsterWeapon_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_MonsterWeapon post_RPG_Character_MonsterWeapon_Type();
};

class RPG_Character_AttackForm_Type
 : public RPG_Character_AttackForm_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AttackForm post_RPG_Character_AttackForm_Type();
};

class RPG_Character_MonsterAttackAction_Type
 : public RPG_Character_MonsterAttackAction_Type_pimpl
{
 public:
  RPG_Character_MonsterAttackAction_Type();

//   virtual void pre();
  virtual void monsterWeapon(const RPG_Character_MonsterWeapon&);
  virtual void attackBonus(signed char);
  virtual void attackForm(const RPG_Character_AttackForm&);
  virtual void damage(const RPG_Chance_DiceRoll&);
  virtual void numAttacksPerRound(unsigned char);
  virtual RPG_Character_MonsterAttackAction post_RPG_Character_MonsterAttackAction_Type();

 private:
  RPG_Character_MonsterAttackAction myCurrentMonsterAttackAction;
};

class RPG_Character_MonsterAttack_Type
 : public RPG_Character_MonsterAttack_Type_pimpl
{
 public:
  RPG_Character_MonsterAttack_Type();

//   virtual void pre();
  virtual void baseAttackBonus(signed char);
  virtual void grappleBonus(signed char);
  virtual void attackAction(const RPG_Character_MonsterAttackAction&);
  virtual RPG_Character_MonsterAttack post_RPG_Character_MonsterAttack_Type();

 private:
  RPG_Character_MonsterAttack myCurrentMonsterAttack;
};

class RPG_Character_SavingThrowModifiers_Type
 : public RPG_Character_SavingThrowModifiers_Type_pimpl
{
 public:
  RPG_Character_SavingThrowModifiers_Type();

//   virtual void pre();
  virtual void fortitude(signed char);
  virtual void reflex(signed char);
  virtual void will(signed char);
  virtual RPG_Character_SavingThrowModifiers post_RPG_Character_SavingThrowModifiers_Type();

 private:
  RPG_Character_SavingThrowModifiers myCurrentSavingThrowModifiers;
};

class RPG_Character_Attributes_Type
 : public RPG_Character_Attributes_Type_pimpl
{
 public:
  RPG_Character_Attributes_Type();

//   virtual void pre();
  virtual void strength(unsigned char);
  virtual void dexterity(unsigned char);
  virtual void constitution(unsigned char);
  virtual void intelligence(unsigned char);
  virtual void wisdom(unsigned char);
  virtual void charisma(unsigned char);
  virtual RPG_Character_Attributes post_RPG_Character_Attributes_Type();

 private:
  RPG_Character_Attributes myCurrentAttributes;
};

class RPG_Character_Skill_Type
 : public RPG_Character_Skill_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Skill post_RPG_Character_Skill_Type();
};

class RPG_Character_SkillValue_Type
 : public RPG_Character_SkillValue_Type_pimpl
{
 public:
  RPG_Character_SkillValue_Type();

//   virtual void pre();
  virtual void skill(const RPG_Character_Skill&);
  virtual void rank(signed char);
  virtual RPG_Character_SkillValue post_RPG_Character_SkillValue_Type();

 private:
  RPG_Character_SkillValue myCurrentSkill;
};

class RPG_Character_Skills_Type
 : public RPG_Character_Skills_Type_pimpl
{
 public:
  RPG_Character_Skills_Type();

//   virtual void pre();
  virtual void skill(const RPG_Character_SkillValue&);
  virtual RPG_Character_Skills post_RPG_Character_Skills_Type();

 private:
  RPG_Character_Skills myCurrentSkills;
};

class RPG_Character_Feat_Type
 : public RPG_Character_Feat_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Feat post_RPG_Character_Feat_Type();
};

class RPG_Character_Feats_Type
 : public RPG_Character_Feats_Type_pimpl
{
 public:
  RPG_Character_Feats_Type();

//   virtual void pre();
  virtual void feat(const RPG_Character_Feat&);
  virtual RPG_Character_Feats post_RPG_Character_Feats_Type();

 private:
  RPG_Character_Feats myCurrentFeats;
};

class RPG_Character_Terrain_Type
  : public RPG_Character_Terrain_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Terrain post_RPG_Character_Terrain_Type();
};

class RPG_Character_Climate_Type
  : public RPG_Character_Climate_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Climate post_RPG_Character_Climate_Type();
};

class RPG_Character_Environment_Type
 : public RPG_Character_Environment_Type_pimpl
{
 public:
  RPG_Character_Environment_Type();

//   virtual void pre();
  virtual void terrain(const RPG_Character_Terrain&);
  virtual void climate(const RPG_Character_Climate&);
  virtual RPG_Character_Environment post_RPG_Character_Environment_Type();

 private:
  RPG_Character_Environment myCurrentEnvironment;
};

class RPG_Character_Organization_Type
 : public RPG_Character_Organization_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Organization post_RPG_Character_Organization_Type();
};

class RPG_Character_OrganizationSlaverStep_Type
 : public RPG_Character_OrganizationSlaverStep_Type_pimpl
{
  public:
   RPG_Character_OrganizationSlaverStep_Type();

//   virtual void pre();
   virtual void name(const std::string&);
   virtual void range(const RPG_Chance_ValueRange&);
   virtual RPG_Character_OrganizationSlaverStep post_RPG_Character_OrganizationSlaverStep_Type();

  private:
   RPG_Character_OrganizationSlaverStep myCurrentOrganizationSlaverStep;
};

class RPG_Character_OrganizationStep_Type
 : public RPG_Character_OrganizationStep_Type_pimpl
{
 public:
  RPG_Character_OrganizationStep_Type();

//   virtual void pre();
  virtual void type(const RPG_Character_Organization&);
  virtual void range(const RPG_Chance_ValueRange&);
  virtual void slaves(const RPG_Character_OrganizationSlaverStep&);
  virtual RPG_Character_OrganizationStep post_RPG_Character_OrganizationStep_Type();

 private:
  RPG_Character_OrganizationStep myCurrentOrganizationStep;
};

class RPG_Character_Organizations_Type
 : public RPG_Character_Organizations_Type_pimpl
{
 public:
  RPG_Character_Organizations_Type();

//   virtual void pre();
  virtual void step(const RPG_Character_OrganizationStep&);
  virtual RPG_Character_Organizations post_RPG_Character_Organizations_Type();

 private:
  RPG_Character_Organizations myCurrentOrganizations;
};

class RPG_Character_MonsterAdvancementStep_Type
 : public RPG_Character_MonsterAdvancementStep_Type_pimpl
{
 public:
  RPG_Character_MonsterAdvancementStep_Type();

//   virtual void pre();
  virtual void size(const RPG_Character_Size&);
  virtual void range(const RPG_Chance_ValueRange&);
  virtual RPG_Character_MonsterAdvancementStep post_RPG_Character_MonsterAdvancementStep_Type();

 private:
  RPG_Character_MonsterAdvancementStep myCurrentAdvancementStep;
};

class RPG_Character_MonsterAdvancement_Type
 : public RPG_Character_MonsterAdvancement_Type_pimpl
{
 public:
  RPG_Character_MonsterAdvancement_Type();

//   virtual void pre();
  virtual void step(const RPG_Character_MonsterAdvancementStep&);
  virtual RPG_Character_MonsterAdvancement post_RPG_Character_MonsterAdvancement_Type();

 private:
  RPG_Character_MonsterAdvancement myCurrentMonsterAdvancement;
};

class RPG_Character_AlignmentCivic_Type
 : public RPG_Character_AlignmentCivic_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AlignmentCivic post_RPG_Character_AlignmentCivic_Type();
};

class RPG_Character_AlignmentEthic_Type
 : public RPG_Character_AlignmentEthic_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AlignmentEthic post_RPG_Character_AlignmentEthic_Type();
};

class RPG_Character_Alignment_Type
 : public RPG_Character_Alignment_Type_pimpl
{
 public:
  RPG_Character_Alignment_Type();

//   virtual void pre();
  virtual void civic(const RPG_Character_AlignmentCivic&);
  virtual void ethic(const RPG_Character_AlignmentEthic&);
  virtual RPG_Character_Alignment post_RPG_Character_Alignment_Type();

 private:
  RPG_Character_Alignment myCurrentAlignment;
};

class RPG_Character_MonsterPropertiesXML_Type
 : public RPG_Character_MonsterPropertiesXML_Type_pimpl
{
 public:
  RPG_Character_MonsterPropertiesXML_Type();

//   virtual void pre();
  virtual void name(const ::std::string&);
  virtual void size(const RPG_Character_Size&);
  virtual void type(const RPG_Character_MonsterType&);
  virtual void hitDice(const RPG_Chance_DiceRoll&);
  virtual void initiative(signed char);
  virtual void speed(unsigned char);
  virtual void armorClass(const RPG_Character_MonsterArmorClass&);
  virtual void attack(const RPG_Character_MonsterAttack&);
  virtual void space(unsigned char);
  virtual void reach(unsigned char);
  virtual void saves(const RPG_Character_SavingThrowModifiers&);
  virtual void attributes(const RPG_Character_Attributes&);
  virtual void skills(const RPG_Character_Skills&);
  virtual void feats(const RPG_Character_Feats&);
  virtual void environment(const RPG_Character_Environment&);
  virtual void organizations(const RPG_Character_Organizations&);
  virtual void challengeRating(unsigned char);
  virtual void treasureModifier(unsigned char);
  virtual void alignment(const RPG_Character_Alignment&);
  virtual void advancement(const RPG_Character_MonsterAdvancement&);
  virtual void levelAdjustment(unsigned char);
  virtual RPG_Character_MonsterPropertiesXML post_RPG_Character_MonsterPropertiesXML_Type();

 private:
  RPG_Character_MonsterPropertiesXML myCurrentProperties;
};

#endif
