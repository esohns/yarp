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

#include "rpg_magic_XML_parser_base.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Magic_School_Type
 : public RPG_Magic_School_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_School post_RPG_Magic_School_Type();
};

class RPG_Magic_Domain_Type
 : public RPG_Magic_Domain_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_Domain post_RPG_Magic_Domain_Type();
};

class RPG_Magic_Spell_Type
 : public RPG_Magic_Spell_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_Spell post_RPG_Magic_Spell_Type();
};

class RPG_Magic_SpecialAbilityClass_Type
 : public RPG_Magic_SpecialAbilityClass_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_SpecialAbilityClass post_RPG_Magic_SpecialAbilityClass_Type();
};

class RPG_Magic_SpecialAbility_Type
 : public RPG_Magic_SpecialAbility_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Magic_SpecialAbility post_RPG_Magic_SpecialAbility_Type();
};

class RPG_Magic_SpecialAbilityProperties_Type
 : public RPG_Magic_SpecialAbilityProperties_Type_pimpl
{
 public:
  RPG_Magic_SpecialAbilityProperties_Type();

//   virtual void pre();
  virtual void abilityClass(const RPG_Magic_SpecialAbilityClass&);
  virtual void type(const RPG_Magic_SpecialAbility&);
  virtual void usage(const RPG_Common_Usage&);
  virtual RPG_Magic_SpecialAbilityProperties post_RPG_Magic_SpecialAbilityProperties_Type();

 private:
  RPG_Magic_SpecialAbilityProperties myCurrentProperties;
};

#endif
