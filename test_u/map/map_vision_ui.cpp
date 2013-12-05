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
#include "stdafx.h"

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_client_defines.h"
#include "rpg_client_common.h"
#include "rpg_client_engine.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_window_main.h"
#include "rpg_client_window_level.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_ui_tools.h"

#include "rpg_sound_defines.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_engine_defines.h"
#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"

#include "rpg_map_defines.h"
#include "rpg_map_common.h"
#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_monster_defines.h"
#include "rpg_monster_dictionary.h"

#include "rpg_player_defines.h"

#include "rpg_item_defines.h"
#include "rpg_item_commodity.h"
#include "rpg_item_instance_manager.h"

#include "rpg_character_defines.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include <ace/ACE.h>
#include <ace/High_Res_Timer.h>
#include <ace/Get_Opt.h>
#include <ace/Log_Msg.h>

#include <SDL.h>

#include <string>
#include <sstream>
#include <iostream>

#define MAP_VISION_UI_DEF_FLOOR_PLAN "test_plan"

Uint32
event_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  RPG_TRACE(ACE_TEXT("::event_timer_SDL_cb"));

  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(argument_in);
  ACE_ASSERT(data);

  SDL_Event sdl_event;
  sdl_event.type = SDL_NOEVENT;

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(data->lock);

    data->hover_time += interval_in;
    data->gtk_time += interval_in;
    if (data->do_hover &&
        (data->hover_time >= RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY))
    {
      // mouse is hovering --> trigger an event
      sdl_event.type = RPG_GRAPHICS_SDL_HOVEREVENT;
      sdl_event.user.code = static_cast<int>(data->hover_time);

      if (SDL_PushEvent(&sdl_event))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                   SDL_GetError()));
    } // end IF

    // dispatch GTK events (if any ?)
    if (data->gtk_time < RPG_CLIENT_SDL_GTKEVENT_RESOLUTION)
      return interval_in; // re-schedule

    data->gtk_time = 0;
  } // end lock scope

  // dispatch a pending GTK event
  GDK_THREADS_ENTER();
  if (gtk_events_pending())
  {
    // there are pending GTK events --> trigger an event
    sdl_event.type = RPG_CLIENT_SDL_GTKEVENT;

    if (SDL_PushEvent(&sdl_event))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                 SDL_GetError()));
  } // end IF
  GDK_THREADS_LEAVE();

  // trigger regular screen refreshes !
  sdl_event.type = RPG_CLIENT_SDL_TIMEREVENT;

  if (SDL_PushEvent(&sdl_event))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               SDL_GetError()));

  // re-schedule
  return interval_in;
}

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string base_path;
#ifdef BASEDIR
  base_path = ACE_TEXT_ALWAYS_CHAR(BASEDIR);
#endif
  std::string config_path = RPG_Common_File_Tools::getConfigDataDirectory(base_path,
                                                                          true);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE]: graphics dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE]: item dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  path = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY);
#else
  path = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY));
#endif
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(MAP_VISION_UI_DEF_FLOOR_PLAN);
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  std::cout << ACE_TEXT("-l [FILE]: level map (*") << ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT) << ACE_TEXT(") [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("monster");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += RPG_MONSTER_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-m [FILE]: monster dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  path = ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE);
#else
  path = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE));
#endif
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY);
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENTITY_PROFILE_EXT);
  std::cout << ACE_TEXT("-p [FILE]: entity profile (*") << ACE_TEXT(RPG_ENGINE_ENTITY_PROFILE_EXT) << ACE_TEXT(") [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT("-v       : print version information and exit") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& graphicsDictionary_out,
                  std::string& itemDictionary_out,
                  std::string& levelMap_out,
                  std::string& monsterDictionary_out,
                  std::string& entityProfile_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  std::string base_path;
#ifdef BASEDIR
  base_path = ACE_TEXT_ALWAYS_CHAR(BASEDIR);
#endif
  std::string config_path = RPG_Common_File_Tools::getConfigDataDirectory(base_path,
                                                                          true);

  // init results
  graphicsDictionary_out = config_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  itemDictionary_out = config_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  levelMap_out = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY);
