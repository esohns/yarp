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

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/POSIX_Proactor.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Version.h"

#if defined (VALGRIND_USE)
#include "valgrind/valgrind.h"
#endif // VALGRIND_USE

#include "common_file_tools.h"
#include "common_os_tools.h"

#include "common_event_tools.h"

#include "common_logger_queue.h"
#include "common_log_tools.h"

#include "common_parser_common.h"

#include "common_signal_tools.h"

#include "common_timer_tools.h"

#include "common_ui_defines.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager.h"
#include "common_ui_gtk_manager_common.h"

#include "stream_allocatorheap.h"

#include "stream_stat_common.h"

#include "net_common_tools.h"
#include "net_configuration.h"
#include "net_connection_manager.h"

#include "net_client_defines.h"

#include "net_server_defines.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"

#include "rpg_magic_defines.h"

#include "rpg_item_defines.h"

#include "rpg_player_defines.h"

#include "rpg_monster_defines.h"

#include "rpg_net_common.h"
#include "rpg_net_defines.h"

#include "rpg_net_protocol_listener.h"
#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_messagehandler.h"

#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_sound_common_tools.h"
#include "rpg_sound_defines.h"

#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
#include "rpg_client_engine.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_network_manager.h"
#include "rpg_client_window_main.h"

#include "net_callbacks.h"
#include "net_client_common.h"
#include "net_client_eventhandler.h"
#include "net_client_signalhandler.h"
#include "net_defines.h"

// ******* WORKAROUND *************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE* __cdecl __iob_func (void)
{
  return _iob;
}
#endif // ACE_WIN32 || ACE_WIN64
// ********************************

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_NET_CONFIGURATION_SUBDIRECTORY),
                                                          true);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:") << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]] : UI file [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [VALUE]   : peer ping interval (ms) [")
            << NET_CLIENT_DEF_SERVER_PING_INTERVAL_MS
            << ACE_TEXT_ALWAYS_CHAR ("] {0 --> OFF})")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l           : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m           : receive uni/multi/broadcast UDP [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-n [STRING]  : network interface [\"")
            << ACE_TEXT_ALWAYS_CHAR (RPG_NET_DEFAULT_NETWORK_INTERFACE)
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  // *TODO*: this doesn't really make sense (yet)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o           : use loopback [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r           : use reactor [")
            << RPG_ENGINE_USES_REACTOR
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [VALUE]   : statistics reporting interval (second(s)) [")
            << RPG_NET_DEFAULT_STATISTICS_REPORTING_INTERVAL
            << ACE_TEXT_ALWAYS_CHAR ("] {0 --> OFF})")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t           : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v           : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x [VALUE]   : #dispatch threads [")
            << (RPG_ENGINE_USES_REACTOR ? NET_CLIENT_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS
                                        : NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned int& peerPingInterval_out,
                     bool& logToFile_out,
                     bool& useUDP_out,
                     std::string& networkInterface_out,
                     bool& useLoopback_out,
                     bool& useReactor_out,
                     unsigned int& statisticsReportingInterval_out,
                     bool& traceInformation_out,
                     std::string& UIFile_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numDispatchThreads_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_NET_CONFIGURATION_SUBDIRECTORY),
                                                          true);

  // initialize results
  peerPingInterval_out            = NET_CLIENT_DEF_SERVER_PING_INTERVAL_MS;
  logToFile_out                   = false;
  useUDP_out                      = false;
  networkInterface_out            =
    ACE_TEXT_ALWAYS_CHAR (RPG_NET_DEFAULT_NETWORK_INTERFACE);
  useLoopback_out                 = false;
  useReactor_out                  = RPG_ENGINE_USES_REACTOR;
  statisticsReportingInterval_out =
    RPG_NET_DEFAULT_STATISTICS_REPORTING_INTERVAL;
  traceInformation_out            = false;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  UIFile_out                      = path;
  printVersionAndExit_out         = false;
  numDispatchThreads_out          =
    (RPG_ENGINE_USES_REACTOR ? NET_CLIENT_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS
                             : NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS);

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("g::i:k:lmn:ors:tvx:"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'g':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
        if (opt_arg)
          UIFile_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          UIFile_out.clear ();
        break;
      }
      case 'i':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> peerPingInterval_out;
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        useUDP_out = true;
        break;
      }
      case 'n':
      {
        networkInterface_out =
          ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'o':
      {
        useLoopback_out = true;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 's':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> statisticsReportingInterval_out;
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
      case 'x':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> numDispatchThreads_out;
        break;
      }
      // error handling
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%c\", continuing\n"),
                    option));
        break;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_initializeSignals (bool useReactor_in,
                      bool allowUserRuntimeStats_in,
                      ACE_Sig_Set& signals_out,
                      ACE_Sig_Set& ignoredSignals_out)
{
  RPG_TRACE (ACE_TEXT ("::do_initializeSignals"));

  int result = -1;

  // initialize return value(s)
  result = signals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF
  result = ignoredSignals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF

  // *PORTABILITY*: on Windows most signals are not defined,
  // and ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_out.sig_add(SIGSEGV);          // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  signals_out.sig_add (SIGBREAK);          // 21      /* Ctrl-Break sequence */
  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  result = signals_out.fill_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeStats_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
  } // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */

  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  // --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34
