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

// *NOTE*: need this to import correct PACKAGE_STRING/VERSION/... !
#ifdef HAVE_CONFIG_H
#include <rpg_config.h>
#endif

#include <rpg_client_defines.h>
#include <rpg_client_common.h>
#include <rpg_client_callbacks.h>
#include <rpg_client_window_main.h>
#include <rpg_client_window_level.h>

#include <rpg_map_defines.h>
#include <rpg_map_common_tools.h>
#include <rpg_map_level.h>

#include <rpg_engine_common_tools.h>

#include <rpg_monster_defines.h>

#include <rpg_character_defines.h>
#include <rpg_character_player.h>

#include <rpg_item_defines.h>

#include <rpg_magic_defines.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_SDL_tools.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_sound_defines.h>
#include <rpg_sound_dictionary.h>
#include <rpg_sound_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <gnome.h>
#include <glade/glade.h>
#include <gtk/gtk.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <ace/ACE.h>
#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/High_Res_Timer.h>
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>
#include <ace/Thread_Manager.h>

#include <string>
#include <iostream>
#include <sstream>

// init statics
static GtkWidget* main_dialog = NULL;

Uint32
event_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  RPG_TRACE(ACE_TEXT("::event_timer_SDL_cb"));

  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (argument_in);
  ACE_ASSERT(data);

  SDL_Event event;
  event.type = SDL_NOEVENT;
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(data->lock);

    data->hover_time += interval_in;
    data->gtk_time += interval_in;
    if (data->hover_time >= RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY)
    {
      // mouse is hovering --> trigger an event
      event.type = RPG_GRAPHICS_SDL_HOVEREVENT;
      event.user.code = static_cast<int> (data->hover_time);

      if (SDL_PushEvent(&event))
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
    event.type = RPG_CLIENT_SDL_GTKEVENT;

    if (SDL_PushEvent(&event))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                 SDL_GetError()));
  } // end IF
  GDK_THREADS_LEAVE();

  // re-schedule
  return interval_in;
}

Uint32
input_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  RPG_TRACE(ACE_TEXT("::input_timer_SDL_cb"));

  // create a timer event
  SDL_Event event;
  event.type = RPG_CLIENT_SDL_TIMEREVENT;
  event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent(&event))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput(const unsigned long& timeout_in,
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
               SDL_GetError()));

    return;
  } // end IF

  // loop until something interesting happens
  do
  {
    if (SDL_WaitEvent(&event_out) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 SDL_GetError()));

      // what else can we do ?
      break;
    } // end IF
    if (event_out.type == SDL_KEYDOWN ||
        event_out.type == SDL_KEYUP ||
        event_out.type == SDL_MOUSEMOTION ||
        event_out.type == SDL_MOUSEBUTTONDOWN ||
        event_out.type == SDL_QUIT ||
        event_out.type == RPG_CLIENT_SDL_TIMEREVENT)
      break;
  } while (true);

  // clean up
  if (timeout_in &&
      (event_out.type != RPG_CLIENT_SDL_TIMEREVENT))
    if (!SDL_RemoveTimer(timer))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF
}

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_CLIENT_DEF_INI_FILE;
  std::cout << ACE_TEXT("-c [FILE]   : config file") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_MONSTER_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-e [FILE]   : monster dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-f [FILE]   : floor plan (*.txt)") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_GRAPHICS_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-g [FILE]   : graphics dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_ITEM_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-i [FILE]   : item dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_MAGIC_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-m [FILE]   : magic dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_SOUND_DEF_DICTIONARY_FILE;
  std::cout << ACE_TEXT("-s [FILE]   : sound dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  path = base_data_path;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_COMMON_DEF_CONFIG_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_CLIENT_DEF_GNOME_UI_FILE;
  std::cout << ACE_TEXT("-u [FILE]   : UI file") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x<[VALUE]> : use thread pool <#threads>") << ACE_TEXT(" [") << RPG_CLIENT_DEF_ACE_USES_TP  << ACE_TEXT(" : ") << RPG_CLIENT_DEF_ACE_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& iniFile_out,
                  std::string& monsterDictionary_out,
                  std::string& floorPlan_out,
                  std::string& graphicsDictionary_out,
                  std::string& itemDictionary_out,
                  bool& logToFile_out,
                  std::string& magicDictionary_out,
                  std::string& soundDictionary_out,
                  bool& traceInformation_out,
                  std::string& UIfile_out,
                  bool& printVersionAndExit_out,
                  unsigned long& numThreadPoolThreads_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  iniFile_out = base_data_path;
  iniFile_out += ACE_DIRECTORY_SEPARATOR_STR;
  iniFile_out += RPG_COMMON_DEF_CONFIG_SUB;
  iniFile_out += ACE_DIRECTORY_SEPARATOR_STR;
  iniFile_out += RPG_CLIENT_DEF_INI_FILE;

  monsterDictionary_out = base_data_path;
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  monsterDictionary_out += RPG_COMMON_DEF_CONFIG_SUB;
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  monsterDictionary_out += RPG_MONSTER_DEF_DICTIONARY_FILE;

  floorPlan_out.clear();

  graphicsDictionary_out = base_data_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  graphicsDictionary_out += RPG_COMMON_DEF_CONFIG_SUB;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  graphicsDictionary_out += RPG_GRAPHICS_DEF_DICTIONARY_FILE;

  itemDictionary_out = base_data_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionary_out += RPG_COMMON_DEF_CONFIG_SUB;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionary_out += RPG_ITEM_DEF_DICTIONARY_FILE;

  logToFile_out            = false;

  magicDictionary_out = base_data_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionary_out += RPG_COMMON_DEF_CONFIG_SUB;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionary_out += RPG_MAGIC_DEF_DICTIONARY_FILE;

  soundDictionary_out = base_data_path;
  soundDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  soundDictionary_out += RPG_COMMON_DEF_CONFIG_SUB;
  soundDictionary_out += ACE_DIRECTORY_SEPARATOR_STR;
  soundDictionary_out += RPG_SOUND_DEF_DICTIONARY_FILE;

  traceInformation_out     = false;

  UIfile_out = base_data_path;
  UIfile_out += ACE_DIRECTORY_SEPARATOR_STR;
  UIfile_out += RPG_COMMON_DEF_CONFIG_SUB;
  UIfile_out += ACE_DIRECTORY_SEPARATOR_STR;
  UIfile_out += RPG_CLIENT_DEF_GNOME_UI_FILE;

  printVersionAndExit_out  = false;

  numThreadPoolThreads_out = (RPG_CLIENT_DEF_ACE_USES_TP ? RPG_CLIENT_DEF_ACE_NUM_TP_THREADS : 0);

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:e:f:g:i:lm:s:tu:vx::"),
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
      case 'c':
      {
        iniFile_out = argumentParser.opt_arg();

        break;
      }
      case 'e':
      {
        monsterDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'f':
      {
        floorPlan_out = argumentParser.opt_arg();

        break;
      }
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
        logToFile_out = true;

        break;
      }
      case 'm':
      {
        magicDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 's':
      {
        soundDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UIfile_out = argumentParser.opt_arg();

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        numThreadPoolThreads_out = RPG_CLIENT_DEF_ACE_NUM_TP_THREADS;
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> numThreadPoolThreads_out;

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
                   argumentParser.last_option()));

        return false;
      }
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("found long option \"%s\", aborting\n"),
                   argumentParser.long_option()));

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

