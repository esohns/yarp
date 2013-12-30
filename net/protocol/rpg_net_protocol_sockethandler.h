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

#ifndef RPG_NET_PROTOCOL_SOCKETHANDLER_H
#define RPG_NET_PROTOCOL_SOCKETHANDLER_H

#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_stream.h"

#include <rpg_net_stream_socket_base.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class ACE_Message_Block;

class RPG_Protocol_Export RPG_Net_Protocol_SocketHandler
 : public RPG_Net_StreamSocketBase<RPG_Net_Protocol_ConfigPOD,
                                   RPG_Net_Protocol_RuntimeStatistic,
                                   RPG_Net_Protocol_Stream>
{
 public:
  RPG_Net_Protocol_SocketHandler();
  virtual ~RPG_Net_Protocol_SocketHandler();

  // override some task-based members
//   virtual int svc(void);
  virtual int open(void* = NULL); // args
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

 private:
  typedef RPG_Net_StreamSocketBase<RPG_Net_Protocol_ConfigPOD,
                                   RPG_Net_Protocol_RuntimeStatistic,
                                   RPG_Net_Protocol_Stream> inherited;

  // implement (part of) RPG_Net_IConnection
  // *WARNING*: this is just a stub...
  virtual void ping();

  // stop worker
  void shutdown();

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_SocketHandler(const RPG_Net_Protocol_SocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_SocketHandler& operator=(const RPG_Net_Protocol_SocketHandler&));
};

#endif
