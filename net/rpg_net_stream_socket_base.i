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

#include "rpg_net_common.h"
#include "rpg_net_defines.h"
#include "rpg_net_remote_comm.h"

#include <ace/Message_Block.h>

template <typename StreamType>
RPG_Net_StreamSocketBase<StreamType>::RPG_Net_StreamSocketBase()
 : myCurrentMessageLength(0),
   myReceivedMessageBytes(0),
   myCurrentBuffer(NULL),
   myCurrentMessage(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::RPG_Net_StreamSocketBase"));

}

RPG_Net_StreamSocketBase<StreamType>::~RPG_Net_StreamSocketBase()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::~RPG_Net_StreamSocketBase"));

}

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::open(void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::open"));

  // sanity check
  ACE_ASSERT(arg_in);

  // init/start data processing stream
  RPG_Net_StreamConfigPOD* stream_config = ACE_static_cast(RPG_Net_StreamConfigPOD*,
                                                           arg_in);
  myStream.init(stream_config);
  myStream.start();

  // start client ping timer and register us at the reactor
  // *NOTE*: this is done by the base class !
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    // *NOTE*: this might have happened because there are too many
    // open connections... --> not an error !
    if (ACE_OS::last_error())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to inherited::open(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));
    } // end IF

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  return 0;
}

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::handle_input(ACE_HANDLE handle_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_input"));

  ACE_UNUSED_ARG(handle_in);

  size_t bytes_received = 0;
  if (myCurrentMessageLength == 0)
  {
    // don't know anything --> start reading a new message from the socket
    if (myCurrentBuffer == NULL)
    {
      // sanity check
      ACE_ASSERT(myCurrentMessage == NULL);

      // *TODO*: use an allocator to do this !
      try
      {
        myCurrentBuffer = new ACE_Message_Block(RPG_NET_DEF_NETWORK_BUFFER_SIZE,    // size
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

        // reactor will invoke handle_close() --> we commit suicide
        return -1;
      }
      if (!myCurrentBuffer)
      {
        ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("unable to allocate memory, returning\n")));

        // clean up
        cancelTimer();

        // reactor will invoke handle_close() --> we commit suicide
        return -1;
      } // end IF
    } // end IF

    // remember this buffer as our head...
    myCurrentMessage = myCurrentBuffer;

    // read some data from the socket...
    bytes_received = peer_.recv(myCurrentBuffer->wr_ptr(),
                                myCurrentBuffer->size());
    switch (bytes_received)
    {
      case -1:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%s\", returning\n"),
                   ACE_OS::strerror(errno)));

        // clean up
        cancelTimer();
        delete myCurrentBuffer;
        myCurrentBuffer = NULL;
        myCurrentMessage = NULL;

        // reactor will invoke handle_close() --> we commit suicide
        return -1;
      }
      // *** GOOD CASES ***
      case 0:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("socket was closed by the peer...\n")));

        // clean up
        cancelTimer();
        delete myCurrentBuffer;
        myCurrentBuffer = NULL;
        myCurrentMessage = NULL;

        // reactor will invoke handle_close() --> we commit suicide
        return -1;
      }
      default:
      {
        // adjust write pointer
        myCurrentBuffer->wr_ptr(bytes_received);

        // ... and the running message counter
        myReceivedMessageBytes += bytes_received;

        // debug info
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("received %u bytes...\n"),
                   bytes_received));

        // OK, perhaps we can start interpreting the message...
        break;
      }
    } // end SWITCH

    // check if we received the full message header yet...
    if (myCurrentMessage->length() < sizeof(RPG_Net_Remote_Comm::MessageHeader))
    {
      // wait for more data...
      return 0;
    } // end IF

    // OK, we can start interpreting this message...
    RPG_Net_Remote_Comm::MessageHeader* message_header = ACE_static_cast(RPG_Net_Remote_Comm::MessageHeader*,
                                                                         myCurrentMessage->rd_ptr());
    // *TODO*: *PORTABILITY*: handle endianness && type issues !
    myCurrentMessageLength = message_header->messageLength + sizeof(unsigned long);
  } // end IF

  // check if there is any space in the current buffer
  if (myCurrentBuffer->size() == 0)
  {
    // *TODO*: use an allocator to do this !
    ACE_Message_Block* buffer = NULL;
    try
    {
      buffer = new ACE_Message_Block(RPG_NET_DEF_NETWORK_BUFFER_SIZE,    // size
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

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    }
    if (!buffer)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("unable to allocate memory, returning\n")));

      // clean up
      cancelTimer();

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    } // end IF

    // string this new buffer onto our chain...
    myCurrentBuffer->cont(buffer);

    // ... and remember our current buffer...
    myCurrentBuffer = buffer;
  } // end IF

  // ... read some more data into the current buffer
  bytes_received = peer_.recv(myCurrentBuffer->wr_ptr(),
                              myCurrentBuffer->size());
  switch (bytes_received)
  {
    case -1:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%s\", returning\n"),
                 ACE_OS::strerror(errno)));

      // clean up
      cancelTimer();
      delete myCurrentMessage;
      myCurrentBuffer = NULL;
      myCurrentMessage = NULL;

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("socket was closed by the peer...\n")));

      // clean up
      cancelTimer();
      delete myCurrentMessage;
      myCurrentBuffer = NULL;
      myCurrentMessage = NULL;

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    }
    default:
    {
      // adjust write pointer
      myCurrentBuffer->wr_ptr(bytes_received);

      // ... and the running message counter
      myReceivedMessageBytes += bytes_received;

      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("received %u bytes [current: %u, total: %u]...\n"),
                 bytes_received,
                 myReceivedMessageBytes,
                 myCurrentMessageLength));

      // OK, perhaps we can start processing the message...
      break;
    }
  } // end SWITCH

  // check if we received the whole message yet...
  if (myCurrentMessage->total_length() < myCurrentMessageLength))
  {
    // wait for more data...
    return 0;
  } // end IF

  // OK, we have all of it !

  // check if we have received (part of) the next message...
  if (myCurrentMessage->total_length() > myCurrentMessageLength)
  {
    // start a new message and copy the overlapping data

  } // end IF

  // push the finished message onto our stream for processing
  if (myStream.put(myCurrentMessage) == -1)
  {

  } // end IF

  // ... start reading the next message ASAP
  myCurrentMessageLength = 0;
  myReceivedMessageBytes = 0;
  myCurrentBuffer = NULL;
  myCurrentMessage = NULL;

  return 0;
}

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::handle_timeout(const ACE_Time_Value& tv_in,
                                                     const void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_timeout"));

  ACE_UNUSED_ARG(tv_in);
  ACE_UNUSED_ARG(arg_in);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("timer (ID: %d) expired...sending ping\n"),
             myTimerID));

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

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::handle_close(ACE_HANDLE handle_in,
                                                   ACE_Reactor_Mask mask_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_close"));

  // clean up timer, if necessary (i.e. returned -1 from handle_timeout)
  // *NOTE*: this works only for single-threaded reactors (see above) !!!
  if (myTimerID != -1)
    cancelTimer();

  // *NOTE*: base class will commit suicide properly --> cleans us up
  return inherited::handle_close(handle_in,
                                 mask_in);
}
