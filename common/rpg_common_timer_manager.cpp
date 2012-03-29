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
#include "stdafx.h"

#include "rpg_common_timer_manager.h"

#include "rpg_common_macros.h"
#include "rpg_common_itimer.h"

RPG_Common_Timer_Manager::RPG_Common_Timer_Manager()
 : myTimerQueue(NULL, // thread manager --> use default
                NULL) // timer queue --> allocate new
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Timer_Manager::RPG_Common_Timer_Manager"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...\n")));

  // ok: activate timer queue
  if (myTimerQueue.activate() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to activate() timer dispatch queue: \"%m\", continuing\n")));
//   else
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("activating timer dispatch queue...DONE\n")));
}

RPG_Common_Timer_Manager::~RPG_Common_Timer_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Timer_Manager::~RPG_Common_Timer_Manager"));

  // clean up
  {
    // synch access to timer queue
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    fini_timers();

    myTimerQueue.deactivate();
    // make sure the dispatcher thread is really dead...
    myTimerQueue.wait();
  } // end lock scope

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivated timers and dispatcher...\n")));
}

void
RPG_Common_Timer_Manager::fini_timers()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Timer_Manager::fini_timers"));

  TIMERHEAPITERATOR_TYPE iterator(*myTimerQueue.timer_queue());
  for (iterator.first();
       !iterator.isdone();
       iterator.next())
    if (myTimerQueue.cancel(iterator.item()->get_timer_id(), NULL) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to cancel() timer (ID: %u): \"%m\", continuing\n"),
                 iterator.item()->get_timer_id()));
}

bool
RPG_Common_Timer_Manager::scheduleTimer(ACE_Event_Handler* handler_in,
                                        const void* act_in,
                                        const ACE_Time_Value& wakeup_in,
                                        const bool& isOneShot_in,
                                        int& timerID_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Timer_Manager::scheduleTimer"));

  ACE_ASSERT(handler_in);

  // init return value(s)
  timerID_out = 0;

  // synch access to timer queue
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // schedule a new timer
    timerID_out = myTimerQueue.schedule(handler_in,                                          // handler
                                        act_in,                                              // argument
                                        ACE_OS::gettimeofday () + wakeup_in,                 // wakeup time
                                        (!isOneShot_in ? wakeup_in : ACE_Time_Value::zero)); // interval ?
    if (timerID_out == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule() timer: \"%m\", aborting\n")));

      timerID_out = 0;

      return false;
    } // end IF
  } // end lock scope

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("scheduled new %s timer (wakeup: %us; ID: %u)...\n"),
//              (isOneShot_in ? ACE_TEXT("(regular)") : ACE_TEXT("")),
//              wakeup_in.sec(),
//              timerID_out));

  return true;
}

void
RPG_Common_Timer_Manager::cancelTimer(const int& timerID_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Timer_Manager::cancelTimer"));

  // synch access to timer queue
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    if (myTimerQueue.cancel(timerID_in, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to cancel() timer (ID: %u): \"%m\", returning\n"),
                 timerID_in));

      return;
    } // end IF
  } // end lock scope

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("cancelled timer (ID: %u)...\n"),
//              timerID_in));
}

void
RPG_Common_Timer_Manager::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Timer_Manager::dump_state"));

  // synch access to timer queue
  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // *TODO*: print some meaningful data
  ACE_ASSERT(false);
}
