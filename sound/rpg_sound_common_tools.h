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
#ifndef RPG_SOUND_COMMON_TOOLS_H
#define RPG_SOUND_COMMON_TOOLS_H

#include "rpg_sound_incl.h"
#include "rpg_sound_common.h"

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Sound_Common_Tools
{
 public:
  static void init(const std::string&,    // sound directory
                   const unsigned long&); // cache size
  static void playSound(const RPG_Sound_Event&); // event

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Sound_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Common_Tools(const RPG_Sound_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Common_Tools& operator=(const RPG_Sound_Common_Tools&));

  // helper methods
  // init string conversion (and any other) tables
  static void initStringConversionTables();

  static std::string            mySoundDirectory;
  static ACE_Thread_Mutex       myLock;
  static unsigned long          myOldestCacheEntry;
  static unsigned long          myCacheSize;
  static RPG_Sound_SoundCache_t mySoundCache;
  static bool                   myInitialized;
};

#endif
