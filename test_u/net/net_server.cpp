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

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

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

#include "net_common_tools.h"
#include "net_configuration.h"

#include "net_server_common_tools.h"
#include "net_server_defines.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"

#include "rpg_engine_defines.h"

#include "rpg_net_common.h"
#include "rpg_net_defines.h"

#include "rpg_net_protocol_messagehandler.h"
#include "rpg_net_protocol_network.h"

#include "rpg_client_common.h"

#include "net_callbacks.h"
#include "net_common.h"
#include "net_defines.h"
#include "net_server_common.h"
#include "net_server_eventhandler.h"
#include "net_server_signalhandler.h"

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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c [VALUE]   : max #connections ([")
            << RPG_NET_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(NET_SERVER_UI_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]] : UI file [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [VALUE]   : client ping interval (ms) [")
            << NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL_MS
            << ACE_TEXT_ALWAYS_CHAR ("] {0 --> OFF})")
            << std::endl;
//  std::cout << ACE_TEXT_ALWAYS_CHAR("-k [VALUE]  : client keep-alive timeout ([")
//            << RPG_NET_SERVER_DEF_CLIENT_KEEPALIVE
//            << ACE_TEXT_ALWAYS_CHAR("] second(s) {0 --> no timeout})")
//            << std::endl;
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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p [VALUE]   : listening port [")
            << RPG_NET_DEFAULT_PORT
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
            << (RPG_ENGINE_USES_REACTOR ? NET_SERVER_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS
                                        : NET_SERVER_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     unsigned int& maxNumConnections_out,
                     unsigned int& clientPingInterval_out,
                     //unsigned int& keepAliveTimeout_out,
                     bool& logToFile_out,
                     bool& useUDP_out,
                     std::string& networkInterface_out,
                     bool& useLoopback_out,
                     unsigned short& listeningPortNumber_out,
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
  maxNumConnections_out           =
      RPG_NET_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS;
  clientPingInterval_out          = NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL_MS;
//  keepAliveTimeout_out = RPG_NET_SERVER_DEF_CLIENT_KEEPALIVE;
  logToFile_out                   = false;
  useUDP_out                      = false;
  networkInterface_out            =
    ACE_TEXT_ALWAYS_CHAR (RPG_NET_DEFAULT_NETWORK_INTERFACE);
  useLoopback_out                 = false;
  listeningPortNumber_out         = RPG_NET_DEFAULT_PORT;
  useReactor_out                  = RPG_ENGINE_USES_REACTOR;
  statisticsReportingInterval_out =
      RPG_NET_DEFAULT_STATISTICS_REPORTING_INTERVAL;
  traceInformation_out            = false;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_FILE);
  UIFile_out                      = path;
  printVersionAndExit_out         = false;
  numDispatchThreads_out =
    (RPG_ENGINE_USES_REACTOR ? NET_SERVER_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS
                             : NET_SERVER_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS);
  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("c:g::i:k:lmn:op:rs:tvx:"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> maxNumConnections_out;
        break;
      }
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
        converter >> clientPingInterval_out;
        break;
      }
//      case 'k':
//      {
//        converter.clear();
//        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
//        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg());
//        converter >> keepAliveTimeout_out;
//        break;
//      }
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
      case 'p':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> listeningPortNumber_out;
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
  if (allowUserRuntimeStats_in)
  {
    signals_out.sig_add (SIGBREAK);        // 21      /* Ctrl-Break sequence */
    ignoredSignals_out.sig_add (SIGBREAK); // 21      /* Ctrl-Break sequence */
  } // end IF
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
do_work (unsigned int maxNumConnections_in,
         unsigned int pingInterval_in,
         //unsigned int keepAliveTimeout_in,
         bool useUDP_in,
         const std::string& networkInterface_in,
         bool useLoopback_in,
         unsigned short listeningPortNumber_in,
         bool useReactor_in,
         unsigned int statisticsReportingInterval_in,
         unsigned int numDispatchThreads_in,
         const std::string& UIDefinitionFile_in,
         struct Net_Server_GTK_CBData& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
         ACE_Sig_Set& previousSignalMask_inout)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  CBData_in.configuration->parser_configuration.debugParser = true;
  CBData_in.configuration->parser_configuration.debugScanner = true;

  // step0a: initialize stream configuration object
  struct Common_Parser_FlexAllocatorConfiguration allocator_configuration;
  struct Stream_ModuleConfiguration module_configuration;
  struct RPG_Net_Protocol_ModuleHandlerConfiguration modulehandler_configuration;
  modulehandler_configuration.allocatorConfiguration = &allocator_configuration;
  modulehandler_configuration.parserConfiguration =
    &CBData_in.configuration->parser_configuration;
  modulehandler_configuration.protocolOptions =
    &CBData_in.configuration->protocol_configuration.protocolOptions;
  struct RPG_Net_Protocol_StreamConfiguration stream_configuration;
  RPG_Net_Protocol_StreamConfiguration_t stream_configuration_2;

  Net_Server_EventHandler ui_message_handler (&CBData_in);
  RPG_Net_Protocol_MessageHandler_Module message_handler (NULL,
                                                          ACE_TEXT_ALWAYS_CHAR ("MessageHandler"));
  modulehandler_configuration.subscriber = &ui_message_handler;

  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Stream_AllocatorConfiguration> heap_allocator;
  RPG_Net_MessageAllocator_t message_allocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                &heap_allocator,
                                                true); // block ?

  stream_configuration.messageAllocator = &message_allocator;
  stream_configuration.module =
    (!UIDefinitionFile_in.empty () ? &message_handler
                                   : NULL);
  stream_configuration_2.initialize (module_configuration,
                                     modulehandler_configuration,
                                     stream_configuration);
  CBData_in.configuration->protocol_configuration.connectionConfiguration.allocatorConfiguration =
    &allocator_configuration;
  CBData_in.configuration->protocol_configuration.connectionConfiguration.streamConfiguration =
    &stream_configuration_2;
  CBData_in.configuration->protocol_configuration.connectionConfiguration.messageAllocator =
    &message_allocator;

  // step0b: initialize event dispatch
  if (!Common_Event_Tools::initializeEventDispatch (CBData_in.configuration->dispatch_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
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
    ACE_Time_Value interval (statisticsReportingInterval_in,
                             0);
    timer_id =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (handler_p,                  // event handler
                                                            NULL,                       // ACT
                                                            COMMON_TIME_NOW + interval, // first wakeup time
                                                            interval);                  // interval
    if (timer_id == -1)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to schedule timer: \"%m\", returning\n")));

      // clean up
      COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();

      return;
    } // end IF
  } // end IF

  // step2: signal handling
  if (useReactor_in)
    CBData_in.listenerHandle = RPG_NET_PROTOCOL_LISTENER_SINGLETON::instance ();
  else
    CBData_in.listenerHandle =
      RPG_NET_PROTOCOL_ASYNCH_LISTENER_SINGLETON::instance ();
  // event handler for signals
  struct Common_EventDispatchState dispatch_state_s;
  CBData_in.configuration->signal_configuration.dispatchState =
    &dispatch_state_s;
  CBData_in.configuration->signal_configuration.mode =
    COMMON_SIGNAL_DISPATCH_SIGNAL;
  CBData_in.configuration->signal_configuration.stopEventDispatchOnShutdown =
    true;
  Net_Server_SignalHandler signal_handler (timer_id/*,
                                           CBData_in.listenerHandle,
                                           RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance (),
                                           useReactor_in*/);
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
      result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timer_id,
                                                                   &act_p);
      if (result <= 0)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    timer_id));
    } // end IF
    COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();

    return;
  } // end IF

  // step3: initialize connection manager
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->initialize (maxNumConnections_in,
                                                                         ACE_Time_Value (0, NET_STATISTIC_DEFAULT_VISIT_INTERVAL_MS * 1000));
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->set (CBData_in.configuration->protocol_configuration.connectionConfiguration,
                                                                  NULL);

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
    CBData_in.configuration->gtk_configuration.eventHooks.initHook = idle_initialize_server_UI_cb;
    //CBData_in.GTKState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
    CBData_in.UIState = &state_r;
    CBData_in.UIState->builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFile_in, static_cast<GtkBuilder*> (NULL));
    //CBData_in.GTKState.userData = &CBData_in;

    gtk_manager_p->start (NULL);
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));

      // clean up
      if (timer_id != -1)
      {
        result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timer_id,
                                                                     &act_p);
        if (result <= 0)
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                      timer_id));
      } // end IF
      COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
      gtk_manager_p->stop (true,
                           false);
      Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                     previousSignalActions_inout,
                                     previousSignalMask_inout);

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
      result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timer_id,
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
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   previousSignalActions_inout,
                                   previousSignalMask_inout);

    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  // step4c: start listening
  //Net_SocketHandlerConfiguration_t socket_handler_configuration;
  ////ACE_OS::memset (&socket_handler_configuration, 0, sizeof (socket_handler_configuration));
  //socket_handler_configuration.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
  //socket_handler_configuration.messageAllocator = &message_allocator;
  //socket_handler_configuration.socketConfiguration =
  //    configuration.socketConfiguration;
  //Net_ListenerConfiguration_t listener_configuration;
  //ACE_OS::memset (&listener_configuration, 0, sizeof (listener_configuration));
  //listener_configuration.addressFamily = ACE_ADDRESS_FAMILY_INET;
  //listener_configuration.allocator = &message_allocator;
  //listener_configuration.connectionManager =
  //  NET_CONNECTIONMANAGER_SINGLETON::instance ();
  //listener_configuration.portNumber = listeningPortNumber_in;
  //listener_configuration.socketHandlerConfiguration =
  //  &socket_handler_configuration;
  //listener_configuration.statisticCollectionInterval =
  //  statisticsReportingInterval_in;
  //listener_configuration.useLoopbackDevice = useLoopback_in;
  CBData_in.configuration->protocol_configuration.connectionConfiguration.socketConfiguration.address.set (static_cast<u_short> (listeningPortNumber_in),
                                                                                                           static_cast<ACE_UINT32> (INADDR_ANY),
                                                                                                           1,        // encode port number ?
                                                                                                           0);       // map ?
  if (!CBData_in.listenerHandle->initialize (CBData_in.configuration->protocol_configuration.connectionConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize listener, returning\n")));

    // clean up
    Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                               false); // wait ?
    if (timer_id != -1)
    {
      result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timer_id,
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
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   previousSignalActions_inout,
                                   previousSignalMask_inout);

    return;
  } // end IF
  CBData_in.listenerHandle->start (NULL);
  if (!CBData_in.listenerHandle->isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start listener (port: %u), returning\n"),
                listeningPortNumber_in));

    // clean up
    Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                               false); // wait ?
    if (timer_id != -1)
    {
      result = COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (timer_id,
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
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
    Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                   previousSignalActions_inout,
                                   previousSignalMask_inout);

    return;
  } // end IF

  // *NOTE*: from this point on, clean up any remote connections !

  Common_Event_Tools::dispatchEvents (dispatch_state_s);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished event dispatch...\n")));

  // clean up
  // *NOTE*: listener has stopped, interval timer has been cancelled,
  // and connections have been aborted...
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
  COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
  Common_Signal_Tools::finalize (COMMON_SIGNAL_DISPATCH_SIGNAL,
                                 previousSignalActions_inout,
                                 previousSignalMask_inout);

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

  // step0: init
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
  unsigned int max_num_connections =
    RPG_NET_MAXIMUM_NUMBER_OF_OPEN_CONNECTIONS;
  unsigned int ping_interval = NET_SERVER_DEFAULT_CLIENT_PING_INTERVAL_MS;
  //  unsigned int keep_alive_timeout            = RPG_NET_SERVER_DEFAULT_TCP_KEEPALIVE;
  bool log_to_file = false;
  bool use_udp = false;
  std::string network_interface =
    ACE_TEXT_ALWAYS_CHAR (RPG_NET_DEFAULT_NETWORK_INTERFACE);
  bool use_loopback = false;
  unsigned short listening_port_number = RPG_NET_DEFAULT_PORT;
  bool use_reactor = RPG_ENGINE_USES_REACTOR;
  unsigned int statistics_reporting_interval =
    RPG_NET_DEFAULT_STATISTICS_REPORTING_INTERVAL;
  bool trace_information = false;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_FILE);
  std::string UI_file = path;
  bool print_version_and_exit = false;
  unsigned int num_dispatch_threads =
    (RPG_ENGINE_USES_REACTOR ? NET_SERVER_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS
                             : NET_SERVER_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS);
  struct RPG_Client_Configuration configuration;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            max_num_connections,
                            ping_interval,
                            //keep_alive_timeout,
                            log_to_file,
                            use_udp,
                            network_interface,
                            use_loopback,
                            listening_port_number,
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
  // *NOTE*: probably requires CAP_NET_BIND_SERVICE
  if (listening_port_number <= 1023)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("using (privileged) port #: %d...\n"),
                listening_port_number));
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES <=
      std::numeric_limits<unsigned int>::max ())
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could lead to deadlocks...\n")));
  if (!UI_file.empty() &&
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
                                           true,
                                           true,
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

  struct Net_Server_GTK_CBData gtk_cb_user_data;
  gtk_cb_user_data.allowUserRuntimeStatistic =
      (statistics_reporting_interval == 0); // handle SIGUSR1/SIGBREAK
                                            // iff regular reporting
                                            // is off
  gtk_cb_user_data.configuration = &configuration;

  // step1e: initialize logging and/or tracing
  //Common_Logger logger (&gtk_cb_user_data.logStack,
  //                      &gtk_cb_user_data.stackLock);
  std::string log_file;
  if (log_to_file)
    log_file =
      Net_Server_Common_Tools::getNextLogFileName (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
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

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
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

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (max_num_connections,
           ping_interval,
           //keep_alive_timeout,
           use_udp,
           network_interface,
           use_loopback,
           listening_port_number,
           use_reactor,
           statistics_reporting_interval,
           num_dispatch_threads,
           UI_file,
           gtk_cb_user_data,
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
           previous_signal_mask);
  timer.stop ();

  // debug info
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

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
  std::string user_time_string;
  std::string system_time_string;
  user_time_string =
    Common_Timer_Tools::periodToString (user_time);
  system_time_string =
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
