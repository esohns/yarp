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
#include "rpg_client_callbacks.h"
#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_engine.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_GTK_manager.h"
#include "rpg_client_logger.h"
#include "rpg_client_ui_tools.h"
#include "rpg_client_window_main.h"
#include "rpg_client_window_level.h"

#include "rpg_sound_defines.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"
#include "rpg_graphics_style.h"

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
#include "rpg_player_common_tools.h"

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

#include "ace/ACE.h"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/High_Res_Timer.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"

#include "SDL.h"

#include <string>
#include <sstream>
#include <iostream>

#define MAP_VISION_UI_DEF_FLOOR_PLAN "test_plan"

Uint32
event_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  RPG_TRACE(ACE_TEXT("::event_timer_SDL_cb"));

  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(argument_in);
  ACE_ASSERT(data);

  SDL_Event sdl_event;
  sdl_event.type = SDL_NOEVENT;

  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(data->lock);

    data->hover_time += interval_in;
    //data->gtk_time += interval_in;
    if (data->do_hover &&
        (data->hover_time > RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY))
    {
      // mouse is hovering --> trigger an event
      sdl_event.type = RPG_GRAPHICS_SDL_HOVEREVENT;
      sdl_event.user.code = static_cast<int>(data->hover_time);

      if (SDL_PushEvent(&sdl_event))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                   ACE_TEXT(SDL_GetError())));
    } // end IF
  } // end lock scope

  // re-schedule
  return interval_in;
}

void
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-d       : debug")
            << ACE_TEXT(" [\"")
            << RPG_CLIENT_DEF_DEBUG
            << ACE_TEXT("\"]")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE]: graphics dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE]: item dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("map");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("data");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  std::cout << ACE_TEXT("-l [FILE]: level map (*")
            << ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT)
            << ACE_TEXT(") [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("monster");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += RPG_MONSTER_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-m [FILE]: monster dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
	path = data_path;
	path += RPG_Player_Common_Tools::getPlayerProfilesDirectory();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);
  std::cout << ACE_TEXT("-p [FILE]: entity profile (*")
            << ACE_TEXT(RPG_PLAYER_PROFILE_EXT)
            << ACE_TEXT(") [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT("-v       : print version information and exit")
            << std::endl;
}

bool
do_processArguments(const int& argc_in,
                    ACE_TCHAR** argv_in, // cannot be const...
                    bool& debug_out,
                    std::string& graphicsDictionary_out,
                    std::string& itemDictionary_out,
                    std::string& levelMap_out,
                    std::string& monsterDictionary_out,
                    std::string& entityProfile_out,
                    bool& traceInformation_out,
                    bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::do_processArguments"));

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  // init results
  debug_out               = RPG_CLIENT_DEF_DEBUG;

  graphicsDictionary_out  = configuration_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary_out +=
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);

  itemDictionary_out      = configuration_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);

  levelMap_out            = data_path;
  levelMap_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  levelMap_out += ACE_TEXT_ALWAYS_CHAR("map");
  levelMap_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  levelMap_out += ACE_TEXT_ALWAYS_CHAR("data");
  levelMap_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  levelMap_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  levelMap_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  levelMap_out +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  levelMap_out += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  monsterDictionary_out   = configuration_path;
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR("character");
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR("monster");
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monsterDictionary_out += RPG_MONSTER_DICTIONARY_FILE;

	entityProfile_out       = data_path;
	entityProfile_out += RPG_Player_Common_Tools::getPlayerProfilesDirectory();
  entityProfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  entityProfile_out += RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_NAME));
  entityProfile_out += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);

  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("dg:i:l:m:p:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        debug_out = true;

        break;
      }
      case 'g':
      {
        graphicsDictionary_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'i':
      {
				itemDictionary_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'l':
      {
				levelMap_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'm':
      {
				monsterDictionary_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'p':
      {
				entityProfile_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

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
                   ACE_TEXT(argumentParser.last_option())));

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
           const RPG_Graphics_SDL_VideoConfiguration_t& videoConfiguration_in)
{
  RPG_TRACE(ACE_TEXT("::do_initGUI"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(graphicsDirectory_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid directory \"%s\", aborting\n"),
               ACE_TEXT(graphicsDirectory_in.c_str())));

    return false;
  } // end IF
  RPG_Graphics_Common_Tools::init(graphicsDirectory_in,
                                  RPG_CLIENT_GRAPHICS_DEF_CACHESIZE,
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
	caption = ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);
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
  RPG_Graphics_t icon_graphic =
      RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT(icon_graphic.type.image == IMAGE_WM_ICON);
  std::string path = graphicsDirectory_in;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_IMAGES_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += icon_graphic.file;
  SDL_Surface* icon_image = NULL;
  icon_image = RPG_Graphics_Surface::load(path,   // graphics file
                                          false); // don't convert to display format (no screen yet !)
  if (!icon_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
               ACE_TEXT(path.c_str())));

    return false;
  } // end IF
  SDL_WM_SetIcon(icon_image, // surface
                 NULL);      // mask (--> everything)
