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

#include <map>

#include "ace/Global_Macros.h"
#include "ace/Task_Ex_T.h"
#include "ace/Time_Value.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common.h"
//#include "common_icontrol.h"
#include "common_idumpstate.h"

#include "common_time_common.h"
#include "common_timer_handler.h"

#include "rpg_engine_common.h"
#include "rpg_engine_event_common.h"
#include "rpg_engine_exports.h"

// forward declaration(s)
class RPG_Engine;

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Engine_Event_Manager
 : public ACE_Task_Ex<ACE_MT_SYNCH,
                      RPG_Engine_Event_t,
                      Common_TimePolicy_t>,
   //public Common_IControl,
   public Common_Timer_Handler,
   public Common_IDumpState
{
  typedef ACE_Task_Ex<ACE_MT_SYNCH,
                      RPG_Engine_Event_t,
                      Common_TimePolicy_t> inherited;
  typedef Common_Timer_Handler inherited2;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Engine_Event_Manager,
                             ACE_SYNCH_RECURSIVE_MUTEX>;

 public:
  void init(RPG_Engine*); // engine handle

  // manage generic event sources
	// *IMPORTANT*: fire&forget API !!!
  long schedule(RPG_Engine_Event_t*,   // event handle
                const ACE_Time_Value&, // interval (or delay)
                bool = false);         // one-shot ?
  void cancel(const long&); // timer (!) id

  // manage entities
  void add(const RPG_Engine_EntityID_t&, // id
           const ACE_Time_Value&);       // activation interval
  void remove(const RPG_Engine_EntityID_t&); // id
  void reschedule(const RPG_Engine_EntityID_t&, // id
                  const ACE_Time_Value&);       // activation interval

  // implement Common_IControl
  virtual void start();
  virtual void stop(bool = true); // locked access ?
  virtual bool isRunning() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  virtual ~RPG_Engine_Event_Manager();
  RPG_Engine_Event_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Event_Manager(const RPG_Engine_Event_Manager&))
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Event_Manager& operator=(const RPG_Engine_Event_Manager&))

  // implement task-based members
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int svc(void);

  // implement RPG_Common_ITimer interface
  virtual void handle (const void*); // ACT (if any)

	// helper methods
	void cancel_all();
	void handleEvent(const RPG_Engine_Event_t&);

  RPG_Engine* myEngine;

  // helper types
  typedef std::map<long, RPG_Engine_Event_t*> RPG_Engine_EventTimers_t;
  typedef RPG_Engine_EventTimers_t::const_iterator RPG_Engine_EventTimersConstIterator_t;
  typedef std::map<RPG_Engine_EntityID_t, long> RPG_Engine_EntityTimers_t;
  typedef RPG_Engine_EntityTimers_t::const_iterator RPG_Engine_EntityTimersConstIterator_t;

  // helper classes
  struct monster_remove
  {
    RPG_Engine* engine;
    bool        locked_access;
    bool        remove_monsters;

    bool operator()(const RPG_Engine_EntityID_t&);
  };
  struct invisible_remove
  {
    RPG_Engine*           engine;
    bool                  locked_access;
    RPG_Engine_EntityID_t entity_id;

    bool operator()(const RPG_Engine_EntityID_t&);
  };

  ACE_SYNCH_MUTEX           myLock;
  ACE_Time_Value            myGameClockStart;

  RPG_Engine_EventTimers_t  myTimers;
	// *NOTE*: used for mapping purposes only
  RPG_Engine_EntityTimers_t myEntityTimers;
};

typedef ACE_Singleton<RPG_Engine_Event_Manager,
                      ACE_SYNCH_RECURSIVE_MUTEX> RPG_ENGINE_EVENT_MANAGER_SINGLETON;
//RPG_ENGINE_SINGLETON_DECLARE(ACE_Singleton,
//                             RPG_Engine_Event_Manager,
//                             ACE_Recursive_Thread_Mutex);
#endif
