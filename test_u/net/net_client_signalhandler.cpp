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

#include "rpg_net_common_tools.h"

#include "rpg_common_defines.h"

#include <ace/Reactor.h>
#include <ace/Proactor.h>

Net_Client_SignalHandler::Net_Client_SignalHandler(const std::string& serverHostname_in,
                                                   const unsigned short& serverPort_in,
                                                   RPG_Net_Client_Connector* connector_in)
 : inherited(ACE_Reactor::instance(),         // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myPeerAddress(serverPort_in,
                 serverHostname_in.c_str()),
   myConnector(connector_in),
   myAsynchConnector(NULL)
{
  RPG_TRACE(ACE_TEXT("Net_Client_SignalHandler::Net_Client_SignalHandler"));

}

Net_Client_SignalHandler::Net_Client_SignalHandler(const std::string& serverHostname_in,
                                                   const unsigned short& serverPort_in,
                                                   RPG_Net_Client_AsynchConnector* connector_in)
 : inherited(ACE_Reactor::instance(),         // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myPeerAddress(serverPort_in,
                 serverHostname_in.c_str()),
   myConnector(NULL),
   myAsynchConnector(connector_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_SignalHandler::Net_Client_SignalHandler"));

}

Net_Client_SignalHandler::~Net_Client_SignalHandler()
{
  RPG_TRACE(ACE_TEXT("Net_Client_SignalHandler::~Net_Client_SignalHandler"));

}

int
Net_Client_SignalHandler::handle_signal(int signal_in,
                                        siginfo_t* info_in,
                                        ucontext_t* context_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_SignalHandler::handle_signal"));

  ACE_UNUSED_ARG(context_in);

  // debug info
  if (info_in == NULL)
  {
/*    // *PORTABILITY*: tracing in a signal handler context is not portable
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%D: received [%S], but no siginfo_t was available, continuing\n"),
               signal_in));*/
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

  bool stop_event_dispatching = false;
  bool connect_to_server = false;
  bool abort_oldest = false;
  switch (signal_in)
  {
    case SIGINT:
    case SIGTERM:
// *PORTABILITY*: this isn't portable: on Windows SIGQUIT and SIGHUP are not defined...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
    case SIGHUP:
    case SIGQUIT:
#endif
    {
//       // *PORTABILITY*: tracing in a signal handler context is not portable
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("shutting down...\n")));

      // shutdown...
      stop_event_dispatching = true;

      break;
    }
// *PORTABILITY*: this isn't portable: on Windows SIGUSR1 and SIGUSR2 are not defined,
// so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
    case SIGUSR1:
#else
    case SIGBREAK:
#endif
    {
      // (try to) connect...
      connect_to_server = true;

      break;
    }
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
    case SIGUSR2:
#else
    case SIGABRT:
#endif
    {
      // (try to) abort oldest connection...
      abort_oldest = true;

      break;
    }
    default:
    {
      //// *PORTABILITY*: tracing in a signal handler context is not portable
      //ACE_DEBUG((LM_ERROR,
      //           ACE_TEXT("received unknown signal: \"%S\", continuing\n"),
      //           signal_in));

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
    int success = -1;
    if (myConnector)
    {
      RPG_Net_SocketHandler* handler = NULL;
      success = myConnector->connect(handler,                                // service handler
                                     myPeerAddress,                          // remote SAP
                                     ACE_Synch_Options::defaults,            // synch options
                                     ACE_sap_any_cast(const ACE_INET_Addr&), // local SAP
                                     1,                                      // re-use address (SO_REUSEADDR) ?
                                     O_RDWR,                                 // flags
                                     0);                                     // perms
    } // end IF
    else
    {
      success = myAsynchConnector->connect(myPeerAddress,                          // remote SAP
                                           ACE_sap_any_cast(const ACE_INET_Addr&), // local SAP
                                           1,                                      // re-use address (SO_REUSEADDR) ?
                                           NULL);                                  // ACT
    } // end ELSE
    if (success == -1)
    {
      // debug info
      ACE_TCHAR buf[RPG_COMMON_BUFSIZE];
      ACE_OS::memset(buf,
                     0,
                     (RPG_COMMON_BUFSIZE * sizeof(ACE_TCHAR)));
      if (myPeerAddress.addr_to_string(buf,
                                       (RPG_COMMON_BUFSIZE * sizeof(ACE_TCHAR))) == -1)
      {
/*        // *PORTABILITY*: tracing in a signal handler context is not portable
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));*/
      } // end IF
 /*      // *PORTABILITY*: tracing in a signal handler context is not portable
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Connector::connect(%s): \"%m\", continuing\n"),
                 buf));*/

      // release an existing connection, maybe that helps...
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortOldestConnection();
    } // end IF
  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers...
    // - break out of any (blocking) calls
    // --> (try to) terminate in a well-behaved manner

    // stop reactor/proactor
    if ((reactor()->end_event_loop() == -1) ||
        (ACE_Proactor::instance()->end_event_loop() == -1))
    {
      //// *PORTABILITY*: tracing in a signal handler context is not portable
      //ACE_DEBUG((LM_ERROR,
      //           ACE_TEXT("failed to terminate event handling: \"%m\", continuing\n")));
    } // end IF

    // de-register from the reactor...
    return -1;
  } // end IF

  return 0;
}
