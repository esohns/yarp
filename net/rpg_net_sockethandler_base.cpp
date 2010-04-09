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

#include "rpg_net_sockethandler_base.h"

#include "rpg_net_connection_manager.h"

#include <ace/OS.h>
#include <ace/Reactor.h>

RPG_Net_SocketHandler_Base::RPG_Net_SocketHandler_Base()
 : inherited(NULL,                     // no specific thread manager
             NULL,                     // no specific message queue
             ACE_Reactor::instance())//, // default reactor
//    myUserData(),
//    myIsRegistered(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::RPG_Net_SocketHandler_Base"));

  // init user data
  ACE_OS::memset(&myUserData,
                 0,
                 sizeof(RPG_Net_ConfigPOD));

  // (try to) register with the connection manager...
  // *NOTE*: this SHOULD init() myUserData
  // *WARNING*: as we register BEFORE the connection has fully opened, there
  // may be a small window for races (i.e. problems during shutdown)...
  myIsRegistered = RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->registerConnection(this);
}

RPG_Net_SocketHandler_Base::~RPG_Net_SocketHandler_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::~RPG_Net_SocketHandler_Base"));

}

int
RPG_Net_SocketHandler_Base::open(void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::open"));

  // sanity check
  if (!myIsRegistered)
  {
    // too many connections...
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to register connection (ID: %u), aborting\n"),
//                getID()));

    return -1;
  } // end IF

  // call baseclass...
  if (inherited::open(arg_in) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Svc_Handler::open(): \"%p\", aborting\n")));

    return -1;
  } // end IF

  // *NOTE*: we're registered with the reactor (READ_MASK) at this point

  // ...register for writes (WRITE_MASK) as well
  if (reactor()->register_handler(this,
                                  ACE_Event_Handler::WRITE_MASK) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::register_handler(WRITE_MASK): \"%p\", aborting\n")));

    return -1;
  } // end IF

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

int
RPG_Net_SocketHandler_Base::handle_close(ACE_HANDLE handle_in,
                                         ACE_Reactor_Mask mask_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::handle_close"));

  // de-register with connection manager
  if (myIsRegistered)
  {
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->deregisterConnection(getID());

    // remember this...
    myIsRegistered = false;
  } // end IF

  // *NOTE*: this is called when:
  // - the client closes the socket --> child handle_xxx() returns -1
  // - we reject the connection (too many open)
  // *NOTE*: this will destroy ourself in an ordered way...
  return inherited::handle_close(handle_in,
                                 mask_in);
}

void
RPG_Net_SocketHandler_Base::init(const RPG_Net_ConfigPOD& userData_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::init"));

  myUserData = userData_in;
}

const bool
RPG_Net_SocketHandler_Base::isRegistered() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::isRegistered"));

  return myIsRegistered;
}

void
RPG_Net_SocketHandler_Base::abort()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::abort"));

  // call baseclass - will clean everything (including ourselves !) up
  // --> invokes handle_close
  int result = inherited::close(0);
  if (result == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Svc_Handler::close(0): \"%p\", returning\n")));
  } // end IF
}

const unsigned long
RPG_Net_SocketHandler_Base::getID() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::getID"));

  // *NOTE*: this isn't entirely portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  return get_handle();
#else
  // *TODO*: clean this up !
  return ACE_reinterpret_cast(unsigned long,
                              get_handle());
#endif
}

void
RPG_Net_SocketHandler_Base::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::dump_state"));

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
               ACE_TEXT("failed to ACE_SOCK_Stream::get_local_addr(): \"%p\", aborting\n")));

    return;
  } // end IF
  else if (address.addr_to_string(buf, (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%p\", aborting\n")));

    return;
  } // end IF
  localAddress = buf;

  ACE_OS::memset(buf,
                 0,
                 (BUFSIZ * sizeof(ACE_TCHAR)));
  if (peer().get_remote_addr(address) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%p\", aborting\n")));

    return;
  } // end IF
  else if (address.addr_to_string(buf, (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%p\", aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection [%u]: (\"%s\") <--> (\"%s\")\n"),
             getID(),
             localAddress.c_str(),
             buf));
}
