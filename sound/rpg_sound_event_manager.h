/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef RPG_SOUND_EVENT_MANAGER_H
#define RPG_SOUND_EVENT_MANAGER_H

#include "rpg_sound_exports.h"
#include "rpg_sound_defines.h"

#include "rpg_common_itimer.h"
#include "rpg_common_idumpstate.h"

#include <SDL.h>

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Sound_Export RPG_Sound_Event_Manager
 : public RPG_Common_ITimer
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Sound_Event_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  void init(const std::string&,                          // (ambient) sound directory
            const bool& = RPG_SOUND_AMBIENT_DEF_USE_CD); // use audio CD for ambient sound ?
  void fini();

  // handle ambient music
  virtual void start();
  virtual void stop();
  virtual bool isPlaying() const;

  // implement RPG_Common_ITimer interface
  // *WARNING*: NOT to be called by the user
  virtual void handleTimeout(const void*); // ACT (if any)

 private:
  virtual ~RPG_Sound_Event_Manager();
  RPG_Sound_Event_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Event_Manager(const RPG_Sound_Event_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Event_Manager& operator=(const RPG_Sound_Event_Manager&));

  // helper methods
  static int dirent_selector(const dirent*);
  static int dirent_comparator(const dirent**,
                               const dirent**);
  void initCD(const int& = 0); // drive (0: default)
  void cancel(const bool& = true); // locked access ?

  // helper types
  typedef std::vector<std::string> RPG_Sound_SampleRepository_t;
  typedef RPG_Sound_SampleRepository_t::const_iterator RPG_Sound_SampleRepositoryConstIterator_t;

  mutable ACE_Thread_Mutex myLock;
  long                     myTimerID;
  std::string              myRepository;
  bool                     myUseCDROM;
  SDL_CD*                  myCDROM;
  int                      myTrackOrChannel;
  bool                     myInitialized;
};

typedef ACE_Singleton<RPG_Sound_Event_Manager,
                      ACE_Recursive_Thread_Mutex> RPG_SOUND_EVENT_MANAGER_SINGLETON;
RPG_SOUND_SINGLETON_DECLARE(ACE_Singleton,
                            RPG_Sound_Event_Manager,
                            ACE_Recursive_Thread_Mutex);
#endif
