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

#ifndef RPG_ENGINE_DEFINES_H
#define RPG_ENGINE_DEFINES_H

// task-specific
#define RPG_ENGINE_DEF_TASK_GROUP_ID        1
#define RPG_ENGINE_DEF_AI_TASK_GROUP_ID     2
#define RPG_ENGINE_DEF_MAX_QUEUE_SLOTS      100
#define RPG_ENGINE_DEF_EVENT_PEEK_INTERVAL  50 // ms

#define RPG_ENGINE_ROUND_INTERVAL           6 // s
#define RPG_ENGINE_FEET_PER_SQUARE          5

#define RPG_ENGINE_DEF_MAX_NUM_SPAWNED      0 // don't auto-spawn
#define RPG_ENGINE_DEF_MAX_RAD_SPAWNED      5
#define RPG_ENGINE_DEF_ENTITY_SPRITE_FILE   "human.png"
#define RPG_ENGINE_DEF_CREATURE_SPRITE_FILE "goblin.png"

// XML-specific
#define RPG_ENGINE_SCHEMA_FILE              "rpg_engine.xsd"
#define RPG_ENGINE_LEVEL_NAME_MAX_LENGTH    30
#define RPG_ENGINE_DEF_LEVEL_NAME           "default level"
#define RPG_ENGINE_DEF_LEVEL_FILE           "default_level"
#define RPG_ENGINE_LEVEL_FILE_EXT           ".level"

// CONFIGDIR-specific
#define RPG_ENGINE_DEF_CONFIG_SUB           "engine"

#endif