const bool
init_threadPool()
{
  RPG_TRACE(ACE_TEXT("::init_threadPool"));

  ACE_TP_Reactor* threadpool_reactor = NULL;
  ACE_NEW_RETURN(threadpool_reactor,
                 ACE_TP_Reactor(),
                 false);
  ACE_Reactor* new_reactor = NULL;
  ACE_NEW_RETURN(new_reactor,
                 ACE_Reactor(threadpool_reactor, 1), // delete in dtor
                 false);
  // make this the "default" reactor...
  ACE_Reactor::instance(new_reactor, 1); // delete in dtor

  return true;
}

static
ACE_THR_FUNC_RETURN
tp_worker_func(void* args_in)
{
  RPG_TRACE(ACE_TEXT("::tp_worker_func"));

  ACE_UNUSED_ARG(args_in);

  // *NOTE*: asynchronous writing to a closed socket triggers the
  // SIGPIPE signal (default action: abort).
  // --> as this doesn't use select(), guard against this (ignore the signal)
  ACE_Sig_Action no_sigpipe(static_cast<ACE_SignalHandler> (SIG_IGN));
  ACE_Sig_Action original_action;
  no_sigpipe.register_action(SIGPIPE, &original_action);

  while (!ACE_Reactor::event_loop_done())
  {
    // block and wait for an event...
    if (ACE_Reactor::instance()->handle_events(NULL) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("(%t) error handling events: \"%m\"\n")));
  } // end WHILE

  // clean up
  no_sigpipe.restore_action(SIGPIPE, original_action);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

static
ACE_THR_FUNC_RETURN
reactor_worker_func(void* args_in)
{
  RPG_TRACE(ACE_TEXT("::reactor_worker_func"));

  ACE_UNUSED_ARG(args_in);

  // *NOTE*: asynchronous writing to a closed socket triggers the
  // SIGPIPE signal (default action: abort).
  // --> as this doesn't use select(), guard against this (ignore the signal)
  ACE_Sig_Action no_sigpipe(static_cast<ACE_SignalHandler> (SIG_IGN));
  ACE_Sig_Action original_action;
  no_sigpipe.register_action(SIGPIPE, &original_action);

  // assume ownership over the reactor...
  ACE_Reactor::instance()->owner(ACE_OS::thr_self(),
                                 NULL);

  if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", aborting\n")));
  } // end IF

  // clean up
  no_sigpipe.restore_action(SIGPIPE, original_action);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

