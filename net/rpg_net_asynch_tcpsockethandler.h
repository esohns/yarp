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

#ifndef RPG_NET_ASYNCH_TCP_SOCKETHANDLER_T_H
#define RPG_NET_ASYNCH_TCP_SOCKETHANDLER_T_H

#include "rpg_net_exports.h"
#include "rpg_net_sockethandler_base.h"

#include "rpg_common_referencecounter_base.h"

#include "ace/Global_Macros.h"
#include "ace/Asynch_IO.h"
#include "ace/Notification_Strategy.h"
#include "ace/Message_Block.h"
#include "ace/INET_Addr.h"
#include "ace/Event_Handler.h"

class RPG_Net_Export RPG_Net_AsynchTCPSocketHandler
 : public RPG_Net_SocketHandlerBase,
   //public RPG_Common_ReferenceCounterBase,
   public ACE_Service_Handler,
   public ACE_Notification_Strategy
{
 public:
  virtual ~RPG_Net_AsynchTCPSocketHandler();

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

 protected:
  RPG_Net_AsynchTCPSocketHandler();

  // helper method(s)
  void initiate_read_stream();

  virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result&); // result

  ACE_Asynch_Read_Stream  myInputStream;
  ACE_Asynch_Write_Stream myOutputStream;

 private:
  typedef RPG_Net_SocketHandlerBase inherited;
  //typedef RPG_Common_ReferenceCounterBase inherited2;
  typedef ACE_Service_Handler inherited2;
  typedef ACE_Notification_Strategy inherited3;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchTCPSocketHandler(const RPG_Net_AsynchTCPSocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_AsynchTCPSocketHandler& operator=(const RPG_Net_AsynchTCPSocketHandler&));

  // helper method(s)
  ACE_Message_Block* allocateMessage(unsigned int); // requested size

  ACE_INET_Addr           myLocalSAP;
  ACE_INET_Addr           myRemoteSAP;
};

#endif
