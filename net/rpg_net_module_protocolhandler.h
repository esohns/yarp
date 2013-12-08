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

#include "rpg_common_timerhandler.h"
#include "rpg_common_itimer.h"

#include "rpg_stream_task_base_synch.h"
#include "rpg_stream_streammodule.h"

#include <ace/Time_Value.h>
#include <ace/Reactor.h>

// forward declaration(s)
class RPG_Stream_IAllocator;
class RPG_Net_SessionMessage;
class RPG_Net_Message;

class RPG_Net_Module_ProtocolHandler
 : public RPG_Stream_TaskBaseSynch<RPG_Net_SessionMessage,
                                   RPG_Net_Message>,
   public RPG_Common_ITimer
{
 public:
  RPG_Net_Module_ProtocolHandler();
  virtual ~RPG_Net_Module_ProtocolHandler();

  // initialization
  bool init(RPG_Stream_IAllocator*,  // message allocator
            const unsigned int&,     // session ID
            const unsigned int& = 0, // peer "ping" interval (i.e. keep-alive) [0 --> OFF]
            const bool& = true,      // automatically reply to "ping" messages (auto-"pong")
            const bool& = false);    // print dot ('.') for every received "pong" to stderr

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Message*&, // data message handle
                                 bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage(RPG_Net_SessionMessage*&, // session message handle
                                    bool&);                   // return value: pass message downstream ?

  // implement RPG_Common_ITimer
  virtual void handleTimeout(const void*); // asynchronous completion token

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef RPG_Stream_TaskBaseSynch<RPG_Net_SessionMessage,
                                   RPG_Net_Message> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_ProtocolHandler(const RPG_Net_Module_ProtocolHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_ProtocolHandler& operator=(const RPG_Net_Module_ProtocolHandler&));

  // helper methods
  RPG_Net_Message* allocateMessage(const unsigned int&); // requested size

  // timer stuff
  RPG_Common_TimerHandler myPingHandler;
  long                    myPingTimerID;

  RPG_Stream_IAllocator*  myAllocator;
  unsigned int            mySessionID;
  unsigned int            myCounter;
	unsigned int            myPingInterval;
  bool                    myAutomaticPong;
  bool                    myPrintPongDot;
  bool                    myIsInitialized;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Module_ProtocolHandler);

#endif
