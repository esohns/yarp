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

#include <string>

#if defined (SDL_USE)
#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"
#endif // SDL_USE

#include "ace/Global_Macros.h"
#include "ace/Thread_Mutex.h"
#include "ace/Time_Value.h"

#include "rpg_sound_defines.h"
#include "rpg_sound_event.h"
#include "rpg_sound_common.h"
//#include "rpg_sound_incl.h"

class RPG_Sound_Common_Tools
{
 public:
  static bool initialize (const struct RPG_Sound_SDLConfiguration&, // SDL config parameters
                          const std::string&,                       // sound directory
                          bool = RPG_SOUND_AMBIENT_DEF_USE_CD,      // use CD ?
                          unsigned int = RPG_SOUND_DEF_CACHESIZE,   // cache size
                          bool = false);                            // mute ?
  static void finalize ();

  static void soundToFile (const RPG_Sound_t&, // sound
                           std::string&);      // return value: FQ filename

  // *NOTE*: returns the channel# that is playing the sound/music/...
  static int play (enum RPG_Sound_Event, // event
                   ACE_Time_Value&);     // return value: length
  static int play (const std::string&, // sound file
                   ACE_UINT8,          // volume (0-128)
                   ACE_Time_Value&);   // return value: length
  static bool isPlaying (int = -1); // channel (-1: ALL channels)
  static void stop (int = -1); // channel (-1: ALL channels)

#if defined (SDL_USE)
  // *NOTE*: returns the track# that is playing
  // *NOTE*: passing NULL will invoke all commands on the last drive SDL_OpenCD()ed
  static int playRandomTrack (SDL_CD* = NULL); // CDROM handle
#endif // SDL_USE

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Sound_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Common_Tools (const RPG_Sound_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Common_Tools& operator= (const RPG_Sound_Common_Tools&))

  static void initializeStringConversionTables ();

  static bool                              myIsMuted;
  static std::string                       mySoundDirectory;
  static struct RPG_Sound_SDLConfiguration myConfig;

  static ACE_Thread_Mutex                  myCacheLock;
  static unsigned int                      myOldestCacheEntry;
  static unsigned int                      myCacheSize;
  static RPG_Sound_SoundCache_t            mySoundCache;

  static bool                              myInitialized;
};

#endif
