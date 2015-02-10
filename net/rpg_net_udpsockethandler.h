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

#ifndef RPG_NET_UDPSOCKETHANDLER_H
#define RPG_NET_UDPSOCKETHANDLER_H

//#include "rpg_common_referencecounter_base.h"
//#include "rpg_common_irefcount.h"

//#include "rpg_net_iconnection.h"
//#include "rpg_net_iconnectionmanager.h"

#include "ace/Event_Handler.h"
//#include "ace/SOCK_Stream.h"
#include "ace/Synch.h"
#include "ace/Reactor_Notification_Strategy.h"

class RPG_Net_UDPSocketHandler
 //: public RPG_Common_IRefCount
 : public ACE_Event_Handler
{
 public:
  //// override some event handler methods
  //virtual ACE_Event_Handler::Reference_Count add_reference(void);
  //// *IMPORTANT NOTE*: this API works as long as the reactor doesn't manage
  //// the lifecycle of the event handler. To avoid unforseen behavior, make sure
  //// that the event handler has been properly deregistered from the reactor
  //// before removing the last reference (delete on zero).
  //virtual ACE_Event_Handler::Reference_Count remove_reference(void);

  virtual int open(void* = NULL); // args

  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,                        // handle
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK); // event mask

//  // implement (part of) RPG_Net_IConnection
//  virtual void close();
//  virtual void info(ACE_HANDLE&,           // return value: handle
//                    ACE_INET_Addr&,        // return value: local SAP
//                    ACE_INET_Addr&) const; // return value: remote SAP
//  virtual unsigned int id() const;

 protected:
//  typedef RPG_Net_IConnectionManager<ConfigurationType,
//                                     StatisticsContainerType> MANAGER_T;
//  RPG_Net_UDPSocketHandler(MANAGER_T*);
  RPG_Net_UDPSocketHandler ();
  virtual ~RPG_Net_UDPSocketHandler ();

  ACE_Reactor_Notification_Strategy myNotificationStrategy;
//  MANAGER_T*                        myManager;
  //ConfigurationType                 myUserData;
//  bool                              myIsRegistered;

 private:
  //typedef RPG_Common_IRefCount inherited;
  typedef ACE_Event_Handler inherited;

//  ACE_UNIMPLEMENTED_FUNC(RPG_Net_UDPSocketHandlerBase());
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_UDPSocketHandler (const RPG_Net_UDPSocketHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_UDPSocketHandler& operator=(const RPG_Net_UDPSocketHandler&));

  //// implement RPG_Common_IRefCount
  //virtual void increase ();
  //virtual void decrease ();
  //virtual unsigned int count ();
  //// *NOTE*: this call should block IF the count is > 0 and wait
  //// until the count reaches 0 the next time
  //virtual void wait_zero ();
};

//// include template implementation
//#include "rpg_net_tcpsockethandler_base.inl"

#endif
