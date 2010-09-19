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

#include "rpg_stream_statemachine_control.h"

RPG_Stream_StateMachine_Control::RPG_Stream_StateMachine_Control()
 : myState(RPG_Stream_StateMachine_Control::INIT)
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StateMachine_Control::RPG_Stream_StateMachine_Control"));

}

RPG_Stream_StateMachine_Control::~RPG_Stream_StateMachine_Control()
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StateMachine_Control::~RPG_Stream_StateMachine_Control"));

}

const RPG_Stream_StateMachine_Control::Control_StateType
RPG_Stream_StateMachine_Control::getState() const
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StateMachine_Control::getState"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  return myState;
}

const bool
RPG_Stream_StateMachine_Control::changeState(const Control_StateType& newState_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StateMachine_Control::changeState"));

  // synchronize access to state machine...
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(myLock);

  switch (myState)
  {
    case RPG_Stream_StateMachine_Control::INIT:
    {
      switch (newState_in)
      {
        // good case
        case RPG_Stream_StateMachine_Control::RUNNING:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("state switch: INIT --> RUNNING\n")));

          invokeCallback(newState_in);

          myState = newState_in;

          return true;
        }
        // error case
        case RPG_Stream_StateMachine_Control::INIT:
        case RPG_Stream_StateMachine_Control::PAUSED:
        case RPG_Stream_StateMachine_Control::STOPPED:
        case RPG_Stream_StateMachine_Control::FINISHED:
        default:
        {
          // what else can we do ?

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Stream_StateMachine_Control::RUNNING:
    {
      switch (newState_in)
      {
        // good case
        case RPG_Stream_StateMachine_Control::PAUSED:
        case RPG_Stream_StateMachine_Control::STOPPED:
        case RPG_Stream_StateMachine_Control::FINISHED:
        {
          std::string newStateString;
          ControlState2String(newState_in,
                              newStateString);
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("state switch: RUNNING --> %s\n"),
//                      newStateString.c_str()));

          invokeCallback(newState_in);

          myState = newState_in;

          return true;
        }
        // error case
        case RPG_Stream_StateMachine_Control::INIT:
        default:
        {
          // what else can we do ?

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Stream_StateMachine_Control::PAUSED:
    {
      switch (newState_in)
      {
        // good case
        case RPG_Stream_StateMachine_Control::PAUSED: // just like a tape-recorder...
        case RPG_Stream_StateMachine_Control::RUNNING: // ...but we also allow this to resume
        case RPG_Stream_StateMachine_Control::STOPPED:
        {
          // need to handle a special case: PAUSED --> PAUSED is logically mapped to
          // PAUSED --> RUNNING, just like a tape recorder...
          // *IMPORTANT NOTE*: make sure our children are aware of this behaviour !!!
          Control_StateType newState = (newState_in == RPG_Stream_StateMachine_Control::PAUSED) ?
                                                       RPG_Stream_StateMachine_Control::RUNNING :
                                                       newState_in;

          std::string newStateString;
          ControlState2String(newState,
                              newStateString);
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("state switch: PAUSED --> %s\n"),
                     newStateString.c_str()));

          invokeCallback(newState);

          myState = newState;

          return true;
        }
        // error case
        case RPG_Stream_StateMachine_Control::INIT:
        case RPG_Stream_StateMachine_Control::FINISHED:
        default:
        {
          // what else can we do ?

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Stream_StateMachine_Control::STOPPED:
    {
      switch (newState_in)
      {
        // good cases
        // *NOTE*: we have to allow this...
        // (scenario: asynchronous user abort via stop())
        case RPG_Stream_StateMachine_Control::FINISHED:
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("state switch: STOPPED --> FINISHED\n")));

          invokeCallback(newState_in);

          myState = newState_in;

          return true;
        }
        // error cases
        case RPG_Stream_StateMachine_Control::INIT:
        case RPG_Stream_StateMachine_Control::PAUSED:
        case RPG_Stream_StateMachine_Control::RUNNING:
        default:
        {
          // what else can we do ?

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Stream_StateMachine_Control::FINISHED:
    {
      switch (newState_in)
      {
        // *IMPORTANT NOTE*: the whole stream needs to re-initialize BEFORE this happens...
        // good case
        case RPG_Stream_StateMachine_Control::RUNNING:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("state switch: FINISHED --> RUNNING\n")));

          invokeCallback(newState_in);

          myState = newState_in;

          return true;
        }
        // error case
        case RPG_Stream_StateMachine_Control::INIT:
        case RPG_Stream_StateMachine_Control::PAUSED:
        case RPG_Stream_StateMachine_Control::STOPPED:
        default:
        {
          // what else can we do ?

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      // what else can we do ?

      break;
    }
  } // end SWITCH

  // Note: when we get here, an invalid state change happened... --> check implementation !!!!
  std::string currentStateString;
  std::string newStateString;
  ControlState2String(myState,
                      currentStateString);
  ControlState2String(newState_in,
                      newStateString);

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("invalid state switch: \"%s\" --> \"%s\" --> check implementation !, returning\n"),
             currentStateString.c_str(),
             newStateString.c_str()));

  return false;
}

void
RPG_Stream_StateMachine_Control::invokeCallback(const Control_StateType& newState_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StateMachine_Control::invokeCallback"));

  // invoke callback...
  try
  {
    onStateChange(newState_in);
  }
  catch (...)
  {
    std::string currentStateString;
    std::string newStateString;
    ControlState2String(myState,
                        currentStateString);
    ControlState2String(newState_in,
                        newStateString);

    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_StateMachine_Control::onStateChange: \"%s --> %s\", continuing\n"),
               currentStateString.c_str(),
               newStateString.c_str()));
  }
}

void
RPG_Stream_StateMachine_Control::ControlState2String(const Control_StateType& state_in,
                                                  std::string& stateString_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StateMachine_Control::ControlState2String"));

  // init return value(s)
  stateString_out = ACE_TEXT("UNDEFINED_STATE");
  switch (state_in)
  {
    case RPG_Stream_StateMachine_Control::INIT:
    {
      stateString_out = ACE_TEXT("INIT");

      break;
    }
    case RPG_Stream_StateMachine_Control::RUNNING:
    {
      stateString_out = ACE_TEXT("RUNNING");

      break;
    }
    case RPG_Stream_StateMachine_Control::PAUSED:
    {
      stateString_out = ACE_TEXT("PAUSED");

      break;
    }
    case RPG_Stream_StateMachine_Control::STOPPED:
    {
      stateString_out = ACE_TEXT("STOPPED");

      break;
    }
    case RPG_Stream_StateMachine_Control::FINISHED:
    {
      stateString_out = ACE_TEXT("FINISHED");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid state: %d, aborting\n"),
                 state_in));

      break;
    }
  } // end SWITCH
}
