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
#include "rpg_character_class_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

const RPG_Character_MetaClass
RPG_Character_Class_Common_Tools::subClassToMetaClass(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Class_Common_Tools::subClassToMetaClass"));

  switch (subClass_in)
  {
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_RANGER:
    case SUBCLASS_BARBARIAN:
    case SUBCLASS_WARLORD:
    {
      return METACLASS_WARRIOR;
    }
    case SUBCLASS_WIZARD:
    case SUBCLASS_SORCERER:
    case SUBCLASS_WARLOCK:
    {
      return METACLASS_WIZARD;
    }
    case SUBCLASS_CLERIC:
    case SUBCLASS_DRUID:
    case SUBCLASS_MONK:
    case SUBCLASS_AVENGER:
    case SUBCLASS_INVOKER:
    case SUBCLASS_SHAMAN:
    {
      return METACLASS_PRIEST;
    }
    case SUBCLASS_THIEF:
    case SUBCLASS_BARD:
    {
      return METACLASS_ROGUE;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: %s, aborting\n"),
                 RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));
    }
  } // end SWITCH

  return RPG_CHARACTER_METACLASS_INVALID;
}

RPG_Character_Class
RPG_Character_Class_Common_Tools::classXMLTreeToClass(const RPG_Character_ClassXML_XMLTree_Type& class_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Class_Common_Tools::classXMLTreeToClass"));

  RPG_Character_Class result;

  result.metaClass = RPG_Character_MetaClassHelper::stringToRPG_Character_MetaClass(class_in.metaClass());
  for (RPG_Character_ClassXML_XMLTree_Type::subClass_const_iterator iterator = class_in.subClass().begin();
       iterator != class_in.subClass().end();
       iterator++)
    result.subClasses.insert(RPG_Common_SubClassHelper::stringToRPG_Common_SubClass(*iterator));

  return result;
}
