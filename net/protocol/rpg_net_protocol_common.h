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

#ifndef RPG_NET_PROTOCOL_COMMON_H
#define RPG_NET_PROTOCOL_COMMON_H

#include <string>

#include "rpg_map_common.h"

#include "rpg_engine_command.h"
#include "rpg_engine_common.h"

// *IMPORTANT NOTE*: unfortunately, cannot inherit from enum RPG_Engine_Command (yet ?)
enum RPG_Net_Protocol_Engine_Command
{ // *WARNING*: must match enum RPG_Engine_Command exactly
  NET_COMMAND_ATTACK = 0,
  NET_COMMAND_ATTACK_FULL,
  NET_COMMAND_ATTACK_STANDARD,
  NET_COMMAND_DOOR_CLOSE,
  NET_COMMAND_DOOR_OPEN,
  NET_COMMAND_IDLE,
  NET_COMMAND_RUN,
  NET_COMMAND_SEARCH,
  NET_COMMAND_STEP,
  NET_COMMAND_STOP,
  NET_COMMAND_TRAVEL,
  NET_COMMAND_E2C_ENTITY_ADD,
  NET_COMMAND_E2C_ENTITY_REMOVE,
  NET_COMMAND_E2C_ENTITY_HIT,
  NET_COMMAND_E2C_ENTITY_MISS,
  NET_COMMAND_E2C_ENTITY_CONDITION,
  NET_COMMAND_E2C_ENTITY_POSITION,
  NET_COMMAND_E2C_ENTITY_VISION,
  NET_COMMAND_E2C_ENTITY_LEVEL_UP,
  NET_COMMAND_E2C_ENTITY_STATE,
  NET_COMMAND_E2C_INIT,
  NET_COMMAND_E2C_MESSAGE,
  NET_COMMAND_E2C_QUIT,
  //
  NET_COMMAND_MAX,
  NET_COMMAND_INVALID,
  //
  NET_COMMAND_LEVEL_LOAD = RPG_ENGINE_COMMAND_INVALID + 1,
  NET_COMMAND_PLAYER_LOAD,
  //
  RPG_NET_PROTOCOL_COMMAND_MAX,
  RPG_NET_PROTOCOL_INVALID
};

struct RPG_Net_Protocol_Command
{
  enum RPG_Net_Protocol_Engine_Command command;
  RPG_Map_Position_t                   position;
  RPG_Map_Path_t                       path;
  RPG_Engine_EntityID_t                target;
  ////////////////////////////////////////
  std::string                          xml;
};

#endif
