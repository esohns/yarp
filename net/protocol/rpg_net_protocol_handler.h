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

#ifndef RPG_NET_PROTOCOL_HANDLER_H
#define RPG_NET_PROTOCOL_HANDLER_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_ilock.h"

#include "common_itimerhandler.h"
#include "common_time_common.h"
#include "common_timer_handler.h"

#include "stream_common.h"

#include "stream_misc_parser.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_parser_driver.h"
#include "rpg_net_protocol_session_message.h"

// forward declaration(s)
class Stream_IAllocator;

extern const char rpg_net_protocol_handler_module_name_string[];

class RPG_Net_Protocol_Handler
 : public Stream_Module_Parser_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                 Stream_ControlMessage_t,
                                 RPG_Net_Protocol_Message,
                                 RPG_Net_Protocol_SessionMessage,
                                 RPG_Net_Protocol_ParserDriver,
                                 struct Stream_UserData>
 , public Common_ITimerHandler
{
  typedef Stream_Module_Parser_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                 Stream_ControlMessage_t,
                                 RPG_Net_Protocol_Message,
                                 RPG_Net_Protocol_SessionMessage,
                                 RPG_Net_Protocol_ParserDriver,
                                 struct Stream_UserData> inherited;

 public:
// *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  RPG_Net_Protocol_Handler (ISTREAM_T*); // stream handle
#else
  RPG_Net_Protocol_Handler (typename inherited::ISTREAM_T*); // stream handle
#endif // ACE_WIN32 || ACE_WIN64
  virtual ~RPG_Net_Protocol_Handler ();

  // initialization
  virtual bool initialize (const struct RPG_Net_Protocol_ModuleHandlerConfiguration&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase
  // virtual void handleDataMessage (RPG_Net_Protocol_Message*&, // data message handle
  //                                 bool&);                     // return value: pass message downstream ?
  virtual void handleSessionMessage (RPG_Net_Protocol_SessionMessage*&, // session message handle
                                     bool&);                            // return value: pass message downstream ?

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Handler ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Handler (const RPG_Net_Protocol_Handler&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Handler& operator= (const RPG_Net_Protocol_Handler&))

  // implement Common_ITimerHandler
  inline virtual const long get_2 () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (-1); ACE_NOTREACHED (return -1;) }
  virtual void handle (const void*); // asynchronous completion token

  // implement RPG_Net_Protocol_ParserDriver
  virtual void record (struct RPG_Net_Protocol_Command&); // data record

  // helper methods
  RPG_Net_Protocol_Message* allocateMessage (unsigned int); // requested size

  // timer stuff
  Common_Timer_Handler pingHandler_;
  long                 timerId_;

  unsigned int         sessionId_;
  unsigned int         counter_;
};

#endif
