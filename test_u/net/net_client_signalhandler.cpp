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

#include <ace/Reactor.h>
#include <ace/Proactor.h>

#include "rpg_common_macros.h"
#include "rpg_common_timer_manager.h"

#include "rpg_net_common.h"
#include "rpg_net_common_tools.h"

Net_Client_SignalHandler::Net_Client_SignalHandler(const long& timerID_in,
                                                   const ACE_INET_Addr& peerSAP_in,
                                                   RPG_Net_Client_IConnector* connector_in,
                                                   const bool& useReactor_in)
 : inherited(NULL,                            // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myTimerID(timerID_in),
   myPeerAddress(peerSAP_in),
   myConnector(connector_in),
   myUseReactor(useReactor_in),
   mySignal(-1)//,
#if defined(ACE_WIN32) || defined(ACE_WIN64)
   ,mySigInfo(ACE_INVALID_HANDLE),
   myUContext(-1)
#else
   //mySigInfo(),
   //myUContext()
#endif
{
  RPG_TRACE(ACE_TEXT("Net_Client_SignalHandler::Net_Client_SignalHandler"));

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  ACE_OS::memset(&mySigInfo, 0, sizeof(mySigInfo));
  ACE_OS::memset(&myUContext, 0, sizeof(myUContext));
#endif
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

  // *IMPORTANT NOTE*: in signal context, most actions are forbidden, so save
  // the state and notify the reactor/proactor for callback instead (see below)

  // save state
  mySignal = signal_in;
  ACE_OS::memset(&mySigInfo, 0, sizeof(mySigInfo));
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  mySigInfo.si_handle_ = static_cast<ACE_HANDLE>(info_in);
#else
  if (info_in)
    mySigInfo = *info_in;
#endif
  if (context_in)
    myUContext = *context_in;

  // schedule the reactor (see below)
  ACE_Reactor::instance()->notify(this);

  return 0;
}

int
Net_Client_SignalHandler::handle_exception(ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_SignalHandler::handle_exception"));

  ACE_UNUSED_ARG(handle_in);

  // collect some context information...
  std::string information;
  RPG_Net_Common_Tools::retrieveSignalInfo(mySignal,
                                           mySigInfo,
                                           &myUContext,
                                           information);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%D: received [%S]: %s\n"),
             mySignal,
             information.c_str()));

  bool stop_event_dispatching = false;
  bool connect_to_server = false;
  bool abort_oldest = false;
  switch (mySignal)
  {
    case SIGINT:
    {
      //ACE_DEBUG((LM_DEBUG,
      //           ACE_TEXT("shutting down...\n")));

      // shutdown...
      stop_event_dispatching = true;

      break;
    }
// *PORTABILITY*: this isn't portable: on Windows SIGUSR1 and SIGUSR2 are not defined,
// --> handle SIGBREAK (21) and SIGTERM (15) instead...
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
    case SIGTERM:
#endif
    {
      // (try to) abort oldest connection...
      abort_oldest = true;

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received invalid/unknown signal: \"%S\", continuing\n"),
                 mySignal));

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
    try
    {
      myConnector->connect(myPeerAddress);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnector::connect(), continuing\n")));
    }
  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers...
    // --> (try to) terminate in a well-behaved manner

    // stop interval timer (might spawn new connections)
    if (myTimerID >= 0)
    {
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(myTimerID, NULL) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   myTimerID));
      myTimerID = -1;
    } // end IF
    myConnector->abort();
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->stop();
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

    // stop reactor (&& proactor, if applicable)
    int result = ACE_Reactor::instance()->end_event_loop();
    if (result == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
    if (!myUseReactor)
    {
      result = ACE_Proactor::instance()->end_event_loop();
      if (result == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Proactor::end_event_loop(): \"%m\", continuing\n")));
    } // end IF
  } // end IF

  return 0;
}
