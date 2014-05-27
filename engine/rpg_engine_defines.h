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
#define RPG_ENGINE_TASK_GROUP_ID                     102
#define RPG_ENGINE_AI_TASK_GROUP_ID                  103
#define RPG_ENGINE_AI_TASK_THREAD_NAME               "RPG AI"
#define RPG_ENGINE_AI_DEF_NUM_THREADS                3
#define RPG_ENGINE_TASK_THREAD_NAME                  "RPG state engine"
#define RPG_ENGINE_MAX_QUEUE_SLOTS                   100
//#define RPG_ENGINE_EVENT_PEEK_INTERVAL      50 // ms

#define RPG_ENGINE_SPEED_MODIFIER                    20 // --> 1/20 speed <==> normal
#define RPG_ENGINE_ROUND_INTERVAL                    6 // s
#define RPG_ENGINE_FEET_PER_SQUARE                   5

#define RPG_ENGINE_DEF_ENTITY_SPRITE                 SPRITE_HUMAN

// level-specific
#define RPG_ENGINE_LEVEL_DEF_NAME                    "default level"

#define RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_PLANE       PLANE_MATERIAL
#define RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_TERRAIN     TERRAIN_UNDER_GROUND
#define RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_CLIMATE     CLIMATE_TEMPERATE
#define RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_TIMEOFDAY   TIMEOFDAY_DAYTIME
#define RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_LIGHTING    AMBIENCE_DARKNESS
#define RPG_ENGINE_LEVEL_ENVIRONMENT_DEF_OUTDOORS    false

#define RPG_ENGINE_LEVEL_AI_SPAWN_TYPES_MAX          3
#define RPG_ENGINE_LEVEL_AI_DEF_SPAWN_TYPE           "Bugbear"
#define RPG_ENGINE_LEVEL_AI_DEF_SPAWN_TIMER_INTERVAL 10 // second(s)
#define RPG_ENGINE_LEVEL_AI_SPAWN_TIMER_INTERVAL_MAX 60 // second(s)
#define RPG_ENGINE_LEVEL_AI_DEF_SPAWN_PROBABILITY    0.3F // --> 30 %
#define RPG_ENGINE_LEVEL_AI_NUM_SPAWNED_MAX          50 // [0 --> unlimited]
#define RPG_ENGINE_LEVEL_AI_DEF_AMBLE_PROBABILITY    0.5F // --> 50 %

// XML-specific
#define RPG_ENGINE_SCHEMA_FILE                       "rpg_engine.xsd"
#define RPG_ENGINE_LEVEL_NAME_MAX_LENGTH             30
#define RPG_ENGINE_DEF_LEVEL_FILE                    "dungeon_one"
#define RPG_ENGINE_LEVEL_FILE_EXT                    ".level"
// *TODO*: convert if necessary...
#define RPG_ENGINE_ENTITY_DEF_FILE                   "default_player"
#define RPG_ENGINE_ENTITY_PROFILE_EXT                ".entity"
// default (profiles) data directory tree organization
#define RPG_ENGINE_ENTITY_SUB                        "profiles"

// network-specific
#define RPG_ENGINE_DEF_NETWORK_SERVER                "localhost"
#define RPG_ENGINE_DEF_NETWORK_PORT                  6667
#define RPG_ENGINE_DEF_NETWORK_PASSWORD              "secretpasswordhere"
#define RPG_ENGINE_DEF_NETWORK_NICK                  "Wiz"
#define RPG_ENGINE_DEF_NETWORK_USER                  "user"
#define RPG_ENGINE_DEF_NETWORK_REALNAME              "Ronnie Reagan"
#define RPG_ENGINE_DEF_NETWORK_CHANNEL               "#foobar"

#endif
