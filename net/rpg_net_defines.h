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

#ifndef RPG_NET_DEFINES_H
#define RPG_NET_DEFINES_H

#include "ace/Default_Constants.h"

// CONFIGDIR-specific
#define RPG_NET_CONFIGURATION_SUBDIRECTORY            "net"

// *** network-related ***
// *PORTABILITY*: interface names are not portable, so we let the
// user choose the interface from a list on Windows (see select_Interface())...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_NET_DEFAULT_NETWORK_INTERFACE             ""
#else
#define RPG_NET_DEFAULT_NETWORK_INTERFACE             "eth0"
#endif

#define RPG_NET_DEFAULT_PORT                          32767
#define RPG_NET_DEFAULT_IP_MULTICAST_ADDRESS          "224.0.0.1"
#define RPG_NET_DEFAULT_IP_BROADCAST_ADDRESS          "255.255.255.255"

// default event dispatcher (default: use asynch I/O (proactor))
//#define RPG_NET_USES_REACTOR                          false
//#define RPG_NET_CONNECTION_HANDLER_THREAD_NAME        "RPG connection dispatch"
//#define RPG_NET_CONNECTION_HANDLER_THREAD_GROUP_ID    2

#define RPG_NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE    ACE_DEFAULT_MAX_SOCKET_BUFSIZ
#define RPG_NET_DEFAULT_TCP_NODELAY                   true
#define RPG_NET_DEFAULT_TCP_KEEPALIVE                 false
#define RPG_NET_DEFAULT_TCP_LINGER                    10 // seconds {0 --> off}

#define RPG_NET_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS    10
// *WARNING*: this needs to be AT LEAST sizeof(RPG_Net_Remote_Comm::MessageHeader)
#define RPG_NET_STREAM_BUFFER_SIZE                    1024 // 1 kB

// *** protocol-related ***
#define RPG_NET_DEFAULT_TRANSPORTLAYER                NET_TRANSPORTLAYER_TCP
#define RPG_NET_PING_AUTO_ANSWER                      true // auto-send "PONG"s

// *** pro/reactor-related ***
//#define RPG_NET_TASK_GROUP_ID                         11
// *** stream-related ***
// *IMPORTANT NOTE*: any of these COULD seriously affect performance
#define RPG_NET_MAXIMUM_QUEUE_SLOTS                   1000
#define RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES   0 // do not limit

#define RPG_NET_STATISTICS_COLLECTION_INTERVAL        60 // seconds [0 --> OFF]
#define RPG_NET_DEFAULT_STATISTICS_REPORTING_INTERVAL 0  // seconds [0 --> OFF]

#endif
