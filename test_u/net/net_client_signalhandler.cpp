/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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

#include "net_client_signalhandler.h"

#include "common_timer_manager.h"
#include "common_tools.h"

#include "common_ui_gtk_manager.h"

#include "net_connection_manager_common.h"

#include "rpg_common_macros.h"

#include "rpg_net_common_tools.h"

Net_Client_SignalHandler::Net_Client_SignalHandler (long actionTimerID_in,
                                                    const ACE_INET_Addr& peerSAP_in,
                                                    Net_Client_IConnector* connector_in,
                                                    bool useReactor_in)
 : inherited (this,          // event handler handle
              useReactor_in) // use reactor ?
 , myActionTimerID (actionTimerID_in)
 , myPeerAddress (peerSAP_in)
 , myConnector (connector_in)
 , myUseReactor (useReactor_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_SignalHandler::Net_Client_SignalHandler"));

}

Net_Client_SignalHandler::~Net_Client_SignalHandler ()
{
  RPG_TRACE (ACE_TEXT ("Net_Client_SignalHandler::~Net_Client_SignalHandler"));

}

bool
Net_Client_SignalHandler::handleSignal (int signal_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_SignalHandler::handleSignal"));

  bool stop_event_dispatching = false;
  bool connect = false;
  bool abort = false;
  switch (signal_in)
  {
    case SIGINT:
// *PORTABILITY*: on Windows SIGQUIT is not defined
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGQUIT:
#endif
    {
      //ACE_DEBUG((LM_DEBUG,
      //           ACE_TEXT("shutting down...\n")));

      // shutdown...
      stop_event_dispatching = true;

      break;
    }
// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGUSR1:
#else
    case SIGBREAK:
#endif
    {
      // (try to) connect...
      connect = true;

      break;
    }
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGHUP:
    case SIGUSR2:
#endif
    case SIGTERM:
    {
      // (try to) abort a connection...
      abort = true;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid/unknown signal: \"%S\", aborting\n"),
                  signal_in));
      return false;
    }
  } // end SWITCH

  // ...abort ?
  if (abort)
  {
    // release an existing connection...
    NET_TCPCONNECTIONMANAGER_SINGLETON::instance ()->abortOldestConnection ();
  } // end IF

  // ...connect ?
  if (connect)
  {
    try
    {
      myConnector->connect (myPeerAddress);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Net_IConnector::connect(), aborting\n")));
      return false;
    }
  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers...
    // --> (try to) terminate in a well-behaved manner

    // step1: stop all open connections

    // stop action timer (might spawn new connections otherwise)
    if (myActionTimerID >= 0)
    {
      const void* act = NULL;
      if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (myActionTimerID,
                                                              &act) <= 0)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", aborting\n"),
                    myActionTimerID));

        // clean up
        myActionTimerID = -1;

        return false;
      } // end IF

      // clean up
      myActionTimerID = -1;
    } // end IF
    try
    {
      myConnector->abort ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_Client_IConnector_t::abort(), aborting\n")));
      return false;
    }
    NET_TCPCONNECTIONMANAGER_SINGLETON::instance ()->stop ();
    NET_TCPCONNECTIONMANAGER_SINGLETON::instance ()->abortConnections ();
    // *IMPORTANT NOTE*: as long as connections are inactive (i.e. events are
    // dispatched by reactor thread(s), there is no real reason to wait here)
    //RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

    // step2: stop GTK event dispatch
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();

    // step3: stop reactor (&& proactor, if applicable)
    Common_Tools::finalizeEventDispatch (true,          // stop reactor ?
                                         !myUseReactor, // stop proactor ?
                                         -1);           // group ID (--> don't block !)
  } // end IF

  return true;
}
