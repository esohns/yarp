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

//#include "rpg_common_timer_manager.h"

#include "rpg_net_defines.h"
#include "rpg_net_stream_common.h"

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::RPG_Net_StreamUDPSocketBase_t ()//MANAGER_T* manager_in)
 : inherited ()//manager_in)
 //, inherited2 ()
 //, myUserData ()
// , myStream ()
 , myCurrentReadBuffer (NULL)
// , myLock ()
 , myCurrentWriteBuffer (NULL)
 , mySerializeOutput (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_StreamUDPSocketBase_t::RPG_Net_StreamUDPSocketBase_t"));

}

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::~RPG_Net_StreamUDPSocketBase_t ()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamUDPSocketBase_t::~RPG_Net_StreamUDPSocketBase_t"));

  // clean up
  if (myUserData.module)
  {
    if (myStream.find (myUserData.module->name ()))
      if (myStream.remove (myUserData.module->name (),
                           ACE_Module_Base::M_DELETE_NONE) == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Stream::remove(\"%s\"): \"%m\", continuing\n"),
                    ACE_TEXT_ALWAYS_CHAR (myUserData.module->name ())));

    if (myUserData.deleteModule)
      delete myUserData.module;
  } // end IF

  if (myCurrentReadBuffer)
    myCurrentReadBuffer->release ();
  if (myCurrentWriteBuffer)
    myCurrentWriteBuffer->release ();
}

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
int
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::open (const ACE_INET_Addr& peerAddress_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamUDPSocketBase_t::open"));

  // step0: init this
  mySerializeOutput = myUserData.serializeOutput;

  // step1: init/start stream
  myUserData.sessionID = reinterpret_cast<unsigned int> (inherited::get_handle ()); // (== socket handle)
  // step1a: connect stream head message queue with the reactor notification
  // pipe ?
  if (!myUserData.useThreadPerConnection)
  {
    // *IMPORTANT NOTE*: enable the reference counting policy, as this will
    // be registered with the reactor several times (1x READ_MASK, nx
    // WRITE_MASK); therefore several threads MAY be dispatching notifications
    // (yes, even concurrently; myLock enforces the proper sequence order, see
    // handle_output()) on the SAME handler. When the socket closes, the event
    // handler should thus not be destroyed() immediately, but simply purge any
    // pending notifications (see handle_close()) and de-register; after the
    // last active notification has been dispatched, it will be safely deleted
    inherited2::reference_counting_policy ().value (ACE_Event_Handler::Reference_Counting_Policy::ENABLED);
    // *IMPORTANT NOTE*: due to reference counting, the
    // ACE_Svc_Handle::shutdown() method will crash, as it references a
    // connection recycler AFTER removing the connection from the reactor (which
    // releases a reference). In the case that "this" is the final reference,
    // this leads to a crash. (see code)
    // --> avoid invoking ACE_Svc_Handle::shutdown()
    // --> this means that "manual" cleanup is necessary (see handle_close())
    //inherited2::closing_ = true;

    myUserData.notificationStrategy = &myNotificationStrategy;
  } // end IF
  // step1b: init final module (if any)
  if (myUserData.module)
  {
    IMODULE_TYPE* imodule_handle = NULL;
    // need a downcast...
    imodule_handle = dynamic_cast<IMODULE_TYPE*>(myUserData.module);
    if (!imodule_handle)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("%s: dynamic_cast<RPG_Stream_IModule> failed, aborting\n"),
                 ACE_TEXT_ALWAYS_CHAR(myUserData.module->name())));
      return -1;
    } // end IF
    MODULE_TYPE* clone = NULL;
    try
    {
      clone = imodule_handle->clone ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("%s: caught exception in RPG_Stream_IModule::clone(), aborting\n"),
                 ACE_TEXT_ALWAYS_CHAR (myUserData.module->name ())));
      return -1;
    }
    if (!clone)
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("%s: failed to RPG_Stream_IModule::clone(), aborting\n"),
                 ACE_TEXT_ALWAYS_CHAR (myUserData.module->name ())));
      return -1;
    }
    myUserData.module = clone;
    myUserData.deleteModule = true;
  } // end IF
  // step1c: init stream
  if (!myStream.init (myUserData))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to init processing stream, aborting\n")));

    // clean up
    delete myUserData.module;
    myUserData.module = NULL;

    return -1;
  } // end IF
  //myStream.dump_state();
  // *NOTE*: as soon as this returns, data starts arriving at
  // handle_output()/msg_queue()
  myStream.start ();
  if (!myStream.isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to start processing stream, aborting\n")));
    return -1;
  } // end IF

  // step2: tweak socket, register I/O handle with the reactor, ...
  // *NOTE*: as soon as this returns, data starts arriving at handle_input()
  int result = inherited::open (ACE_Addr::sap_any,        // local
                                ACE_PROTOCOL_FAMILY_INET, // protocol family
                                0,                        // protocol
                                0);                       // reuse_addr
  if (result == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Dgram::open(): \"%m\", aborting\n")));
    return -1;
  } // end IF

  // *NOTE*: let the reactor manage this handler...
  // *WARNING*: this has some implications (see close() below)
  if (!myUserData.useThreadPerConnection)
    inherited2::remove_reference ();

  return 0;
}

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
int
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::handle_input (ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamUDPSocketBase_t::handle_input"));

  ACE_UNUSED_ARG(handle_in);

  // sanity check
  ACE_ASSERT(myCurrentReadBuffer == NULL);

  // read some data from the socket
  myCurrentReadBuffer = allocateMessage(myUserData.bufferSize);
  if (myCurrentReadBuffer == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocateMessage(%u), aborting\n"),
               myUserData.bufferSize));

    return -1;
  } // end IF

  // read some data from the socket...
  ACE_INET_Addr peer_address;
  size_t bytes_received = inherited::recv (myCurrentReadBuffer->wr_ptr (),
                                           myCurrentReadBuffer->size (),
                                           peer_address,
                                           0);
  switch (bytes_received)
  {
    case -1:
    {
      // *IMPORTANT NOTE*: a number of issues can occur here:
      // - connection reset by peer
      // - connection abort()ed locally
      int error = ACE_OS::last_error ();
      if ((error != ECONNRESET) &&
          (error != EPIPE) &&      // <-- connection reset by peer
          // -------------------------------------------------------------------
          (error != EBADF) &&
          (error != ENOTSOCK) &&
          (error != ECONNABORTED)) // <-- connection abort()ed locally
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%m\", returning\n")));

      // clean up
      myCurrentReadBuffer->release ();
      myCurrentReadBuffer = NULL;

      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: socket was closed by the peer...\n"),
