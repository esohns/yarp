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

#include "rpg_net_protocol_iIRCControl.h"

#include <stream_task_base_synch.h>
#include <stream_streammodule.h>

#include <ace/Global_Macros.h>

#include <string>

// forward declaration(s)
class Stream_IAllocator;
class RPG_Net_Protocol_SessionMessage;
class RPG_Net_Protocol_Message;
class RPG_Net_Protocol_Stream;
class RPG_Net_Protocol_IRCMessage;

class RPG_Net_Protocol_Module_IRCHandler
 : public Stream_TaskBaseSynch<RPG_Net_Protocol_SessionMessage,
                               RPG_Net_Protocol_Message>,
   public RPG_Net_Protocol_IIRCControl
{
//   // grant access to init()
//   friend class RPG_Net_Protocol_Stream;

 public:
  RPG_Net_Protocol_Module_IRCHandler();
  virtual ~RPG_Net_Protocol_Module_IRCHandler();

  // initialization
  const bool init(Stream_IAllocator*,        // message allocator
                  const unsigned long&,      // default (message) buffer size
                  const bool& = false,       // automatically answer "ping" messages (client)
                  const bool& = false);      // print dot ('.') for every answered PING to stdlog (client)

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Protocol_Message*&, // data message handle
                                 bool&);                     // return value: pass message downstream ?

  // implement RPG_Net_Protocol_IIRCControl
  virtual void joinIRC(const RPG_Net_Protocol_IRCLoginOptions&, // login details
                       const RPG_Net_Protocol_INotify*);        // data callback
  virtual void registerNotification(const RPG_Net_Protocol_INotify*); // data
  virtual void sendMessage(const std::string&); // message
  virtual void leaveIRC(const std::string&); // reason

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef Stream_TaskBaseSynch<RPG_Net_Protocol_SessionMessage,
                               RPG_Net_Protocol_Message> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCHandler(const RPG_Net_Protocol_Module_IRCHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCHandler& operator=(const RPG_Net_Protocol_Module_IRCHandler&));

  // helper methods
  RPG_Net_Protocol_Message* allocateMessage(const unsigned long&); // requested size
  // *NOTE*: "fire-and-forget" - the argument is consumed
  void sendMessage(RPG_Net_Protocol_IRCMessage*&); // command handle

  RPG_Net_Protocol_INotify* myDataSink;
  std::string               myChannelName;
  Stream_IAllocator*        myAllocator;
  unsigned long             myDefaultBufferSize;
  bool                      myAutomaticPong;
  bool                      myPrintPingPongDot;
  bool                      myIsInitialized;
  bool                      myReceivedInitialNotice;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Protocol_Module_IRCHandler);

#endif
