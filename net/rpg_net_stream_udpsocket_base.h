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

#ifndef RPG_NET_STREAM_UDPSOCKET_BASE_H
#define RPG_NET_STREAM_UDPSOCKET_BASE_H

#include "rpg_net_stream_common.h"
#include "rpg_net_sockethandler_base.h"
//#include "rpg_net_iconnectionmanager.h"

#include "ace/config-lite.h"
#include "ace/INET_Addr.h"
#include "ace/Event_Handler.h"
#include "ace/Message_Block.h"
#include "ace/Synch.h"

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
class RPG_Net_StreamUDPSocketBase
 : public SocketType,
   public SocketHandlerType
{
 public:
  int open (const ConfigurationType&, // configuration
            const ACE_INET_Addr&);    // peer address
  //virtual int close(u_long = 0); // args

  // *NOTE*: enqueue any received data onto our stream for further processing
  virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

  // implement RPG_Common_IStatistic
  // *NOTE*: delegate these to our stream
  virtual bool collect(StatisticsContainerType&) const; // return value: statistic data
  virtual void report() const;

 protected:
//  typedef RPG_Net_IConnectionManager<ConfigurationType,
//                                     StatisticsContainerType> MANAGER_T;
//  RPG_Net_StreamUDPSocketBase(MANAGER_T*); // connection manager handle
  RPG_Net_StreamUDPSocketBase();
  virtual ~RPG_Net_StreamUDPSocketBase();

  RPG_Net_StreamSocketConfiguration myConfiguration;
  StreamType                        myStream;
  ACE_Message_Block*                myCurrentReadBuffer;
  ACE_Thread_Mutex                  mySendLock;
  ACE_Message_Block*                myCurrentWriteBuffer;
  // *IMPORTANT NOTE*: in a threaded environment, workers MAY
  // dispatch the reactor notification queue concurrently (most notably,
  // ACE_TP_Reactor) --> enforce proper serialization
  bool                              mySerializeOutput;

  // helper method(s)
  ACE_Message_Block* allocateMessage(const unsigned int&); // requested size

 private:
  typedef SocketType inherited;
  typedef SocketHandlerType inherited2;

//  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamSocketBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamUDPSocketBase(const RPG_Net_StreamUDPSocketBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamUDPSocketBase& operator=(const RPG_Net_StreamUDPSocketBase&));
};

// include template implementation
#include "rpg_net_stream_udpsocket_base.inl"

#endif
