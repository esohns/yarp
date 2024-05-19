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

#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "gtk/gtk.h"

#include "ace/Synch_Traits.h"

#include "common.h"

#include "common_parser_common.h"

#include "common_signal_common.h"

#include "common_ui_common.h"

#include "common_ui_gtk_common.h"

#include "rpg_map_common.h"

#include "rpg_net_protocol_configuration.h"

#include "rpg_engine_common.h"

#include "rpg_sound_common.h"
#include "rpg_sound_event.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_iwindow_base.h"
#include "rpg_graphics_style.h"

// forward declaration(s)
class RPG_Client_Engine;
class RPG_Engine;

typedef std::map<RPG_Engine_EntityID_t, enum RPG_Graphics_Sprite> RPG_Client_Entities_t;
typedef RPG_Client_Entities_t::const_iterator RPG_Client_EntitiesIterator_t;

struct RPG_Client_State
{
  struct RPG_Graphics_Style style;
  RPG_Client_Entities_t     entities;
};

struct RPG_Client_GTK_CBData
 : Common_UI_GTK_CBData
{
  RPG_Client_GTK_CBData()
   : Common_UI_GTK_CBData ()
   , doHover (true)
   , hoverTime (0)
   , entityFilter (NULL)
   , mapFilter (NULL)
   , savedStateFilter (NULL)
#if defined (SDL_USE)
   , screen (NULL)
#elif defined (SDL2_USE)
   , renderer (NULL)
   , screen (NULL)
   , GLContext (NULL)
#endif // SDL_USE || SDL2_USE
   , screenLock (NULL,
                 NULL)
   , eventTimer (0)
   , clientEngine (NULL)
   , schemaRepository ()
   , entity ()
   , levelEngine (NULL)
   , levelMetadata ()
   , mapConfiguration ()
   , subClass (RPG_COMMON_SUBCLASS_INVALID)
  {}

  bool                                   doHover;
  unsigned int                           hoverTime;
  GtkFileFilter*                         entityFilter;
  GtkFileFilter*                         mapFilter;
  GtkFileFilter*                         savedStateFilter;
#if defined (SDL_USE)
  SDL_Surface*                           screen;
#elif defined (SDL2_USE)
  SDL_Renderer*                          renderer;
  SDL_Window*                            screen;
  SDL_GLContext                          GLContext;
#endif // SDL_USE || SDL2_USE
  ACE_Thread_Mutex                       screenLock; // video access
  SDL_TimerID                            eventTimer;
  RPG_Client_Engine*                     clientEngine;
  std::string                            schemaRepository;
  struct RPG_Engine_Entity               entity;
  RPG_Engine*                            levelEngine;
  struct RPG_Engine_LevelMetaData        levelMetadata;
  struct RPG_Map_FloorPlan_Configuration mapConfiguration;
  // runtime state
  unsigned int                           assignedSkillPointsPreLevelUp;
  enum RPG_Common_SubClass               subClass;
};

struct RPG_Client_SDL_InputConfiguration
{
  bool use_UNICODE;
  int  key_repeat_initial_delay;
  int  key_repeat_interval;
};

struct RPG_Client_AudioConfiguration
{
  struct RPG_Sound_SDLConfiguration SDL_configuration;
  std::string                       repository;
  std::string                       dictionary;
  bool                              use_CD;
  bool                              mute;
};

struct RPG_Client_Configuration
{
  // *** signals ***
  struct Common_SignalHandlerConfiguration   signal_configuration;
  // *** UI ***
  Common_UI_GTK_Configuration_t              gtk_configuration;
  // *** timers ***
  struct Common_TimerConfiguration           timer_configuration;
  // *** event ***
  struct Common_EventDispatchConfiguration   dispatch_configuration;
  // *** UI ***
  //std::string                           glade_file;
  // *** input ***
  struct RPG_Client_SDL_InputConfiguration   input_configuration;
  // *** sound ***
  struct RPG_Client_AudioConfiguration       audio_configuration;
  // *** graphics ***
  struct RPG_Graphics_SDL_VideoConfiguration video_configuration;
  std::string                                graphics_directory;
  std::string                                graphics_dictionary;
  // *** network ***
  struct Common_FlexBisonParserConfiguration parser_configuration;
  struct RPG_Net_Protocol_Configuration      protocol_configuration;
  // *** magic ***
  std::string                                magic_dictionary;
  // *** item ***
  std::string                                item_dictionary;
  // *** monster ***
  std::string                                monster_dictionary;
  // *** map ***
  struct RPG_Map_FloorPlan_Configuration     map_configuration;
  std::string                                map_file;
};

enum RPG_Client_Command
{
  COMMAND_CURSOR_DRAW = 0,
  COMMAND_CURSOR_INVALIDATE_BG,
  COMMAND_CURSOR_RESTORE_BG,
  COMMAND_CURSOR_SET,
  COMMAND_ENTITY_DRAW,
  COMMAND_ENTITY_REMOVE,
  COMMAND_ENTITY_RESTORE_BG,
  COMMAND_PLAY_SOUND,
  COMMAND_SET_VIEW,
  COMMAND_SET_VISION_RADIUS,
  COMMAND_TILE_HIGHLIGHT_DRAW,
  COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG,
  COMMAND_TILE_HIGHLIGHT_RESTORE_BG,
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
  enum RPG_Client_Command   command;
  RPG_Map_Position_t        previous;
  // *NOTE*: depending on the scenario, these could be map or screen coordinates !
  RPG_Client_Position_t     position;
  RPG_Graphics_IWindowBase* window;
  enum RPG_Graphics_Cursor  cursor;
  RPG_Engine_EntityID_t     entity_id;
  enum RPG_Sound_Event      sound;
  std::string               message;
  // *TODO*: this does not really belong here...
  RPG_Map_Path_t            path;
  RPG_Map_Position_t        source;
  RPG_Map_Positions_t       positions;
  ACE_UINT8                 radius; // map squares
};
typedef std::deque<struct RPG_Client_Action> RPG_Client_Actions_t;
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

typedef std::vector<SDL_Surface*> RPG_Client_BlendingMaskCache_t;
typedef RPG_Client_BlendingMaskCache_t::iterator RPG_Client_BlendingMaskCacheIterator_t;

#endif
