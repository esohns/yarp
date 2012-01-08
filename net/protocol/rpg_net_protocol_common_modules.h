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

#ifndef RPG_NET_PROTOCOL_COMMON_MODULES_H
#define RPG_NET_PROTOCOL_COMMON_MODULES_H

#include "rpg_net_protocol_module_IRCsplitter.h"
#include "rpg_net_protocol_module_IRCstreamer.h"
#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_IRCmessage.h"
#include "rpg_net_protocol_common.h"

#include <rpg_stream_streammodule.h>
#include <rpg_stream_streammodule_base.h>

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType> class RPG_Net_Module_RuntimeStatistic;

// declare module(s)
DATASTREAM_MODULE_DUPLEX(RPG_Net_Protocol_Module_IRCSplitter,
                         RPG_Net_Protocol_Module_IRCStreamer,
                         RPG_Net_Protocol_Module_IRCMarshal);
typedef RPG_Net_Module_RuntimeStatistic<RPG_Net_Protocol_SessionMessage,
                                        RPG_Net_Protocol_Message,
                                        RPG_Net_Protocol_CommandType_t,
                                        RPG_Net_Protocol_RuntimeStatistic> RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T;
DATASTREAM_MODULE_T(RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T,
                    RPG_Net_Protocol_Module_RuntimeStatistic);

#endif
