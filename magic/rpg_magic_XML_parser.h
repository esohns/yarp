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
#ifndef RPG_MAGIC_XML_PARSER_H
#define RPG_MAGIC_XML_PARSER_H

#include "rpg_magic_XML_types.h"
#include "rpg_magic_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Magic_School_Type
 : public RPG_Magic_School_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_School post_RPG_Magic_School_Type();
};

class RPG_Magic_SubSchool_Type
  : public RPG_Magic_SubSchool_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_SubSchool post_RPG_Magic_SubSchool_Type();
};

class RPG_Magic_Descriptor_Type
  : public RPG_Magic_Descriptor_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Descriptor post_RPG_Magic_Descriptor_Type();
};

class RPG_Magic_Domain_Type
 : public RPG_Magic_Domain_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_Domain post_RPG_Magic_Domain_Type();
};

class RPG_Magic_SpellType_Type
  : public RPG_Magic_SpellType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
   virtual RPG_Magic_SpellType post_RPG_Magic_SpellType_Type();
};

class RPG_Magic_AbilityClass_Type
 : public RPG_Magic_AbilityClass_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_AbilityClass post_RPG_Magic_AbilityClass_Type();
};

class RPG_Magic_AbilityType_Type
 : public RPG_Magic_AbilityType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_AbilityType post_RPG_Magic_AbilityType_Type();
};

class RPG_Magic_Spell_Type_Type
  : public RPG_Magic_Spell_Type_Type_pskel
{
  public:
    RPG_Magic_Spell_Type_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_SpellType&);
    virtual void school(const RPG_Magic_School&);
    virtual void subSchool(const RPG_Magic_SubSchool&);
    virtual void descriptor(const RPG_Magic_Descriptor&);
    virtual RPG_Magic_Spell_Type post_RPG_Magic_Spell_Type_Type();

  private:
    RPG_Magic_Spell_Type myCurrentType;
};

class RPG_Magic_CasterClassUnion_Type
  : public RPG_Magic_CasterClassUnion_Type_pskel
{
  public:
    RPG_Magic_CasterClassUnion_Type();

//   virtual void pre();
    virtual void _characters(const ::xml_schema::ro_string&);
    virtual RPG_Magic_CasterClassUnion post_RPG_Magic_CasterClassUnion_Type();

  private:
    RPG_Magic_CasterClassUnion myCurrentCasterClassUnion;
};

class RPG_Magic_Spell_Level_Type
  : public RPG_Magic_Spell_Level_Type_pskel
{
  public:
    RPG_Magic_Spell_Level_Type();

//     virtual void pre();
    virtual void casterClass(const RPG_Magic_CasterClassUnion&);
    virtual void level(unsigned char);
    virtual RPG_Magic_Spell_Level post_RPG_Magic_Spell_Level_Type();

  private:
    RPG_Magic_Spell_Level myCurrentSpellLevel;
};

class RPG_Magic_Spell_RangeEffect_Type
  : public RPG_Magic_Spell_RangeEffect_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Spell_RangeEffect post_RPG_Magic_Spell_RangeEffect_Type();
};

class RPG_Magic_Spell_RangeProperties_Type
  : public RPG_Magic_Spell_RangeProperties_Type_pskel
{
  public:
    RPG_Magic_Spell_RangeProperties_Type();

//     virtual void pre();
    virtual void max(unsigned int);
    virtual void increment(unsigned int);
    virtual void effect(const RPG_Magic_Spell_RangeEffect&);
    virtual RPG_Magic_Spell_RangeProperties post_RPG_Magic_Spell_RangeProperties_Type();

  private:
    RPG_Magic_Spell_RangeProperties myCurrentProperties;
};

class RPG_Magic_Spell_Target_Type
  : public RPG_Magic_Spell_Target_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Spell_Target post_RPG_Magic_Spell_Target_Type();
};

class RPG_Magic_Spell_TargetProperties_Type
  : public RPG_Magic_Spell_TargetProperties_Type_pskel
{
  public:
    RPG_Magic_Spell_TargetProperties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_Spell_Target&);
    virtual void value(unsigned int);
    virtual void range(const RPG_Dice_Roll&);
    virtual void levelIncrement(unsigned char);
    virtual void area(const RPG_Common_AreaOfEffect&);
    virtual void radius(unsigned char);
    virtual void height(unsigned char);
    virtual void rangeIsInHD(bool);
    virtual RPG_Magic_Spell_TargetProperties post_RPG_Magic_Spell_TargetProperties_Type();

  private:
    RPG_Magic_Spell_TargetProperties myCurrentProperties;
};

class RPG_Magic_Spell_Duration_Type
  : public RPG_Magic_Spell_Duration_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Spell_Duration post_RPG_Magic_Spell_Duration_Type();
};