//   // don't show (double) cursor
//   SDL_ShowCursor(SDL_DISABLE);

	userData_in.screen =
      RPG_Graphics_SDL_Tools::initScreen(videoConfiguration_in);
	if (!userData_in.screen)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_SDL_Tools::initScreen(%d,%d,%d), aborting\n"),
							 videoConfiguration_in.screen_width,
							 videoConfiguration_in.screen_height,
							 videoConfiguration_in.screen_colordepth));

    return false;
  } // end IF

  return true;
}

void
do_work(const RPG_Client_Configuration_t& configuration_in,
        const std::string& schemaRepository_in,
        const std::string& levelFilename_in,
        const std::string& playerProfile_in,
        RPG_Client_GTK_CBData_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  std::string empty;
  // step0: init: random seed, string conversion facilities, ...
  RPG_Engine_Common_Tools::init(schemaRepository_in,
                                configuration_in.magic_dictionary,
                                configuration_in.item_dictionary,
                                configuration_in.monster_dictionary);

  // step1: load level map
  RPG_Engine_Level_t level;
  if (!RPG_Engine_Level::load(levelFilename_in,
                              schemaRepository_in,
                              level))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to load level map (was: \"%s\"), aborting\n"),
               ACE_TEXT(levelFilename_in.c_str())));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded level map: \"%s\"\n%s\n"),
             ACE_TEXT(levelFilename_in.c_str()),
             ACE_TEXT(RPG_Map_Level::info(level.map).c_str())));

  // step2: process doors
  RPG_Map_Positions_t door_positions;
  for (RPG_Map_DoorsConstIterator_t iterator = level.map.plan.doors.begin();
       iterator != level.map.plan.doors.end();
       iterator++)
  {
    // *WARNING*: set iterators are CONST for a good reason !
    // --> (but we know what we're doing)...
    const_cast<RPG_Map_Door_t&>(*iterator).outside =
        RPG_Map_Common_Tools::door2exitDirection((*iterator).position,
                                                 level.map.plan);

    door_positions.insert((*iterator).position);
  } // end FOR

  // step2: init client / engine / gtk cb user data
  RPG_Client_Engine client_engine;
  RPG_Engine        level_engine;
//   user_data.lock;
  userData_in.do_hover              = true;
  userData_in.hover_time            = 0;
  userData_in.XML                   = NULL;
  userData_in.entity_filter         = NULL;
  userData_in.map_filter            = NULL;
  userData_in.screen                = NULL;
  userData_in.event_timer           = NULL;
  userData_in.client_engine         = &client_engine;
  userData_in.schema_repository     = schemaRepository_in;

  GDK_THREADS_ENTER();
  if (!do_initGUI(configuration_in.graphics_dictionary,
                  configuration_in.graphics_directory,
                  userData_in,                           // GTK cb data
                  configuration_in.video_configuration)) // SDL_video configuration
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize video, aborting\n")));

    GDK_THREADS_LEAVE();

    return;
  } // end IF
  GDK_THREADS_LEAVE();
  ACE_ASSERT(userData_in.screen);
  //ACE_ASSERT(user_data.XML);

  RPG_Client_Common_Tools::init(configuration_in.input_configuration,
                                configuration_in.audio_configuration.SDL_configuration,
                                configuration_in.audio_configuration.repository,
                                configuration_in.audio_configuration.use_CD,
                                RPG_SOUND_DEF_CACHESIZE,
                                configuration_in.audio_configuration.mute,
                                configuration_in.audio_configuration.dictionary,
                                configuration_in.graphics_directory,
                                RPG_CLIENT_GRAPHICS_DEF_CACHESIZE,
                                configuration_in.graphics_dictionary,
                                true);

  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);
  short int HP = std::numeric_limits<short int>::max();
  RPG_Magic_Spells_t spells;
  userData_in.entity.character = RPG_Player::load(playerProfile_in,
                                                  schemaRepository_in,
                                                  condition,
                                                  HP,
                                                  spells);
  if (!userData_in.entity.character)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Player::load(\"%s\"), aborting\n"),
               ACE_TEXT(playerProfile_in.c_str())));

    return;
  } // end IF
  userData_in.entity.position = level.map.start;