//                  handle_in));

      // clean up
      myCurrentReadBuffer->release ();
      myCurrentReadBuffer = NULL;

      return -1;
    }
    default:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: received %u bytes...\n"),
//                  handle_in,
//                  bytes_received));

      // adjust write pointer
      myCurrentReadBuffer->wr_ptr (bytes_received);

      break;
    }
  } // end SWITCH

  // push the buffer onto our stream for processing
  // *NOTE*: the stream assumes ownership of the buffer
  if (myStream.put (myCurrentReadBuffer) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::put(): \"%m\", aborting\n")));

      // clean up
    myCurrentReadBuffer->release ();
    myCurrentReadBuffer = NULL;

    return -1;
  } // end IF
  myCurrentReadBuffer = NULL;

  return 0;
}

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
int
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::handle_close (ACE_HANDLE handle_in,
                                                                ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamUDPSocketBase_t::handle_close"));

  switch (mask_in)
  {
    case ACE_Event_Handler::READ_MASK:       // --> socket has been closed
    case ACE_Event_Handler::ALL_EVENTS_MASK: // --> connect failed (e.g. connection refused) /
                                             //     accept failed (e.g. too many connections) /
                                             //     select failed (EBADF see Select_Reactor_T.cpp) /
                                             //     asynch abort
    {
      // step1: wait for all workers within the stream (if any)
      if (myStream.isRunning ())
      {
        myStream.stop ();
        myStream.waitForCompletion ();
      } // end IF

      // step2: purge any pending notifications ?
      // *IMPORTANT NOTE*: if called from a non-reactor context, or when using a
      // a multithreaded reactor, there may still be in-flight notifications
      // being dispatched at this stage, so this just speeds things up a little
      if (!myUserData.useThreadPerConnection)
        if (inherited2::reactor ()->purge_pending_notifications(this,
                                                                ACE_Event_Handler::ALL_EVENTS_MASK) == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Reactor::purge_pending_notifications(%@): \"%m\", continuing\n"),
                      this));

      break;
    }
    case ACE_Event_Handler::EXCEPT_MASK:
      //if (handle_in == ACE_INVALID_HANDLE) // <-- notification has completed (!useThreadPerConnection)
      //  ACE_DEBUG((LM_ERROR,
      //             ACE_TEXT("notification completed, continuing\n")));
      break;
    default:
      // *PORTABILITY*: this isn't entirely portable...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
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

  //// invoke base-class maintenance
  //return inherited2::handle_close (handle_in,
  //                                 mask_in);
  return 0;
}

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
bool
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::collect (StatisticsContainerType& data_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamUDPSocketBase_t::collect"));

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

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
void
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::report () const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamUDPSocketBase_t::report"));

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

template <typename ConfigurationType,
          typename StatisticsContainerType,
          typename StreamType,
          typename SocketType,
          typename SocketHandlerType>
ACE_Message_Block*
RPG_Net_StreamUDPSocketBase_t<ConfigurationType,
                              StatisticsContainerType,
                              StreamType,
                              SocketType,
                              SocketHandlerType>::allocateMessage (const unsigned int& requestedSize_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_StreamUDPSocketBase_t::allocateMessage"));

  // init return value(s)
  ACE_Message_Block* message_out = NULL;

  try
  {
    message_out = static_cast<ACE_Message_Block*> (myUserData.messageAllocator->malloc (requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
                requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_IAllocator::malloc(%u), aborting\n"),
                requestedSize_in));
  } // end IF

  return message_out;
}
