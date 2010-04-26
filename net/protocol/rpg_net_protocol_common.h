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

#ifndef RPG_NET_PROTOCOL_COMMON_H
#define RPG_NET_PROTOCOL_COMMON_H

// #include "rpg_net_protocol_module_IRCsplitter.h"
// #include "rpg_net_protocol_module_IRCstreamer.h"
#include "rpg_net_protocol_IRCmessage.h"

#include <rpg_net_module_runtimestatistic.h>

#include <stream_streammodule_base.h>

#include <ace/Singleton.h>
#include <ace/Synch.h>

// forward declaration(s)
template <typename ConfigType> class RPG_Net_Connection_Manager;
class RPG_Net_Protocol_Module_IRCSplitter;
class RPG_Net_Protocol_Module_IRCStreamer;
class RPG_Net_Protocol_SessionMessage;
class RPG_Net_Protocol_Message;

struct RPG_Net_Protocol_ConfigPOD
{
  // ************ connection config data ************
  unsigned long            clientPingInterval; // used by the server...
  int                      socketBufferSize;
  Stream_IAllocator*       messageAllocator;
  unsigned long            defaultBufferSize;
  // ************ stream config data ************
  bool                     debugParser;
  unsigned long            sessionID; // (== socket handle !)
  unsigned long            statisticsReportingInterval;
  // ************ runtime data ************
  RPG_Net_RuntimeStatistic currentStatistics;
  ACE_Time_Value           lastCollectionTimestamp;
};

typedef ACE_Singleton<RPG_Net_Connection_Manager<RPG_Net_Protocol_ConfigPOD>,
                      ACE_Recursive_Thread_Mutex> RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON;

// declare module(s)
DATASTREAM_MODULE_DUPLEX(RPG_Net_Protocol_Module_IRCSplitter,
                         RPG_Net_Protocol_Module_IRCStreamer,
                         RPG_Net_Protocol_Module_IRCMarshal);
typedef RPG_Net_Module_RuntimeStatistic<RPG_Net_Protocol_SessionMessage,
                                        RPG_Net_Protocol_Message,
                                        RPG_Net_Protocol_CommandType_t> RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T;
DATASTREAM_MODULE_WRITER_ONLY_T(RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T,
                                RPG_Net_Module_RuntimeStatistic);

#endif