const bool
do_initAudio(const SDL_audio_config_t& audioConfig_in)
{
  RPG_TRACE(ACE_TEXT("::do_initAudio"));

  // init SDL audio handling
//   SDL_AudioSpec wanted;
//   wanted.freq = audioConfig_in.frequency;
//   wanted.format = audioConfig_in.format;
//   wanted.channels = audioConfig_in.channels;
//   wanted.samples = audioConfig_in.samples;
// //   wanted.callback = fill_audio;
// //   wanted.userdata = NULL;
//
//   // Open the audio device, forcing the desired format
//   if (SDL_OpenAudio(&wanted, NULL) < 0)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_OpenAudio(): \"%s\", aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
  if (Mix_OpenAudio(audioConfig_in.frequency,
                    audioConfig_in.format,
                    audioConfig_in.channels,
                    audioConfig_in.samples) < 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_OpenAudio(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
//   Mix_AllocateChannels(4);
  SDL_audio_config_t obtained;
  obtained.frequency = 0;
  obtained.format = 0;
  obtained.channels = 0;
  obtained.samples = 0;
  if (Mix_QuerySpec(&obtained.frequency,
                    &obtained.format,
                    &obtained.channels) == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_QuerySpec(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  char driver[MAXPATHLEN];
  if (!SDL_AudioDriverName(driver, sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AudioDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF

  // initialize audioCD playing
  if (SDL_CDNumDrives() <= 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CDNumDrives(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  SDL_CD* cdrom = NULL;
  cdrom = SDL_CDOpen(0); // open default drive

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("*** audio capabilities (driver: \"%s\") ***\nfrequency: %d\nformat: %u\nchannels: %u\nCD [id, status]: \"%s\" [%d, %d]\n"),
             driver,
             obtained.frequency,
             static_cast<unsigned long> (obtained.format),
             static_cast<unsigned long> (obtained.channels),
             SDL_CDName(0),
             cdrom->id,
             cdrom->status));

  return true;
}

const bool
do_initGUI(const std::string& graphicsDirectory_in,
           const std::string& UIfile_in,
           GTK_cb_data_t& userData_in,
           const SDL_video_config_t& videoConfig_in)
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
  if (!RPG_Common_File_Tools::isReadable(UIfile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("UI definition file \"%s\" doesn't exist, aborting\n"),
               UIfile_in.c_str()));

    return false;
  } // end IF
  ACE_ASSERT(userData_in.xml == NULL);

  // step1: load widget tree
  userData_in.xml = glade_xml_new(UIfile_in.c_str(), // definition file
                                  NULL,              // root widget --> construct all
                                  NULL);             // domain
  if (!userData_in.xml)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_new(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // step2: retrieve dialog handles
  main_dialog = GTK_WIDGET(glade_xml_get_widget(userData_in.xml,
                                                RPG_CLIENT_DEF_GNOME_MAINDIALOG_NAME));
  if (!main_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_MAINDIALOG_NAME));

    // clean up
    g_object_unref(G_OBJECT(userData_in.xml));
    userData_in.xml = NULL;

    return false;
  } // end IF

  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(userData_in.xml,
                                                            RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));

    // clean up
    g_object_unref(G_OBJECT(userData_in.xml));
    userData_in.xml = NULL;

    return false;
  } // end IF

  // step3: populate combobox
  GtkComboBox* available_characters = GTK_COMBO_BOX(glade_xml_get_widget(userData_in.xml,
                                                                         RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(available_characters);
  if (!available_characters)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));

    // clean up
    g_object_unref(G_OBJECT(userData_in.xml));
    userData_in.xml = NULL;

    return false;
  } // end IF
  gtk_cell_layout_clear(GTK_CELL_LAYOUT(available_characters));
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  if (!renderer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.xml));
    userData_in.xml = NULL;

    return false;
  } // end IF
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(available_characters), renderer,
                             TRUE); // expand ?
