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
#include <ace/INET_Addr.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_iconnectionmanager.h"

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::RPG_Net_SocketHandlerBase(MANAGER_T* manager_in)
 : inherited(NULL,                     // no specific thread manager
             NULL,                     // no specific message queue
             ACE_Reactor::instance()), // default reactor
   myNotificationStrategy(ACE_Reactor::instance(),       // reactor
                          this,                          // event handler
                          ACE_Event_Handler::WRITE_MASK), // handle output only
   myManager(manager_in),
//    myUserData(),
   myIsRegistered(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::RPG_Net_SocketHandlerBase"));

  // sanity check(s)
//  ACE_ASSERT(myManager);

  //  // register notification strategy ?
  //  inherited::msg_queue()->notification_strategy(&myNotificationStrategy);

  // init user data
  ACE_OS::memset(&myUserData, 0, sizeof(ConfigType));

  if (myManager)
  {
    // (try to) get user data from the connection manager...
    try
    {
      myManager->getConfig(myUserData);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::getConfig(), continuing\n")));
    }
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::~RPG_Net_SocketHandlerBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::~RPG_Net_SocketHandlerBase"));

  if (myManager && myIsRegistered)
  { // (try to) de-register with connection manager
    try
    {
      myManager->deregisterConnection(this);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::deregisterConnection(), continuing\n")));
    }
  } // end IF

//  // *IMPORTANT NOTE*: the streamed socket handler uses the stream head modules' queue
//  // --> this happens too late, as the stream/queue will have been deleted by now...
//  if (reactor()->purge_pending_notifications(this, ACE_Event_Handler::ALL_EVENTS_MASK) == -1)
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_Reactor::purge_pending_notifications(%@): \"%m\", continuing\n"),
//               this));
}

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::open"));

  // sanity check(s)
//  if (!myManager)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("invalid connection manager, aborting\n")));

//    return -1;
//  } // end IF

  // step1: tweak socket
  // *TODO*: there is a design glitch here: this class SHOULD NOT make
  // assumptions about ConfigType !
  if (myUserData.socketBufferSize)
    if (!RPG_Net_Common_Tools::setSocketBuffer(peer_.get_handle(),
                                               SO_RCVBUF,
                                               myUserData.socketBufferSize))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setSocketBuffer(%u) for %u, aborting\n"),
                 myUserData.socketBufferSize,
                 id()));

    return -1;
  } // end IF
  if (!RPG_Net_Common_Tools::setNoDelay(peer_.get_handle(),
                                        RPG_NET_DEF_SOCK_NODELAY))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to setNoDelay(%u, %s), aborting\n"),
               id(),
               (RPG_NET_DEF_SOCK_NODELAY ? ACE_TEXT("true")
							                           : ACE_TEXT("false"))));

    return -1;
  } // end IF
  if (!RPG_Net_Common_Tools::setKeepAlive(peer_.get_handle(),
                                          RPG_NET_DEF_SOCK_KEEPALIVE))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to setLinger(%u, %s), aborting\n"),
               id(),
               (RPG_NET_DEF_SOCK_LINGER ? ACE_TEXT("true")
							                          : ACE_TEXT("false"))));

    return -1;
  } // end IF
  if (!RPG_Net_Common_Tools::setLinger(peer_.get_handle(),
                                       RPG_NET_DEF_SOCK_LINGER))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to setLinger(%u, %s), aborting\n"),
               id(),
               ((RPG_NET_DEF_SOCK_LINGER > 0) ? ACE_TEXT("true")
							                                : ACE_TEXT("false"))));

    return -1;
  } // end IF

  // register with the reactor...
  if (inherited::open(arg_in) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Svc_Handler::open(%u): \"%m\", aborting\n"),
               id()));

    return -1;
  } // end IF

  // *NOTE*: we're registered with the reactor (READ_MASK) at this point

//   // ...register for writes (WRITE_MASK) as well
//   if (reactor()->register_handler(this,
//                                   ACE_Event_Handler::WRITE_MASK) == -1)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_Reactor::register_handler(WRITE_MASK): \"%m\", aborting\n")));
//
//     return -1;
//   } // end IF

