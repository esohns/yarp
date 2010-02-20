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
#include "rpg_magic_common_tools.h"

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_magic_incl.h"

#include <rpg_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Magic_SchoolToStringTable_t RPG_Magic_SchoolHelper::myRPG_Magic_SchoolToStringTable;
RPG_Magic_SubSchoolToStringTable_t RPG_Magic_SubSchoolHelper::myRPG_Magic_SubSchoolToStringTable;
RPG_Magic_DescriptorToStringTable_t RPG_Magic_DescriptorHelper::myRPG_Magic_DescriptorToStringTable;
RPG_Magic_DomainToStringTable_t RPG_Magic_DomainHelper::myRPG_Magic_DomainToStringTable;
RPG_Magic_SpellTypeToStringTable_t RPG_Magic_SpellTypeHelper::myRPG_Magic_SpellTypeToStringTable;
RPG_Magic_AbilityClassToStringTable_t RPG_Magic_AbilityClassHelper::myRPG_Magic_AbilityClassToStringTable;
RPG_Magic_AbilityTypeToStringTable_t RPG_Magic_AbilityTypeHelper::myRPG_Magic_AbilityTypeToStringTable;
RPG_Magic_Spell_EffectToStringTable_t RPG_Magic_Spell_EffectHelper::myRPG_Magic_Spell_EffectToStringTable;
RPG_Magic_Spell_DurationToStringTable_t RPG_Magic_Spell_DurationHelper::myRPG_Magic_Spell_DurationToStringTable;
RPG_Magic_Spell_PreconditionToStringTable_t RPG_Magic_Spell_PreconditionHelper::myRPG_Magic_Spell_PreconditionToStringTable;

void RPG_Magic_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::initStringConversionTables"));

  RPG_Magic_SchoolHelper::init();
  RPG_Magic_SubSchoolHelper::init();
  RPG_Magic_DescriptorHelper::init();
  RPG_Magic_DomainHelper::init();
  RPG_Magic_SpellTypeHelper::init();
  RPG_Magic_AbilityClassHelper::init();
  RPG_Magic_AbilityTypeHelper::init();
  RPG_Magic_Spell_EffectHelper::init();
  RPG_Magic_Spell_DurationHelper::init();
  RPG_Magic_Spell_PreconditionHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Magic_Common_Tools: initialized string conversion tables...\n")));
}

const std::string RPG_Magic_Common_Tools::spellTypeToString(const RPG_Magic_Spell_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellTypeToString"));

  std::string result = RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(type_in.type);
  result += ACE_TEXT_ALWAYS_CHAR(" (");
  result += RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString(type_in.school);
  if (type_in.subSchool != RPG_MAGIC_SUBSCHOOL_INVALID)
  {
    result += ACE_TEXT_ALWAYS_CHAR(" / ");
    result += RPG_Magic_SubSchoolHelper::RPG_Magic_SubSchoolToString(type_in.subSchool);
  } // end IF
  result += ACE_TEXT_ALWAYS_CHAR(")");
  if (!type_in.descriptors.empty())
  {
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    for (std::vector<RPG_Magic_Descriptor>::const_iterator iterator = type_in.descriptors.begin();
         iterator != type_in.descriptors.end();
         iterator++)
    {
      result += RPG_Magic_DescriptorHelper::RPG_Magic_DescriptorToString(*iterator);
      result += ACE_TEXT_ALWAYS_CHAR(",");
    } // end FOR
    result.erase(--(result.end()));
  } // end IF

  return result;
}

const std::string RPG_Magic_Common_Tools::spellDurationToString(const RPG_Magic_Spell_DurationProperties& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellDurationToString"));

  std::string result = RPG_Magic_Spell_DurationHelper::RPG_Magic_Spell_DurationToString(duration_in.type);
  result += ACE_TEXT_ALWAYS_CHAR(", ");
  if (duration_in.period.typeDice != RPG_DICE_DIETYPE_INVALID)
  {
    result += RPG_Dice_Common_Tools::rollToString(duration_in.period);
  } // end IF
  else
  {
    std::stringstream converter;
    converter << duration_in.duration;
    result += converter.str();
    if (duration_in.levelIncrement != 0)
    {
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter << ACE_static_cast(unsigned int, duration_in.levelIncrement);
      result += ACE_TEXT_ALWAYS_CHAR(" + [casterLevel / ");
      result += converter.str();
      result += ACE_TEXT_ALWAYS_CHAR("]");
    } // end IF
  } // end ELSE
  if (duration_in.dismissible)
    result += ACE_TEXT_ALWAYS_CHAR(", dismissible");

  return result;
}
