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
#include "stdafx.h"

#include "rpg_net_client_sockethandler.h"

#include "rpg_net_remote_comm.h"
#include "rpg_net_common_tools.h"

#include "rpg_common_macros.h"

#include <ace/Reactor.h>

#include <string>
#include <iostream>

RPG_Net_Client_SocketHandler::RPG_Net_Client_SocketHandler()
 : inherited(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::RPG_Net_Client_SocketHandler"));

  // *TODO*: clean this up !!!
  ACE_ASSERT(false);
}

RPG_Net_Client_SocketHandler::RPG_Net_Client_SocketHandler(MANAGER_t* manager_in)
 : inherited(manager_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::RPG_Net_Client_SocketHandler"));

}

RPG_Net_Client_SocketHandler::~RPG_Net_Client_SocketHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::~RPG_Net_Client_SocketHandler"));

}

int
RPG_Net_Client_SocketHandler::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::open"));

  // call baseclass...
  // *NOTE*: this registers this instance with the reactor (READ_MASK)
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_SocketHandlerBase::open(): \"%m\", aborting\n")));

    return -1;
  } // end IF
//  // register with reactor...
//  if (reactor()->register_handler(this,
//                                  READ_MASK) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_Reactor::register_handler(): \"%m\", aborting\n")));

//    return -1;
//  } // end IF

//  // debug info
//  ACE_INET_Addr localAddress;
//  if (peer().get_local_addr(localAddress) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_SOCK_Stream::get_local_addr(): \"%m\", aborting\n")));

//    return -1;
//  } // end IF
//  ACE_INET_Addr remoteAddress;
//  if (peer().get_remote_addr(remoteAddress) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%m\", aborting\n")));

//    return -1;
//  } // end IF
//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("connected (handle: %d) \"%s\" | \"%s:%u\" <--> \"%s:%u\"\n"),
//             peer().get_handle(),
//             localAddress.get_host_name(),
//             localAddress.get_host_addr(),
//             localAddress.get_port_number(),
//             remoteAddress.get_host_name(),
//             remoteAddress.get_port_number()));

  return 0;
}

int
RPG_Net_Client_SocketHandler::handle_input(ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::handle_input"));

  ACE_UNUSED_ARG(handle_in);

  // step0: init buffer
  RPG_Net_Remote_Comm::PingMessage data;
  ACE_OS::memset(&data,
                 0,
                 sizeof(RPG_Net_Remote_Comm::PingMessage));

  // step1: read data
  size_t bytes_received = 0;
  // *TODO*: do blocking IO until further notice...
  if (peer().recv_n(static_cast<void*>(&data),                // buffer
                    sizeof(RPG_Net_Remote_Comm::PingMessage), // length
                    NULL,                                     // timeout --> block
                    &bytes_received) == -1)                   // number of recieved bytes
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Stream::recv_n(): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    // --> reactor will invoke handle_close() --> close the socket
    return -1;
  } // end IF

  switch (bytes_received)
  {
    // *NOTE*: peer MAY only close this socket for system shutdown/application restart !!!
    case 0:
    {
      // *** peer has closed the socket ***

      // --> reactor will invoke handle_close() --> close the socket
      return -1;
    }
    case -1:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::recv_n(): \"%m\", aborting\n")));

      // --> reactor will invoke handle_close() --> close the socket
      return -1;
    }
    default:
    {
      // --> socket is probably non-blocking...
      if (bytes_received != sizeof(RPG_Net_Remote_Comm::PingMessage))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("only managed to read %u/%u bytes, aborting\n"),
                   bytes_received,
                   sizeof(RPG_Net_Remote_Comm::PingMessage)));

        // --> reactor will invoke handle_close() --> close the socket
        return -1;
      }

      break;
    }
  } // end SWITCH

//   // step2: print data
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("[%u]: received %u bytes [length: %u; type: \"%s\"; counter: %u]\n"),
//              handle_in,
//              bytes_received,
//              data.messageHeader.messageLength,
//              RPG_Net_Common_Tools::messageType2String(data.messageHeader.messageType).c_str(),
//              data.counter));

  switch (data.messageHeader.messageType)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      // reply with a "PONG"
      RPG_Net_Remote_Comm::PongMessage reply;
      ACE_OS::memset(&reply,
                     0,
                     sizeof(RPG_Net_Remote_Comm::PongMessage));
      reply.messageHeader.messageLength = sizeof(RPG_Net_Remote_Comm::PongMessage) - sizeof(unsigned long);
      reply.messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PONG;

      // step2: send it over the net...
      size_t bytes_sent = peer().send_n(static_cast<const void*>(&reply),         // buffer
                                        sizeof(RPG_Net_Remote_Comm::PongMessage), // length
                                        NULL,                                     // timeout --> block
                                        &bytes_sent);                             // number of sent bytes
      // *NOTE*: we'll ALSO get here when the client has closed the socket
      // in a well-behaved way... --> don't treat this as an error !
      switch (bytes_sent)
      {
        case -1:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_SOCK_Stream::send_n(): \"%m\", aborting\n")));

          // --> reactor will invoke handle_close() --> close the socket
          return -1;
        }
        case 0:
        default:
        {
          if (bytes_sent == sizeof(RPG_Net_Remote_Comm::PongMessage))
          {
            // *** GOOD CASE ***

            // debug info
            std::cerr << '.';

            break;
          } // end IF

          // --> socket is probably non-blocking...
          // *TODO*: support/use non-blocking sockets !
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("only managed to send %u/%u bytes, aborting\n"),
                     bytes_sent,
                     sizeof(RPG_Net_Remote_Comm::PongMessage)));

          // --> reactor will invoke handle_close() --> close the socket
          return -1;
        }
      } // end SWITCH

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("protocol error, aborting\n")));

      // --> reactor will invoke handle_close() --> close the socket
      return -1;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Net_Client_SocketHandler::handle_close(ACE_HANDLE handle_in,
                                           ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::handle_close"));

  // *NOTE*: this is called when:
  // - the server closes the socket
  // - client closed() handler itself (failure to connect ?)

  // debug info
  if (handle_in != ACE_INVALID_HANDLE)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("lost connection %d to server...\n"),
               handle_in));

  return inherited::handle_close(handle_in,
                                 mask_in);
}

bool
RPG_Net_Client_SocketHandler::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::collect"));

  // *TODO*
  ACE_ASSERT(false);

  return false;
}

void
RPG_Net_Client_SocketHandler::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_SocketHandler::report"));

  // *TODO*
  ACE_ASSERT(false);
}
