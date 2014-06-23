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

#include "rpg_common_referencecounter_base.h"

#include "rpg_common_macros.h"

#include <ace/Guard_T.h>

RPG_Common_ReferenceCounterBase::RPG_Common_ReferenceCounterBase(const unsigned int& initCount_in,
                                                                 const bool& deleteOnZero_in)
 : myCounter(initCount_in),
   myDeleteOnZero(deleteOnZero_in),
   myCondition(myLock)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::RPG_Common_ReferenceCounterBase"));

}

//RPG_Common_ReferenceCounterBase::RPG_Common_ReferenceCounterBase(const RPG_Common_ReferenceCounterBase& counter_in)
// : myCounter(counter_in.myCounter),
//   myDeleteOnZero(counter_in.myDeleteOnZero),
//   myCondition(counter_in.myLock) // *NOTE*: use the SAME lock !
//{
//  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::RPG_Common_ReferenceCounterBase"));
//
//}

RPG_Common_ReferenceCounterBase::~RPG_Common_ReferenceCounterBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::~RPG_Common_ReferenceCounterBase"));

}

void
RPG_Common_ReferenceCounterBase::increase()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::increase"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  myCounter++;
}

void
RPG_Common_ReferenceCounterBase::decrease()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::decrease"));

  bool destroy = false;

  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    myCounter--;

    // awaken any waiters...
    if (myCounter == 0)
    {
      // final signal
      myCondition.broadcast();

      destroy = myDeleteOnZero;
    } // end IF
  } // end lock scope

  if (destroy)
    delete this;
}

unsigned int
RPG_Common_ReferenceCounterBase::count()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::count"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  return myCounter;
}

void
RPG_Common_ReferenceCounterBase::wait_zero()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::wait_zero"));

  {
    // need lock
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    while (myCounter)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("waiting (count: %u)...\n"),
                 myCounter));

      myCondition.wait();
    } // end WHILE
  } // end lock scope

//  ACE_DEBUG((LM_INFO,
//             ACE_TEXT("leaving...\n")));
}

void
RPG_Common_ReferenceCounterBase::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Common_ReferenceCounterBase::dump_state"));

  // dump an "atomic" state...
  {
    // lock here...
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("count: %u\n"),
               myCounter));
  } // end lock scope
}
