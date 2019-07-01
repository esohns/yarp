/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns                                      *
 *   erik.sohns@web.de                                                     *
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

#include <iostream>
#include <sstream>
#include <string>

#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"
#include "ace/Get_Opt.h"
#include "ace/Global_Macros.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Thread_Manager.h"
#include "ace/Version.h"

#include "SDL.h"
#include "SDL_mixer.h"
//#include "SDL/SDL_framerate.h"

#include "common_file_tools.h"

//#include "common_logger.h"
#include "common_log_tools.h"

#include "common_timer_tools.h"

#include "common_ui_glade_definition.h"
#include "common_ui_gtk_manager.h"

#include "stream_allocatorheap.h"

#include "net_common_tools.h"
#include "net_connection_manager_common.h"

#include "net_client_defines.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_magic_defines.h"

#include "rpg_item_defines.h"

#include "rpg_character_defines.h"

#include "rpg_player.h"
#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_monster_defines.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_defines.h"
#include "rpg_map_level.h"

#include "rpg_net_common.h"
#include "rpg_net_defines.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_network.h"

#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#include "rpg_sound_common.h"
#include "rpg_sound_common_tools.h"
#include "rpg_sound_defines.h"
#include "rpg_sound_dictionary.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_callbacks.h"
#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
#include "rpg_client_engine.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_ui_tools.h"
#include "rpg_client_window_level.h"
#include "rpg_client_window_main.h"

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
    ACE_Guard<ACE_Thread_Mutex> aGuard (data->GTKState.lock);

    data->hoverTime += interval_in;
    //data->gtk_time += interval_in;
    if (data->doHover &&
        (data->hoverTime > RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY))
    {
      // mouse is hovering --> trigger an event
      sdl_event.type = RPG_GRAPHICS_SDL_HOVEREVENT;
      sdl_event.user.code = static_cast<int>(data->hoverTime);

      if (SDL_PushEvent(&sdl_event))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                   ACE_TEXT(SDL_GetError())));
    } // end IF
  } // end lock scope

  // re-schedule
  return interval_in;
}

Uint32
input_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  RPG_TRACE(ACE_TEXT("::input_timer_SDL_cb"));

  // create a timer event
  SDL_Event sdl_event;
  sdl_event.type = RPG_CLIENT_SDL_TIMEREVENT;
  sdl_event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent(&sdl_event))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               ACE_TEXT(SDL_GetError())));

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput(const unsigned int& timeout_in,
                    SDL_Event& event_out)
{
  RPG_TRACE(ACE_TEXT("::do_SDL_waitForInput"));

  SDL_TimerID timer = NULL;
  if (timeout_in)
    timer = SDL_AddTimer((timeout_in * 1000), // interval (ms)
                         input_timer_SDL_cb,  // timeout callback
                         NULL);               // callback argument
  ACE_ASSERT(timer);
  if (!timer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AddTimer(%u): \"%s\", aborting\n"),
               (timeout_in * 1000),
               ACE_TEXT(SDL_GetError())));

    return;
  } // end IF

  // loop until something interesting happens
  do
  {
    if (SDL_WaitEvent(&event_out) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 ACE_TEXT(SDL_GetError())));

      // what else can we do ?
      break;
    } // end IF
    if ((event_out.type == SDL_KEYDOWN)              ||
        (event_out.type == SDL_KEYUP)                ||
        (event_out.type == SDL_MOUSEMOTION)          ||
        (event_out.type == SDL_MOUSEBUTTONDOWN)      ||
        (event_out.type == SDL_QUIT)                 ||
        (event_out.type == RPG_CLIENT_SDL_TIMEREVENT))
      break;
  } while (true);

  // clean up
  if (timeout_in &&
      (event_out.type != RPG_CLIENT_SDL_TIMEREVENT))
    if (!SDL_RemoveTimer(timer))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               ACE_TEXT(SDL_GetError())));
  } // end IF
}

void
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-a         : no audio")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("client");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_CONFIGURATION_FILE);
  std::cout << ACE_TEXT("-c [FILE]  : configuration file")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-d         : debug mode")
            << ACE_TEXT(" [")
            << RPG_CLIENT_DEF_DEBUG
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("monster");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MONSTER_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-e [FILE]  : monster dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("engine");
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
  std::cout << ACE_TEXT("-f [FILE]  : level plan (*")
            << ACE_TEXT(RPG_ENGINE_LEVEL_FILE_EXT)
            << ACE_TEXT(") [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE]  : graphics dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE]  : item dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-l         : log to a file")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("magic");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-m [FILE]  : magic dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
#if defined (DEBUG_DEBUGGER)
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("engine");
#endif
  std::cout << ACE_TEXT("-n         : skip intro")
            << ACE_TEXT(" [\"")
            << false
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-r [DIR]   : schema repository")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("sound");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-s [FILE]  : sound dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-t         : trace information")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("client");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_UI_FILE);
  std::cout << ACE_TEXT("-u [FILE]  : UI file")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-x [VALUE] : #dispatch threads [")
            << NET_CLIENT_DEF_NUM_DISPATCH_THREADS
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-z [STRING]: SDL video driver [\"")
            << ACE_TEXT(RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME)
            << ACE_TEXT("\"]")
            << std::endl;
}

bool
do_processArguments(const int& argc_in,
                    ACE_TCHAR** argv_in, // cannot be const...
                    bool& mute_out,
                    std::string& configurationFile_out,
                    bool& debug_out,
                    std::string& monsterDictionary_out,
                    std::string& floorPlan_out,
                    std::string& graphicsDictionary_out,
                    std::string& itemDictionary_out,
                    bool& logToFile_out,
                    std::string& magicDictionary_out,
                    bool& skipIntro_out,
                    std::string& schemaRepository_out,
                    std::string& soundDictionary_out,
                    bool& traceInformation_out,
                    std::string& UIfile_out,
                    bool& printVersionAndExit_out,
                    unsigned int& numDispatchThreads_out,
                    std::string& videoDriver_out)
{
  RPG_TRACE(ACE_TEXT("::do_processArguments"));

  // init results
  mute_out                = false;

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  configurationFile_out   = configuration_path;
  configurationFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  configurationFile_out += ACE_TEXT_ALWAYS_CHAR("client");
  configurationFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  configurationFile_out += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_CONFIGURATION_FILE);

  debug_out               = RPG_CLIENT_DEF_DEBUG;

  monsterDictionary_out   = configuration_path;
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR("character");
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR("monster");
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monsterDictionary_out +=
      ACE_TEXT_ALWAYS_CHAR(RPG_MONSTER_DICTIONARY_FILE);

  floorPlan_out           = data_path;
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR("engine");
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR("data");
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  floorPlan_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  floorPlan_out +=
      RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  floorPlan_out += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  graphicsDictionary_out  = configuration_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary_out +=
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);

  itemDictionary_out      = configuration_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);

  logToFile_out           = false;

  magicDictionary_out     = configuration_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR("magic");
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);

  skipIntro_out           = false;

  schemaRepository_out    = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schemaRepository_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schemaRepository_out += ACE_TEXT_ALWAYS_CHAR("engine");