//  for (int i = ACE_SIGRTMIN;
//       i <= ACE_SIGRTMAX;
//       i++)
//    signals_out.sig_del (i);

  if (!useReactor_in)
  {
    ACE_POSIX_Proactor* proactor_impl =
        dynamic_cast<ACE_POSIX_Proactor*> (ACE_Proactor::instance ()->implementation ());
    ACE_ASSERT (proactor_impl);
    if (proactor_impl->get_impl_type () == ACE_POSIX_Proactor::PROACTOR_SIG)
      signals_out.sig_del (COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL);
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
}

void
do_work (unsigned int peerPingInterval_in,
         bool useUDP_in,
         const std::string& networkInterface_in,
         bool useLoopback_in,
         bool useReactor_in,
         unsigned int statisticsReportingInterval_in,
         unsigned int numDispatchThreads_in,
         const std::string& UIDefinitionFile_in,
         struct Net_Client_GTK_CBData& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
         ACE_Sig_Set& previousSignalMask_inout)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step-2: initialize timer manager
  Common_Timer_Manager_t* timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  timer_manager_p->initialize (CBData_in.configuration->timer_configuration);
  timer_manager_p->start (NULL);

  std::string monster_dictionary =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  std::string item_dictionary =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true); // configuration-

  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  item_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  std::string magic_dictionary =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  // step-1: initialize engine
  std::vector<std::string> schema_directories_a;
  schema_directories_a.push_back (CBData_in.schemaRepository); // engine-

  std::string schema_path = Common_File_Tools::getWorkingDirectory ();
  schema_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_path += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING);
  schema_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  schema_directories_a.push_back (schema_path);

  schema_path = Common_File_Tools::getWorkingDirectory ();
  schema_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_path += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING);
  schema_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  schema_directories_a.push_back (schema_path);
  RPG_Engine_Common_Tools::initialize (schema_directories_a,
                                       magic_dictionary,
                                       item_dictionary,
                                       monster_dictionary);

  CBData_in.configuration->graphics_dictionary =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  CBData_in.configuration->graphics_dictionary +=
    ACE_DIRECTORY_SEPARATOR_CHAR_A;
  CBData_in.configuration->graphics_dictionary +=
    ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  CBData_in.configuration->audio_configuration.dictionary =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  CBData_in.configuration->audio_configuration.dictionary +=
    ACE_DIRECTORY_SEPARATOR_CHAR_A;
  CBData_in.configuration->audio_configuration.dictionary +=
    ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DICTIONARY_FILE);

  CBData_in.configuration->audio_configuration.repository =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING),
                                                          false); // data-
  CBData_in.configuration->graphics_directory =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          false); // data-
  RPG_Client_Common_Tools::initialize (CBData_in.configuration->input_configuration,
                                       CBData_in.configuration->audio_configuration.SDL_configuration,
                                       CBData_in.configuration->audio_configuration.repository,
                                       CBData_in.configuration->audio_configuration.use_CD,
                                       RPG_SOUND_DEF_CACHESIZE,
                                       CBData_in.configuration->audio_configuration.mute,
                                       CBData_in.configuration->audio_configuration.dictionary,
                                       CBData_in.configuration->graphics_directory,
                                       RPG_GRAPHICS_DEF_CACHESIZE,
                                       CBData_in.configuration->graphics_dictionary,
                                       false); // initialize SDL ?

  CBData_in.configuration->video_configuration.screen_width =
    RPG_CLIENT_VIDEO_DEF_WIDTH;
  CBData_in.configuration->video_configuration.screen_height =
    RPG_CLIENT_VIDEO_DEF_HEIGHT;
  CBData_in.configuration->video_configuration.screen_colordepth =
    RPG_CLIENT_VIDEO_DEF_BPP;
  CBData_in.configuration->video_configuration.double_buffer =
    RPG_CLIENT_VIDEO_DEF_DOUBLEBUFFER;
  CBData_in.configuration->video_configuration.use_OpenGL =
    RPG_CLIENT_VIDEO_DEF_OPENGL;
  CBData_in.configuration->video_configuration.full_screen =
    RPG_CLIENT_VIDEO_DEF_FULLSCREEN;
  CBData_in.configuration->video_configuration.video_driver =
    ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME);

  RPG_Client_Engine client_engine;
  RPG_Engine level_engine;
  level_engine.initialize (&client_engine,   // client engine handle
                           NET_ROLE_CLIENT); // role
  CBData_in.clientEngine = &client_engine;
  CBData_in.levelEngine = &level_engine;

  RPG_Graphics_SDL_Tools::initializeVideo (CBData_in.configuration->video_configuration,                     // configuration
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE), // window/icon caption
                                           CBData_in.screen,                                                 // window surface
                                           CBData_in.renderer,                                               // renderer
                                           CBData_in.GLContext,                                              // OpenGL context
                                           true);                                                            // initialize window ?
  RPG_Graphics_Common_Tools::initialize (CBData_in.configuration->graphics_directory,
                                         RPG_CLIENT_GRAPHICS_DEF_CACHESIZE,
                                         true); // initialize SDL ?
  struct SDL_Rect dirty_region;
  ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->setCursor (CURSOR_NORMAL,
                                                                 dirty_region,
                                                                 true); // locked access ?

  // step4a: setup main "window"
  struct RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = RPG_CLIENT_GRAPHICS_DEF_WINDOWSTYLE_TYPE;
  std::string title = ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);
  SDL_Surface* surface_p = NULL;