#else
  levelMap_out = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY));
#endif
  levelMap_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  levelMap_out += ACE_TEXT_ALWAYS_CHAR(MAP_VISION_UI_DEF_FLOOR_PLAN);
  levelMap_out += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  monsterDictionary_out = config_path;
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR("character");
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR("monster");
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monsterDictionary_out += RPG_MONSTER_DEF_DICTIONARY_FILE;

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  entityProfile_out = ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE);
#else
  entityProfile_out = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE));
#endif
  entityProfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  entityProfile_out += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY);
  entityProfile_out += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENTITY_PROFILE_EXT);

  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("g:i:l:m:p:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'g':
      {
        graphicsDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'i':
      {
        itemDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'l':
      {
        levelMap_out = argumentParser.opt_arg();

        break;
      }
      case 'm':
      {
        monsterDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'p':
      {
        entityProfile_out = argumentParser.opt_arg();

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      // error handling
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   argumentParser.last_option()));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%c\", aborting\n"),
                   option));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

bool
do_initGUI(const std::string& graphicsDictionary_in,
           const std::string& graphicsDirectory_in,
           RPG_Client_GTK_CBData_t& userData_in,
           const RPG_Client_SDL_VideoConfig_t& videoConfig_in)
{
  RPG_TRACE(ACE_TEXT("::do_initGUI"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(graphicsDirectory_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid directory \"%s\", aborting\n"),
               graphicsDirectory_in.c_str()));

    return false;
  } // end IF
  RPG_Graphics_Common_Tools::init(graphicsDirectory_in,
                                  RPG_CLIENT_DEF_GRAPHICS_CACHESIZE,
                                  false);
  RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(graphicsDictionary_in
#ifdef _DEBUG
                                                      ,true
#else
                                                      ,false
#endif
                                                      );

  // init SDL UI handling

  // ***** keyboard setup *****
  // enable Unicode translation
  SDL_EnableUNICODE(1);
  // enable key repeat
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                      SDL_DEFAULT_REPEAT_INTERVAL);
//   // ignore keyboard events
//   SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
//   SDL_EventState(SDL_KEYUP, SDL_IGNORE);

  // SDL event filter (filter mouse motion events and the like)
//   SDL_SetEventFilter(event_filter_SDL_cb);

  // ***** window/screen setup *****
  // set window caption
  std::string caption;
  caption = ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE);
//   caption += ACE_TEXT_ALWAYS_CHAR(" ");
//   caption += RPG_VERSION;
  gchar* caption_utf8 = RPG_Client_UI_Tools::Locale2UTF8(caption);
  SDL_WM_SetCaption(caption.c_str(),  // window caption
                    caption.c_str()); // icon caption
  // clean up
  g_free(caption_utf8);
  // set window icon
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_WM_ICON;
  RPG_Graphics_t icon_graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT(icon_graphic.type.image == IMAGE_WM_ICON);
  std::string path = graphicsDirectory_in;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_DEF_IMAGES_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += icon_graphic.file;
  SDL_Surface* icon_image = NULL;
  icon_image = RPG_Graphics_Surface::load(path,   // graphics file
                                          false); // don't convert to display format (no screen yet !)
  if (!icon_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
               path.c_str()));

    return false;
  } // end IF
  SDL_WM_SetIcon(icon_image, // surface
                 NULL);      // mask (--> everything)
//   // don't show (double) cursor
//   SDL_ShowCursor(SDL_DISABLE);

  userData_in.screen = RPG_Graphics_SDL_Tools::initScreen(videoConfig_in.screen_width,
                                                          videoConfig_in.screen_height,
                                                          videoConfig_in.screen_colordepth,
                                                          videoConfig_in.doubleBuffer,
                                                          videoConfig_in.useOpenGL,
                                                          videoConfig_in.fullScreen);
  if (!userData_in.screen)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_SDL_Tools::initScreen(%d,%d,%d), aborting\n"),
               videoConfig_in.screen_width,
               videoConfig_in.screen_height,
               videoConfig_in.screen_colordepth));

    return false;
  } // end IF

  return true;
}