#endif

  soundDictionary_out     = configuration_path;
  soundDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  soundDictionary_out += ACE_TEXT_ALWAYS_CHAR("sound");
  soundDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  soundDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DICTIONARY_FILE);

  traceInformation_out    = false;

  UIfile_out              = configuration_path;
  UIfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  UIfile_out += ACE_TEXT_ALWAYS_CHAR("client");
  UIfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UIfile_out += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_UI_FILE);

  printVersionAndExit_out = false;

  numDispatchThreads_out  = NET_CLIENT_DEF_NUM_DISPATCH_THREADS;

  videoDriver_out         =
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME);

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("ac:de:f:g:i:lm:nr:s:tu:vx:z:"),
                             1,                         // skip command name
                             1,                         // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS, // ordering
                             0);                        // don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'a':
      {
        mute_out = true;
        break;
      }
      case 'c':
      {
        configurationFile_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      case 'd':
      {
        debug_out = true;
        break;
      }
      case 'e':
      {
        monsterDictionary_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      case 'f':
      {
        floorPlan_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
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
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        magicDictionary_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      case 'n':
      {
        skipIntro_out = true;
        break;
      }
      case 'r':
      {
        schemaRepository_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      case 's':
      {
        soundDictionary_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'u':
      {
        UIfile_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
      case 'x':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        converter >> numDispatchThreads_out;
        break;
      }
      case 'z':
      {
        videoDriver_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("option \"%c\" requires an argument, aborting\n"),
                   argumentParser.opt_opt()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.last_option())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("found long option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.long_option())));
        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

bool
do_runIntro(SDL_Surface*      targetSurface_in,
            RPG_Common_ILock* screenLock_in)
{
  RPG_TRACE(ACE_TEXT("::do_runIntro"));

  ACE_ASSERT(targetSurface_in);

  // step1: play intro music
  ACE_Time_Value length;
  RPG_Sound_Common_Tools::play(EVENT_MAIN_TITLE,
                               length);

  // step2: show start logo
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_INTRO_MAIN;
  SDL_Surface* logo = RPG_Graphics_Common_Tools::loadGraphic(type,
                                                             true,   // convert to display format
                                                             false); // don't cache
  if (!logo)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               ACE_TEXT(RPG_Graphics_Common_Tools::typeToString(type).c_str())));

    return false;
  } // end IF
  // *TODO* stretch this image fullscreen
  SDL_Rect dirty_region;
  // center logo image
  if (screenLock_in)
    screenLock_in->lock();
  RPG_Graphics_Surface::put(std::make_pair(((targetSurface_in->w - logo->w) / 2),  // location x
                                           ((targetSurface_in->h - logo->h) / 2)), // location y
                            *logo,
                            targetSurface_in,
                            dirty_region);
  if (screenLock_in)
    screenLock_in->unlock();

//   SDL_FreeSurface(logo);
  RPG_Graphics_Common_Tools::fade(true,                                                // fade in
                                  5.0,                                                 // interval
                                  RPG_Graphics_SDL_Tools::getColor(COLOR_BLACK,
                                                                   *targetSurface_in), // fade from black
                                  screenLock_in,                                       // screen lock interface handle
                                  targetSurface_in);                                   // target surface (e.g. screen)
  SDL_Event event;
  do_SDL_waitForInput(10,     // wait 10 seconds max
                      event);
//   do_handleSDLEvent(event);
  RPG_Graphics_Common_Tools::fade(false,                                               // fade out
                                  3.0,                                                 // interval
                                  RPG_Graphics_SDL_Tools::getColor(COLOR_BLACK,
                                                                   *targetSurface_in), // fade to black
                                  screenLock_in,                                       // screen lock interface handle
                                  targetSurface_in);                                   // target surface (e.g. screen)

  // clean up
  SDL_FreeSurface(logo);

  return true;
}

void
do_work (const RPG_Client_Configuration_t& configuration_in,
         const std::string& schemaRepository_in,
         Common_UI_GladeDefinition& UIDefinition_in,
         RPG_Client_GTK_CBData_t& GTKUserData_in,
         const std::string& UIDefinitionFile_in,
         bool skipIntro_in,
         bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: init RPG engine
  RPG_Engine_Common_Tools::init (schemaRepository_in,
                                 configuration_in.magic_dictionary,
                                 configuration_in.item_dictionary,
                                 configuration_in.monster_dictionary);
  if (!RPG_Client_Common_Tools::initialize (configuration_in.input_configuration,
                                            configuration_in.audio_configuration.SDL_configuration,
                                            configuration_in.audio_configuration.repository,
                                            configuration_in.audio_configuration.use_CD,
                                            RPG_SOUND_DEF_CACHESIZE,
                                            configuration_in.audio_configuration.mute,
                                            configuration_in.audio_configuration.dictionary,
                                            configuration_in.graphics_directory,
                                            RPG_GRAPHICS_DEF_CACHESIZE,
                                            configuration_in.graphics_dictionary,
                                            false)) // init SDL ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Client_Common_Tools::init(), returning\n")));

    // clean up
    RPG_Engine_Common_Tools::fini ();

    return;
  } // end IF

  // step2: init UI
  RPG_Client_Engine client_engine;
  RPG_Engine level_engine;
  level_engine.initialize (&client_engine);
  GTKUserData_in.clientEngine     = &client_engine;
  GTKUserData_in.schemaRepository = schemaRepository_in;
  GTKUserData_in.entity.position  =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
//   GTKUserData_in.entity.actions();
//   GTKUserData_in.entity.modes();
//  GTKUserData_in.entity.sprite();
  GTKUserData_in.entity.is_spawned = false;
  GTKUserData_in.levelEngine      = &level_engine;

  // ***** window setup *****
  std::string caption =
      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);
//   caption += ACE_TEXT_ALWAYS_CHAR(" ");
//   caption += RPG_VERSION;
  // ***** window/screen setup *****
  if (!RPG_Graphics_SDL_Tools::initVideo (configuration_in.video_configuration, // configuration
                                          caption,                              // window/icon caption
                                          GTKUserData_in.screen,                // window surface
                                          true))                                // init window ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::initVideo(), returning\n")));

    // clean up
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini ();

    return;
  } // end IF
  ACE_ASSERT (GTKUserData_in.screen != NULL);
  if (!RPG_Graphics_Common_Tools::initialize (configuration_in.graphics_directory,
                                              RPG_CLIENT_GRAPHICS_DEF_CACHESIZE,
                                              true))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initialize(): \"%m\", returning\n")));

    // clean up
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini ();

    return;
  } // end IF

  SDL_Rect dirty_region;
  ACE_OS::memset (&dirty_region, 0, sizeof (dirty_region));
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->setCursor (CURSOR_NORMAL,
                                                                 dirty_region);

  // ***** mouse setup *****
  SDL_WarpMouse ((GTKUserData_in.screen->w / 2),
                 (GTKUserData_in.screen->h / 2));

  // step3: run intro ?
  if (!skipIntro_in &&
      !do_runIntro (GTKUserData_in.screen,
                    &client_engine))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to run intro, returning\n")));
    return;
  } // end IF

  // step4a: setup main "window"
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = RPG_CLIENT_GRAPHICS_DEF_WINDOWSTYLE_TYPE;
  std::string title =
    ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);
  RPG_Client_Window_Main main_window (RPG_Graphics_Size_t (GTKUserData_in.screen->w,
                                                           GTKUserData_in.screen->h), // size
                                      type,                                           // interface elements
                                      title,                                          // title (== caption)
                                      FONT_MAIN_LARGE);                               // title font
  main_window.setScreen (GTKUserData_in.screen);
  if (!main_window.initialize (&client_engine,
                               RPG_CLIENT_WINDOW_DEF_EDGE_AUTOSCROLL,
                               &level_engine,
                               debug_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Client_Window_Main::initialize (), returning\n")));
    return;
  } // end IF

  // step4b: client engine
  client_engine.initialize (&level_engine,
                            main_window.child (WINDOW_MAP),
                            &UIDefinition_in,
                            debug_in);

  // step4c: queue initial drawing
  RPG_Client_Action client_action;
  client_action.command = COMMAND_WINDOW_DRAW;
  client_action.previous =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  client_action.position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  client_action.window = &main_window;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_action.entity_id = 0;
  client_action.sound = RPG_SOUND_EVENT_INVALID;
  client_action.source =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  client_action.radius = 0;
  client_engine.action (client_action);
  client_action.command = COMMAND_WINDOW_REFRESH;
  client_engine.action (client_action);

  RPG_Graphics_IWindowBase* level_window = main_window.child (WINDOW_MAP);
  ACE_ASSERT (level_window);
  // init/add entity to the graphics cache
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->init (&client_engine,
                                                            level_window);
  RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->init (&client_engine,
                                                          level_window);

  // start painting...
  client_engine.start ();
  if (!client_engine.isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start client engine, returning\n")));

    // clean up
    level_engine.stop ();
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini ();

    return;
  } // end IF

  // step4d: start timer (triggers hover events)
  GTKUserData_in.eventTimer = NULL;
  GTKUserData_in.eventTimer =
    SDL_AddTimer (RPG_CLIENT_SDL_EVENT_TIMEOUT, // interval (ms)
                  event_timer_SDL_cb,           // event timer callback
                  &GTKUserData_in);             // callback argument
  if (!GTKUserData_in.eventTimer)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_AddTimer(%u): \"%s\", returning\n"),
                RPG_CLIENT_SDL_EVENT_TIMEOUT,
                ACE_TEXT (SDL_GetError ())));

    // clean up