#if defined (SDL_USE)
  surface_p = GTKUserData_in.screen;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  surface_p = SDL_GetWindowSurface (CBData_in.screen);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (surface_p);
  RPG_Client_Window_Main main_window (RPG_Graphics_Size_t (surface_p->w,
                                                           surface_p->h), // size
                                      type,                               // interface elements
                                      title,                              // title (== caption)
                                      FONT_MAIN_LARGE);                   // title font
  main_window.initializeSDL (CBData_in.renderer,
                             CBData_in.screen,
                             CBData_in.GLContext);
  main_window.initialize (&client_engine,
                          RPG_CLIENT_WINDOW_DEF_EDGE_AUTOSCROLL,
                          &level_engine,
                          false); // debug ?

  // step3: initialize connection manager
  CBData_in.configuration->protocol_configuration.connectionConfiguration.allocatorConfiguration =
    &CBData_in.allocatorConfiguration;
  CBData_in.configuration->protocol_configuration.connectionConfiguration.messageAllocator =
    &CBData_in.messageAllocator;

  RPG_Net_Protocol_Connection_Manager_t* connection_manager_p =
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  connection_manager_p->initialize (NET_CONNECTION_MAXIMUM_NUMBER_OF_OPEN,
                                    ACE_Time_Value (0, NET_STATISTIC_DEFAULT_VISIT_INTERVAL_MS * 1000));
  struct Net_UserData user_data_s;
  connection_manager_p->set (CBData_in.configuration->protocol_configuration.connectionConfiguration,
                             &user_data_s);

  // step4b: client engine
  RPG_Graphics_IWindowBase* map_window_p = main_window.child (WINDOW_MAP);
  ACE_ASSERT (map_window_p);
  client_engine.initialize (&level_engine,
                            map_window_p,
                            //&UIDefinition_in,
                            false); // debug ?

  // step4c: queue initial drawing
  struct RPG_Client_Action client_action;
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

  // initialize/add entity to the graphics cache
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->initialize (NULL, // screen lock handle
                                                                  map_window_p);
  RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->initialize (NULL, // screen lock handle
                                                                map_window_p);

  // start painting...
  client_engine.start (NULL);

  CBData_in.allocatorConfiguration.defaultBufferSize =
    RPG_NET_PROTOCOL_MAXIMUM_FRAME_SIZE;

  // step0a: initialize stream configuration object
  struct Stream_ModuleConfiguration module_configuration;
  struct RPG_Net_Protocol_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.allocatorConfiguration =
    &CBData_in.allocatorConfiguration;
  modulehandler_configuration.parserConfiguration =
    &CBData_in.configuration->parser_configuration;
  modulehandler_configuration.protocolOptions =
    &CBData_in.configuration->protocol_configuration.protocolOptions;
  struct RPG_Net_Protocol_StreamConfiguration stream_configuration;
  RPG_Net_Protocol_StreamConfiguration_t stream_configuration_2;

  CBData_in.configuration->dispatch_configuration.dispatch =
    (useReactor_in ? COMMON_EVENT_DISPATCH_REACTOR
                   : COMMON_EVENT_DISPATCH_PROACTOR);
  if (useReactor_in)
    CBData_in.configuration->dispatch_configuration.numberOfReactorThreads =
      numDispatchThreads_in;
  else
    CBData_in.configuration->dispatch_configuration.numberOfProactorThreads =
      numDispatchThreads_in;

  Net_Client_EventHandler ui_message_handler (&CBData_in);
  RPG_Net_Protocol_MessageHandler_Module message_handler (NULL,
                                                          ACE_TEXT_ALWAYS_CHAR ("MessageHandler"));
  modulehandler_configuration.subscriber = &ui_message_handler;

  // ************ connection configuration data ************
  CBData_in.configuration->protocol_configuration.connectionConfiguration.streamConfiguration =
    &stream_configuration_2;
  CBData_in.configuration->protocol_configuration.protocolOptions.pingInterval.usec (peerPingInterval_in * 1000);
  CBData_in.configuration->protocol_configuration.protocolOptions.printPongMessages =
    true;
  // ************ socket / stream configuration data ************

  stream_configuration.messageAllocator = &CBData_in.messageAllocator;
  stream_configuration.module =
    (!UIDefinitionFile_in.empty () ? &message_handler
                                   : NULL);
  stream_configuration_2.initialize (module_configuration,
                                     modulehandler_configuration,
                                     stream_configuration);

  // step0b: initialize event dispatch
  if (!Common_Event_Tools::initializeEventDispatch (CBData_in.configuration->dispatch_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    RPG_Engine_Common_Tools::finalize ();
    return;
  } // end IF

  // step1: initialize regular (global) statistics reporting
  Net_StreamStatisticHandler_t statistics_handler (COMMON_STATISTIC_ACTION_REPORT,
                                                   RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance (),
                                                   false);
  long timer_id = -1;
  if (statisticsReportingInterval_in)
  {
    ACE_Event_Handler* handler_p = &statistics_handler;
    ACE_Time_Value interval (statisticsReportingInterval_in, 0);
    timer_id =
      timer_manager_p->schedule (handler_p,                  // event handler
                                 NULL,                       // ACT
                                 COMMON_TIME_NOW + interval, // first wakeup time
                                 interval);                  // interval
    if (timer_id == -1)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));

      // clean up
      timer_manager_p->stop ();
      RPG_Engine_Common_Tools::finalize ();

      return;
    } // end IF
  } // end IF

  // step2: signal handling
  struct Common_EventDispatchState dispatch_state_s;
  CBData_in.configuration->signal_configuration.dispatchState =
    &dispatch_state_s;
  CBData_in.configuration->signal_configuration.mode =
    COMMON_SIGNAL_DISPATCH_SIGNAL;
  CBData_in.configuration->signal_configuration.stopEventDispatchOnShutdown =
    true;
  Net_Client_SignalHandler signal_handler (CBData_in.configuration->dispatch_configuration,
                                           CBData_in.configuration->protocol_configuration.connectionConfiguration);
  signal_handler.initialize (CBData_in.configuration->signal_configuration);
  int result = -1;
  const void* act_p = NULL;
  if (!Common_Signal_Tools::initialize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                        signalSet_in,
                                        ignoredSignalSet_in,
                                        &signal_handler,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeSignals(), returning\n")));

    // clean up
    if (timer_id != -1)
    {
      result = timer_manager_p->cancel (timer_id,
                                        &act_p);
      if (result <= 0)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    timer_id));
    } // end IF
    timer_manager_p->stop ();
    RPG_Engine_Common_Tools::finalize ();

    return;
  } // end IF

  // step4: handle events (signals, incoming connections/data, timers, ...)
  // reactor/proactor event loop:
  // - dispatch connection attempts to acceptor
  // - dispatch socket events
  // timer events:
  // - perform statistics collecting/reporting
  // [GTK events:]
  // - dispatch UI events (if any)

  // step4a: start GTK event loop ?
  if (!UIDefinitionFile_in.empty ())
  {
    Common_UI_GTK_Manager_t* gtk_manager_p =
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
    ACE_ASSERT (gtk_manager_p);
    Common_UI_GTK_State_t& state_r =
      const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR ());
    CBData_in.configuration->gtk_configuration.eventHooks.finiHook = idle_finalize_UI_cb;
    CBData_in.configuration->gtk_configuration.eventHooks.initHook = idle_initialize_client_UI_cb;
    //CBData_in.GTKState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
    CBData_in.UIState = &state_r;
    CBData_in.UIState->builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFile_in, static_cast<GtkBuilder*> (NULL));

    gtk_manager_p->start (NULL);
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));

      // clean up
      if (timer_id != -1)
      {
        result = timer_manager_p->cancel (timer_id,
                                          &act_p);
        if (result <= 0)
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                      timer_id));
      } // end IF
      timer_manager_p->stop ();
      gtk_manager_p->stop (true,
                           false);
      Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                     previousSignalActions_inout,
                                     previousSignalMask_inout);
      RPG_Engine_Common_Tools::finalize ();

      return;
    } // end IF
  } // end IF

  // step4b: initialize worker(s)
  int group_id = -1;
  dispatch_state_s.configuration =
    &CBData_in.configuration->dispatch_configuration;
  if (!Common_Event_Tools::startEventDispatch (dispatch_state_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, returning\n")));

    // clean up
    if (timer_id != -1)
    {
      result = timer_manager_p->cancel (timer_id,
                                        &act_p);
      if (result <= 0)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    timer_id));
    } // end IF
    //		{ // synch access
    //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

    //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
    //					 iterator != CBData_in.event_source_ids.end();
    //					 iterator++)
    //				g_source_remove(*iterator);
    //		} // end lock scope
    if (!UIDefinitionFile_in.empty ())
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop (true,
                                                          false);
    timer_manager_p->stop (true,
                           false);
    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   previousSignalActions_inout,
                                   previousSignalMask_inout);
    RPG_Engine_Common_Tools::finalize ();

    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  // *NOTE*: from this point on, clean up any remote connections !

  // step6b: dispatch SDL events
  union SDL_Event sdl_event;
  bool done = false;
  RPG_Graphics_IWindowBase* window = NULL;
  RPG_Graphics_IWindowBase* previous_window = NULL;
  RPG_Graphics_Position_t mouse_position;
  do
  {
#if defined (SDL_USE)
    sdl_event.type = SDL_NOEVENT;
#elif defined (SDL2_USE)
    sdl_event.type = SDL_FIRSTEVENT;
#elif defined (SDL3_USE)
    sdl_event.type = SDL_EVENT_FIRST;
#endif // SDL_USE || SDL2_USE || SDL3_USE
    window = NULL;

    client_action.command = RPG_CLIENT_COMMAND_INVALID;
    client_action.position = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                             std::numeric_limits<unsigned int>::max ());
    client_action.window = NULL;
    client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
    client_action.entity_id = 0;
    client_action.path.clear ();

    mouse_position = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                     std::numeric_limits<unsigned int>::max ());

    ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));

    // step6a: get next pending event
