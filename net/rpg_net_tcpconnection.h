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

#ifndef RPG_NET_TCPCONNECTION_H
#define RPG_NET_TCPCONNECTION_H

#include "rpg_net_exports.h"
#include "rpg_net_socket_common.h"
#include "rpg_net_socketconnection_base.h"
#include "rpg_net_transportlayer_tcp.h"

#include "ace/Event_Handler.h"

class RPG_Net_Export RPG_Net_TCPConnection
 : public RPG_Net_SocketConnectionBase<RPG_Net_TCPHandler,
                                       RPG_Net_TransportLayer_TCP,
                                       RPG_Net_ConfigPOD,
                                       RPG_Net_RuntimeStatistic>
{
 public:
  RPG_Net_TCPConnection();

  // implement (part of) RPG_Net_ITransportLayer
  virtual void info (ACE_HANDLE&,           // return value: handle
                     ACE_INET_Addr&,        // return value: local SAP
                     ACE_INET_Addr&) const; // return value: remote SAP

  //// override some task-based members
  //virtual int open(void* = NULL); // args
  //virtual int close(u_long = 0); // args

//  // *NOTE*: enqueue any received data onto our stream for further processing
//   virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

 private:
  typedef RPG_Net_SocketConnectionBase<RPG_Net_TCPHandler,
                                       RPG_Net_TransportLayer_TCP,
                                       RPG_Net_ConfigPOD,
                                       RPG_Net_RuntimeStatistic> inherited;

  //// override some task-based members
  //virtual int svc(void);

  //// stop worker, if any
  //void shutdown();

  virtual ~RPG_Net_TCPConnection();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TCPConnection(const RPG_Net_TCPConnection&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_TCPConnection& operator=(const RPG_Net_TCPConnection&));
};

/////////////////////////////////////////

class RPG_Net_Export RPG_Net_AsynchTCPConnection
 : public RPG_Net_SocketConnectionBase<RPG_Net_AsynchTCPHandler,
                                       RPG_Net_TransportLayer_TCP,
                                       RPG_Net_ConfigPOD,
                                       RPG_Net_RuntimeStatistic>
{
 public:
  RPG_Net_AsynchTCPConnection ();

  //// override some task-based members
  //virtual int open(void* = NULL); // args
  //virtual int close(u_long = 0); // args

  //  // *NOTE*: enqueue any received data onto our stream for further processing
  //   virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

 private:
  typedef RPG_Net_SocketConnectionBase<RPG_Net_AsynchTCPHandler,
                                       RPG_Net_TransportLayer_TCP,
                                       RPG_Net_ConfigPOD,
                                       RPG_Net_RuntimeStatistic> inherited;

  //// override some task-based members
  //virtual int svc(void);

  //// stop worker, if any
  //void shutdown();

  virtual ~RPG_Net_AsynchTCPConnection ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_AsynchTCPConnection (const RPG_Net_AsynchTCPConnection&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_AsynchTCPConnection& operator=(const RPG_Net_AsynchTCPConnection&));
};

#endif
