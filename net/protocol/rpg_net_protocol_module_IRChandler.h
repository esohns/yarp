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

#include <list>
#include <string>

#include "ace/Condition_T.h"
#include "ace/Global_Macros.h"
#include "ace/Synch.h"
#include "ace/Synch_Traits.h"

#include "common.h"

#include "stream_streammodule_base.h"
#include "stream_task_base_synch.h"

#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_iIRCControl.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_stream_common.h"

// forward declaration(s)
class Stream_IAllocator;


class RPG_Protocol_Export RPG_Net_Protocol_Module_IRCHandler
 : public Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 RPG_Net_Protocol_SessionMessage,
                                 RPG_Net_Protocol_Message>
 , public RPG_Net_Protocol_IIRCControl
{
 public:
  RPG_Net_Protocol_Module_IRCHandler ();
  virtual ~RPG_Net_Protocol_Module_IRCHandler ();

  // initialization
  bool initialize (Stream_IAllocator*, // message allocator
                   unsigned int,       // default (message) buffer size
                   bool = false,       // automatically answer "ping" messages
                   bool = false);      // print dot ('.') for every answered PING to stdlog

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (RPG_Net_Protocol_Message*&, // data message handle
                                  bool&);                     // return value: pass message downstream ?
  virtual void handleSessionMessage (RPG_Net_Protocol_SessionMessage*&, // session message handle
                                     bool&);                            // return value: pass message downstream ?

  // implement RPG_Net_Protocol_IIRCControl
  virtual void subscribe (RPG_Net_Protocol_INotify_t*); // new subscriber
  virtual void unsubscribe (RPG_Net_Protocol_INotify_t*); // existing subscriber
  virtual void registerConnection (const RPG_Net_Protocol_IRCLoginOptions&); // login details
  virtual void nick (const std::string&); // nick
  virtual void quit (const std::string&); // reason
  virtual void join (const string_list_t&,  // channel(s)
                     const string_list_t&); // key(s)
  virtual void part (const string_list_t&); // channel(s)
  virtual void mode (const std::string&,    // nick/channel
                     const char&,           // user/channel mode
                     const bool&,           // enable ?
                     const string_list_t&); // any parameters
  virtual void topic (const std::string&,  // channel
                      const std::string&); // topic
  virtual void names (const string_list_t&); // channel(s)
  virtual void list (const string_list_t&); // channel(s)
  virtual void invite (const std::string&,  // nick
                       const std::string&); // channel
  virtual void kick (const std::string&,  // channel
                     const std::string&,  // nick
                     const std::string&); // comment
  virtual void send (const string_list_t&, // receiver(s) [nick/channel]
                     const std::string&);  // message
  virtual void who (const std::string&, // name
                    const bool&);       // query ops only ?
  virtual void whois (const std::string&,    // server
                      const string_list_t&); // nickmask(s)
  virtual void whowas (const std::string&,   // nick
                       const unsigned long&, // count
                       const std::string&);  // server
  virtual void away (const std::string&); // message
  virtual void users (const std::string&); // server
  virtual void userhost (const string_list_t&); // nicknames

  // implement Common_IDumpState
  virtual void dump_state () const;

 private:
  typedef Stream_TaskBaseSynch_T<Common_TimePolicy_t,
                                 RPG_Net_Protocol_SessionMessage,
                                 RPG_Net_Protocol_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Module_IRCHandler (const RPG_Net_Protocol_Module_IRCHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Module_IRCHandler& operator=(const RPG_Net_Protocol_Module_IRCHandler&));

  // helper methods
  RPG_Net_Protocol_Message* allocateMessage(unsigned int); // requested size
  // *NOTE*: "fire-and-forget" - the argument is consumed
  void sendMessage (RPG_Net_Protocol_IRCMessage*&); // command handle

  // convenient types
  typedef std::list<RPG_Net_Protocol_INotify_t*> Subscribers_t;
  typedef Subscribers_t::iterator SubscribersIterator_t;

  // lock to protect mySubscribers and myConnectionIsAlive (!)
  // *NOTE*: make this recursive so that users may unsubscribe from within the
  // notification callbacks...
  // *WARNING*: consider race conditions here
  ACE_Recursive_Thread_Mutex             myLock;
  Subscribers_t                          mySubscribers;

  Stream_IAllocator*                     myAllocator;
  unsigned int                           myDefaultBufferSize;
  bool                                   myAutomaticPong;
  bool                                   myPrintPingPongDot;
  bool                                   myIsInitialized;

  // *NOTE*: obviously, there is a delay between connection establishment and
  // reception of the welcome NOTICE: let the users wait for it so they can
  // safely start registering connections
  ACE_Thread_Mutex                       myConditionLock;
  ACE_Thread_Condition<ACE_Thread_Mutex> myCondition;
  bool                                   myConnectionIsAlive;
  bool                                   myReceivedInitialNotice;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                        // task synch type
                              Common_TimePolicy_t,                 // time policy
                              Stream_ModuleConfiguration_t,        // configuration type
                              RPG_Net_Protocol_Module_IRCHandler); // writer type

#endif
