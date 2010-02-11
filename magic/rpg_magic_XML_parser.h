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
    virtual void school(const RPG_Magic_School&);
    virtual void subSchool(const RPG_Magic_SubSchool&);
    virtual void descriptor(const RPG_Magic_Descriptor&);
    virtual RPG_Magic_Spell_Type post_RPG_Magic_Spell_Type_Type();

  private:
    RPG_Magic_Spell_Type myCurrentType;
};

class RPG_Magic_Spell_Effect_Type
  : public RPG_Magic_Spell_Effect_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Magic_Spell_Effect post_RPG_Magic_Spell_Effect_Type();
};

class RPG_Magic_Spell_Range_Type
  : public RPG_Magic_Spell_Range_Type_pskel
{
  public:
    RPG_Magic_Spell_Range_Type();

//     virtual void pre();
    virtual void maxRange(unsigned char);
    virtual void increment(unsigned char);
    virtual void area(const RPG_Common_AreaOfEffect&);
    virtual void effect(const RPG_Magic_Spell_Effect&);
    virtual RPG_Magic_Spell_Range post_RPG_Magic_Spell_Range_Type();

  private:
    RPG_Magic_Spell_Range myCurrentRange;
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
    virtual void duration(unsigned int);
    virtual void period(const RPG_Dice_Roll&);
    virtual void dismissible(bool);
    virtual RPG_Magic_Spell_DurationProperties post_RPG_Magic_Spell_DurationProperties_Type();

  private:
    RPG_Magic_Spell_DurationProperties myCurrentProperties;
};

class RPG_Magic_Spell_Properties_Type
  : public RPG_Magic_Spell_Properties_Type_pskel
{
  public:
    RPG_Magic_Spell_Properties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_Spell_Type&);
    virtual void name(const RPG_Magic_SpellType&);
    virtual void level(unsigned char);
    virtual void casterClass(const RPG_Common_SubClass&);
    virtual void domain(const RPG_Magic_Domain&);
    virtual void domainLevel(unsigned char);
    virtual void cost(unsigned char);
    virtual void action(const RPG_Common_ActionType&);
    virtual void range(const RPG_Magic_Spell_Range&);
    virtual void duration(const RPG_Magic_Spell_DurationProperties&);
    virtual RPG_Magic_Spell_Properties post_RPG_Magic_Spell_Properties_Type();

  private:
    RPG_Magic_Spell_Properties myCurrentProperties;
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

#endif