class RPG_Magic_Spell_DurationProperties_Type
  : public RPG_Magic_Spell_DurationProperties_Type_pskel
{
  public:
    RPG_Magic_Spell_DurationProperties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_Spell_Duration&);
    virtual void base(unsigned int);
    virtual void duration(unsigned int);
    virtual void levelIncrement(unsigned char);
    virtual void levelIncrementMax(unsigned char);
    virtual void period(const RPG_Dice_Roll&);
    virtual void dismissible(bool);
    virtual RPG_Magic_Spell_DurationProperties post_RPG_Magic_Spell_DurationProperties_Type();

  private:
    RPG_Magic_Spell_DurationProperties myCurrentProperties;
};

class RPG_Magic_Spell_Precondition_Type
  : public RPG_Magic_Spell_Precondition_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Spell_Precondition post_RPG_Magic_Spell_Precondition_Type();
};

class RPG_Magic_Spell_PreconditionProperties_Type
  : public RPG_Magic_Spell_PreconditionProperties_Type_pskel
{
  public:
    RPG_Magic_Spell_PreconditionProperties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_Spell_Precondition&);
    virtual void value(long long);
    virtual void attribute(const RPG_Common_Attribute&);
    virtual void condition(const RPG_Character_Condition&);
    virtual void size(const RPG_Character_Size&);
    virtual RPG_Magic_Spell_PreconditionProperties post_RPG_Magic_Spell_PreconditionProperties_Type();

  private:
    RPG_Magic_Spell_PreconditionProperties myCurrentProperties;
};

class RPG_Magic_Spell_Effect_Type
  : public RPG_Magic_Spell_Effect_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Spell_Effect post_RPG_Magic_Spell_Effect_Type();
};

class RPG_Magic_Spell_EffectProperties_Type
  : public RPG_Magic_Spell_EffectProperties_Type_pskel
{
  public:
    RPG_Magic_Spell_EffectProperties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_Spell_Effect&);
    virtual void base(const RPG_Common_Amount&);
    virtual void levelIncrement(const RPG_Common_Amount&);
    virtual void levelIncrementMax(unsigned char);
    virtual void effect(const RPG_Common_EffectType&);
    virtual RPG_Magic_Spell_EffectProperties post_RPG_Magic_Spell_EffectProperties_Type();

  private:
    RPG_Magic_Spell_EffectProperties myCurrentProperties;
};

class RPG_Magic_Spell_PropertiesXML_Type
  : public RPG_Magic_Spell_PropertiesXML_Type_pskel
{
  public:
    RPG_Magic_Spell_PropertiesXML_Type();

//     virtual void pre();
    virtual void name(const ::std::string&);
    virtual void type(const RPG_Magic_Spell_Type&);
    virtual void level(const RPG_Magic_Spell_Level&);
    virtual void cost(unsigned int);
    virtual void action(const RPG_Common_ActionType&);
    virtual void range(const RPG_Magic_Spell_RangeProperties&);
    virtual void target(const RPG_Magic_Spell_TargetProperties&);
    virtual void duration(const RPG_Magic_Spell_DurationProperties&);
    virtual void precondition(const RPG_Magic_Spell_PreconditionProperties&);
    virtual void save(const RPG_Common_SavingThrowCheck&);
    virtual void effect(const RPG_Magic_Spell_EffectProperties&);
    virtual void saveable(const RPG_Common_SavingThrow&);
    virtual void resistible(bool);
    virtual RPG_Magic_Spell_PropertiesXML post_RPG_Magic_Spell_PropertiesXML_Type();

  private:
    RPG_Magic_Spell_PropertiesXML myCurrentProperties;
};

class RPG_Magic_SpellLikeProperties_Type
  : public RPG_Magic_SpellLikeProperties_Type_pskel
{
  public:
    RPG_Magic_SpellLikeProperties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_SpellType&);
    virtual void casterLevel(unsigned char);
    virtual void duration(const RPG_Common_Duration&);
    virtual void save(const RPG_Common_SavingThrowCheck&);
    virtual RPG_Magic_SpellLikeProperties post_RPG_Magic_SpellLikeProperties_Type();

  private:
    RPG_Magic_SpellLikeProperties myCurrentProperties;
};

class RPG_Magic_Dictionary_Type
  : public RPG_Magic_Dictionary_Type_pskel
{
  public:
    RPG_Magic_Dictionary_Type(RPG_Magic_Dictionary_t*); // spell dictionary
    virtual ~RPG_Magic_Dictionary_Type();

//   virtual void pre();
    virtual void spell(const RPG_Magic_Spell_PropertiesXML&);
    virtual void post_RPG_Magic_Dictionary_Type();

  private:
  // safety measures
    ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Dictionary_Type());
    ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Dictionary_Type(const RPG_Magic_Dictionary_Type&));
    ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Dictionary_Type& operator=(const RPG_Magic_Dictionary_Type&));

    RPG_Magic_Dictionary_t* myDictionary;
};

#endif
