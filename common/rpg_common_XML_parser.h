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
#ifndef RPG_COMMON_XML_PARSER_H
#define RPG_COMMON_XML_PARSER_H

#include "rpg_common_XML_types.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Common_SubClass_Type
  : public RPG_Common_SubClass_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Common_SubClass post_RPG_Common_SubClass_Type();
};

class RPG_Common_Attribute_Type
 : public RPG_Common_Attribute_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Common_Attribute post_RPG_Common_Attribute_Type();
};

class RPG_Common_PhysicalDamageType_Type
 : public RPG_Common_PhysicalDamageType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Common_PhysicalDamageType post_RPG_Common_PhysicalDamageType_Type();
};

class RPG_Common_ActionType_Type
  : public RPG_Common_ActionType_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Common_ActionType post_RPG_Common_ActionType_Type();
};

class RPG_Common_AreaOfEffect_Type
  : public RPG_Common_AreaOfEffect_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Common_AreaOfEffect post_RPG_Common_AreaOfEffect_Type();
};

class RPG_Common_EffectType_Type
  : public RPG_Common_EffectType_Type_pskel,
    public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Common_EffectType post_RPG_Common_EffectType_Type();
};

class RPG_Common_CheckType_Type
 : public RPG_Common_CheckType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Common_CheckType post_RPG_Common_CheckType_Type();
};

class RPG_Common_SavingThrow_Type
 : public RPG_Common_SavingThrow_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Common_SavingThrow post_RPG_Common_SavingThrow_Type();
};

class RPG_Common_BaseCheckTypeUnion_Type
 : public RPG_Common_BaseCheckTypeUnion_Type_pskel
{
 public:
  RPG_Common_BaseCheckTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Common_BaseCheckTypeUnion post_RPG_Common_BaseCheckTypeUnion_Type();

 private:
  RPG_Common_BaseCheckTypeUnion myBaseCheckTypeUnion;
};

class RPG_Common_SaveReductionType_Type
  : public RPG_Common_SaveReductionType_Type_pskel,
    public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Common_SaveReductionType post_RPG_Common_SaveReductionType_Type();
};

class RPG_Common_SavingThrowCheck_Type
  : public RPG_Common_SavingThrowCheck_Type_pskel
{
  public:
    RPG_Common_SavingThrowCheck_Type();

//   virtual void pre();
    virtual void type(const RPG_Common_SavingThrow&);
    virtual void attribute(const RPG_Common_Attribute&);
    virtual void difficultyClass(unsigned char);
    virtual void reduction(const RPG_Common_SaveReductionType&);
    virtual RPG_Common_SavingThrowCheck post_RPG_Common_SavingThrowCheck_Type();

  private:
    RPG_Common_SavingThrowCheck myCurrentCheck;
};

class RPG_Common_Usage_Type
 : public RPG_Common_Usage_Type_pskel
{
 public:
  RPG_Common_Usage_Type();

//   virtual void pre();
  virtual void numUses(unsigned char);
  virtual void period(unsigned int);
  virtual void interval(const RPG_Dice_Roll&);
  virtual RPG_Common_Usage post_RPG_Common_Usage_Type();

 private:
  RPG_Common_Usage myCurrentUsage;
};

class RPG_Common_Duration_Type
 : public RPG_Common_Duration_Type_pskel
{
 public:
  RPG_Common_Duration_Type();

//   virtual void pre();
  virtual void activation(unsigned int);
  virtual void interval(unsigned int);
  virtual void total(unsigned int);
//   virtual void vicinity(bool);
  virtual RPG_Common_Duration post_RPG_Common_Duration_Type();

 private:
  RPG_Common_Duration myCurrentDuration;
};

class RPG_Common_Camp_Type
  : public RPG_Common_Camp_Type_pskel,
    public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Common_Camp post_RPG_Common_Camp_Type();
};

#endif
