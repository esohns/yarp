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

#include <rpg_engine_common.h>
#include <rpg_engine.h>

#include <rpg_graphics_common.h>
#include <rpg_graphics_iwindow.h>
#include <rpg_graphics_cursor.h>

#include <rpg_map_common.h>

#include <glade/glade.h>
#include <gtk/gtk.h>
#include <SDL/SDL.h>

#include <ace/Synch.h>

#include <string>
#include <deque>
#include <map>

// forward declaration(s)
class RPG_Client_Engine;

struct RPG_Client_GTK_CBData_t
{
 inline RPG_Client_GTK_CBData_t()
  : lock(NULL, NULL),
    do_hover(true),
    hover_time(0),
    gtk_time(0),
    gtk_main_quit_invoked(false),
    xml(NULL),
    entity_filter(NULL),
    map_filter(NULL),
    screen(NULL),
    event_timer(NULL),
    client_engine(NULL),
//    schemaRepository(),
//     entity(),
    level_engine(NULL)//,
//     map_config()
 {
//    entity.character = NULL;
//    entity.position = std::make_pair(0, 0);
//    entity.actions;
//    entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
//    entity.graphic = NULL;
 };

  ACE_Thread_Mutex           lock;
  bool                       do_hover;
  unsigned int               hover_time;
  unsigned int               gtk_time;
  bool                       gtk_main_quit_invoked;
  GladeXML*                  xml;
  GtkFileFilter*             entity_filter;
  GtkFileFilter*             map_filter;
  SDL_Surface*               screen;
  SDL_TimerID                event_timer;
  RPG_Client_Engine*         client_engine;
  std::string                schemaRepository;
  RPG_Engine_Entity          entity;
  RPG_Engine*                level_engine;
  RPG_Engine_LevelMeta_t     level_meta_data;
  RPG_Map_FloorPlan_Config_t map_config;
};

// *NOTE* types as used by SDL
struct RPG_Client_SDL_AudioConfig_t
{
  int    frequency;
  Uint16 format;
//   Uint8  channels;
  int    channels;
  Uint16 samples;
};

// *NOTE* types as used by SDL
struct RPG_Client_SDL_VideoConfig_t
{
  int  screen_width;
  int  screen_height;
  int  screen_colordepth; // bits/pixel
//   Uint32 screen_flags;
  bool fullScreen;
  bool doubleBuffer;
};

struct RPG_Client_Config
{
  // *** reactor ***
  unsigned int                 num_threadpool_threads; // 0: don't use threadpool
  // *** UI ***
  std::string                  glade_file;
//   GTK_cb_data_t      gtk_cb_data;
  // *** sound ***
  RPG_Client_SDL_AudioConfig_t audio_config;
  std::string                  sound_directory;
  unsigned int                 sound_cache_size;
  std::string                  sound_dictionary;
  // *** graphics ***
  RPG_Client_SDL_VideoConfig_t video_config;
  std::string                  graphics_directory;
  unsigned int                 graphics_cache_size;
  std::string                  graphics_dictionary;
  // *** magic ***
  std::string                  magic_dictionary;
  // *** item ***
  std::string                  item_dictionary;
  // *** monster ***
  std::string                  monster_dictionary;
  // *** map ***
  RPG_Map_FloorPlan_Config_t   map_config;
  std::string                  map_file;
};

enum RPG_Client_Command
{
  COMMAND_CURSOR_DRAW = 0,
  COMMAND_CURSOR_INVALIDATE_BG,
  COMMAND_CURSOR_RESTORE_BG,
  COMMAND_CURSOR_SET,
  COMMAND_ENTITY_DRAW,
  COMMAND_SET_VIEW,
  COMMAND_TILE_HIGHLIGHT_DRAW,
  COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG,
  COMMAND_TILE_HIGHLIGHT_RESTORE_BG,
  COMMAND_TILE_HIGHLIGHT_STORE_BG,
  COMMAND_TOGGLE_DOOR,
  COMMAND_WINDOW_BORDER_DRAW,
  COMMAND_WINDOW_DRAW,
  COMMAND_WINDOW_INIT,
  COMMAND_WINDOW_REFRESH,
  //
  RPG_CLIENT_COMMAND_MAX,
  RPG_CLIENT_COMMAND_INVALID
};

typedef std::pair<int, int> RPG_Client_SignedPosition_t;
typedef std::pair<unsigned int, unsigned int> RPG_Client_Position_t;

struct RPG_Client_Action
{
  RPG_Client_Command    command;
  // *NOTE*: depending on the scenario, these could be map or screen coordinates !
  RPG_Client_Position_t position;
  RPG_Graphics_IWindow* window;
  RPG_Graphics_Cursor   cursor;
  RPG_Engine_EntityID_t entity_id;
  RPG_Map_Path_t        path;
};
typedef std::deque<RPG_Client_Action> RPG_Client_Actions_t;
typedef RPG_Client_Actions_t::const_iterator RPG_Client_ActionsIterator_t;

enum RPG_Client_MiniMapTile
{
  MINIMAPTILE_NONE = 0,
  MINIMAPTILE_DOOR,
  MINIMAPTILE_FLOOR,
  MINIMAPTILE_MONSTER,
  MINIMAPTILE_PLAYER,
  MINIMAPTILE_PLAYER_ACTIVE,
  MINIMAPTILE_STAIRS,
  //
  RPG_CLIENT_MINIMAPTILE_MAX,
  RPG_CLIENT_MINIMAPTILE_INVALID
};

enum RPG_Client_SelectionMode
{
  SELECTIONMODE_NORMAL = 0,
  SELECTIONMODE_PATH,
  //
  RPG_CLIENT_SELECTIONMODE_MAX,
  RPG_CLIENT_SELECTIONMODE_INVALID
};

typedef std::map<RPG_Engine_EntityID_t, RPG_Map_Positions_t> RPG_Client_SeenPositions_t;
typedef RPG_Client_SeenPositions_t::const_iterator RPG_Client_SeenPositionsConstIterator_t;

#endif
