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

#ifndef RPG_NET_REMOTE_COMM_H
#define RPG_NET_REMOTE_COMM_H

#include "ace/Global_Macros.h"

class RPG_Net_Remote_Comm
{
 public:
  // define different types of messages
  enum MessageType : ACE_UINT32
  {
    RPG_NET_PING = 0,
    RPG_NET_PONG = 1,
  };

  // define a common message header
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
  struct MessageHeader
  {
    // *NOTE*: messageLength is (currently) defined as:
    // *PORTABILITY*: total message length - sizeof(unsigned int) !
    ACE_UINT32  messageLength;
    MessageType messageType;
#ifdef __GNUC__
  } __attribute__ ((__packed__));
#else
  };
#endif

  // -----------**** protocol messages ****-----------
  struct PingMessage
  {
    MessageHeader messageHeader;
    ACE_UINT32    counter;
#ifdef __GNUC__
  } __attribute__ ((__packed__));
#else
  };
#endif
  struct PongMessage
  {
    MessageHeader messageHeader;
#ifdef __GNUC__
  } __attribute__ ((__packed__));
#else
  };
#endif
  // -----------**** protocol messages END ****-----------
#ifdef _MSC_VER
#pragma pack(pop)
#endif

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Remote_Comm ());
  ACE_UNIMPLEMENTED_FUNC (virtual ~RPG_Net_Remote_Comm ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Remote_Comm (const RPG_Net_Remote_Comm&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Remote_Comm& operator= (const RPG_Net_Remote_Comm&));
};

// convenient typedefs
typedef RPG_Net_Remote_Comm::MessageType RPG_Net_MessageType;
typedef RPG_Net_Remote_Comm::MessageHeader RPG_Net_MessageHeader;

#endif
