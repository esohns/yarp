/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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

#ifndef RPG_NET_CLIENT_SOCKETHANDLER_H
#define RPG_NET_CLIENT_SOCKETHANDLER_H

#include "rpg_net_client_exports.h"
#include "rpg_net_common.h"
#include "rpg_net_sockethandler_base.h"

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>

class RPG_Net_Client_Export RPG_Net_Client_SocketHandler
 : public RPG_Net_SocketHandlerBase<RPG_Net_ConfigPOD,
                                    RPG_Net_RuntimeStatistic>
{
 protected:
  // convenient types
  typedef RPG_Net_IConnectionManager<RPG_Net_ConfigPOD,
                                     RPG_Net_RuntimeStatistic> MANAGER_t;

 public:
  RPG_Net_Client_SocketHandler(MANAGER_t*);
  // *TODO*: make this private !!!
  RPG_Net_Client_SocketHandler();
  virtual ~RPG_Net_Client_SocketHandler();

  virtual int open(void* = NULL); // args
  virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE);
  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
                           ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

  // implement RPG_Common_IStatistic
  // *NOTE*: delegate these to our stream
  virtual bool collect(RPG_Net_RuntimeStatistic&) const; // return value: statistic data
  virtual void report() const;

 private:
  typedef RPG_Net_SocketHandlerBase<RPG_Net_ConfigPOD,
                                    RPG_Net_RuntimeStatistic> inherited;

  // safety measures
  //ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_SocketHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_SocketHandler(const RPG_Net_Client_SocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_SocketHandler& operator=(const RPG_Net_Client_SocketHandler&));
};

#endif
