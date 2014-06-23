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

#include "rpg_net_module_runtimestatistic.h"

#include "rpg_stream_streammodule_base.h"

#include "rpg_common.h"

#include <ace/Synch_Traits.h>

typedef RPG_Net_Module_RuntimeStatistic_t<ACE_MT_SYNCH,
                                          RPG_Common_TimePolicy_t,
                                          RPG_Net_Protocol_SessionMessage,
                                          RPG_Net_Protocol_Message,
                                          RPG_Net_Protocol_CommandType_t,
                                          RPG_Net_Protocol_RuntimeStatistic> RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T;

// declare module(s)
DATASTREAM_MODULE_DUPLEX(ACE_MT_SYNCH,                        // task synch type
                         RPG_Common_TimePolicy_t,             // time policy
                         RPG_Net_Protocol_Module_IRCSplitter, // reader type
                         RPG_Net_Protocol_Module_IRCStreamer, // writer type
                         RPG_Net_Protocol_Module_IRCMarshal); // name
DATASTREAM_MODULE_INPUT_ONLY_T(ACE_MT_SYNCH,                                // task synch type
                               RPG_Common_TimePolicy_t,                     // time policy
                               RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T, // writer type
                               RPG_Net_Protocol_Module_RuntimeStatistic);   // name

#endif
