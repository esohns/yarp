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

#ifndef RPG_NET_UDPCONNECTION_H
#define RPG_NET_UDPCONNECTION_H

#include "rpg_net_exports.h"
#include "rpg_net_socket_common.h"

#include "ace/Task.h"
#include "ace/INET_Addr.h"

class RPG_Net_Export RPG_Net_UDPConnection
 : public RPG_Net_UDPHandler,
   public ACE_Task<ACE_MT_SYNCH>
{
 public:
  RPG_Net_UDPConnection (const ACE_INET_Addr&); // peer address

//  // implement (part of) RPG_Net_IConnection
//  virtual void ping ();

  // override some task-based members
  virtual int svc (void);
  virtual int open (void* = NULL); // args
  virtual int close (u_long = 0); // args

//  // *NOTE*: enqueue any received data onto our stream for further processing
//   virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: send any enqueued data back to the client...
  virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

 private:
  typedef RPG_Net_UDPHandler inherited;
  typedef ACE_Task<ACE_MT_SYNCH> inherited2;

  // stop worker, if any
  void shutdown ();

  virtual ~RPG_Net_UDPConnection ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_UDPConnection ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_UDPConnection (const RPG_Net_UDPConnection&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_UDPConnection& operator= (const RPG_Net_UDPConnection&));

  ACE_INET_Addr peerAddress_;
};

///////////////////////////////////////////
//
//class RPG_Net_Export RPG_Net_IPMulticastConnection
// : public RPG_Net_IPMulticastHandler,
//   public ACE_Task<ACE_MT_SYNCH>
//{
// public:
//  RPG_Net_IPMulticastConnection (const ACE_INET_Addr&); // peer address
//
//  //  // implement (part of) RPG_Net_IConnection
//  //  virtual void ping();
//
//  // override some task-based members
//  virtual int svc (void);
//  virtual int open (void* = NULL); // args
//  virtual int close (u_long = 0); // args
//
//  //  // *NOTE*: enqueue any received data onto our stream for further processing
//  //   virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
//  // *NOTE*: this is called when:
//  // - handle_xxx() returns -1
//  virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
//                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);
//
// private:
//  typedef RPG_Net_IPMulticastHandler inherited;
//  typedef ACE_Task<ACE_MT_SYNCH> inherited2;
//
//  // stop worker, if any
//  void shutdown ();
//
//  virtual ~RPG_Net_IPMulticastConnection ();
//  ACE_UNIMPLEMENTED_FUNC (RPG_Net_IPMulticastConnection ());
//  ACE_UNIMPLEMENTED_FUNC (RPG_Net_IPMulticastConnection (const RPG_Net_IPMulticastConnection&));
//  ACE_UNIMPLEMENTED_FUNC (RPG_Net_IPMulticastConnection& operator=(const RPG_Net_IPMulticastConnection&));
//};
//
///////////////////////////////////////////
//
//class RPG_Net_Export RPG_Net_IPBroadcastConnection
// : public RPG_Net_IPBroadcastHandler,
//   public ACE_Task<ACE_MT_SYNCH>
//{
// public:
//  RPG_Net_IPBroadcastConnection (const ACE_INET_Addr&); // peer address
//
//  //  // implement (part of) RPG_Net_IConnection
//  //  virtual void ping();
//
//  // override some task-based members
//  virtual int svc (void);
//  virtual int open (void* = NULL); // args
//  virtual int close (u_long = 0); // args
//
//  //  // *NOTE*: enqueue any received data onto our stream for further processing
//  //   virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
//  // *NOTE*: this is called when:
//  // - handle_xxx() returns -1
//  virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
//                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);
//
//  private:
//  typedef RPG_Net_IPBroadcastHandler inherited;
//  typedef ACE_Task<ACE_MT_SYNCH> inherited2;
//
//  // stop worker, if any
//  void shutdown ();
//
//  virtual ~RPG_Net_IPBroadcastConnection ();
//  ACE_UNIMPLEMENTED_FUNC (RPG_Net_IPBroadcastConnection ());
//  ACE_UNIMPLEMENTED_FUNC (RPG_Net_IPBroadcastConnection (const RPG_Net_IPBroadcastConnection&));
//  ACE_UNIMPLEMENTED_FUNC (RPG_Net_IPBroadcastConnection& operator=(const RPG_Net_IPBroadcastConnection&));
//};

#endif
