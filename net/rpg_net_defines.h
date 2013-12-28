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

// CONFIGDIR-specific
#define RPG_NET_CONFIG_SUB                             "net"

// *** network-related ***
// *PORTABILITY*: interface names are not portable, so we let the
// user choose the interface from a list on Windows (see select_Interface())...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_NET_DEF_CNF_NETWORK_INTERFACE              ""
#else
#define RPG_NET_DEF_CNF_NETWORK_INTERFACE              "eth0"
#endif

// default event handler (default: use asynch I/O (proactor))
#define RPG_NET_USES_REACTOR                           false
#define RPG_NET_CONNECTION_HANDLER_THREAD_NAME         "RPG connection dispatch"
#define RPG_NET_CONNECTION_HANDLER_THREAD_GROUP_ID     2

// 1024 * 1024 --> 1 MByte
// *NOTE*: make this an even number to (gracefully) cope with Linux oddities...
#define RPG_NET_DEF_SOCK_RECVBUF_SIZE                  131072 // 128Kb
// #define RPG_NET_DEF_SOCK_RECVBUF_SIZE             1048576 // 1Mb
#define RPG_NET_SOCK_NODELAY                           true
#define RPG_NET_SOCK_KEEPALIVE                         60 // seconds

#define RPG_NET_MAX_NUM_OPEN_CONNECTIONS               10
// *WARNING*: this needs to be AT LEAST sizeof(RPG_Net_Remote_Comm::MessageHeader)
#define RPG_NET_STREAM_BUFFER_SIZE                     1024 // 1 kB

// *** protocol-related ***
#define RPG_NET_PING_AUTO_ANSWER                       true // auto-send "PONG"s

// *** pro/reactor-related ***
#define RPG_NET_TASK_GROUP_ID                          11
// *** stream-related ***
// *IMPORTANT NOTE*: any of these COULD seriously affect performance
#define RPG_NET_MAX_QUEUE_SLOTS                        1000
#define RPG_NET_MAX_MESSAGES                           100

#define RPG_NET_STATISTICS_COLLECT_INTERVAL            60 // seconds [0 --> OFF]
#define RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL      0  // seconds [0 --> OFF]

#endif
