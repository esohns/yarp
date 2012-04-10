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
#include "stdafx.h"

#include "rpg_engine_level.h"

#include <rpg_map_pathfinding_tools.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Engine_Level::RPG_Engine_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::RPG_Engine_Level"));

}

RPG_Engine_Level::~RPG_Engine_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::~RPG_Engine_Level"));

}

void
RPG_Engine_Level::init(const RPG_Engine_Level_t& level_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::init"));

  myLevelMeta = level_in.level_meta;
  inherited::init(level_in.map);
}

void
RPG_Engine_Level::save(const std::string& filename_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::save"));

}

void
RPG_Engine_Level::handleDoor(const RPG_Map_Position_t& position_in,
                             const bool& open_in,
                             bool& toggled_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::handleDoor"));

  // init return value(s)
  toggled_out = false;

  RPG_Map_Door_t position_door;
  position_door.position = position_in;
  position_door.outside = DIRECTION_INVALID;
  position_door.is_open = false;
  position_door.is_locked = false;
  position_door.is_broken = false;

  RPG_Map_DoorsIterator_t iterator = myMap.plan.doors.find(position_door);
  // sanity check
  ACE_ASSERT(iterator != myMap.plan.doors.end());

  if (open_in)
  {
    // sanity check
    if ((*iterator).is_open)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already open...\n"),
//                  position_in.first,
//                  position_in.second));

      return;
    } // end IF

    // cannot simply open locked doors...
    if ((*iterator).is_locked)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("door [%u,%u] is locked...\n"),
                 position_in.first,
                 position_in.second));

      return;
    } // end IF
  } // end IF
  else
  {
    // sanity check
    if (!(*iterator).is_open)
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("door [%u,%u] already closed...\n"),
//                  position_in.first,
//                  position_in.second));

      return;
    } // end IF
  } // end ELSE

  // *WARNING*: set iterators are CONST for a good reason !
  // --> (but we know what we're doing)...
  (const_cast<RPG_Map_Door_t&>(*iterator)).is_open = open_in;

  toggled_out = true;
}

bool
RPG_Engine_Level::findPath(const RPG_Map_Position_t& start_in,
                           const RPG_Map_Position_t& end_in,
                           const RPG_Map_Positions_t& obstacles_in,
                           RPG_Map_Path_t& path_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Level::findPath"));

  // init result
  path_out.clear();

  // sanity check
  if (start_in == end_in)
    return true;

  RPG_Map_Pathfinding_Tools::findPath(getSize(),
                                      obstacles_in,
                                      start_in,
                                      DIRECTION_INVALID,
                                      end_in,
                                      path_out);

  return ((path_out.size() >= 2) &&
          (path_out.front().first == start_in) &&
          (path_out.back().first == end_in));
}