//    level_engine.stop();
    client_engine.stop ();
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini ();

    return;
  } // end IF

  // step5: init networking
  // step5a: init stream configuration object
  Stream_AllocatorHeap heap_allocator;
  RPG_Net_StreamMessageAllocator_t message_allocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                      &heap_allocator);
  RPG_Net_Protocol_Configuration protocol_configuration;
  ACE_OS::memset (&protocol_configuration,
                  0,
                  sizeof (RPG_Net_Protocol_Configuration));
  // ************ connection configuration ************
  protocol_configuration.socketConfiguration.bufferSize =
    RPG_NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;
//  net_configuration.useThreadPerConnection = false;
//  net_configuration.serializeOutput = false;
  // ************ stream configuration ************
  protocol_configuration.streamConfiguration.messageAllocator =
    &message_allocator;
  //protocol_configuration.streamConfiguration.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
//  net_configuration.notificationStrategy = NULL;
//  net_configuration.module = NULL; // just use the default stream...
  // *WARNING*: set at runtime, by the appropriate connection handler
//  net_configuration.sessionID = 0; // (== socket handle !)
//  net_configuration.statisticsReportingInterval = 0; // == off
  // ************ stream config data ************
  //protocol_configuration.protocolConfiguration.peerPingInterval = 0; // *NOTE*: don't ping the server...
  //protocol_configuration.protocolConfiguration.pingAutoAnswer = true;
  //  net_configuration.printPongMessages = false;

  // step5b: setup dispatch of network events
  bool serialize_output = false;
  if (!Common_Tools::initializeEventDispatch (RPG_NET_USES_REACTOR,
                                              configuration_in.num_dispatch_threads,
                                              serialize_output))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize network event dispatch, returning\n")));

    // clean up
    if (!SDL_RemoveTimer(GTKUserData_in.eventTimer))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                 ACE_TEXT(SDL_GetError())));
//    level_engine.stop();
    client_engine.stop();
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini();

    return;
  } // end IF

  // step5c: init connection manager
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
  RPG_Net_Protocol_SessionData session_data;
  ACE_OS::memset (&session_data, 0, sizeof (RPG_Net_Protocol_SessionData));
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->set (protocol_configuration,
                                                                  &session_data);

  // step5d: start worker(s)
  int group_id = -1;
  if (!Common_Tools::startEventDispatch (RPG_NET_USES_REACTOR,
                                         configuration_in.num_dispatch_threads,
                                         group_id))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start network event dispatch, returning\n")));

    // clean up
    if (!SDL_RemoveTimer (GTKUserData_in.eventTimer))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));
