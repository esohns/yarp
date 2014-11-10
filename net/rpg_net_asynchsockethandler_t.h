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

#ifndef RPG_NET_ASYNCHSOCKETHANDLER_T_H
#define RPG_NET_ASYNCHSOCKETHANDLER_T_H

//#include "rpg_net_iconnection.h"
//#include "rpg_net_iconnectionmanager.h"

#include "rpg_common_referencecounter_base.h"

#include "ace/Global_Macros.h"
#include "ace/Asynch_IO.h"
#include "ace/Notification_Strategy.h"
#include "ace/Message_Block.h"
#include "ace/INET_Addr.h"
#include "ace/Event_Handler.h"

template <typename ConfigType,
          typename StatisticsContainerType>
class RPG_Net_AsynchSocketHandler
 : public ACE_Service_Handler,
   public ACE_Notification_Strategy,
   public RPG_Common_ReferenceCounterBase//,
//   public RPG_Net_IConnection<StatisticsContainerType>
{
 public:
  virtual ~RPG_Net_AsynchSocketHandler();

  //check if registration with the connection manager was OK...
  virtual void open(ACE_HANDLE,          // (socket) handle
                    ACE_Message_Block&); // initial data (if any)
  virtual void addresses(const ACE_INET_Addr&,  // remote address
                         const ACE_INET_Addr&); // local address
  virtual int handle_output(ACE_HANDLE) = 0; // (socket) handle
  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,                        // handle
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK); // event mask

  // implement ACE_Notification_Strategy
  virtual int notify(void);
  virtual int notify(ACE_Event_Handler*, // event handler handle
                     ACE_Reactor_Mask);  // mask

//  // implement (part of) RPG_Net_IConnection
//  virtual void info(ACE_HANDLE&,           // return value: handle
//                    ACE_INET_Addr&,        // return value: local SAP
//                    ACE_INET_Addr&) const; // return value: remote SAP
//  virtual void abort();
//  virtual unsigned int id() const;
//  // implement RPG_Common_IRefCount
//  using RPG_Common_ReferenceCounterBase::increase;
//  using RPG_Common_ReferenceCounterBase::decrease;
//  using RPG_Common_ReferenceCounterBase::count;
//  using RPG_Common_ReferenceCounterBase::wait_zero;
//  // implement RPG_Common_IDumpState
//  virtual void dump_state() const;

 protected:
//  typedef RPG_Net_IConnectionManager<ConfigType,
//                                     StatisticsContainerType> MANAGER_T;
//  RPG_Net_AsynchSocketHandler(MANAGER_T*); // manager handle
  RPG_Net_AsynchSocketHandler();

  // helper method(s)
  void initiate_read_stream();

  virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result&); // result

  ConfigType              myUserData;
  ACE_Asynch_Read_Stream  myInputStream;
  ACE_Asynch_Write_Stream myOutputStream;
//  MANAGER_T*              myManager;
//  bool                    myIsRegistered;

 private:
  typedef ACE_Service_Handler inherited;
  typedef RPG_Common_ReferenceCounterBase inherited2;

//  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchSocketHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchSocketHandler(const RPG_Net_AsynchSocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchSocketHandler& operator=(const RPG_Net_AsynchSocketHandler&));

  // helper method(s)
  ACE_Message_Block* allocateMessage(const unsigned int&); // requested size

  ACE_INET_Addr           myLocalSAP;
  ACE_INET_Addr           myRemoteSAP;
};

// include template definition
#include "rpg_net_asynchsockethandler_t.inl"

#endif
