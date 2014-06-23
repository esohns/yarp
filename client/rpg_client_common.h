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

#ifndef RPG_CLIENT_COMMON_H
#define RPG_CLIENT_COMMON_H

#include "rpg_engine_common.h"
#include "rpg_engine.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_iwindow_base.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_style.h"

#include "rpg_sound_common.h"
#include "rpg_sound_event.h"

#include "rpg_map_common.h"

#include "glade/glade.h"

#include "gtk/gtk.h"

#include "SDL.h"

#include "ace/Synch.h"

#include <map>
#include <deque>
#include <string>

// forward declaration(s)
class RPG_Client_Engine;

typedef std::deque<std::string> RPG_Client_MessageStack_t;
typedef RPG_Client_MessageStack_t::const_iterator RPG_Client_MessageStackConstIterator_t;
//typedef std::deque<ACE_Log_Record> RPG_Client_LogRecordStack_t;

typedef std::map<RPG_Engine_EntityID_t, RPG_Graphics_Sprite> RPG_Client_Entities_t;
typedef RPG_Client_Entities_t::const_iterator RPG_Client_EntitiesIterator_t;

struct RPG_Client_State_t
{
  RPG_Graphics_Style    style;
  RPG_Client_Entities_t entities;
};

struct RPG_Client_GTK_CBData_t
{
 inline RPG_Client_GTK_CBData_t()
  : lock(NULL,
         NULL),
    do_hover(true),
    hover_time(0),
    //GTK_time(0),
    //GTK_done(false),
    XML(NULL),
//		log_stack(),
    entity_filter(NULL),
    map_filter(NULL),
    screen(NULL),
    event_timer(NULL),
    client_engine(NULL),
//    schema_repository(),
//    entity(),
    level_engine(NULL)//,
//		level_metadata(),
//    map_configuration()
 { };

  ACE_Recursive_Thread_Mutex        lock;
  bool                              do_hover;
  unsigned int                      hover_time;
  //unsigned int                      GTK_time;
  //bool                              GTK_done;
  GladeXML*                         XML;
	RPG_Client_MessageStack_t         log_stack;
  GtkFileFilter*                    entity_filter;
  GtkFileFilter*                    map_filter;
  GtkFileFilter*                    savedstate_filter;
  SDL_Surface*                      screen;
  ACE_Thread_Mutex                  screen_lock; // video access
  SDL_TimerID                       event_timer;
  RPG_Client_Engine*                client_engine;
  std::string                       schema_repository;
  RPG_Engine_Entity_t               entity;
  RPG_Engine*                       level_engine;
  RPG_Engine_LevelMetaData_t        level_metadata;
  RPG_Map_FloorPlan_Configuration_t map_configuration;
};

struct RPG_Client_SDL_InputConfiguration_t
{
	bool use_UNICODE;
  int  key_repeat_initial_delay;
	int  key_repeat_interval;
};

struct RPG_Client_AudioConfiguration_t
{
  RPG_Sound_SDLConfiguration_t SDL_configuration;
  std::string                  repository;
  std::string                  dictionary;
  bool                         use_CD;
  bool                         mute;
};

struct RPG_Client_NetworkConfiguration_t
{
  std::string    server;
  unsigned short port;
  std::string    password;
  std::string    nick;
  std::string    user;
  std::string    realname;
  std::string    channel;
};

struct RPG_Client_Configuration_t
{
  // *** reactor ***
  unsigned int                          num_dispatch_threads; // 1: don't use a threadpool
  // *** UI ***
  std::string                           glade_file;
	// *** input ***
	RPG_Client_SDL_InputConfiguration_t   input_configuration;
  // *** sound ***
  RPG_Client_AudioConfiguration_t       audio_configuration;
  // *** graphics ***
  RPG_Graphics_SDL_VideoConfiguration_t video_configuration;
  std::string                           graphics_directory;
  std::string                           graphics_dictionary;
  // *** network ***
  RPG_Client_NetworkConfiguration_t     network_configuration;
  // *** magic ***
  std::string                           magic_dictionary;
  // *** item ***
  std::string                           item_dictionary;
  // *** monster ***
  std::string                           monster_dictionary;
  // *** map ***
  RPG_Map_FloorPlan_Configuration_t     map_configuration;
  std::string                           map_file;
};

