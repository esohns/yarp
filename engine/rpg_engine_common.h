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

#ifndef RPG_ENGINE_COMMON_H
#define RPG_ENGINE_COMMON_H

#include <deque>
#include <set>
#include <string>
#include <list>
#include <map>

#include "ace/INET_Addr.h"
#include "ace/Log_Msg.h"
#include "ace/Time_Value.h"

//#include "net_configuration.h"

//#include "net_client_iconnector.h"

#include "rpg_common_condition.h"
//#include "rpg_common_incl.h"
#include "rpg_common_plane.h"
#include "rpg_common_terrain.h"
#include "rpg_common_climate.h"
#include "rpg_common_timeofday.h"
#include "rpg_common_ambientlighting.h"
#include "rpg_common_environment.h"

#include "rpg_player_base.h"

#include "rpg_monster_spawn.h"

#include "rpg_map_common.h"

#include "rpg_engine_command.h"
#include "rpg_engine_entitymode.h"

typedef std::set<enum RPG_Engine_EntityMode> RPG_Engine_EntityMode_t;
typedef RPG_Engine_EntityMode_t::const_iterator RPG_Engine_EntityModeConstIterator_t;

typedef unsigned int RPG_Engine_EntityID_t;

struct RPG_Engine_Action
{
  enum RPG_Engine_Command command;
  RPG_Map_Position_t      position;
  RPG_Map_Path_t          path;
  RPG_Engine_EntityID_t   target;
};
typedef std::deque<struct RPG_Engine_Action> RPG_Engine_Actions_t;
typedef RPG_Engine_Actions_t::iterator RPG_Engine_ActionsIterator_t;
typedef RPG_Engine_Actions_t::const_iterator RPG_Engine_ActionsConstIterator_t;

struct RPG_Engine_Entity
{
  RPG_Player_Base*        character;
  RPG_Engine_EntityMode_t modes;
  RPG_Map_Position_t      position;
  RPG_Engine_Actions_t    actions;
  // monster - onlies
  bool                    is_spawned;
};
typedef std::map<RPG_Engine_EntityID_t, struct RPG_Engine_Entity*> RPG_Engine_Entities_t;
typedef RPG_Engine_Entities_t::iterator RPG_Engine_EntitiesIterator_t;
typedef RPG_Engine_Entities_t::const_iterator RPG_Engine_EntitiesConstIterator_t;

typedef std::list<RPG_Engine_EntityID_t> RPG_Engine_EntityList_t;
typedef RPG_Engine_EntityList_t::iterator RPG_Engine_EntityListIterator_t;
typedef RPG_Engine_EntityList_t::const_iterator RPG_Engine_EntityListConstIterator_t;

struct RPG_Engine_Spawn
{
  struct RPG_Monster_Spawn spawn;
  long                     timer_id;
};
typedef std::vector<struct RPG_Engine_Spawn> RPG_Engine_Spawns_t;
typedef RPG_Engine_Spawns_t::iterator RPG_Engine_SpawnsIterator_t;
typedef RPG_Engine_Spawns_t::const_iterator RPG_Engine_SpawnsConstIterator_t;

struct RPG_Engine_LevelMetaData
{
  std::string                   name;

  struct RPG_Common_Environment environment;

  // (roaming) monsters
  RPG_Engine_Spawns_t           spawns;
  unsigned int                  max_num_spawned; // 0: don't auto-spawn, INT_MAX: unlimited
};
struct RPG_Engine_LevelData
{
  struct RPG_Engine_LevelMetaData metadata;
  struct RPG_Map                  map;
};

//typedef std::map<RPG_Map_Position_t, SDL_Surface*> RPG_Engine_EntityGraphics_t;
//typedef RPG_Engine_EntityGraphics_t::const_iterator RPG_Engine_EntityGraphicsConstIterator_t;

