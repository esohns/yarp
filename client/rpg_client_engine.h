/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef RPG_CLIENT_ENGINE_H
#define RPG_CLIENT_ENGINE_H

#include "rpg_client_common.h"

#include <rpg_engine_common.h>
#include <rpg_engine_iwindow.h>

#include <rpg_graphics_iwindow.h>

#include <rpg_common_icontrol.h>
#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Task.h>
#include <ace/Atomic_Op_T.h>
#include <ace/Condition_T.h>
#include <ace/Synch.h>

// forward declarations
class RPG_Engine_Level;

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Client_Engine
 : public ACE_Task<ACE_MT_SYNCH>,
   public RPG_Common_IControl,
   public RPG_Common_IDumpState,
   public RPG_Engine_IWindow
{
 public:
  RPG_Client_Engine();
  // *WARNING*: UI handle needs to be of WINDOW_MAP type !!!
  RPG_Client_Engine(RPG_Engine_Level*,      // level state
                    RPG_Graphics_IWindow*); // UI handle
  virtual ~RPG_Client_Engine();

  // implement RPG_Common_IControl
  virtual void start();
  virtual void stop();
  virtual const bool isRunning();

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

  // implement RPG_Engine_IWindow
  // *NOTE*: these are called by the level engine ONLY
  virtual void init();
  virtual void redraw();
  virtual void toggleDoor(const RPG_Map_Position_t&);
  virtual void center(const RPG_Map_Position_t&);

  // *WARNING*: UI handle needs to be of WINDOW_MAP type !!!
  void init(RPG_Engine_Level*,      // level state
            RPG_Graphics_IWindow*); // UI handle
  void action(const RPG_Client_Action&); // action

 private:
  typedef ACE_Task<ACE_MT_SYNCH> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Engine(const RPG_Client_Engine&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Engine& operator=(const RPG_Client_Engine&));

  // override task-based members
//   virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int svc(void);

  // process actions
  void handleActions();

  // *IMPORTANT NOTE*: need this ONLY to handle control messages...
//   RPG_Engine_MessageQueue         myQueue;

  // make our API re-entrant
  mutable ACE_Thread_Mutex        myLock;
  // implement blocking wait...
  ACE_Condition<ACE_Thread_Mutex> myCondition;

  bool                            myStop;

  RPG_Engine_Level*               myLevelState;
  RPG_Graphics_IWindow*           myLevelWindow;

  RPG_Client_Actions_t            myActions;
};

#endif
