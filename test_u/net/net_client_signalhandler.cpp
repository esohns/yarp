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

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Assert.h"
#include "ace/Log_Msg.h"

#include "net_client_common_tools.h"

#include "rpg_common_macros.h"

#include "rpg_net_defines.h"

#include "rpg_net_protocol_listener.h"

#include "rpg_client_network_manager.h"

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
  bool shutdown = false;
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
      shutdown = true;

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
    ACE_ASSERT (handle_);
    RPG_Net_Protocol_IConnection_t* iconnection_p = connection_manager_p->get (handle_);
    if (!iconnection_p)
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to retrieve connection handle (was: 0x%@), returning\n"),
                  handle_));
      return;
    } // end IF

    RPG_Client_Network_Manager* network_manager_p =
      RPG_CLIENT_NETWORK_MANAGER_SINGLETON::instance ();
    ACE_ASSERT (network_manager_p);
    network_manager_p->add (iconnection_p);

    iconnection_p->decrease ();
  } // end IF

  // ...shutdown ?
  if (shutdown)
  {
    // stop everything, i.e.
    // - leave SDL event loop
    union SDL_Event sdl_event;
#if defined (SDL_USE) || defined (SDL2_USE)
    sdl_event.type = SDL_QUIT;
#elif defined (SDL3_USE)
    sdl_event.type = SDL_EVENT_QUIT;
#endif // SDL_USE || SDL2_USE || SDL3_USE

    // push it onto the event queue
    if (SDL_PushEvent (&sdl_event) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));
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
