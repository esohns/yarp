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

#ifndef RPG_COMMON_TIMER_MANAGER_H
#define RPG_COMMON_TIMER_MANAGER_H

#include "rpg_common_timerhandler.h"
#include "rpg_common_idumpstate.h"

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

// timer queue
#include <ace/Timer_Queue_T.h>
#include <ace/Timer_Heap_T.h>
#include <ace/Timer_Queue_Adapters.h>

// forward declarations
class RPG_Common_ITimer;

class RPG_Common_Timer_Manager
 : public RPG_Common_IDumpState
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<RPG_Common_Timer_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  const bool scheduleTimer(const ACE_Event_Handler&, // timer dispatch handler
                           const ACE_Time_Value&,    // period (starting NOW)
                           const bool&,              // reschedule interval ?
                           int&);                    // return value: timer ID
  void cancelTimer(const int&); // timer ID

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  // safety measures
  RPG_Common_Timer_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Timer_Manager(const RPG_Common_Timer_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Timer_Manager& operator=(const RPG_Common_Timer_Manager&));
  virtual ~RPG_Common_Timer_Manager();

  // these typedefs ensure that we use the minimal amount of locking necessary
  typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_Null_Mutex> UPCALL_TYPE;
  typedef ACE_Timer_Heap_T<ACE_Event_Handler*, UPCALL_TYPE, ACE_Null_Mutex> TIMERHEAP_TYPE;
  typedef ACE_Timer_Heap_Iterator_T<ACE_Event_Handler*, UPCALL_TYPE, ACE_Null_Mutex> TIMERHEAPITERATOR_TYPE;
// typedef ACE_Thread_Timer_Queue_Adapter<TIMERHEAP_TYPE> TIMERQUEUE_TYPE;
  typedef ACE_Thread_Timer_Queue_Adapter<TIMERHEAP_TYPE> RPG_Common_TimerQueue_t;

  // helper methods
  void fini_timers();

  // make API re-entrant
  mutable ACE_Thread_Mutex myLock;

  // timer queue
  RPG_Common_TimerQueue_t  myTimerQueue;
};

typedef ACE_Singleton<RPG_Common_Timer_Manager,
                      ACE_Recursive_Thread_Mutex> RPG_COMMON_TIMERMANAGER_SINGLETON;

#endif