//    level_engine.stop();
    client_engine.stop ();
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini ();

    return;
  } // end IF

  // *NOTE*: from this point on, clean up any connections !

  // step6: dispatch UI events
  // step6a: start GTK event loop
  GTKUserData_in.GTKState.initializationHook = idle_initialize_UI_cb;
  GTKUserData_in.GTKState.finalizationHook = idle_finalize_UI_cb;
  GTKUserData_in.GTKState.gladeXML[ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
  GTKUserData_in.GTKState.userData = &GTKUserData_in;

  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
  if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start GTK event dispatch, returning\n")));

    // clean up
    if (!SDL_RemoveTimer(GTKUserData_in.eventTimer))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                 ACE_TEXT(SDL_GetError())));
    //		level_engine.stop();
    client_engine.stop();
    COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    RPG_Client_Common_Tools::finalize ();
    RPG_Engine_Common_Tools::fini();

    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->abort ();
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->wait ();
    Common_Tools::finalizeEventDispatch (RPG_NET_USES_REACTOR,
                                         !RPG_NET_USES_REACTOR,
                                         group_id);

    return;
  } // end IF

  // step6b: dispatch SDL events
  SDL_Event sdl_event;
  bool done = false;
  RPG_Graphics_IWindowBase* window = NULL;
  RPG_Graphics_IWindowBase* previous_window = NULL;
  RPG_Graphics_Position_t mouse_position;
  do
  {
    sdl_event.type = SDL_NOEVENT;
    window = NULL;
    client_action.command = RPG_CLIENT_COMMAND_INVALID;
    client_action.position = std::make_pair(std::numeric_limits<int>::max(),
                                            std::numeric_limits<int>::max());
    client_action.window = NULL;
    client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
    client_action.entity_id = 0;
    client_action.path.clear();
    mouse_position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                    std::numeric_limits<unsigned int>::max());
    ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));

    // step6a: get next pending event
//     if (SDL_PollEvent(&event) == -1)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_PollEvent(): \"%s\", aborting\n"),
//                  SDL_GetError()));
//
//       break;
//     } // end IF
    if (SDL_WaitEvent(&sdl_event) == 0)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 ACE_TEXT(SDL_GetError())));

      break;
    } // end IF

    // if necessary, reset hover_time
    if ((sdl_event.type != RPG_GRAPHICS_SDL_HOVEREVENT) &&
        //(sdl_event.type != RPG_CLIENT_SDL_GTKEVENT) &&
        (sdl_event.type != RPG_CLIENT_SDL_TIMEREVENT))
    {
      // synch access
      ACE_Guard<ACE_Thread_Mutex> aGuard (GTKUserData_in.GTKState.lock);

      GTKUserData_in.hoverTime = 0;
    } // end IF

    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (sdl_event.key.keysym.sym)
        {
          //case SDLK_q:
          //{
          //  // finished event processing
          //  done = true;

          //  break;
          //}
          default:
            break;
        } // end SWITCH

        if (done)
          break; // leave
        // *WARNING*: falls through !
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
            ACE_Guard<ACE_Thread_Mutex> aGuard(GTKUserData_in.GTKState.lock);

            GTKUserData_in.doHover = true;
          } // end IF
          else
          {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

            // synch access
            ACE_Guard<ACE_Thread_Mutex> aGuard(GTKUserData_in.GTKState.lock);

            GTKUserData_in.doHover = false;
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
            int x, y;
            Uint8 button_state = SDL_GetMouseState(&x, &y);
            ACE_UNUSED_ARG(button_state);
            mouse_position = std::make_pair(x, y);

            break;
          }
        } // end SWITCH
        window = main_window.getWindow(mouse_position);
        if (!window)
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("failed to RPG_Graphics_SDLWindowBase::getWindow(%u,%u), continuing\n"),
                     mouse_position.first, mouse_position.second));
        } // end IF

        // first steps on mouse motion:
        // 0. (re-)draw cursor (handled below)
        // 1. notify previously "active" window upon losing "focus"
        if ((window || previous_window) &&
            (sdl_event.type == SDL_MOUSEMOTION))
        {
          // step1: notify previous window (if any)
          if (previous_window &&
//               (previous_window != mainWindow)
              (previous_window != window))
          {
            sdl_event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;

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

        // 2. notify "active" window (if any)
        if (window)
        {
          try
          {
            window->handleEvent(sdl_event,
                                window,
                                dirty_region);
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
          }
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
        break;
      case SDL_VIDEOEXPOSE:
      {
        int x, y;
        Uint8 button_state = SDL_GetMouseState(&x, &y);
        ACE_UNUSED_ARG(button_state);
        mouse_position = std::make_pair(x, y);
        window = main_window.getWindow(mouse_position);

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unknown/invalid SDL event type (was: %d), continuing\n"),
                   sdl_event.type));

        break;
      }
    } // end SWITCH

    // update screen ?
    if ((dirty_region.w != 0) ||
        (dirty_region.h != 0))
    {
      client_action.command = COMMAND_WINDOW_REFRESH;
      client_action.window = window;
      client_engine.action(client_action);
    } // end IF

    // redraw cursor ?
    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
      {
        // map hasn't changed --> no need to redraw
        if ((dirty_region.w == 0) &&
            (dirty_region.h == 0))
          break;

        // *WARNING*: falls through !
      }
      case SDL_MOUSEMOTION:
      case RPG_GRAPHICS_SDL_HOVEREVENT:
      {
        // sanity check
        if (!window)
          break; // nothing to do...

        // map has changed, cursor MAY have been drawn over...
        // --> redraw cursor
        client_action.command = COMMAND_CURSOR_DRAW;
        client_action.position = mouse_position;
        client_action.window = window;
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
//         map_window.refresh(NULL);
//       }
//       catch (...)
//       {
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("caught exception in RPG_Graphics_IWindow::refresh(), continuing\n")));
//       }
//     } // end IF
  } while (!done);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("left event loop...\n")));

  // step7: clean up
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
  if (!SDL_RemoveTimer(GTKUserData_in.eventTimer))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               ACE_TEXT(SDL_GetError())));
  level_engine.stop();
  client_engine.stop();
  COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
  RPG_Client_Common_Tools::finalize ();
  RPG_Engine_Common_Tools::fini();
  // done handling UI events

  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->abort ();
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->wait ();
  Common_Tools::finalizeEventDispatch (RPG_NET_USES_REACTOR,
                                       !RPG_NET_USES_REACTOR,
                                       group_id);
  // no more data will arrive from here on...

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseIniFile (const std::string& iniFilename_in,
                 RPG_Client_Configuration_t& config_out)
{
  RPG_TRACE (ACE_TEXT ("::do_parseIniFile"));

  // init return value(s)
  //config_out.audio_configuration.mute = false;
  config_out.audio_configuration.SDL_configuration.frequency =
      RPG_SOUND_AUDIO_DEF_FREQUENCY;
  config_out.audio_configuration.SDL_configuration.format    =
      RPG_SOUND_AUDIO_DEF_FORMAT;
  config_out.audio_configuration.SDL_configuration.channels  =
      RPG_SOUND_AUDIO_DEF_CHANNELS;
  config_out.audio_configuration.SDL_configuration.chunksize =
      RPG_SOUND_AUDIO_DEF_CHUNKSIZE;
  config_out.audio_configuration.use_CD = RPG_SOUND_AMBIENT_DEF_USE_CD;

  config_out.video_configuration.screen_width      = RPG_CLIENT_VIDEO_DEF_WIDTH;
  config_out.video_configuration.screen_height     = RPG_CLIENT_VIDEO_DEF_HEIGHT;
  config_out.video_configuration.screen_colordepth = RPG_CLIENT_VIDEO_DEF_BPP;
  config_out.video_configuration.full_screen       =
      RPG_CLIENT_VIDEO_DEF_FULLSCREEN;
  config_out.video_configuration.double_buffer     =
      RPG_CLIENT_VIDEO_DEF_DOUBLEBUFFER;

  config_out.network_configuration.server   = RPG_ENGINE_NETWORK_DEF_SERVER;
  config_out.network_configuration.port     = RPG_ENGINE_NETWORK_DEF_PORT;
  config_out.network_configuration.password = RPG_ENGINE_NETWORK_DEF_PASSWORD;
  config_out.network_configuration.nick     = RPG_ENGINE_NETWORK_DEF_NICKNAME;
  config_out.network_configuration.user     = RPG_ENGINE_NETWORK_DEF_USERNAME;
  config_out.network_configuration.realname = RPG_ENGINE_NETWORK_DEF_REALNAME;
  config_out.network_configuration.channel  = RPG_ENGINE_NETWORK_DEF_CHANNEL;

  ACE_Configuration_Heap config_heap;
  if (config_heap.open())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Configuration_Heap::open failed, returning\n")));

    return;
  } // end IF

  ACE_Ini_ImpExp import(config_heap);
  if (import.import_config(iniFilename_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Ini_ImpExp::import_config(\"%s\") failed, returning\n"),
               ACE_TEXT(iniFilename_in.c_str())));

    return;
  } // end IF

  // find/open "client" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_CONFIGURATION_SECTIONHEADER_CLIENT),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               ACE_TEXT(RPG_CLIENT_CONFIGURATION_SECTIONHEADER_CONNECTION)));

    return;
  } // end IF

  // import values...
  int val_index = 0;
  ACE_TString val_name, val_value;
  ACE_Configuration::VALUETYPE val_type;
  std::istringstream converter;
  while (config_heap.enumerate_values(section_key,
                                      val_index,
                                      val_name,
                                      val_type) == 0)
  {
    if (config_heap.get_string_value(section_key,
                                     val_name.c_str(),
                                     val_value))
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
                 ACE_TEXT(val_name.c_str())));

      return;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                ACE_TEXT(val_name.c_str()),