//     if (SDL_PollEvent(&event) == -1)
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_PollEvent(): \"%s\", aborting\n"),
//                  SDL_GetError()));
//       break;
//     } // end IF
    if (SDL_WaitEvent (&sdl_event) == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      break;
    } // end IF

    // if necessary, reset hover_time
    if ((sdl_event.type != RPG_GRAPHICS_SDL_HOVEREVENT) &&
        //(sdl_event.type != RPG_CLIENT_SDL_GTKEVENT) &&
        (sdl_event.type != RPG_CLIENT_SDL_TIMEREVENT))
    { ACE_GUARD (ACE_Thread_Mutex, aGuard, CBData_in.UIState->lock);
      CBData_in.hoverTime = 0;
    } // end IF

    switch (sdl_event.type)
    {
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_KEYDOWN:
#elif defined (SDL3_USE)
      case SDL_EVENT_KEY_DOWN:
#endif // SDL_USE || SDL2_USE || SDL3_USE
      {
        switch (sdl_event.key.keysym.sym)
        {
          //case SDLK_l:
          //{
          //  g_idle_add (idle_leave_game_cb,
          //              &CBData_in);
          //  break;
          //}
          //case SDLK_u:
          //{
          //  g_idle_add (idle_raise_UI_cb,
          //              &CBData_in);
          //  break;
          //}
          //case SDLK_y:
          //{
          //  CBData_in.subClass =
          //      static_cast<enum RPG_Common_SubClass> (sdl_event.key.padding2);
          //  g_idle_add (idle_level_up_cb,
          //              &CBData_in);
          //  break;
          //}
          //case SDLK_z:
          //{
          //  g_idle_add (idle_update_profile_cb,
          //              &CBData_in);
          //  break;
          //}
          default:
            break;
        } // end SWITCH

        // *WARNING*: falls through !
      }
#if defined (SDL_USE)
      case SDL_ACTIVEEVENT:
      {
        // *NOTE*: when the mouse leaves the window, it's NOT hovering
        // --> stop generating any hover events !
        if (sdl_event.active.state & SDL_APPMOUSEFOCUS)
        {
          if (sdl_event.active.gain & SDL_APPMOUSEFOCUS)
          {
//           ACE_DEBUG ((LM_DEBUG,
//                       ACE_TEXT ("gained mouse coverage...\n")));

            ACE_GUARD (ACE_Thread_Mutex, aGuard, GTKUserData_in.UIState->lock);
            GTKUserData_in.doHover = true;
          } // end IF
          else
          {
//           ACE_DEBUG ((LM_DEBUG,
//                       ACE_TEXT ("lost mouse coverage...\n")));

            ACE_GUARD (ACE_Thread_Mutex, aGuard, GTKUserData_in.UIState->lock);
            GTKUserData_in.doHover = false;
          } // end ELSE
        } // end IF

        // *WARNING*: falls through !
      }
#endif // SDL_USE
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
#elif defined (SDL3_USE)
      case SDL_EVENT_MOUSE_MOTION:
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
#endif // SDL_USE || SDL2_USE || SDL3_USE
      case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
      {
        // find window
        switch (sdl_event.type)
        {
#if defined (SDL_USE) || defined (SDL2_USE)
          case SDL_MOUSEMOTION:
#elif defined (SDL3_USE)
          case SDL_EVENT_MOUSE_MOTION:
#endif // SDL_USE || SDL2_USE || SDL3_USE
          {
            mouse_position = std::make_pair (sdl_event.motion.x,
                                             sdl_event.motion.y);
            break;
          }
#if defined (SDL_USE) || defined (SDL2_USE)
          case SDL_MOUSEBUTTONDOWN:
#elif defined (SDL3_USE)
          case SDL_EVENT_MOUSE_BUTTON_DOWN:
#endif // SDL_USE || SDL2_USE || SDL3_USE
          {
            mouse_position = std::make_pair (sdl_event.button.x,
                                             sdl_event.button.y);
            break;
          }
          default:
          {
#if defined (SDL_USE) || defined (SDL2_USE)
            int x, y;
            Uint8 button_state = SDL_GetMouseState (&x, &y);
#elif defined (SDL3_USE)
            float x, y;
            Uint32 button_state = SDL_GetMouseState (&x, &y);
#endif // SDL_USE || SDL2_USE || SDL3_USE
            ACE_UNUSED_ARG (button_state);
            mouse_position =
              std::make_pair (static_cast<unsigned int> (x), static_cast<unsigned int> (y));
            break;
          }
        } // end SWITCH
        window = main_window.getWindow (mouse_position);
        if (!window)
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("failed to RPG_Graphics_SDLWindowBase::getWindow(%u,%u), continuing\n"),
                      mouse_position.first, mouse_position.second));

        // first steps on mouse motion:
        // 0. (re-)draw cursor (handled below)
        // 1. notify previously "active" window upon losing "focus"
        if ((window || previous_window) &&
#if defined (SDL_USE) || defined (SDL2_USE)
            (sdl_event.type == SDL_MOUSEMOTION))
