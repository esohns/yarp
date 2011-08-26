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
#include <rpg_engine_level.h>

#include <rpg_graphics_common.h>
#include <rpg_graphics_iwindow.h>
#include <rpg_graphics_cursor.h>

#include <rpg_map_common.h>

#include <glade/glade.h>
#include <SDL/SDL.h>

#include <ace/Synch.h>

#include <string>
#include <deque>

// forward declaration(s)
class RPG_Client_Engine;

struct RPG_Client_GTK_CBData_t
{
 inline RPG_Client_GTK_CBData_t()
  : lock(NULL, NULL),
    hover_time(0),
    gtk_time(0),
    gtk_main_quit_invoked(false),
    xml(NULL),
    screen(NULL),
    event_timer(NULL),
//     previous_window(NULL),
//     map_window(NULL),
    client_engine(NULL),
//     current_entity(),
    level_engine(NULL)
 {
//    entity.character = NULL;
//    entity.position = std::make_pair(0, 0);
// //    entity.actions;
//    entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
//    entity.graphic = NULL;
 };

  ACE_Thread_Mutex        lock;
  unsigned long           hover_time;
  unsigned long           gtk_time;
  bool                    gtk_main_quit_invoked;
  GladeXML*               xml;
  SDL_Surface*            screen;
  SDL_TimerID             event_timer;
//   RPG_Graphics_IWindow*   previous_window;
//   RPG_Client_WindowLevel* map_window;
  RPG_Client_Engine*      client_engine;
  std::string             schemaRepository;
  RPG_Engine_Entity       entity;
  RPG_Engine_Level*       level_engine;
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
  int    screen_width;
  int    screen_height;
  int    screen_colordepth; // bits/pixel
//   Uint32 screen_flags;
  bool   fullScreen;
  bool   doubleBuffer;
};

struct RPG_Client_MapConfig_t
{
  unsigned long min_room_size; // 0: don't care
  bool          doors;
  bool          corridors;
  unsigned long max_num_doors_per_room;
  bool          maximize_rooms;
  unsigned long num_areas;
  bool          square_rooms;
  unsigned long map_size_x;
  unsigned long map_size_y;
};

struct RPG_Client_Config
{
  // *** reactor ***
  unsigned long                num_threadpool_threads; // 0: don't use threadpool
  // *** UI ***
  std::string                  glade_file;
//   GTK_cb_data_t      gtk_cb_data;
  // *** sound ***
  RPG_Client_SDL_AudioConfig_t audio_config;
  std::string                  sound_directory;
  unsigned long                sound_cache_size;
  std::string                  sound_dictionary;
  // *** graphics ***
  RPG_Client_SDL_VideoConfig_t video_config;
  std::string                  graphics_directory;
  unsigned long                graphics_cache_size;
  std::string                  graphics_dictionary;
  // *** magic ***
  std::string                  magic_dictionary;
  // *** item ***
  std::string                  item_dictionary;
  // *** monster ***
  std::string                  monster_dictionary;
  // *** map ***
  RPG_Client_MapConfig_t       map_config;
  std::string                  map_file;
};

enum RPG_Client_Command
{
  COMMAND_CURSOR_DRAW = 0,
  COMMAND_CURSOR_INVALIDATE_BG,
  COMMAND_CURSOR_RESTORE_BG,
  COMMAND_CURSOR_SET,
  COMMAND_SET_VIEW,
  COMMAND_TILE_HIGHLIGHT_DRAW,
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
typedef std::pair<int, int> RPG_Client_Position_t;

struct RPG_Client_Action
{
  RPG_Client_Command command;
  RPG_Map_Position_t map_position;
  RPG_Graphics_Position_t graphics_position;
  RPG_Graphics_IWindow* window;
  RPG_Graphics_Cursor cursor;
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
  MINIMAPTILE_STAIRS,
  //
  RPG_CLIENT_MINIMAPTILE_MAX,
  RPG_CLIENT_MINIMAPTILE_INVALID
};

#endif
