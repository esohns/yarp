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

#ifndef RPG_NET_PROTOCOL_MODULE_IRCHANDLER_H
#define RPG_NET_PROTOCOL_MODULE_IRCHANDLER_H

#include "rpg_net_sessionmessage.h"

#include <stream_task_base_synch.h>
#include <stream_streammodule_base.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class Stream_IAllocator;
class Stream_MessageBase;
class RPG_Net_Protocol_Message;

class RPG_Net_Protocol_Module_IRCHandler
 : public Stream_TaskBaseSynch<RPG_Net_SessionMessage>
{
 public:
  RPG_Net_Protocol_Module_IRCHandler();
  virtual ~RPG_Net_Protocol_Module_IRCHandler();

  // initialization
  const bool init(Stream_IAllocator*,       // message allocator
                  const bool& = false,      // automatically answer "ping" messages (client)
                  const bool& = false);     // print dot ('.') for every answered PING to stdlog (client)

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(Stream_MessageBase*&, // data message handle
                                 bool&);               // return value: pass message downstream ?

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef Stream_TaskBaseSynch<RPG_Net_SessionMessage> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCHandler(const RPG_Net_Protocol_Module_IRCHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCHandler& operator=(const RPG_Net_Protocol_Module_IRCHandler&));

  // helper methods
  RPG_Net_Protocol_Message* allocateMessage(const unsigned long&); // requested size

  Stream_IAllocator* myAllocator;
  bool               myAutomaticPong;
  bool               myPrintPongDot;
  bool               myIsInitialized;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Protocol_Module_IRCHandler);

#endif
