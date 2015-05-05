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
#include <list>
#include <sstream>
#include <string>

#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Version.h"

#ifdef RPG_ENABLE_VALGRIND_SUPPORT
#include "valgrind/valgrind.h"
#endif

#include "common_file_tools.h"
#include "common_timer_manager.h"
#include "common_tools.h"

#include "common_ui_defines.h"
//#include "common_ui_glade_definition.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager.h"

#include "stream_allocatorheap.h"

#include "net_connection_manager.h"

#include "net_client_connector_common.h"
#include "net_client_defines.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice.h"

#include "rpg_common.h"
#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

//#include "rpg_net_defines.h"
//#include "rpg_net_common.h"
//#include "rpg_net_module_eventhandler.h"
//
//#include "rpg_net_server_defines.h"

#include "rpg_client_defines.h"
#include "rpg_client_logger.h"
#include "rpg_client_ui_tools.h"

#include "net_callbacks.h"
#include "net_common.h"
#include "net_defines.h"
#include "net_eventhandler.h"

#include "net_client_signalhandler.h"
#include "net_client_timeouthandler.h"

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif // #ifdef DEBUG_DEBUGGER

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT("-a           : alternating mode [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-c [VALUE]   : max #connections [")
            << NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS
            << ACE_TEXT("] {0 --> unlimited}")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  //  path += ACE_TEXT_ALWAYS_CHAR("net");
  //  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#endif
  path += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  std::cout << ACE_TEXT("-g[[STRING]] : UI file [\"")
            << path
            << ACE_TEXT("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT("-h [STRING]  : server hostname [\"")
            << NET_CLIENT_DEF_SERVER_HOSTNAME
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-i [VALUE]   : connection interval (second(s)) [")
            << NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL
            << ACE_TEXT("] {0 --> OFF}")
            << std::endl;
  std::cout << ACE_TEXT("-l           : log to a file [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-p [VALUE]   : server port [")
            << RPG_NET_SERVER_DEFAULT_LISTENING_PORT
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-r           : use reactor [")
            << RPG_NET_USES_REACTOR
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-s           : server ping interval (millisecond(s)) [")
            << NET_CLIENT_DEF_SERVER_PING_INTERVAL
            << ACE_TEXT("] {0 --> OFF}")
            << std::endl;
  std::cout << ACE_TEXT("-t           : trace information [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-u           : use UDP [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-v           : print version information and exit [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]   : #dispatch threads [")
            << NET_CLIENT_DEF_NUM_DISPATCH_THREADS
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-y           : run stress-test [")
            << false
            << ACE_TEXT("]")
            << std::endl;
}

bool
do_processArguments (const int& argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& alternatingMode_out,
                     unsigned int& maxNumConnections_out,
                     std::string& UIFile_out,
                     std::string& serverHostname_out,
                     unsigned int& connectionInterval_out,
                     bool& logToFile_out,
                     unsigned short& serverPortNumber_out,
                     bool& useReactor_out,
                     unsigned int& serverPingInterval_out,
                     bool& traceInformation_out,
                     bool& useUDP_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numDispatchThreads_out,
                     bool& runStressTest_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
#endif // #ifdef BASEDIR

  // init results
  alternatingMode_out     = false;
  maxNumConnections_out   = NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  //  path += ACE_TEXT_ALWAYS_CHAR("net");
  //  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#endif
  path += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  UIFile_out              = path;
  serverHostname_out      = NET_CLIENT_DEF_SERVER_HOSTNAME;
  connectionInterval_out  = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  logToFile_out           = false;
  serverPortNumber_out    = RPG_NET_SERVER_DEFAULT_LISTENING_PORT;
  useReactor_out          = RPG_NET_USES_REACTOR;
  serverPingInterval_out  = NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  traceInformation_out    = false;
  useUDP_out              = false;
  printVersionAndExit_out = false;
  numDispatchThreads_out  = NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
  runStressTest_out       = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("ac:g::h:i:lp:rs:tuvx:y"),
                             1,                          // skip command name
                             1,                          // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                             0);                         // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'a':
      {
        alternatingMode_out = true;
        break;
      }
      case 'c':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
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
      case 'h':
      {
        serverHostname_out = argumentParser.opt_arg();
        break;
      }
      case 'i':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> connectionInterval_out;
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'p':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> serverPortNumber_out;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 's':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> serverPingInterval_out;
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'u':
      {
        useUDP_out = true;
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
        converter << argumentParser.opt_arg();
        converter >> numDispatchThreads_out;
        break;
      }
      case 'y':
      {
        runStressTest_out = true;
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

void
do_initializeSignals (bool allowUserRuntimeConnect_in,
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

  // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
  //                ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
//  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  if (allowUserRuntimeConnect_in)
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
                ACE_TEXT("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeConnect_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
  } // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */

#ifdef RPG_ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);        // 64
#endif
#endif
}

void
do_work (Net_Client_TimeoutHandler::ActionMode_t actionMode_in,
         unsigned int maxNumConnections_in,
         const std::string& UIDefinitionFile_in,
         const std::string& serverHostname_in,
         unsigned int connectionInterval_in,
         unsigned short serverPortNumber_in,
         bool useReactor_in,
         unsigned int serverPingInterval_in,
         unsigned int numDispatchThreads_in,
         bool useUDP_in,
         Net_GTK_CBData_t& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  int result = -1;

  // step0a: initialize randomization
  try
  {
    RPG_Dice::init ();
  }
  catch(...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Dice::init, returning\n")));
    return;
  }

  // step0b: initialize stream configuration object
  Stream_ModuleConfiguration_t module_configuration;
  ACE_OS::memset (&module_configuration, 0, sizeof (module_configuration));

  Net_EventHandler ui_event_handler (&CBData_in);
  RPG_Net_Module_EventHandler_Module event_handler (ACE_TEXT_ALWAYS_CHAR ("EventHandler"),
                                                    NULL);
  RPG_Net_Module_EventHandler* eventHandler_impl = NULL;
  eventHandler_impl =
    dynamic_cast<RPG_Net_Module_EventHandler*> (event_handler.writer ());
  if (!eventHandler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Module_EventHandler> failed, returning\n")));
    return;
  } // end IF
  eventHandler_impl->initialize (&CBData_in.subscribers,
                                 &CBData_in.subscribersLock);
  eventHandler_impl->subscribe (&ui_event_handler);

  Stream_AllocatorHeap heap_allocator;
  RPG_Net_StreamMessageAllocator_t message_allocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                      &heap_allocator,
                                                      false);
  Net_Configuration_t configuration;
  ACE_OS::memset (&configuration, 0, sizeof (Net_Configuration_t));
  // ************ connection configuration data ************
  configuration.protocolConfiguration.peerPingInterval =
      ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? 0
                                                                   : serverPingInterval_in);
  configuration.protocolConfiguration.pingAutoAnswer = true;
  configuration.protocolConfiguration.printPongMessages = true;
  // ************ socket / stream configuration data ************
  configuration.socketConfiguration.bufferSize =
   RPG_NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;

  configuration.streamConfiguration.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
  configuration.streamConfiguration.messageAllocator = &message_allocator;
  configuration.streamConfiguration.moduleConfiguration = &module_configuration;
  configuration.streamConfiguration.module =
      (!UIDefinitionFile_in.empty () ? &event_handler
                                     : NULL);

  //  config.useThreadPerConnection = false;
  //  config.serializeOutput = false;

  //  config.notificationStrategy = NULL;
//  config.delete_module = false;
  // *WARNING*: set at runtime, by the appropriate connection handler
//  config.sessionID = 0; // (== socket handle !)
//  config.statisticsReportingInterval = 0; // == off
//	config.printFinalReport = false;
  // ************ runtime data ************
//	config.currentStatistics = {};
//	config.lastCollectionTimestamp = ACE_Time_Value::zero;

  // step0c: initialize event dispatch
  if (!Common_Tools::initializeEventDispatch (useReactor_in,
                                              numDispatchThreads_in,
                                              configuration.streamConfiguration.serializeOutput))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize event dispatch, returing\n")));
    return;
  } // end IF

  // step0d: initialize client connector
  Net_SocketHandlerConfiguration_t socket_handler_configuration;
  socket_handler_configuration.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
  socket_handler_configuration.messageAllocator = &message_allocator;
  socket_handler_configuration.socketConfiguration =
    configuration.socketConfiguration;
  Net_Client_IConnector_t* connector_p = NULL;
  if (useReactor_in)
    ACE_NEW_NORETURN (connector_p,
                      Net_Client_Connector_t (&socket_handler_configuration,
                                              NET_CONNECTIONMANAGER_SINGLETON::instance (),
                                              0));
  else
    ACE_NEW_NORETURN (connector_p,
                      Net_Client_AsynchConnector_t (&socket_handler_configuration,
                                                    NET_CONNECTIONMANAGER_SINGLETON::instance (),
                                                    0));
  if (!connector_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, returning\n")));
    return;
  } // end IF

  // step0e: initialize connection manager
  Net_IInetConnectionManager_t* iconnection_manager_p =
    Net_Common_Tools::getConnectionManager ();
  ACE_ASSERT (iconnection_manager_p);
  Net_InetConnectionManager_t* connection_manager_p =
    dynamic_cast<Net_InetConnectionManager_t*> (iconnection_manager_p);
  ACE_ASSERT (connection_manager_p);
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
  connection_manager_p->set (configuration,
                             &configuration.streamSessionData);

  // step0f: initialize action timer
  ACE_INET_Addr peer_address (serverPortNumber_in,
                              serverHostname_in.c_str (),
                              AF_INET);
  Net_Client_TimeoutHandler timeout_handler (actionMode_in,
                                             maxNumConnections_in,
                                             peer_address,
                                             connector_p);
  CBData_in.timeoutHandler = &timeout_handler;
  CBData_in.timerId = -1;
  if (UIDefinitionFile_in.empty ())
  {
    // schedule action interval timer
    ACE_Event_Handler* event_handler = &timeout_handler;
    ACE_Time_Value interval (((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? (NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000)
                                                                                          : connectionInterval_in),
                             ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000)
                                                                                          : 0));
    CBData_in.timerId =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (event_handler,                    // event handler
                                                            NULL,                             // ACT
                                                            COMMON_TIME_POLICY () + interval, // first wakeup time
                                                            interval);                        // interval
    if (CBData_in.timerId == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to schedule action timer: \"%m\", aborting\n")));

      // clean up
      COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
      delete connector_p;

      return;
    } // end IF
  } // end IF
  const void* act_p = NULL;

  // step0f: initialize signal handling
  Net_Client_SignalHandler signal_handler (CBData_in.timerId, // action timer id
                                           peer_address,      // remote SAP
                                           connector_p,       // connector
                                           useReactor_in);    // use reactor ?
  if (!Common_Tools::initializeSignals (signalSet_in,
                                        ignoredSignalSet_in,
                                        &signal_handler,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeSignals(), aborting\n")));

    // clean up
    if (CBData_in.timerId != -1)
    {
      result =
        COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (CBData_in.timerId,
                                                            &act_p);
      if (result <= 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    CBData_in.timerId));
    } // end IF
    COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
    connector_p->abort ();
    delete connector_p;

    return;
  } // end IF

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step1a: start GTK event loop ?
  if (!UIDefinitionFile_in.empty ())
  {
    CBData_in.GTKState.finalizationHook = idle_finalize_UI_cb;
    CBData_in.GTKState.initializationHook = idle_initialize_client_UI_cb;
    //CBData_in.GTKState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    //  std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
    CBData_in.GTKState.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
      std::make_pair (UIDefinitionFile_in, static_cast<GtkBuilder*> (NULL));
    CBData_in.GTKState.userData = &CBData_in;

    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
    if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, aborting\n")));

      // clean up
      if (CBData_in.timerId != -1)
      {
        result =
          COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (CBData_in.timerId,
                                                              &act_p);
        if (result <= 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                      CBData_in.timerId));
      } // end IF
      COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
      connector_p->abort ();
      delete connector_p;
      Common_Tools::finalizeSignals (signalSet_in,
                                     useReactor_in,
                                     previousSignalActions_inout);

      return;
    } // end IF
  } // end IF

  // step5b: init worker(s)
  int group_id = -1;
  if (!Common_Tools::startEventDispatch (useReactor_in,
                                         numDispatchThreads_in,
                                         group_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, aborting\n")));

    // clean up
    if (CBData_in.timerId != -1)
    {
      result =
        COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (CBData_in.timerId,
                                                            &act_p);
      if (result <= 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                    CBData_in.timerId));
    } // end IF
//		{ // synch access
//			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//					 iterator != CBData_in.event_source_ids.end();
//					 iterator++)
//				g_source_remove(*iterator);
//		} // end lock scope
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
    connector_p->abort ();
    delete connector_p;
    Common_Tools::finalizeSignals (signalSet_in,
                                   useReactor_in,
                                   previousSignalActions_inout);

    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  // step5c: connect immediately ?
  if (UIDefinitionFile_in.empty () && (connectionInterval_in == 0))
  {
    bool result_2 = connector_p->connect (peer_address);
    if (!useReactor_in)
    {
      ACE_Time_Value delay (1, 0);
      ACE_OS::sleep (delay);
      if (NET_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections () != 1)
        result = false;
    } // end IF

    if (!result_2)
    {
      char buffer[BUFSIZ];
      ACE_OS::memset (buffer, 0, sizeof (buffer));
      result = peer_address.addr_to_string (buffer,
                                            sizeof (buffer));
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string: \"%m\", continuing\n")));
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to connect to \"%s\", returning\n"),
                  ACE_TEXT (buffer)));

      // clean up
      if (CBData_in.timerId != -1)
      {
        result =
            COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (CBData_in.timerId,
                                                                &act_p);
        if (result <= 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                      CBData_in.timerId));
      } // end IF
      //		{ // synch access
      //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

      //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
      //					 iterator != CBData_in.event_source_ids.end();
      //					 iterator++)
      //				g_source_remove(*iterator);
      //		} // end lock scope
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
      COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();
      connector_p->abort ();
      delete connector_p;
      Common_Tools::finalizeSignals (signalSet_in,
                                     useReactor_in,
                                     previousSignalActions_inout);

      return;
    } // end IF
  } // end IF

  // *NOTE*: from this point on, clean up any remote connections !

  // step6: dispatch events
  // *NOTE*: when using a thread pool, handle things differently...
  if (numDispatchThreads_in > 1)
  {
    if (ACE_Thread_Manager::instance ()->wait_grp (group_id) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                  group_id));
  } // end IF
  else
  {
    if (useReactor_in)
    {
/*      // *WARNING*: restart system calls (after e.g. SIGINT) for the reactor
      ACE_Reactor::instance()->restart(1);
*/
      if (ACE_Reactor::instance ()->run_reactor_event_loop (0) == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to handle events: \"%m\", aborting\n")));
    } // end IF
    else
      if (ACE_Proactor::instance ()->proactor_run_event_loop (0) == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to handle events: \"%m\", aborting\n")));
  } // end ELSE

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished event dispatch...\n")));

  // step7: clean up
  if (!UIDefinitionFile_in.empty ())
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->wait ();
  //		{ // synch access
  //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

  //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
  //					 iterator != CBData_in.event_source_ids.end();
  //					 iterator++)
  //				g_source_remove(*iterator);
  //		} // end lock scope
  COMMON_TIMERMANAGER_SINGLETON::instance ()->stop ();

  connection_manager_p->abort ();
  // *IMPORTANT NOTE*: as long as connections are inactive (i.e. events are
  // dispatched by reactor thread(s), there is no real reason to wait here)
  connection_manager_p->wait ();

  Common_Tools::finalizeSignals (signalSet_in,
                                 useReactor_in,
                                 previousSignalActions_inout);
