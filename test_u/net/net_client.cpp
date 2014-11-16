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

#include <string>
#include <iostream>
#include <sstream>
#include <list>

#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Version.h"
#include "ace/Get_Opt.h"
#include "ace/Profile_Timer.h"
#include "ace/Signal.h"
#include "ace/Sig_Handler.h"
#include "ace/Connector.h"
#include "ace/SOCK_Connector.h"
#include "ace/High_Res_Timer.h"

#ifdef RPG_ENABLE_VALGRIND_SUPPORT
#include "valgrind/valgrind.h"
#endif

#include "rpg_dice.h"

#include "rpg_common_macros.h"
#include "rpg_common.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_timer_manager.h"

#include "rpg_stream_allocatorheap.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_connection_manager.h"
#include "rpg_net_stream_messageallocator.h"
#include "rpg_net_module_eventhandler.h"

#include "rpg_net_client_defines.h"
#include "rpg_net_client_connector.h"
#include "rpg_net_client_asynchconnector.h"

#include "rpg_client_defines.h"
#include "rpg_client_GTK_manager.h"
#include "rpg_client_logger.h"
#include "rpg_client_ui_tools.h"

#include "rpg_net_server_defines.h"

#include "rpg_config.h"

#include "net_defines.h"
#include "net_common.h"
#include "net_callbacks.h"
#include "net_eventhandler.h"

#include "net_client_timeouthandler.h"
#include "net_client_signalhandler.h"

void
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif // #ifdef BASEDIR

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
  std::cout << ACE_TEXT("-g [[STRING]]: UI file [\"")
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
            << RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-y           : run stress-test [")
            << false
            << ACE_TEXT("]")
            << std::endl;
}

bool
do_processArguments(const int& argc_in,
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
	RPG_TRACE(ACE_TEXT("::do_processArguments"));

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                           true);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
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
  numDispatchThreads_out  = RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
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
init_signals(const bool& allowUserRuntimeConnect_in,
             ACE_Sig_Set& signals_inout)
{
  RPG_TRACE(ACE_TEXT("::init_signals"));

  // init return value(s)
  if (signals_inout.empty_set() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Sig_Set::empty_set(): \"%m\", aborting\n")));

    return;
  } // end IF

  // *PORTABILITY*: on Windows most signals are not defined,
  // and ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  signals_inout.sig_add(SIGINT);         // 2       /* interrupt */
  signals_inout.sig_add(SIGILL);         // 4       /* illegal instruction - invalid function image */
  signals_inout.sig_add(SIGFPE);         // 8       /* floating point exception */
//  signals_inout.sig_add(SIGSEGV);        // 11      /* segment violation */
  signals_inout.sig_add(SIGTERM);        // 15      /* Software termination signal from kill */
  if (allowUserRuntimeConnect_in)
    signals_inout.sig_add(SIGBREAK);     // 21      /* Ctrl-Break sequence */
  signals_inout.sig_add(SIGABRT);        // 22      /* abnormal termination triggered by abort call */
  signals_inout.sig_add(SIGABRT_COMPAT); // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  if (signals_inout.fill_set() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Sig_Set::fill_set(): \"%m\", aborting\n")));

    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_inout.sig_del(SIGKILL);        // 9       /* Kill signal */
  signals_inout.sig_del(SIGSTOP);        // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeConnect_in)
    signals_inout.sig_del(SIGUSR1);      // 10      /* User-defined signal 1 */
  // *NOTE* core dump on SIGSEGV
  signals_inout.sig_del(SIGSEGV);        // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_inout.sig_del(SIGPIPE);        // 12      /* Broken pipe: write to pipe with no readers */

#ifdef RPG_ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_inout.sig_del(SIGRTMAX);     // 64
#endif
#endif
}