void
do_work(const RPG_Client_Config& config_in,
        const std::string& schemaRepository_in,
        const std::string& levelFilename_in,
        const std::string& playerProfile_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  std::string empty;
  // step0: init: random seed, string conversion facilities, ...
  RPG_Engine_Common_Tools::init(schemaRepository_in,
                                config_in.magic_dictionary,
                                config_in.item_dictionary,
                                config_in.monster_dictionary);

  // step1: load level map
  RPG_Engine_Level_t level = RPG_Engine_Level::load(levelFilename_in,
                                                    schemaRepository_in);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded level map: \"%s\"\n%s\n"),
             levelFilename_in.c_str(),
             RPG_Map_Level::info(level.map).c_str()));

  // step2: process doors
  RPG_Map_Positions_t door_positions;
  for (RPG_Map_DoorsConstIterator_t iterator = level.map.plan.doors.begin();
       iterator != level.map.plan.doors.end();
       iterator++)
  {
    // *WARNING*: set iterators are CONST for a good reason !
    // --> (but we know what we're doing)...
    const_cast<RPG_Map_Door_t&>(*iterator).outside = RPG_Map_Common_Tools::door2exitDirection((*iterator).position,
                                                                                              level.map.plan);

    door_positions.insert((*iterator).position);
  } // end FOR

  // step2: init client / engine / gtk cb user data
  RPG_Client_Engine client_engine;
  RPG_Engine        level_engine;
  RPG_Client_GTK_CBData_t userData;
//   userData.lock;
  userData.do_hover              = true;
  userData.hover_time            = 0;
  userData.gtk_time              = 0;
  userData.gtk_main_quit_invoked = false;
  userData.xml                   = NULL;
  userData.entity_filter         = NULL;
  userData.map_filter            = NULL;
  userData.screen                = NULL;
  userData.event_timer           = NULL;
  userData.client_engine         = &client_engine;
  userData.schemaRepository      = schemaRepository_in;

  GDK_THREADS_ENTER();
  if (!do_initGUI(config_in.graphics_dictionary,
                  config_in.graphics_directory,
                  userData,                      // GTK cb data
                  config_in.video_config))       // SDL_video configuration
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize video, aborting\n")));

    GDK_THREADS_LEAVE();

    return;
  } // end IF
  GDK_THREADS_LEAVE();
  ACE_ASSERT(userData.screen);
  //ACE_ASSERT(userData.xml);

  RPG_Client_Common_Tools::init(config_in.audio_config.sdl_config,
                                config_in.audio_config.repository,
                                config_in.audio_config.use_CD,
                                RPG_SOUND_DEF_CACHESIZE,
                                config_in.audio_config.mute,
                                config_in.audio_config.dictionary,
                                config_in.graphics_directory,
                                RPG_CLIENT_DEF_GRAPHICS_CACHESIZE,
                                config_in.graphics_dictionary,
                                true);

  userData.entity                = RPG_Engine_Common_Tools::loadEntity(playerProfile_in,
                                                                       schemaRepository_in);
  ACE_ASSERT(userData.entity.character);
  userData.entity.position = level.map.start;
//   userData.entity.actions();
  //userData.entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  //userData.entity.graphic = NULL;
  userData.level_engine = &level_engine;
  userData.map_config = config_in.map_config;

  // ***** mouse setup *****
  SDL_WarpMouse((userData.screen->w / 2),
                (userData.screen->h / 2));

