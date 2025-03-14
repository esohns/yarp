﻿/***************************************************************************
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

#include "ace/Atomic_Op_T.h"
#include "ace/Global_Macros.h"
#include "ace/Message_Queue_T.h"
#include "ace/Synch_Traits.h"
#include "ace/Task.h"

#include "common_idumpstate.h"
#include "common_ilock.h"
#include "common_task_base.h"
#include "common_time_common.h"

#include "rpg_graphics_iwindow_base.h"
#include "rpg_graphics_style.h"

#include "rpg_engine_common.h"
#include "rpg_engine_iclient.h"

#include "rpg_client_common.h"

// forward declaration(s)
class RPG_Engine;

class RPG_Client_Engine
 : public Common_TaskBase_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_MT_SYNCH>,
                            ACE_Task<ACE_MT_SYNCH> >
 , public RPG_Engine_IClient
{
  typedef Common_TaskBase_T<ACE_MT_SYNCH,
                            Common_TimePolicy_t,
                            ACE_Message_Block,
                            ACE_Message_Queue<ACE_MT_SYNCH>,
                            ACE_Task<ACE_MT_SYNCH> > inherited;

 public:
  RPG_Client_Engine ();
  virtual ~RPG_Client_Engine ();

  // implement Common_IControl
  inline virtual void stop (bool waitForCompletion_in = true, bool = false) { shutDown_ = true; condition_.broadcast (); if (waitForCompletion_in) inherited::wait (false); }
  inline virtual bool isShuttingDown () const { return shutDown_; }

  // implement Common_IDumpState
  virtual void dump_state () const;

  // implement Common_ILock
  virtual bool lock (bool = true); // block ?
  virtual int unlock (bool = false); // unlock completely ?

  void redraw (bool = true); // refresh ?
  // *TODO* these need consideration/redesign
  inline void setStyle (const struct RPG_Graphics_Style& style_in) { state_.style = style_in; }
  inline struct RPG_Graphics_Style getStyle () const { return state_.style; }
  void setView (const RPG_Map_Position_t&,
                bool = true); // refresh ?

  // implement RPG_Engine_IClient
  // *NOTE* this translates RPG_Engine_Command + parameters to one ore more
  // RPG_Client_Action(s)
  // *NOTE*: currently handles COMMAND_E2C_xxx types and
  //         COMMAND_DOOR_OPEN|COMMAND_DOOR_CLOSE ONLY
  virtual void notify (enum RPG_Engine_Command,
                       const struct RPG_Engine_ClientNotificationParameters&,
                       bool = true); // lock (engine) ?
  virtual void notify (const struct RPG_Engine_Action&); // action

  // *WARNING*: window handle needs to be of WINDOW_MAP type !!!
  void initialize (RPG_Engine*,               // (level) state
                   RPG_Graphics_IWindowBase*, // window handle (may be NULL iff server session)
                   //RPG_Client_IWidgetUI_t*,   // widget UI interface handle
                   bool = false);             // debug ?
  inline RPG_Graphics_IWindowBase* window () { return window_; }

  // local client|client
  void action (const RPG_Client_Action&); // action
  // network
  void inject (const RPG_Client_Action&); // action

  inline void mode (enum RPG_Client_SelectionMode mode_in) { selectionMode_ = mode_in; }
  inline void clear () { selectionMode_ = SELECTIONMODE_NORMAL; }
  inline enum RPG_Client_SelectionMode mode () const { return selectionMode_; }

  void centerOnActive (bool); // keep active player centered ?
  bool getCenterOnActive () const; // return value: keep active player centered ?

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Engine (const RPG_Client_Engine&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Engine& operator=(const RPG_Client_Engine&));

  // override task-based members
  virtual int close (u_long = 0);
  virtual int svc (void);

  // process actions
  void handleActions ();

  // make API re-entrant
  mutable ACE_Thread_Mutex        lock_;
  // implement blocking wait...
  ACE_Condition<ACE_Thread_Mutex> condition_;

  bool                            shutDown_;

  RPG_Engine*                     engine_;
  RPG_Graphics_IWindowBase*       window_;
  //RPG_Client_IWidgetUI_t*         myWidgetInterface;

  RPG_Client_Actions_t            actions_;
  struct RPG_Client_State         state_;

  RPG_Client_SelectionMode        selectionMode_;
  bool                            centerOnActivePlayer_;
  ACE_Thread_Mutex                screenLock_;
  bool                            debug_;
};

#endif