//   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(available_characters), renderer,
//                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(available_characters), renderer,
                                 ACE_TEXT_ALWAYS_CHAR("text"), 0,
                                 NULL);
  GtkListStore* list = gtk_list_store_new(1,
                                          G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_list_store_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.xml));
    userData_in.xml = NULL;

    return false;
  } // end IF
  ::load_character_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
                            list);
  gtk_combo_box_set_model(available_characters,
                          GTK_TREE_MODEL(list));
  g_object_unref(G_OBJECT(list));
  gtk_widget_set_sensitive(GTK_WIDGET(available_characters),
                           (g_list_length(gtk_container_get_children(GTK_CONTAINER(available_characters))) ? TRUE
                                                                                                           : FALSE));

  // step4a: connect default signals
  gpointer userData_p = const_cast<GTK_cb_data_t*> (&userData_in);
  g_signal_connect(main_dialog,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(quit_activated_GTK_cb),
//                    G_CALLBACK(gtk_widget_destroyed),
//                    &main_dialog,
                   userData_p);
  g_signal_connect(about_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &about_dialog);

   // step4b: connect custom signals
  GtkButton* button = NULL;
  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("create")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(create_character_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("create_character_activated_GTK_cb"),
//                                 G_CALLBACK(create_character_activated_GTK_cb),
//                                 userData_p);

  GtkFileChooser* filechooser = NULL;
  filechooser = GTK_FILE_CHOOSER(glade_xml_get_widget(userData_in.xml,
                                                      RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser);
  g_signal_connect(filechooser,
                   ACE_TEXT_ALWAYS_CHAR("file-activated"),
                   G_CALLBACK(character_file_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("character_file_activated_GTK_cb"),
//                                 G_CALLBACK(character_file_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(load_character_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("load_character_activated_GTK_cb"),
//                                 G_CALLBACK(load_character_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(save_character_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("save_character_activated_GTK_cb"),
//                                 G_CALLBACK(save_character_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(join_game_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("join_game_activated_GTK_cb"),
//                                 G_CALLBACK(join_game_activated_GTK_cb),
//                                 userData_p);

  GtkComboBox* combobox = NULL;
  combobox = GTK_COMBO_BOX(glade_xml_get_widget(userData_in.xml,
                                                RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(characters_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("characters_activated_GTK_cb"),
//                                 G_CALLBACK(characters_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("refresh")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(characters_refresh_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("characters_refresh_activated_GTK_cb"),
//                                 G_CALLBACK(characters_refresh_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("quit")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(quit_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("quit_activated_GTK_cb"),
//                                 G_CALLBACK(quit_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("about")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(about_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("about_activated_GTK_cb"),
//                                 G_CALLBACK(about_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("properties")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(properties_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("properties_activated_GTK_cb"),
//                                 G_CALLBACK(properties_activated_GTK_cb),
//                                 userData_p);

  // step5: auto-connect signals/slots
//   glade_xml_signal_autoconnect(xml);

//   // step6: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(const_cast<GtkWidget*> (//                                                                parentWidget_in)));

  // step6: draw it
  gtk_widget_show_all(main_dialog);

  // step7: activate first repository entry (if any)
  if (gtk_widget_is_sensitive(GTK_WIDGET(available_characters)))
    gtk_combo_box_set_active(available_characters, 0);

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
  SDL_WM_SetCaption(ACE_TEXT_ALWAYS_CHAR(RPG_PACKAGE_STRING),  // window caption
                    ACE_TEXT_ALWAYS_CHAR(RPG_PACKAGE_STRING)); // icon caption
  // set window icon
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_WM_ICON;
  RPG_Graphics_t icon_graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT(icon_graphic.type.image == IMAGE_WM_ICON);
  std::string path = graphicsDirectory_in;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += RPG_GRAPHICS_TILE_DEF_IMAGES_SUB;
  path += ACE_DIRECTORY_SEPARATOR_STR;
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
                                                          videoConfig_in.fullScreen);

  return (userData_in.screen != NULL);
}

const bool
do_runIntro(SDL_Surface* screen_in)
{
  RPG_TRACE(ACE_TEXT("::do_runIntro"));

  // step1: play intro music
  RPG_Sound_Common_Tools::play(EVENT_MAIN_TITLE);

  // step2: show start logo
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_INTRO_MAIN;
  SDL_Surface* logo = RPG_Graphics_Common_Tools::loadGraphic(type);
  if (!logo)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::typeToString(type).c_str()));

    return false;
  } // end IF
  // *TODO* stretch this image fullscreen
  // center logo image
  RPG_Graphics_Surface::put((screen_in->w - logo->w) / 2, // location x
                            (screen_in->h - logo->h) / 2, // location y
                            *logo,
                            screen_in);
//   SDL_FreeSurface(logo);
  RPG_Graphics_Common_Tools::fade(true,                                // fade in
                                  5.0,                                 // interval
                                  RPG_Graphics_SDL_Tools::CLR32_BLACK, // fade from black
                                  screen_in);                          // screen
  SDL_Event event;
  do_SDL_waitForInput(10,     // wait 10 seconds max
                      event);
//   do_handleSDLEvent(event);
  RPG_Graphics_Common_Tools::fade(false,                               // fade out
                                  3.0,                                 // interval
                                  RPG_Graphics_SDL_Tools::CLR32_BLACK, // fade to black
                                  screen_in);                          // screen

  return true;
}

void
do_work(const RPG_Client_Config& config_in,
        const std::string& schemaRepository_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0a: init RPG engine
  RPG_Engine_Common_Tools::init(config_in.magic_dictionary,
                                config_in.item_dictionary,
                                config_in.monster_dictionary);
  RPG_Sound_Common_Tools::initStringConversionTables();
  RPG_Graphics_Common_Tools::initStringConversionTables();

  // step0b: (if necessary) init the TP_Reactor
  if (config_in.num_threadpool_threads)
  {
    if (!init_threadPool())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_threadPool(), aborting\n")));

      return;
    } // end IF
  } // end IF

  // step1: init audio
  RPG_Sound_Common_Tools::init(config_in.sound_directory,
                               config_in.sound_cache_size);
  //  init sound dictionary
  try
  {
    RPG_SOUND_DICTIONARY_SINGLETON::instance()->init(config_in.sound_dictionary);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Sound_Dictionary::init, returning\n")));

    return;
  }
  if (!do_initAudio(config_in.audio_config))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize audio, aborting\n")));

    return;
  } // end IF

  // step2: init UI
  // init graphics dictionary
  try
  {
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(config_in.graphics_dictionary);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_Dictionary::init, returning\n")));

    return;
  }

  GTK_cb_data_t userData;
//   userData.lock;
  userData.hover_time            = 0;
  userData.gtk_time              = 0;
  userData.gtk_main_quit_invoked = false;
  userData.xml                   = NULL;
  userData.screen                = NULL;
  userData.event_timer           = 0;
  userData.previous_window       = NULL;
  userData.main_window           = NULL;
  userData.map_window            = NULL;
  userData.plan.size_x           = 0;
  userData.plan.size_y           = 0;
//   userData.plan.unmapped;
//   userData.plan.walls;
//   userData.plan.doors;
//   userData.seed_points;
  userData.schemaRepository      = schemaRepository_in;
//   userData.player;

  GDK_THREADS_ENTER();
  if (!do_initGUI(config_in.graphics_directory, // graphics directory
                  config_in.glade_file,         // glade file
                  userData,                     // GTK cb data
                  config_in.video_config))      // SDL video config
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize video, aborting\n")));

    GDK_THREADS_LEAVE();

    return;
  } // end IF
  GDK_THREADS_LEAVE();
  ACE_ASSERT(main_dialog);
  ACE_ASSERT(userData.screen);

  // ***** mouse setup *****
  SDL_WarpMouse((userData.screen->w / 2),
                (userData.screen->h / 2));

  RPG_Graphics_Common_Tools::init(config_in.graphics_directory,
                                  config_in.graphics_cache_size);

