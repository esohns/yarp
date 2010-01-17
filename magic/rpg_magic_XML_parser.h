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

class RPG_Magic_Domain_Type
 : public RPG_Magic_Domain_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_Domain post_RPG_Magic_Domain_Type();
};

class RPG_Magic_Spell_Type
 : public RPG_Magic_Spell_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_Spell post_RPG_Magic_Spell_Type();
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

class RPG_Magic_SpellProperties_Type
  : public RPG_Magic_SpellProperties_Type_pskel
{
  public:
    RPG_Magic_SpellProperties_Type();

//     virtual void pre();
    virtual void type(const RPG_Magic_Spell&);
    virtual void casterLevel(unsigned char);
    virtual void duration(const RPG_Common_Duration&);
    virtual void save(const RPG_Common_SavingThrowCheck&);
    virtual RPG_Magic_SpellProperties post_RPG_Magic_SpellProperties_Type();

  private:
    RPG_Magic_SpellProperties myCurrentProperties;
};

#endif
