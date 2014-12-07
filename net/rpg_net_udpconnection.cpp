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

#include "rpg_net_udpconnection.h"

#include "rpg_net_defines.h"
//#include "rpg_net_connection_manager_common.h"

//#include "rpg_common_defines.h"
#include "rpg_common_macros.h"

RPG_Net_UDPConnection::RPG_Net_UDPConnection (const ACE_INET_Addr& peerAddress_in)
//: inherited (RPG_NET_CONNECTIONMANAGER_SINGLETON::instance())
 : inherited ()
 //, inherited2 (NULL, // default thread manager
 //              NULL) // default message queue
 , peerAddress_ (peerAddress_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::RPG_Net_UDPConnection"));

  //if (inherited::open () == -1)
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ACE_SOCK_Dgram::open(): \"%m\", continuing\n")));
}

RPG_Net_UDPConnection::~RPG_Net_UDPConnection ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::~RPG_Net_UDPConnection"));

  if (ACE_SOCK_Dgram::close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram::close(): \"%m\", continuing\n")));

  //// wait for our worker (if any)
  //if (inherited::myUserData.useThreadPerConnection)
  //  if (inherited2::wait () == -1)
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
}

void
RPG_Net_UDPConnection::info (ACE_HANDLE& handle_out,
                             ACE_INET_Addr& localSAP_out,
                             ACE_INET_Addr& remoteSAP_out) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::info"));

  handle_out = ACE_Event_Handler::get_handle ();
  if (inherited::get_local_addr (localSAP_out) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_SOCK_Dgram::get_local_addr(): \"%m\", continuing\n")));
  //localSAP_out = inherited::myAddress;
  //remoteSAP_out = ACE_sap_any_cast(ACE_INET_Addr&);
  remoteSAP_out = peerAddress_;
}

void
RPG_Net_UDPConnection::init (RPG_Net_ClientServerRole_t role_in,
                             unsigned short port_in,
                             bool useLoopback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_UDPConnection::init"));

  RPG_Net_TransportLayer_Base::init (role_in,
                                     port_in,
                                     useLoopback_in);

  if (inherited::myAddress.get_port_number() != myPort)
  {
    if (ACE_SOCK_Dgram::close() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Dgram::close(): \"%m\", continuing\n")));
    inherited::myAddress.set_port_number(myPort, 1);
  } // end IF
  if (ACE_SOCK_Dgram::open(myAddress,
                           ACE_PROTOCOL_FAMILY_INET,
                           0,
                           0) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram::open(): \"%m\", continuing\n")));
}