//   // step3: run intro
//   if (!do_runIntro())
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to run intro, aborting\n")));
//
//     return;
//   } // end IF

  // step5b: setup style
  RPG_Graphics_MapStyle_t map_style;
  map_style.floor_style = RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE;
  map_style.edge_style = RPG_CLIENT_DEF_GRAPHICS_EDGESTYLE;
  map_style.wall_style = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE;
  map_style.half_height_walls = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF;
  map_style.door_style = RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE;

  // step5c: level engine
  level_engine.init(&client_engine,
                    level);
  level_engine.start();
  if (!level_engine.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start level engine, aborting\n")));

    return;
  } // end IF

  // step5d: setup main "window"
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = RPG_CLIENT_DEF_GRAPHICS_WINDOWSTYLE_TYPE;
  std::string title = ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE);
  RPG_Client_Window_Main mainWindow(RPG_Graphics_Size_t(userData.screen->w,
                                                        userData.screen->h), // size
                                    type,                                    // interface elements
                                    title,                                   // title (== caption)
                                    FONT_MAIN_LARGE);                        // title font
  mainWindow.setScreen(userData.screen);
  mainWindow.init(&client_engine,
                  RPG_CLIENT_DEF_WINDOW_EDGE_AUTOSCROLL,
                  &level_engine,
                  map_style);

  // step5e: client engine
  client_engine.init(&level_engine,
                     mainWindow.child(WINDOW_MAP),
                     userData.xml);

  // step5f: trigger initial drawing
  RPG_Client_Action client_action;
  client_action.command = COMMAND_WINDOW_DRAW;
  client_action.position = std::make_pair(0, 0);
  client_action.window = &mainWindow;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_engine.action(client_action);
  client_action.command = COMMAND_WINDOW_REFRESH;
  client_action.window = &mainWindow;
  client_engine.action(client_action);

  RPG_Client_Window_Level* level_window = dynamic_cast<RPG_Client_Window_Level*>(mainWindow.child(WINDOW_MAP));
  ACE_ASSERT(level_window);

  // activate the current character
  RPG_Engine_EntityID_t id = level_engine.add(&(userData.entity));
  level_engine.setActive(id);
  // init/add entity to the graphics cache
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->init(level_window);
  RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->init(level_window);

  // center on character
  client_action.command = COMMAND_SET_VIEW;
  client_action.position = userData.entity.position;
  client_action.window = level_window;
  client_engine.action(client_action);

  // start painting...
  client_engine.centerOnActive(true);
  client_engine.start();
  if (!client_engine.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start client engine, aborting\n")));

    // clean up
    level_engine.stop();

    return;
  } // end IF

  // step5g: start timer (triggers hover- and GTK events)
  userData.event_timer = NULL;
  userData.event_timer = SDL_AddTimer(RPG_CLIENT_SDL_EVENT_TIMEOUT, // interval (ms)
                                      event_timer_SDL_cb,           // event timer callback
                                      &userData);                   // callback argument
  if (!userData.event_timer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AddTimer(%u): \"%s\", aborting\n"),
               RPG_CLIENT_SDL_EVENT_TIMEOUT,
               SDL_GetError()));

    // clean up
    level_engine.stop();
    client_engine.stop();

    return;
  } // end IF

  // step7: dispatch SDL (and GTK) events
//   gtk_main();
  SDL_Event sdl_event;
  bool done = false;
  RPG_Graphics_IWindow* window = NULL;
  RPG_Graphics_IWindow* previous_window = NULL;
  bool schedule_redraw = false;
  bool previous_redraw = false;
  RPG_Graphics_Position_t mouse_position;
  bool equipped_lightsource = false;
  do
  {
    sdl_event.type = SDL_NOEVENT;
    window = NULL;
    schedule_redraw = false;
    client_action.command = RPG_CLIENT_COMMAND_INVALID;
    client_action.previous = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                            std::numeric_limits<unsigned int>::max());
    client_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                            std::numeric_limits<unsigned int>::max());
    client_action.window = NULL;
    client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
    client_action.entity_id = 0;
    client_action.path.clear();
    client_action.source = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max());
    client_action.positions.clear();
    previous_redraw = false;
    mouse_position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                    std::numeric_limits<unsigned int>::max());
//     refresh_screen = false;

    // step1: get next pending event
