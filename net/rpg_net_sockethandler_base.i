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
#include "rpg_net_common_tools.h"
#include "rpg_net_iconnectionmanager.h"

#include <rpg_common_macros.h>

#include <ace/Reactor.h>
#include <ace/INET_Addr.h>

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::RPG_Net_SocketHandlerBase(RPG_Net_IConnectionManager<ConfigType,
                                                                                                         StatisticsContainerType>* manager_in)
 : inherited(NULL,                     // no specific thread manager
             NULL,                     // no specific message queue
             ACE_Reactor::instance()), // default reactor
//    myUserData(),
   myIsInitialized(false),
   myID(0),
   myIsRegistered(false),
   myManager(manager_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::RPG_Net_SocketHandlerBase"));

//   // init user data
//   ACE_OS::memset(&myUserData,
//                  0,
//                  sizeof(ConfigType));

  if (myManager)
  {
    // (try to) register with the connection manager...
    // *NOTE*: we do it here because we WANT to init() myUserData early...
    // *WARNING*: as we register BEFORE the connection has fully opened, there
    // may be a small window for races...
    try
    {
      myIsRegistered = myManager->registerConnection(this);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
    }
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::~RPG_Net_SocketHandlerBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::~RPG_Net_SocketHandlerBase"));

  if (myManager)
  { // (try to) de-register with connection manager
    if (myIsRegistered)
    {
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
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::open"));

  // sanity check(s)
  if (!myIsRegistered)
  {
    // too many connections...
    ACE_OS::last_error(EBUSY);

//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to register connection (ID: %u), aborting\n"),
//                getID()));

    // acceptor/connector will invoke close() --> handle_close()
    return -1;
  } // end IF
  ACE_ASSERT(myIsInitialized);

  // step1: tweak socket
  // *TODO*: assumptions about ConfigType ?!?: clearly a design glitch
  // --> implement higher up !
  if (myUserData.socketBufferSize)
  {
    if (!RPG_Net_Common_Tools::setSocketBuffer(get_handle(),
                                               SO_RCVBUF,
                                               myUserData.socketBufferSize))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setSocketBuffer(%u) for %u, aborting\n"),
                 myUserData.socketBufferSize,
                 get_handle()));

      // reactor will invoke close() --> handle_close()
      return -1;
    } // end IF
  } // end IF
  if (!RPG_Net_Common_Tools::setNoDelay(get_handle(),
                                        RPG_NET_DEF_SOCK_NODELAY))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to setNoDelay(%u, %s), aborting\n"),
               get_handle(),
               (RPG_NET_DEF_SOCK_NODELAY ? ACE_TEXT("true") : ACE_TEXT("false"))));

      // reactor will invoke handle_close()
    return -1;
  } // end IF

  // register with the reactor...
  if (inherited::open(arg_in) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Svc_Handler::open(): \"%m\", aborting\n")));

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