//int
//RPG_Net_UDPConnection::svc (void)
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::svc"));
//
//  ACE_DEBUG ((LM_DEBUG,
//    ACE_TEXT ("(%t) worker (connection: %u) starting...\n"),
//    inherited::myStream.getSessionID ()));
//
//  ssize_t bytes_sent = 0;
//  while (true)
//  {
//    if (inherited::myCurrentWriteBuffer == NULL)
//      if (inherited::myStream.get (inherited::myCurrentWriteBuffer, NULL) == -1) // block
//      {
//      ACE_DEBUG ((LM_ERROR,
//        ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n")));
//      return -1;
//      } // end IF
//
//    // finished ?
//    if (inherited::myCurrentWriteBuffer->msg_type () == ACE_Message_Block::MB_STOP)
//    {
//      inherited::myCurrentWriteBuffer->release ();
//      inherited::myCurrentWriteBuffer = NULL;
//
//      //       ACE_DEBUG((LM_DEBUG,
//      //                  ACE_TEXT("[%u]: finished sending...\n"),
//      //                  peer_.get_handle()));
//
//      break;
//    } // end IF
//
//    // put some data into the socket...
//    bytes_sent = inherited::send (inherited::myCurrentWriteBuffer->rd_ptr (),
//                                  inherited::myCurrentWriteBuffer->length (),
//                                  peerAddress_,
//                                  0);
//    switch (bytes_sent)
//    {
//      case -1:
//      {
//        // connection reset by peer/broken pipe ? --> not an error
//        int error = ACE_OS::last_error ();
//        if ((error != ECONNRESET) &&
//            (error != ENOTSOCK) &&
//            (error != EPIPE))
//            ACE_DEBUG ((LM_ERROR,
//                        ACE_TEXT ("failed to ACE_SOCK_Stream::send(): \"%m\", returning\n")));
//
//        inherited::myCurrentWriteBuffer->release ();
//        inherited::myCurrentWriteBuffer = NULL;
//
//        // nothing to do but wait for our shutdown signal (see above)...
//        break;
//      }
//        // *** GOOD CASES ***
//      case 0:
//      {
//        inherited::myCurrentWriteBuffer->release ();
//        inherited::myCurrentWriteBuffer = NULL;
//
//        //         ACE_DEBUG((LM_DEBUG,
//        //                    ACE_TEXT("[%u]: socket was closed by the peer...\n"),
//        //                    peer_.get_handle()));
//
//        // nothing to do but wait for our shutdown signal (see above)...
//        break;
//      }
//      default:
//      {
//        //         ACE_DEBUG((LM_DEBUG,
//        //                   ACE_TEXT("[%u]: sent %u bytes...\n"),
//        //                   peer_.get_handle(),
//        //                   bytes_sent));
//
//        // finished with this buffer ?
//        if (static_cast<size_t>(bytes_sent) == inherited::myCurrentWriteBuffer->length ())
//        {
//          // get the next one...
//          inherited::myCurrentWriteBuffer->release ();
//          inherited::myCurrentWriteBuffer = NULL;
//        } // end IF
//        else
//        {
//          // there's more data --> adjust read pointer
//          inherited::myCurrentWriteBuffer->rd_ptr (bytes_sent);
//        } // end ELSE
//
//        break;
//      }
//    } // end SWITCH
//  } // end WHILE
//
//  //ACE_DEBUG((LM_DEBUG,
//  //           ACE_TEXT("(%t) worker (connection: %u) joining...\n"),
//  //           inherited::myStream.getSessionID()));
//
//  return 0;
//}

int
RPG_Net_UDPConnection::open (void* arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::open"));

  ACE_UNUSED_ARG (arg_in);

  std::string address = ACE_TEXT_ALWAYS_CHAR(RPG_NET_DEFAULT_IP_BROADCAST_ADDRESS);
  address += ':';
  std::ostringstream converter;
  converter << RPG_NET_DEFAULT_PORT;
  address += converter.str();
  if (inherited::myAddress.set(ACE_TEXT(address.c_str()), AF_INET) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::set(): \"%m\", continuing\n")));
  if (ACE_SOCK_Dgram::open(myAddress,
                           ACE_PROTOCOL_FAMILY_INET,
                           0,
                           0) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram::open(): \"%m\", continuing\n")));

  //// step1: init/start stream, tweak socket, register reading data with reactor
  //// , ...
  //int result = inherited::open (peerAddress_);
  //if (result == -1)
  //{
  //  ACE_TCHAR buffer[BUFSIZ];
  //  ACE_OS::memset (buffer, 0, sizeof (buffer));
  //  if (peerAddress_.addr_to_string (buffer, sizeof (buffer), 1) == -1)
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to RPG_Net_UDPHandler::open(\"%s\"): \"%m\", aborting\n"),
  //              ACE_TEXT (buffer)));
  //  return -1;
  //} // end IF

  //// step2: start a worker ?
  //if (inherited::myUserData.useThreadPerConnection)
  //{
  //  ACE_thread_t thread_ids[1];
  //  thread_ids[0] = 0;
  //  ACE_hthread_t thread_handles[1];
  //  thread_handles[0] = 0;
  //  char thread_name[RPG_COMMON_BUFSIZE];
  //  ACE_OS::memset (thread_name, 0, sizeof (thread_name));
  //  ACE_OS::strcpy (thread_name,
  //                  RPG_NET_CONNECTION_HANDLER_THREAD_NAME);
  //  const char* thread_names[1];
  //  thread_names[0] = thread_name;
  //  if (inherited2::activate ((THR_NEW_LWP |
  //                             THR_JOINABLE |
  //                             THR_INHERIT_SCHED),                        // flags
  //                             1,                                          // # threads
  //                             0,                                          // force spawning
  //                             ACE_DEFAULT_THREAD_PRIORITY,                // priority
  //                             RPG_NET_CONNECTION_HANDLER_THREAD_GROUP_ID, // group id
  //                             NULL,                                       // corresp. task --> use 'this'
  //                             thread_handles,                             // thread handle(s)
  //                             NULL,                                       // thread stack(s)
  //                             NULL,                                       // thread stack size(s)
  //                             thread_ids,                                 // thread id(s)
  //                             thread_names) == -1)                        // thread name(s)
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to ACE_Task_Base::activate(): \"%m\", aborting\n")));
  //    return -1;
  //  } // end IF
  //} // end IF

  //  // step3: register this connection
  //  if (inherited::myManager)
  //  {
  //    // (try to) register with the connection manager...
  //    try
  //    {
  //      inherited::myIsRegistered = inherited::myManager->registerConnection(this);
  //    }
  //    catch (...)
  //    {
  //      ACE_DEBUG((LM_ERROR,
  //                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
  //    }
  //    if (!inherited::myIsRegistered)
  //    {
  //      // (most probably) too many connections...
  //      ACE_OS::last_error(EBUSY);
  //      return -1;
  //    } // end IF
  //  } // end IF

  return 0;
}