//  { // synch access
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//				 iterator != CBData_in.event_source_ids.end();
//				 iterator++)
//			g_source_remove(*iterator);
//	} // end lock scope
  delete connector_p;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            << RPG_VERSION
#endif
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... Need this, as the library soname is compared to this
  // string
  std::ostringstream version_number;
  version_number << ACE::major_version();
  version_number << ACE_TEXT(".");
  version_number << ACE::minor_version();
  version_number << ACE_TEXT(".");

  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // step0: initialize
// *PORTABILITY*: on Windows, initialize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
#endif // #ifdef BASEDIR

  // step1a set defaults
  Net_Client_TimeoutHandler::ActionMode_t action_mode =
   Net_Client_TimeoutHandler::ACTION_NORMAL;
  bool alternating_mode                               = false;
  unsigned int max_num_connections                    =
   NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  //  path += ACE_TEXT_ALWAYS_CHAR("net");
  //  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#endif
  path += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  std::string UI_file                                 = path;
  std::string server_hostname                         =
   NET_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned int connection_interval                    =
   NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  bool log_to_file                                    = false;
  unsigned short server_port_number                   =
   RPG_NET_SERVER_DEFAULT_LISTENING_PORT;
  bool use_reactor                                    = RPG_NET_USES_REACTOR;
  unsigned int server_ping_interval                   =
   NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  bool trace_information                              = false;
  bool use_UDP                                        = false;
  bool print_version_and_exit                         = false;
  unsigned int num_dispatch_threads                   =
   NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
  bool run_stress_test                                = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            alternating_mode,
                            max_num_connections,
                            UI_file,
                            server_hostname,
                            connection_interval,
                            log_to_file,
                            server_port_number,
                            use_reactor,
                            server_ping_interval,
                            trace_information,
                            use_UDP,
                            print_version_and_exit,
                            num_dispatch_threads,
                            run_stress_test))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES <=
      std::numeric_limits<unsigned int>::max ())
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could lead to deadlocks...\n")));
  if ((!UI_file.empty () && (alternating_mode || run_stress_test)) ||
      (run_stress_test && ((server_ping_interval != 0) ||
                           (connection_interval != 0)))           ||
      (alternating_mode && run_stress_test))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF
  if (!UI_file.empty () &&
      !Common_File_Tools::isReadable (UI_file))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid UI definition file (was: %s), aborting\n"),
                ACE_TEXT (UI_file.c_str ())));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF
  if (num_dispatch_threads == 0)
    num_dispatch_threads = 1;

  if (alternating_mode)
    action_mode = Net_Client_TimeoutHandler::ACTION_ALTERNATING;
  if (run_stress_test)
    action_mode = Net_Client_TimeoutHandler::ACTION_STRESS;

  // step1d: pre-initialize signal handling
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  do_initializeSignals ((connection_interval == 0), // allow SIGUSR1/SIGBREAK iff
                                                    // regular connections are off
                        signal_set,
                        ignored_signal_set);
  Common_SignalActions_t previous_signal_actions;
  if (!Common_Tools::preInitializeSignals (signal_set,
                                           use_reactor,
                                           previous_signal_actions))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  Net_GTK_CBData_t gtk_cb_user_data;
  // step1e: initialize logging and/or tracing
  RPG_Client_Logger logger (&gtk_cb_user_data.logStack,
                            &gtk_cb_user_data.stackLock);
  std::string log_file;
  if (log_to_file)
    log_file = RPG_Common_File_Tools::getLogFilename (ACE::basename (argv_in[0]));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),    // program name
                                        log_file,                      // logfile
                                        false,                         // log to syslog ?
                                        false,                         // trace messages ?
                                        trace_information,             // debug messages ?
                                        (UI_file.empty () ? NULL
                                                          : &logger))) // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  // *TODO*: the reasoning here is incomplete
  bool use_fd_based_reactor = use_reactor;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  use_fd_based_reactor = (use_reactor && !COMMON_EVENT_WINXX_USE_WFMO_REACTOR);
#endif
  if (!Common_Tools::setResourceLimits (use_fd_based_reactor, // file descriptors
                                        true))                // stack traces
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::setResourceLimits(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1h: init GLIB / G(D|T)K[+] / GNOME ?
  //Common_UI_GladeDefinition ui_definition (argc_in,
  //                                         argv_in);
  Common_UI_GtkBuilderDefinition ui_definition (argc_in,
                                                argv_in);
  if (!UI_file.empty ())
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                              argv_in,
                                                              &gtk_cb_user_data.GTKState,
                                                              &ui_definition);

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (action_mode,
           max_num_connections,
           UI_file,
           server_hostname,
           connection_interval,
           server_port_number,
           use_reactor,
           server_ping_interval,
           num_dispatch_threads,
           use_UDP,
           gtk_cb_user_data,
           signal_set,
           ignored_signal_set,
           previous_signal_actions);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  RPG_Common_Tools::period2String (working_time,
                                   working_time_string);

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
#endif
    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  RPG_Common_Tools::period2String (user_time,
                                   user_time_string);
  RPG_Common_Tools::period2String (system_time,
                                   system_time_string);

  // debug info
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              user_time_string.c_str (),
              system_time_string.c_str ()));
#endif

// *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
