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

#ifndef RPG_NET_COMMON_MODULES_H
#define RPG_NET_COMMON_MODULES_H

#include "rpg_net_common.h"
#include "rpg_net_sessionmessage.h"
#include "rpg_net_message.h"
#include "rpg_net_remote_comm.h"

#include "rpg_stream_streammodule.h"

template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType> class RPG_Net_Module_RuntimeStatistic;

// declare module(s)
typedef RPG_Net_Module_RuntimeStatistic<RPG_Net_SessionMessage,
                                        RPG_Net_Message,
                                        RPG_Net_MessageType,
                                        RPG_Net_RuntimeStatistic> RPG_NET_MODULE_RUNTIMESTATISTICS_T;
DATASTREAM_MODULE_T(RPG_NET_MODULE_RUNTIMESTATISTICS_T, // type
                    RPG_Net_Module_RuntimeStatistic);   // name

#endif
