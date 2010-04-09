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

#ifndef RPG_NET_SOCKETHANDLER_H
#define RPG_NET_SOCKETHANDLER_H

#include "rpg_net_sockethandler_base.h"

#include <ace/Global_Macros.h>
#include <ace/Atomic_Op.h>
#include <ace/Time_Value.h>

class RPG_Net_SocketHandler
 : public RPG_Net_SocketHandler_Base
{
 public:
  RPG_Net_SocketHandler();
  virtual ~RPG_Net_SocketHandler(); // we'll self-destruct !

  virtual int open(void*); // args

  // *NOTE*: this will work only for single-threaded reactors where
  // handle_input and handle_timeout are always invoked SEQUENTIALLY (and NEVER
  // "SIMULTANEOUSLY")

  // *NOTE*: the default behavior simply ignores any received data...
  virtual int handle_input(ACE_HANDLE); // handle

  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE,
                           ACE_Reactor_Mask);

 private:
  typedef RPG_Net_SocketHandler_Base inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandler(const RPG_Net_SocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandler& operator=(const RPG_Net_SocketHandler&));

  // helper methods
  inline void cancelTimer()
  {
    // *NOTE*: DONT'T invoke our send timer anymore ! Otherwise
    // there's a racing condition (especially when using a multithreaded
    // reactor !!!)
    if (reactor()->cancel_timer(this,    // handler
                                1) != 1) // don't call handle_close()
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%s\", continuing\n"),
                 ACE_OS::strerror(ACE_OS::last_error())));
    } // end IF
    myTimerID = -1;
  };

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex, unsigned long> myCurrentID;

  // remember our timer ID
  int myTimerID;
};

#endif