//   if (!myIsRegistered)
//   {
//     // too many connections...
//
//     clean up
//     if (reactor()->remove_handler(this,
//                                   (ACE_Event_Handler::ALL_EVENTS_MASK |
//                                    ACE_Event_Handler::DONT_CALL)) == -1)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Reactor::remove_handler(): \"%s\", aborting\n"),
//                  ACE_OS::strerror(ACE_OS::last_error())));
//     } // end IF
//
//     return -1;
//   } // end IF

  return 0;
}

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::close"));
  // [*NOTE*: hereby we override the default behavior of a ACE_Svc_Handler,
  // which would call handle_close() AGAIN]

  // *NOTE*: this method will be invoked
  // - by the worker which calls this after returning from svc()
  //    --> in this case, this should be a NOP (triggered from handle_close(),
  //        which was invoked by the reactor) - we override the default
  //        behavior of a ACE_Svc_Handler, which would call handle_close() AGAIN
  // - by the connector/acceptor when open() fails (e.g. too many connections !)
  //    --> in this case, we WANT the default behavior (call handle_close())
  // - by an external thread closing down the connection (see abort())
  //    --> close the socket !

  switch (arg_in)
  {
    // called by:
    // - any worker from ACE_Task_Base on clean-up
    // - acceptor/connector if there are too many connections (i.e. open() returned -1)
    case 0:
    {
      // check specifically for the first case...
      if (ACE_OS::thr_equal(ACE_Thread::self(), last_thread()))
      {
//       if (module())
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("\"%s\" worker thread (ID: %t) leaving...\n"),
//                    ACE_TEXT_ALWAYS_CHAR(name())));
//       else
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("worker thread (ID: %t) leaving...\n")));

        // don't do anything...
        break;
      } // end IF

      // too many connections: invoke inherited default behavior
      // --> close() --> handle_close() --> ... --> delete "this"
      // --> simply fall through to the next case
    }
    // called by external thread wanting to close our connection (see abort())
    case 1:
    {
      // *NOTE*: this is NOT the elegant way to go about "aborting" a connection
      // (even though it works), as it will confuse the reactor
      // --> simply call handle_close instead...
//       int result = inherited::peer_.close();
//       if (result == -1)
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to ACE_SOCK_Stream::close(): \"%m\", returning\n")));
      return inherited::close();
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument: %u, returning\n"),
                 arg_in));

      // what else can we do ?
      break;
    }
  } // end SWITCH

  return 0;
}

// template <typename ConfigType,
//           typename StatisticsContainerType>
// int
// RPG_Net_SocketHandlerBase<ConfigType,
//                           StatisticsContainerType>::handle_close(ACE_HANDLE handle_in,
//                                                                  ACE_Reactor_Mask mask_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::handle_close"));
//
// //   // de-register with connection manager
// //   // *NOTE*: we do it here, while our handle is still "valid"...
// //   if (myIsRegistered)
// //   {
// //     RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->deregisterConnection(getID());
// //
// //     // remember this...
// //     myIsRegistered = false;
// //   } // end IF
//
//   // *NOTE*: called when:
//   // - the client closes the socket --> child handle_xxx() returns -1
//   // - we reject the connection (too many open)
//   // *NOTE*: will delete "this" in an ordered way...
//   return inherited::handle_close(handle_in,
//                                  mask_in);
// }

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::init(const ConfigType& userData_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::init"));

  myUserData = userData_in;
  myIsInitialized = true;
}

// const bool
// RPG_Net_SocketHandlerBase::isRegistered() const
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::isRegistered"));
//
//   return myIsRegistered;
// }

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::abort()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::abort"));

  // call baseclass - will clean everything (including ourselves !) up
  // --> triggers handle_close()
  int result = close(1);
  if (result == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_SocketHandlerBase::close(1): \"%m\", continuing\n")));
}

template <typename ConfigType,
          typename StatisticsContainerType>
const unsigned long
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::getID() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::getID"));

  // *NOTE*: this isn't entirely portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  return get_handle();
#else
  // *TODO*: clean this up !
  return ACE_reinterpret_cast(unsigned long,
                              get_handle());
#endif
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_SocketHandlerBase<ConfigType,
                          StatisticsContainerType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::dump_state"));

  // debug info
  ACE_TCHAR buf[BUFSIZ];
  ACE_OS::memset(buf,
                 0,
                 (BUFSIZ * sizeof(ACE_TCHAR)));
  std::string localAddress;
  ACE_INET_Addr address;
  if (peer().get_local_addr(address) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Stream::get_local_addr(): \"%m\", aborting\n")));

    return;
  } // end IF
  else if (address.addr_to_string(buf, (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", aborting\n")));

    return;
  } // end IF
  localAddress = buf;

  ACE_OS::memset(buf,
                 0,
                 (BUFSIZ * sizeof(ACE_TCHAR)));
  if (peer().get_remote_addr(address) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%m\", aborting\n")));

    return;
  } // end IF
  else if (address.addr_to_string(buf, (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection [%u]: (\"%s\") <--> (\"%s\")\n"),
             getID(),
             localAddress.c_str(),
             buf));
}
