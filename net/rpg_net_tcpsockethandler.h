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

#ifndef RPG_NET_TCPSOCKETHANDLER_H
#define RPG_NET_TCPSOCKETHANDLER_H

#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"
#include "ace/Synch.h"
#include "ace/Event_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"

class RPG_Net_TCPSocketHandler
 : public ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_MT_SYNCH>
{
 public:
  //// override some event handler methods
  //virtual ACE_Event_Handler::Reference_Count add_reference (void);
  //// *IMPORTANT NOTE*: this API works as long as the reactor doesn't manage
  //// the lifecycle of the event handler. To avoid unforseen behavior, make sure
  //// that the event handler has been properly deregistered from the reactor
  //// before removing the last reference (delete on zero).
  //virtual ACE_Event_Handler::Reference_Count remove_reference (void);

  // override some task-based members
  virtual int open(void* = NULL); // args

  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,                        // handle
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK); // event mask

 protected:
  RPG_Net_TCPSocketHandler ();
  virtual ~RPG_Net_TCPSocketHandler ();

  ACE_Reactor_Notification_Strategy myNotificationStrategy;

 private:
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_MT_SYNCH> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_TCPSocketHandler (const RPG_Net_TCPSocketHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_TCPSocketHandler& operator=(const RPG_Net_TCPSocketHandler&));
};

#endif
