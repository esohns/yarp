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

#include <ace/Message_Block.h>

#include "rpg_stream_iallocator.h"

#include "rpg_net_defines.h"
#include "rpg_net_iconnectionmanager.h"

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

  // step1: wait for all workers within the stream (if any)
  myStream.waitForCompletion();

	// step2: purge any pending notifications ?
	if (!inherited::myUserData.useThreadPerConnection)
    if (reactor()->purge_pending_notifications(this, ACE_Event_Handler::ALL_EVENTS_MASK) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::purge_pending_notifications(%@): \"%m\", continuing\n"),
                 this));

	// clean up
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

  // sanity check(s)
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
    if (myStream.push(inherited::myUserData.module))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
                 inherited::myUserData.module->name()));

      return -1;
    } // end IF

  if (!inherited::myUserData.useThreadPerConnection)
  {
    // "borrow" message queue from stream head
    MODULE_TYPE* module = NULL;
    module = myStream.head();
    if (!module)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("no head module found, returning\n")));

      // reactor will invoke close() --> handle_close()
      return -1;
    } // end IF
    inherited::msg_queue(module->reader()->msg_queue());
    inherited::msg_queue()->notification_strategy(&myNotificationStrategy);
  } // end IF

  if (!myStream.init(inherited::myUserData))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init processing stream, aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF
  //myStream.dump_state();

  // *NOTE*: after this returns, data MAY start arriving at handle_output()
  myStream.start();
  if (!myStream.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start processing stream, aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

  // step2: register io handle with the reactor
  // *NOTE*: this is done by the base class
  // *NOTE*: as soon as this returns, data MAY start arriving
  // at handle_input() and fill the stream
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    // *NOTE*: may have happened because:
		// - too many open local connections
		// - socket has been closed asynchronously
		int error = ACE_OS::last_error();
		if ((error != EBUSY) &&  // <-- too many open local connections
			  (error != ENOTSOCK)) // <-- socket has been closed asynchronously
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
			// *IMPORTANT NOTE*: a number of issues can occur here:
			// - connection reset by peer
			// - connection aborted locally
	    int error = ACE_OS::last_error();
      if ((error != ECONNRESET) && // 
          (error != EPIPE) &&      // <-- connection reset by peer
					(error != ENOTSOCK) &&
					(error != ECONNABORTED)) // <-- connection abort()ed (?) locally
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

  if (myCurrentWriteBuffer == NULL)
  {
    // send next data chunk from the stream...
    // *IMPORTANT NOTE*: NEVER block, as outbound data has been notified to the reactor
		ACE_Time_Value no_wait(ACE_OS::gettimeofday());
    if (getq(myCurrentWriteBuffer, &no_wait) == -1)
    {
			// *IMPORTANT NOTE*: a number of issues can occur here:
			// - connection has been closed in the meantime
	    int error = ACE_OS::last_error();
      if ((error != ESHUTDOWN) &&
				  (error != ENOTSOCK)) // <-- connection has been closed in the meantime
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Task::getq(): \"%m\", aborting\n")));

      // reactor will invoke handle_close()
      return -1;
    } // end IF
  } // end IF
	ACE_ASSERT(myCurrentWriteBuffer);

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
  ssize_t bytes_sent = inherited::peer_.send(myCurrentWriteBuffer->rd_ptr(),
                                             myCurrentWriteBuffer->length());
  switch (bytes_sent)
  {
    case -1:
    {
      // connection reset by peer/broken pipe ? --> not an error
			int error = ACE_OS::last_error();
      if ((error != ECONNRESET) &&
          (error != EPIPE))
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
			myCurrentWriteBuffer->rd_ptr(static_cast<size_t>(bytes_sent));
      if (myCurrentWriteBuffer->length() > 0)
				break; // there's more data

			// clean up
      myCurrentWriteBuffer->release();
      myCurrentWriteBuffer = NULL;

      break;
    }
  } // end SWITCH

  // immediately reschedule sending ?
  if ((myCurrentWriteBuffer == NULL) && msg_queue()->is_empty())
	{
		if (reactor()->cancel_wakeup(this,
                                 ACE_Event_Handler::WRITE_MASK) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::cancel_wakeup(): \"%m\", continuing\n")));
	} // end IF
	else
		if (reactor()->schedule_wakeup(this,
                                   ACE_Event_Handler::WRITE_MASK) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::schedule_wakeup(): \"%m\", continuing\n")));

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
    myStream.stop();

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
