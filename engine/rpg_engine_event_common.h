﻿/***************************************************************************
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

#ifndef RPG_ENGINE_EVENT_COMMON_H
#define RPG_ENGINE_EVENT_COMMON_H

#include "rpg_engine_common.h"

enum RPG_Engine_EventType
{
  EVENT_ENTITY_ACTIVATE = 0,
  EVENT_ENTITY_SPAWN,
	// -----------------------
	EVENT_QUIT,
  // -----------------------
  RPG_ENGINE_EVENT_MAX,
  RPG_ENGINE_EVENT_INVALID
};

struct RPG_Engine_Event
{
 RPG_Engine_Event ()
  : type (RPG_ENGINE_EVENT_INVALID)
  , entity_id (0)
  , timer_id (-1)
  , free (false)
 {}

  enum RPG_Engine_EventType type;
  RPG_Engine_EntityID_t     entity_id;
  long                      timer_id;
  bool                      free; // clean up ASAP ?
};

#endif
