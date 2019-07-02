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

#ifndef RPG_NET_PROTOCOL_STREAM_H
#define RPG_NET_PROTOCOL_STREAM_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_configuration.h"

#include "common_time_common.h"

#include "stream_common.h"

#include "stream_net_io_stream.h"

#include "net_connection_manager.h"

#include "rpg_net_module_eventhandler.h"
#include "rpg_net_module_protocolhandler.h"

#include "rpg_net_protocol_common_modules.h"
#include "rpg_net_protocol_configuration.h"
//#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_message.h"
//#include "rpg_net_protocol_network.h"
#include "rpg_net_protocol_session_message.h"

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 RPG_Net_Protocol_ConnectionConfiguration,
                                 struct Net_StreamConnectionState,
                                 Net_StreamStatistic_t,
                                 struct Net_UserData> RPG_Net_Protocol_Connection_Manager_t;


extern const char protocol_stream_name_string_[];

class RPG_Net_Protocol_Stream
 : public Stream_Module_Net_IO_Stream_T <ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         protocol_stream_name_string_,
                                         enum Stream_ControlType,
                                         enum Stream_SessionMessageType,
                                         enum Stream_StateMachine_ControlState,
                                         struct RPG_Net_Protocol_StreamState,
                                         struct RPG_Net_Protocol_StreamConfiguration,
                                         struct Stream_Statistic,
                                         Common_Timer_Manager_t,
                                         struct Common_FlexParserAllocatorConfiguration,
                                         struct Stream_ModuleConfiguration,
                                         struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                         struct RPG_Net_Protocol_SessionData,
                                         RPG_Net_Protocol_SessionData_t,
                                         Stream_ControlMessage_t,
                                         RPG_Net_Protocol_Message,
                                         RPG_Net_Protocol_SessionMessage,
                                         ACE_INET_Addr,
                                         RPG_Net_Protocol_Connection_Manager_t,
                                         struct Stream_UserData>
{
  typedef Stream_Module_Net_IO_Stream_T <ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         protocol_stream_name_string_,
                                         enum Stream_ControlType,
                                         enum Stream_SessionMessageType,
                                         enum Stream_StateMachine_ControlState,
                                         struct RPG_Net_Protocol_StreamState,
                                         struct RPG_Net_Protocol_StreamConfiguration,
                                         struct Stream_Statistic,
                                         Common_Timer_Manager_t,
                                         struct Common_FlexParserAllocatorConfiguration,
                                         struct Stream_ModuleConfiguration,
                                         struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                         struct RPG_Net_Protocol_SessionData,
                                         RPG_Net_Protocol_SessionData_t,
                                         Stream_ControlMessage_t,
                                         RPG_Net_Protocol_Message,
                                         RPG_Net_Protocol_SessionMessage,
                                         ACE_INET_Addr,
                                         RPG_Net_Protocol_Connection_Manager_t,
                                         struct Stream_UserData> inherited;

 public:
  RPG_Net_Protocol_Stream ();
  virtual ~RPG_Net_Protocol_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*,
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&, // configuration
                           ACE_HANDLE);

  // implement Common_IStatistic_T
  // *NOTE*: delegate this to myRuntimeStatistic
  //virtual bool collect (RPG_Net_Protocol_RuntimeStatistic&); // return value: statistic data
  // this is just a dummy (use statisticsReportingInterval instead)
  //virtual void report () const;

  // *TODO*: re-consider this API
  void ping ();

 private:
//   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Stream())
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Stream (const RPG_Net_Protocol_Stream&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Stream& operator=(const RPG_Net_Protocol_Stream&))

  // modules
  //RPG_Net_HeaderParser_Module    headerParser_;
  //Net_Module_RuntimeStatistic_Module runtimeStatistic_;
  typename inherited::IO_MODULE_T IOHandler_;
  RPG_Net_ProtocolHandler_Module  protocolHandler_;
  RPG_Net_EventHandler_Module     eventHandler_;
};

#endif
