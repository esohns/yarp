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

#include "ace/Basic_Types.h"

#include "rpg_map_common.h"

#include "rpg_engine_common.h"

#include "rpg_graphics_cursor.h"
#include "rpg_graphics_windowtype.h"

#include "rpg_sound_event.h"

enum RPG_Net_Protocol_MessageType
{
  NET_MESSAGE_TYPE_INITIAL = 0,
  NET_MESSAGE_TYPE_LEVEL,
  NET_MESSAGE_TYPE_PLAYER,
  NET_MESSAGE_TYPE_ENGINE_COMMAND,
  NET_MESSAGE_TYPE_CLIENT_COMMAND,
  ////////////////////////////////////////
  NET_MESSAGE_TYPE_MAX,
  NET_MESSAGE_TYPE_INVALID
};

struct RPG_Net_Protocol_Command
{
  RPG_Net_Protocol_Command ()
   : type (NET_MESSAGE_TYPE_INVALID)
   , xml ()
   , command (-1)
   , position ()
   , path ()
   , entity_id (0)
   , previous (std::make_pair (std::numeric_limits<unsigned int>::max (),
                               std::numeric_limits<unsigned int>::max ()))
   , window (RPG_GRAPHICS_WINDOWTYPE_INVALID)
   , cursor (RPG_GRAPHICS_CURSOR_INVALID)
   , sound (RPG_SOUND_EVENT_INVALID)
   , message ()
   , source (std::make_pair (std::numeric_limits<unsigned int>::max (),
                             std::numeric_limits<unsigned int>::max ()))
   , positions ()
   , radius (0)
  {}

  enum RPG_Net_Protocol_MessageType    type;

  ////////////////////////////////////////

  std::string                          xml;

  ////////////////////////////////////////

  int                                  command; // client- || engine-

  ////////////////////////////////////////

  RPG_Map_Position_t                   position;
  RPG_Map_Path_t                       path;
  RPG_Engine_EntityID_t                entity_id;

  ////////////////////////////////////////
  RPG_Map_Position_t                   previous;
  enum RPG_Graphics_WindowType         window;
  enum RPG_Graphics_Cursor             cursor;
  enum RPG_Sound_Event                 sound;
  std::string                          message;
  RPG_Map_Position_t                   source;
  RPG_Map_Positions_t                  positions;
  ACE_UINT8                            radius;
};

struct RPG_Net_Protocol_MessageData
{
  RPG_Net_Protocol_MessageData ()
   : connectionId (ACE_INVALID_HANDLE)
   , command ()
  {}

  ACE_HANDLE                      connectionId;
  struct RPG_Net_Protocol_Command command;
};

#endif
