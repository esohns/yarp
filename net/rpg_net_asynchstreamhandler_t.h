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

#ifndef RPG_NET_ASYNCH_STREAMHANDLER_T_H
#define RPG_NET_ASYNCH_STREAMHANDLER_T_H

#include "rpg_net_asynchsockethandler_t.h"

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor_Notification_Strategy.h>

// forward declaration(s)
class ACE_Message_Block;

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
class RPG_Net_AsynchStreamHandler_T
 : public RPG_Net_AsynchSocketHandler_T<ConfigType,
                                        StatisticsContainerType>,
   public ACE_Event_Handler
{
 public:
  typedef RPG_Net_IConnectionManager<ConfigType,
                                     StatisticsContainerType> MANAGER_T;

  RPG_Net_AsynchStreamHandler_T(MANAGER_T*);
  // *TODO*: remove this stub
  RPG_Net_AsynchStreamHandler_T();
  virtual ~RPG_Net_AsynchStreamHandler_T();

  // implement (part of) RPG_Net_IConnection
  virtual void ping();

  // override some service methods
  virtual void open(ACE_HANDLE,          // (socket) handle
                    ACE_Message_Block&); // initial data (if any)
  virtual int handle_output(ACE_HANDLE); // (socket) handle
  virtual int handle_close(ACE_HANDLE,        // (socket) handle
                           ACE_Reactor_Mask); // (select) mask

  // implement RPG_Common_IStatistic
  // *NOTE*: delegate these to our stream
  virtual bool collect(StatisticsContainerType&) const; // return value: statistic data
  virtual void report() const;

 protected:
  virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result&); // result

  // *NOTE*: (try to) handle short writes gracefully...
  ACE_Message_Block*                myBuffer;
  StreamType                        myStream;
  ACE_Reactor_Notification_Strategy myNotificationStrategy;

 private:
  typedef RPG_Net_AsynchSocketHandler_T<ConfigType,
                                        StatisticsContainerType> inherited;

  //ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchStreamHandler_T());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchStreamHandler_T(const RPG_Net_AsynchStreamHandler_T&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchStreamHandler_T& operator=(const RPG_Net_AsynchStreamHandler_T&));
};

// include template definition
#include "rpg_net_asynchstreamhandler_t.i"

#endif