//  if (myManager)
//  {
//    // (try to) register with the connection manager...
//    // *WARNING*: as we register BEFORE the connection has fully opened, there
//    // is a small window for races...
//    try
//    {
//      myIsRegistered = myManager->registerConnection(this);
//    }
//    catch (...)
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
//    }
//  } // end IF

  return 0;
}

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::handle_close(ACE_HANDLE handle_in,
                                                                 ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::handle_close"));

  switch (mask_in)
  {
		case ACE_Event_Handler::READ_MASK:       // --> socket has been closed
			break;
    case ACE_Event_Handler::EXCEPT_MASK:
      //if (handle_in == ACE_INVALID_HANDLE) // <-- notification has completed (!useThreadPerConnection)
      //  ACE_DEBUG((LM_ERROR,
      //             ACE_TEXT("notification completed, continuing\n")));
      break;
		case ACE_Event_Handler::ALL_EVENTS_MASK: // --> accept failed (e.g. too many connections) ?
    {
			// sanity check: accept failed ?
			if ((mask_in == ACE_Event_Handler::ALL_EVENTS_MASK) &&
				  (handle_in != ACE_INVALID_HANDLE))
			{
      // *PORTABILITY*: this isn't entirely portable...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("handle_close called for unknown reasons (handle: %@, mask: %u) --> check implementation !, continuing\n"),
								   handle_in,
								   mask_in));
#else
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("handle_close called for unknown reasons (handle: %d, mask: %u) --> check implementation !, continuing\n"),
								   handle_in,
								   mask_in));
#endif
			} // end IF

			inherited::shutdown();

      break;
    }
    default:
      // *PORTABILITY*: this isn't entirely portable...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("handle_close called for unknown reasons (handle: %@, mask: %u) --> check implementation !, continuing\n"),
								 handle_in,
								 mask_in));
#else
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("handle_close called for unknown reasons (handle: %d, mask: %u) --> check implementation !, continuing\n"),
								 handle_in,
								 mask_in));
#endif
      break;
  } // end SWITCH

  // *NOTE*: this MAY "delete this"...
  return inherited::handle_close(handle_in,
                                 mask_in);
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::info(ACE_HANDLE& handle_out,
                                                         ACE_INET_Addr& localSAP_out,
                                                         ACE_INET_Addr& remoteSAP_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::info"));

  handle_out = peer_.get_handle();
  if (inherited::peer_.get_local_addr(localSAP_out) == -1)
  {
    // *NOTE*: socket already closed ? --> not an error
    int error = ACE_OS::last_error();
    if ((error != EBADF) &&
			  (error != ENOTSOCK))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::get_local_addr(): \"%m\", continuing\n")));
  } // end IF
  if (inherited::peer_.get_remote_addr(remoteSAP_out) == -1)
  {
    // *NOTE*: socket already closed ? --> not an error
    int error = ACE_OS::last_error();
    if ((error != ENOTCONN) &&
        (error != EBADF) &&
				(error != ENOTSOCK))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%m\", continuing\n")));
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::abort()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::abort"));

  // simply close the underlying socket
  // *IMPORTANT NOTE*: the reactor cleans everything up...
  int result = peer_.close();
  if (result == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_IO::close(): \"%m\", continuing\n")));
}

template <typename ConfigType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::id() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::id"));

  // *PORTABILITY*: this isn't entirely portable...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  return peer_.get_handle();
#else
  return reinterpret_cast<unsigned int>(peer_.get_handle());
#endif
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::increase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::increase"));

	// *TODO*: this doesn't work !
	inherited::add_reference();
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::decrease()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::decrease"));

	// *TODO*: this doesn't work !
	inherited::remove_reference();
}

template <typename ConfigType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::count()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::count"));

	// *TODO*: implement this !
  ACE_ASSERT(false);

	return 0;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::wait_zero()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::wait_zero"));

	// *TODO*: implement this !
  ACE_ASSERT(false);
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::dump_state"));

  ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
  ACE_OS::memset(buffer, 0, sizeof(buffer));
  std::string localAddress;
  ACE_INET_Addr address;
  if (inherited::peer_.get_local_addr(address) == -1)
  {
    // *NOTE*: socket already closed ? --> not an error
    int error = ACE_OS::last_error();
    if ((error != EBADF) &&
			  (error != ENOTSOCK))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::get_local_addr(): \"%m\", continuing\n")));
  } // end IF
  else if (address.addr_to_string(buffer,
		                              sizeof(buffer)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  localAddress = buffer;

  ACE_OS::memset(buffer, 0, sizeof(buffer));
  if (inherited::peer_.get_remote_addr(address) == -1)
  {
    // *NOTE*: socket already closed ? --> not an error
    int error = ACE_OS::last_error();
    if ((error != ENOTCONN) &&
        (error != EBADF) &&
				(error != ENOTSOCK))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%m\", continuing\n")));
  } // end IF
  else if (address.addr_to_string(buffer,
		                              sizeof(buffer)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection [%u]: (\"%s\") <--> (\"%s\")\n"),
             id(),
             localAddress.c_str(),
             buffer));
}
