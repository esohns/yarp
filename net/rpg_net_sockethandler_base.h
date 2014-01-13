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

#include "rpg_common_referencecounter_base.h"

#include "rpg_net_iconnection.h"
#include "rpg_net_iconnectionmanager.h"

#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/Synch.h>
#include <ace/Reactor_Notification_Strategy.h>

template <typename ConfigType,
          typename StatisticsContainerType>
class RPG_Net_SocketHandlerBase
 : public ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_MT_SYNCH>,
	 public RPG_Common_ReferenceCounterBase,
   public RPG_Net_IConnection<StatisticsContainerType>
{
 public:
	// override some event handler methods
	virtual ACE_Event_Handler::Reference_Count add_reference(void);
  // *IMPORTANT NOTE*: this API works as long as the reactor doesn't manage
	// the lifecycle of the event handler. To avoid unforseen behavior, make sure
	// that the event handler has been properly deregistered from the reactor
	// before removing the last reference (delete on zero).
	virtual ACE_Event_Handler::Reference_Count remove_reference(void);

  // override some task-based members
  virtual int open(void* = NULL); // args

  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,                        // handle
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK); // event mask

  // implement (part of) RPG_Net_IConnection
  virtual void info(ACE_HANDLE&,           // return value: handle
                    ACE_INET_Addr&,        // return value: local SAP
                    ACE_INET_Addr&) const; // return value: remote SAP
  virtual void abort();
  virtual unsigned int id() const;
	// implement RPG_Common_IRefCount
  virtual void increase();
  virtual void decrease();
  using RPG_Common_ReferenceCounterBase::count;
  using RPG_Common_ReferenceCounterBase::wait_zero;
  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 protected:
  typedef RPG_Net_IConnectionManager<ConfigType,
                                     StatisticsContainerType> MANAGER_T;
  RPG_Net_SocketHandlerBase(MANAGER_T*);
  virtual ~RPG_Net_SocketHandlerBase();

  ACE_Reactor_Notification_Strategy myNotificationStrategy;
  MANAGER_T*                        myManager;
  ConfigType                        myUserData;
  bool                              myIsRegistered;

 private:
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_MT_SYNCH> inherited;
	typedef RPG_Common_ReferenceCounterBase inherited2;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase(const RPG_Net_SocketHandlerBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase& operator=(const RPG_Net_SocketHandlerBase&));
};

// include template implementation
#include "rpg_net_sockethandler_base.i"

#endif