void
do_work(Net_Client_TimeoutHandler::ActionMode_t actionMode_in,
        unsigned int maxNumConnections_in,
        bool hasUI_in,
        const std::string& serverHostname_in,
        unsigned int connectionInterval_in,
        unsigned short serverPortNumber_in,
        bool useReactor_in,
        unsigned int serverPingInterval_in,
        unsigned int numDispatchThreads_in,
        bool useUDP_in,
        Net_GTK_CBData_t& CBData_in,
        ACE_Sig_Set& signalSet_inout,
        RPG_Common_SignalActions_t& previousSignalActions_inout)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0b: init randomization
  try
  {
    RPG_Dice::init();
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Dice::init, returning\n")));
    return;
  }

  // step0c: init stream configuration object
  Net_EventHandler ui_event_handler(&CBData_in);
  RPG_Net_Module_EventHandler_Module event_handler(std::string("EventHandler"),
                                                   NULL);
  RPG_Net_Module_EventHandler* eventHandler_impl = NULL;
  eventHandler_impl =
      dynamic_cast<RPG_Net_Module_EventHandler*>(event_handler.writer());
  if (!eventHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_EventHandler> failed, aborting\n")));
    return;
  } // end IF
  eventHandler_impl->init(&CBData_in.subscribers,
                          &CBData_in.lock);
  eventHandler_impl->subscribe(&ui_event_handler);
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_StreamMessageAllocator messageAllocator(RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                  &heapAllocator);
  RPG_Net_ConfigPOD configuration;
  ACE_OS::memset(&configuration, 0, sizeof(configuration));
  // ************ connection config data ************
  configuration.peerPingInterval =
      ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? 0
                                                                   : serverPingInterval_in);
  configuration.pingAutoAnswer = true;
  configuration.printPongMessages = true;
  configuration.socketBufferSize = RPG_NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;
  configuration.messageAllocator = &messageAllocator;
  configuration.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
//  config.useThreadPerConnection = false;
//  config.serializeOutput = false;
  // ************ stream config data ************
//  config.notificationStrategy = NULL;
  configuration.module = (hasUI_in ? &event_handler
                                   : NULL);
//  config.delete_module = false;
  // *WARNING*: set at runtime, by the appropriate connection handler
//  config.sessionID = 0; // (== socket handle !)
//  config.statisticsReportingInterval = 0; // == off
//	config.printFinalReport = false;
  // ************ runtime data ************
//	config.currentStatistics = {};
//	config.lastCollectionTimestamp = ACE_Time_Value::zero;

  // step0d: init event dispatch
  if (!RPG_Net_Common_Tools::initEventDispatch(useReactor_in,
                                               numDispatchThreads_in,
                                               configuration.serializeOutput))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init event dispatch, aborting\n")));
    return;
  } // end IF

  // step1: init client connector
  RPG_Net_Client_IConnector* connector = NULL;
  if (useReactor_in)
    ACE_NEW_NORETURN(connector, RPG_Net_Client_Connector());
  else
    ACE_NEW_NORETURN(connector, RPG_Net_Client_AsynchConnector());
  if (!connector)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));
    return;
  } // end IF

  // step2: init connection manager
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max());
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->set(configuration); // will be passed to all handlers

  // step3: init action timer ?
  ACE_INET_Addr peer_address(serverPortNumber_in,
                             serverHostname_in.c_str(),
                             AF_INET);
  Net_Client_TimeoutHandler timeout_handler((hasUI_in ? Net_Client_TimeoutHandler::ACTION_STRESS
                                                      : actionMode_in),
                                            maxNumConnections_in,
                                            peer_address,
                                            connector);
  CBData_in.timeout_handler = &timeout_handler;
  CBData_in.timer_id = -1;
  if (!hasUI_in)
  {
    // schedule action interval timer
    ACE_Event_Handler* event_handler = &timeout_handler;
    ACE_Time_Value interval(((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? (NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000)
                                                                                         : connectionInterval_in),
                            ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000)
                                                                                         : 0));
    CBData_in.timer_id =
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(event_handler,                       // event handler
                                                              NULL,                                // ACT
                                                              RPG_COMMON_TIME_POLICY() + interval, // first wakeup time
                                                              interval);                           // interval
    if (CBData_in.timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule action timer: \"%m\", aborting\n")));

      // clean up
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      delete connector;

      return;
    } // end IF
  } // end IF

  // step4: init signal handling
  Net_Client_SignalHandler signal_handler(CBData_in.timer_id, // action timer id
                                          peer_address,       // remote SAP
                                          connector,          // connector
                                          useReactor_in);     // use reactor ?
  if (!RPG_Common_Tools::initSignals(signalSet_inout,
                                     &signal_handler,
                                     previousSignalActions_inout))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init signal handling, aborting\n")));

    // clean up
    if (CBData_in.timer_id != -1)
    {
      const void* act = NULL;
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(CBData_in.timer_id,
                                                                &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   CBData_in.timer_id));
    } // end IF
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;

    return;
  } // end IF

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step5a: start GTK event loop ?
  if (hasUI_in)
  {
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->start();
    if (!RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->isRunning())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to start GTK event dispatch, aborting\n")));

      // clean up
      if (CBData_in.timer_id != -1)
      {
        const void* act = NULL;
        if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(CBData_in.timer_id,
                                                                  &act) <= 0)
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                     CBData_in.timer_id));
      } // end IF
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      connector->abort();
      delete connector;
      RPG_Common_Tools::finiSignals(signalSet_inout,
                                    useReactor_in,
                                    previousSignalActions_inout);

      return;
    } // end IF
  } // end IF

  // step5b: init worker(s)
  int group_id = -1;
  if (!RPG_Net_Common_Tools::startEventDispatch(useReactor_in,
                                                numDispatchThreads_in,
                                                group_id))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start event dispatch, aborting\n")));

    // clean up
    if (CBData_in.timer_id != -1)
    {
      const void* act = NULL;
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(CBData_in.timer_id,
                                                                &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   CBData_in.timer_id));
		} // end IF
