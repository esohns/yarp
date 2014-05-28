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

#include "rpg_common_exports.h"
#include "rpg_common.h"
#include "rpg_common_icontrol.h"
#include "rpg_common_idumpstate.h"

#include <ace/Singleton.h>
#include <ace/Time_Value.h>

// *NOTE*: global time policy (supplies gettimeofday())
static RPG_Common_TimePolicy_t RPG_COMMON_TIME_POLICY;

class RPG_Common_Export RPG_Common_Timer_Manager
 : public RPG_Common_TimerQueue_t,
   public RPG_Common_IControl,
   public RPG_Common_IDumpState
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<RPG_Common_Timer_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  void resetInterval(const long&,            // timer ID
                     const ACE_Time_Value&); // interval

  // implement RPG_Common_IControl
  virtual void stop();
  virtual bool isRunning() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef RPG_Common_TimerQueue_t inherited;

  virtual void start();

  // safety measures
  RPG_Common_Timer_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Timer_Manager(const RPG_Common_Timer_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Timer_Manager& operator=(const RPG_Common_Timer_Manager&));
  virtual ~RPG_Common_Timer_Manager();

  // helper methods
  void fini_timers();

  // *IMPORTANT NOTE*: this is only the functor, individual handlers are
  //                   managed in the queue
  RPG_Common_TimeoutUpcall_t   myTimerHandler;
	RPG_Common_TimerQueueImpl_t* myTimerQueue;
};

typedef ACE_Singleton<RPG_Common_Timer_Manager,
                      ACE_Recursive_Thread_Mutex> RPG_COMMON_TIMERMANAGER_SINGLETON;
RPG_COMMON_SINGLETON_DECLARE(ACE_Singleton,
                             RPG_Common_Timer_Manager,
                             ACE_Recursive_Thread_Mutex);

#endif
