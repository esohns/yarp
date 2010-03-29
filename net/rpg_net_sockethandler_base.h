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

#ifndef RPG_NET_SOCKETHANDLER_BASE_H
#define RPG_NET_SOCKETHANDLER_BASE_H

#include "rpg_net_iconnection.h"

#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>

class RPG_Net_SocketHandler_Base
 : public ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_NULL_SYNCH>,
   public RPG_Net_IConnection
{
 private:
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_NULL_SYNCH> inherited;

 public:
  virtual ~RPG_Net_SocketHandler_Base(); // we'll self-destruct !

  // *IMPORTANT NOTE*: we overload this to automatically (de-)register ourselves
  // with the connection manager... this way, we can always keep a consistent
  // state of currently open connections
  // *IMPORTANT NOTE*: if this returns -1, the caller will still need to
  // clean "this" up !
  virtual int open(void*); // args
  virtual int handle_close(ACE_HANDLE,        // handle
                           ACE_Reactor_Mask); // mask

  // implement RPG_Common_IConnection
  virtual void abort();
  virtual const unsigned long getID() const;
  virtual void dump_state() const;

 protected:
  RPG_Net_SocketHandler_Base();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandler_Base(const RPG_Net_SocketHandler_Base&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandler_Base& operator=(const RPG_Net_SocketHandler_Base&));

  bool myIsRegistered;
};

#endif
