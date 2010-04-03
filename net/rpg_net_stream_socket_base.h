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

#ifndef RPG_NET_STREAMSOCKET_BASE_H
#define RPG_NET_STREAMSOCKET_BASE_H

#include "rpg_net_sockethandler.h"

#include <ace/Global_Macros.h>

// forward declarations
class ACE_Message_Block;

template <typename StreamType>
class RPG_Net_StreamSocketBase
 : public RPG_Net_SocketHandler
{
 public:
  RPG_Net_StreamSocketBase();
  virtual ~RPG_Net_StreamSocketBase(); // we'll self-destruct !

  virtual int open(void*); // args

  // *NOTE*: this will work only for single-threaded reactors where
  // handle_input and handle_timeout are always invoked SEQUENTIALLY (and NEVER
  // "SIMULTANEOUSLY")

  // *NOTE*: enqueue any received data onto our stream for further processing
  virtual int handle_input(ACE_HANDLE); // handle

  // *NOTE*: this is called when:
  // - handle_xxx() returns -1
  virtual int handle_close(ACE_HANDLE,
                           ACE_Reactor_Mask);

 private:
  typedef RPG_Net_SocketHandler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamSocketBase(const RPG_Net_StreamSocketBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamSocketBase& operator=(const RPG_Net_StreamSocketBase&));

  // processing stream
  StreamType    myStream;

  unsigned long      myCurrentMessageLength;
  size_t             myReceivedMessageBytes;
  ACE_Message_Block* myCurrentBuffer;
  ACE_Message_Block* myCurrentMessage;
};

#endif