#elif defined (SDL3_USE)
            (sdl_event.type == SDL_EVENT_MOUSE_MOTION))
#endif // SDL_USE || SDL2_USE || SDL3_USE
        {
          // step1: notify previous window (if any)
          if (previous_window &&
//               (previous_window != mainWindow)
              (previous_window != window))
          {
            sdl_event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;

            try {
              previous_window->handleEvent (sdl_event,
                                            previous_window,
                                            dirty_region);
            } catch (...) {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }

#if defined (SDL_USE) || defined (SDL2_USE)
            sdl_event.type = SDL_MOUSEMOTION;
#elif defined (SDL3_USE)
            sdl_event.type = SDL_EVENT_MOUSE_MOTION;
#endif // SDL_USE || SDL2_USE || SDL3_USE
          } // end IF
        } // end IF
        // remember last "active" window
        previous_window = window;

        // 2. notify "active" window (if any)
        if (window)
        {
          try {
            window->handleEvent (sdl_event,
                                 window,
                                 dirty_region);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
          }
        } // end IF

        break;
      }
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_QUIT:
#elif defined (SDL3_USE)
      case SDL_EVENT_QUIT:
#endif // SDL_USE || SDL2_USE || SDL3_USE
      {
        // finished event processing
        done = true;

        break;
      }