enum RPG_Client_Command
{
  COMMAND_CURSOR_DRAW = 0,
  COMMAND_CURSOR_INVALIDATE_BG,
  COMMAND_CURSOR_SET,
  COMMAND_ENTITY_DRAW,
  COMMAND_ENTITY_REMOVE,
  COMMAND_PLAY_SOUND,
  COMMAND_SET_VIEW,
  COMMAND_SET_VISION_RADIUS,
  COMMAND_TILE_HIGHLIGHT_DRAW,
  COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG,
  COMMAND_TILE_HIGHLIGHT_RESTORE_BG,
//  COMMAND_TILE_HIGHLIGHT_STORE_BG,
  COMMAND_TOGGLE_DOOR,
  COMMAND_WINDOW_BORDER_DRAW,
  COMMAND_WINDOW_DRAW,
  COMMAND_WINDOW_HIDE,
  COMMAND_WINDOW_INIT,
  COMMAND_WINDOW_REFRESH,
  COMMAND_WINDOW_UPDATE_MESSAGEWINDOW,
  COMMAND_WINDOW_UPDATE_MINIMAP,
  //
  RPG_CLIENT_COMMAND_MAX,
  RPG_CLIENT_COMMAND_INVALID
};

enum RPG_Client_Repository
{
  REPOSITORY_PROFILES = 0,
  REPOSITORY_MAPS,
  REPOSITORY_ENGINESTATE,
  //
  RPG_CLIENT_REPOSITORY_MAX,
  RPG_CLIENT_REPOSITORY_INVALID
};

typedef std::pair<int, int> RPG_Client_SignedPosition_t;
typedef std::pair<unsigned int, unsigned int> RPG_Client_Position_t;

struct RPG_Client_Action
{
  RPG_Client_Command        command;
  RPG_Map_Position_t        previous;
  // *NOTE*: depending on the scenario, these could be map or screen coordinates !
  RPG_Client_Position_t     position;
  RPG_Graphics_IWindowBase* window;
  RPG_Graphics_Cursor       cursor;
  RPG_Engine_EntityID_t     entity_id;
  RPG_Sound_Event           sound;
  std::string               message;
  // *TODO*: this does not really belong here...
  RPG_Map_Path_t            path;
  RPG_Map_Position_t        source;
  RPG_Map_Positions_t       positions;
  unsigned char             radius; // map squares
};
typedef std::deque<RPG_Client_Action> RPG_Client_Actions_t;
typedef RPG_Client_Actions_t::const_iterator RPG_Client_ActionsIterator_t;

// *TODO*
enum RPG_Client_MiniMapTile
{
  MINIMAPTILE_NONE = 0,
  MINIMAPTILE_DOOR,
  MINIMAPTILE_FLOOR,
  MINIMAPTILE_MONSTER,
  MINIMAPTILE_PLAYER,
  MINIMAPTILE_PLAYER_ACTIVE,
  MINIMAPTILE_STAIRS,
  MINIMAPTILE_WALL,
  //
  RPG_CLIENT_MINIMAPTILE_MAX,
  RPG_CLIENT_MINIMAPTILE_INVALID
};

enum RPG_Client_SelectionMode
{
  SELECTIONMODE_AIM_CIRCLE = 0,
  SELECTIONMODE_AIM_SQUARE,
  SELECTIONMODE_NORMAL,
  SELECTIONMODE_PATH,
  //
  RPG_CLIENT_SELECTIONMODE_MAX,
  RPG_CLIENT_SELECTIONMODE_INVALID
};

typedef std::map<RPG_Engine_EntityID_t, RPG_Map_Positions_t> RPG_Client_SeenPositions_t;
typedef RPG_Client_SeenPositions_t::const_iterator RPG_Client_SeenPositionsConstIterator_t;
typedef RPG_Client_SeenPositions_t::iterator RPG_Client_SeenPositionsIterator_t;

typedef std::vector<SDL_Surface*> RPG_Client_BlendingMaskCache_t;
typedef RPG_Client_BlendingMaskCache_t::iterator RPG_Client_BlendingMaskCacheIterator_t;

#endif
