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

#ifndef RPG_COMMON_REFERENCECOUNTER_BASE_H
#define RPG_COMMON_REFERENCECOUNTER_BASE_H

#include "rpg_common_exports.h"
#include "rpg_common_irefcount.h"
#include "rpg_common_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Condition_T.h>
#include <ace/Synch.h>

class RPG_Common_Export RPG_Common_ReferenceCounterBase
 : virtual public RPG_Common_IRefCount,
   public RPG_Common_IDumpState
{
 public:
  virtual ~RPG_Common_ReferenceCounterBase();

  // implement RPG_Common_IRefCount
  virtual void increase();
  virtual void decrease();
  virtual unsigned int count();
  virtual void wait_zero();

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 protected:
  // *WARNING*: "delete on 0" may not work predictably if there are
  // any waiters (or in ANY multithreaded context, for that matter)...
  RPG_Common_ReferenceCounterBase(const unsigned int& = 1, // initial reference count
                                  const bool& = true);     // delete on 0 ?

  mutable ACE_Recursive_Thread_Mutex        myLock;
  unsigned int                              myCounter;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ReferenceCounterBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ReferenceCounterBase(const RPG_Common_ReferenceCounterBase&););
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ReferenceCounterBase& operator=(const RPG_Common_ReferenceCounterBase&));

  bool                                      myDeleteOnZero;
  ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;
};

#endif
