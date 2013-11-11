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

#include "rpg_net_common.h"

#include "rpg_common_defines.h"

#include <ace/Reactor.h>

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler(const std::string& serverHostname_in,
                                                     const unsigned short& serverPort_in,
                                                     RPG_Net_Client_Connector* connector_in)
 : inherited(ACE_Reactor::instance(),         // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myPeerAddress(serverPort_in,
                 serverHostname_in.c_str()),
   myConnector(connector_in),
   myAsynchConnector(NULL)
{
  RPG_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::Net_Client_TimeoutHandler"));

}

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler(const std::string& serverHostname_in,
                                                     const unsigned short& serverPort_in,
                                                     RPG_Net_Client_AsynchConnector* connector_in)
 : inherited(ACE_Reactor::instance(),         // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myPeerAddress(serverPort_in,
                 serverHostname_in.c_str()),
   myConnector(NULL),
   myAsynchConnector(connector_in)
{
  RPG_TRACE(ACE_TEXT("Net_Client_TimeoutHandler::Net_Client_TimeoutHandler"));

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
  ACE_UNUSED_ARG(arg_in);

  // step1: connect to server...
  int success = -1;
  if (myConnector)
  {
    RPG_Net_Client_SocketHandler* handler = NULL;
    success = myConnector->connect(handler,                                // service handler
                                   myPeerAddress,                          // remote SAP
                                   ACE_Synch_Options::defaults,            // synch options
                                   ACE_sap_any_cast(const ACE_INET_Addr&), // local SAP
                                   0,                                      // re-use address (SO_REUSEADDR) ?
                                   O_RDWR,                                 // flags
                                   0);                                     // perms
  }
  else
    success = myAsynchConnector->connect(myPeerAddress,                          // remote SAP
                                         ACE_sap_any_cast(const ACE_INET_Addr&), // local SAP
                                         1,                                      // re-use address (SO_REUSEADDR) ?
                                         NULL);                                  // ACT
  if (success == -1)
  {
    // debug info
    ACE_TCHAR buf[RPG_COMMON_BUFSIZE];
    ACE_OS::memset(buf,
                   0,
                   (RPG_COMMON_BUFSIZE * sizeof(ACE_TCHAR)));
    if (myPeerAddress.addr_to_string(buf,
                                     (RPG_COMMON_BUFSIZE * sizeof(ACE_TCHAR))) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Connector::connect(%s): \"%m\", continuing\n"),
               buf));

    // release an existing connection, maybe that helps...
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortOldestConnection();
  } // end IF

  return 0;
}
