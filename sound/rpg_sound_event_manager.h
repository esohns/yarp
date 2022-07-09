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

#include <string>
#include <vector>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_idumpstate.h"

#include "common_timer_handler.h"

#include "rpg_sound_defines.h"

class RPG_Sound_Event_Manager
 : public Common_Timer_Handler
{
  typedef Common_Timer_Handler inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Sound_Event_Manager,
                             ACE_SYNCH_MUTEX>;

 public:
  void initialize (const std::string&,                   // (ambient) sound directory
                   bool = RPG_SOUND_AMBIENT_DEF_USE_CD); // use audio CD for ambient sound ?
  void finalize ();

  // handle ambient music
  virtual void start();
  virtual void stop();
  virtual bool isPlaying() const;

  // implement RPG_Common_ITimer interface
  // *WARNING*: NOT to be called by the user
  virtual void handle (const void*); // ACT (if any)

 private:
  virtual ~RPG_Sound_Event_Manager();
  RPG_Sound_Event_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Event_Manager(const RPG_Sound_Event_Manager&))
  ACE_UNIMPLEMENTED_FUNC(RPG_Sound_Event_Manager& operator=(const RPG_Sound_Event_Manager&))

  // helper methods
  static int dirent_selector(const dirent*);
  static int dirent_comparator(const dirent**,
                               const dirent**);
#if defined (SDL_USE)
  void initializeCD (const int& = 0); // drive (0: default)
#endif // SDL_USE
  void cancel (bool = true); // locked access ?

  // helper types
  typedef std::vector<std::string> RPG_Sound_SampleRepository_t;
  typedef RPG_Sound_SampleRepository_t::const_iterator RPG_Sound_SampleRepositoryConstIterator_t;

  mutable ACE_SYNCH_MUTEX myLock;
  long                    myTimerID;
  std::string             myRepository;
#if defined (SDL_USE)
  bool                    myUseCDROM;
  SDL_CD*                 myCDROM;
#endif // SDL_USE
  int                     myTrackOrChannel;
  bool                    myInitialized;
};

typedef ACE_Singleton<RPG_Sound_Event_Manager,
                      ACE_SYNCH_MUTEX> RPG_SOUND_EVENT_MANAGER_SINGLETON;
//RPG_SOUND_SINGLETON_DECLARE(ACE_Singleton,
//                            RPG_Sound_Event_Manager,
//                            ACE_Recursive_Thread_Mutex);
#endif
