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

#ifndef RPG_NET_SOCKETCONNECTION_BASE_H
#define RPG_NET_SOCKETCONNECTION_BASE_H

#include "rpg_net_connection_base.h"

#include "ace/Event_Handler.h"

template <typename SocketHandlerType,
          typename TransportLayerType,
          typename ConfigurationType,
          typename StatisticsContainerType>
class RPG_Net_SocketConnectionBase
  : public SocketHandlerType,
    public TransportLayerType,
    public RPG_Net_ConnectionBase<ConfigurationType,
                                  StatisticsContainerType>
{
 public:
  virtual ~RPG_Net_SocketConnectionBase();

  // implement (part of) RPG_Net_ITransportLayer
  virtual void ping (); // ping the peer !
  virtual unsigned int id () const;
  virtual void dump_state () const;

  // implement RPG_Common_IStatistic
  // *NOTE*: delegate these to the stream
  virtual bool collect (StatisticsContainerType&) const; // return value: statistic data
  virtual void report () const;

  //// override some task-based members
  //virtual int open(void* = NULL); // args
  //virtual int close(u_long = 0); // args

//  // *NOTE*: enqueue any received data onto our stream for further processing
//   virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

 protected:
  RPG_Net_SocketConnectionBase();

 private:
  typedef SocketHandlerType inherited;
  typedef TransportLayerType inherited2;
  typedef RPG_Net_ConnectionBase<ConfigurationType,
                                 StatisticsContainerType> inherited3;

  //// override some task-based members
  //virtual int svc(void);

  //// stop worker, if any
  //void shutdown();

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketConnectionBase(const RPG_Net_SocketConnectionBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketConnectionBase& operator=(const RPG_Net_SocketConnectionBase&));
};

#include "rpg_net_socketconnection_base.inl"

#endif
