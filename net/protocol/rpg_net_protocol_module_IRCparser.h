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

#ifndef RPG_NET_PROTOCOL_MODULE_IRCPARSER_H
#define RPG_NET_PROTOCOL_MODULE_IRCPARSER_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common.h"

#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_IRCparser_driver.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_stream_common.h"

// forward declaration(s)
class Stream_IAllocator;

class RPG_Net_Protocol_Module_IRCParser
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 RPG_Net_Protocol_SessionMessage,
                                 RPG_Net_Protocol_Message>
{
 public:
  RPG_Net_Protocol_Module_IRCParser ();
  virtual ~RPG_Net_Protocol_Module_IRCParser ();

  // configuration / initialization
  bool initialize (Stream_IAllocator*,                          // message allocator
                   bool = RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES, // crunch messages ?
                   bool = RPG_NET_PROTOCOL_DEF_TRACE_SCANNING,  // debug scanner ?
                   bool = RPG_NET_PROTOCOL_DEF_TRACE_PARSING);  // debug parser ?

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (RPG_Net_Protocol_Message*&, // data message handle
                                  bool&);                     // return value: pass message downstream ?

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 RPG_Net_Protocol_SessionMessage,
                                 RPG_Net_Protocol_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Module_IRCParser (const RPG_Net_Protocol_Module_IRCParser&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Module_IRCParser& operator=(const RPG_Net_Protocol_Module_IRCParser&));

  // helper methods
  RPG_Net_Protocol_Message* allocateMessage (unsigned int); // requested size

  // message allocator
  Stream_IAllocator*               myAllocator;

  // driver
  bool                             myDebugScanner;
  bool                             myDebugParser;
  RPG_Net_Protocol_IRCParserDriver myDriver;

  bool                             myCrunchMessages;
  bool                             myIsInitialized;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                       // task synch type
                              Common_TimePolicy_t,                // time policy
                              RPG_Net_Protocol_Configuration,     // configuration type
                              RPG_Net_Protocol_Module_IRCParser); // writer type

#endif
