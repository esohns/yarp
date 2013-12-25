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
 : inherited(ACE_Reactor::instance(), // use global (default) reactor
             1),                      // always accept ALL pending connections
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

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to accept connection...\n")));

  // *TODO*: what else can we do ?
  inherited::dump();

  // *NOTE*: we want to remain registered with the reactor...
  return 0;
}

void
RPG_Net_Server_Listener::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::start"));

  if (myIsListening)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("already listening --> nothing to do, returning\n")));

    return;
  } // end IF

  if (myIsOpen)
  {
    // OK: already open (maybe suspended ?) --> try to resume listening...
    if (resume() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Acceptor::resume(): \"%m\", returning\n")));
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
               ACE_TEXT("not initialized, returning\n")));

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
  if (open(local_sap,               // local SAP
           ACE_Reactor::instance(), // corresp. reactor
           ACE_NONBLOCK,            // flags (use non-blocking sockets !)
           //0,                       // flags (default is blocking sockets)
           1,                       // always accept ALL pending connections
           1) == -1)                // try to re-use address
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::open(): \"%m\", returning\n")));

    return;
  } // end IF

  myIsOpen = true;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started listening (port: %u)...\n"),
             myListeningPort));

  // all is well...
  myIsListening = true;
}

void
RPG_Net_Server_Listener::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Server_Listener::stop"));

  if (!myIsListening)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("not listening --> nothing to do, returning\n")));

    return;
  } // end IF

  // *NOTE*: MUST be open (otherwise there's some logic error somewhere...)
  // OK: already open --> try to suspend listening...
  if (suspend() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::suspend(): \"%m\", returning\n")));
  else
  {
    myIsListening = false;

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("suspended listening...\n")));
  } // end ELSE
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

  // *TODO*: do something meaningful here...
  ACE_ASSERT(false);
}