//                val_type));

    // *TODO*: move these strings...
    if (val_name == ACE_TEXT("audio_frequency"))
    {
      config_out.audio_configuration.SDL_configuration.frequency =
          ::atoi(val_value.c_str());
    }
//     else if (val_name == ACE_TEXT("audio_format"))
//     {
//       audioConfig_out.SDL_configuration.format = ::atoi(val_value.c_str());
//     }
    else if (val_name == ACE_TEXT("audio_channels"))
    {
      config_out.audio_configuration.SDL_configuration.channels =
          ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("audio_chunksize"))
    {
      config_out.audio_configuration.SDL_configuration.chunksize =
          ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("audio_cd"))
    {
      converter.str(val_value.c_str());
      converter >> config_out.audio_configuration.use_CD;
      if (converter.fail())
      {
        converter.clear();
        converter >> std::boolalpha >> config_out.audio_configuration.use_CD;
      } // end IF
    }
    else if (val_name == ACE_TEXT("screen_width"))
    {
      config_out.video_configuration.screen_width = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_height"))
    {
      config_out.video_configuration.screen_height = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_colordepth"))
    {
      config_out.video_configuration.screen_colordepth =
          ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_fullscreen"))
    {
      converter.str(val_value.c_str());
      converter >> config_out.video_configuration.full_screen;
      if (converter.fail())
      {
        converter.clear();
        converter >> std::boolalpha
                  >> config_out.video_configuration.full_screen;
      } // end IF
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 ACE_TEXT(val_name.c_str())));
    } // end ELSE

    ++val_index;
  } // end WHILE

   // find/open "connection" section...
   if (config_heap.open_section(config_heap.root_section(),
                                ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_CONFIGURATION_SECTIONHEADER_CONNECTION),
                                0, // MUST exist !
                                section_key) != 0)
   {
     ACE_ERROR((LM_ERROR,
                ACE_TEXT("failed to ACE_Configuration_Heap::open_section(\"%s\"), returning\n"),
                ACE_TEXT(RPG_CLIENT_CONFIGURATION_SECTIONHEADER_CONNECTION)));

     return;
   } // end IF

   // import values...
   val_index = 0;
   while (config_heap.enumerate_values(section_key,
                                       val_index,
                                       val_name,
                                       val_type) == 0)
   {
     if (config_heap.get_string_value(section_key,
                                      val_name.c_str(),
                                      val_value))
     {
       ACE_ERROR((LM_ERROR,
                  ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
                  ACE_TEXT(val_name.c_str())));

       return;
     } // end IF

 //     ACE_DEBUG((LM_DEBUG,
 //                ACE_TEXT("enumerated %s, type %d\n"),
 //                ACE_TEXT(val_name.c_str()),
 //                val_type));

     // *TODO*: move these strings...
     if (val_name == ACE_TEXT("server"))
     {
       config_out.network_configuration.server = val_value.c_str();
     }
     else if (val_name == ACE_TEXT("port"))
     {
       config_out.network_configuration.port = ::atoi(val_value.c_str());
     }
     else if (val_name == ACE_TEXT("password"))
     {
       config_out.network_configuration.password = val_value.c_str();
     }
     else if (val_name == ACE_TEXT("nick"))
     {
       config_out.network_configuration.nick = val_value.c_str();
     }
     else if (val_name == ACE_TEXT("user"))
     {
       config_out.network_configuration.user = val_value.c_str();
     }
     else if (val_name == ACE_TEXT("realname"))
     {
       config_out.network_configuration.realname = val_value.c_str();
     }
     else if (val_name == ACE_TEXT("channel"))
     {
       config_out.network_configuration.channel = val_value.c_str();
     }
     else
     {
       ACE_ERROR((LM_ERROR,
                  ACE_TEXT("unexpected key \"%s\", continuing\n"),
                  ACE_TEXT(val_name.c_str())));
     } // end ELSE

     ++val_index;
   } // end WHILE

   ACE_DEBUG((LM_DEBUG,
              ACE_TEXT("imported \"%s\"...\n"),
              ACE_TEXT(iniFilename_in.c_str())));
}

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  // step1: print program name/version
//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            << YARP_VERSION
#endif
            << std::endl;

  // step2: print ACE version
  // create ACE version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... We need this, as the library soname is compared to this
  // string
  std::ostringstream version_number;
  version_number << ACE::major_version();
  version_number << ACE_TEXT(".");
  version_number << ACE::minor_version();
  if (ACE::beta_version())
  {
    version_number << ACE_TEXT(".");
    version_number << ACE::beta_version();
  } // end IF

