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
#include "stdafx.h"

#include "rpg_net_sockethandler.h"

#include <ace/Message_Block.h>

#include "rpg_common_defines.h"

#include "rpg_net_defines.h"
#include "rpg_net_connection_manager.h"

RPG_Net_SocketHandler::RPG_Net_SocketHandler()
 : inherited(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance())
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::RPG_Net_SocketHandler"));

}

RPG_Net_SocketHandler::~RPG_Net_SocketHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::~RPG_Net_SocketHandler"));

  // wait for our worker (if any)
  if (inherited::myUserData.useThreadPerConnection)
		if (inherited::wait() == -1)
			ACE_DEBUG((LM_ERROR,
			           ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
}

int
RPG_Net_SocketHandler::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::svc"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker (connection: %u) starting...\n"),
             myStream.getSessionID()));

  ssize_t bytes_sent = 0;
  while (true)
  {
    if (myCurrentWriteBuffer == NULL)
      if (myStream.get(myCurrentWriteBuffer, NULL) == -1) // block
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Stream::get(): \"%m\", aborting\n")));

        // what else can we do ?
        return -1;
      } // end IF

    // finished ?
    if (myCurrentWriteBuffer->msg_type() == ACE_Message_Block::MB_STOP)
    {
      myCurrentWriteBuffer->release();
      myCurrentWriteBuffer = NULL;

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: finished sending...\n"),
//                  peer_.get_handle()));

      // leave loop, we're finished
      break;
    } // end IF

    // put some data into the socket...
    bytes_sent = peer_.send(myCurrentWriteBuffer->rd_ptr(),
                            myCurrentWriteBuffer->length(),
                            0, // default behavior
//                             MSG_DONTWAIT, // don't block
                            NULL); // block if necessary...
    switch (bytes_sent)
    {
      case -1:
      {
        // connection reset by peer/broken pipe ? --> not an error
        if ((ACE_OS::last_error() != ECONNRESET) &&
            (ACE_OS::last_error() != EPIPE))
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_SOCK_Stream::send(): \"%m\", returning\n")));

        myCurrentWriteBuffer->release();
        myCurrentWriteBuffer = NULL;

        // nothing to do but wait for our shutdown signal (see above)...
        break;
      }
      // *** GOOD CASES ***
      case 0:
      {
        myCurrentWriteBuffer->release();
        myCurrentWriteBuffer = NULL;

//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("[%u]: socket was closed by the peer...\n"),
//                    peer_.get_handle()));

        // nothing to do but wait for our shutdown signal (see above)...
        break;
      }
      default:
      {
//         ACE_DEBUG((LM_DEBUG,
//                   ACE_TEXT("[%u]: sent %u bytes...\n"),
//                   peer_.get_handle(),
//                   bytes_sent));

        // finished with this buffer ?
        if (static_cast<size_t>(bytes_sent) == myCurrentWriteBuffer->length())
        {
          // get the next one...
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
  } // end WHILE

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("(%t) worker (connection: %u) joining...\n"),
  //           myStream.getSessionID()));

  return 0;
}

int
RPG_Net_SocketHandler::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::open"));

  // init/start stream, register reading data with reactor...
  // --> done by the base class
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_StreamSocketBase::open(): \"%m\", aborting\n")));

    // MOST PROBABLE REASON: too many open connections...

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

  // OK: start a worker ?
  if (inherited::myUserData.useThreadPerConnection)
  {
		ACE_thread_t thread_ids[1];
		thread_ids[0] = 0;
		ACE_hthread_t thread_handles[1];
		thread_handles[0] = 0;
		char thread_name[RPG_COMMON_BUFSIZE];
		ACE_OS::memset(thread_name, 0, sizeof(thread_name));
		ACE_OS::strcpy(thread_name, RPG_NET_CONNECTION_HANDLER_THREAD_NAME);
		const char* thread_names[1];
		thread_names[0] = thread_name;
    if (inherited::activate((THR_NEW_LWP |
														 THR_JOINABLE |
														 THR_INHERIT_SCHED),                        // flags
														1,                                          // # threads
														0,                                          // force spawning
														ACE_DEFAULT_THREAD_PRIORITY,                // priority
														RPG_NET_CONNECTION_HANDLER_THREAD_GROUP_ID, // group id
														NULL,                                       // corresp. task --> use 'this'
														thread_handles,                             // thread handle(s)
														NULL,                                       // thread stack(s)
														NULL,                                       // thread stack size(s)
														thread_ids,                                 // thread id(s)
														thread_names) == -1)                        // thread name(s)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", aborting\n")));

      // reactor will invoke close() --> handle_close()
      return -1;
    } // end IF
  } // end IF

  return 0;
}

