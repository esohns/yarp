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

#ifndef RPG_NET_PROTOCOL_STREAM_COMMON_H
#define RPG_NET_PROTOCOL_STREAM_COMMON_H

#include "ace/INET_Addr.h"

#include "stream_common.h"
#include "stream_session_data.h"

#include "stream_net_common.h"

#include "net_common.h"
#include "net_iconnection.h"

//#include "rpg_net_protocol_network.h"

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct Net_StreamConnectionState,
                          Net_StreamStatistic_t> RPG_Net_Protocol_IConnection_t;

struct RPG_Net_Protocol_SessionData
 : Stream_SessionData
{
  RPG_Net_Protocol_SessionData ()
   : Stream_SessionData ()
   , connection (NULL)
   , connectionStates ()
   , statistic ()
  {}

  RPG_Net_Protocol_IConnection_t* connection;
  Stream_Net_ConnectionStates_t   connectionStates; // i/o module
  Net_StreamStatistic_t           statistic;
};
typedef Stream_SessionData_T<struct RPG_Net_Protocol_SessionData> RPG_Net_Protocol_SessionData_t;

struct RPG_Net_Protocol_StreamState
 : Stream_State
{
  RPG_Net_Protocol_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
  {}

  struct RPG_Net_Protocol_SessionData* sessionData;
};

#endif
