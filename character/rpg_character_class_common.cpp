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
#include "rpg_character_class_common.h"

#include "rpg_character_metaclass.h"
#include "rpg_character_subclass.h"

#include <ace/Log_Msg.h>

#include <string>

const RPG_Character_MetaClass RPG_Character_Class_Common_Tools::subClassToMetaClass(const RPG_Character_SubClass& subClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Class_Common_Tools::subClassToMetaClass"));

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
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid subclass: %s, aborting\n"),
                 RPG_Character_SubClassHelper::RPG_Character_SubClassToString(subClass_in).c_str()));
    }
  } // end SWITCH
}
