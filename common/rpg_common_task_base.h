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

#ifndef RPG_COMMON_TASK_BASE_H
#define RPG_COMMON_TASK_BASE_H

#include "rpg_common_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Task.h>

#include <string>

// forward declaration(s)
class ACE_Message_Block;
class ACE_Time_Value;

template <typename TaskSynchStrategyType,
          typename TimePolicyType>
class RPG_Common_TaskBase
 : public ACE_Task<TaskSynchStrategyType,
                   TimePolicyType>,
   public RPG_Common_IDumpState
{
 public:
  virtual ~RPG_Common_TaskBase();

  // override ACE_Task_Base members
  virtual int close(u_long = 0);

  // implement RPG_Common_IDumpState
  // *NOTE*: this is just a default stub...
  virtual void dump_state() const;

 protected:
  RPG_Common_TaskBase(const std::string&,      // thread name
                      const int&,              // thread group id
                      const unsigned int& = 1, // # thread(s)
                      const bool& = true);     // auto-start ?

  // override ACE_Task_Base members
  virtual int open(void* = NULL);

  // helper methods
  void shutdown();

 private:
  typedef ACE_Task<TaskSynchStrategyType,
                   TimePolicyType> inherited;

  // override/hide ACE_Task_Base members
  virtual int put(ACE_Message_Block*,
                  ACE_Time_Value*);
  virtual int svc(void);
  virtual int module_closed(void);

  ACE_UNIMPLEMENTED_FUNC(RPG_Common_TaskBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_TaskBase(const RPG_Common_TaskBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_TaskBase& operator=(const RPG_Common_TaskBase&));

  std::string  myThreadName;
  unsigned int myNumThreads;
};

// include template implementation
#include "rpg_common_task_base.inl"

#endif
