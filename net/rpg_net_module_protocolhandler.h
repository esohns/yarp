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

//#include "ace/Reactor.h"
#include "ace/Synch_Traits.h"
//#include "ace/Time_Value.h"

#include "common.h"
#include "common_itimer.h"
#include "common_timerhandler.h"

#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

#include "net_configuration.h"

// forward declaration(s)
class Stream_IAllocator;
class Net_SessionMessage;
class Net_Message;

class RPG_Net_Module_ProtocolHandler
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 Net_SessionMessage,
                                 Net_Message>
 , public Common_ITimer
{
 public:
  RPG_Net_Module_ProtocolHandler ();
  virtual ~RPG_Net_Module_ProtocolHandler ();

  // initialization
  bool initialize (Stream_IAllocator*, // message allocator
                   unsigned int,       // session ID
                   unsigned int = 0,   // peer "ping" interval (ms) [0 --> OFF]
                   bool = true,        // automatically reply to "ping" messages (auto-"pong")
                   bool = false);      // print dot ('.') for every received "pong" to stdlog

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (Net_Message*&, // data message handle
                                  bool&);        // return value: pass message downstream ?
  virtual void handleSessionMessage (Net_SessionMessage*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement Common_ITimer
  virtual void handleTimeout (const void*); // asynchronous completion token

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 Net_SessionMessage,
                                 Net_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Module_ProtocolHandler (const RPG_Net_Module_ProtocolHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Module_ProtocolHandler& operator=(const RPG_Net_Module_ProtocolHandler&));

  // helper methods
  Net_Message* allocateMessage (unsigned int); // requested size

  // timer stuff
  Common_TimerHandler myPingHandler;
  long                myPingTimerID;

  Stream_IAllocator*  myAllocator;
  unsigned int        mySessionID;
  unsigned int        myCounter;
  unsigned int        myPingInterval;
  bool                myAutomaticPong;
  bool                myPrintPongDot;
  bool                myIsInitialized;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY(ACE_MT_SYNCH,                    // task synch type
                             Common_TimePolicy_t,             // time policy type
                             Net_Configuration_t,             // configuration type
                             RPG_Net_Module_ProtocolHandler); // writer type

#endif
