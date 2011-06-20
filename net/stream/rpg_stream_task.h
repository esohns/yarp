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

#ifndef RPG_STREAM_TASK_H
#define RPG_STREAM_TASK_H

#include "rpg_stream_itask.h"

#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Task.h>

// forward declaration(s)
class RPG_Stream_SessionMessage;
class ACE_Message_Block;
class ACE_Time_Value;

class RPG_Stream_Task
   // *TODO*: minimize locking: synchronous children should be able to use ACE_NULL_SYNCH
 : public ACE_Task<ACE_MT_SYNCH>,
   public RPG_Stream_ITask,
   public RPG_Common_IDumpState
{
 public:
  virtual ~RPG_Stream_Task();

  // override task-based members
  // *NOTE*: can't "hide" these in C++ :-(
  // --> we implement dummy stubs which SHALL be overridden...
  virtual int put(ACE_Message_Block*,
                  ACE_Time_Value*);
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int module_closed(void);
  virtual int svc(void);

  // implement (part of) RPG_Stream_ITask
  // *NOTE*: these are just default NOP implementations...
//   virtual void handleDataMessage(RPG_Stream_MessageBase*&, // data message handle
//                                  bool&);               // return value: pass this message downstream ?
  virtual void handleSessionMessage(RPG_Stream_SessionMessage*&, // session message handle
                                    bool&);                   // return value: pass this message downstream ?
  virtual void handleProcessingError(const ACE_Message_Block* const); // message handle

  // implement RPG_Common_IDumpState
  // *NOTE*: just a default implementation...
  virtual void dump_state() const;

 protected:
  // needs to be subclassed...
  RPG_Stream_Task();

  // helper methods
  // standard message handling (to be used by both asynch/synch children)
  void handleMessage(ACE_Message_Block*, // message handle
                     bool&);             // return value: stop processing ?

  // default implementation to handle control messages
  virtual void handleControlMessage(ACE_Message_Block*, // control message
                                    bool&,              // return value: stop processing ?
                                    bool&);             // return value: pass message downstream ?

 private:
  typedef ACE_Task<ACE_MT_SYNCH> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Task(const RPG_Stream_Task&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Task& operator=(const RPG_Stream_Task&));
};

#endif