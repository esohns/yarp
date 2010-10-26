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

#include "rpg_net_protocol_sockethandler.h"

#include "rpg_net_protocol_common.h"

#include <rpg_net_connection_manager.h>

RPG_Net_Protocol_SocketHandler::RPG_Net_Protocol_SocketHandler()
 : inherited(RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance())
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::RPG_Net_Protocol_SocketHandler"));

}

RPG_Net_Protocol_SocketHandler::~RPG_Net_Protocol_SocketHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::~RPG_Net_Protocol_SocketHandler"));

  // wait for our worker (if any)

  // *WARNING*: cannot use wait(), as this dtor is invoked by the reactor itself
  // on ACE_Svc_Handler::destroy() --> apparently, this deadlocks on some
  // internal (non-recursive) lock...
  wait();
}

// int
// RPG_Net_Protocol_SocketHandler::svc(void)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::svc"));
//
//   // *NOTE*: asynchronous writing to a closed socket triggers the
//   // SIGPIPE signal (default action: abort).
//   // --> as this doesn't use select(), guard against this (ignore the signal)
//   ACE_Sig_Action no_sigpipe(static_cast<ACE_SignalHandler> (SIG_IGN));
//   ACE_Sig_Action original_action;
//   no_sigpipe.register_action(SIGPIPE, &original_action);
//
//   int return_value = 0;
//   ssize_t bytes_sent = 0;
//   while (true)
//   {
//     if (myCurrentWriteBuffer == NULL)
//     {
//       if (myStream.get(myCurrentWriteBuffer, NULL) == -1) // block
//       {
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("worker thread (ID: %t) failed to ACE_Stream::get(): \"%m\", aborting\n")));
//
//         // what else can we do ?
//         return_value = -1;
//
//         break;
//       } // end IF
//     } // end IF
//
//     // finished ?
//     if (myCurrentWriteBuffer->msg_type() == ACE_Message_Block::MB_STOP)
//     {
//       myCurrentWriteBuffer->release();
//       myCurrentWriteBuffer = NULL;
//
// //       ACE_DEBUG((LM_DEBUG,
// //                  ACE_TEXT("[%u]: finished sending...\n"),
// //                  peer_.get_handle()));
//
//       // leave loop, we're finished
//       break;
//     } // end IF
//
//     // put some data into the socket...
//     bytes_sent = peer_.send(myCurrentWriteBuffer->rd_ptr(),
//                             myCurrentWriteBuffer->length(),
//                             NULL); // default behavior
//     switch (bytes_sent)
//     {
//       case -1:
//       {
//         // connection reset by peer/broken pipe ? --> not an error
//         if ((ACE_OS::last_error() != ECONNRESET) &&
//             (ACE_OS::last_error() != EPIPE))
//           ACE_DEBUG((LM_ERROR,
//                      ACE_TEXT("failed to ACE_SOCK_Stream::send(): \"%m\", returning\n")));
//
//         myCurrentWriteBuffer->release();
//         myCurrentWriteBuffer = NULL;
//
//         // what else can we do ?
//         return_value = -1;
//
//         // nothing to do but wait for our shutdown signal (see above)...
//         break;
//       }
//       // *** GOOD CASES ***
//       case 0:
//       {
//         myCurrentWriteBuffer->release();
//         myCurrentWriteBuffer = NULL;
//
// //         ACE_DEBUG((LM_DEBUG,
// //                    ACE_TEXT("[%u]: socket was closed by the peer...\n"),
// //                    peer_.get_handle()));
//
//         // nothing to do but wait for our shutdown signal (see above)...
//         break;
//       }
//       default:
//       {
// //         ACE_DEBUG((LM_DEBUG,
// //                    ACE_TEXT("[%u]: sent %u bytes...\n"),
// //                    peer_.get_handle(),
// //                    bytes_sent));
//
//         // finished with this buffer ?
//         if (static_cast<size_t> (bytes_sent) == myCurrentWriteBuffer->length())
//         {
//           // get the next one...
//           myCurrentWriteBuffer->release();
//           myCurrentWriteBuffer = NULL;
//         } // end IF
//         else
//         {
//           // there's more data --> adjust read pointer
//           myCurrentWriteBuffer->rd_ptr(bytes_sent);
//         } // end ELSE
//
//         break;
//       }
//     } // end SWITCH
//   } // end WHILE
//
//   // clean up
//   no_sigpipe.restore_action(SIGPIPE, original_action);
//
//   return return_value;
// }

int
RPG_Net_Protocol_SocketHandler::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::open"));

  // *NOTE*: fini() on the stream invokes close() which will reset any module's
  // writer/reader tasks --> in order to allow module reuse, reset this here !
  ACE_ASSERT(myUserData.module);
  try
  {
    myUserData.module->resetReaderWriter();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in Stream_Module::resetReaderWriter(), continuing\n")));
  }

  // init/start stream, register reading data with reactor...
  // --> done by the base class
  if (inherited::open(arg_in))
  {
    // MOST PROBABLE REASON: too many open connections...

    if (ACE_OS::last_error() != EBUSY)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to inherited::open(): \"%m\", aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

//   // OK: start a worker
//   if (activate((THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
//                1,                           // # threads
//                0,                           // force spawning
//                ACE_DEFAULT_THREAD_PRIORITY, // priority
//                -1,                          // group id
//                NULL,                        // corresp. task --> use 'this'
//                NULL,                        // thread handle(s)
//                NULL,                        // thread stack(s)
//                NULL,                        // thread stack size(s)
//                NULL))                       // thread id(s)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to activate(): \"%m\", aborting\n")));
//
//     // reactor will invoke close() --> handle_close()
//     return -1;
//   } // end IF

  return 0;
}

int
RPG_Net_Protocol_SocketHandler::handle_close(ACE_HANDLE handle_in,
                                             ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::handle_close"));

  // deal with our worker
  if (thr_count())
  { // stop worker
    try
    {
      // *NOTE*: cannot flush(), as this deactivates() the queue as well,
      // which causes mayhem for our (blocked) worker...
//       myStream.head()->reader()->flush();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in ACE_Task::flush(): \"%m\", continuing\n")));

      // *NOTE*: what else can we do ?
    }
    shutdown();

    // *NOTE*: we defer waiting for our worker to the dtor
  } // end IF

  // invoke base class maintenance
  // *NOTE*: in the end, this will "delete this"...
  return inherited::handle_close(handle_in,
                                 mask_in);
}

void
RPG_Net_Protocol_SocketHandler::shutdown()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::shutdown"));

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

    // what else can we do ?
    return;
  } // end IF

  try
  {
    if (myStream.head()->reader()->put(stop_mb, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to ACE_Task::put(): \"%m\", continuing\n")));

      stop_mb->release();
    } // end IF
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_Task::put(): \"%m\", aborting\n")));

    stop_mb->release();

    // what else can we do ?
    return;
  }
}