//   // step3: run intro
//   if (!do_runIntro())
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to run intro, aborting\n")));
//
//     return;
//   } // end IF

  // step4: setup main "window"
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = RPG_CLIENT_DEF_GRAPHICS_WINDOWSTYLE_TYPE;
  std::string title = RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE;
  RPG_Client_WindowMain mainWindow(RPG_Graphics_WindowSize_t(userData.screen->w,
                                                             userData.screen->h), // size
                                   type,                                          // interface elements
                                   title,                                         // title (== caption)
                                   FONT_MAIN_LARGE);                              // title font
  mainWindow.setScreen(userData.screen);
  mainWindow.init();
  mainWindow.draw();
  mainWindow.refresh();

  // step4b: set default cursor
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set(CURSOR_NORMAL);

  // step5: setup level "window"
  RPG_Client_WindowLevel mapWindow(mainWindow); // parent
  mapWindow.setScreen(userData.screen);

  // step5a: setup style
  RPG_Graphics_MapStyle_t mapStyle;
  mapStyle.floor_style = RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE;
  mapStyle.wall_style = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE;
  mapStyle.half_height_walls = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF;
  mapStyle.door_style = RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE;

  // step5b: setup map
  userData.plan.size_x = 0;
  userData.plan.size_y = 0;
  userData.plan.unmapped.clear();
  userData.plan.walls.clear();
  userData.plan.doors.clear();
  userData.seed_points.clear();
  if (config_in.map_file.empty())
    RPG_Map_Common_Tools::createFloorPlan(config_in.map_config.map_size_x,
                                          config_in.map_config.map_size_y,
                                          config_in.map_config.num_areas,
                                          config_in.map_config.square_rooms,
                                          config_in.map_config.maximize_rooms,
                                          config_in.map_config.min_room_size,
                                          config_in.map_config.doors,
                                          config_in.map_config.corridors,
                                          true, // *NOTE*: currently, doors fill one position
                                          config_in.map_config.max_num_doors_per_room,
                                          userData.seed_points,
                                          userData.plan);
  else
  {
    if (!RPG_Map_Common_Tools::load(config_in.map_file,
                                    userData.seed_points,
                                    userData.plan))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Map_Common_Tools::load(\"%s\"), aborting\n"),
                 config_in.map_file.c_str()));

      return;
    } // end IF
  } // end ELSE

//   RPG_Map_Common_Tools::displayFloorPlan(userData.seedPoints,
//                                          userData.plan);

  mapWindow.init(mapStyle,
                 userData.plan);
  // refresh screen
  try
  {
    mapWindow.draw();
    mapWindow.refresh();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
  }

  // start timer (triggers hover- and processes GTK events)
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

    return;
  } // end IF

  // step6: setup event loops
  // - perform (signal handling, socket I/O, ...) --> ACE_Reactor
  // - UI events --> GTK main loop [--> SDL event handler]

//   // *NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);

//   // setup dispatch of SDL events from the GTK (== "main") thread
//   userData.previous_window = NULL;
//   userData.main_window = &mainWindow;
//   userData.map_window = &mapWindow;
//   GDK_THREADS_ENTER();
//   guint SDLEventHandlerID = gtk_idle_add(do_SDLEventLoop_GTK_cb,
//                                          &userData);
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("installed SDL event handler (ID: %u)...\n"),
//              SDLEventHandlerID));
//   // setup dispatch of a hook when gkt_main_quit() is invoked
//   guint quitHandlerID = gtk_quit_add(0,                   // main level: 0 (current)
//                                      gtk_quit_handler_cb, // handler
//                                      &userData);          // user data
//   GDK_THREADS_LEAVE();
//   if (quitHandlerID == 0)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to gtk_quit_add(), aborting\n")));
//
//     return;
//   } // end IF
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("installed GTK quit handler (ID: %u)...\n"),
//              quitHandlerID));

  // setup dispatch of network events
  // *NOTE*: if we use a thread pool, we invoke a different function...
  // *CONSIDER*: potential source for races here
  // --> network connection events arrive before dispatch of events begins
  int grp_id = -1;
  if (config_in.num_threadpool_threads)
  {
    // start a (group of) worker(s)...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(config_in.num_threadpool_threads, // # threads
                                                     ::tp_worker_func,            // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end IF
  else
  {
    // start a worker...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(1,                           // # threads
                                                     ::reactor_worker_func,       // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end ELSE
  if (grp_id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
               (config_in.num_threadpool_threads ? config_in.num_threadpool_threads : 1)));

    GDK_THREADS_LEAVE();

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started group (ID: %u) of %u worker thread(s)...\n"),
             grp_id,
             (config_in.num_threadpool_threads ? config_in.num_threadpool_threads : 1)));

  // step7: dispatch SDL (and GTK) events