//   userData_in.entity.actions();
  //userData_in.entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  //userData_in.entity.graphic = NULL;
  userData_in.level_engine = &level_engine;
  userData_in.map_configuration = configuration_in.map_configuration;

  // ***** mouse setup *****
  SDL_WarpMouse((userData_in.screen->w / 2),
                (userData_in.screen->h / 2));

//   // step3: run intro
//   if (!do_runIntro())
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to run intro, aborting\n")));
//
//     return;
//   } // end IF

//  // step5b: setup style
//  RPG_Graphics_Style map_style;
//  map_style.floor = RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE;
//  map_style.edge = RPG_CLIENT_DEF_GRAPHICS_EDGESTYLE;
//  map_style.wall = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE;
//  map_style.half_height_walls = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF;
//  map_style.door = RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE;

  // step5c: level engine
  level_engine.init(&client_engine);
  level_engine.set(level);
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
	type.image = RPG_CLIENT_GRAPHICS_DEF_WINDOWSTYLE_TYPE;
  std::string title =
		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);
  RPG_Client_Window_Main main_window(RPG_Graphics_Size_t(userData_in.screen->w,
                                                         userData_in.screen->h), // size
                                     type,                                       // interface elements
                                     title,                                      // title (== caption)
                                     FONT_MAIN_LARGE);                           // title font
  main_window.setScreen(userData_in.screen);
  main_window.init(&client_engine,
		               RPG_CLIENT_WINDOW_DEF_EDGE_AUTOSCROLL,
                   &level_engine,
									 false);

  // step5e: client engine
  RPG_Client_GTKUIDefinition ui_definition(&userData_in);
  client_engine.init(&level_engine,
                     main_window.child(WINDOW_MAP),
                     &ui_definition,
										 false);

  // step5f: trigger initial drawing
  RPG_Client_Action client_action;
  client_action.command = COMMAND_WINDOW_DRAW;
  client_action.position = std::make_pair(0, 0);
  client_action.window = &main_window;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_engine.action(client_action);
  client_action.command = COMMAND_WINDOW_REFRESH;
  client_engine.action(client_action);

  RPG_Client_Window_Level* level_window =
      dynamic_cast<RPG_Client_Window_Level*>(main_window.child(WINDOW_MAP));
  ACE_ASSERT(level_window);

  // activate the current character
  RPG_Engine_EntityID_t id = level_engine.add(&(userData_in.entity));
  level_engine.setActive(id);
  // init/add entity to the graphics cache
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->init(&client_engine,
                                                          level_window);
  RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->init(&client_engine,
                                                        level_window);

  // center on character
  client_action.command = COMMAND_SET_VIEW;
  client_action.position = userData_in.entity.position;
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
  userData_in.event_timer = NULL;
  userData_in.event_timer = SDL_AddTimer(RPG_CLIENT_SDL_EVENT_TIMEOUT, // interval (ms)
                                         event_timer_SDL_cb,            // event timer callback
                                         &userData_in);                 // callback argument
  if (!userData_in.event_timer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AddTimer(%u): \"%s\", aborting\n"),
               RPG_CLIENT_SDL_EVENT_TIMEOUT,
               ACE_TEXT(SDL_GetError())));

    // clean up
    level_engine.stop();
    client_engine.stop();

    return;
  } // end IF

  // step7: dispatch SDL (and GTK) events
//   gtk_main();
  SDL_Event sdl_event;
  bool done = false;
  RPG_Graphics_IWindowBase* window = NULL;
  RPG_Graphics_IWindowBase* previous_window = NULL;