//		{ // synch access
//			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//					 iterator != CBData_in.event_source_ids.end();
//					 iterator++)
//				g_source_remove(*iterator);
//		} // end lock scope
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->stop();
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;
    RPG_Common_Tools::finiSignals(signalSet_inout,
                                  useReactor_in,
                                  previousSignalActions_inout);

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started event dispatch...\n")));

  // step5c: connect immediately ?
  if (!hasUI_in && (connectionInterval_in == 0))
    connector->connect(peer_address);

  // *NOTE*: from this point on, we need to clean up any remote connections !

  // step6: dispatch events
  // *NOTE*: when using a thread pool, handle things differently...
  if (numDispatchThreads_in > 1)
	{
    if (ACE_Thread_Manager::instance()->wait_grp(group_id) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                 group_id));
	} // end IF
  else
  {
    if (useReactor_in)
    {
/*      // *WARNING*: restart system calls (after e.g. SIGINT) for the reactor
      ACE_Reactor::instance()->restart(1);
*/
      if (ACE_Reactor::instance()->run_reactor_event_loop(0) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to handle events: \"%m\", aborting\n")));
    } // end IF
    else
      if (ACE_Proactor::instance()->proactor_run_event_loop(0) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to handle events: \"%m\", aborting\n")));
  } // end ELSE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished event dispatch...\n")));

  // step7: clean up
  // *NOTE*: any action timer has been cancelled, connections have been
  // aborted and any GTK event dispatcher has returned by now...
  RPG_Common_Tools::finiSignals(signalSet_inout,
                                useReactor_in,
                                previousSignalActions_inout);
//  { // synch access
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//				 iterator != CBData_in.event_source_ids.end();
//				 iterator++)
//			g_source_remove(*iterator);
//	} // end lock scope
//  RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->stop();
  RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
  delete connector;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

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
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step0: init
// *PORTABILITY*: on Windows, need to init ACE...
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

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
#if defined(DEBUG_DEBUGGER)
  configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
