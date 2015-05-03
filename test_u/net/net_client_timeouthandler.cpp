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

#include "net_client_timeouthandler.h"

#include "net_defines.h"
#include "net_common.h"
#include "net_connection_manager_common.h"

#include "rpg_dice.h"

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler (ActionMode_t mode_in,
                                                      unsigned int maxNumConnections_in,
                                                      const ACE_INET_Addr& remoteSAP_in,
                                                      Net_Client_IConnector_t* connector_in)
 : inherited (NULL,                           // default reactor
              ACE_Event_Handler::LO_PRIORITY) // priority
 , alternatingMode_ (ALTERNATING_CONNECT)
 , connector_ (connector_in)
 , maxNumConnections_ (maxNumConnections_in)
 , mode_ (mode_in)
 , peerAddress_ (remoteSAP_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_TimeoutHandler::Net_Client_TimeoutHandler"));

}

Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler ()
{
  RPG_TRACE (ACE_TEXT ("Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler"));

}

int
Net_Client_TimeoutHandler::handle_timeout (const ACE_Time_Value& tv_in,
                                           const void* arg_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::handle_timeout"));

  ACE_UNUSED_ARG (tv_in);

  //const Net_GTK_CBData_t* user_data = reinterpret_cast<const Net_GTK_CBData_t*>(arg_in);
  //ActionMode_t action_mode = (user_data ? ACTION_GTK : myMode);
  ActionMode_t action_mode = mode_;
  bool do_connect = false;
  unsigned int num_connections =
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections ();

  switch (action_mode)
  {
    case ACTION_NORMAL:
    {
      do_connect = true;
      break;
    }
    case ACTION_ALTERNATING:
    {
      switch (alternatingMode_)
      {
        case ALTERNATING_CONNECT:
        {
          // sanity check: max num connections already reached ?
          // --> abort the oldest one first
          if (maxNumConnections_ &&
              (num_connections >= maxNumConnections_))
          {
            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("closing oldest connection...\n")));

            NET_CONNECTIONMANAGER_SINGLETON::instance ()->abortOldestConnection ();
          } // end IF
          do_connect = true;
          break;
        }
        case ALTERNATING_ABORT:
        {
          // sanity check
          if (num_connections == 0)
            break; // nothing to do...

          // grab a (random) connection handler
          // *WARNING*: there's a race condition here...
          RPG_Dice_RollResult_t result;
          RPG_Dice::generateRandomNumbers(num_connections,
                                          1,
                                          result);
          Net_InetConnectionManager_t::CONNECTION_T* connection_p =
            NET_CONNECTIONMANAGER_SINGLETON::instance ()->operator [] (result.front () - 1);
          if (!connection_p)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to retrieve connection #%d, returning\n"),
                        result.front () - 1));
            return 0;
          } // end IF

          try
          {
            connection_p->close ();
          }
          catch (...)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in Net_IConnection_T::close(), aborting\n")));

            // clean up
            connection_p->decrease ();

            return -1;
          }

          // clean up
          connection_p->decrease ();

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid alternating mode (was: %d), aborting\n"),
                      alternatingMode_));
          return -1;
        }
      } // end SWITCH

      int temp = alternatingMode_;
      alternatingMode_ =
        static_cast<Net_Client_TimeoutHandler::AlternatingMode_t> (++temp);
      if (alternatingMode_ == ALTERNATING_MAX)
        alternatingMode_ = ALTERNATING_CONNECT;

      break;
    }
    case ACTION_STRESS:
    {
      // allow some probability for closing connections in between
      // *WARNING*: there's a race condition here...
      if ((num_connections > 0) &&
          RPG_Dice::probability (NET_CLIENT_U_TEST_ABORT_PROBABILITY))
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("closing newest connection...\n")));

        NET_CONNECTIONMANAGER_SINGLETON::instance ()->abortNewestConnection ();
      } // end IF

      // allow some probability for opening connections in between
      if (RPG_Dice::probability (NET_CLIENT_U_TEST_CONNECT_PROBABILITY))
        do_connect = true;

      // ping the server

      // sanity check
      // *WARNING*: there's a race condition here...
      if (num_connections == 0)
        break;

      // grab a (random) connection handler
      // *WARNING*: there's a race condition here...
      RPG_Dice_RollResult_t result;
      RPG_Dice::generateRandomNumbers (num_connections,
                                       1,
                                       result);
      Net_InetConnectionManager_t::CONNECTION_T* connection_base_p =
          NET_CONNECTIONMANAGER_SINGLETON::instance ()->operator [] (result.front () - 1);
      if (!connection_base_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to retrieve connection #%d, returning\n"),
                    result.front () - 1));
        return 0;
      } // end IF
      Net_ITransportLayer_t* connection_p =
        dynamic_cast<Net_ITransportLayer_t*> (connection_base_p);
      if (!connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<Net_ITransportLayer_t*> (%@), returning\n"),
                    connection_base_p));
        return 0;
      } // end IF

      try
      {
        connection_p->ping ();
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Net_IConnection_T::ping(), aborting\n")));

        // clean up
        connection_base_p->decrease ();
        connection_base_p->close ();

        return -1;
      }

      // clean up
      connection_base_p->decrease ();

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid mode (was: %d), aborting\n"),
                  mode_));
      return -1;
    }
  } // end IF

  if (do_connect && connector_)
  {
    bool result = false;
    try
    {
      result = connector_->connect (peerAddress_);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Net_Client_IConnector_t::connect(), aborting\n")));
      return -1;
    }
    if (!result)
    {
      ACE_TCHAR buffer[BUFSIZ];
      ACE_OS::memset(buffer, 0, sizeof (buffer));
      int result_2 = peerAddress_.addr_to_string (buffer, sizeof (buffer));
      if (result_2 == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Client_IConnector::connect(\"%s\"), continuing\n"),
                  buffer));
    } // end IF
  } // end IF

  return 0;
}
