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

#ifndef RPG_NET_MODULE_PROTOCOLHANDLER_H
#define RPG_NET_MODULE_PROTOCOLHANDLER_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_ilock.h"

#include "common_itimerhandler.h"
#include "common_time_common.h"
#include "common_timer_handler.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

#include "rpg_net_protocol_session_message.h"
#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"

// forward declaration(s)
class Stream_IAllocator;

extern const char rpg_net_protocolhandler_module_name_string[];

class RPG_Net_ProtocolHandler
 : public Stream_TaskBaseSynch_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                 Stream_ControlMessage_t,
                                 RPG_Net_Protocol_Message,
                                 RPG_Net_Protocol_SessionMessage,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData>
 , public Common_ITimerHandler
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
  RPG_Net_ProtocolHandler (ISTREAM_T*); // stream handle
  virtual ~RPG_Net_ProtocolHandler ();

  // initialization
  bool initialize (Stream_IAllocator*, // message allocator
                   unsigned int = 0,   // peer "ping" interval (ms) [0 --> OFF]
                   bool = true,        // automatically reply to "ping" messages (auto-"pong")
                   bool = false);      // print dot ('.') for every received "pong" to stdlog

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (RPG_Net_Protocol_Message*&, // data message handle
                                  bool&);                     // return value: pass message downstream ?
  virtual void handleSessionMessage (RPG_Net_Protocol_SessionMessage*&, // session message handle
                                     bool&);                            // return value: pass message downstream ?

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_ProtocolHandler ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_ProtocolHandler (const RPG_Net_ProtocolHandler&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_ProtocolHandler& operator= (const RPG_Net_ProtocolHandler&))

  // implement Common_ITimerHandler
  inline virtual const long get_2 () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
  virtual void handle (const void*); // asynchronous completion token

  // helper methods
  RPG_Net_Protocol_Message* allocateMessage (unsigned int); // requested size

  // timer stuff
  Common_Timer_Handler pingHandler_;
  long                 timerId_;

  //Stream_IAllocator*   allocator_;
  unsigned int         sessionId_;
  unsigned int         counter_;
  unsigned int         pingInterval_;
  bool                 automaticPong_;
  bool                 printPongDot_;
  bool                 isInitialized_;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (struct RPG_Net_Protocol_SessionData,        // session data type
                              enum Stream_SessionMessageType,             // session event type
                              struct RPG_Net_Protocol_ModuleHandlerConfiguration,  // module handler configuration type
                              rpg_net_protocolhandler_module_name_string,
                              Stream_INotify_t,                           // stream notification interface type
                              RPG_Net_ProtocolHandler);                   // writer type

#endif