#endif // #ifdef BASEDIR

  // step1a set defaults
  Net_Client_TimeoutHandler::ActionMode_t action_mode = Net_Client_TimeoutHandler::ACTION_NORMAL;
  bool alternating_mode                               = false;
  unsigned int max_num_connections                    = NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  //  path += ACE_TEXT_ALWAYS_CHAR("net");
  //  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  //#endif
  path += ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_FILE);
  std::string UI_file                                 = path;
  std::string server_hostname                         = NET_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned int connection_interval                    = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  bool log_to_file                                    = false;
  unsigned short server_port_number                   = RPG_NET_SERVER_DEFAULT_LISTENING_PORT;
  bool use_reactor                                    = RPG_NET_USES_REACTOR;
  unsigned int server_ping_interval                   = NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  bool trace_information                              = false;
  bool use_UDP                                        = false;
  bool print_version_and_exit                         = false;
  unsigned int num_dispatch_threads                   = RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
  bool run_stress_test                                = false;

  // step1b: parse/process/validate configuration
  if (!do_processArguments(argc_in,
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
    do_printUsage(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  if ((!UI_file.empty() && (alternating_mode || run_stress_test))                      ||
      (run_stress_test && ((server_ping_interval != 0) || (connection_interval != 0))) ||
      (alternating_mode && run_stress_test))
	{
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid arguments, aborting\n")));

    do_printUsage(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF
  if (!UI_file.empty() &&
      !RPG_Common_File_Tools::isReadable(UI_file))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid UI definition file (was: %s), aborting\n"),
               ACE_TEXT(UI_file.c_str())));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
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
  ACE_Sig_Set signal_set(0);
  init_signals((connection_interval == 0), // allow SIGUSR1/SIGBREAK IFF
                                           // regular connections are off
               signal_set);
  RPG_Common_SignalActions_t previous_signal_actions;
  if (!RPG_Common_Tools::preInitSignals(signal_set,
                                        use_reactor,
                                        previous_signal_actions))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::preInitSignals(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  Net_GTK_CBData_t gtk_cb_user_data;
  // step1e: initialize logging and/or tracing
  RPG_Client_Logger logger(&gtk_cb_user_data.log_stack,
                           &gtk_cb_user_data.lock);
  std::string log_file;
  if (log_to_file)
    log_file = RPG_Common_File_Tools::getLogFilename(ACE::basename(argv_in[0]));
  if (!RPG_Common_Tools::initLogging(ACE::basename(argv_in[0]),   // program name
                                     log_file,                    // logfile
                                     false,                       // log to syslog ?
                                     false,                       // trace messages ?
                                     trace_information,           // debug messages ?
                                     (UI_file.empty() ? NULL
                                                      : &logger))) // logger ?
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1f: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_SUCCESS;
  } // end IF

  // step1g: set process resource limits
  // *NOTE*: settings will be inherited by any child processes
  // *TODO*: the reasoning here is incomplete
  bool use_fd_based_reactor = use_reactor;
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  use_fd_based_reactor = (use_reactor && !RPG_COMMON_USE_WFMO_REACTOR);
#endif
  if (!RPG_Common_Tools::initResourceLimits(use_fd_based_reactor, // file descriptors
                                            true))                // stack traces
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initResourceLimits(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF

  // step1h: init GLIB / G(D|T)K[+] / GNOME ?
	Net_GTKUIDefinition gtk_initializer(Net_GTKUIDefinition::ROLE_CLIENT,
                                      true,
                                      &gtk_cb_user_data);
  if (!UI_file.empty())
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->init(argc_in,
                                                       argv_in,
                                                       UI_file,
                                                       &gtk_initializer);

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work(action_mode,
          max_num_connections,
          !UI_file.empty (),
          server_hostname,
          connection_interval,
          server_port_number,
          use_reactor,
          server_ping_interval,
          num_dispatch_threads,
          use_UDP,
          gtk_cb_user_data,
          signal_set,
          previous_signal_actions);
  timer.stop();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             ACE_TEXT(working_time_string.c_str())));

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

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif
    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset(&elapsed_rusage, 0, sizeof(elapsed_rusage));
  process_profile.elapsed_rusage(elapsed_rusage);
  ACE_Time_Value user_time(elapsed_rusage.ru_utime);
  ACE_Time_Value system_time(elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  RPG_Common_Tools::period2String(user_time,
                                  user_time_string);
  RPG_Common_Tools::period2String(system_time,
                                  system_time_string);

  // debug info
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
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
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str()));
#endif

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