//  bool schedule_redraw = false;
//  bool previous_redraw = false;
  RPG_Graphics_Position_t mouse_position;
  bool equipped_lightsource = false;
  SDL_Rect dirty_region;
  do
  {
    sdl_event.type = SDL_NOEVENT;
    window = NULL;
//    schedule_redraw = false;
    client_action.command = RPG_CLIENT_COMMAND_INVALID;
    client_action.previous =
        std::make_pair(std::numeric_limits<unsigned int>::max(),
                       std::numeric_limits<unsigned int>::max());
    client_action.position =
        std::make_pair(std::numeric_limits<unsigned int>::max(),
                       std::numeric_limits<unsigned int>::max());
    client_action.window = NULL;
    client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
    client_action.entity_id = 0;
    client_action.path.clear();
    client_action.source =
        std::make_pair(std::numeric_limits<unsigned int>::max(),
                       std::numeric_limits<unsigned int>::max());
    client_action.positions.clear();
//    previous_redraw = false;
    mouse_position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                    std::numeric_limits<unsigned int>::max());
    ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));

    // step1: get next pending event
//     if (SDL_PollEvent(&event) == -1)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_PollEvent(): \"%s\", aborting\n"),
//                  ACE_TEXT(SDL_GetError())));
//
//       return;
//     } // end IF
    if (SDL_WaitEvent(&sdl_event) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 ACE_TEXT(SDL_GetError())));

      // clean up
      level_engine.stop();
      client_engine.stop();

      return;
    } // end IF

    // if necessary, reset hover_time
    if ((sdl_event.type != RPG_GRAPHICS_SDL_HOVEREVENT) &&
        (sdl_event.type != RPG_CLIENT_SDL_TIMEREVENT))
    {
      // synch access
      ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(userData_in.lock);

      userData_in.hover_time = 0;
    } // end IF

    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (sdl_event.key.keysym.sym)
        {
//          case SDLK_b:
//          {
//            level_window->toggleVisionBlend();

//            schedule_redraw = true;
//            client_action.window = level_window;

//            break;
//          }
          case SDLK_l:
          {
            equipped_lightsource = !equipped_lightsource;

            RPG_Player_Player_Base* player_base = NULL;
            player_base =
                dynamic_cast<RPG_Player_Player_Base*>(userData_in.entity.character);
            ACE_ASSERT(player_base);
            if (equipped_lightsource)
            {
              // equip a light and see what happens...
              RPG_Character_EquipmentSlot slot =
                  ((player_base->getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
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

                RPG_Item_Commodity* commodity =
                    dynamic_cast<RPG_Item_Commodity*>(handle);
                ACE_ASSERT(commodity);
                RPG_Item_CommodityUnion commodity_type =
                    commodity->getCommoditySubType();
                if (commodity_type.discriminator !=
                    RPG_Item_CommodityUnion::COMMODITYLIGHT)
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

//            schedule_redraw = true;
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
            ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(userData_in.lock);

            userData_in.do_hover = true;
          } // end IF
          else
          {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

            // synch access
            ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(userData_in.lock);

            userData_in.do_hover = false;
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
            mouse_position =
                RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();

            break;
          }
        } // end SWITCH
        window = main_window.getWindow(mouse_position);
        ACE_ASSERT(window);

        // first steps on mouse motion:
        // 0. restore cursor BG
        // 1. notify previously "active" window upon losing "focus"
        if (sdl_event.type == SDL_MOUSEMOTION)
        {
          // step1: notify previous window (if any)
          if (previous_window &&
//               (previous_window != mainWindow)
              (previous_window != window))
          {
            sdl_event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
//            previous_redraw = false;
            try
            {
              previous_window->handleEvent(sdl_event,
                                           previous_window,
                                           dirty_region);
            }
            catch (...)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }

            sdl_event.type = SDL_MOUSEMOTION;
          } // end IF
        } // end IF
        // remember last "active" window
        previous_window = window;

        // notify "active" window
        SDL_Rect dirty_region_2;
        ACE_OS::memset(&dirty_region_2, 0, sizeof(dirty_region_2));
        try
        {
          window->handleEvent(sdl_event,
                              window,
                              dirty_region_2);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
        }
        if (dirty_region_2.w || dirty_region_2.h)
        {
          dirty_region = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                             dirty_region_2);
          client_action.window = window;
        } // end IF

        break;
      }
      case SDL_QUIT:
      {
        // finished event processing
        done = true;

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
    if (dirty_region.w || dirty_region.h)
    {
      client_action.command = COMMAND_WINDOW_DRAW;
      client_engine.action(client_action);

      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                                                         std::numeric_limits<unsigned int>::max()));
