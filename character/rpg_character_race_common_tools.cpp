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
#include "rpg_character_race_common_tools.h"

#include "rpg_character_race.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Character_Race_t
RPG_Character_Race_Common_Tools::raceXMLTreeToRace(const RPG_Character_PlayerXML_XMLTree_Type::race_sequence& races_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Common_Tools::raceXMLTreeToRace"));

  RPG_Character_Race_t result;

  RPG_Character_Race current;
  for (RPG_Character_PlayerXML_XMLTree_Type::race_const_iterator iterator = races_in.begin();
       iterator != races_in.end();
       iterator++)
  {
    current = RPG_Character_RaceHelper::stringToRPG_Character_Race(*iterator);
    if (current > RACE_NONE)
      result.set(current - 1); // *NOTE*: -1 !
  } // end FOR

  return result;
}
