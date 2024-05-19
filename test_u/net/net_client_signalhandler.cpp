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

#include <sstream>
#include <string>

#include "ace/Assert.h"
#include "ace/Log_Msg.h"
#include "ace/Proactor.h"
#include "ace/Reactor.h"

#include "common_timer_manager.h"
#include "common_tools.h"

#include "net_connection_manager.h"

#include "net_client_common_tools.h"

#include "rpg_common_macros.h"

#include "rpg_net_defines.h"

#include "rpg_net_protocol_connection_manager.h"

Net_Client_SignalHandler::Net_Client_SignalHandler (struct Common_EventDispatchConfiguration& dispatchConfiguration_in,
                                                    RPG_Net_Protocol_ConnectionConfiguration& connectionConfiguration_in)
 : inherited (this) // event handler handle
 , dispatchConfiguration_ (&dispatchConfiguration_in)
 , connectionConfiguration_ (&connectionConfiguration_in)
 , asynchConnector_ (true) // managed ?
 , connector_ (true) // managed ?
 , handle_ (ACE_INVALID_HANDLE)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_SignalHandler::Net_Client_SignalHandler"));

}

void
Net_Client_SignalHandler::handle (const struct Common_Signal& signal_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_SignalHandler::handle"));

  RPG_Net_Protocol_Connection_Manager_t* connection_manager_p =
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);

  bool connect = false;
  bool stop_event_dispatching = false;
  bool report = false;
  switch (signal_in.signal)
  {
    // *PORTABILITY*: on Windows SIGQUIT is not defined
    case SIGINT:
#if defined (ACE_WIN32) && defined (ACE_WIN64)
#else
    case SIGHUP:
    case SIGQUIT:
#endif // ACE_WIN32 || ACE_WIN64
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("shutting down...\n")));

      // shutdown...
      stop_event_dispatching = true;

      break;
    }
// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) instead...
#if defined (ACE_WIN32) && defined (ACE_WIN64)
    case SIGBREAK:
#else
    case SIGUSR1:
#endif // ACE_WIN32 || ACE_WIN64
    { // (try to) connect to server
      connect = true;
      break;
    }
    case SIGTERM:
    {
      // dump statistics
      report = true;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid/unknown signal: \"%S\", returning\n"),
                  signal_in));
      return;
    }
  } // end SWITCH

  // connect ?
  if (connect)
  {
    if (handle_ != ACE_INVALID_HANDLE)
    {
      RPG_Net_Protocol_IConnection_t* iconnection_p = connection_manager_p->get (handle_);
      if (iconnection_p)
      {
        iconnection_p->abort ();
        iconnection_p->decrease ();
      } // end IF
      else
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("failed to retrieve connection handle (was: 0x%@), continuing\n"),
                    handle_));
      handle_ = ACE_INVALID_HANDLE;
    } // end IF
    ACE_ASSERT (handle_ == ACE_INVALID_HANDLE);

    struct Net_UserData user_data_s;
    ACE_INET_Addr server_address (static_cast<u_short> (RPG_NET_DEFAULT_PORT),
                                  ACE_LOCALHOST,
                                  AF_INET);
    if (dispatchConfiguration_->dispatch == COMMON_EVENT_DISPATCH_PROACTOR)
    {
      handle_ =
        Net_Client_Common_Tools::connect<RPG_Net_Protocol_AsynchConnector_t> (asynchConnector_,
                                                                              *connectionConfiguration_,
                                                                              user_data_s,
                                                                              server_address,
                                                                              false, // wait for completion ?
                                                                              true); // peer address ?
    } // end IF
    else
    {
      handle_ =
        Net_Client_Common_Tools::connect<RPG_Net_Protocol_Connector_t> (connector_,
                                                                        *connectionConfiguration_,
                                                                        user_data_s,
                                                                        server_address,
                                                                        false, // wait for completion ?
                                                                        true); // peer address ?
    } // end ELSE
  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers...
    // --> (try to) terminate in a well-behaved manner

    // step1: stop/abort/wait for connections
    connection_manager_p->stop (false,  // wait for completion ?
                                false);
    connection_manager_p->abort ();
    // *IMPORTANT NOTE*: as long as connections are inactive (i.e. events are
    // dispatched by reactor thread(s), there is no real reason to wait here)
    //RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

    // step2: stop reactor (&& proactor, if applicable)
    Common_Event_Tools::finalizeEventDispatch (*inherited::configuration_->dispatchState,
                                               false,  // wait for completion ?
                                               false); // release singletons ?
  } // end IF

  // report ?
  if (report)
  { ACE_ASSERT (handle_);
    RPG_Net_Protocol_IConnection_t* iconnection_p = connection_manager_p->get (handle_);
    if (!iconnection_p)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to retrieve connection handle (was: 0x%@), returning\n"),
                  handle_));
      return;
    } // end IF
    RPG_Net_Protocol_IStreamConnection_t* istream_connection_p =
      dynamic_cast<RPG_Net_Protocol_IStreamConnection_t*> (iconnection_p);
    ACE_ASSERT (istream_connection_p);
    RPG_Net_Protocol_Stream& stream_r =
      const_cast<RPG_Net_Protocol_Stream&> (istream_connection_p->stream ());
    Net_IStreamStatisticHandler_t* report_p = &stream_r;
    ACE_ASSERT (report_p);
    try {
      report_p->report ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_IStatistic::report(), returning\n")));
      iconnection_p->decrease ();
      return;
    }
    iconnection_p->decrease ();
  } // end IF
}
