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

#ifndef RPG_ENGINE_H
#define RPG_ENGINE_H

#include "rpg_engine_exports.h"
#include "rpg_engine_common.h"
#include "rpg_engine_iclient.h"
#include "rpg_engine_messagequeue.h"
#include "rpg_engine_entitymode.h"
#include "rpg_engine_level.h"
#include "rpg_engine_event_manager.h"

#include <rpg_map_common.h>
#include <rpg_map_common_tools.h>

#include <rpg_common_icontrol.h>
#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Task.h>
#include <ace/Atomic_Op_T.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Engine_Export RPG_Engine
 : public ACE_Task<ACE_MT_SYNCH>,
   public RPG_Common_IControl,
   public RPG_Common_IDumpState,
   public RPG_Engine_Level
{
  // AI thread(s) require access to the entity action queues...
  friend class RPG_Engine_Event_Manager;

 public:
  RPG_Engine();
  virtual ~RPG_Engine();

  // implement RPG_Common_IControl
  virtual void start();
  virtual void stop();
  virtual bool isRunning() const;

  static void wait_all();

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

  // *WARNING*: DO NOT USE while the engine isRunning() !
  void init(RPG_Engine_IClient*,        // client interface handle
            const RPG_Engine_Level_t&); // level
  // *WARNING*: handle with care !
  void lock();
  void unlock();

  // *WARNING*: fire&forget API, added NPC (!) entities are controlled by the engine
  RPG_Engine_EntityID_t add(RPG_Engine_Entity*); // entity
  void remove(const RPG_Engine_EntityID_t&); // id
  bool exists(const RPG_Engine_EntityID_t&) const; // id
  void action(const RPG_Engine_EntityID_t&, // id
              const RPG_Engine_Action&,     // action
              const bool& = true);          // locked access ?

  void setActive(const RPG_Engine_EntityID_t&); // id
  RPG_Engine_EntityID_t getActive(const bool& = true) const; // locked access ?
  void mode(const RPG_Engine_EntityMode&); // add mode (to active entity)
  void clear(const RPG_Engine_EntityMode&); // clear mode (from active entity)
  bool hasMode(const RPG_Engine_EntityMode&) const; // mode

  RPG_Map_Position_t getPosition(const RPG_Engine_EntityID_t&,
                                 const bool& = true) const; // locked access ?
  RPG_Map_Position_t findValid(const RPG_Map_Position_t&,  // center
                               const unsigned int&) const; // max (square !) radius
  RPG_Engine_EntityID_t hasEntity(const RPG_Map_Position_t&,
                                  const bool& = true) const; // locked access ?
  RPG_Engine_EntityList_t entities(const RPG_Map_Position_t&) const; // sort: position (closest first)
  bool isMonster(const RPG_Engine_EntityID_t&,
                 const bool& = true) const; // locked access ?
  std::string getName(const RPG_Engine_EntityID_t&,
                      const bool& = true) const; // locked access ?
  unsigned int numSpawned() const;

  // vision
  unsigned char getVisibleRadius(const RPG_Engine_EntityID_t&,
                                 const bool& = true) const;    // locked access ?
  void getVisiblePositions(const RPG_Engine_EntityID_t&, // id
                           RPG_Map_Positions_t&,         // return value: (currently) visible positions
                           const bool& = true) const;    // locked access ?
  bool canSee(const RPG_Engine_EntityID_t&, // id
              const RPG_Map_Position_t&,    // position
              const bool& = true) const;    // locked access ?
  bool canSee(const RPG_Engine_EntityID_t&, // id
              const RPG_Engine_EntityID_t&, // target id
              const bool& = true) const;    // locked access ?

  bool findPath(const RPG_Map_Position_t&, // start position
                const RPG_Map_Position_t&, // end position
                RPG_Map_Path_t&,           // return value: (partial) path A --> B
                const bool& = true) const; // locked access ?

  bool canReach(const RPG_Engine_EntityID_t&, // id
                const RPG_Map_Position_t&,    // target position
                const bool& = true) const;    // locked access ?

  // map
  RPG_Engine_LevelMeta_t getMeta(const bool& = true) const; // locked access ?
  RPG_Map_Position_t getStartPosition(const bool& = true) const; // locked access ?
  RPG_Map_Size_t getSize(const bool& = true) const; // locked access ?
  RPG_Map_DoorState state(const RPG_Map_Position_t&,
                          const bool& = true) const; // locked access ?

  bool isValid(const RPG_Map_Position_t&,
               const bool& = true) const; // locked access ?
  bool isCorner(const RPG_Map_Position_t&,
                const bool& = true) const; // locked access ?
  RPG_Map_Element getElement(const RPG_Map_Position_t&,
                             const bool& = true) const; // locked access ?
  RPG_Map_Positions_t getObstacles(const bool&,               // include entities ?
                                   const bool& = true) const; // locked access ?
  RPG_Map_Positions_t getWalls(const bool& = true) const; // locked access ?
  RPG_Map_Positions_t getDoors(const bool& = true) const; // locked access ?

 private:
  typedef ACE_Task<ACE_MT_SYNCH> inherited;
  typedef RPG_Engine_Level inherited2;

  // hide unwanted funcionality
  using RPG_Engine_Level::init;
  using RPG_Engine_Level::getMeta;
  using RPG_Engine_Level::findPath;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine(const RPG_Engine&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine& operator=(const RPG_Engine&));

  // override task-based members
  virtual int open(void* = NULL);
  virtual int close(u_long = 0);
  virtual int svc(void);

  // helper methods
  // *NOTE*: requires myLock to be held !
  bool isBlocked(const RPG_Map_Position_t&) const;

  // helper classes
  struct distance_sort_t
  {
    const RPG_Engine* const engine;
    bool                    locked_access;
    RPG_Map_Position_t      reference_position;

    bool operator()(const RPG_Engine_EntityID_t&, const RPG_Engine_EntityID_t&);
  };

  void clearEntityActions(const RPG_Engine_EntityID_t& = 0); // entity ID (default: ALL)

  // perform (one round of) actions
  void handleEntities();

  // helper types
  typedef std::vector<std::pair<RPG_Engine_Command, RPG_Engine_ClientParameters_t*> > RPG_Engine_ClientNotifications_t;
  typedef RPG_Engine_ClientNotifications_t::const_iterator RPG_Engine_ClientNotificationsConstIterator_t;

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex,
                       RPG_Engine_EntityID_t> myCurrentID;

  // *IMPORTANT NOTE*: need this ONLY to handle control messages...
  RPG_Engine_MessageQueue                     myQueue;

  // make our API re-entrant
  mutable ACE_Thread_Mutex                    myLock;
  //// implement blocking wait...
  //ACE_Condition<ACE_Recursive_Thread_Mutex>   myCondition;

  RPG_Engine_Entities_t                       myEntities;
  RPG_Engine_EntityID_t                       myActivePlayer;

  RPG_Engine_IClient*                         myClient;
};
#endif
