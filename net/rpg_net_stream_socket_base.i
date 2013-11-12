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

#include "rpg_net_defines.h"
#include "rpg_net_iconnectionmanager.h"

#include "rpg_stream_iallocator.h"

#include <ace/Message_Block.h>

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::RPG_Net_StreamSocketBase(MANAGER_t* manager_in)
 : inherited(manager_in),
   myCurrentReadBuffer(NULL),
   myCurrentWriteBuffer(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::RPG_Net_StreamSocketBase"));

}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::~RPG_Net_StreamSocketBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::~RPG_Net_StreamSocketBase"));

  // wait for all workers within the stream (if any)
  myStream.waitForCompletion();
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::open"));

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

  // *TODO*: assumptions about ConfigType ?!?: clearly a design glitch
  // --> implement higher up !

  // step1: init/start data processing stream
  inherited::myUserData.sessionID = inherited::getID(); // (== socket handle)
  if (inherited::myUserData.module)
  {
    if (myStream.push(inherited::myUserData.module))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
                 inherited::myUserData.module->name()));

      return -1;
    } // end IF
  } // end IF
  if (!myStream.init(inherited::myUserData))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init processing stream, aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

//   myStream.dump_state();
  myStream.start();
  if (!myStream.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start processing stream, aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

  // "borrow" message queue from stream head
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = myStream.head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF
  inherited::msg_queue(module->reader()->msg_queue());
  ACE_Reactor_Notification_Strategy& strategy = inherited::myNotificationStrategy;
  inherited::msg_queue()->notification_strategy(&strategy);

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
          typename StatisticsContainerType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::handle_input(ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_input"));

  ACE_UNUSED_ARG(handle_in);

  size_t bytes_received = 0;

  // sanity check
  ACE_ASSERT(myCurrentReadBuffer == NULL);

  // read some data from the socket
  myCurrentReadBuffer = allocateMessage(inherited::myUserData.defaultBufferSize);
  if (myCurrentReadBuffer == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocateMessage(%u), aborting\n"),
               inherited::myUserData.defaultBufferSize));

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

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::handle_output(ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_output"));

  ACE_UNUSED_ARG(handle_in);

  ssize_t bytes_sent = 0;

  if (myCurrentWriteBuffer == NULL)
  {
    // get next data chunk from the stream...
    // *NOTE*: should NEVER block (barring context switches...)
    if (inherited::getq(myCurrentWriteBuffer, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Task::getq(): \"%m\", aborting\n")));

      // reactor will invoke handle_close()
      return -1;
    } // end IF
  } // end IF

  // finished ?
  if (myCurrentWriteBuffer->msg_type() == ACE_Message_Block::MB_STOP)
  {
    myCurrentWriteBuffer->release();
    myCurrentWriteBuffer = NULL;

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: finished sending...\n"),
//                  peer_.get_handle()));

    // finished
    return -1;
  } // end IF

  // put some data into the socket...
  bytes_sent = inherited::peer_.send(myCurrentWriteBuffer->rd_ptr(),
                                     myCurrentWriteBuffer->length());
  switch (bytes_sent)
  {
    case -1:
    {
        // connection reset by peer/broken pipe ? --> not an error
      if ((ACE_OS::last_error() != ECONNRESET) &&
          (ACE_OS::last_error() != EPIPE))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_SOCK_Stream::send(): \"%m\", aborting\n")));

      // clean up
      myCurrentWriteBuffer->release();
      myCurrentWriteBuffer = NULL;

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
      myCurrentWriteBuffer->release();
      myCurrentWriteBuffer = NULL;

      // reactor will invoke handle_close()
      return -1;
    }
    default:
    {
//       ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("[%u]: sent %u bytes...\n"),
//                 handle_in,
//                 bytes_sent));

      // finished with this buffer ?
      if (static_cast<size_t> (bytes_sent) == myCurrentWriteBuffer->length())
      {
        // clean up
        myCurrentWriteBuffer->release();
        myCurrentWriteBuffer = NULL;
      } // end IF
      else
      {
        // there's more data --> adjust read pointer
        myCurrentWriteBuffer->rd_ptr(bytes_sent);
      } // end ELSE

      break;
    }
  } // end SWITCH

  // immediately reschedule sending ?
  if (myCurrentWriteBuffer)
    inherited::reactor()->schedule_wakeup(this,
                                          ACE_Event_Handler::WRITE_MASK);

  return 0;
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
int
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::handle_close(ACE_HANDLE handle_in,
                                                   ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_close"));

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
          typename StatisticsContainerType,
          typename StreamType>
bool
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::collect(StatisticsContainerType& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::collect"));

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
          typename StatisticsContainerType,
          typename StreamType>
void
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::report"));

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
          typename StatisticsContainerType,
          typename StreamType>
ACE_Message_Block*
RPG_Net_StreamSocketBase<ConfigType,
                         StatisticsContainerType,
                         StreamType>::allocateMessage(const unsigned int& requestedSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::allocateMessage"));

  // init return value(s)
  ACE_Message_Block* message_out = NULL;

  try
  {
    message_out = static_cast<ACE_Message_Block*>(inherited::myUserData.messageAllocator->malloc(requestedSize_in));
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
