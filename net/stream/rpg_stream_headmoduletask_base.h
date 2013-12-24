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

#ifndef RPG_STREAM_HEADMODULETASK_BASE_H
#define RPG_STREAM_HEADMODULETASK_BASE_H

#include "rpg_stream_task_base.h"
#include "rpg_stream_istreamcontrol.h"
#include "rpg_stream_statemachine_control.h"
#include "rpg_stream_session_message.h"
#include "rpg_stream_session_message_base.h"
#include "rpg_stream_session_config_base.h"
#include "rpg_stream_messagequeue.h"

#include <ace/Global_Macros.h>
#include <ace/Time_Value.h>
#include <ace/Synch.h>

// forward declaration(s)
class ACE_Message_Block;
class RPG_Stream_MessageBase;
class RPG_Stream_IAllocator;

template <typename DataType,
          typename SessionConfigType,
          typename SessionMessageType,
          typename ProtocolMessageType>
class RPG_Stream_HeadModuleTaskBase
 : public RPG_Stream_TaskBase<SessionMessageType,
                              ProtocolMessageType>,
   public RPG_Stream_IStreamControl,
   public RPG_Stream_StateMachine_Control
{
 public:
  virtual ~RPG_Stream_HeadModuleTaskBase();

  // override some task-based members
  virtual int put(ACE_Message_Block*, // data chunk
                  ACE_Time_Value*);   // timeout value
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int module_closed(void);
  virtual int svc(void);

  // implement (part of) RPG_Stream_ITaskBase
  // *NOTE*: these are just default NOP implementations...
  // *WARNING*: need to implement this in the base class to shut up the linker
  // about missing template instantiations...
//   virtual void handleDataMessage(RPG_Stream_MessageBase*&, // data message handle
//                                  bool&);               // return value: pass message downstream ?

  // implement RPG_Stream_IStreamControl
  virtual void start();
  virtual void stop();
  virtual void pause();
  virtual void rewind();
	// *NOTE*: for the time being, this simply waits for any worker threads to join
  virtual void waitForCompletion();
  virtual bool isRunning() const;

 protected:
  // needs to be subclassed...
  RPG_Stream_HeadModuleTaskBase(const bool& = false,  // active object ?
		                            const bool& = false); // auto-start ?

  // override: handle MB_STOP control messages to trigger shutdown
  // of the worker thread...
  virtual void handleControlMessage(ACE_Message_Block*, // control message
                                    bool&,              // return value: stop processing ?
                                    bool&);             // return value: pass message downstream ?

  // convenience methods to send (session-specific) notifications downstream
  // *WARNING*: - handle with care -
  bool putSessionMessage(const unsigned int&,                          // session ID
                         const RPG_Stream_SessionMessageType&,         // session message type
                         const DataType&,                              // data
                         const ACE_Time_Value& = ACE_Time_Value::zero, // start of session
                         const bool& = false) const;                   // user abort ?
  // *NOTE*: session message assumes lifetime responsibility for data
  // --> method implements a "fire-and-forget" strategy !
  bool putSessionMessage(const unsigned int&,                  // session ID
                         const RPG_Stream_SessionMessageType&, // session message type
                         SessionConfigType*&,                  // config data
                         RPG_Stream_IAllocator* = NULL) const; // allocator (NULL ? --> use "new")

  // implement state machine callback
  // *NOTE*: this method is threadsafe
  virtual void onStateChange(const Control_StateType&); // new state

  // *NOTE*: functionally, this does the same as stop(), with the
  // difference that stop() will wait for any worker(s)
	// --> i.e. stop() MUST NOT be called within a worker thread itself
  // so it calls this to signal an end
  virtual void finished();

  // *IMPORTANT NOTE*: children SHOULD set these during initialization !
  RPG_Stream_IAllocator*                    myAllocator;
  unsigned int                              mySessionID;
	bool                                      myIsActive;

 private:
  typedef RPG_Stream_TaskBase<SessionMessageType,
                              ProtocolMessageType> inherited;
  typedef RPG_Stream_StateMachine_Control inherited2;
  typedef RPG_Stream_HeadModuleTaskBase<DataType,
				                                SessionConfigType,
				                                SessionMessageType,
				                                ProtocolMessageType> own_type;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_HeadModuleTaskBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_HeadModuleTaskBase(const RPG_Stream_HeadModuleTaskBase<DataType,
								                                                                           SessionConfigType,
								                                                                           SessionMessageType,
								                                                                           ProtocolMessageType>&));
  // *TODO*: apparently, ACE_UNIMPLEMENTED_FUNC gets confused by template arguments...
//   ACE_UNIMPLEMENTED_FUNC(RPG_Stream_HeadModuleTaskBase<DataType,SessionConfigType,SessionMessageType>& operator=(const RPG_Stream_HeadModuleTaskBase<DataType,SessionConfigType,SessionMessageType>&));

  // allow blocking wait in waitForCompletion()
//  ACE_Recursive_Thread_Mutex                myLock;
	ACE_Thread_Mutex                          myLock;
	//  ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;
	ACE_Condition<ACE_Thread_Mutex>           myCondition;
  unsigned int                              myCurrentNumThreads;
  RPG_Stream_MessageQueue                   myQueue;
  bool                                      myAutoStart;
  DataType                                  myUserData;
};

// include template implementation
#include "rpg_stream_headmoduletask_base.i"

#endif
