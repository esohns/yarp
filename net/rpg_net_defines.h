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

// *** trace log ***
// *PORTABILITY*: pathnames are not portable, so we (try to) use %TEMP% for Windows...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_NET_DEF_LOG_DIRECTORY                 "TEMP"
#else
#define RPG_NET_DEF_LOG_DIRECTORY                 "/var/tmp"
#endif
#define RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX    "net_server"
#define RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX    "net_client"
#define RPG_NET_DEF_LOG_FILENAME_SUFFIX           ".log"

// DATADIR-specific
#define RPG_NET_DEF_DATA_SUB                      "net"

// - WARNING: current implementation cannot support numbers that have
//   more than 7 digits !!!
// - WARNING: current implementation cannot support 0 !!!
#define RPG_NET_DEF_LOG_MAXNUMFILES               5

// *** network-related ***
// *PORTABILITY*: interface names are not portable, so we let the
// user choose the interface from a list on Windows (see select_Interface())...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_NET_DEF_CNF_NETWORK_INTERFACE         ""
#else
#define RPG_NET_DEF_CNF_NETWORK_INTERFACE         "eth0"
#endif

// use a thread pool for handling network data ?
#define RPG_NET_DEF_SERVER_USES_TP                false
#define RPG_NET_DEF_SERVER_NUM_TP_THREADS         10

// 1024 * 1024 --> 1 MByte
// *NOTE*: make this an even number so we can cope with Linux oddities...
#define RPG_NET_DEF_SOCK_RECVBUF_SIZE             131072 // 128Kb
// #define RPG_NET_DEF_SOCK_RECVBUF_SIZE             1048576 // 1Mb
#define RPG_NET_DEF_SOCK_NODELAY                  true

#define RPG_NET_DEF_KEEPALIVE                     60
#define RPG_NET_DEF_LISTENING_PORT                10101
#define RPG_NET_DEF_MAX_NUM_OPEN_CONNECTIONS      10
// *WARNING*: this needs to be AT LEAST sizeof(RPG_Net_Remote_Comm::MessageHeader)
#define RPG_NET_DEF_NETWORK_BUFFER_SIZE           1024 // 1 kB

// *** protocol-related ***
#define RPG_NET_DEF_PING_INTERVAL                 5 // (server) [0 --> OFF]
#define RPG_NET_DEF_CLIENT_PING_PONG              true // (client) as a client, play "PONG"

// *** stream-related ***
#define RPG_NET_DEF_GROUP_ID_TASK                 11
// *IMPORTANT NOTE*: set to too small a value, any of these MAY seriously
// affect performance !!!
#define RPG_NET_DEF_MAX_QUEUE_SLOTS               1000
#define RPG_NET_DEF_MAX_MESSAGES                  100

// seconds, 0 --> OFF
#define RPG_NET_DEF_STATISTICS_COLLECT_INTERVAL   0
// seconds, 0 --> OFF
#define RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL 3600 // 1 hour

#endif
