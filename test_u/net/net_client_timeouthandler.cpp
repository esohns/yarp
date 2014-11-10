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

#include "ace/Event_Handler.h"

#include "rpg_dice.h"

#include "rpg_net_connection_manager_common.h"

#include "net_defines.h"
#include "net_common.h"

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler(ActionMode_t mode_in,
                                                     unsigned int maxNumConnections_in,
                                                     const ACE_INET_Addr& remoteSAP_in,
                                                     RPG_Net_Client_IConnector* connector_in)
 : inherited(NULL,                            // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myMode(mode_in),
	 myAlternatingMode(ALTERNATING_CONNECT),
   myMaxNumConnections(maxNumConnections_in),
   myPeerAddress(remoteSAP_in),
   myConnector(connector_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::Net_Client_TimeoutHandler"));

  ACE_ASSERT(myConnector);
}

Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler()
{
  RPG_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::~Net_Client_TimeoutHandler"));

}

int
Net_Client_TimeoutHandler::handle_timeout(const ACE_Time_Value& tv_in,
                                          const void* arg_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::handle_timeout"));

  ACE_UNUSED_ARG(tv_in);

  //const Net_GTK_CBData_t* user_data = reinterpret_cast<const Net_GTK_CBData_t*>(arg_in);
  //ActionMode_t action_mode = (user_data ? ACTION_GTK : myMode);
  ActionMode_t action_mode = myMode;
  bool do_connect = false;
  unsigned int num_connections =
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->numConnections();

  switch (action_mode)
  {
    case ACTION_NORMAL:
    {
      do_connect = true;
      break;
    }
    case ACTION_ALTERNATING:
    {
      switch (myAlternatingMode)
      {
        case ALTERNATING_CONNECT:
        {
          // sanity check: max num connections already reached ?
          // --> abort the oldest one first
          if (myMaxNumConnections &&
              (num_connections >= myMaxNumConnections))
          {
            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("closing oldest connection...\n")));

            RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortOldestConnection();
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
          const RPG_Net_Connection_Manager_t::CONNECTION_TYPE* connection_handler =
              RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->operator [](result.front() - 1);
          if (!connection_handler)
          {
            //						ACE_DEBUG((LM_DEBUG,
            //											 ACE_TEXT("failed to retrieve connection handler, continuing\n")));
            return 0;
          } // end IF

          try
          {
            const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->close();
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Net_IConnection::abort(), aborting\n")));

            // clean up
            const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->decrease();

            return -1;
          }

          // clean up
          const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->decrease();

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid alternating mode (was: %d), aborting\n"),
                     myAlternatingMode));
          return -1;
        }
      } // end SWITCH

      int temp = myAlternatingMode;
      myAlternatingMode =
          static_cast<Net_Client_TimeoutHandler::AlternatingMode_t>(++temp);
      if (myAlternatingMode == ALTERNATING_MAX)
        myAlternatingMode = ALTERNATING_CONNECT;

      break;
    }
    case ACTION_STRESS:
    {
      // allow some probability for closing connections in between
      // *WARNING*: there's a race condition here...
      if ((num_connections > 0) &&
          RPG_Dice::probability(NET_CLIENT_U_TEST_ABORT_PROBABILITY))
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("closing newest connection...\n")));

        RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortNewestConnection();
      } // end IF

      // allow some probability for opening connections in between
      if (RPG_Dice::probability(NET_CLIENT_U_TEST_CONNECT_PROBABILITY))
        do_connect = true;

      // ping the server

      // sanity check
      // *WARNING*: there's a race condition here...
      if (num_connections == 0)
        break;

      // grab a (random) connection handler
      // *WARNING*: there's a race condition here...
      RPG_Dice_RollResult_t result;
      RPG_Dice::generateRandomNumbers(num_connections,
                                      1,
                                      result);
      const RPG_Net_Connection_Manager_t::CONNECTION_TYPE* connection_handler =
          RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->operator [](result.front() - 1);
      if (!connection_handler)
      {
        //				ACE_DEBUG((LM_ERROR,
        //									 ACE_TEXT("failed to retrieve connection handler, continuing")));
        return 0;
      } // end IF

      try
      {
        const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->ping();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Net_IConnection::ping(), aborting\n")));

        // clean up
        const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->decrease();

        return -1;
      }

      // clean up
      const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->decrease();

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid mode (was: %d), aborting\n"),
                 myMode));
      return -1;
    }
  } // end IF

  if (do_connect)
  {
    try
    {
      myConnector->connect(myPeerAddress);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnector::connect(), aborting\n")));
      return -1;
    }
  } // end IF

  return 0;
}
