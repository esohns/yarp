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

#include "rpg_net_asynchlistener.h"

#include "rpg_net_defines.h"
#include "rpg_net_common.h"

#include <ace/OS.h>
#include <ace/INET_Addr.h>

RPG_Net_AsynchListener::RPG_Net_AsynchListener()
 : //inherited(),
   myIsInitialized(false),
   myIsListening(false),
   myListeningPort(RPG_NET_DEF_LISTENING_PORT)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::RPG_Net_AsynchListener"));

}

RPG_Net_AsynchListener::~RPG_Net_AsynchListener()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::~RPG_Net_AsynchListener"));

}

void
RPG_Net_AsynchListener::init(const unsigned short& listeningPort_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::init"));

  // *NOTE*: changes won't become active until the listener is "restarted"...
  myListeningPort = listeningPort_in;

  // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set listening port: %u\n"),
//              myListeningPort));

  myIsInitialized = true;
}

const bool
RPG_Net_AsynchListener::isInitialized() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::isInitialized"));

  return myIsInitialized;
}

void
RPG_Net_AsynchListener::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::start"));

  if (myIsListening)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("already listening --> nothing to do, returning\n")));

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
  ACE_INET_Addr local_SAP(myListeningPort, // local SAP
                          // *PORTABILITY*: needed to disambiguate this under Windows :-(
                          // *TODO*: bind to specific interface/address ?
                          static_cast<ACE_UINT32>(INADDR_ANY));
  if (inherited::open(local_SAP,                  // local SAP
                      0,                          // bytes_to_read
                      1,                          // pass_addresses
                      ACE_DEFAULT_ASYNCH_BACKLOG, // backlog
                      1,                          // reuse_addr
                      NULL,                       // proactor
                      true,                       // validate_new_connection
                      1,                          // reissue_accept
                      -1) == -1)                  // number_of_initial_accepts
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
RPG_Net_AsynchListener::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::stop"));

  if (!myIsListening)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("not listening --> nothing to do, returning\n")));

    return;
  } // end IF

  if (inherited::cancel() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Acceptor::cancel(): \"%m\", continuing\n")));
  else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("stopped listening (port: %u)...\n"),
               myListeningPort));

  myIsListening = false;
}

bool
RPG_Net_AsynchListener::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::isRunning"));

  return myIsListening;
}

void
RPG_Net_AsynchListener::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::dump_state"));

  // *TODO*: do something meaningful here...
  ACE_ASSERT(false);
}

RPG_Net_StreamHandler_t*
RPG_Net_AsynchListener::make_handler(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchListener::make_handler"));

  // init return value(s)
  RPG_Net_StreamHandler_t* handler_out = NULL;

  // default behavior
  ACE_NEW_NORETURN(handler_out,
                   RPG_Net_StreamHandler_t(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()));

  return handler_out;
}
