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

#ifndef RPG_STREAM_TASK_BASE_SYNCH_H
#define RPG_STREAM_TASK_BASE_SYNCH_H

#include "rpg_stream_task_base.h"

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

// forward declaration(s)
class ACE_Message_Block;
class ACE_Time_Value;

template <typename TimePolicyType,
          typename SessionMessageType,
          typename ProtocolMessageType>
class RPG_Stream_TaskBaseSynch
// *TODO*: figure out how to use ACE_NULL_SYNCH in this case...
 : public RPG_Stream_TaskBase<ACE_MT_SYNCH,
                              TimePolicyType,
                              SessionMessageType,
                              ProtocolMessageType>
{
 public:
  virtual ~RPG_Stream_TaskBaseSynch();

  // override some task-based members
  virtual int put(ACE_Message_Block*, // data chunk
                  ACE_Time_Value*);   // timeout value
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int module_closed(void);

  // this is a NOP, not an active object
  virtual void waitForIdleState() const;

 protected:
  RPG_Stream_TaskBaseSynch();

 private:
  typedef RPG_Stream_TaskBase<ACE_MT_SYNCH,
                              TimePolicyType,
                              SessionMessageType,
                              ProtocolMessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskBaseSynch(const RPG_Stream_TaskBaseSynch&));
//   ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskBaseSynch& operator=(const RPG_Stream_TaskBaseSynch&));
};

// include template implementation
#include "rpg_stream_task_base_synch.inl"

#endif