#if defined (SDL2_USE)
      case SDL_APP_TERMINATING:
      case SDL_APP_LOWMEMORY:
      case SDL_APP_WILLENTERBACKGROUND:
      case SDL_APP_DIDENTERBACKGROUND:
      case SDL_APP_WILLENTERFOREGROUND:
      case SDL_APP_DIDENTERFOREGROUND:
      case SDL_LOCALECHANGED:
      case SDL_DISPLAYEVENT:
      case SDL_WINDOWEVENT:
      {
        if (sdl_event.type != SDL_WINDOWEVENT)
          goto continue_;
        switch (sdl_event.window.event)
        {
          case SDL_WINDOWEVENT_EXPOSED:
          case SDL_WINDOWEVENT_RESIZED:
            break;
          case SDL_WINDOWEVENT_ENTER:
          {
//           ACE_DEBUG ((LM_DEBUG,
//                       ACE_TEXT ("gained mouse coverage...\n")));

            ACE_GUARD (ACE_Thread_Mutex, aGuard, CBData_in.UIState->lock);
            CBData_in.doHover = true;
            break;
          }
          case SDL_WINDOWEVENT_LEAVE:
          {
//           ACE_DEBUG ((LM_DEBUG,
//                       ACE_TEXT ("lost mouse coverage...\n")));

            ACE_GUARD (ACE_Thread_Mutex, aGuard, CBData_in.UIState->lock);
            CBData_in.doHover = false;
            break;
          }

          default:
            break;
        } // end SWITCH

        // *WARNING*: falls through !
continue_:;
      }
#endif // SDL2_USE
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_SYSWMEVENT:
#endif // SDL_USE || SDL2_USE
#if defined (SDL_USE)
      case SDL_VIDEORESIZE:
      case SDL_VIDEOEXPOSE:
#endif // SDL_USE
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_KEYUP:
#elif defined (SDL3_USE)
      case SDL_EVENT_KEY_UP:
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (SDL2_USE)
      case SDL_TEXTEDITING:
      case SDL_TEXTINPUT:
      case SDL_KEYMAPCHANGED:
      case SDL_TEXTEDITING_EXT:
#endif // SDL2_USE
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_MOUSEBUTTONUP:
#elif defined (SDL3_USE)
      case SDL_EVENT_MOUSE_BUTTON_UP:
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (SDL2_USE)
      case SDL_MOUSEWHEEL:
#endif // SDL2_USE
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_JOYAXISMOTION:
      case SDL_JOYBALLMOTION:
      case SDL_JOYHATMOTION:
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
#elif defined (SDL3_USE)
      case SDL_EVENT_JOYSTICK_AXIS_MOTION:
      case SDL_EVENT_JOYSTICK_BALL_MOTION:
      case SDL_EVENT_JOYSTICK_HAT_MOTION:
      case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
      case SDL_EVENT_JOYSTICK_BUTTON_UP:
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (SDL2_USE)
      case SDL_JOYDEVICEADDED:
      case SDL_JOYDEVICEREMOVED:
      case SDL_CONTROLLERAXISMOTION:
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
      case SDL_CONTROLLERDEVICEADDED:
      case SDL_CONTROLLERDEVICEREMOVED:
      case SDL_CONTROLLERDEVICEREMAPPED:
      case SDL_CONTROLLERTOUCHPADDOWN:
      case SDL_CONTROLLERTOUCHPADMOTION:
      case SDL_CONTROLLERTOUCHPADUP:
      case SDL_CONTROLLERSENSORUPDATE:
      case SDL_FINGERDOWN:
      case SDL_FINGERUP:
      case SDL_FINGERMOTION:
      case SDL_DOLLARGESTURE:
      case SDL_DOLLARRECORD:
      case SDL_MULTIGESTURE:
      case SDL_CLIPBOARDUPDATE:
      case SDL_DROPFILE:
      case SDL_DROPTEXT:
      case SDL_DROPBEGIN:
      case SDL_DROPCOMPLETE:
      case SDL_AUDIODEVICEADDED:
      case SDL_AUDIODEVICEREMOVED:
      case SDL_SENSORUPDATE:
      case SDL_RENDER_TARGETS_RESET:
      case SDL_RENDER_DEVICE_RESET:
      case SDL_POLLSENTINEL:
#endif // SDL2_USE
      case RPG_CLIENT_SDL_TIMEREVENT:
      {
#if defined (SDL_USE) || defined (SDL2_USE)
        int x, y;
        Uint8 button_state = SDL_GetMouseState (&x, &y);
#elif defined (SDL3_USE)
        float x, y;
        Uint32 button_state = SDL_GetMouseState (&x, &y);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        ACE_UNUSED_ARG (button_state);
        mouse_position =
          std::make_pair (static_cast<unsigned int> (x), static_cast<unsigned int> (y));
        window = main_window.getWindow (mouse_position);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unknown/invalid SDL event type (was: %u: 0x%x), continuing\n"),
                    sdl_event.type, sdl_event.type));
        break;
      }
    } // end SWITCH

    // update screen ?
    if ((dirty_region.w != 0) || (dirty_region.h != 0))
    { ACE_ASSERT (window);
      client_action.command = COMMAND_WINDOW_REFRESH;
      client_action.window = window;
      client_engine.action (client_action);
    } // end IF

    // redraw cursor ?
    switch (sdl_event.type)
    {
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
#elif defined (SDL3_USE)
      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
#endif // SDL_USE || SDL2_USE || SDL3_USE
      {
        // map hasn't changed --> no need to redraw
        if ((dirty_region.w == 0) && (dirty_region.h == 0))
          break;

        // *WARNING*: falls through !
      }
#if defined (SDL_USE) || defined (SDL2_USE)
      case SDL_MOUSEMOTION:
#elif defined (SDL3_USE)
      case SDL_EVENT_MOUSE_MOTION:
#endif // SDL_USE || SDL2_USE || SDL3_USE
      case RPG_GRAPHICS_SDL_HOVEREVENT:
      {
        // sanity check
        if (!window)
          break; // nothing to do...

        // map has changed, cursor MAY have been drawn over...
        // --> redraw cursor ?
#if defined (SDL_USE) || defined (SDL2_USE)
        if ((sdl_event.type == SDL_MOUSEMOTION) ||
#elif defined (SDL3_USE)
        if ((sdl_event.type == SDL_EVENT_MOUSE_MOTION) ||
#endif // SDL_USE || SDL2_USE || SDL3_USE
            (dirty_region.w != 0) || (dirty_region.h != 0))
        {
          client_action.command = COMMAND_CURSOR_DRAW;
          client_action.position = mouse_position;
          client_action.window = window;
          client_engine.action (client_action);
        } // end IF

        break;
      }
      default:
        break;
    } // end SWITCH
  } while (!done);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("left SDL event loop...\n")));

  // clean up
  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
  connection_manager_p->stop (false,   // wait for completion ?
                              false);  // N/A
  connection_manager_p->abort (false); // wait for completion ?
  connection_manager_p->wait (true);   // N/A
  // *NOTE*: connections have been aborted...
  if (!UIDefinitionFile_in.empty ())
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop (true,
                                                        false);

  Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                             true,   // wait for completion ?
                                             false); // release singletons ?

  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("finished event dispatch...\n")));

  RPG_CLIENT_NETWORK_MANAGER_SINGLETON::close ();
  client_engine.stop (true,  // wait for completion ?
                      false);
  level_engine.stop (true); // locked access ?
  RPG_ENGINE_EVENT_MANAGER_SINGLETON::close ();
  timer_manager_p->stop (true,
                         false);
  Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                 previousSignalActions_inout,
                                 previousSignalMask_inout);
  RPG_Client_Common_Tools::finalize ();
  RPG_Engine_Common_Tools::finalize ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  std::cout << programName_in
#if defined (HAVE_CONFIG_H)
            << ACE_TEXT_ALWAYS_CHAR (" : ")
            << yarp_PACKAGE_VERSION
#endif // HAVE_CONFIG_H
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... Need this, as the library soname is compared to this
  // string
  std::ostringstream version_number;
  version_number << ACE::major_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::minor_version ();
  if (ACE::beta_version ())
  {
    version_number << ACE_TEXT_ALWAYS_CHAR (".");
    version_number << ACE::beta_version ();
  } // end IF
  std::cout << ACE_TEXT_ALWAYS_CHAR ("ACE: ")
            << version_number.str ()
            << std::endl;
  //   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // step0: initialize
// *PORTABILITY*: on Windows, init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_NET_CONFIGURATION_SUBDIRECTORY),
                                                          true);

  // step1a set defaults
  unsigned int peer_ping_interval_ms = NET_CLIENT_DEF_SERVER_PING_INTERVAL_MS;
  bool log_to_file = false;
  bool use_udp = false;
  std::string network_interface =
    ACE_TEXT_ALWAYS_CHAR (RPG_NET_DEFAULT_NETWORK_INTERFACE);
  bool use_loopback = false;
  bool use_reactor = RPG_ENGINE_USES_REACTOR;
  unsigned int statistics_reporting_interval =
    RPG_NET_DEFAULT_STATISTICS_REPORTING_INTERVAL;
  bool trace_information = false;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  std::string UI_file = path;
  bool print_version_and_exit = false;
  unsigned int num_dispatch_threads =
    (RPG_ENGINE_USES_REACTOR ? NET_CLIENT_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS
                             : NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS);
  struct RPG_Client_Configuration configuration;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            peer_ping_interval_ms,
                            log_to_file,
                            use_udp,
                            network_interface,
                            use_loopback,
                            use_reactor,
                            statistics_reporting_interval,
                            trace_information,
                            UI_file,
                            print_version_and_exit,
                            num_dispatch_threads))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES &&
      RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES <= std::numeric_limits<unsigned int>::max ())
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could lead to deadlocks...\n")));
  if (!UI_file.empty () &&
      !Common_File_Tools::isReadable (UI_file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid UI definition file (was: %s), aborting\n"),
                ACE_TEXT (UI_file.c_str ())));

    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF
  if (num_dispatch_threads == 0)
    num_dispatch_threads = 1;

  // step1d: pre-init signal handling
  ACE_Sig_Set signal_set (false); // fill ?
  ACE_Sig_Set ignored_signal_set (false); // fill ?
  do_initializeSignals (use_reactor,
                        (statistics_reporting_interval == 0), // handle SIGUSR1/SIGBREAK
                                                              // iff regular reporting
                                                              // is off
                        signal_set,
                        ignored_signal_set);
  Common_SignalActions_t previous_signal_actions;
  ACE_Sig_Set previous_signal_mask (false); // fill ?
  if (!Common_Signal_Tools::preInitialize (signal_set,
                                           COMMON_SIGNAL_DISPATCH_SIGNAL,
                                           true,  // use networking ?
                                           false, // use asynch timers ?
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  struct Net_Client_GTK_CBData gtk_cb_user_data;
  gtk_cb_user_data.allowUserRuntimeStatistic =
      (statistics_reporting_interval == 0); // handle SIGUSR1/SIGBREAK
                                            // iff regular reporting
                                            // is off
  gtk_cb_user_data.configuration = &configuration;
  gtk_cb_user_data.schemaRepository = Common_File_Tools::getWorkingDirectory ();
  gtk_cb_user_data.schemaRepository += ACE_DIRECTORY_SEPARATOR_STR;
  gtk_cb_user_data.schemaRepository +=
    ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING);
  gtk_cb_user_data.schemaRepository += ACE_DIRECTORY_SEPARATOR_STR;
  gtk_cb_user_data.schemaRepository +=
    ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);

  // step1e: initialize logging and/or tracing
  //Common_Logger logger (&gtk_cb_user_data.logStack,
  //                      &gtk_cb_user_data.stackLock);

  std::string log_file;
  if (log_to_file)
    log_file = Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                 ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])), // program name
                                            log_file,                      // logfile
                                            true,                          // log to syslog ?
                                            false,                         // trace messages ?
                                            trace_information,             // debug messages ?
                                            //(UI_file.empty () ? NULL
                                            //                  : &logger))) // logger ?
                                            NULL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  // *TODO*: the reasoning here is incomplete
  bool use_fd_based_reactor = use_reactor;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  use_fd_based_reactor = (use_reactor && !COMMON_EVENT_WINXX_USE_WFMO_REACTOR);
