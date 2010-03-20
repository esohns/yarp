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
             ACE_Reactor::instance()), // default reactor
   myIsRegistered(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::RPG_Net_SocketHandler_Base"));

}

RPG_Net_SocketHandler_Base::~RPG_Net_SocketHandler_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::~RPG_Net_SocketHandler_Base"));

}

int RPG_Net_SocketHandler_Base::open(void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::open"));

  // call baseclass...
  if (inherited::open(arg_in) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Svc_Handler::open(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return -1;
  } // end IF

  // *IMPORTANT NOTE*: we're registered with the reactor (READ_MASK) at this point...

  if (!RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->registerConnection(this))
  {
    // too many connections...

    // clean up
    if (reactor()->remove_handler(this,
                                  (ACE_Event_Handler::READ_MASK |
                                   ACE_Event_Handler::DONT_CALL)) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::remove_handler(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));
    } // end IF

    return -1;
  } // end IF

  // all is well...
  myIsRegistered = true;

  return 0;
}

int RPG_Net_SocketHandler_Base::handle_close(ACE_HANDLE handle_in,
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

  // *IMPORTANT NOTE*: this is called when:
  // - the client closes the socket --> child handle_xxx() returns -1
  return inherited::handle_close(handle_in,
                                 mask_in);
}

void RPG_Net_SocketHandler_Base::abort()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::abort"));

  // call baseclass... --> will clean everything (including ourselves !) up
  // --> invokes handle_close
  int result = inherited::close(0);
  if (result == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Svc_Handler::close(): \"%s\", returning\n"),
               ACE_OS::strerror(errno)));
  } // end IF
}

void RPG_Net_SocketHandler_Base::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::dump_state"));

  // debug info
  ACE_INET_Addr remoteAddress;
  if (peer().get_remote_addr(remoteAddress) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%s\" --> check implementation, returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("client connection (host: \"%s\", port: %u) --> ID: %d\n"),
             remoteAddress.get_host_name(),
             remoteAddress.get_port_number(),
             peer().get_handle()));
}

const unsigned long RPG_Net_SocketHandler_Base::getID() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SocketHandler_Base::getID"));

  // *IMPORTANT NOTE*: this isn't entirely portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  return get_handle();
#else
  // *TODO*: clean this up !
  return ACE_reinterpret_cast(unsigned long,
                              get_handle());
#endif
}
