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

#ifndef RPG_STREAM_TASK_ASYNCH_H
#define RPG_STREAM_TASK_ASYNCH_H

#include "rpg_stream_task.h"
#include "rpg_stream_messagequeue.h"

#include <ace/Global_Macros.h>

// *NOTE*: the message queue needs to be synched so that shutdown can be
// asynchronous...
class RPG_Stream_TaskAsynch
 : public RPG_Stream_Task
{
 public:
  virtual ~RPG_Stream_TaskAsynch();

  // override task-based members
  virtual int put(ACE_Message_Block*, // data chunk
                  ACE_Time_Value*);   // timeout value
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int module_closed(void);
  virtual int svc(void);

  // delegate to myQueue !
  virtual void waitForIdleState() const;

 protected:
  // needs to be subclassed...
  RPG_Stream_TaskAsynch();

//   ACE_hthread_t       myThreadID;
  ACE_thread_t        myThreadID;

 private:
  typedef RPG_Stream_Task inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskAsynch(const RPG_Stream_TaskAsynch&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_TaskAsynch& operator=(const RPG_Stream_TaskAsynch&));

  // helper methods
  // enqueue MB_STOP --> stop worker thread
  // *WARNING*: handle with EXTREME care, you should NEVER use this directly
  // if in stream context (i.e. task/module is part of a stream)
  void shutdown();

  RPG_Stream_MessageQueue myQueue;
};

#endif
