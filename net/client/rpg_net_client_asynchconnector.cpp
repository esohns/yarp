/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#include "rpg_net_client_asynchconnector.h"

#include "rpg_net_common.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

RPG_Net_Client_AsynchConnector::RPG_Net_Client_AsynchConnector()
 : inherited()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::RPG_Net_Client_AsynchConnector"));

  // init base class
  if (inherited::open(false,                    // pass addresses
                      ACE_Proactor::instance(), // default proactor
                      false) == -1)             // validate new connections
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Connector::open(): \"%m\", continuing\n")));
}

RPG_Net_Client_AsynchConnector::~RPG_Net_Client_AsynchConnector()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::~RPG_Net_Client_AsynchConnector"));

}

RPG_Net_AsynchStreamHandler_t*
RPG_Net_Client_AsynchConnector::make_handler(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::make_handler"));

  // init return value(s)
  RPG_Net_AsynchStreamHandler_t* handler_out = NULL;

  // default behavior
  ACE_NEW_NORETURN(handler_out,
                   RPG_Net_AsynchStreamHandler_t(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()));

  return handler_out;
}

void
RPG_Net_Client_AsynchConnector::connect(const ACE_INET_Addr& peer_address)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::connect"));

	int result = -1;
	result = inherited::connect(peer_address,                           // remote SAP
															ACE_sap_any_cast(const ACE_INET_Addr&), // local SAP
															1,                                      // re-use address (SO_REUSEADDR) ?
															NULL);                                  // ACT
	if (result == -1)
	{
    ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
    ACE_OS::memset(buffer,
                    0,
                    (RPG_COMMON_BUFSIZE * sizeof(ACE_TCHAR)));
    if (peer_address.addr_to_string(buffer,
                                    sizeof(buffer)) == -1)
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Connector::connect(%s): \"%m\", aborting\n"),
               buffer));
	} // end IF
}
