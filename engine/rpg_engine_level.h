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

#include "rpg_engine_common.h"

#include <rpg_map_common.h>

#include <ace/Global_Macros.h>
#include <ace/Atomic_Op_T.h>
#include <ace/Condition_T.h>
#include <ace/Synch.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Engine_Level
{
 public:
  RPG_Engine_Level();
  RPG_Engine_Level(const RPG_Map_FloorPlan_t&);
  virtual ~RPG_Engine_Level();

  void init(const RPG_Map_FloorPlan_t&); // map level
  // *WARNING*: fire&forget API, added entities are controlled by the engine !
  const RPG_Engine_EntityID_t add(RPG_Engine_Entity&); // entity
  void remove(const RPG_Engine_EntityID_t&); // id
  void action(const RPG_Engine_EntityID_t&, // id
              const RPG_Engine_Action&);    // action

  const RPG_Map_Dimensions_t getDimensions() const;
  const RPG_Map_Element getElement(const RPG_Map_Position_t&) const;
  const RPG_Engine_EntityGraphics_t getGraphics() const;
  const RPG_Map_Position_t getPosition(const RPG_Engine_EntityID_t&) const;

  const RPG_Map_Door_t getDoor(const RPG_Map_Position_t&) const;
  void handleDoor(const RPG_Map_Position_t&,
                  const bool&);              // open ? : close
  const RPG_Map_Positions_t getWalls() const;

  // perform (one round of) actions
  void handleEntities();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Level(const RPG_Engine_Level&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Level& operator=(const RPG_Engine_Level&));

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex, RPG_Engine_EntityID_t> myCurrentID;

  // make our API re-entrant
  mutable ACE_Recursive_Thread_Mutex        myLock;
  // implement blocking wait...
  ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;

  RPG_Engine_Entities_t                     myEntities;
  RPG_Map_FloorPlan_t                       myFloorPlan;
};

#endif