// int
// RPG_Net_SocketHandler::handle_input(ACE_HANDLE handle_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::handle_input"));
//
//   ACE_UNUSED_ARG(handle_in);
//
//   size_t bytes_received = 0;
//
//   // sanity check
//   ACE_ASSERT(myCurrentReadBuffer == NULL);
//
//   // read some data from the socket
//   myCurrentReadBuffer = allocateMessage(myDefaultBufferSize);
//   if (myCurrentReadBuffer == NULL)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to allocateMessage(%u), aborting\n"),
//                myDefaultBufferSize));
//
//     // reactor will invoke handle_close()
//     return -1;
//   } // end IF
//
//   // read some data from the socket...
//   bytes_received = inherited::peer_.recv(myCurrentReadBuffer->wr_ptr(),
//                                          myCurrentReadBuffer->size());
//   switch (bytes_received)
//   {
//     case -1:
//     {
//       // connection reset by peer ? --> not an error
//       if ((ACE_OS::last_error() != ECONNRESET) &&
//           (ACE_OS::last_error() != EPIPE))
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%m\", returning\n")));
//
//       // clean up
//       myCurrentReadBuffer->release();
//       myCurrentReadBuffer = NULL;
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
//       myCurrentReadBuffer->release();
//       myCurrentReadBuffer = NULL;
//
//       // reactor will invoke handle_close()
//       return -1;
//     }
//     default:
//     {
//       // debug info
// //       ACE_DEBUG((LM_DEBUG,
// //                  ACE_TEXT("[%u]: received %u bytes...\n"),
// //                  handle_in,
// //                  bytes_received));
//
//       // adjust write pointer
//       myCurrentReadBuffer->wr_ptr(bytes_received);
//
//       break;
//     }
//   } // end SWITCH
//
//   // push the buffer onto our stream for processing
//   if (myStream.put(myCurrentReadBuffer) == -1)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_Stream::put(): \"%m\", aborting\n")));
//
//       // clean up
//     myCurrentReadBuffer->release();
//     myCurrentReadBuffer = NULL;
//
//     // reactor will invoke handle_close()
//     return -1;
//   } // end IF
//
//   // ... bye bye
//   myCurrentReadBuffer = NULL;
//
//   return 0;
// }

int
RPG_Net_SocketHandler::handle_close(ACE_HANDLE handle_in,
                                    ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::handle_close"));

  // deal with our worker, if any
  if (inherited::myUserData.useThreadPerConnection)
		shutdown();

  // invoke base class maintenance
  // *NOTE*: in the end, this will "delete this"...
  return inherited::handle_close(handle_in,
                                 mask_in);
}

void
RPG_Net_SocketHandler::shutdown()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandler::shutdown"));

	ACE_Message_Block* stop_mb = NULL;
	ACE_NEW_NORETURN(stop_mb,
									 ACE_Message_Block(0,                                  // size
																		 ACE_Message_Block::MB_STOP,         // type
																		 NULL,                               // continuation
																		 NULL,                               // data
																		 NULL,                               // buffer allocator
																		 NULL,                               // locking strategy
																		 ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
																		 ACE_Time_Value::zero,               // execution time
																		 ACE_Time_Value::max_time,           // deadline time
																		 NULL,                               // data block allocator
																		 NULL));                             // message allocator)
	if (!stop_mb)
	{
		ACE_DEBUG((LM_ERROR,
								ACE_TEXT("failed to allocate ACE_Message_Block: \"%m\", aborting\n")));

		return;
	} // end IF

	if (inherited::putq(stop_mb, NULL) == -1)
	{
		ACE_DEBUG((LM_ERROR,
							ACE_TEXT("failed to ACE_Task::putq(): \"%m\", continuing\n")));

		stop_mb->release();
	} // end IF

  // *NOTE*: defer waiting for any worker(s) to the dtor
}
