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

// #include "ace/Synch_Traits.h"

// #include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_handler.h"
#include "rpg_net_protocol_streamer.h"

//typedef Net_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
//                                          Common_TimePolicy_t,
//                                          RPG_Net_Protocol_SessionMessage,
//                                          RPG_Net_Protocol_Message,
//                                          RPG_Net_Protocol_CommandType_t,
//                                          RPG_Net_Protocol_RuntimeStatistic> RPG_Net_Protocol_Module_Statistic_ReaderTask_t;
//typedef Net_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
//                                          Common_TimePolicy_t,
//                                          RPG_Net_Protocol_SessionMessage,
//                                          RPG_Net_Protocol_Message,
//                                          RPG_Net_Protocol_CommandType_t,
//                                          RPG_Net_Protocol_RuntimeStatistic> RPG_Net_Protocol_Module_Statistic_WriterTask_t;

// declare module(s)
//DATASTREAM_MODULE_DUPLEX (ACE_MT_SYNCH,                                   // task synch type
//                          Common_TimePolicy_t,                            // time policy type
//                          RPG_Net_Protocol_Configuration,                 // configuration type
//                          RPG_Net_Protocol_Module_Statistic_ReaderTask_t, // reader type
//                          RPG_Net_Protocol_Module_Statistic_WriterTask_t, // writer type
//                          RPG_Net_Protocol_Module_RuntimeStatistic);      // name

DATASTREAM_MODULE_DUPLEX (struct RPG_Net_Protocol_SessionData,                // session data type
                          enum Stream_SessionMessageType,                     // session event type
                          struct RPG_Net_Protocol_ModuleHandlerConfiguration, // configuration type
                          rpg_net_protocol_handler_module_name_string,
                          Stream_INotify_t,
                          RPG_Net_Protocol_Streamer,                          // reader type
                          RPG_Net_Protocol_Handler,                           // writer type
                          RPG_Net_Protocol_Marshal);                          // name

#endif
