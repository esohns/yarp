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
#include "IRC_client_tools.h"

#include "IRC_common.h"

#include <rpg_net_defines.h>
#include <rpg_net_connection_manager.h>

#include <rpg_net_protocol_defines.h>

#include <rpg_common_macros.h>

const bool
IRC_Client_Tools::connect(RPG_Stream_IAllocator* messageAllocator_in,
                          const RPG_Net_Protocol_IRCLoginOptions& loginOptions_in,
                          const bool& debugScanner_in,
                          const bool& debugParser_in,
                          const unsigned long& statisticsReportingInterval_in,
                          const std::string& serverHostname_in,
                          const unsigned short& serverPortNumber_in,
                          RPG_Stream_Module* finalModule_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_Tools::connect"));

  // sanity check(s)
  ACE_ASSERT(finalModule_in);

  // step1: setup configuration passed to processing stream
  RPG_Net_Protocol_ConfigPOD stream_config;
  // ************ connection config data ************
  stream_config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  stream_config.messageAllocator = messageAllocator_in;
  stream_config.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
  // ************ protocol config data **************
  stream_config.clientPingInterval = 0; // servers do this...
  stream_config.loginOptions = loginOptions_in;
  // ************ stream config data ****************
  stream_config.module = finalModule_in;
  stream_config.crunchMessageBuffers = RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES;
  stream_config.debugScanner = debugScanner_in;
  stream_config.debugParser = debugParser_in;
  // *WARNING*: set at runtime (by the connection handler)
  stream_config.sessionID = 0; // (== socket handle !)
  stream_config.statisticsReportingInterval = statisticsReportingInterval_in;
  // ************ runtime statistics data ***********
  stream_config.currentStatistics.numDataMessages = 0;
  stream_config.currentStatistics.numBytes = 0;
  stream_config.lastCollectionTimestamp = ACE_Time_Value::zero;
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->set(stream_config);

  // step2: init client connector
  IRC_Client_Connector connector(ACE_Reactor::instance(), // reactor
                                 ACE_NONBLOCK);           // flags: non-blocking I/O
//                                  0);                      // flags (*TODO*: ACE_NONBLOCK ?);

  // step3: (try to) connect to the server
  IRC_Client_SocketHandler* handler = NULL;
  ACE_INET_Addr remote_address(serverPortNumber_in, // remote SAP
                               serverHostname_in.c_str());
  if (connector.connect(handler,                     // service handler
      remote_address/*,              // remote SAP
          ACE_Synch_Options::defaults, // synch options
          ACE_INET_Addr::sap_any,      // local SAP
          0,                           // try to re-use address (SO_REUSEADDR)
          O_RDWR,                      // flags
          0*/) == -1)                  // perms
  {
    // debug info
    ACE_TCHAR buf[BUFSIZ];
    ACE_OS::memset(buf,
                   0,
                   (BUFSIZ * sizeof(ACE_TCHAR)));
    if (remote_address.addr_to_string(buf,
        (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Connector::connect(%s): \"%m\", aborting\n"),
                        buf));

    return false;
  } // end IF
  // sanity check
  ACE_ASSERT(handler);

  // *NOTE* handlers automagically register with the connection manager and
  // will also de-register and self-destruct on disconnects !

  return true;
}

const std::string
IRC_Client_Tools::UTF82Locale(const gchar* string_in,
                              const gssize& length_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_Tools::UTF82Locale"));

  // init result
  std::string result;

  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  converted_text = g_locale_from_utf8(string_in,          // text
                                      length_in,          // number of bytes
                                      NULL,               // bytes read (don't care)
                                      NULL,               // bytes written (don't care)
                                      &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert string: \"%s\", aborting\n"),
               conversion_error->message));

    // clean up
    g_error_free(conversion_error);
  } // end IF
  else
    result = converted_text;

  // clean up
  g_free(converted_text);

  return result;
}

gchar*
IRC_Client_Tools::Locale2UTF8(const std::string& string_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_Tools::Locale2UTF8"));

  // init result
  gchar* result = NULL;

  GError* conversion_error = NULL;
  result = g_locale_to_utf8(string_in.c_str(),  // text
                            -1,                 // \0-terminated
                            NULL,               // bytes read (don't care)
                            NULL,               // bytes written (don't care)
                            &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert string: \"%s\", aborting\n"),
               conversion_error->message));

      // clean up
    g_error_free(conversion_error);

    return NULL;
  } // end IF

  return result;
}
