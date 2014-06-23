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

#ifndef RPG_Net_Module_EventHandler_H
#define RPG_Net_Module_EventHandler_H

#include "rpg_net_exports.h"
#include "rpg_net_common.h"

#include "rpg_stream_task_base_synch.h"
#include "rpg_stream_streammodule_base.h"

#include "rpg_common.h"
#include "rpg_common_isubscribe.h"
#include "rpg_common_iclone.h"

// forward declaration(s)
class RPG_Net_SessionMessage;
class RPG_Net_Message;
class ACE_Recursive_Thread_Mutex;

class RPG_Net_Export RPG_Net_Module_EventHandler
 : public RPG_Stream_TaskBaseSynch<RPG_Common_TimePolicy_t,
                                   RPG_Net_SessionMessage,
                                   RPG_Net_Message>,
   public RPG_Common_ISubscribe<RPG_Net_INotify_t>,
   public RPG_Common_IClone<MODULE_TYPE>
{
 public:
  RPG_Net_Module_EventHandler();
  virtual ~RPG_Net_Module_EventHandler();

  void init(RPG_Net_NotifySubscribers_t*, // subscribers (handle)
            ACE_Recursive_Thread_Mutex*); // subscribers lock

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Message*&, // data message handle
                                 bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage(RPG_Net_SessionMessage*&, // session message handle
                                    bool&);                   // return value: pass message downstream ?

  // implement RPG_Common_ISubscribe
  virtual void subscribe(RPG_Net_INotify_t*); // new subscriber
  virtual void unsubscribe(RPG_Net_INotify_t*); // existing subscriber

  // implement RPG_Common_IClone
  virtual MODULE_TYPE* clone();

 private:
  typedef RPG_Stream_TaskBaseSynch<RPG_Common_TimePolicy_t,
                                   RPG_Net_SessionMessage,
                                   RPG_Net_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_EventHandler(const RPG_Net_Module_EventHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_EventHandler& operator=(const RPG_Net_Module_EventHandler&));

  // *NOTE*: make this recursive so that users may unsubscribe from within the
  // notification callbacks...
  // *WARNING*: implies CAREFUL iteration
  ACE_Recursive_Thread_Mutex*  myLock;
  RPG_Net_NotifySubscribers_t* mySubscribers;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY(ACE_MT_SYNCH,                 // task synch type
                             RPG_Common_TimePolicy_t,      // time policy
                             RPG_Net_Module_EventHandler); // writer type

#endif
