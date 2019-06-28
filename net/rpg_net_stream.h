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

#ifndef RPG_NET_STREAM_H
#define RPG_NET_STREAM_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_configuration.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"

#include "rpg_net_module_eventhandler.h"
#include "rpg_net_module_protocolhandler.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_session_message.h"

extern const char stream_name_string_[];

class RPG_Net_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_State,
                        struct RPG_Net_Protocol_StreamConfiguration,
                        struct Stream_Statistic,
                        struct Common_FlexParserAllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                        struct RPG_Net_Protocol_SessionData,
                        RPG_Net_Protocol_SessionData_t,
                        Stream_ControlMessage_t,
                        RPG_Net_Protocol_Message,
                        RPG_Net_Protocol_SessionMessage>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Stream_State,
                        struct RPG_Net_Protocol_StreamConfiguration,
                        struct Stream_Statistic,
                        struct Common_FlexParserAllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                        struct RPG_Net_Protocol_SessionData,
                        RPG_Net_Protocol_SessionData_t,
                        Stream_ControlMessage_t,
                        RPG_Net_Protocol_Message,
                        RPG_Net_Protocol_SessionMessage> inherited;

 public:
  RPG_Net_Stream ();
  virtual ~RPG_Net_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*,
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const inherited::CONFIGURATION_T&); // configuration

  // *TODO*: re-consider this API
  void ping ();

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  //virtual bool collect (Stream_Statistic_t&); // return value: statistic data
  //virtual void report () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Stream (const RPG_Net_Stream&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Stream& operator= (const RPG_Net_Stream&));

  // modules
  //RPG_Net_HeaderParser_Module    headerParser_;
  //Net_Module_RuntimeStatistic_Module runtimeStatistic_;
  RPG_Net_ProtocolHandler_Module protocolHandler_;
  RPG_Net_EventHandler_Module    eventHandler_;
};

#endif
