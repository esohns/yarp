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

#ifndef RPG_NET_PROTOCOL_CONFIGURATION_H
#define RPG_NET_PROTOCOL_CONFIGURATION_H

#include "ace/Time_Value.h"

#include "stream_common.h"

#include "net_configuration.h"

#include "rpg_net_protocol_common.h"

typedef Stream_Statistic_t RPG_Net_Protocol_RuntimeStatistic;
struct RPG_Net_Protocol_SessionData
{
  RPG_Net_Protocol_RuntimeStatistic currentStatistics;
  ACE_Time_Value                    lastCollectionTimestamp;
};

struct RPG_Net_Protocol_StreamConfiguration
{
  bool crunchMessageBuffers;
  bool debugScanner;
  bool debugParser;
};

struct RPG_Net_Protocol_ProtocolConfiguration
{
  RPG_Net_Protocol_StreamConfiguration streamConfiguration;
  unsigned int                         clientPingInterval; // server only
  RPG_Net_Protocol_IRCLoginOptions     loginOptions;
};

struct RPG_Net_Protocol_Configuration
{
  // **************************** socket data **********************************
  Net_SocketConfiguration_t              socketConfiguration;
  // **************************** stream data **********************************
  Stream_Configuration_t                 streamConfiguration;
  RPG_Net_Protocol_SessionData           streamSessionData;
  // *************************** protocol data *********************************
  RPG_Net_Protocol_ProtocolConfiguration protocolConfiguration;
};

#endif