int
RPG_Net_UDPConnection::close (u_long arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::close"));
  // [*NOTE*: hereby we override the default behavior of a ACE_Svc_Handler,
  // which would call handle_close() AGAIN]

  // *NOTE*: this method will be invoked
  // - by any worker after returning from svc()
  //    --> in this case, this should be a NOP (triggered from handle_close(),
  //        which was invoked by the reactor) - we override the default
  //        behavior of a ACE_Svc_Handler, which would call handle_close() AGAIN
  // - by the connector/acceptor when open() fails (e.g. too many connections !)
  //    --> shutdown

  switch (arg_in)
  {
    // called by:
    // - any worker from ACE_Task_Base on clean-up
    //// - acceptor/connector if there are too many connections (i.e. open()
    ////   returned -1)
    case NORMAL_CLOSE_OPERATION:
    {
      //// check specifically for the first case...
      //if (ACE_OS::thr_equal (ACE_Thread::self (),
      //                       inherited::last_thread ()))
      //{
      //  //       if (module())
      //  //         ACE_DEBUG((LM_DEBUG,
      //  //                    ACE_TEXT("\"%s\" worker thread (ID: %t) leaving...\n"),
      //  //                    ACE_TEXT_ALWAYS_CHAR(name())));
      //  //       else
      //  //         ACE_DEBUG((LM_DEBUG,
      //  //                    ACE_TEXT("worker thread (ID: %t) leaving...\n")));

      //  break;
      //} // end IF

      // too many connections: invoke inherited default behavior
      // --> simply fall through to the next case
    }
      //// called by external (e.g. reactor) thread wanting to close the connection
      //// (e.g. too many connections)
      //// *NOTE*: this eventually calls handle_close() (see below)
    case CLOSE_DURING_NEW_CONNECTION:
      //return inherited::close ();
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument: %u, returning\n"),
                  arg_in));

      break;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Net_UDPConnection::handle_output (ACE_HANDLE handle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::handle_output"));

  ACE_UNUSED_ARG (handle_in);

  // *IMPORTANT NOTE*: in a threaded environment, workers MAY be
  // dispatching the reactor notification queue concurrently (most notably,
  // ACE_TP_Reactor) --> enforce proper serialization
  if (inherited::mySerializeOutput)
    inherited::mySendLock.acquire ();

  if (inherited::myCurrentWriteBuffer == NULL)
  {
    // send next data chunk from the stream...
    // *IMPORTANT NOTE*: should NEVER block, as available outbound data has
    // been notified to the reactor
    int result = -1;
    //if (!inherited::myUserData.useThreadPerConnection)
      result = inherited::myStream.get (inherited::myCurrentWriteBuffer,
                                        const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero));
    //else
    //  result = inherited2::getq (inherited::myCurrentWriteBuffer,
    //                             const_cast<ACE_Time_Value*> (&ACE_Time_Value::zero));
    if (result == -1)
    {
      // *IMPORTANT NOTE*: a number of issues can occur here:
      // - connection has been closed in the meantime
      // - queue has been deactivated
      int error = ACE_OS::last_error ();
      if ((error != EAGAIN) ||  // <-- connection has been closed in the meantime
          (error != ESHUTDOWN)) // <-- queue has been deactivated
        ACE_DEBUG ((LM_ERROR,
                    (inherited::myConfiguration.useThreadPerConnection ? ACE_TEXT ("failed to ACE_Task::getq(): \"%m\", aborting\n")
                                                                       : ACE_TEXT ("failed to ACE_Stream::get(): \"%m\", aborting\n"))));

      // clean up
      if (inherited::mySerializeOutput)
        inherited::mySendLock.release ();

      return -1;
    } // end IF
  } // end IF
  ACE_ASSERT (inherited::myCurrentWriteBuffer);

  // finished ?
  if (inherited::myConfiguration.useThreadPerConnection &&
      inherited::myCurrentWriteBuffer->msg_type () == ACE_Message_Block::MB_STOP)
  {
    inherited::myCurrentWriteBuffer->release ();
    inherited::myCurrentWriteBuffer = NULL;

    //       ACE_DEBUG((LM_DEBUG,
    //                  ACE_TEXT("[%u]: finished sending...\n"),
    //                  peer_.get_handle ()));

    // clean up
    if (inherited::mySerializeOutput)
      inherited::mySendLock.release ();

    return -1;
  } // end IF

  // put some data into the socket...
  ssize_t bytes_sent = inherited::send (inherited::myCurrentWriteBuffer->rd_ptr (),
                                        inherited::myCurrentWriteBuffer->length (),
                                        peerAddress_,
                                        0);
  switch (bytes_sent)
  {
    case -1:
    {
      // *IMPORTANT NOTE*: a number of issues can occur here:
      // - connection reset by peer
      // - connection abort()ed locally
      int error = ACE_OS::last_error ();
      if ((error != ECONNRESET) &&
          (error != ECONNABORTED) &&
          (error != EPIPE) &&      // <-- connection reset by peer
          // -------------------------------------------------------------------
          (error != ENOTSOCK) &&
          (error != EBADF))        // <-- connection abort()ed locally
          ACE_DEBUG ((LM_ERROR,
          ACE_TEXT ("failed to ACE_SOCK_Stream::send(): \"%m\", aborting\n")));

      // clean up
      inherited::myCurrentWriteBuffer->release ();
      inherited::myCurrentWriteBuffer = NULL;
      if (inherited::mySerializeOutput)
        inherited::mySendLock.release ();

      return -1;
    }
      // *** GOOD CASES ***
    case 0:
    {
      //       ACE_DEBUG((LM_DEBUG,
      //                  ACE_TEXT("[%u]: socket was closed by the peer...\n"),
      //                  handle_in));

      // clean up
      inherited::myCurrentWriteBuffer->release ();
      inherited::myCurrentWriteBuffer = NULL;
      if (inherited::mySerializeOutput)
        inherited::mySendLock.release ();

      return -1;
    }
    default:
    {
      //       ACE_DEBUG ((LM_DEBUG,
      //                  ACE_TEXT ("[%u]: sent %u bytes...\n"),
      //                  handle_in,
      //                  bytes_sent));

      // finished with this buffer ?
      inherited::myCurrentWriteBuffer->rd_ptr (static_cast<size_t> (bytes_sent));
      if (inherited::myCurrentWriteBuffer->length () > 0)
        break; // there's more data

      // clean up
      inherited::myCurrentWriteBuffer->release ();
      inherited::myCurrentWriteBuffer = NULL;

      break;
    }
  } // end SWITCH

  // immediately reschedule sending ?
  //  if ((myCurrentWriteBuffer == NULL) && inherited::msg_queue()->is_empty())
  //  {
  //    if (inherited::reactor()->cancel_wakeup(this,
  //                                            ACE_Event_Handler::WRITE_MASK) == -1)
  //      ACE_DEBUG((LM_ERROR,
  //                 ACE_TEXT("failed to ACE_Reactor::cancel_wakeup(): \"%m\", continuing\n")));
  //  } // end IF
  //  else
  if (inherited::myCurrentWriteBuffer != NULL)
  {
    // clean up
    if (inherited::mySerializeOutput)
      inherited::mySendLock.release ();

    return 1;
  } // end IF
  //if (inherited::reactor()->schedule_wakeup(this,
  //                                          ACE_Event_Handler::WRITE_MASK) == -1)
  //  ACE_DEBUG((LM_ERROR,
  //             ACE_TEXT("failed to ACE_Reactor::schedule_wakeup(): \"%m\", continuing\n")));

  // clean up
  if (inherited::mySerializeOutput)
    inherited::mySendLock.release ();

  return 0;
}

