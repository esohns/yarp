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
#include "rpg_net_common_tools.h"
#include "rpg_net_message.h"

#include <stream_iallocator.h>

#include <ace/Message_Block.h>

template <typename ConfigType,
          typename StreamType>
RPG_Net_StreamSocketBase<ConfigType, StreamType>::RPG_Net_StreamSocketBase()
 : myCurrentWriteBuffer(NULL),
   myAllocator(NULL),
   myDefaultBufferSize(RPG_NET_DEF_NETWORK_BUFFER_SIZE),
   myCurrentReadBuffer(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::RPG_Net_StreamSocketBase"));

}

template <typename ConfigType,
          typename StreamType>
RPG_Net_StreamSocketBase<ConfigType, StreamType>::~RPG_Net_StreamSocketBase()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::~RPG_Net_StreamSocketBase"));

  // wait for all workers within the stream (if any)
  myStream.waitForCompletion();
}

template <typename ConfigType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType, StreamType>::open(void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::open"));

  // sanity check
  ACE_ASSERT(arg_in);
  // *NOTE*: we should have initialized by now...
  // --> make sure this was successful before we proceed
  if (!inherited::myIsInitialized)
  {
    // (most probably) too many connections...
    ACE_OS::last_error(EBUSY);

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

  // step0: retrieve config, init ourselves
  myAllocator = inherited::myUserData.messageAllocator;
  // sanity check
  ACE_ASSERT(myAllocator);

  myDefaultBufferSize = inherited::myUserData.defaultBufferSize;
  // sanity check
  ACE_ASSERT(myDefaultBufferSize);

  // step1: init/start data processing stream
  inherited::myUserData.sessionID = inherited::getID(); // (== socket handle)
  if (!myStream.init(inherited::myUserData))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init processing stream, aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF
  myStream.start();
  if (!myStream.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start processing stream, aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

  // step2: register our handle with the reactor
  // *NOTE*: --> done by the base class
  // *WARNING*: as soon as this returns, data MAY start arriving
  // at handle_input() and fill our stream (even more so on multithreaded
  // reactors)
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    // *NOTE*: this MAY have happened because there are too many
    // open local connections... --> not an error !
    if (ACE_OS::last_error() != EBUSY)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to inherited::open(): \"%m\", aborting\n")));

    // clean up
    myStream.stop();

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

  return 0;
}

template <typename ConfigType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType, StreamType>::handle_input(ACE_HANDLE handle_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_input"));

  ACE_UNUSED_ARG(handle_in);

  size_t bytes_received = 0;

  // sanity check
  ACE_ASSERT(myCurrentReadBuffer == NULL);

  // read some data from the socket
  myCurrentReadBuffer = allocateMessage(myDefaultBufferSize);
  if (myCurrentReadBuffer == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocateMessage(%u), aborting\n"),
               myDefaultBufferSize));

    // reactor will invoke handle_close()
    return -1;
  } // end IF

  // read some data from the socket...
  bytes_received = inherited::peer_.recv(myCurrentReadBuffer->wr_ptr(),
                                         myCurrentReadBuffer->size());
  switch (bytes_received)
  {
    case -1:
    {
      // connection reset by peer ? --> not an error
      if ((ACE_OS::last_error() != ECONNRESET) &&
          (ACE_OS::last_error() != EPIPE))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%m\", returning\n")));

      // clean up
      myCurrentReadBuffer->release();
      myCurrentReadBuffer = NULL;

      // reactor will invoke handle_close()
      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: socket was closed by the peer...\n"),
//                  handle_in));

      // clean up
      myCurrentReadBuffer->release();
      myCurrentReadBuffer = NULL;

      // reactor will invoke handle_close()
      return -1;
    }
    default:
    {
      // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: received %u bytes...\n"),
//                  handle_in,
//                  bytes_received));

      // adjust write pointer
      myCurrentReadBuffer->wr_ptr(bytes_received);

      break;
    }
  } // end SWITCH

  // push the buffer onto our stream for processing
  if (myStream.put(myCurrentReadBuffer) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::put(): \"%m\", aborting\n")));

      // clean up
    myCurrentReadBuffer->release();
    myCurrentReadBuffer = NULL;

    // reactor will invoke handle_close()
    return -1;
  } // end IF

  // ... bye bye
  myCurrentReadBuffer = NULL;

  return 0;
}

