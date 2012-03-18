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

#ifndef RPG_ENGINE_LEVEL_H
#define RPG_ENGINE_LEVEL_H

#include "rpg_engine_exports.h"
#include "rpg_engine_common.h"
#include "rpg_engine_iwindow.h"
#include "rpg_engine_messagequeue.h"

#include <rpg_map_common.h>
#include <rpg_map_level.h>

#include <rpg_common_icontrol.h>
#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Task.h>
#include <ace/Atomic_Op_T.h>
#include <ace/Condition_T.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Engine_Export RPG_Engine_Level
 : public RPG_Map_Level,
   public ACE_Task<ACE_MT_SYNCH>,
   public RPG_Common_IControl,
   public RPG_Common_IDumpState
{
 public:
  RPG_Engine_Level();
  RPG_Engine_Level(RPG_Engine_IWindow*, // UI handle
                   const RPG_Map_t&);   // map
  virtual ~RPG_Engine_Level();

  // implement RPG_Common_IControl
  virtual void start();
  virtual void stop();
  virtual const bool isRunning();

  static void wait_all();

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

  // *WARNING*: DO NOT USE this while the engine is running !
  void init(RPG_Engine_IWindow*, // UI handle
            const RPG_Map_t&);   // map
  void save(const std::string&) const; // FQ filename

  // *WARNING*: fire&forget API, added entities are controlled by the engine !
  const RPG_Engine_EntityID_t add(RPG_Engine_Entity*); // entity
  void remove(const RPG_Engine_EntityID_t&); // id
  void action(const RPG_Engine_EntityID_t&, // id
              const RPG_Engine_Action&);    // action

  void setActive(const RPG_Engine_EntityID_t&); // id
  const RPG_Engine_EntityID_t getActive() const; // return value: id (if any)
  void mode(const RPG_Engine_PlayerMode&); // add mode (to active entity)
  void clear(const RPG_Engine_PlayerMode&); // clear mode (from active entity)
  const bool hasMode(const RPG_Engine_PlayerMode&) const; // mode

  const RPG_Engine_EntityID_t hasEntity(const RPG_Map_Position_t&) const;
  const bool isMonster(const RPG_Engine_EntityID_t&) const;

  const RPG_Map_Element getElement(const RPG_Map_Position_t&) const;
  const SDL_Surface* getGraphics(const RPG_Engine_EntityID_t&) const;
  const RPG_Engine_EntityGraphics_t getGraphics() const;
  const RPG_Map_Position_t getPosition(const RPG_Engine_EntityID_t&) const;
  const RPG_Map_Door_t getDoor(const RPG_Map_Position_t&) const;

  using RPG_Map_Level::getStartPosition;
  using RPG_Map_Level::getSeedPoints;
  using RPG_Map_Level::getFloorPlan;
  using RPG_Map_Level::getDimensions;

 private:
  typedef RPG_Map_Level inherited;
  typedef ACE_Task<ACE_MT_SYNCH> inherited2;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Level(const RPG_Engine_Level&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Level& operator=(const RPG_Engine_Level&));

  // override task-based members
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int svc(void);

  // helper methods
  void handleDoor(const RPG_Map_Position_t&, // position
                  const bool&,               // open ? : close
                  bool&);                    // return value: toggled ?
  void clearEntityActions(const RPG_Engine_EntityID_t& = 0); // entity ID (default: ALL)

  // perform (one round of) actions
  void handleEntities();

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex, RPG_Engine_EntityID_t> myCurrentID;

  // *IMPORTANT NOTE*: need this ONLY to handle control messages...
  RPG_Engine_MessageQueue         myQueue;

  // make our API re-entrant
  mutable ACE_Thread_Mutex        myLock;
  // implement blocking wait...
  ACE_Condition<ACE_Thread_Mutex> myCondition;

  RPG_Engine_Entities_t           myEntities;
  RPG_Engine_EntityID_t           myActivePlayer;

  RPG_Engine_IWindow*             myClient;
};

#endif
