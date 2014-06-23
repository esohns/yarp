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

#include "rpg_common_task_base.h"

// forward declaration(s)
class RPG_Stream_MessageBase;

template <typename TaskSynchStrategyType,
          typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType>
class RPG_Stream_TaskBase
 : public RPG_Common_TaskBase<TaskSynchStrategyType,
                              TimePolicyType>,
   public RPG_Stream_ITaskBase<SessionMessageType,
                               ProtocolMessageType>
{
 public:
  virtual ~RPG_Stream_TaskBase();

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
  // *NOTE*: this is just a default stub...
  virtual void dump_state() const;

 protected:
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
  typedef RPG_Common_TaskBase<TaskSynchStrategyType,
                              TimePolicyType> inherited;
  typedef RPG_Stream_ITaskBase<SessionMessageType,
                               ProtocolMessageType> inherited2;

  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskBase(const RPG_Stream_TaskBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskBase& operator=(const RPG_Stream_TaskBase&));
};

// include template implementation
#include "rpg_stream_task_base.inl"

#endif
