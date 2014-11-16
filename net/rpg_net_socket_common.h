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

#ifndef RPG_NET_SOCKET_COMMON_H
#define RPG_NET_SOCKET_COMMON_H

#include "rpg_net_stream_common.h"
#include "rpg_net_stream_asynch_tcpsocket_base_t.h"
#include "rpg_net_stream_asynch_udpsocket_base_t.h"
#include "rpg_net_stream_tcpsocket_base_t.h"
#include "rpg_net_stream_udpsocket_base_t.h"
#include "rpg_net_stream.h"
#include "rpg_net_asynch_tcpsockethandler.h"
#include "rpg_net_asynch_udpsockethandler.h"
#include "rpg_net_tcpsockethandler.h"
#include "rpg_net_udpsockethandler.h"

#include "ace/SOCK_Dgram.h"
#include "ace/SOCK_Dgram_Mcast.h"
#include "ace/SOCK_Dgram_Bcast.h"

typedef RPG_Net_StreamAsynchTCPSocketBase_t<RPG_Net_ConfigPOD,
                                            RPG_Net_RuntimeStatistic,
                                            RPG_Net_Stream,
                                            RPG_Net_AsynchTCPSocketHandler> RPG_Net_AsynchTCPHandler;
typedef RPG_Net_StreamAsynchUDPSocketBase_t<RPG_Net_ConfigPOD,
                                            RPG_Net_RuntimeStatistic,
                                            RPG_Net_Stream,
                                            ACE_SOCK_Dgram,
                                            RPG_Net_AsynchUDPSocketHandler> RPG_Net_AsynchUDPHandler;
typedef RPG_Net_StreamAsynchUDPSocketBase_t<RPG_Net_ConfigPOD,
                                            RPG_Net_RuntimeStatistic,
                                            RPG_Net_Stream,
                                            ACE_SOCK_Dgram_Mcast,
                                            RPG_Net_AsynchUDPSocketHandler> RPG_Net_AsynchIPMulticastHandler;
typedef RPG_Net_StreamAsynchUDPSocketBase_t<RPG_Net_ConfigPOD,
                                            RPG_Net_RuntimeStatistic,
                                            RPG_Net_Stream,
                                            ACE_SOCK_Dgram_Bcast,
                                            RPG_Net_AsynchUDPSocketHandler> RPG_Net_AsynchIPBroadcastHandler;

typedef RPG_Net_StreamTCPSocketBase_t<RPG_Net_ConfigPOD,
                                      RPG_Net_RuntimeStatistic,
                                      RPG_Net_Stream,
                                      RPG_Net_TCPSocketHandler> RPG_Net_TCPHandler;
typedef RPG_Net_StreamUDPSocketBase_t<RPG_Net_ConfigPOD,
                                      RPG_Net_RuntimeStatistic,
                                      RPG_Net_Stream,
                                      ACE_SOCK_Dgram,
                                      RPG_Net_UDPSocketHandler> RPG_Net_UDPHandler;
typedef RPG_Net_StreamUDPSocketBase_t<RPG_Net_ConfigPOD,
                                      RPG_Net_RuntimeStatistic,
                                      RPG_Net_Stream,
                                      ACE_SOCK_Dgram_Mcast,
                                      RPG_Net_UDPSocketHandler> RPG_Net_IPMulticastHandler;
typedef RPG_Net_StreamUDPSocketBase_t<RPG_Net_ConfigPOD,
                                      RPG_Net_RuntimeStatistic,
                                      RPG_Net_Stream,
                                      ACE_SOCK_Dgram_Bcast,
                                      RPG_Net_UDPSocketHandler> RPG_Net_IPBroadcastHandler;

#endif
