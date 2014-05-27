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

#include "rpg_net_server_asynchlistener.h"

#include <ace/Default_Constants.h>
#include <ace/OS.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/POSIX_Asynch_IO.h>

#include "rpg_common_macros.h"

#include "rpg_net_common.h"

#include "rpg_net_server_defines.h"

RPG_Net_Server_AsynchListener::RPG_Net_Server_AsynchListener()
 : //inherited(),
   myIsInitialized(false),
   myIsListening(false),
   myListeningPort(RPG_NET_SERVER_DEF_LISTENING_PORT),
   myUseLoopback(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::RPG_Net_Server_AsynchListener"));

}

RPG_Net_Server_AsynchListener::~RPG_Net_Server_AsynchListener()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::~RPG_Net_Server_AsynchListener"));

  if (myIsListening)
    if (inherited::cancel() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Asynch_Acceptor::cancel(): \"%m\", continuing")));
}

void
RPG_Net_Server_AsynchListener::init(const unsigned short& listeningPort_in,
                                    const bool& useLoopback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::init"));

  // *NOTE*: changes won't become active until the listener is "restarted"...
  myListeningPort = listeningPort_in;
  myUseLoopback = useLoopback_in;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set listening port: %u\n"),
//              myListeningPort));

  myIsInitialized = true;
}

const bool
RPG_Net_Server_AsynchListener::isInitialized() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::isInitialized"));

  return myIsInitialized;
}

void
RPG_Net_Server_AsynchListener::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::start"));

  // sanity check(s)
  if (myIsListening)
    return; // nothing to do...

  // sanity check: configured ?
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, aborting\n")));

    return;
  } // end IF

  // not running --> start listening
  ACE_INET_Addr local_sap;
  if (myUseLoopback)
    local_sap.set(myListeningPort, // port
                  // *PORTABILITY*: needed to disambiguate this under Windows :-(
                  // *TODO*: bind to specific interface/address ?
                  ACE_LOCALHOST,   // hostname
                  1,               // encode ?
                  AF_INET);        // address family
  else
    local_sap.set(myListeningPort,                     // port
                  // *PORTABILITY*: needed to disambiguate this under Windows :-(
                  // *TODO*: bind to specific interface/address ?
                  static_cast<ACE_UINT32>(INADDR_ANY), // hostname
                  1,                                   // encode ?
                  0);                                  // map ?
  if (inherited::open(local_sap,                  // local SAP
                      0,                          // bytes_to_read
                      1,                          // pass_addresses ?
                      ACE_DEFAULT_ASYNCH_BACKLOG, // backlog
                      1,                          // reuse address ?
                      NULL,                       // proactor (use default)
                      true,                       // validate new connections ?
                      1,                          // reissue_accept ?
                      -1) == -1)                  // number of initial accepts
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Acceptor::open(%u): \"%m\", returning\n"),
               myListeningPort));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started listening (port: %u)...\n"),
             myListeningPort));

  // all is well...
  myIsListening = true;
}

void
RPG_Net_Server_AsynchListener::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::stop"));

  if (!myIsListening)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("not listening --> nothing to do, returning\n")));

    return;
  } // end IF

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  ACE_POSIX_Asynch_Accept* accept =
      dynamic_cast<ACE_POSIX_Asynch_Accept*>(inherited::asynch_accept().implementation());
  if (!accept)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<ACE_POSIX_Asynch_Accept> failed, aborting\n")));

    return;
  }
  if (accept->close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_POSIX_Asynch_Accept::close(): \"%m\", continuing\n")));
#else
  if (inherited::cancel() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Acceptor::cancel(): \"%m\", continuing\n")));
	if (ACE_OS::closesocket(inherited::handle()) == -1)
		ACE_DEBUG((LM_ERROR,
		           ACE_TEXT("failed to ACE_OS::closesocket(): \"%m\", continuing\n")));
	inherited::handle(ACE_INVALID_HANDLE);
	if (false);
#endif
  else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("stopped listening (port: %u)...\n"),
               myListeningPort));

  myIsListening = false;
}

bool
RPG_Net_Server_AsynchListener::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::isRunning"));

  return myIsListening;
}

void
RPG_Net_Server_AsynchListener::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::dump_state"));

  // *TODO*: do something meaningful here...
  ACE_ASSERT(false);
}

RPG_Net_AsynchStreamHandler_t*
RPG_Net_Server_AsynchListener::make_handler(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::make_handler"));

  // init return value(s)
  RPG_Net_AsynchStreamHandler_t* handler_out = NULL;

  // default behavior
  ACE_NEW_NORETURN(handler_out,
                   RPG_Net_AsynchStreamHandler_t(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()));

  return handler_out;
}

int
RPG_Net_Server_AsynchListener::validate_connection(const ACE_Asynch_Accept::Result& result_in,
                                                   const ACE_INET_Addr& remoteSAP_in,
                                                   const ACE_INET_Addr& localSAP_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::validate_connection"));

	// success ?
	if (result_in.success() == 0)
	{
		// debug info
    ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
    ACE_OS::memset(buffer, 0, sizeof(buffer));
    if (remoteSAP_in.addr_to_string(buffer, sizeof(buffer)) == -1)
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
		ACE_DEBUG((LM_ERROR,
			         ACE_TEXT("failed to ACE_Asynch_Acceptor::accept(\"%s\"): \"%s\", aborting\n"),
               buffer,
							 ACE_OS::strerror(result_in.error())));
	} // end IF

	return ((result_in.success() == 1) ? 0 : -1);
}

int
RPG_Net_Server_AsynchListener::should_reissue_accept()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_AsynchListener::should_reissue_accept"));

  // default behavior: delegate to baseclass
  return inherited::should_reissue_accept();
}