//   gtk_main();
  SDL_Event event;
  bool done = false;
  RPG_Graphics_IWindow* window = NULL;
  RPG_Graphics_IWindow* previous_window = NULL;
  bool need_redraw = false;
  RPG_Graphics_Position_t mouse_position;
  do
  {
    window = NULL;
    need_redraw = false;
    mouse_position = std::make_pair(0, 0);

    // step1: get pending event
    if (SDL_WaitEvent(&event) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

    // if necessary, reset hover_time
    if ((event.type != RPG_GRAPHICS_SDL_HOVEREVENT) &&
        (event.type != RPG_CLIENT_SDL_GTKEVENT))
    {
      // synch access
      ACE_Guard<ACE_Thread_Mutex> aGuard(userData.lock);

      userData.hover_time = 0;
    } // end IF

    switch (event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_m:
          {
            std::string dump_path = RPG_MAP_DUMP_DIR;
            dump_path += ACE_DIRECTORY_SEPARATOR_STR;
            dump_path += ACE_TEXT("map.txt");
            if (!RPG_Map_Common_Tools::save(dump_path,            // file
                                            userData.seed_points, // seed points
                                            userData.plan))       // plan
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), aborting\n"),
                         dump_path.c_str()));

              return;
            } // end IF

            break;
          }
          case SDLK_q:
          {
            // finished event processing
            done = true;

            break;
          }
          default:
          {
            break;
          }
        } // end SWITCH

        if (done)
          break; // leave
        // *WARNING*: falls through !
      }
      case SDL_ACTIVEEVENT:
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
      {
        // find window
        switch (event.type)
        {
          case SDL_MOUSEMOTION:
            mouse_position = std::make_pair(event.motion.x,
                                            event.motion.y); break;
          case SDL_MOUSEBUTTONDOWN:
            mouse_position = std::make_pair(event.button.x,
                                            event.button.y); break;
          default:
          {
            int x,y;
            SDL_GetMouseState(&x, &y);
            mouse_position = std::make_pair(x, y);

            break;
          }
        } // end SWITCH
        window = mainWindow.getWindow(mouse_position);
        ACE_ASSERT(window);

        // notify previously "active" window upon losing "focus"
        if (event.type == SDL_MOUSEMOTION)
        {
          if (previous_window &&
//               (previous_window != mainWindow)
              (previous_window != window))
          {
            event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
            try
            {
              previous_window->handleEvent(event,
                                           previous_window,
                                           need_redraw);
            }
            catch (...)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }
            event.type = SDL_MOUSEMOTION;
          } // end IF
        } // end IF
        // remember last "active" window
        previous_window = window;

        // notify "active" window
        try
        {
          window->handleEvent(event,
                              window,
                              need_redraw);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
        }

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
      case RPG_CLIENT_SDL_TIMEREVENT:
      default:
      {

        break;
      }
    } // end SWITCH

    // redraw map ?
    if (need_redraw)
    {
      try
      {
        mapWindow.draw();
        mapWindow.refresh();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
      }
    } // end IF

    // redraw cursor ?
    switch (event.type)
    {
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
      {
        // map hasn't changed --> no need to redraw
        if (!need_redraw)
          break;

        // *WARNING*: falls through !
      }
      case SDL_MOUSEMOTION:
      case RPG_GRAPHICS_SDL_HOVEREVENT:
      {
        // map has changed, cursor MAY have been drawn over...
        // --> redraw cursor
        SDL_Rect dirtyRegion;
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put(mouse_position.first,
                                                               mouse_position.second,
                                                               userData.screen,
                                                               dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     userData.screen);

        break;
      }
      default:
      {
        break;
      }
    } // end SWITCH
  } while (!done);

  // step8: clean up
  if (!SDL_RemoveTimer(userData.event_timer))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF

  // done handling UI events

  // stop reactor
  if (ACE_Reactor::instance()->end_event_loop() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));

  // ... wait for the reactor worker(s) to join
  ACE_Thread_Manager::instance()->wait_grp(grp_id);

  // no more data will arrive from here on...

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseIniFile(const std::string& iniFilename_in,
                RPG_Client_Config& config_out)
{
  RPG_TRACE(ACE_TEXT("::do_parseIniFile"));

  // init return value(s)
  config_out.audio_config.frequency = RPG_CLIENT_DEF_AUDIO_FREQUENCY;
  config_out.audio_config.format = RPG_CLIENT_DEF_AUDIO_FORMAT;
  config_out.audio_config.channels = RPG_CLIENT_DEF_AUDIO_CHANNELS;
  config_out.audio_config.samples = RPG_CLIENT_DEF_AUDIO_SAMPLES;

  config_out.video_config.screen_width = RPG_CLIENT_DEF_VIDEO_W;
  config_out.video_config.screen_height = RPG_CLIENT_DEF_VIDEO_H;
  config_out.video_config.screen_colordepth = RPG_CLIENT_DEF_VIDEO_BPP;
  config_out.video_config.fullScreen = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  config_out.video_config.doubleBuffer = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;

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
               iniFilename_in.c_str()));

    return;
  } // end IF

  // find/open "client" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               RPG_CLIENT_CNF_CLIENT_SECTION_HEADER,
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               RPG_CLIENT_CNF_CLIENT_SECTION_HEADER));

    return;
  } // end IF

  // import values...
  int val_index = 0;
  ACE_TString val_name, val_value;
  ACE_Configuration::VALUETYPE val_type;
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
                 val_name.c_str()));

      return;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    // *TODO*: move these strings...
    if (val_name == ACE_TEXT("audio_frequency"))
    {
      config_out.audio_config.frequency = ::atoi(val_value.c_str());
    }
//     else if (val_name == ACE_TEXT("audio_format"))
//     {
//       audioConfig_out.format = ::atoi(val_value.c_str());
//     }
    else if (val_name == ACE_TEXT("audio_channels"))
    {
      config_out.audio_config.channels = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("audio_samples"))
    {
      config_out.audio_config.samples = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_width"))
    {
      config_out.video_config.screen_width = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_height"))
    {
      config_out.video_config.screen_height = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_colordepth"))
    {
      config_out.video_config.screen_colordepth = ::atoi(val_value.c_str());
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 val_name.c_str()));
    } // end ELSE

    ++val_index;
  } // end WHILE

