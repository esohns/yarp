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

#include "rpg_net_listener.h"

#include <ace/OS.h>
#include <ace/Reactor.h>

RPG_Net_Listener::RPG_Net_Listener()
  : inherited(ACE_Reactor::instance(), // use global reactor
              0),                      // flags (*TODO*: ACE_NONBLOCK ?)
    myIsInitialized(false),
    myIsListening(false),
    myIsOpen(false),
    myListeningPort(0)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::RPG_Net_Listener"));

}

RPG_Net_Listener::~RPG_Net_Listener()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::~RPG_Net_Listener"));

  if (myIsOpen)
  {
    close();
  } // end IF
}

void RPG_Net_Listener::init(const unsigned short& listeningPort_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::init"));

  // *IMPORTANT NOTE*: changes won't become active until the listener is
  // "restarted"...
  myListeningPort = listeningPort_in;

  // start maintenance task
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("set listening port: %u\n"),
             myListeningPort));

  myIsInitialized = true;
}

const bool RPG_Net_Listener::isInitialized() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::isInitialized"));

  return myIsInitialized;
}

void RPG_Net_Listener::start()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::start"));

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
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Acceptor::resume(): \"%s\", returning\n"),
                 ACE_OS::strerror(errno)));
    } // end IF
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
  if (open(ACE_INET_Addr(myListeningPort, // local SAP
           // *PORTABILITY*: needed to disambiguate this under Windows :-(
                         ACE_static_cast(ACE_UINT32, INADDR_ANY)),
           ACE_Reactor::instance(),       // corresp. reactor
           0,                             // flags (*TODO*: ACE_NONBLOCK ?)
           1,                             // accept all pending connections
           1) == -1)                      // try to re-use address
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::open(): \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end IF

  myIsOpen = true;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started listening...\n")));

  // all is well...
  myIsListening = true;
}

void RPG_Net_Listener::stop()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::stop"));

  if (!myIsListening)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("not listening --> nothing to do, returning\n")));

    return;
  } // end IF

  // *IMPORTANT NOTE*: MUST be open (otherwise there's some logic
  // error somewhere...)
  // OK: already open --> try to suspend listening...
  if (suspend() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Acceptor::suspend(): \"%s\", returning\n"),
               ACE_OS::strerror(errno)));
  } // end IF
  else
  {
    myIsListening = false;

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("suspended listening...\n")));
  } // end ELSE
}

void RPG_Net_Listener::dump()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::dump"));

  // *TODO*: do something meaningful here...
}

const bool RPG_Net_Listener::isRunning()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Listener::isRunning"));

  return myIsListening;
}
