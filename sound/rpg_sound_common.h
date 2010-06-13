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
#ifndef RPG_SOUND_COMMON_H
#define RPG_SOUND_COMMON_H

#include "rpg_sound_incl.h"

#include <SDL/SDL_mixer.h>

#include <string>
#include <map>
#include <vector>

typedef RPG_Sound RPG_Sound_t;
typedef std::map<RPG_Sound_Event, RPG_Sound_t> RPG_Sound_Dictionary_t;
typedef RPG_Sound_Dictionary_t::const_iterator RPG_Sound_DictionaryIterator_t;

struct RPG_Sound_SoundCacheNode_t
{
  RPG_Sound_Event event;
  Mix_Chunk*      chunk;

  inline bool operator==(const RPG_Sound_SoundCacheNode_t& rhs_in) const
  { return (this->event == rhs_in.event); }
};
typedef std::vector<RPG_Sound_SoundCacheNode_t> RPG_Sound_SoundCache_t;
typedef RPG_Sound_SoundCache_t::iterator RPG_Sound_SoundCacheIterator_t;

#endif