//   // find/open "connection" section...
//   if (config_heap.open_section(config_heap.root_section(),
//                                RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER,
//                                0, // MUST exist !
//                                section_key) != 0)
//   {
//     ACE_ERROR((LM_ERROR,
//                ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
//                RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER));
//
//     return;
//   } // end IF
//
//   // import values...
//   val_index = 0;
//   while (config_heap.enumerate_values(section_key,
//                                       val_index,
//                                       val_name,
//                                       val_type) == 0)
//   {
//     if (config_heap.get_string_value(section_key,
//                                      val_name.c_str(),
//                                      val_value))
//     {
//       ACE_ERROR((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
//                  val_name.c_str()));
//
//       return;
//     } // end IF
//
// //     ACE_DEBUG((LM_DEBUG,
// //                ACE_TEXT("enumerated %s, type %d\n"),
// //                val_name.c_str(),
// //                val_type));
//
//     // *TODO*: move these strings...
//     if (val_name == ACE_TEXT("server"))
//     {
//       serverHostname_out = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("port"))
//     {
//       serverPortNumber_out = ::atoi(val_value.c_str());
//     }
//     else if (val_name == ACE_TEXT("password"))
//     {
//       loginOptions_out.password = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("nick"))
//     {
//       loginOptions_out.nick = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("user"))
//     {
//       loginOptions_out.user.username = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("realname"))
//     {
//       loginOptions_out.user.realname = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("channel"))
//     {
//       loginOptions_out.channel = val_value.c_str();
//     }
//     else
//     {
//       ACE_ERROR((LM_ERROR,
//                  ACE_TEXT("unexpected key \"%s\", continuing\n"),
//                  val_name.c_str()));
//     } // end ELSE
//
//     ++val_index;
//   } // end WHILE

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("imported \"%s\"...\n"),
//              configFilename_in.c_str()));
}

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in << ACE_TEXT(" : ") << RPG_VERSION << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  version_number << ACE::major_version();
  version_number << ACE_TEXT(".");
  version_number << ACE::minor_version();
  if (ACE::beta_version())
  {
    version_number << ACE_TEXT(".");
    version_number << ACE::beta_version();
  } // end IF

  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
{
//   RPG_TRACE(ACE_TEXT("::main"));

  // step1: init ACE
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  // step1 init/validate configuration
  std::string base_data_path;
#ifdef DATADIR
  base_data_path = DATADIR;
#else
  base_data_path = RPG_Common_File_Tools::getWorkingDirectory(); // fallback
#endif // #ifdef DATADIR

  // step1a: process commandline arguments
  std::string iniFile = base_data_path;
  iniFile += ACE_DIRECTORY_SEPARATOR_STR;
  iniFile += RPG_COMMON_DEF_CONFIG_SUB;
  iniFile += ACE_DIRECTORY_SEPARATOR_STR;
  iniFile += RPG_CLIENT_DEF_INI_FILE;

  std::string monsterDictionary = base_data_path;
  monsterDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  monsterDictionary += RPG_COMMON_DEF_CONFIG_SUB;
  monsterDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  monsterDictionary += RPG_MONSTER_DEF_DICTIONARY_FILE;

  std::string graphicsDictionary = base_data_path;
  graphicsDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  graphicsDictionary += RPG_COMMON_DEF_CONFIG_SUB;
  graphicsDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  graphicsDictionary += RPG_GRAPHICS_DEF_DICTIONARY_FILE;

  std::string itemDictionary = base_data_path;
  itemDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionary += RPG_COMMON_DEF_CONFIG_SUB;
  itemDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  itemDictionary += RPG_ITEM_DEF_DICTIONARY_FILE;

  std::string magicDictionary = base_data_path;
  magicDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionary += RPG_COMMON_DEF_CONFIG_SUB;
  magicDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  magicDictionary += RPG_MAGIC_DEF_DICTIONARY_FILE;

  std::string soundDictionary = base_data_path;
  soundDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  soundDictionary += RPG_COMMON_DEF_CONFIG_SUB;
  soundDictionary += ACE_DIRECTORY_SEPARATOR_STR;
  soundDictionary += RPG_SOUND_DEF_DICTIONARY_FILE;

  std::string UIfile = base_data_path;
  UIfile += ACE_DIRECTORY_SEPARATOR_STR;
  UIfile += RPG_COMMON_DEF_CONFIG_SUB;
  UIfile += ACE_DIRECTORY_SEPARATOR_STR;
  UIfile += RPG_CLIENT_DEF_GNOME_UI_FILE;

  std::string schemaRepository = base_data_path;
  schemaRepository += ACE_DIRECTORY_SEPARATOR_STR;
  schemaRepository += RPG_COMMON_DEF_CONFIG_SUB;

  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schemaRepository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               schemaRepository.c_str()));

    // try fallback
    schemaRepository.clear();
  } // end IF

  std::string floorPlan;
  bool logToFile                     = false;
  bool traceInformation              = false;
  bool printVersionAndExit           = false;
  unsigned long numThreadPoolThreads = (RPG_CLIENT_DEF_ACE_USES_TP ? RPG_CLIENT_DEF_ACE_NUM_TP_THREADS : 0);
  if (!(process_arguments(argc_in,
                          argv_in,
                          iniFile,
                          monsterDictionary,
                          floorPlan,
                          graphicsDictionary,
                          itemDictionary,
                          logToFile,
                          magicDictionary,
                          soundDictionary,
                          traceInformation,
                          UIfile,
                          printVersionAndExit,
                          numThreadPoolThreads)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!RPG_Common_File_Tools::isReadable(iniFile) ||
      !RPG_Common_File_Tools::isReadable(monsterDictionary) ||
      !RPG_Common_File_Tools::isReadable(graphicsDictionary) ||
      !RPG_Common_File_Tools::isReadable(itemDictionary) ||
      !RPG_Common_File_Tools::isReadable(magicDictionary) ||
      !RPG_Common_File_Tools::isReadable(soundDictionary) ||
      !RPG_Common_File_Tools::isReadable(UIfile) ||
      !RPG_Common_File_Tools::isReadable(floorPlan))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step1c: set correct trace level
  // step2c: set correct verbosity
  u_long process_priority_mask = (LM_SHUTDOWN |
//                                   LM_TRACE |  // <-- DISABLE trace messages !
//                                   LM_DEBUG |  // <-- DISABLE ACE_DEBUG messages !
                                  LM_INFO |
                                  LM_NOTICE |
                                  LM_WARNING |
                                  LM_STARTUP |
                                  LM_ERROR |
                                  LM_CRITICAL |
                                  LM_ALERT |
                                  LM_EMERGENCY);
  if (traceInformation)
  {
    process_priority_mask |= LM_DEBUG;

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF
  // set process priority mask...
  ACE_LOG_MSG->priority_mask(process_priority_mask,
                             ACE_Log_Msg::PROCESS);

  // step1db: init configuration object
  RPG_Client_Config config;

  // *** reactor ***
  config.num_threadpool_threads            = numThreadPoolThreads;

  // *** UI ***
  config.glade_file                        = UIfile;
//   config.gtk_cb_data                       = userData;

  // *** sound ***
  config.audio_config.frequency            = RPG_CLIENT_DEF_AUDIO_FREQUENCY;
  config.audio_config.format               = RPG_CLIENT_DEF_AUDIO_FORMAT;
  config.audio_config.channels             = RPG_CLIENT_DEF_AUDIO_CHANNELS;
  config.audio_config.samples              = RPG_CLIENT_DEF_AUDIO_SAMPLES;
  config.sound_directory                   = base_data_path;
  config.sound_directory                   += ACE_DIRECTORY_SEPARATOR_STR;
  config.sound_directory                   += RPG_COMMON_DEF_DATA_SUB;
  config.sound_directory                   += ACE_DIRECTORY_SEPARATOR_STR;
  config.sound_directory                   += RPG_SOUND_DEF_DATA_SUB;
  config.sound_cache_size                  = RPG_CLIENT_DEF_SOUND_CACHESIZE;
  config.sound_dictionary                  = soundDictionary;

  // *** graphics ***
  config.video_config.screen_width         = RPG_CLIENT_DEF_VIDEO_W;
  config.video_config.screen_height        = RPG_CLIENT_DEF_VIDEO_H;
  config.video_config.screen_colordepth    = RPG_CLIENT_DEF_VIDEO_BPP;
  config.video_config.fullScreen           = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  config.video_config.doubleBuffer         = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;
  config.graphics_directory                = base_data_path;
  config.graphics_directory                += ACE_DIRECTORY_SEPARATOR_STR;
  config.graphics_directory                += RPG_COMMON_DEF_DATA_SUB;
  config.graphics_directory                += ACE_DIRECTORY_SEPARATOR_STR;
  config.graphics_directory                += RPG_GRAPHICS_DEF_DATA_SUB;
  config.graphics_cache_size               = RPG_CLIENT_DEF_GRAPHICS_CACHESIZE;
  config.graphics_dictionary               = graphicsDictionary;

  // *** magic ***
  config.magic_dictionary                  = magicDictionary;

  // *** item ***
  config.item_dictionary                  = itemDictionary;

  // *** monster ***
  config.monster_dictionary               = monsterDictionary;

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
  config.map_file                          = floorPlan;

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

  // step1de: parse .ini file (if any)
  if (!iniFile.empty())
    do_parseIniFile(iniFile,
                    config);

  // step2a: init SDL
  if (SDL_Init(SDL_INIT_TIMER | // timers
               SDL_INIT_AUDIO |
               SDL_INIT_VIDEO |
               SDL_INIT_CDROM | // audioCD playback
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

  // step2b: init GLIB / G(D|T)K[+] / GNOME
  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(&argc_in,
           &argv_in);
//   GnomeClient* gnomeSession = NULL;
//   gnomeSession = gnome_client_new();
//   ACE_ASSERT(gnomeSession);
//   gnome_client_set_program(gnomeSession, ACE::basename(argv_in[0]));
  GnomeProgram* gnomeProgram = NULL;
  gnomeProgram = gnome_program_init(RPG_CLIENT_DEF_GNOME_APPLICATION_ID,  // app ID
//                                     ACE_TEXT_ALWAYS_CHAR(VERSION),     // version
                                    ACE_TEXT_ALWAYS_CHAR(RPG_VERSION),    // version
                                    LIBGNOMEUI_MODULE,                    // module info
                                    argc_in,                              // cmdline
                                    argv_in,                              // cmdline
                                    NULL);                                // property name(s)
  ACE_ASSERT(gnomeProgram);

  ACE_High_Res_Timer timer;
  timer.start();
  // step3: do actual work
  do_work(config,
          schemaRepository);
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

  // step4: clean up
  TTF_Quit();
  SDL_Quit();

  // stop profile timer...
  process_profile.stop();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time(elapsed_time) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset(&elapsed_rusage,
                 0,
                 sizeof(ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage(elapsed_rusage);
  ACE_Time_Value user_time(elapsed_rusage.ru_utime);
  ACE_Time_Value system_time(elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  RPG_Common_Tools::period2String(user_time,
                                  user_time_string);
  RPG_Common_Tools::period2String(system_time,
                                  system_time_string);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str(),
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

  // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