int
RPG_Net_UDPConnection::handle_close (ACE_HANDLE handle_in,
                                     ACE_Reactor_Mask mask_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::handle_close"));

  switch (mask_in)
  {
    case ACE_Event_Handler::READ_MASK:       // --> socket has been closed
    case ACE_Event_Handler::ALL_EVENTS_MASK: // --> connect failed (e.g. connection refused) /
      //     accept failed (e.g. too many connections) /
      //     select failed (EBADF see Select_Reactor_T.cpp)
      //     asynch abort
      break;
    case ACE_Event_Handler::EXCEPT_MASK:
      // *IMPORTANT NOTE*: the TP_Reactor dispatches notifications in parallel
      // to regular socket events, thus several notifications may be in flight
      // at the same time. In order to avoid confusion when the socket closes,
      // proper synchronization is handled through the reference counting
      // mechanism, i.e. the handler is only deleted after the last reference
      // has been released. Still, handling notifications after this occurs
      // will fail, and the reactor will invoke this method each time. As there
      // is no need to go through the shutdown procedure several times, bail
      // out early here
      //if (handle_in == ACE_INVALID_HANDLE) // <-- notification has completed (!useThreadPerConnection)
      //  ACE_DEBUG((LM_ERROR,
      //             ACE_TEXT("notification completed, continuing\n")));
      return 0;
    default:
      // *PORTABILITY*: this isn't entirely portable...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("handle_close called for unknown reasons (handle: %@, mask: %u) --> check implementation !, continuing\n"),
                  handle_in,
                  mask_in));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("handle_close called for unknown reasons (handle: %d, mask: %u) --> check implementation !, continuing\n"),
                  handle_in,
                  mask_in));