//#endif
  if (!Common_OS_Tools::setResourceLimits (use_fd_based_reactor, // file descriptors
                                           true))                // stack traces
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_OS_Tools::setResourceLimits(), aborting\n")));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step2a: initialize SDL
  // RPG_Graphics_Common_Tools::preInitialize ();
  RPG_Sound_Common_Tools::preInitialize ();

  Uint32 SDL_init_flags = 0;
  SDL_init_flags |= SDL_INIT_TIMER;                                            // timers
  SDL_init_flags |= (configuration.audio_configuration.mute ? 0
                                                            : SDL_INIT_AUDIO); // audio
  SDL_init_flags |= SDL_INIT_VIDEO;                                            // video
#if defined (SDL_USE)
  SDL_init_flags |=
      ((configuration.audio_configuration.mute ||
        !configuration.audio_configuration.use_CD) ? 0
                                                   : SDL_INIT_CDROM);          // audioCD playback
#endif // SDL_USE
//  SDL_init_flags |= SDL_INIT_JOYSTICK;                                         // joystick
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_init_flags |= SDL_INIT_NOPARACHUTE;                                        /**< Don't catch fatal signals */
#endif // SDL_USE || SL2_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (SDL_USE)
  SDL_init_flags |= SDL_INIT_EVENTTHREAD;                                        /**< Not supported on all OS's */
#elif defined (SDL2_USE)
  SDL_init_flags |= SDL_INIT_EVENTS;
#endif // SDL_USE || SL2_USE
#endif // ACE_WIN32 || ACE_WIN64
  if (SDL_Init (SDL_init_flags) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Init(0x%x): \"%s\", aborting\n"),
                SDL_init_flags,
                ACE_TEXT (SDL_GetError ())));

  // clean up
  Common_Log_Tools::finalizeLogging ();
  // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF
  if (TTF_Init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to TTF_Init(): \"%s\", aborting\n"),
                ACE_TEXT (TTF_GetError ())));

    // clean up
    Common_Log_Tools::finalizeLogging ();
    SDL_Quit ();
    // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1h: initialize GLIB / G(D|T)K[+] / GNOME ?
  //Common_UI_GladeDefinition ui_definition (argc_in,
  //                                         argv_in);
  Common_UI_GtkBuilderDefinition_t ui_definition;
  if (!UI_file.empty ())
  {
    configuration.gtk_configuration.definition = &ui_definition;
    configuration.gtk_configuration.CBData = &gtk_cb_user_data;
    bool result_2 =
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (configuration.gtk_configuration);
    ACE_ASSERT (result_2);
  } // end IF

  ACE_High_Res_Timer highres_timer;
  highres_timer.start ();
  // step2: do actual work
  do_work (peer_ping_interval_ms,
           use_udp,
           network_interface,
           use_loopback,
           use_reactor,
           statistics_reporting_interval,
           num_dispatch_threads,
           UI_file,
           gtk_cb_user_data,
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
           previous_signal_mask);
  highres_timer.stop ();

  // debug info
  ACE_Time_Value working_time;
  highres_timer.elapsed_time (working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // step4: clean up
  TTF_Quit ();
  SDL_Quit ();

  // stop profile timer...
  process_profile.stop ();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time (elapsed_time) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
  std::string user_time_string = Common_Timer_Tools::periodToString (user_time);
  std::string system_time_string =
    Common_Timer_Tools::periodToString (system_time);

  // debug info
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#else
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
#endif // ACE_WIN32 || ACE_WIN64

  Common_Log_Tools::finalizeLogging ();
// *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
