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

#ifndef RPG_NET_SERVER_DEFINES_H
#define RPG_NET_SERVER_DEFINES_H

#include "ace/ACE.h"

#include "net_common.h"

// *** trace log ***
#define RPG_NET_SERVER_LOG_FILENAME_PREFIX                   "rpg_net_server"

// - WARNING: current implementation cannot support numbers that have
//   more than 7 digits !!!
// - WARNING: current implementation cannot support 0 !!!
#define RPG_NET_SERVER_LOG_MAXIMUM_NUMBER_OF_FILES           5

#define RPG_NET_SERVER_DEFAULT_NUMBER_OF_DISPATCHING_THREADS 10

#define RPG_NET_SERVER_DEFAULT_TRANSPORT_LAYER               TRANSPORTLAYER_TCP
#define RPG_NET_SERVER_DEFAULT_LISTENING_PORT                10101
#define RPG_NET_SERVER_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS    ACE::max_handles()

#define RPG_NET_SERVER_DEFAULT_STATISTICS_REPORTING_INTERVAL 3600 // seconds [0 --> OFF]

#define RPG_NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL          10000 // ms [0 --> OFF]
//#define RPG_NET_SERVER_DEFAULT_TCP_KEEPALIVE              0  // seconds [0 --> no timeout]

#endif
