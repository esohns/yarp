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

#ifndef RPG_STREAM_MESSAGEQUEUE_BASE_H
#define RPG_STREAM_MESSAGEQUEUE_BASE_H

#include "rpg_stream_imessagequeue.h"

#include "rpg_common_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Message_Queue.h>

template <typename TaskSynchType,
          typename TimePolicyType>
class RPG_Stream_MessageQueueBase
 : public ACE_Message_Queue<TaskSynchType,
                            TimePolicyType>,
   public RPG_Stream_IMessageQueue,
   public RPG_Common_IDumpState
{
 public:
  RPG_Stream_MessageQueueBase(const unsigned int&); // max number of queued items
  virtual ~RPG_Stream_MessageQueueBase();

  // implement RPG_Common_IDumpState
  // *IMPORTANT NOTE*: children SHOULD override this...
  virtual void dump_state() const;

 protected:
  // convenient types...
  typedef ACE_Message_Queue<TaskSynchType,
                            TimePolicyType> MESSAGEQUEUE_T;
  typedef ACE_Message_Queue_Iterator<TaskSynchType,
                                     TimePolicyType> MESSAGEQUEUEITERATOR_T;
  typedef RPG_Stream_MessageQueueBase<TaskSynchType,
                                      TimePolicyType> own_type;

  // *IMPORTANT NOTE*: override this so that the queue considers the number of
  // enqueued items (instead of the amount of enqueued bytes) to determine its
  // water mark...
  virtual bool is_full_i(void);

 private:
  typedef ACE_Message_Queue<TaskSynchType,
                            TimePolicyType> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageQueueBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageQueueBase(const RPG_Stream_MessageQueueBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageQueueBase& operator=(const RPG_Stream_MessageQueueBase&));
};

// include template definition
#include "rpg_stream_messagequeue_base.inl"

#endif
