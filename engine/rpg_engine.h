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

#include <string>

#include "ace/Atomic_Op_T.h"
#include "ace/Global_Macros.h"
#include "ace/Message_Queue_T.h"
#include "ace/Synch.h"
#include "ace/Synch_Traits.h"
#include "ace/Task.h"

#include "common.h"
#include "common_icontrol.h"

#include "net_configuration.h"

#include "net_client_common.h"

#include "rpg_map_common.h"
#include "rpg_map_common_tools.h"

#include "rpg_engine_common.h"
#include "rpg_engine_entitymode.h"
#include "rpg_engine_exports.h"
#include "rpg_engine_event_manager.h"
#include "rpg_engine_level.h"
#include "rpg_engine_messagequeue.h"

// forward declarations
class RPG_Engine_IClient;

/**
        @author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Engine_Export RPG_Engine
 : public ACE_Task<ACE_MT_SYNCH, Common_TimePolicy_t>
 , public RPG_Engine_Level
 , public Common_IControl
{
  // AI thread(s) require access to the entity action queues...
  friend class RPG_Engine_Event_Manager;

 public:
  RPG_Engine ();
  virtual ~RPG_Engine ();

  // implement Common_IControl
  virtual void start ();
  virtual void stop (bool = true); // locked access ?
  virtual bool isRunning () const;

  static void wait_all ();

  // implement Common_IDumpState
  virtual void dump_state () const;

  // *WARNING*: handle with care !
  void lock () const;
  void unlock () const;

  void initialize (RPG_Engine_IClient*); // client interface handle
  // *WARNING*: DO NOT USE while the engine isRunning() !
  void set (const RPG_Engine_Level_t&); // level

  // *WARNING*: fire&forget API, added NPC (!) entities are controlled by the engine
  RPG_Engine_EntityID_t add (RPG_Engine_Entity_t*, // entity handle
                             bool = true);         // locked access ?
  void remove (const RPG_Engine_EntityID_t&); // id
  bool exists (const RPG_Engine_EntityID_t&) const; // id
  void action (const RPG_Engine_EntityID_t&, // id
               const RPG_Engine_Action_t&,   // action
               bool = true);                 // locked access ?

  // state
  bool load (const std::string&,  // FQ filename
             const std::string&); // schema repository
  bool save (const std::string&); // descriptor

  void setActive (const RPG_Engine_EntityID_t&, // id
                  bool = true);                 // locked access ?
  RPG_Engine_EntityID_t getActive (bool = true) const; // locked access ?
  void mode (const RPG_Engine_EntityMode&); // add mode (to active entity)
  void clear (const RPG_Engine_EntityMode&); // clear mode (from active entity)
  bool hasMode (const RPG_Engine_EntityMode&) const; // mode

  RPG_Map_Position_t getPosition (const RPG_Engine_EntityID_t&, // id
                                  bool = true) const;           // locked access ?
  RPG_Map_Position_t findValid (const RPG_Map_Position_t&, // center
                                unsigned int = 0,          // max (square !) radius [0: whereever]
                                bool = true) const;        // locked access ?
  RPG_Engine_EntityID_t hasEntity (const RPG_Map_Position_t&, // position
                                   bool = true) const;        // locked access ?
  RPG_Engine_EntityList_t entities (const RPG_Map_Position_t&, // sort: position (closest first)
                                    bool = true) const;        // locked access ?
  bool isMonster (const RPG_Engine_EntityID_t&, // id
                  bool = true) const;           // locked access ?
  std::string getName (const RPG_Engine_EntityID_t&, // id
                       bool = true) const;           // locked access ?
  RPG_Character_Class getClass (const RPG_Engine_EntityID_t&, // id
                                bool = true) const;           // locked access ?
  unsigned int numSpawned (const std::string&, // type (empty: total)
                           bool = true) const; // locked access ?

  // vision
  unsigned char getVisibleRadius (const RPG_Engine_EntityID_t&, // id
                                  bool = true) const;           // locked access ?
  void getVisiblePositions (const RPG_Engine_EntityID_t&, // id
                            RPG_Map_Positions_t&,         // return value: (currently) visible positions
                            bool = true) const;           // locked access ?
  bool canSee (const RPG_Engine_EntityID_t&, // id
               const RPG_Map_Position_t&,    // position
               bool = true) const;           // locked access ?
  bool canSee (const RPG_Engine_EntityID_t&, // id
               const RPG_Engine_EntityID_t&, // target id
               bool = true) const;           // locked access ?

  bool findPath (const RPG_Map_Position_t&, // start position
                 const RPG_Map_Position_t&, // end position
                 RPG_Map_Path_t&,           // return value: (partial) path A --> B
                 bool = true) const;        // locked access ?

  bool canReach (const RPG_Engine_EntityID_t&, // id
                 const RPG_Map_Position_t&,    // target position
                 bool = true) const;           // locked access ?

  // map
  RPG_Engine_LevelMetaData_t getMetaData (bool = true) const; // locked access ?
  RPG_Map_Position_t getStartPosition (bool = true) const; // locked access ?
  RPG_Map_Positions_t getSeedPoints (bool = true) const; // locked access ?
  RPG_Map_Size_t getSize (bool = true) const; // locked access ?
  RPG_Map_DoorState state (const RPG_Map_Position_t&, // position
                           bool = true) const;        // locked access ?

  bool isValid (const RPG_Map_Position_t&, // position
                bool = true) const;        // locked access ?
  bool isCorner (const RPG_Map_Position_t&, // position
                 bool = true) const;        // locked access ?
  RPG_Map_Element getElement (const RPG_Map_Position_t&, // position
                              bool = true) const;        // locked access ?
  RPG_Map_Positions_t getObstacles (const bool&,        // include entities ?
                                    bool = true) const; // locked access ?
  RPG_Map_Positions_t getWalls (bool = true) const; // locked access ?
  RPG_Map_Positions_t getDoors (bool = true) const; // locked access ?

 private:
  typedef ACE_Task<ACE_MT_SYNCH, Common_TimePolicy_t> inherited;
  typedef RPG_Engine_Level inherited2;

  // hide unwanted funcionality
  using RPG_Engine_Level::init;
  using RPG_Engine_Level::getMetaData;
  using RPG_Engine_Level::getSeedPoints;
  using RPG_Engine_Level::findPath;

  ACE_UNIMPLEMENTED_FUNC (RPG_Engine (const RPG_Engine&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Engine& operator=(const RPG_Engine&));

  // override task-based members
  virtual int open (void* = NULL);
  virtual int close (u_long = 0);
  virtual int svc (void);

  // helper methods
  // *NOTE*: requires myLock to be held !
  bool isBlocked (const RPG_Map_Position_t&) const;

  // helper classes
  struct distance_sort_t
  {
    const RPG_Engine* const engine;
    bool                    locked_access;
    RPG_Map_Position_t      reference_position;

    bool operator()(const RPG_Engine_EntityID_t&,
                    const RPG_Engine_EntityID_t&);
  };

  void clearEntityActions (const RPG_Engine_EntityID_t& = 0, // entity ID (default: ALL)
                           bool = true);                     // locked access ?

  // perform (one round of) actions
  void handleEntities ();

  // helper types
  typedef std::vector<std::pair<RPG_Engine_Command,
                                RPG_Engine_ClientNotificationParameters_t> > RPG_Engine_ClientNotifications_t;
  typedef RPG_Engine_ClientNotifications_t::const_iterator RPG_Engine_ClientNotificationsConstIterator_t;
  typedef ACE_Message_Queue<ACE_MT_SYNCH,
                            Common_TimePolicy_t> MESSAGE_QUEUE_T;

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex,
                       RPG_Engine_EntityID_t> myCurrentID;

  // *IMPORTANT NOTE*: need this ONLY to handle control messages...
  RPG_Engine_MessageQueue                     myQueue;

  // make API re-entrant
  mutable ACE_Thread_Mutex                    myLock;
  //// implement blocking wait...
  //ACE_Condition<ACE_Recursive_Thread_Mutex>   myCondition;

  RPG_Engine_Entities_t                       myEntities;
  RPG_Engine_EntityID_t                       myActivePlayer;

  RPG_Engine_IClient*                         myClient;
  Net_Configuration_t                         myNetConfiguration;
  Net_Client_IConnector_t*                    myConnector;
};
#endif
