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

#include <ace/Reactor.h>

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::RPG_Net_AsynchStreamHandler_T()
 : inherited(NULL),
   myBuffer(NULL),
   myNotificationStrategy(ACE_Reactor::instance(),       // default reactor
                          this,                          // event handler
                          ACE_Event_Handler::WRITE_MASK) // handle output only
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::RPG_Net_AsynchStreamHandler_T"));

  ACE_ASSERT(false);
  ACE_NOTREACHED(return;)
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::RPG_Net_AsynchStreamHandler_T(MANAGER_t* manager_in)
 : inherited(manager_in),
   myBuffer(NULL),
   myNotificationStrategy(ACE_Reactor::instance(),       // default reactor
                          this,                          // event handler
                          ACE_Event_Handler::WRITE_MASK) // handle output only
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::RPG_Net_AsynchStreamHandler_T"));

}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::~RPG_Net_AsynchStreamHandler_T()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::~RPG_Net_AsynchStreamHandler_T"));

  // wait for all workers within the stream (if any)
  myStream.waitForCompletion();
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
void
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::ping()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::ping"));

  myStream.ping();
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
void
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::open(ACE_HANDLE handle_in,
                                                ACE_Message_Block& messageBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::open"));

  // sanity check
  ACE_ASSERT(handle_in);
  // *NOTE*: we should have initialized by now...
  // --> make sure this was successful before we proceed
  if (!inherited::myIsInitialized)
  {
    // (most probably) too many connections...
    ACE_OS::last_error(EBUSY);

    // clean up
    delete this;

    return;
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

      // clean up
      delete this;

      return;
    } // end IF
  } // end IF
  if (!myStream.init(inherited::myUserData))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init processing stream, aborting\n")));

    // clean up
    delete this;

    return;
  } // end IF

//   myStream.dump_state();
  myStream.start();
  if (!myStream.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start processing stream, aborting\n")));

    // clean up
    delete this;

    return;
  } // end IF

  // get notified when there is data to write
  // *TODO*: should not be necessary, override reply() in the stream head module
  // and invoke an asynch write directly
  ACE_Module<ACE_MT_SYNCH>* module = NULL;
  module = myStream.head();
  if (!module)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no head module found, returning\n")));

    // clean up
    delete this;

    return;
  } // end IF
  module->reader()->msg_queue()->notification_strategy(&myNotificationStrategy);
/*  inherited::msg_queue(module->reader()->msg_queue());
  inherited::msg_queue()->notification_strategy(&myNotificationStrategy);
*/

  // tweak socket, init & start I/O
  inherited::open(handle_in, messageBlock_in);
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
int
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::handle_output(ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::handle_output"));

  ACE_UNUSED_ARG(handle_in);

  if (myBuffer == NULL)
  {
    // get next data chunk from the stream...
    // *NOTE*: should NEVER block (barring context switches...)
    if (myStream.get(myBuffer, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Stream::get(): \"%m\", aborting\n")));

      // reactor will invoke handle_close()
      return -1;
    } // end IF
  } // end IF

  // sanity check: finished processing connection ?
  if (myBuffer->msg_type() == ACE_Message_Block::MB_STOP)
  {
    myBuffer->release();
    myBuffer = NULL;

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: finished sending...\n"),
//                  peer_.get_handle()));

    // finished
    return -1;
  } // end IF

  // start (asynch) write...
  if (inherited::myOutputStream.write(*myBuffer,           // data
                                      myBuffer->size(),    // bytes to write
                                      NULL,                // ACT
                                      0,                   // priority
                                      ACE_SIGRTMIN) == -1) // signal number
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Write_Stream::write(%u): \"%m\", aborting\n"),
               myBuffer->size()));

    // clean up
    myBuffer->release();
    myBuffer = NULL;

    return -1;
  } // end IF

  return 0;
}

template <typename ConfigType,
          typename StatisticsContainerType,
          typename StreamType>
int
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::handle_close(ACE_HANDLE handle_in,
                                                        ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::handle_close"));

  // clean up
  if (myStream.isRunning())
  {
    myStream.stop();
  } // end IF
  if (myBuffer)
  {
    myBuffer->release();
    myBuffer = NULL;
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
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::collect(StatisticsContainerType& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::collect"));

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
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::report() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::report"));

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
void
RPG_Net_AsynchStreamHandler_T<ConfigType,
                              StatisticsContainerType,
                              StreamType>::handle_read_stream(const ACE_Asynch_Read_Stream::Result& result)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchStreamHandler_T::handle_read_stream"));

  ACE_DEBUG((LM_DEBUG, "********************\n"));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "bytes_to_read", result.bytes_to_read()));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "handle", result.handle()));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred()));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "act", (uintptr_t)result.act()));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "success", result.success()));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "completion_key", (uintptr_t)result.completion_key()));
  ACE_DEBUG((LM_DEBUG, "%s = %d\n", "error", result.error()));
  ACE_DEBUG((LM_DEBUG, "********************\n"));
#if 0
  // This can overrun the ACE_Log_Msg buffer and do bad things.
  // Re-enable it at your risk.

  // Reset pointers.
  result.message_block ().rd_ptr ()[result.bytes_transferred ()] = '\0';

  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "message_block", result.message_block ().rd_ptr ()));
#endif /* 0 */

  // sanity check
  // *TODO*: can this happen in asynch I/O scenarios ?
  if (result.success() == 0)
  {
    // connection reset by peer ? --> not an error
    if ((ACE_OS::last_error() != ECONNRESET) &&
        (ACE_OS::last_error() != EPIPE))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to read from input stream (%d): %d, \"%m\", aborting\n"),
                 result.handle(),
                 result.error()));

    // clean up
    result.message_block().release();
    delete this; // close connection

    return;
  } // end IF

  switch (result.bytes_transferred())
  {
    case -1:
    {
      // connection reset by peer ? --> not an error
      // *TODO*: can this happen in asynch I/O scenarios ?
      if ((ACE_OS::last_error() != ECONNRESET) &&
          (ACE_OS::last_error() != EPIPE))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to read from input stream (%d): %d, \"%m\", aborting\n"),
                   result.handle(),
                   result.error()));

      // clean up
      result.message_block().release();
      delete this; // close connection

      break;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: socket was closed by the peer...\n"),
//                  myHandle));

      // clean up
      result.message_block().release();
      delete this; // close connection

      break;
    }
    default:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%d]: received %u bytes...\n"),
//                  result.handle(),
//                  result.bytes_transferred()));

      // push the buffer onto our stream for processing
      if (myStream.put(&result.message_block(), NULL) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Stream::put(): \"%m\", aborting\n")));

        // clean up
        result.message_block().release();
        delete this; // close connection

        return;
      } // end IF

      // start next read
      inherited::initiate_read_stream();

      break;
    }
  } // end SWITCH
}
