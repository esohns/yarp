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

#include "rpg_net_sessionmessage.h"
#include "rpg_net_remote_comm.h"

#include <stream_task_base_synch.h>
#include <stream_streammodule_base.h>

#include <ace/Time_Value.h>
#include <ace/Reactor.h>

// forward declaration(s)
class Stream_IAllocator;
class Stream_MessageBase;

class RPG_Net_Module_ProtocolHandler
 : public Stream_TaskBaseSynch<RPG_Net_SessionMessage>
{
 public:
  RPG_Net_Module_ProtocolHandler();
  virtual ~RPG_Net_Module_ProtocolHandler();

  // initialization
  const bool init(Stream_IAllocator*,       // message allocator
                  const unsigned long& = 0, // client "ping" interval (server) [0 --> OFF]
                  const bool& = false,      // automatically answer "ping" messages (client)
                  const bool& = false);     // print dot ('.') for every answered PING to stderr (client)

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(Stream_MessageBase*&, // data message handle
                                 bool&);               // return value: pass message downstream ?
//   virtual void handleSessionMessage(RPG_Net_SessionMessage*&, // session message handle
//                                     bool&);                   // return value: pass message downstream ?

  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef Stream_TaskBaseSynch<RPG_Net_SessionMessage> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_ProtocolHandler(const RPG_Net_Module_ProtocolHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_ProtocolHandler& operator=(const RPG_Net_Module_ProtocolHandler&));

  // helper methods
  RPG_Net_Message* allocateMessage(const unsigned long&); // requested size
  inline void cancelTimer()
  {
    if (myTimerID != -1)
    {
      if (reactor()->cancel_timer(this,    // handler
                                  1) != 1) // don't call handle_close()
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%m\", continuing\n")));
      } // end IF
      myTimerID = -1;
    } // end IF
  };

  Stream_IAllocator* myAllocator;
  int                myTimerID;
  unsigned long      myCounter;
  bool               myAutomaticPong;
  bool               myPrintPongDot;
  bool               myIsInitialized;

//   unsigned long      mySessionID;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Module_ProtocolHandler);

#endif
