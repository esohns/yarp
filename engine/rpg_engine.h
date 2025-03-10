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

#include "ace/Atomic_Op.h"
#include "ace/Global_Macros.h"
#include "ace/Message_Queue_T.h"
#include "ace/Synch_Traits.h"
#include "ace/Task.h"

#include "common_parser_common.h"

#include "common_time_common.h"

#include "stream_allocatorheap.h"

#include "net_common.h"

#include "rpg_map_common.h"
#include "rpg_map_common_tools.h"

#include "rpg_engine_common.h"
#include "rpg_engine_entitymode.h"
#include "rpg_engine_event_manager.h"
#include "rpg_engine_level.h"
#include "rpg_engine_messagequeue.h"

// forward declarations
class RPG_Engine_IClient;

class RPG_Engine
 : public ACE_Task<ACE_MT_SYNCH,
                   Common_TimePolicy_t>
 , public RPG_Engine_Level
 //, public Common_IControl
{
  typedef ACE_Task<ACE_MT_SYNCH,
                   Common_TimePolicy_t> inherited;
  typedef RPG_Engine_Level inherited2;

  // AI thread(s) require access to the entity action queues...
  friend class RPG_Engine_Event_Manager;

 public:
  RPG_Engine ();
  virtual ~RPG_Engine ();

  // implement Common_IControl
  virtual void start ();
  virtual void stop (bool = true); // locked access ?
  inline virtual bool isRunning () const { return (inherited::thr_count_ > 0); }

  static void wait_all ();

  // implement Common_IDumpState
  virtual void dump_state () const;

  // *WARNING*: handle with care !
  inline void lock () const { int result = lock_.acquire (); ACE_ASSERT (result == 0); }
  inline void unlock () const { int result = lock_.release (); ACE_ASSERT (result == 0); }

  void initialize (RPG_Engine_IClient*,                           // client interface handle
                   enum Net_ClientServerRole = NET_ROLE_INVALID); // role
  inline enum Net_ClientServerRole role () { return role_; }

  // *WARNING*: DO NOT USE while the engine isRunning() !
  void set (const struct RPG_Engine_LevelData&); // level

  // *WARNING*: fire&forget API, added NPC (!) entities are controlled by the engine
  RPG_Engine_EntityID_t add (struct RPG_Engine_Entity*, // entity handle
                             bool = true);         // locked access ?
  void remove (RPG_Engine_EntityID_t, // id
               bool = true);          // locked access ?
  bool exists (RPG_Engine_EntityID_t) const; // id
  void action (RPG_Engine_EntityID_t,           // id
               const struct RPG_Engine_Action&, // action
               bool = true);                    // locked access ?
  inline void inject (RPG_Engine_EntityID_t id_in,
                      const struct RPG_Engine_Action& action_in,
                      bool lockedAccess_in) { action_impl (id_in, action_in, lockedAccess_in); }

  // state
  bool load (const std::string&,  // FQ filename
             const std::string&); // schema repository
  bool save (const std::string&); // descriptor

  void setActive (RPG_Engine_EntityID_t, // id
                  bool = true);          // locked access ?
  RPG_Engine_EntityID_t getActive (bool = true) const; // locked access ?
  void mode (enum RPG_Engine_EntityMode); // add mode (to active entity)
  void clear (enum RPG_Engine_EntityMode); // clear mode (from active entity)
  bool hasMode (enum RPG_Engine_EntityMode) const; // mode

  RPG_Map_Position_t getPosition (RPG_Engine_EntityID_t, // id
                                  bool = true) const;    // locked access ?
  RPG_Map_Position_t findValid (const RPG_Map_Position_t&, // center
                                unsigned int = 0,          // max (square !) radius [0: whereever]
                                bool = true) const;        // locked access ?
  RPG_Engine_EntityID_t hasEntity (const RPG_Map_Position_t&, // position
                                   bool = true) const;        // locked access ?
  // *NOTE*: returns sorted list (closest first)
  RPG_Engine_EntityList_t entities (const RPG_Map_Position_t&, // position
                                    bool = true) const;        // locked access ?
  bool isMonster (RPG_Engine_EntityID_t, // id
                  bool = true) const;    // locked access ?
  std::string getName (RPG_Engine_EntityID_t, // id
                       bool = true) const;    // locked access ?
  struct RPG_Character_Class getClass (RPG_Engine_EntityID_t, // id
                                       bool = true) const;    // locked access ?
  // *NOTE*: maps to RPG_Engine_Common_Tools::isCharacterDisabled(), i.e. HP > 0
  bool isAble (RPG_Engine_EntityID_t,                         // id
               bool = true) const;                            // locked access ?
  unsigned int numSpawned (const std::string&, // type (empty: total)
                           bool = true) const; // locked access ?

  // vision
  ACE_UINT8 getVisibleRadius (RPG_Engine_EntityID_t, // id
                              bool = true) const;    // locked access ?
  void getVisiblePositions (RPG_Engine_EntityID_t, // id
                            RPG_Map_Positions_t&,  // return value: (currently) visible positions
                            bool = true) const;    // locked access ?
  bool canSee (RPG_Engine_EntityID_t,     // id
               const RPG_Map_Position_t&, // position
               bool = true) const;        // locked access ?
  bool canSee (RPG_Engine_EntityID_t, // id
               RPG_Engine_EntityID_t, // target id
               bool = true) const;    // locked access ?
  const RPG_Map_Positions_t& getSeen (RPG_Engine_EntityID_t);
  bool hasSeen (RPG_Engine_EntityID_t,     // id
                const RPG_Map_Position_t&, // position
                bool = true) const;        // locked access ?

  bool findPath (const RPG_Map_Position_t&, // start position
                 const RPG_Map_Position_t&, // end position
                 RPG_Map_Path_t&,           // return value: (partial) path A --> B
                 bool = true) const;        // locked access ?

  bool canReach (RPG_Engine_EntityID_t,     // id
                 const RPG_Map_Position_t&, // target position
                 bool = true) const;        // locked access ?

  // map
  struct RPG_Engine_LevelMetaData getMetaData (bool = true) const; // locked access ?
  RPG_Map_Position_t getStartPosition (bool = true) const; // locked access ?
  RPG_Map_Positions_t getSeedPoints (bool = true) const; // locked access ?
  RPG_Map_Size_t getSize (bool = true) const; // locked access ?
  enum RPG_Map_DoorState state (const RPG_Map_Position_t&, // position
                                bool = true) const;        // locked access ?

  // either floor or an open/broken (!) door ?
  bool isValid (const RPG_Map_Position_t&, // position
                bool = true) const;        // locked access ?
  bool isCorner (const RPG_Map_Position_t&, // position
                 bool = true) const;        // locked access ?
  RPG_Map_Element getElement (const RPG_Map_Position_t&, // position
                              bool = true) const;        // locked access ?
  RPG_Map_Positions_t getObstacles (bool,               // include entities ?
                                    bool = true) const; // locked access ?
  RPG_Map_Positions_t getWalls (bool = true) const; // locked access ?
  RPG_Map_Positions_t getDoors (bool = true) const; // locked access ?

 private:
  // hide unwanted funcionality
  using RPG_Engine_Level::init;
  using RPG_Engine_Level::getMetaData;
  using RPG_Engine_Level::getSeedPoints;
  using RPG_Engine_Level::findPath;

  ACE_UNIMPLEMENTED_FUNC (RPG_Engine (const RPG_Engine&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Engine& operator=(const RPG_Engine&))

  // override task-based members
  virtual int open (void* = NULL);
  virtual int close (u_long = 0);
  virtual int svc (void);

  // helper methods
  bool isBlocked (const RPG_Map_Position_t&, // position
                  bool = true) const;        // locked access ?

  // helper classes
  struct distance_sort
  {
    const RPG_Engine* const engine;
    bool                    locked_access;
    RPG_Map_Position_t      reference_position;

    bool operator() (const RPG_Engine_EntityID_t&,
                     const RPG_Engine_EntityID_t&);
  };

  void action_impl (RPG_Engine_EntityID_t,           // id
                    const struct RPG_Engine_Action&, // action
                    bool = true);                    // locked access ?
  void clearEntityActions (RPG_Engine_EntityID_t = 0, // id (default: ALL)
                           bool = true);              // locked access ?

  // perform (one round of) actions
  void handleEntities ();

  // helper types
  typedef std::vector<std::pair<enum RPG_Engine_Command,
                                struct RPG_Engine_ClientNotificationParameters> > RPG_Engine_ClientNotifications_t;
  typedef RPG_Engine_ClientNotifications_t::const_iterator RPG_Engine_ClientNotificationsConstIterator_t;
  typedef ACE_Message_Queue<ACE_MT_SYNCH,
                            Common_TimePolicy_t> MESSAGE_QUEUE_T;
  // typedef Stream_AllocatorHeap_T<ACE_MT_SYNCH,
  //                                struct Common_Parser_FlexAllocatorConfiguration> HEAP_ALLOCATOR_T;

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex,
                       RPG_Engine_EntityID_t>     currentID;

  RPG_Engine_EntityID_t                           activePlayer_;
  RPG_Engine_IClient*                             client_;
  RPG_Engine_Entities_t                           entities_;
  // make API re-entrant
  mutable ACE_SYNCH_MUTEX                         lock_;
  // *IMPORTANT NOTE*: need this ONLY to handle control messages...
  RPG_Engine_MessageQueue                         queue_;
  enum Net_ClientServerRole                       role_;
  RPG_Engine_SeenPositions_t                      seenPositions_;
};

#endif