struct RPG_Engine_CombatantSequenceElement
{
  // needed for proper sorting...
  bool operator< (const struct RPG_Engine_CombatantSequenceElement& rhs_in) const
  {
    if (initiative < rhs_in.initiative)
      return true;

    if (initiative > rhs_in.initiative)
      return false;

    if (DEXModifier < rhs_in.DEXModifier)
      return true;

    if (DEXModifier > rhs_in.DEXModifier)
      return false;

    // *NOTE*: a conflict between monsters doesn't matter, really...
    // --> if either one is a PC, we have a conflict
    ACE_ASSERT(handle && rhs_in.handle);
    try {
      if (handle->isPlayerCharacter() ||
          rhs_in.handle->isPlayerCharacter())
      {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("sort-conflict !\n")));

        // resolve the conflict by comparing adresses...
        return (this < &rhs_in);
      } // end IF
    } catch (...) {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Engine_CombatantSequenceElement::operator<, aborting\n")));
    }

    // no conflict for monsters...
    return false;
  }
  bool operator> (const struct RPG_Engine_CombatantSequenceElement& rhs_in) const
  {
    return (!(*this < rhs_in) && !(*this == rhs_in));
  }
  bool operator== (const struct RPG_Engine_CombatantSequenceElement& rhs_in) const
  {
    if (initiative == rhs_in.initiative)
    {
      if (DEXModifier == rhs_in.DEXModifier)
      {
        // *NOTE*: a conflict between monsters doesn't matter, really...
        // --> if either one is a PC, we have a conflict
        ACE_ASSERT(handle && rhs_in.handle);
        try {
          if (handle->isPlayerCharacter() ||
              rhs_in.handle->isPlayerCharacter())
          {
//             ACE_DEBUG((LM_DEBUG,
//                        ACE_TEXT("sort-conflict !\n")));

            // resolve the conflict by comparing adresses...
            return (this < &rhs_in);
          } // end IF
        } catch (...) {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in RPG_Engine_CombatantSequenceElement::operator==, aborting\n")));
        }
      } // end IF
    } // end IF

    // no conflict for monsters...
    return false;
  }
  bool operator!= (const struct RPG_Engine_CombatantSequenceElement& rhs_in) const
  {
    return !operator==(rhs_in);
  }

  ACE_INT8               initiative;
  ACE_INT16              DEXModifier;
  const RPG_Player_Base* handle;
};

// struct lessThanRPG_Engine_CombatantSequenceElement_t
// {
//   bool operator()(const RPG_Engine_CombatantSequenceElement_t& lhs_in,
//                   const RPG_Engine_CombatantSequenceElement_t& rhs_in) const
//   {
//     if (lhs_in.initiative < rhs_in.initiative)
//       return true;
//
//     return (lhs_in.initiative > rhs_in.initiative ? false
//                                                   : (lhs_in.DEXModifier < rhs_in.DEXModifier));
//   }
// };

// typedef std::set<RPG_Engine_CombatantSequenceElement_t,
//                  lessThanRPG_Engine_CombatantSequenceElement_t> RPG_Engine_CombatantSequence_t;
// *NOTE*: sort in DESCENDING order !
typedef std::set<struct RPG_Engine_CombatantSequenceElement,
                 std::greater<struct RPG_Engine_CombatantSequenceElement> > RPG_Engine_CombatantSequence_t;
typedef RPG_Engine_CombatantSequence_t::iterator RPG_Engine_CombatantSequenceIterator_t;
typedef RPG_Engine_CombatantSequence_t::const_iterator RPG_Engine_CombatantSequenceConstIterator_t;
// typedef RPG_Engine_CombatantSequence_t::const_reverse_iterator RPG_Engine_CombatantSequenceRIterator_t;

struct RPG_Engine_ClientNotificationParameters
{
  RPG_Engine_EntityID_t     entity_id;
  enum RPG_Common_Condition condition;
  RPG_Map_Positions_t       positions;
  RPG_Map_Position_t        previous_position;
  enum RPG_Common_SubClass  subclass;
  ACE_UINT8                 visible_radius;
  std::string               message;
};
//typedef std::vector<void*> RPG_Engine_ClientParameters_t;
//typedef RPG_Engine_ClientParameters_t::iterator RPG_Engine_ClientParametersIterator_t;
//typedef RPG_Engine_ClientParameters_t::const_iterator RPG_Engine_ClientParametersConstIterator_t;

typedef std::vector<ACE_UINT64> RPG_Engine_LevelToExperienceList_t;
typedef RPG_Engine_LevelToExperienceList_t::const_iterator RPG_Engine_LevelToExperienceListConstIterator_t;
typedef std::map<ACE_UINT8, RPG_Engine_LevelToExperienceList_t> RPG_Engine_CRToExperienceMap_t;
typedef RPG_Engine_CRToExperienceMap_t::const_iterator RPG_Engine_CRToExperienceMapConstIterator_t;

typedef std::map<RPG_Engine_EntityID_t, RPG_Map_Positions_t> RPG_Engine_SeenPositions_t;
typedef RPG_Engine_SeenPositions_t::const_iterator RPG_Engine_SeenPositionsConstIterator_t;
typedef RPG_Engine_SeenPositions_t::iterator RPG_Engine_SeenPositionsIterator_t;

#endif
