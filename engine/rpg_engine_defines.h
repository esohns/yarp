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
#define RPG_ENGINE_TASK_GROUP_ID                    102
#define RPG_ENGINE_TASK_THREAD_NAME                 "RPG engine"
#define RPG_ENGINE_TASK_DEF_NUM_THREADS             3
#define RPG_ENGINE_AI_TASK_GROUP_ID                 103
#define RPG_ENGINE_AI_TASK_THREAD_NAME              "RPG AI"
#define RPG_ENGINE_AI_DEF_NUM_THREADS               3
#define RPG_ENGINE_MAX_QUEUE_SLOTS                  10000
//#define RPG_ENGINE_EVENT_PEEK_INTERVAL      50 // ms

// *IMPORTANT NOTE*: lower speed modifier --> less delay --> higher speed
#define RPG_ENGINE_SPEED_MODIFIER                   10 // --> 1/10 speed <=> normal speed
#define RPG_ENGINE_SPEED_MODIFIER_RUNNING           3
#define RPG_ENGINE_FEET_PER_SQUARE                  5

// *TODO*: remove these...
#define RPG_ENGINE_ENVIRONMENT_DEF_PLANE            PLANE_MATERIAL
#define RPG_ENGINE_ENVIRONMENT_DEF_TERRAIN          TERRAIN_UNDER_GROUND
#define RPG_ENGINE_ENVIRONMENT_DEF_CLIMATE          CLIMATE_TEMPERATE
#define RPG_ENGINE_ENVIRONMENT_DEF_TIMEOFDAY        TIMEOFDAY_DAYTIME
#define RPG_ENGINE_ENVIRONMENT_DEF_LIGHTING         AMBIENCE_DARKNESS
#define RPG_ENGINE_ENVIRONMENT_DEF_OUTDOORS         false
// *TODO*: remove these...
#define RPG_ENGINE_ENCOUNTER_DEF_TYPES_MAX          3
#define RPG_ENGINE_ENCOUNTER_DEF_TYPE               "Bugbear"
#define RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL     10 // second(s)
#define RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL_MAX 60 // second(s)
#define RPG_ENGINE_ENCOUNTER_DEF_PROBABILITY        0.7f // --> 70%
#define RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX    50 // [0 --> unlimited]
#define RPG_ENGINE_ENCOUNTER_DEF_AMBLE_PROBABILITY  0.5F // --> 50%

// XML-specific
#define RPG_ENGINE_SCHEMA_FILE                      "rpg_engine.xsd"
// level-specific
#define RPG_ENGINE_LEVEL_DEF_NAME                   "default level"
#define RPG_ENGINE_LEVEL_NAME_MAX_LENGTH            30
#define RPG_ENGINE_LEVEL_FILE_EXT                   ".level"
// state-specific
#define RPG_ENGINE_STATE_EXT                        ".state"
#define RPG_ENGINE_STATE_SUB                        "engine_state"

// *TODO*: move these to client
#define RPG_ENGINE_USES_REACTOR                     false

#define RPG_ENGINE_SUB_DIRECTORY_STRING             "engine"

// gtk
#define RPG_ENGINE_GTK_LEVEL_FILTER                 "levels"

#endif
