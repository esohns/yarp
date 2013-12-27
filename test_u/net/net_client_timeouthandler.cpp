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

#include <ace/Event_Handler.h>

Net_Client_TimeoutHandler::Net_Client_TimeoutHandler(const unsigned int& maxNumConnections_in,
	                                                   const ACE_INET_Addr& remoteSAP_in,
                                                     RPG_Net_Client_IConnector* connector_in)
 : inherited(NULL,                            // default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myMaxNumConnections(maxNumConnections_in),
   myPeerAddress(remoteSAP_in),
   myConnector(connector_in)
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

	// sanity check: max num connections already reached ?
	// --> abort the oldest one first
	if (myMaxNumConnections &&
		  (RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->numConnections() >= myMaxNumConnections))
		RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortNewestConnection();

	ACE_ASSERT(myConnector);
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

  return 0;
}