//     if (SDL_PollEvent(&event) == -1)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_PollEvent(): \"%s\", aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
    if (SDL_WaitEvent(&sdl_event) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 SDL_GetError()));

      // clean up
      level_engine.stop();
      client_engine.stop();

      return;
    } // end IF

    // if necessary, reset hover_time
    if ((sdl_event.type != RPG_GRAPHICS_SDL_HOVEREVENT) &&
        (sdl_event.type != RPG_CLIENT_SDL_GTKEVENT) &&
        (sdl_event.type != RPG_CLIENT_SDL_TIMEREVENT))
    {
      // synch access
      ACE_Guard<ACE_Thread_Mutex> aGuard(userData.lock);

      userData.hover_time = 0;
    } // end IF

    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (sdl_event.key.keysym.sym)
        {
          case SDLK_b:
          {
            level_window->toggleVisionBlend();

            schedule_redraw = true;
            client_action.window = level_window;

            break;
          }
          case SDLK_l:
          {
            equipped_lightsource = !equipped_lightsource;

            RPG_Player_Player_Base* player_base = NULL;
            player_base = dynamic_cast<RPG_Player_Player_Base*>(userData.entity.character);
            ACE_ASSERT(player_base);
            if (equipped_lightsource)
            {
              // equip a light and see what happens...
              RPG_Character_EquipmentSlot slot = ((player_base->getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                                                              : EQUIPMENTSLOT_HAND_RIGHT);
              RPG_Item_Base* handle = NULL;
              for (RPG_Item_ListIterator_t iterator = player_base->getInventory().myItems.begin();
                   iterator != player_base->getInventory().myItems.end();
                   iterator++)
              {
                handle = NULL;
                if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                                          handle))
                {
                  ACE_DEBUG((LM_ERROR,
                             ACE_TEXT("invalid item ID: %d, aborting\n"),
                             *iterator));

                  return;
                } // end IF
                ACE_ASSERT(handle);

                if (handle->getType() != ITEM_COMMODITY)
                  continue;

                RPG_Item_Commodity* commodity = dynamic_cast<RPG_Item_Commodity*>(handle);
                ACE_ASSERT(commodity);
                RPG_Item_CommodityUnion commodity_type = commodity->getCommoditySubType();
                if (commodity_type.discriminator != RPG_Item_CommodityUnion::COMMODITYLIGHT)
                  continue;

                // OK: equip this item (into the off-hand)
                player_base->getEquipment().equip(*iterator,
                                                  player_base->getOffHand(),
                                                  slot);

                break;
              } // end FOR
            } // end IF
            else
              player_base->defaultEquip();

            schedule_redraw = true;
            client_action.window = level_window;

            break;
          }
          case SDLK_q:
          {
            // finished event processing
            done = true;

            break;
          }
          default:
            break;
        } // end SWITCH

        break;
      }
      case SDL_ACTIVEEVENT:
      {
        // *NOTE*: when the mouse leaves the window, it's NOT hovering
        // --> stop generating any hover events !
        if (sdl_event.active.state & SDL_APPMOUSEFOCUS)
        {
          if (sdl_event.active.gain & SDL_APPMOUSEFOCUS)
          {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("gained mouse coverage...\n")));

            // synch access
            ACE_Guard<ACE_Thread_Mutex> aGuard(userData.lock);

            userData.do_hover = true;
          } // end IF
          else
          {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

            // synch access
            ACE_Guard<ACE_Thread_Mutex> aGuard(userData.lock);

            userData.do_hover = false;
          } // end ELSE
        } // end IF

        // *WARNING*: falls through !
      }
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
      {
        // find window
        switch (sdl_event.type)
        {
          case SDL_MOUSEMOTION:
            mouse_position = std::make_pair(sdl_event.motion.x,
                                            sdl_event.motion.y); break;
          case SDL_MOUSEBUTTONDOWN:
            mouse_position = std::make_pair(sdl_event.button.x,
                                            sdl_event.button.y); break;
          default:
          {
            mouse_position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();

            break;
          }
        } // end SWITCH
        window = mainWindow.getWindow(mouse_position);
        ACE_ASSERT(window);

        // first steps on mouse motion:
        // 0. restore cursor BG
        // 1. notify previously "active" window upon losing "focus"
        if (sdl_event.type == SDL_MOUSEMOTION)
        {
          // step0: restore cursor BG
          client_action.command = COMMAND_CURSOR_RESTORE_BG;
          client_action.window = (previous_window ? previous_window : window);
          client_engine.action(client_action);

          // step1: notify previous window (if any)
          if (previous_window &&
//               (previous_window != mainWindow)
              (previous_window != window))
          {
            sdl_event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
            previous_redraw = false;
            try
            {
              previous_window->handleEvent(sdl_event,
                                           previous_window,
                                           previous_redraw);
            }
            catch (...)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }
            if (previous_redraw)
            {
              try
              {
                previous_window->draw();
                previous_window->refresh();
              }
              catch (...)
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("caught exception in [%@] RPG_Graphics_IWindow::draw()/refresh(), continuing\n"),
                           previous_window));
              }
            } // end IF

            sdl_event.type = SDL_MOUSEMOTION;
          } // end IF
        } // end IF
        // remember last "active" window
        previous_window = window;

        // notify "active" window
        try
        {
          window->handleEvent(sdl_event,
                              window,
                              schedule_redraw);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
        }
        if (schedule_redraw)
          client_action.window = window;

        break;
      }
      case SDL_QUIT:
      {
        // finished event processing
        done = true;

        break;
      }
      case RPG_CLIENT_SDL_GTKEVENT:
      {
        // (at least one) GTK event has arrived --> process pending events
        GDK_THREADS_ENTER();
        while (gtk_events_pending())
          if (gtk_main_iteration_do(FALSE)) // NEVER block !
          {
            // gtk_main_quit() has been invoked --> finished event processing

            // *NOTE*: as gtk_main() is never invoked, gtk_main_iteration_do ALWAYS
            // returns true... provide a workaround by using the gtk_quit_add hook
            // --> check if that has been called...
            // synch access
            {
              ACE_Guard<ACE_Thread_Mutex> aGuard(userData.lock);

              if (userData.gtk_main_quit_invoked)
              {
                done = true;

                break; // ignore any remaining GTK events
              }
            } // end lock scope
          } // end IF
        GDK_THREADS_LEAVE();

        break;
      }
      case RPG_CLIENT_SDL_TIMEREVENT:
      case SDL_KEYUP:
      case SDL_MOUSEBUTTONUP:
      case SDL_JOYAXISMOTION:
      case SDL_JOYBALLMOTION:
      case SDL_JOYHATMOTION:
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
      case SDL_SYSWMEVENT:
      case SDL_VIDEORESIZE:
      case SDL_VIDEOEXPOSE:
      default:
        break;
    } // end SWITCH

    // redraw map ?
    if (schedule_redraw)
    {
      client_action.command = COMMAND_WINDOW_DRAW;
      client_engine.action(client_action);

      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                                                         std::numeric_limits<unsigned int>::max()));
      // --> store/draw the new tile highlight (BG)
      client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
      client_action.position = RPG_Graphics_Common_Tools::screen2Map(RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position(),
                                                                     level_engine.getSize(),
                                                                     level_window->getSize(),
                                                                     level_window->getView());
      client_engine.action(client_action);

      client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
      client_engine.action(client_action);

      // fiddling with the view (probably) invalidates (part of) the cursor BG
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG(level_window->getScreen());
    } // end IF

    // redraw cursor ?
    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
      {
        // map hasn't changed --> no need to redraw
        if (!schedule_redraw)
          break;

        // *WARNING*: falls through !
      }
      case SDL_MOUSEMOTION:
      case RPG_GRAPHICS_SDL_HOVEREVENT:
      {
        // map has changed, cursor MAY have been drawn over...
        // --> redraw cursor
        client_action.command = COMMAND_CURSOR_DRAW;
        client_action.position = mouse_position;
        client_action.window = &mainWindow;
        client_engine.action(client_action);

        break;
      }
      default:
        break;
    } // end SWITCH

