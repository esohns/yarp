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
#include "stdafx.h"

#include "rpg_character_race_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Common_Size
RPG_Character_Race_Common_Tools::race2Size(const RPG_Character_Race_t& races_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Common_Tools::race2Speed"));

  // sanity checks
  if (races_in.none())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid race (was: \"%s\"), aborting\n"),
               RPG_Character_RaceHelper::RPG_Character_RaceToString(RACE_NONE).c_str()));

    return RPG_COMMON_SIZE_INVALID;
  } // end IF

  RPG_Common_Size result = SIZE_SMALL;

  RPG_Common_Size base_size = SIZE_SMALL;
  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < races_in.size();
       index++, race_index++)
    if (races_in.test(index))
    {
      base_size = RPG_Character_Race_Common_Tools::race2Size(static_cast<RPG_Character_Race>(race_index));
      if (base_size > result)
        result = base_size;
    } // end IF

  return result;
}

unsigned char
RPG_Character_Race_Common_Tools::race2Speed(const RPG_Character_Race& race_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Common_Tools::race2Speed"));

  switch (race_in)
  {
    case RACE_DWARF:
    case RACE_GNOME:
    case RACE_HALFLING:
      return 20;
    case RACE_HUMAN:
    case RACE_ELF:
    case RACE_ORC:
      return 30;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid race (was: \"%s\"), aborting\n"),
                 RPG_Character_RaceHelper::RPG_Character_RaceToString(race_in).c_str()));

      break;
    }
  } // end SWITCH

  return 0;
}

bool
RPG_Character_Race_Common_Tools::isCompatible(const RPG_Character_Race& race1_in,
                                              const RPG_Character_Race& race2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Common_Tools::isCompatible"));

	switch (race1_in)
	{
	  case RACE_DWARF:
		{
			switch (race2_in)
			{
			  case RACE_DWARF:
				case RACE_GNOME:
				case RACE_HALFLING:
					return true;
				default:
					break;
			} // end SWITCH

			break;
		}
		case RACE_ELF:
		{
			switch (race2_in)
			{
			  case RACE_ELF:
				case RACE_HUMAN:
					return true;
				default:
					break;
			} // end SWITCH

			break;
		}
		case RACE_GNOME:
		{
			switch (race2_in)
			{
			  case RACE_DWARF:
				case RACE_GNOME:
				case RACE_HALFLING:
					return true;
				default:
					break;
			} // end SWITCH

			break;
		}
		case RACE_HALFLING:
		{
			switch (race2_in)
			{
			  case RACE_DWARF:
				case RACE_GNOME:
				case RACE_HALFLING:
					return true;
				default:
					break;
			} // end SWITCH

			break;
		}
		case RACE_HUMAN:
		{
			switch (race2_in)
			{
			  case RACE_ELF:
				case RACE_HUMAN:
				case RACE_ORC:
					return true;
				default:
					break;
			} // end SWITCH

			break;
		}
		case RACE_ORC:
		{
			switch (race2_in)
			{
				case RACE_HUMAN:
				case RACE_ORC:
					return true;
				default:
					break;
			} // end SWITCH

			break;
		}
		default:
			break;
	} // end SWITCH

	return false;
}

bool
RPG_Character_Race_Common_Tools::hasRace(const RPG_Character_Race_t& races_in,
                                         const RPG_Character_Race& race_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Common_Tools::race2Speed"));

  // sanity checks
  ACE_ASSERT(race_in < RPG_CHARACTER_RACE_MAX);
  if (race_in == RACE_NONE)
    return races_in.none();

  return races_in.test(static_cast<size_t>(race_in) - 1);
}

RPG_Common_Size
RPG_Character_Race_Common_Tools::race2Size(const RPG_Character_Race& race_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Race_Common_Tools::race2Size"));

  switch (race_in)
  {
    case RACE_GNOME:
    case RACE_HALFLING:
      return SIZE_SMALL;
    case RACE_DWARF:
    case RACE_ELF:
    case RACE_HUMAN:
    case RACE_ORC:
      return SIZE_MEDIUM;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid race (was: \"%s\"), aborting\n"),
                 RPG_Character_RaceHelper::RPG_Character_RaceToString(race_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_COMMON_SIZE_INVALID;
}
