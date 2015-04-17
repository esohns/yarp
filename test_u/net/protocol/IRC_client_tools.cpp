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

#include "IRC_client_tools.h"

#include "ace/Log_Msg.h"

#include "common.h"

#include "stream_iallocator.h"

#include "net_stream_common.h"

#include "net_client_common.h"

#include "rpg_common_macros.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_network.h"

bool
IRC_Client_Tools::connect (Stream_IAllocator* messageAllocator_in,
                           const RPG_Net_Protocol_IRCLoginOptions& loginOptions_in,
                           bool debugScanner_in,
                           bool debugParser_in,
                           unsigned int statisticReportingInterval_in,
                           const std::string& serverHostname_in,
                           unsigned short serverPortNumber_in,
                           Common_Module_t* finalModule_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_Tools::connect"));

  // sanity check(s)
  ACE_ASSERT (finalModule_in);

  // step1: setup configuration passed to processing stream
  RPG_Net_Protocol_Configuration configuration;
  ACE_OS::memset (&configuration, 0, sizeof (configuration));

  // ************ socket configuration data ************
  configuration.socketConfiguration.bufferSize =
   NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;
  // ************ protocol config data **************
  configuration.protocolConfiguration.streamConfiguration.crunchMessageBuffers =
    RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES;
  configuration.protocolConfiguration.streamConfiguration.debugScanner =
   debugScanner_in;
  configuration.protocolConfiguration.streamConfiguration.debugParser =
   debugParser_in;
  configuration.protocolConfiguration.clientPingInterval = 0; // servers only
  configuration.protocolConfiguration.loginOptions = loginOptions_in;
  // ************ stream configuration data ****************
  configuration.streamConfiguration.messageAllocator = messageAllocator_in;
  configuration.streamConfiguration.bufferSize = RPG_NET_PROTOCOL_BUFFER_SIZE;
  configuration.streamConfiguration.module = finalModule_in;
  configuration.streamConfiguration.statisticReportingInterval =
    statisticReportingInterval_in;

  RPG_Net_Protocol_SessionData* session_data_p = NULL;
  ACE_NEW_NORETURN (session_data_p,
                    RPG_Net_Protocol_SessionData ());
  if (!session_data_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocator memory: \"%m\", aborting\n")));
    return false;
  } // end IF

  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->set (configuration,
                                                                  session_data_p);

  // step2: init client connector
  RPG_Net_Protocol_Connector_t connector (&configuration,
                                          RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance (),
                                          0);

  // step3: (try to) connect to the server
  ACE_INET_Addr peer_address (serverPortNumber_in,
                              serverHostname_in.c_str ());
  if (!connector.connect (peer_address))
  {
    // debug info
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    int result = peer_address.addr_to_string (buffer,
                                              sizeof (buffer));
    if (result == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to connect(\"%s\"): \"%m\", aborting\n"),
                buffer));
    return false;
  } // end IF

  // *NOTE* handlers automagically register with the connection manager and
  // will also de-register and self-destruct on disconnects !

  return true;
}