//     // enforce fixed FPS
//     SDL_framerateDelay(&fps_manager);
//     if (refresh_screen)
//     {
//       try
//       {
//         mapWindow.refresh(NULL);
//       }
//       catch (...)
//       {
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("caught exception in RPG_Graphics_IWindow::refresh(), continuing\n")));
//       }
//     } // end IF
  } while (!done);

  // step8: clean up
  level_engine.stop();
  client_engine.stop();

  if (!SDL_RemoveTimer(userData.event_timer))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               SDL_GetError()));

  // done handling UI events

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            << RPG_VERSION
#endif
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  if (version_number << ACE::major_version())
  {
    version_number << ACE_TEXT(".");
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", aborting\n")));

    return;
  } // end ELSE
  if (version_number << ACE::minor_version())
  {
    version_number << ACE_TEXT(".");
    if (version_number << ACE::beta_version())
    {

    } // end IF
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert: \"%m\", aborting\n")));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", aborting\n")));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step1: init ACE
//// *PORTABILITY*: on Windows, we need to init ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (ACE::init() == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));
//
//    return EXIT_FAILURE;
//  } // end IF
//#endif

  std::string base_path;
#ifdef BASEDIR
  base_path = ACE_TEXT_ALWAYS_CHAR(BASEDIR);
