/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RPG_ENGINE_MESSAGEQUEUE_H
#define RPG_ENGINE_MESSAGEQUEUE_H

#include "ace/Global_Macros.h"
#include "ace/Message_Queue.h"
#include "ace/Synch_Traits.h"

#include "common.h"

#include "rpg_engine_exports.h"

class RPG_Engine_Export RPG_Engine_MessageQueue
 : public ACE_Message_Queue<ACE_MT_SYNCH, Common_TimePolicy_t>
{
 public:
  RPG_Engine_MessageQueue(const size_t&); // max number of queued items
  virtual ~RPG_Engine_MessageQueue();

 protected:
  // define some convenient types...
  typedef ACE_Message_Queue<ACE_MT_SYNCH,
                            Common_TimePolicy_t> MESSAGEQUEUE_TYPE;
  typedef ACE_Message_Queue_Iterator<ACE_MT_SYNCH,
                                     Common_TimePolicy_t> MESSAGEQUEUEITERATOR_TYPE;

  // *IMPORTANT NOTE*: override this so that it considers the number of enqueued
  // items (instead of the amount of enqueued bytes) to determine its water mark...
  virtual bool is_full_i(void);

 private:
  typedef ACE_Message_Queue<ACE_MT_SYNCH,
                            Common_TimePolicy_t> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_MessageQueue());
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_MessageQueue(const RPG_Engine_MessageQueue&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_MessageQueue& operator=(const RPG_Engine_MessageQueue&));
};

#endif // RPG_ENGINE_MESSAGEQUEUE_H
