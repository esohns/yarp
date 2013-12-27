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

#ifndef RPG_STREAM_STATEMACHINE_CONTROL_H
#define RPG_STREAM_STATEMACHINE_CONTROL_H

#include "rpg_stream_exports.h"

#include <ace/Global_Macros.h>
#include <ace/Synch.h>
#include <ace/Condition_T.h>

#include <string>

class RPG_Stream_Export RPG_Stream_StateMachine_Control
{
 public:
  enum Control_StateType
  {
    INIT = 0,
    RUNNING,
    PAUSED,
    STOPPED,
    FINISHED
  };

  RPG_Stream_StateMachine_Control();
  virtual ~RPG_Stream_StateMachine_Control();

  // info
  static void ControlState2String(const Control_StateType&, // state
                                  std::string&);            // return value: state string

 protected:
  // only children can retrieve state
  const RPG_Stream_StateMachine_Control::Control_StateType getState() const;

  // only children can change state
  // *WARNING*: PAUSED --> PAUSED is silently remapped to PAUSED --> RUNNING
  // in order to resemble a traditional tape recorder...
  // --> children must implement the corresponding behavior !
  bool changeState(const Control_StateType&); // new state

  // callback invoked on change of state
  // *TODO*: make this an interface !
  virtual void onStateChange(const Control_StateType&) = 0; // new state

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StateMachine_Control(const RPG_Stream_StateMachine_Control&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StateMachine_Control& operator=(const RPG_Stream_StateMachine_Control&));

  // helper method
  // *IMPORTANT NOTE*: this method needs to be called with the lock held !
  void invokeCallback(const Control_StateType&); // new state

  // current state
  Control_StateType                  myState;
  // *IMPORTANT NOTE*: this MUST be recursive, so children can retrieve current
  // state from within onStateChange without deadlocking !
  //ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;
  mutable ACE_Recursive_Thread_Mutex myLock;
};

typedef RPG_Stream_StateMachine_Control::Control_StateType RPG_Stream_Control_StateType;

#endif