#endif
  std::string config_path = RPG_Common_File_Tools::getConfigDataDirectory(base_path,
                                                                          true);
  std::string base_data_path = RPG_Common_File_Tools::getConfigDataDirectory(base_path,
                                                                             false);

  // step1: init
  // step1a set defaults
  std::string itemDictionary = config_path;
  itemDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  itemDictionary += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  std::string monsterDictionary = config_path;
  monsterDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  monsterDictionary += ACE_TEXT_ALWAYS_CHAR("character");
  monsterDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionary += ACE_TEXT_ALWAYS_CHAR("monster");
  monsterDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monsterDictionary += ACE_TEXT_ALWAYS_CHAR(RPG_MONSTER_DEF_DICTIONARY_FILE);

  std::string graphicsDictionary = config_path;
  graphicsDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  graphicsDictionary += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  std::string levelMap = ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY);
#else
  std::string levelMap = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_MAP_DEF_REPOSITORY));
#endif
  levelMap += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  levelMap += ACE_TEXT_ALWAYS_CHAR(MAP_VISION_UI_DEF_FLOOR_PLAN);
  levelMap += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  std::string entityProfile = ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE);
#else
  std::string entityProfile = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE));
#endif
  entityProfile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  entityProfile += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_ENTITY);
  entityProfile += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENTITY_PROFILE_EXT);

  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1ba: parse/process/validate configuration
  if (!(process_arguments(argc_in,
                          argv_in,
                          graphicsDictionary,
                          itemDictionary,
                          levelMap,
                          monsterDictionary,
                          entityProfile,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if (!RPG_Common_File_Tools::isReadable(itemDictionary)     ||
      !RPG_Common_File_Tools::isReadable(monsterDictionary)  ||
      !RPG_Common_File_Tools::isReadable(graphicsDictionary) ||
      !RPG_Common_File_Tools::isReadable(entityProfile)  ||
      !RPG_Common_File_Tools::isReadable(levelMap))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid argument(s), aborting\n")));

    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  std::string schemaRepository = config_path;
#if (defined _DEBUG) || (defined DEBUG_RELEASE)
  schemaRepository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schemaRepository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif
  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schemaRepository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               schemaRepository.c_str()));

    // try fallback
    schemaRepository.clear();
  } // end IF
 
  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_INFO |  // <-- DISABLE trace messages !
                                    //LM_DEBUG |
                                    LM_INFO |
                                    LM_NOTICE |
                                    LM_WARNING |
                                    LM_STARTUP |
                                    LM_ERROR |
                                    LM_CRITICAL |
                                    LM_ALERT |
                                    LM_EMERGENCY);

    // set new mask...
    ACE_LOG_MSG->priority_mask(process_priority_mask,
                               ACE_Log_Msg::PROCESS);

    //ACE_LOG_MSG->stop_tracing();

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF

  // step1d: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step1db: init configuration object
  RPG_Client_Config config;

  // *** reactor ***
  config.num_threadpool_threads            = 0;

  // *** UI ***
//  config.glade_file                        = UIfile;
//  config.gtk_cb_data                       = userData;

  // *** sound ***
