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

RPG_Magic_AbilityClass RPG_Magic_AbilityClass_Type::post_RPG_Magic_AbilityClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_AbilityClass_Type::post_RPG_Magic_AbilityClass_Type"));

  return RPG_Magic_AbilityClassHelper::stringToRPG_Magic_AbilityClass(post_string());
}

RPG_Magic_AbilityType RPG_Magic_AbilityType_Type::post_RPG_Magic_AbilityType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_AbilityType_Type::post_RPG_Magic_AbilityType_Type"));

  return RPG_Magic_AbilityTypeHelper::stringToRPG_Magic_AbilityType(post_string());
}

RPG_Magic_SpellProperties_Type::RPG_Magic_SpellProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellProperties_Type::RPG_Magic_SpellProperties_Type"));

  myCurrentProperties.type = RPG_MAGIC_SPELL_INVALID;
  myCurrentProperties.casterLevel = 0;
  myCurrentProperties.duration.activation = 0;
  myCurrentProperties.duration.interval = 0;
  myCurrentProperties.duration.total = 0;
  myCurrentProperties.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.save.difficultyClass = 0;
}

void RPG_Magic_SpellProperties_Type::type(const RPG_Magic_Spell& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Magic_SpellProperties_Type::casterLevel(unsigned char casterLevel_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellProperties_Type::casterLevel"));

  myCurrentProperties.casterLevel = casterLevel_in;
}

void RPG_Magic_SpellProperties_Type::duration(const RPG_Common_Duration& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellProperties_Type::duration"));

  myCurrentProperties.duration = duration_in;
}

void RPG_Magic_SpellProperties_Type::save(const RPG_Common_SavingThrowCheck& save_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellProperties_Type::save"));

  myCurrentProperties.save = save_in;
}

RPG_Magic_SpellProperties RPG_Magic_SpellProperties_Type::post_RPG_Magic_SpellProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_SpellProperties_Type::post_RPG_Magic_SpellProperties_Type"));

  RPG_Magic_SpellProperties result = myCurrentProperties;

  // clear structure
  myCurrentProperties.type = RPG_MAGIC_SPELL_INVALID;
  myCurrentProperties.casterLevel = 0;
  myCurrentProperties.duration.activation = 0;
  myCurrentProperties.duration.interval = 0;
  myCurrentProperties.duration.total = 0;
  myCurrentProperties.save.type = RPG_COMMON_SAVINGTHROW_INVALID;
  myCurrentProperties.save.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  myCurrentProperties.save.difficultyClass = 0;

  return result;
}