//   std::cout << "ACE: " << ACE_VERSION << std::endl;
  std::cout << ACE_TEXT("ACE: ")
            << version_number.str()
            << std::endl;

  // step3: print SDL version(s)
  SDL_version sdl_version_compiled;
  SDL_VERSION(&sdl_version_compiled);
  version_number.str(ACE_TEXT_ALWAYS_CHAR(""));
  version_number << sdl_version_compiled.major;
  version_number << ACE_TEXT(".");
  version_number << sdl_version_compiled.minor;
  version_number << ACE_TEXT(".");
  version_number << sdl_version_compiled.patch;
  std::cout << ACE_TEXT("SDL (compiled against): ")
            << version_number.str()
            << std::endl;
  const SDL_version* sdl_version_linked = SDL_Linked_Version();
  version_number.str("");
  version_number << sdl_version_linked->major;
  version_number << ACE_TEXT(".");
  version_number << sdl_version_linked->minor;
  version_number << ACE_TEXT(".");
  version_number << sdl_version_linked->patch;
  std::cout << ACE_TEXT("SDL (linked against): ")
            << version_number.str()
            << std::endl;
  const SDL_version* mix_version_linked = Mix_Linked_Version();
  version_number.str(ACE_TEXT_ALWAYS_CHAR(""));
  version_number << mix_version_linked->major;
  version_number << ACE_TEXT(".");
  version_number << mix_version_linked->minor;
  version_number << ACE_TEXT(".");
  version_number << mix_version_linked->patch;
  std::cout << ACE_TEXT("SDL_mixer (linked against): ")
            << version_number.str()
            << std::endl;

  // step3: print compiler name/version
  version_number.str(ACE_TEXT_ALWAYS_CHAR(""));
  version_number << ACE::compiler_name();
  version_number << ACE_TEXT(" ");
  version_number << ACE::compiler_major_version();
  version_number << ACE_TEXT(".");
  version_number << ACE::compiler_minor_version();
  if (ACE::compiler_beta_version())
  {
    version_number << ACE_TEXT(".");
    version_number << ACE::compiler_beta_version();
  } // end IF
  std::cout << ACE_TEXT("compiled by: ")
            << version_number.str()
            << std::endl;
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
SDL_main (int argc_in,
          char** argv_in)
#else
int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
#endif
{
  RPG_TRACE (ACE_TEXT ("::main"));

//  // step1: initialize ACE
//// *PORTABILITY*: on Windows, need to init ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (ACE::init () == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
//    return EXIT_FAILURE;
//  } // end IF
//#endif

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  // step1 init/validate configuration
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  // step1a: process commandline arguments
  bool mute_sound                   = false;
  bool debug                        = RPG_CLIENT_DEF_DEBUG;
  std::string configuration_file    = configuration_path;
  configuration_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  configuration_file += ACE_TEXT_ALWAYS_CHAR ("client");
  configuration_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  configuration_file += ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_CONFIGURATION_FILE);

  std::string monster_dictionary    = configuration_path;
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR ("character");
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR ("monster");
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  monster_dictionary += RPG_MONSTER_DICTIONARY_FILE;

  std::string graphics_dictionary   = configuration_path;
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR ("graphics");
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);

  std::string item_dictionary       = configuration_path;
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  item_dictionary += ACE_TEXT_ALWAYS_CHAR ("item");
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);

  std::string magic_dictionary      = configuration_path;
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR ("magic");
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);

  std::string sound_dictionary      = configuration_path;
  sound_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  sound_dictionary += ACE_TEXT_ALWAYS_CHAR ("sound");
  sound_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  sound_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DICTIONARY_FILE);

  std::string UI_file               = configuration_path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  UI_file += ACE_TEXT_ALWAYS_CHAR ("client");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UI_file += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_UI_FILE);

  std::string schema_repository     = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR ("engine");
