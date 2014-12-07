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

#ifndef RPG_NET_STREAM_ASYNCH_TCPSOCKET_BASE_H
#define RPG_NET_STREAM_ASYNCH_TCPSOCKET_BASE_H

#include "rpg_net_stream_common.h"

#include "ace/config-lite.h"
#include "ace/Event_Handler.h"
#include "ace/Message_Block.h"
#include "ace/Asynch_IO.h"

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketHandlerType>
class RPG_Net_StreamAsynchTCPSocketBase
 : public SocketHandlerType,
   public ACE_Event_Handler
{
 public:
  RPG_Net_StreamAsynchTCPSocketBase ();
  virtual ~RPG_Net_StreamAsynchTCPSocketBase ();

  // override some service methods
  virtual void open (ACE_HANDLE,          // (socket) handle
                     ACE_Message_Block&); // initial data (if any)
  virtual int handle_output(ACE_HANDLE); // (socket) handle
  virtual int handle_close(ACE_HANDLE,        // (socket) handle
                           ACE_Reactor_Mask); // (select) mask
  virtual void act (const void*);

 protected:
  virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result&); // result

//  // *TODO*: (try to) handle short writes gracefully...
//  ACE_Message_Block* myBuffer;
  RPG_Net_StreamSocketConfiguration myConfiguration;
  StreamType                        myStream;

 private:
  typedef SocketHandlerType inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_StreamAsynchTCPSocketBase (const RPG_Net_StreamAsynchTCPSocketBase&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_StreamAsynchTCPSocketBase& operator=(const RPG_Net_StreamAsynchTCPSocketBase&));

  ConfigurationType* myUserData;
};

// include template definition
#include "rpg_net_stream_asynch_tcpsocket_base.inl"

#endif
