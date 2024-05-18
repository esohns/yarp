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

#ifndef RPG_NET_PROTOCOL_STREAMER_H
#define RPG_NET_PROTOCOL_STREAMER_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_task_base_synch.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_session_message.h"

extern const char rpg_net_protocol_streamer_module_name_string[];

class RPG_Net_Protocol_Streamer
 : public Stream_TaskBaseSynch_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                 Stream_ControlMessage_t,
                                 RPG_Net_Protocol_Message,
                                 RPG_Net_Protocol_SessionMessage,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData>
{
  typedef Stream_TaskBaseSynch_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                 Stream_ControlMessage_t,
                                 RPG_Net_Protocol_Message,
                                 RPG_Net_Protocol_SessionMessage,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData> inherited;

 public:
  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  RPG_Net_Protocol_Streamer (ISTREAM_T*);                     // stream handle
#else
  RPG_Net_Protocol_Streamer (typename inherited::ISTREAM_T*); // stream handle
#endif // ACE_WIN32 || ACE_WIN64
  inline virtual ~RPG_Net_Protocol_Streamer () {}

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (RPG_Net_Protocol_Message*&, // data message handle
                                  bool&);                     // return value: pass message downstream ?

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Streamer ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Streamer (const RPG_Net_Protocol_Streamer&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Streamer& operator= (const RPG_Net_Protocol_Streamer&))
};

#endif
