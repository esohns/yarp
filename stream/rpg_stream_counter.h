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

#ifndef RPG_STREAM_COUNTER_H
#define RPG_STREAM_COUNTER_H

#include "rpg_stream_irefcount.h"
#include "rpg_stream_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Condition_T.h>
#include <ace/Synch.h>

class RPG_Stream_Counter
 : public RPG_Stream_IRefCount,
   public RPG_Stream_IDumpState
{
 public:
  // implement RPG_Stream_IRefCount
  virtual void increase();
  virtual void decrease();
  virtual const unsigned long refcount();
  virtual void waitcount();

  // implement RPG_Stream_IDumpState
  virtual void dump_state() const;

 protected:
  mutable ACE_Recursive_Thread_Mutex        myLock;

  // safety measures: this is meant to be a subclass !
  RPG_Stream_Counter(const unsigned long&); // initial reference count
  virtual ~RPG_Stream_Counter();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Counter(const RPG_Stream_Counter&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Counter& operator=(const RPG_Stream_Counter&));

  unsigned long                             myCounter;
  // implement blocking wait...
  ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;
};

#endif
