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

#ifndef RPG_NET_PROTOCOL_NETWORK_H
#define RPG_NET_PROTOCOL_NETWORK_H

#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "stream_common.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_configuration.h"
#include "net_connection_manager.h"
#include "net_iconnectionmanager.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_tcpsocket_base.h"
#include "net_tcpsockethandler.h"
#include "net_tcpconnection_base.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"

#include "net_server_asynchlistener.h"
#include "net_server_listener.h"

//#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_stream.h"
#include "rpg_net_protocol_stream_common.h"

typedef Net_TCPConnectionBase_T<ACE_MT_SYNCH,
                                Net_TCPSocketHandler_t,
                                RPG_Net_Protocol_ConnectionConfiguration,
                                struct Net_StreamConnectionState,
                                Net_StreamStatistic_t,
                                RPG_Net_Protocol_Stream,
                                struct Net_UserData> RPG_Net_Protocol_TCPConnection_t;
typedef Net_AsynchTCPConnectionBase_T<Net_AsynchTCPSocketHandler_t,
                                      RPG_Net_Protocol_ConnectionConfiguration,
                                      struct Net_StreamConnectionState,
                                      Net_StreamStatistic_t,
                                      RPG_Net_Protocol_Stream,
                                      struct Net_UserData> RPG_Net_Protocol_AsynchTCPConnection_t;

typedef Net_IConnector_T<ACE_INET_Addr,
                         RPG_Net_Protocol_ConnectionConfiguration> RPG_Net_Protocol_IConnector_t;

typedef Net_Client_Connector_T<ACE_MT_SYNCH,
                               RPG_Net_Protocol_TCPConnection_t,
                               Net_SOCK_Connector,
                               ACE_INET_Addr,
                               RPG_Net_Protocol_ConnectionConfiguration,
                               struct Net_StreamConnectionState,
                               Net_StreamStatistic_t,
                               Net_TCPSocketConfiguration_t,
                               Net_TCPSocketConfiguration_t,
                               RPG_Net_Protocol_Stream,
                               struct Net_UserData> RPG_Net_Protocol_Connector_t;
typedef Net_Client_AsynchConnector_T<RPG_Net_Protocol_AsynchTCPConnection_t,
                                     ACE_INET_Addr,
                                     RPG_Net_Protocol_ConnectionConfiguration,
                                     struct Net_StreamConnectionState,
                                     Net_StreamStatistic_t,
                                     Net_TCPSocketConfiguration_t,
                                     Net_TCPSocketConfiguration_t,
                                     RPG_Net_Protocol_Stream,
                                     struct Net_UserData> RPG_Net_Protocol_AsynchConnector_t;

typedef Net_Server_Listener_T<RPG_Net_Protocol_TCPConnection_t,
                              ACE_SOCK_ACCEPTOR,
                              ACE_INET_Addr,
                              RPG_Net_Protocol_ConnectionConfiguration,
                              struct Net_StreamConnectionState,
                              RPG_Net_Protocol_Stream,
                              struct Net_UserData> RPG_Net_Protocol_TCPListener_t;
typedef Net_Server_AsynchListener_T<RPG_Net_Protocol_AsynchTCPConnection_t,
                                    ACE_INET_Addr,
                                    RPG_Net_Protocol_ConnectionConfiguration,
                                    struct Net_StreamConnectionState,
                                    RPG_Net_Protocol_Stream,
                                    struct Net_UserData> RPG_Net_Protocol_AsynchTCPListener_t;

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 RPG_Net_Protocol_ConnectionConfiguration,
                                 struct Net_StreamConnectionState,
                                 Net_StreamStatistic_t,
                                 struct Net_UserData> RPG_Net_Protocol_IConnection_Manager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 RPG_Net_Protocol_ConnectionConfiguration,
                                 struct Net_StreamConnectionState,
                                 Net_StreamStatistic_t,
                                 struct Net_UserData> RPG_Net_Protocol_Connection_Manager_t;

typedef ACE_Singleton<RPG_Net_Protocol_TCPListener_t,
                      ACE_SYNCH_MUTEX> RPG_NET_PROTOCOL_LISTENER_SINGLETON;
typedef ACE_Singleton<RPG_Net_Protocol_AsynchTCPListener_t,
                      ACE_SYNCH_MUTEX> RPG_NET_PROTOCOL_ASYNCH_LISTENER_SINGLETON;

//RPG_PROTOCOL_SINGLETON_DECLARE (ACE_Singleton,
//                                RPG_Net_Protocol_Connection_Manager_t,
//                                ACE_Recursive_Thread_Mutex);
typedef ACE_Singleton<RPG_Net_Protocol_Connection_Manager_t,
                      ACE_SYNCH_MUTEX> RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON;

#endif
