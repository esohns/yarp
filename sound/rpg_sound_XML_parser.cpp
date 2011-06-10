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
#include "rpg_sound_XML_parser.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Sound_Category RPG_Sound_Category_Type::post_RPG_Sound_Category_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Category_Type::post_RPG_Sound_Category_Type"));

  return RPG_Sound_CategoryHelper::stringToRPG_Sound_Category(post_string());
}

RPG_Sound_Event RPG_Sound_Event_Type::post_RPG_Sound_Event_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Event_Type::post_RPG_Sound_Event_Type"));

  return RPG_Sound_EventHelper::stringToRPG_Sound_Event(post_string());
}

RPG_Sound_Type::RPG_Sound_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Type::RPG_Sound_Type"));

  myCurrentSound.category = RPG_SOUND_CATEGORY_INVALID;
  myCurrentSound.event = RPG_SOUND_EVENT_INVALID;
  myCurrentSound.file.clear();
  myCurrentSound.interval = 0;
}

void RPG_Sound_Type::category(const RPG_Sound_Category& category_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Type::category"));

  myCurrentSound.category = category_in;
}

void RPG_Sound_Type::event(const RPG_Sound_Event& event_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Type::event"));

  myCurrentSound.event = event_in;
}

void RPG_Sound_Type::file(const ::std::string& file_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Type::file"));

  myCurrentSound.file = file_in;
}

void RPG_Sound_Type::interval(const unsigned char interval_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Type::interval"));

  myCurrentSound.interval = interval_in;
}

RPG_Sound RPG_Sound_Type::post_RPG_Sound_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Type::post_RPG_Sound_Type"));

  RPG_Sound result = myCurrentSound;

  // clear structure
  myCurrentSound.category = RPG_SOUND_CATEGORY_INVALID;
  myCurrentSound.event = RPG_SOUND_EVENT_INVALID;
  myCurrentSound.file.clear();
  myCurrentSound.interval = 0;

  return result;
}

RPG_Sound_Dictionary_Type::RPG_Sound_Dictionary_Type(RPG_Sound_Dictionary_t* soundDictionary_in)
 : myDictionary(soundDictionary_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary_Type::RPG_Sound_Dictionary_Type"));

}

RPG_Sound_Dictionary_Type::~RPG_Sound_Dictionary_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary_Type::~RPG_Sound_Dictionary_Type"));

}

// void RPG_Sound_Dictionary_Type::pre()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary_Type::pre"));
//
// }

void RPG_Sound_Dictionary_Type::sound(const RPG_Sound& sound_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary_Type::sound"));

  RPG_Sound_t sound = sound_in;

  myDictionary->insert(std::make_pair(sound.event, sound));
}

void RPG_Sound_Dictionary_Type::post_RPG_Sound_Dictionary_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary_Type::post_RPG_Sound_Dictionary_Type"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing dictionary, retrieved %d sound(s)...\n"),
             myDictionary->size()));
}
