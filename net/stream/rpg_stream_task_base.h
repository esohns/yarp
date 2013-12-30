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

#ifndef RPG_STREAM_TASK_BASE_H
#define RPG_STREAM_TASK_BASE_H

#include "rpg_stream_itask_base.h"

#include "rpg_common_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Task.h>

// forward declaration(s)
class ACE_Message_Block;
class ACE_Time_Value;
class RPG_Stream_MessageBase;

template <typename TaskSynchStrategyType,
          typename SessionMessageType,
          typename ProtocolMessageType>
class RPG_Stream_TaskBase
 : public ACE_Task<TaskSynchStrategyType>,
   public RPG_Stream_ITaskBase<SessionMessageType,
                               ProtocolMessageType>,
   public RPG_Common_IDumpState
{
 public:
  virtual ~RPG_Stream_TaskBase();

  // override task-based members
  // *NOTE*: can't "hide" these in C++ :-(
  // --> implement dummy stubs which SHALL be overridden...
  virtual int put(ACE_Message_Block*,
                  ACE_Time_Value*);
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int module_closed(void);
  virtual int svc(void);

  // implement RPG_Stream_ITaskBase<SessionMessageType>
  // *NOTE*: these are just default NOP implementations...
  // *WARNING*: need to implement this one to shut up the compiler/linker about
  // missing template declarations/instantiations...
//   virtual void handleDataMessage(ProtocolMessageType*&, // data message handle
//                                  bool&);                // return value: pass message downstream ?
  virtual void handleSessionMessage(SessionMessageType*&, // session message handle
                                    bool&);               // return value: pass this message downstream ?
  virtual void handleProcessingError(const ACE_Message_Block* const); // message handle

  // implement RPG_Common_IDumpState
  // *NOTE*: just a default implementation...
  virtual void dump_state() const;

 protected:
  // needs to be subclassed...
  RPG_Stream_TaskBase();

  // helper methods
  // standard message handling (to be used by both asynch/synch children !!!)
  void handleMessage(ACE_Message_Block*, // message handle
                     bool&);             // return value: stop processing ?

  // default implementation to handle control messages
  virtual void handleControlMessage(ACE_Message_Block*, // control message
                                    bool&,              // return value: stop processing ?
                                    bool&);             // return value: pass message downstream ?

 private:
  typedef ACE_Task<TaskSynchStrategyType> inherited;
  typedef RPG_Stream_ITaskBase<SessionMessageType,
                               ProtocolMessageType> inherited2;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskBase(const RPG_Stream_TaskBase<TaskSynchStrategyType,
                                                                       SessionMessageType,
                                                                       ProtocolMessageType>&));
//   ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskBase<SessionMessageType,
//                                          ProtocolMessageType>& operator=(const RPG_Stream_TaskBase<SessionMessageType,
//                                                                                                ProtocolMessageType>&));
};

// include template implementation
#include "rpg_stream_task_base.i"

#endif