#endif

  std::string floor_plan            = data_path;
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  floor_plan += ACE_TEXT_ALWAYS_CHAR ("engine");
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  floor_plan += ACE_TEXT_ALWAYS_CHAR ("data");
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  floor_plan += ACE_TEXT_ALWAYS_CHAR(RPG_MAP_MAPS_SUB);
  floor_plan += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  floor_plan +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
  floor_plan += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);

  bool log_to_file                  = false;
  bool trace_information            = false;
  bool print_version_and_exit       = false;
  unsigned int num_dispatch_threads = NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
  std::string video_driver          =
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME);
  bool skip_intro                   = false;
  if (!do_processArguments (argc_in,
                            argv_in,
                            mute_sound,
                            configuration_file,
                            debug,
                            monster_dictionary,
                            floor_plan,
                            graphics_dictionary,
                            item_dictionary,
                            log_to_file,
                            magic_dictionary,
                            skip_intro,
                            schema_repository,
                            sound_dictionary,
                            trace_information,
                            UI_file,
                            print_version_and_exit,
                            num_dispatch_threads,
                            video_driver))
  {
    do_printUsage (ACE::basename (argv_in[0]));

//    // clean up
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//#endif

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!Common_File_Tools::isReadable (configuration_file)  ||
      !Common_File_Tools::isReadable (monster_dictionary)  ||
      !Common_File_Tools::isReadable (floor_plan)          ||
      !Common_File_Tools::isReadable (graphics_dictionary) ||
      !Common_File_Tools::isReadable (item_dictionary)     ||
      !Common_File_Tools::isReadable (magic_dictionary)    ||
      !Common_File_Tools::isDirectory (schema_repository)  ||
      !Common_File_Tools::isReadable (sound_dictionary)    ||
      !Common_File_Tools::isReadable (UI_file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument: \n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n, aborting\n"),
                ACE_TEXT (configuration_file.c_str ()),
                ACE_TEXT (monster_dictionary.c_str ()),
                ACE_TEXT (floor_plan.c_str ()),
                ACE_TEXT (graphics_dictionary.c_str ()),
                ACE_TEXT (item_dictionary.c_str ()),
                ACE_TEXT (magic_dictionary.c_str ()),
                ACE_TEXT (schema_repository.c_str ()),
                ACE_TEXT (sound_dictionary.c_str ()),
                ACE_TEXT (UI_file.c_str ())));

    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // clean up
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    // clean up
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//#endif

    return EXIT_SUCCESS;
  } // end IF

  RPG_Client_GTK_CBData_t GTK_user_data;

  GTK_user_data.levelMetadata.name                 =
      ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME);
  //
  GTK_user_data.levelMetadata.environment.plane =
      RPG_ENGINE_ENVIRONMENT_DEF_PLANE;
  GTK_user_data.levelMetadata.environment.terrain =
      RPG_ENGINE_ENVIRONMENT_DEF_TERRAIN;
  GTK_user_data.levelMetadata.environment.climate =
      RPG_ENGINE_ENVIRONMENT_DEF_CLIMATE;
  GTK_user_data.levelMetadata.environment.time =
      RPG_ENGINE_ENVIRONMENT_DEF_TIMEOFDAY;
  GTK_user_data.levelMetadata.environment.lighting =
      RPG_ENGINE_ENVIRONMENT_DEF_LIGHTING;
  GTK_user_data.levelMetadata.environment.outdoors =
      RPG_ENGINE_ENVIRONMENT_DEF_OUTDOORS;
  //
  RPG_Engine_Spawn_t spawn;
  spawn.spawn.type = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENCOUNTER_DEF_TYPE);
  spawn.spawn.interval.seconds = RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL;
  spawn.spawn.interval.u_seconds = 0;
  spawn.spawn.probability =	RPG_ENGINE_ENCOUNTER_DEF_PROBABILITY;
  spawn.spawn.max_num_spawned =	RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX;
  spawn.spawn.amble_probability = RPG_ENGINE_ENCOUNTER_DEF_AMBLE_PROBABILITY;
  spawn.timer_id = -1;
  GTK_user_data.levelMetadata.spawns.push_back (spawn);
  GTK_user_data.levelMetadata.max_num_spawned =
      RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX;
  //
  GTK_user_data.mapConfiguration.min_room_size          =
      RPG_CLIENT_MAP_DEF_MIN_ROOM_SIZE;
  GTK_user_data.mapConfiguration.doors =
      RPG_CLIENT_MAP_DEF_DOORS;
  GTK_user_data.mapConfiguration.corridors =
      RPG_CLIENT_MAP_DEF_CORRIDORS;
  GTK_user_data.mapConfiguration.max_num_doors_per_room =
      RPG_CLIENT_MAP_DEF_MAX_NUM_DOORS_PER_ROOM;
  GTK_user_data.mapConfiguration.maximize_rooms =
      RPG_CLIENT_MAP_DEF_MAXIMIZE_ROOMS;
  GTK_user_data.mapConfiguration.num_areas =
      RPG_CLIENT_MAP_DEF_NUM_AREAS;
  GTK_user_data.mapConfiguration.square_rooms =
      RPG_CLIENT_MAP_DEF_SQUARE_ROOMS;
  GTK_user_data.mapConfiguration.map_size_x =
      RPG_CLIENT_MAP_DEF_SIZE_X;
  GTK_user_data.mapConfiguration.map_size_y =
      RPG_CLIENT_MAP_DEF_SIZE_Y;

  // step1c: initialize logging and/or tracing
  Common_Logger logger (&GTK_user_data.logStack,
                        &GTK_user_data.logStackLock);
  std::string log_file;
  if (log_to_file)
    log_file = Common_File_Tools::getLogFilename (ACE::basename (argv_in[0]));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                        log_file,                  // logfile
                                        false,                     // log to syslog ?
                                        false,                     // trace messages ?
                                        trace_information,         // debug messages ?
                                        &logger))                  // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // clean up
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//#endif

    return EXIT_FAILURE;
  } // end IF

  // step1da: init configuration object
  RPG_Client_Configuration_t configuration;
  // *** reactor ***
  configuration.num_dispatch_threads = num_dispatch_threads;

  // *** UI ***
  //configuration.glade_file           = UI_file;

  // *** input ***
  configuration.input_configuration.use_UNICODE              = true;
  configuration.input_configuration.key_repeat_initial_delay =
      SDL_DEFAULT_REPEAT_DELAY;
  configuration.input_configuration.key_repeat_interval      =
      SDL_DEFAULT_REPEAT_INTERVAL;
  // *** sound ***
  configuration.audio_configuration.mute                        = mute_sound;
  configuration.audio_configuration.SDL_configuration.frequency =
      RPG_SOUND_AUDIO_DEF_FREQUENCY;
  configuration.audio_configuration.SDL_configuration.format    =
      RPG_SOUND_AUDIO_DEF_FORMAT;
  configuration.audio_configuration.SDL_configuration.channels  =
      RPG_SOUND_AUDIO_DEF_CHANNELS;
  configuration.audio_configuration.SDL_configuration.chunksize =
      RPG_SOUND_AUDIO_DEF_CHUNKSIZE;
  configuration.audio_configuration.use_CD                      =
      RPG_SOUND_AMBIENT_DEF_USE_CD;
  configuration.audio_configuration.repository                  = data_path;
  configuration.audio_configuration.repository +=
      ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  configuration.audio_configuration.repository += ACE_TEXT_ALWAYS_CHAR("sound");
  configuration.audio_configuration.repository +=
      ACE_DIRECTORY_SEPARATOR_CHAR_A;
  configuration.audio_configuration.repository +=
      ACE_TEXT_ALWAYS_CHAR("data");
#else
  configuration.audio_configuration.repository +=
      ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DATA_SUB);
