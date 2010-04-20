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

#include "net_client_signalhandler.h"

#include <rpg_net_connection_manager.h>

#include <ace/Reactor.h>

Net_Client_SignalHandler::Net_Client_SignalHandler(const std::string& serverHostname_in,
                                                   const unsigned short& serverPort_in,
                                                   RPG_Net_Client_Connector* connector_in)
 : inherited(ACE_Reactor::instance(),         // corresp. reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myPeerAddress(serverPort_in,
                 serverHostname_in.c_str()),
   myConnector(connector_in)
{
  ACE_TRACE(ACE_TEXT("Net_Client_SignalHandler::Net_Client_SignalHandler"));

}

Net_Client_SignalHandler::~Net_Client_SignalHandler()
{
  ACE_TRACE(ACE_TEXT("Net_Client_SignalHandler::~Net_Client_SignalHandler"));

}

int
Net_Client_SignalHandler::handle_signal(int signal_in,
                                        siginfo_t* info_in,
                                        ucontext_t* context_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::handle_signal"));

  ACE_UNUSED_ARG(context_in);

  // debug info
  if (info_in == NULL)
  {
    // *PORTABILITY*: tracing in a signal handler context is not portable
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%D: received [%S], but no siginfo_t was available, continuing\n"),
               signal_in));
  } // end IF
  else
  {
    // collect some context information...
    std::string information;
    RPG_Net_Common_Tools::retrieveSignalInfo(signal_in,
                                             *info_in,
                                             context_in,
                                             information);

//     // *PORTABILITY*: tracing in a signal handler context is not portable
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("%D: received [%S]: %s\n"),
//                signal_in,
//                information.c_str()));
  } // end ELSE

  bool stop_reactor = false;
  bool connect_to_server = false;
  bool abort_oldest = false;
  switch (signal_in)
  {
    case SIGINT:
    case SIGTERM:
    // *PORTABILITY*: this isn't portable: on Windows SIGQUIT and SIGHUP are not defined,
    // so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGHUP:
    case SIGQUIT:
#else
    case SIGBREAK:
    case SIGABRT:
#endif
    {
//       // *PORTABILITY*: tracing in a signal handler context is not portable
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("shutting down...\n")));

      // shutdown...
      stop_reactor = true;

      break;
    }
    case SIGUSR1:
    {
      // (try to) connect...
      connect_to_server = true;

      break;
    }
    case SIGUSR2:
    {
      // (try to) abort oldest connection...
      abort_oldest = true;

      break;
    }
    default:
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown signal: \"%S\", continuing\n"),
                 signal_in));

      break;
    }
  } // end SWITCH

  // ...abort ?
  if (abort_oldest)
  {
    // release an existing connection...
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortOldestConnection();
  } // end IF

  // ...connect ?
  if (connect_to_server)
  {
    RPG_Net_Client_SocketHandler* handler = NULL;
    if (myConnector->connect(handler,                     // service handler
                             myPeerAddress/*,              // remote SAP
                             ACE_Synch_Options::defaults, // synch options
                             ACE_INET_Addr::sap_any,      // local SAP
                             0,                           // try to re-use address (SO_REUSEADDR)
                             O_RDWR,                      // flags
                             0*/) == -1)                  // perms
    {
      // debug info
      ACE_TCHAR buf[BUFSIZ];
      ACE_OS::memset(buf,
                     0,
                     (BUFSIZ * sizeof(ACE_TCHAR)));
      if (myPeerAddress.addr_to_string(buf,
                                       (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
      {
        // *PORTABILITY*: tracing in a signal handler context is not portable
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
      } // end IF
      // *PORTABILITY*: tracing in a signal handler context is not portable
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Connector::connect(%s): \"%m\", continuing\n"),
                 buf));

      // release an existing connection, maybe that helps...
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortOldestConnection();
    } // end IF
  } // end IF

  // ...shutdown ?
  if (stop_reactor)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers...
    // - break out of any (blocking) calls
    // --> (try to) terminate in a well-behaved manner

    // stop reactor
    if (reactor()->end_event_loop() == -1)
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
    } // end IF

    // de-register from the reactor...
    return -1;
  } // end IF

  return 0;
}