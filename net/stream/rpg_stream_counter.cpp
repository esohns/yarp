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
#include "stdafx.h"

#include "rpg_stream_counter.h"

#include "rpg_common_macros.h"

#include <ace/Guard_T.h>
#include <ace/Synch.h>

RPG_Stream_Counter::RPG_Stream_Counter(const unsigned int& initCount_in)
 : myCounter(initCount_in),
   myCondition(myLock)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::RPG_Stream_Counter"));

}

RPG_Stream_Counter::~RPG_Stream_Counter()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::~RPG_Stream_Counter"));

}

void
RPG_Stream_Counter::increase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::increase"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  myCounter++;
}

void
RPG_Stream_Counter::decrease()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::decrease"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  myCounter--;

  // awaken any waiters...
  if (myCounter == 0)
  {
    // final signal
    myCondition.broadcast();
  } // end IF
}

unsigned int
RPG_Stream_Counter::refcount()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::refcount"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  return myCounter;
}

void
RPG_Stream_Counter::waitcount()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::waitcount"));

  {
    // need lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    while (myCounter)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("waiting (count: %d)...\n"),
                 myCounter));

      myCondition.wait();
    } // end WHILE
  } // end lock scope

//  ACE_DEBUG((LM_INFO,
//             ACE_TEXT("leaving...\n")));
}

void
RPG_Stream_Counter::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Counter::dump_state"));

  // dump an "atomic" state...
  {
    // lock here...
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("counter: %d\n"),
               myCounter));
  } // end lock scope
}
