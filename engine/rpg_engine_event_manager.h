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

#ifndef RPG_ENGINE_EVENT_MANAGER_H
#define RPG_ENGINE_EVENT_MANAGER_H

#include "rpg_engine_exports.h"
#include "rpg_engine_event_common.h"

#include <rpg_common_icontrol.h>
#include <rpg_common_itimer.h>
#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Task.h>
#include <ace/Time_Value.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <map>

// forward declaration(s)
class RPG_Engine_Level;

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Engine_Export RPG_Engine_Event_Manager
 : public ACE_Task<ACE_MT_SYNCH>,
   public RPG_Common_IControl,
   public RPG_Common_ITimer,
   public RPG_Common_IDumpState
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Engine_Event_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  void init(const unsigned int&, // max # spawned entities (consecutive)
            RPG_Engine_Level*);  // level engine

  // manage event sources
  long schedule(const RPG_Engine_Event&, // event
                const ACE_Time_Value&,   // interval (or delay)
                const bool& = false);    // one-shot ?
  void remove(const long&); // id

  // implement RPG_Common_IControl
  virtual void start();
  virtual void stop();
  virtual bool isRunning() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

  // implement RPG_Common_ITimer interface
  // *WARNING*: NOT to be called by the user
  virtual void handleTimeout(const void*); // ACT (if any)

 private:
  typedef ACE_Task<ACE_MT_SYNCH> inherited;

  // safety measures
  virtual ~RPG_Engine_Event_Manager();
  RPG_Engine_Event_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Event_Manager(const RPG_Engine_Event_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Event_Manager& operator=(const RPG_Engine_Event_Manager&));

  // override task-based members
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int svc(void);

  static void wait_all();

  // trigger (one round of) entity actions
  void handleEntities();

  RPG_Engine_Level*          myEngine;
  unsigned int               myMaxNumSpawnedEntities;

  // helper types
  typedef std::map<long, RPG_Engine_Event*> RPG_Engine_EventTimerIDs_t;
  typedef RPG_Engine_EventTimerIDs_t::const_iterator RPG_Engine_EventTimerIDsConstIterator_t;

  ACE_Thread_Mutex           myLock;
  RPG_Engine_EventTimerIDs_t myTimers;
};

typedef ACE_Singleton<RPG_Engine_Event_Manager,
                      ACE_Recursive_Thread_Mutex> RPG_ENGINE_EVENT_MANAGER_SINGLETON;
RPG_ENGINE_SINGLETON_DECLARE(ACE_Singleton,
                             RPG_Engine_Event_Manager,
                             ACE_Recursive_Thread_Mutex);
#endif
