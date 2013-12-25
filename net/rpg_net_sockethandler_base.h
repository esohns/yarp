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
#include "rpg_net_iconnectionmanager.h"

#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/Synch.h>
#include <ace/Reactor_Notification_Strategy.h>

template <typename ConfigType,
          typename StatisticsContainerType>
class RPG_Net_SocketHandlerBase
 : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>,
   public RPG_Net_IConnection<ConfigType,
                              StatisticsContainerType>
{
 protected:
  // convenient types
  typedef RPG_Net_IConnectionManager<ConfigType,
                                     StatisticsContainerType> MANAGER_t;

 public:
  virtual ~RPG_Net_SocketHandlerBase(); // we'll self-destruct !

  //check if registration with the connection manager was OK...
  virtual int open(void* = NULL); // args
  // *WARNING*: the default ACE_Svc_Handler implementation calls
  // handle_close(), which we DON'T want in the case where we have our own
  // worker (registered with the reactor, we would get "handle_closed" twice,
  // leading to serious mayhem)
  // *WARNING*: the current algorithm works only for 1 worker !!!
  virtual int close(u_long = 0); // args
//   virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,                        // handle
//                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK); // event mask

  // implement RPG_Net_IConnection
  virtual void info(ACE_HANDLE&,     // handle
		                ACE_INET_Addr&,  // local SAP
										ACE_INET_Addr&); // remote SAP
  virtual void init(const ConfigType&);
//   virtual const bool isRegistered() const;
  virtual void abort();
  virtual unsigned int getID() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 protected:
  // meant to be sub-classed
  RPG_Net_SocketHandlerBase(MANAGER_t*); // manager handle

  ConfigType                        myUserData;
  bool                              myIsInitialized;
  ACE_Reactor_Notification_Strategy myNotificationStrategy;

 private:
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase(const RPG_Net_SocketHandlerBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase& operator=(const RPG_Net_SocketHandlerBase&));

  // *NOTE*: we save this so we can de-register even when our "handle"
  // (getID()) has gone stale...
  unsigned int                      myID;
  bool                              myIsRegistered;
  MANAGER_t*                        myManager;
};

// include template implementation
#include "rpg_net_sockethandler_base.i"

#endif
