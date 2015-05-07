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

#ifndef RPG_NET_COMMON_H
#define RPG_NET_COMMON_H

#include "ace/INET_Addr.h"

#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_configuration.h"
#include "net_connection_manager_common.h"
#include "net_message.h"
#include "net_sessionmessage.h"
#include "net_stream_common.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_tcpsocket_base.h"
#include "net_tcpsockethandler.h"
#include "net_tcpconnection_base.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"

#include "rpg_net_stream.h"

typedef Stream_MessageAllocatorHeapBase_T<Net_Message,
                                          Net_SessionMessage> RPG_Net_StreamMessageAllocator_t;

typedef Net_StreamAsynchTCPSocketBase_T<ACE_INET_Addr,
                                        Net_SocketConfiguration_t,
                                        Net_Configuration_t,
                                        Net_UserData_t,
                                        Net_StreamSessionData_t,
                                        Stream_Statistic_t,
                                        RPG_Net_Stream,
                                        Net_AsynchTCPSocketHandler> RPG_Net_AsynchTCPHandler_t;
typedef Net_StreamTCPSocketBase_T<ACE_INET_Addr,
                                  Net_SocketConfiguration_t,
                                  Net_Configuration_t,
                                  Net_UserData_t,
                                  Net_StreamSessionData_t,
                                  Stream_Statistic_t,
                                  RPG_Net_Stream,
                                  Net_TCPSocketHandler> RPG_Net_TCPHandler_t;
typedef Net_AsynchTCPConnectionBase_T<Net_Configuration_t,
                                      Net_UserData_t,
                                      Net_StreamSessionData_t,
                                      RPG_Net_AsynchTCPHandler_t> RPG_Net_AsynchTCPConnection_t;
typedef Net_TCPConnectionBase_T<Net_Configuration_t,
                                Net_UserData_t,
                                Net_StreamSessionData_t,
                                RPG_Net_TCPHandler_t> RPG_Net_TCPConnection_t;

typedef Net_Client_AsynchConnector_T<ACE_INET_Addr,
                                     Net_SocketConfiguration_t,
                                     Net_Configuration_t,
                                     Net_SocketHandlerConfiguration_t,
                                     Net_UserData_t,
                                     Net_StreamSessionData_t,
                                     RPG_Net_AsynchTCPConnection_t> RPG_Net_Client_AsynchConnector_t;
typedef Net_Client_Connector_T<ACE_INET_Addr,
                               Net_SocketConfiguration_t,
                               Net_Configuration_t,
                               Net_SocketHandlerConfiguration_t,
                               Net_UserData_t,
                               Net_StreamSessionData_t,
                               RPG_Net_TCPConnection_t> RPG_Net_Client_Connector_t;

typedef NET_CONNECTIONMANAGER_SINGLETON RPG_CONNECTIONMANAGER_SINGLETON;

#endif
