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
#include "rpg_magic_XML_parser.h"

#include <ace/Log_Msg.h>

RPG_Magic_School RPG_Magic_School_Type::post_RPG_Magic_School_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_School_Type::post_RPG_Magic_School_Type"));

  return RPG_Magic_SchoolHelper::stringToRPG_Magic_School(post_string());
}

RPG_Magic_Domain RPG_Magic_Domain_Type::post_RPG_Magic_Domain_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Domain_Type::post_RPG_Magic_Domain_Type"));

  return RPG_Magic_DomainHelper::stringToRPG_Magic_Domain(post_string());
}

RPG_Magic_Spell RPG_Magic_Spell_Type::post_RPG_Magic_Spell_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Spell_Type::post_RPG_Magic_Spell_Type"));

  return RPG_Magic_SpellHelper::stringToRPG_Magic_Spell(post_string());
}

RPG_Magic_SpecialAbilityClass RPG_Magic_SpecialAbilityClass_Type::post_RPG_Magic_SpecialAbilityClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbilityClass_Type::post_RPG_Magic_SpecialAbilityClass_Type"));

  return RPG_Magic_SpecialAbilityClassHelper::stringToRPG_Magic_SpecialAbilityClass(post_string());
}

RPG_Magic_SpecialAbility RPG_Magic_SpecialAbility_Type::post_RPG_Magic_SpecialAbility_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbility_Type::post_RPG_Magic_SpecialAbility_Type"));

  return RPG_Magic_SpecialAbilityHelper::stringToRPG_Magic_SpecialAbility(post_string());
}

RPG_Magic_SpecialAbilityProperties_Type::RPG_Magic_SpecialAbilityProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbilityProperties_Type::RPG_Magic_SpecialAbilityProperties_Type"));

  myCurrentProperties.abilityClass = RPG_MAGIC_SPECIALABILITYCLASS_INVALID;
  myCurrentProperties.type = RPG_MAGIC_SPECIALABILITY_INVALID;
  myCurrentProperties.usage.numUses = 0;
  myCurrentProperties.usage.period = 0;
}

void RPG_Magic_SpecialAbilityProperties_Type::abilityClass(const RPG_Magic_SpecialAbilityClass& abilityClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbilityProperties_Type::abilityClass"));

  myCurrentProperties.abilityClass = abilityClass_in;
}

void RPG_Magic_SpecialAbilityProperties_Type::type(const RPG_Magic_SpecialAbility& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbilityProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_SpecialAbilityProperties_Type::usage(const RPG_Common_Usage& usage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbilityProperties_Type::usage"));

  myCurrentProperties.usage = usage_in;
}

RPG_Magic_SpecialAbilityProperties RPG_Magic_SpecialAbilityProperties_Type::post_RPG_Magic_SpecialAbilityProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpecialAbilityProperties_Type::post_RPG_Magic_SpecialAbilityProperties_Type"));

  RPG_Magic_SpecialAbilityProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.abilityClass = RPG_MAGIC_SPECIALABILITYCLASS_INVALID;
  myCurrentProperties.type = RPG_MAGIC_SPECIALABILITY_INVALID;
  myCurrentProperties.usage.numUses = 0;
  myCurrentProperties.usage.period = 0;

  return result;
}