//      // --> store/draw the new tile highlight (BG)
//      client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
//      client_action.position =
//          RPG_Graphics_Common_Tools::screen2Map(RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position(),
//                                                level_engine.getSize(),
//                                                level_window->getSize(),
//                                                level_window->getView());
//      client_engine.action(client_action);

      client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
      client_engine.action(client_action);

//      // fiddling with the view (probably) invalidates (part of) the cursor BG
//      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG(NULL);
    } // end IF

    // redraw cursor ?
    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
      {
        // map hasn't changed --> no need to redraw
        if ((dirty_region.w == 0) && (dirty_region.h == 0))
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
        client_action.window = &main_window;
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
//         level_window.refresh(NULL);
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

  if (!SDL_RemoveTimer(userData_in.event_timer))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               ACE_TEXT(SDL_GetError())));

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
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... We need this, as the library soname is compared to this
  // string.
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
          ACE_TCHAR** argv_in)
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step1: init ACE
// *PORTABILITY*: on Windows, need to init ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
  data_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  // step1: init
  // step1a set defaults
  bool debug                      = RPG_CLIENT_DEF_DEBUG;

  std::string item_dictionary     = configuration_path;
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  item_dictionary += ACE_TEXT_ALWAYS_CHAR("item");
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);

  std::string monster_dictionary  = configuration_path;
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR("character");
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR("monster");
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_MONSTER_DICTIONARY_FILE);

  std::string graphics_dictionary = configuration_path;
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);

  std::string level_map           = data_path;
  level_map += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  level_map += ACE_TEXT_ALWAYS_CHAR("map");
  level_map += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  level_map += ACE_TEXT_ALWAYS_CHAR("data");
  level_map += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  level_map += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  level_map += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  level_map +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  level_map += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

	std::string entity_profile      = data_path;
	entity_profile += RPG_Player_Common_Tools::getPlayerProfilesDirectory();
	entity_profile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
	entity_profile += RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_DEF_NAME));
	entity_profile += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);

  std::string UI_file             = configuration_path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  UI_file += ACE_TEXT_ALWAYS_CHAR("client");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UI_file += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_UI_FILE);

  bool trace_information          = false;
  bool print_version_and_exit     = false;

  // step1ba: parse/process/validate configuration
  if (!do_processArguments(argc_in,
                           argv_in,
                           debug,
                           graphics_dictionary,
                           item_dictionary,
                           level_map,
                           monster_dictionary,
                           entity_profile,
                           trace_information,
                           print_version_and_exit))
  {
    // make 'em learn...
    do_printUsage(std::string(ACE::basename(argv_in[0])));

    // clean up
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      if (ACE::fini() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1bb: validate arguments
  if (!RPG_Common_File_Tools::isReadable(item_dictionary)     ||
      !RPG_Common_File_Tools::isReadable(monster_dictionary)  ||
      !RPG_Common_File_Tools::isReadable(graphics_dictionary) ||
      !RPG_Common_File_Tools::isReadable(entity_profile)      ||
      !RPG_Common_File_Tools::isReadable(level_map)           ||
      !RPG_Common_File_Tools::isReadable(UI_file))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid argument(s), aborting\n")));

    // make 'em learn...
    do_printUsage(std::string(ACE::basename(argv_in[0])));

    // clean up
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      if (ACE::fini() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  std::string schema_repository = configuration_path;
#if defined(DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif
  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schema_repository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               ACE_TEXT(schema_repository.c_str())));

    // try fallback
    schema_repository.clear();
  } // end IF
 
  // step1c: initialize logging and/or tracing
  RPG_Client_GTK_CBData_t user_data;
  RPG_Client_Logger logger(&user_data.log_stack,
                           &user_data.lock);
  std::string log_file;
  if (!RPG_Common_Tools::initLogging(ACE::basename(argv_in[0]), // program name
                                     log_file,                  // logfile
                                     false,                     // log to syslog ?
                                     false,                     // trace messages ?
                                     trace_information,         // debug messages ?
                                     &logger))                  // logger
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initLogging(), aborting\n")));

    // clean up
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      if (ACE::fini() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    // clean up
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      if (ACE::fini() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step1db: init configuration object
  RPG_Client_Configuration_t configuration;

  // *** reactor ***
  configuration.num_dispatch_threads                = 0;

  // *** graphics ***
	configuration.video_configuration.screen_width = RPG_CLIENT_VIDEO_DEF_WIDTH;
	configuration.video_configuration.screen_height = RPG_CLIENT_VIDEO_DEF_HEIGHT;
	configuration.video_configuration.screen_colordepth = RPG_CLIENT_VIDEO_DEF_BPP;
	configuration.video_configuration.full_screen = RPG_CLIENT_VIDEO_DEF_FULLSCREEN;
	configuration.video_configuration.double_buffer = RPG_CLIENT_VIDEO_DEF_DOUBLEBUFFER;
	configuration.video_configuration.use_OpenGL = RPG_CLIENT_VIDEO_DEF_OPENGL;
	//configuration.video_configuration.video_driver = ;

  configuration.graphics_directory = data_path;
  configuration.graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#ifdef BASEDIR
  configuration.graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
  configuration.graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  configuration.graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#else
  configuration.graphics_directory += ACE_TEXT_ALWAYS_CHAR("graphics");
  configuration.graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  configuration.graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#endif
  configuration.graphics_dictionary += graphics_dictionary;

  // *** magic ***
  configuration.magic_dictionary.clear();

  // *** item ***
  configuration.item_dictionary            = item_dictionary;

  // *** monster ***
  configuration.monster_dictionary         = monster_dictionary;

  // *** map ***
	configuration.map_configuration.min_room_size = RPG_CLIENT_MAP_DEF_MIN_ROOM_SIZE;
	configuration.map_configuration.doors = RPG_CLIENT_MAP_DEF_DOORS;
	configuration.map_configuration.corridors = RPG_CLIENT_MAP_DEF_CORRIDORS;
	configuration.map_configuration.max_num_doors_per_room = RPG_CLIENT_MAP_DEF_MAX_NUM_DOORS_PER_ROOM;
	configuration.map_configuration.maximize_rooms = RPG_CLIENT_MAP_DEF_MAXIMIZE_ROOMS;
	configuration.map_configuration.num_areas = RPG_CLIENT_MAP_DEF_NUM_AREAS;
	configuration.map_configuration.square_rooms = RPG_CLIENT_MAP_DEF_SQUARE_ROOMS;
	configuration.map_configuration.map_size_x = RPG_CLIENT_MAP_DEF_SIZE_X;
	configuration.map_configuration.map_size_y = RPG_CLIENT_MAP_DEF_SIZE_Y;
  configuration.map_file                                 = level_map;

  // step2a: init SDL
  if (SDL_Init(SDL_INIT_TIMER | // timers
               //SDL_INIT_AUDIO |
               SDL_INIT_VIDEO |
               //SDL_INIT_CDROM | // audioCD playback
               SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(): \"%s\", aborting\n"),
               ACE_TEXT(SDL_GetError())));

    // clean up
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      if (ACE::fini() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (TTF_Init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to TTF_Init(): \"%s\", aborting\n"),
               ACE_TEXT(SDL_GetError())));

    // clean up
    SDL_VideoQuit();
    SDL_Quit();
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      if (ACE::fini() == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

	// step2b: init GLIB / G(D|T)K[+] / GNOME
	RPG_Client_GTKUIDefinition GTK_initializer(&user_data);
	RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->init(argc_in,
																										 argv_in,
																										 UI_file,
																										 &GTK_initializer);

  ACE_High_Res_Timer timer;
  timer.start();

  // step3: do actual work
  do_work(configuration,
          schema_repository,
          level_map,
          entity_profile,
          user_data);
  timer.stop();

  // step4: clean up
  TTF_Quit();
  SDL_VideoQuit();
  SDL_Quit();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             ACE_TEXT(working_time_string.c_str())));

// *PORTABILITY*: on Windows, must fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