// template <typename StreamType>
// int
// RPG_Net_StreamSocketBase<StreamType>::handle_output(ACE_HANDLE handle_in)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_output"));
//
//   ACE_UNUSED_ARG(handle_in);
//
//   ssize_t bytes_sent = 0;
//
//   if (myCurrentWriteBuffer == NULL)
//   {
//     // get a chunk from the stream...
//     ACE_Time_Value nowait(ACE_OS::gettimeofday());
//     if (myStream.get(myCurrentWriteBuffer, &nowait)) // don't block
//     {
// //       ACE_DEBUG((LM_ERROR,
// //                  ACE_TEXT("failed to ACE_Stream::get(): \"%m\", returning\n")));
//
//       // *TODO*: maybe there was no data ?
//       return 0;
//     } // end IF
//   } // end IF
//
//   // put some data into the socket...
//   bytes_sent = peer_.send(myCurrentWriteBuffer->rd_ptr(),
//                           myCurrentWriteBuffer->length());
//   switch (bytes_sent)
//   {
//     case -1:
//     {
//       // connection reset by peer ? --> not an error
//       if (ACE_OS::last_error() != ECONNRESET)
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to ACE_SOCK_Stream::send(): \"%m\", returning\n")));
//
//       // clean up
//       myCurrentWriteBuffer->release();
//       myCurrentWriteBuffer = NULL;
//
//       // reactor will invoke handle_close()
//       return -1;
//     }
//     // *** GOOD CASES ***
//     case 0:
//     {
// //       ACE_DEBUG((LM_DEBUG,
// //                  ACE_TEXT("[%u]: socket was closed by the peer...\n"),
// //                  handle_in));
//
//       // clean up
//       myCurrentWriteBuffer->release();
//       myCurrentWriteBuffer = NULL;
//
//       // reactor will invoke handle_close()
//       return -1;
//     }
//     default:
//     {
// //       // debug info
// //       ACE_DEBUG((LM_DEBUG,
// //                 ACE_TEXT("[%u]: sent %u bytes...\n"),
// //                 handle_in,
// //                 bytes_sent));
//
//       // finished with this buffer ?
//       if (ACE_static_cast(size_t, bytes_sent) == myCurrentWriteBuffer->length())
//       {
//         // clean up
//         myCurrentWriteBuffer->release();
//         myCurrentWriteBuffer = NULL;
//       } // end IF
//       else
//       {
//         // there's more data --> adjust read pointer
//         myCurrentWriteBuffer->rd_ptr(bytes_sent);
//       } // end ELSE
//
//       break;
//     }
//   } // end SWITCH
//
// //   // immediately reschedule sending ?
// //   if (msg_queue()->is_empty() && (myCurrentWriteBuffer == NULL))
// //     reactor()->cancel_wakeup(this,
// //                              ACE_Event_Handler::WRITE_MASK);
// //   else
// //     reactor()->schedule_wakeup(this,
// //                                ACE_Event_Handler::WRITE_MASK);
//
//   return 0;
// }

template <typename ConfigType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType, StreamType>::handle_close(ACE_HANDLE handle_in,
                                                   ACE_Reactor_Mask mask_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_close"));

  // clean up
  if (myStream.isRunning())
  {
    myStream.stop();
  } // end IF
  if (myCurrentReadBuffer)
  {
    myCurrentReadBuffer->release();
    myCurrentReadBuffer = NULL;
  } // end IF
  if (myCurrentWriteBuffer)
  {
    myCurrentWriteBuffer->release();
    myCurrentWriteBuffer = NULL;
  } // end IF

  // invoke base class maintenance
  // *NOTE*: in the end, this will "delete this"...
  return inherited::handle_close(handle_in,
                                 mask_in);
}

template <typename ConfigType,
          typename StreamType>
const bool
RPG_Net_StreamSocketBase<ConfigType, StreamType>::collect(RPG_Net_RuntimeStatistic& data_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::collect"));

  try
  {
    return myStream.collect(data_out);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Common_IStatistic::collect(), aborting\n")));
  }

  return false;
}

template <typename ConfigType,
          typename StreamType>
void
RPG_Net_StreamSocketBase<ConfigType, StreamType>::report() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::report"));

  try
  {
    return myStream.report();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Common_IStatistic::report(), aborting\n")));
  }
}

template <typename ConfigType,
          typename StreamType>
RPG_Net_Message*
RPG_Net_StreamSocketBase<ConfigType, StreamType>::allocateMessage(const unsigned long& requestedSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::allocateMessage"));

  // init return value(s)
  RPG_Net_Message* message_out = NULL;

  try
  {
    message_out = ACE_static_cast(RPG_Net_Message*,
                                  myAllocator->malloc(requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  } // end IF

  return message_out;
}