#endif
  configuration.audio_configuration.dictionary = sound_dictionary;

  // *** graphics ***
  configuration.video_configuration.screen_width      = RPG_CLIENT_VIDEO_DEF_WIDTH;
  configuration.video_configuration.screen_height     = RPG_CLIENT_VIDEO_DEF_HEIGHT;
  configuration.video_configuration.screen_colordepth =
      RPG_CLIENT_VIDEO_DEF_BPP;
  configuration.video_configuration.double_buffer     =
      RPG_CLIENT_VIDEO_DEF_DOUBLEBUFFER;
  configuration.video_configuration.use_OpenGL        =
      RPG_CLIENT_VIDEO_DEF_OPENGL;
  configuration.video_configuration.full_screen       =
      RPG_CLIENT_VIDEO_DEF_FULLSCREEN;
  configuration.video_configuration.video_driver      = video_driver;
  configuration.graphics_directory   = data_path;
  configuration.graphics_directory   += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  configuration.graphics_directory   += ACE_TEXT_ALWAYS_CHAR("graphics");
  configuration.graphics_directory   += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  configuration.graphics_directory   +=
      ACE_TEXT_ALWAYS_CHAR("data");
#else
  configuration.graphics_directory   +=
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#endif
  configuration.graphics_dictionary  = graphics_dictionary;

  // *** network ***
  configuration.network_configuration.server   = RPG_ENGINE_NETWORK_DEF_SERVER;
  configuration.network_configuration.port     = RPG_ENGINE_NETWORK_DEF_PORT;
  configuration.network_configuration.password =
      RPG_ENGINE_NETWORK_DEF_PASSWORD;
  configuration.network_configuration.nick     = RPG_ENGINE_NETWORK_DEF_NICKNAME;
  configuration.network_configuration.user     = RPG_ENGINE_NETWORK_DEF_USERNAME;
  configuration.network_configuration.realname =
      RPG_ENGINE_NETWORK_DEF_REALNAME;
  configuration.network_configuration.channel  = RPG_ENGINE_NETWORK_DEF_CHANNEL;

  // *** magic ***
  configuration.magic_dictionary     = magic_dictionary;

  // *** item ***
  configuration.item_dictionary      = item_dictionary;

  // *** monster ***
  configuration.monster_dictionary   = monster_dictionary;

  // *** map ***
  configuration.map_configuration    = GTK_user_data.mapConfiguration;
  configuration.map_file             = floor_plan;

//   // step1db: populate config object with default/collected data
//   // ************ connection config data ************
//   configuration.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
//   configuration.messageAllocator = &messageAllocator;
//   configuration.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
//   // ************ protocol config data **************
//   configuration.clientPingInterval = 0; // servers do this...
// //   configuration.loginOptions = userData.loginOptions;
//   // ************ stream config data ****************
//   configuration.debugParser = debugParser;
//   configuration.module = &IRChandlerModule;
//   // *WARNING*: set at runtime, by the appropriate connection handler
//   configuration.sessionID = 0; // (== socket handle !)
//   configuration.statisticsReportingInterval = 0; // == off

  // step1dc: parse .ini file (if any)
  if (!configuration_file.empty())
    do_parseIniFile(configuration_file,
                    configuration);

  // step2a: init SDL
  Uint32 SDL_init_flags = 0;
  SDL_init_flags |= SDL_INIT_TIMER;                                            // timers
  SDL_init_flags |= (configuration.audio_configuration.mute ? 0
                                                            : SDL_INIT_AUDIO); // audio
//  SDL_init_flags |= SDL_INIT_VIDEO;                                            // video
  SDL_init_flags |=
      ((configuration.audio_configuration.mute ||
        !configuration.audio_configuration.use_CD) ? 0
                                                   : SDL_INIT_CDROM);          // audioCD playback
//  SDL_init_flags |= SDL_INIT_JOYSTICK;                                         // joystick
  SDL_init_flags |= SDL_INIT_NOPARACHUTE;                                        /**< Don't catch fatal signals */
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  SDL_init_flags |= SDL_INIT_EVENTTHREAD;                                        /**< Not supported on all OS's */
#endif
  if (SDL_Init(SDL_init_flags) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(0x%x): \"%s\", aborting\n"),
               SDL_init_flags,
               ACE_TEXT(SDL_GetError())));

    // clean up
  Common_Tools::finalizeLogging ();
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//#endif

    return EXIT_FAILURE;
  } // end IF
  if (TTF_Init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to TTF_Init(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    Common_Tools::finalizeLogging ();
    SDL_VideoQuit ();
    SDL_Quit ();
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//#endif

    return EXIT_FAILURE;
  } // end IF

  // step2b: init GLIB / G(D|T)K[+] / GNOME
  Common_UI_GladeDefinition ui_definition (argc_in,
                                           argv_in);
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                            argv_in,
                                                            &GTK_user_data.GTKState,
                                                            &ui_definition);

  ACE_High_Res_Timer timer;
  timer.start ();
  // step3: do actual work
  do_work (configuration,
           schema_repository,
           ui_definition,
           GTK_user_data,
           UI_file,
           skip_intro,
           debug);
  timer.stop ();
  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  Common_Tools::period2String (working_time,
                               working_time_string);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // step4: clean up
  TTF_Quit ();
  SDL_VideoQuit ();
  SDL_Quit ();

  // stop profile timer...
  process_profile.stop();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time (elapsed_time) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    // clean up
    Common_Tools::finalizeLogging ();
//    // *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    if (ACE::fini () == -1)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//    #endif

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  Common_Tools::period2String (user_time,
                               user_time_string);
  Common_Tools::period2String (system_time,
                               system_time_string);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT(user_time_string.c_str()),
              ACE_TEXT(system_time_string.c_str()),
              elapsed_rusage.ru_maxrss,
              elapsed_rusage.ru_ixrss,
              elapsed_rusage.ru_idrss,
              elapsed_rusage.ru_isrss,
              elapsed_rusage.ru_minflt,
              elapsed_rusage.ru_majflt,
              elapsed_rusage.ru_nswap,
              elapsed_rusage.ru_inblock,
              elapsed_rusage.ru_oublock,
              elapsed_rusage.ru_msgsnd,
              elapsed_rusage.ru_msgrcv,
              elapsed_rusage.ru_nsignals,
              elapsed_rusage.ru_nvcsw,
              elapsed_rusage.ru_nivcsw));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#endif

  Common_Tools::finalizeLogging ();

//// *PORTABILITY*: on Windows, must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (ACE::fini () == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
//    return EXIT_FAILURE;
//  } // end IF
//#endif

  return EXIT_SUCCESS;
} // end main
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
