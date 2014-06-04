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

#include "rpg_net_server_listener.h"

#include <ace/OS.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor.h>

#include "rpg_common_macros.h"

#include "rpg_net_server_defines.h"

RPG_Net_Server_Listener::RPG_Net_Server_Listener()
 : inherited(NULL, // use global (default) reactor
             1),   // always accept ALL pending connections
   myIsInitialized(false),
   myIsListening(false),
   myIsOpen(false),
   myListeningPort(RPG_NET_SERVER_DEF_LISTENING_PORT),
	 myUseLoopback(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::RPG_Net_Server_Listener"));

}

RPG_Net_Server_Listener::~RPG_Net_Server_Listener()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::~RPG_Net_Server_Listener"));

  if (myIsOpen)
    close();
}

void
RPG_Net_Server_Listener::init(const unsigned short& listeningPort_in,
                              const bool& useLoopback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::init"));

  // *NOTE*: changes won't become active until the listener is "restarted"...
  myListeningPort = listeningPort_in;
	myUseLoopback = useLoopback_in;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set listening port: %u\n"),
//              myListeningPort));

  myIsInitialized = true;
}

bool
RPG_Net_Server_Listener::isInitialized() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::isInitialized"));

  return myIsInitialized;
}

int
RPG_Net_Server_Listener::handle_accept_error(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::handle_accept_error"));

//  ACE_DEBUG((LM_ERROR,
//             ACE_TEXT("failed to accept connection...\n")));

  inherited::dump();

  // *NOTE*: remain registered with the reactor
  return 0;
}

void
RPG_Net_Server_Listener::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::start"));

  // sanity check(s)
  if (myIsListening)
    return; // nothing to do...

  if (myIsOpen)
  {
    // already open (maybe suspended ?) --> resume listening...
    if (inherited::resume() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Acceptor::resume(): \"%m\", aborting")));
    else
    {
      myIsListening = true;

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("resumed listening...\n")));
    } // end ELSE

    return;
  } // end IF

  // sanity check: configured ?
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized, aborting")));

    return;
  } // end IF

  // not running --> start listening
	ACE_INET_Addr local_sap;
	if (myUseLoopback)
		local_sap.set(myListeningPort, // local SAP
                  // *PORTABILITY*: disambiguation needed under Windows
                  ACE_LOCALHOST,   // hostname
									1,               // encode ?
									AF_INET);        // address family
	else
		local_sap.set(myListeningPort,                     // local SAP
                  // *TODO*: bind to specific interface/address ?
                  static_cast<ACE_UINT32>(INADDR_ANY), // hostname
									1,                                   // encode ?
									0);                                  // map IPv6 to IPv4 ?
	if (inherited::open(local_sap,               // local SAP
											ACE_Reactor::instance(), // corresp. reactor
											ACE_NONBLOCK,            // flags (use non-blocking sockets !)
											//0,                       // flags (default is blocking sockets)
											1,                       // always accept ALL pending connections
											1) == -1)                // try to re-use address
	{
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::open(): \"%m\", aborting")));

    return;
  } // end IF
  else
    myIsOpen = true;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started listening (port: %u)...\n"),
             myListeningPort));

  myIsListening = true;
}

void
RPG_Net_Server_Listener::stop(const bool& lockedAccess_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::stop"));

  ACE_UNUSED_ARG(lockedAccess_in);

  // sanity check(s)
  if (!myIsListening)
    return; // nothing to do...

//  if (inherited::suspend() == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_Acceptor::suspend(): \"%m\", aborting\n")));

//    return;
//  } // end IF
  if (inherited::close() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::close(): \"%m\", aborting\n")));

    // clean up
    myIsListening = false;
    myIsOpen = false;

    return;
  } // end IF
  else
    myIsOpen = false;

  myIsListening = false;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("stopped listening...\n")));
//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("suspended listening...\n")));
}

bool
RPG_Net_Server_Listener::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::isRunning"));

  return myIsListening;
}

void
RPG_Net_Server_Listener::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::dump_state"));

  ACE_TCHAR* buffer = NULL;
  if ((inherited::info(&buffer,
                      RPG_COMMON_BUFSIZE) == -1) ||
      !buffer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::info(): \"%m\", aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%s\n"),
             buffer));

  // clean up
  delete [] buffer;
}