//  config.audio_config.frequency            = RPG_CLIENT_DEF_AUDIO_FREQUENCY;
//  config.audio_config.format               = RPG_CLIENT_DEF_AUDIO_FORMAT;
//  config.audio_config.channels             = RPG_CLIENT_DEF_AUDIO_CHANNELS;
//  config.audio_config.samples              = RPG_CLIENT_DEF_AUDIO_SAMPLES;
//  config.sound_directory = base_data_path;
//  config.sound_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//#ifdef DATADIR
//  config.sound_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DATA_SUB);
//  config.sound_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//  config.sound_directory += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DEF_DATA_SUB);
//#else
//  config.sound_directory += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DEF_DATA_SUB);
//  config.sound_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//  config.sound_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DATA_SUB);
//#endif
//  config.sound_cache_size                  = RPG_CLIENT_DEF_SOUND_CACHESIZE;
//  config.sound_dictionary                  = soundDictionary;

  // *** graphics ***
  config.video_config.screen_width         = RPG_CLIENT_DEF_VIDEO_W;
  config.video_config.screen_height        = RPG_CLIENT_DEF_VIDEO_H;
  config.video_config.screen_colordepth    = RPG_CLIENT_DEF_VIDEO_BPP;
  config.video_config.fullScreen           = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  config.video_config.doubleBuffer         = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;
  config.graphics_directory = base_data_path;
  config.graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#ifdef BASEDIR
  config.graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DATA_SUB);
  config.graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  config.graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
#else
  config.graphics_directory += ACE_TEXT_ALWAYS_CHAR("graphics");
  config.graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  config.graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DATA_SUB);
#endif
  config.graphics_dictionary               = graphicsDictionary;

  // *** magic ***
  config.magic_dictionary.clear();

  // *** item ***
  config.item_dictionary                   = itemDictionary;

  // *** monster ***
  config.monster_dictionary                = monsterDictionary;

  // *** map ***
  config.map_config.min_room_size          = RPG_CLIENT_DEF_MAP_MIN_ROOM_SIZE;
  config.map_config.doors                  = RPG_CLIENT_DEF_MAP_DOORS;
  config.map_config.corridors              = RPG_CLIENT_DEF_MAP_CORRIDORS;
  config.map_config.max_num_doors_per_room = RPG_CLIENT_DEF_MAP_MAX_NUM_DOORS_PER_ROOM;
  config.map_config.maximize_rooms         = RPG_CLIENT_DEF_MAP_MAXIMIZE_ROOMS;
  config.map_config.num_areas              = RPG_CLIENT_DEF_MAP_NUM_AREAS;
  config.map_config.square_rooms           = RPG_CLIENT_DEF_MAP_SQUARE_ROOMS;
  config.map_config.map_size_x             = RPG_CLIENT_DEF_MAP_SIZE_X;
  config.map_config.map_size_y             = RPG_CLIENT_DEF_MAP_SIZE_Y;
  config.map_file                          = levelMap;

//   // step1dc: populate config object with default/collected data
//   // ************ connection config data ************
//   config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
//   config.messageAllocator = &messageAllocator;
//   config.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
//   // ************ protocol config data **************
//   config.clientPingInterval = 0; // servers do this...
// //   config.loginOptions = userData.loginOptions;
//   // ************ stream config data ****************
//   config.debugParser = debugParser;
//   config.module = &IRChandlerModule;
//   // *WARNING*: set at runtime, by the appropriate connection handler
//   config.sessionID = 0; // (== socket handle !)
//   config.statisticsReportingInterval = 0; // == off

  //// step1de: parse .ini file (if any)
  //if (!iniFile.empty())
  //  do_parseIniFile(iniFile,
  //                  config);

  // step2a: init SDL
  if (SDL_Init(SDL_INIT_TIMER | // timers
               //SDL_INIT_AUDIO |
               SDL_INIT_VIDEO |
               //SDL_INIT_CDROM | // audioCD playback
               SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    return EXIT_FAILURE;
  } // end IF
  if (TTF_Init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to TTF_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    return EXIT_FAILURE;
  } // end IF

  // step2b: init GLIB / G(D|T)K[+]
  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(&argc_in,
           &argv_in);

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(config,
          schemaRepository,
          levelMap,
          entityProfile);
  timer.stop();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             working_time_string.c_str()));

//// *PORTABILITY*: on Windows, we must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (ACE::fini() == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
//
//    return EXIT_FAILURE;
//  } // end IF
//#endif

  return EXIT_SUCCESS;
} // end main
