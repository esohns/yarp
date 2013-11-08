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

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>

class RPG_Net_Client_SocketHandler
 : public ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_NULL_SYNCH>
{
 public:
  RPG_Net_Client_SocketHandler();
  virtual ~RPG_Net_Client_SocketHandler();

  virtual int open(void*); // args
  virtual int handle_input(ACE_HANDLE);
  virtual int handle_close(ACE_HANDLE,
                           ACE_Reactor_Mask);

 private:
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM,
                          ACE_NULL_SYNCH> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_SocketHandler(const RPG_Net_Client_SocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_SocketHandler& operator=(const RPG_Net_Client_SocketHandler&));
};

#endif
