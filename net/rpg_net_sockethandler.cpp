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

#include "rpg_net_sockethandler.h"

#include "rpg_net_defines.h"
#include "rpg_net_remote_comm.h"
#include "rpg_net_common_tools.h"

// init statics
ACE_Atomic_Op<ACE_Thread_Mutex,
              unsigned long> RPG_Net_SocketHandler::myCurrentID = 1;

RPG_Net_SocketHandler::RPG_Net_SocketHandler()
 : myTimerID(-1)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler::RPG_Net_SocketHandler"));

}

RPG_Net_SocketHandler::~RPG_Net_SocketHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler::~RPG_Net_SocketHandler"));

  // clean up timer if necessary
  if (myTimerID != -1)
    cancelTimer();
}

int
RPG_Net_SocketHandler::open(void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler::open"));

  // sanity check
  // *NOTE*: we should have registered/initialized by now...
  // --> make sure this was successful before we proceed
  if (!inherited::isRegistered())
  {
    // too many connections...
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to register connection (ID: %u), aborting\n"),
//                getID()));

    // --> reactor will invoke handle_close() --> close the socket
    return -1;
  } // end IF

  // create client ping timer... and register it with the reactor !
  ACE_Time_Value interval(RPG_NET_DEF_PING_INTERVAL, 0);
  myTimerID = reactor()->schedule_timer(this,
                                        NULL,
                                        interval,
                                        interval);
  if (myTimerID == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::schedule_timer(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    // --> reactor will invoke handle_close() --> close the socket
    return -1;
  } // end IF

  // *NOTE*: we're registered with the reactor (TIMER_MASK) at this point...

  // register reading data with reactor...
  // *NOTE*: this is done by the base class !
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    // *NOTE*: this MAY have happened because there are too many
    // open connections... ---> not an error ?
    if (ACE_OS::last_error())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to inherited::open(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));
    } // end IF

    // clean up
    cancelTimer();

    // reactor will invoke handle_close() --> close the socket
    return -1;
  } // end IF

  return 0;
}

int
RPG_Net_SocketHandler::handle_input(ACE_HANDLE handle_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler::handle_input"));

  // *NOTE*: currently, we just ignore all incoming data...
  ACE_UNUSED_ARG(handle_in);

  ACE_Message_Block* chunk = NULL;
  try
  {
    chunk = new ACE_Message_Block(RPG_NET_DEF_NETWORK_BUFFER_SIZE,    // size
                                  ACE_Message_Block::MB_STOP,         // type
                                  NULL,                               // continuation
                                  NULL,                               // data
                                  NULL,                               // buffer allocator
                                  NULL,                               // locking strategy
                                  ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
                                  ACE_Time_Value::zero,               // execution time
                                  ACE_Time_Value::max_time,           // deadline time
                                  NULL,                               // data allocator
                                  NULL);                              // message allocator
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in new, returning\n")));

    // clean up
    cancelTimer();

    // reactor will invoke handle_close() --> close the socket
    return -1;
  }
  if (!chunk)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("unable to allocate memory, returning\n")));

    // clean up
    cancelTimer();

    // reactor will invoke handle_close() --> close the socket
    return -1;
  } // end IF

  // read some data from the socket...
  size_t bytes_received = peer_.recv(chunk->wr_ptr(),
                                     chunk->size());
  switch (bytes_received)
  {
    case -1:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%s\", returning\n"),
                 ACE_OS::strerror(errno)));

      // clean up
      cancelTimer();
      delete chunk;
      chunk = NULL;

      // reactor will invoke handle_close() --> close the socket
      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("socket was closed by the peer...\n")));

      // clean up
      cancelTimer();
      delete chunk;
      chunk = NULL;

      // reactor will invoke handle_close() --> close the socket
      return -1;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("received: %u bytes...\n"),
                 bytes_received));

      // clean up
      delete chunk;
      chunk = NULL;

      break;
    }
  } // end SWITCH

  // clean up
  delete chunk;
  chunk = NULL;

  return 0;
}

int
RPG_Net_SocketHandler::handle_timeout(const ACE_Time_Value& tv_in,
                                      const void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler::handle_timeout"));

  ACE_UNUSED_ARG(tv_in);
  ACE_UNUSED_ARG(arg_in);

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("timer (ID: %d) expired...sending ping\n"),
//              myTimerID));

  // step1: init ping data
  // *TODO*: clean this up and handle endianness consistently !
  RPG_Net_Remote_Comm::RuntimePing data;
  data.messageHeader.messageLength = (sizeof(RPG_Net_Remote_Comm::RuntimePing) -
                                      sizeof(unsigned long));
  data.messageHeader.messageType = RPG_Net_Remote_Comm::RPG_NET_PING;
   // *WARNING*: prefix increment leads to corruption !
  data.counter = myCurrentID++;

  // step2: send it over the net...
  size_t bytes_sent = peer().send_n(ACE_static_cast(const void*,
                                                    &data),                   // buffer
                                    sizeof(RPG_Net_Remote_Comm::RuntimePing), // length
                                    NULL,                                     // timeout --> block
                                    &bytes_sent);                             // number of sent bytes
  // *NOTE*: we'll ALSO get here when the client has closed the socket
  // in a well-behaved way... --> don't treat this as an error !
  switch (bytes_sent)
  {
    case -1:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::send_n(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

      // reactor will invoke handle_close() --> close the socket
      return -1;
    }
    case 0:
    default:
    {
      if (bytes_sent == sizeof(RPG_Net_Remote_Comm::RuntimePing))
      {
        // *** GOOD CASE ***
        break;
      } // end IF

      // --> socket is probably non-blocking...
      // *TODO*: support/use non-blocking sockets !
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("only managed to send %u/%u bytes, aborting\n"),
                 bytes_sent,
                 sizeof(RPG_Net_Remote_Comm::RuntimePing)));

      // reactor will invoke handle_close() --> close the socket
      return -1;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Net_SocketHandler::handle_close(ACE_HANDLE handle_in,
                                    ACE_Reactor_Mask mask_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler::handle_close"));

  // clean up timer, if necessary (i.e. returned -1 from handle_timeout)
  // *NOTE*: this works only for single-threaded reactors (see above) !!!
  if (myTimerID != -1)
    cancelTimer();

  // *NOTE*: base class will commit suicide properly --> cleans us up
  return inherited::handle_close(handle_in,
                                 mask_in);
}
