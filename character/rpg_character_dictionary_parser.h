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
#ifndef RPG_CHARACTER_DICTIONARY_PARSER_H
#define RPG_CHARACTER_DICTIONARY_PARSER_H

#include "rpg_character_dictionary_parser_base.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Dictionary_Parser
 : public RPG_Character_Dictionary_Type_pimpl
{
 public:
  RPG_Character_Dictionary_Parser(RPG_Character_MonsterDictionary_t*); // monster dictionary
  virtual ~RPG_Character_Dictionary_Parser();

//   virtual void pre();
//   virtual void RPG_Character_MonsterDictionary();
  virtual void post_RPG_Character_Dictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary_Parser());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary_Parser(const RPG_Character_Dictionary_Parser&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary_Parser& operator=(const RPG_Character_Dictionary_Parser&));

  RPG_Character_MonsterDictionary_t* myMonsterDictionary;
};

class RPG_Character_MonsterDictionary_Type
 : public RPG_Character_MonsterDictionary_Type_pimpl
{
 public:
  RPG_Character_MonsterDictionary_Type(RPG_Character_MonsterDictionary_t*); // monster dictionary

//   virtual void pre();
  virtual void monster(const RPG_Character_MonsterProperties_XML&);
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

class RPG_Chance_DiceType_Type
 : public RPG_Chance_DiceType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Chance_DiceType post_RPG_Chance_DiceType_Type();
};

class RPG_Chance_Roll_Type
 : public RPG_Chance_Roll_Type_pimpl
{
 public:
  RPG_Chance_Roll_Type();

//   virtual void pre();
  virtual void numDice(unsigned int);
  virtual void typeDice(const RPG_Chance_DiceType&);
  virtual void modifier(long long);
  virtual RPG_Chance_Roll post_RPG_Chance_Roll_Type();

 private:
  RPG_Chance_Roll myCurrentRoll;
};

class RPG_Character_MonsterArmorClass_Type
 : public RPG_Character_MonsterArmorClass_Type_pimpl
{
 public:
  RPG_Character_MonsterArmorClass_Type();

//   virtual void pre();
  virtual void normal(unsigned int);
  virtual void touch(unsigned int);
  virtual void flatFooted(unsigned int);
  virtual RPG_Character_MonsterArmorClass post_RPG_Character_MonsterArmorClass_Type();

 private:
  RPG_Character_MonsterArmorClass myCurrentArmorClass;
};

class RPG_Character_NaturalWeapon_Type
 : public RPG_Character_NaturalWeapon_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_NaturalWeapon post_RPG_Character_NaturalWeapon_Type();
};

class RPG_Character_MonsterAttackForm_Type
 : public RPG_Character_MonsterAttackForm_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_MonsterAttackForm post_RPG_Character_MonsterAttackForm_Type();
};

class RPG_Character_MonsterAttack_Type
 : public virtual RPG_Character_MonsterAttack_Type_pimpl
{
  public:
  RPG_Character_MonsterAttack_Type();

//   virtual void pre();
  virtual void baseAttackBonus(unsigned int);
  virtual void grappleBonus(unsigned int);
  virtual void naturalWeapon(const RPG_Character_NaturalWeapon&);
  virtual void attackBonus(unsigned int);
  virtual void attackForm(const RPG_Character_MonsterAttackForm&);
  virtual void damage(const RPG_Chance_Roll&);
  virtual void numAttacksPerRound(unsigned int);
  virtual RPG_Character_MonsterAttack post_RPG_Character_MonsterAttack_Type();

 private:
  RPG_Character_MonsterAttack myCurrentMonsterAttack;
};

class RPG_Character_SavingThrowModifiers_Type
 : public virtual RPG_Character_SavingThrowModifiers_Type_pimpl
{
  public:
  RPG_Character_SavingThrowModifiers_Type();

//   virtual void pre();
  virtual void fortitude(unsigned int);
  virtual void reflex(unsigned int);
  virtual void will(unsigned int);
  virtual RPG_Character_SavingThrowModifiers post_RPG_Character_SavingThrowModifiers_Type();

 private:
  RPG_Character_SavingThrowModifiers myCurrentSavingThrowModifiers;
};

class RPG_Character_Environment_Type
 : public RPG_Character_Environment_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Environment post_RPG_Character_Environment_Type();
};

class RPG_Character_Organization_Type
 : public RPG_Character_Organization_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Organization post_RPG_Character_Organization_Type();
};

class RPG_Character_MonsterAdvancementStep_Type
 : public RPG_Character_MonsterAdvancementStep_Type_pimpl
{
 public:
  RPG_Character_MonsterAdvancementStep_Type();

//   virtual void pre();
  virtual void size(const RPG_Character_Size&);
  virtual void range(const RPG_Chance_Roll&);
  virtual RPG_Character_MonsterAdvancementStep_t post_RPG_Character_MonsterAdvancementStep_Type();

 private:
  RPG_Character_MonsterAdvancementStep_t myCurrentAdvancementStep;
};

class RPG_Character_MonsterAdvancement_Type
 : public RPG_Character_MonsterAdvancement_Type_pimpl
{
 public:
  RPG_Character_MonsterAdvancement_Type();

//   virtual void pre();
  virtual void step(const RPG_Character_MonsterAdvancementStep_t&);
  virtual RPG_Character_MonsterAdvancement_t post_RPG_Character_MonsterAdvancement_Type();

 private:
  RPG_Character_MonsterAdvancement_t myCurrentMonsterAdvancement;
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

class RPG_Character_MonsterProperties_Type
 : public RPG_Character_MonsterProperties_Type_pimpl
{
 public:
  RPG_Character_MonsterProperties_Type();

//   virtual void pre();
  virtual void name(const ::std::string&);
  virtual void size(const RPG_Character_Size&);
  virtual void type(const RPG_Character_MonsterType&);
  virtual void hitDice(const RPG_Chance_Roll&);
  virtual void initiative(long long);
  virtual void speed(unsigned int);
  virtual void armorClass(const RPG_Character_MonsterArmorClass&);
  virtual void attack(const RPG_Character_MonsterAttack&);
  virtual void space(unsigned int);
  virtual void reach(unsigned int);
  virtual void saves(const RPG_Character_SavingThrowModifiers&);
  virtual void environment(const RPG_Character_Environment&);
  virtual void organization(const RPG_Character_Organization&);
  virtual void challengeRating(unsigned int);
  virtual void treasureModifier(unsigned int);
  virtual void alignment(const RPG_Character_Alignment&);
  virtual void advancement(const RPG_Character_MonsterAdvancement_t&);
  virtual void levelAdjustment(unsigned int);
  virtual RPG_Character_MonsterProperties_XML post_RPG_Character_MonsterProperties_Type();

 private:
  RPG_Character_MonsterProperties_XML myCurrentProperties;
};

#endif