#endif
      break;
  } // end SWITCH

  //// step1: connection shutting down --> signal any worker(s)
  //if (inherited::myUserData.useThreadPerConnection)
  //  shutdown ();

  //  // step2: de-register this connection
  //  if (inherited::myManager &&
  //      inherited::myIsRegistered)
  //  { // (try to) de-register with the connection manager
  //    try
  //    {
  //      inherited::myManager->deregisterConnection(this);
  //    }
  //    catch (...)
  //    {
  //      ACE_DEBUG((LM_ERROR,
  //                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::deregisterConnection(), continuing\n")));
  //    }
  //  } // end IF

  // step3: invoke base-class maintenance
  int result = inherited::handle_close (handle_in, // event handle
                                        mask_in);  // event mask
  if (result == -1)
    // *PORTABILITY*: this isn't entirely portable...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Net_StreamSocketBase::handle_close(%@, %d): \"%m\", continuing\n"),
                handle_in,
                mask_in));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Net_StreamSocketBase::handle_close(%d, %d): \"%m\", continuing\n"),
                handle_in,
                mask_in));
#endif

  return result;
}

//void
//RPG_Net_UDPConnection::shutdown ()
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Net_UDPConnection::shutdown"));
//
//  ACE_Message_Block* stop_mb = NULL;
//  ACE_NEW_NORETURN (stop_mb,
//                    ACE_Message_Block (0,                                  // size
//                                       ACE_Message_Block::MB_STOP,         // type
//                                       NULL,                               // continuation
//                                       NULL,                               // data
//                                       NULL,                               // buffer allocator
//                                       NULL,                               // locking strategy
//                                       ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
//                                       ACE_Time_Value::zero,               // execution time
//                                       ACE_Time_Value::max_time,           // deadline time
//                                       NULL,                               // data block allocator
//                                       NULL));                             // message allocator)
//  if (!stop_mb)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to allocate ACE_Message_Block: \"%m\", aborting\n")));
//    return;
//  } // end IF
//
//  if (inherited2::putq (stop_mb, NULL) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Task::putq(): \"%m\", continuing\n")));
//
//    stop_mb->release ();
//  } // end IF
//
//  // *NOTE*: defer waiting for any worker(s) to the dtor
//}
