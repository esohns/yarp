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

#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <ace/High_Res_Timer.h>

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_timer_manager.h"

#include "rpg_stream_allocatorheap.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_connection_manager.h"
#include "rpg_net_stream_messageallocator.h"

#include "rpg_net_client_defines.h"
#include "rpg_net_client_connector.h"
#include "rpg_net_client_asynchconnector.h"

#include "rpg_net_server_defines.h"

#include "net_common.h"
#include "net_client_timeouthandler.h"
#include "net_client_signalhandler.h"

#define NET_CLIENT_DEF_SERVER_HOSTNAME         ACE_LOCALHOST
#define NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL 0
#define NET_CLIENT_DEF_SERVER_PING_INTERVAL    0

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-h [STRING]: server (host)name [\"") << NET_CLIENT_DEF_SERVER_HOSTNAME << "\"]" << std::endl;
  std::cout << ACE_TEXT("-i [VALUE] : connection interval ([") << NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL << ACE_TEXT("] second(s))") << std::endl;
  std::cout << ACE_TEXT("-l         : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-p [VALUE] : server port [") << RPG_NET_SERVER_DEF_LISTENING_PORT << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-r         : use reactor") << ACE_TEXT(" [") << RPG_NET_USES_REACTOR << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s [VALUE] : ping interval ([") << NET_CLIENT_DEF_SERVER_PING_INTERVAL << ACE_TEXT("] second(s) {0 --> OFF})") << std::endl;
  std::cout << ACE_TEXT("-t         : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE] : #thread pool threads")  << ACE_TEXT(" [") << RPG_NET_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& serverHostname_out,
                  unsigned int& connectionInterval_out,
                  bool& logToFile_out,
                  unsigned short& serverPortNumber_out,
                  bool& useReactor_out,
                  unsigned int& pingInterval_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  unsigned int& numThreadPoolThreads_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  serverHostname_out = NET_CLIENT_DEF_SERVER_HOSTNAME;
  connectionInterval_out = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  logToFile_out = false;
  serverPortNumber_out = RPG_NET_SERVER_DEF_LISTENING_PORT;
  useReactor_out = RPG_NET_USES_REACTOR;
  pingInterval_out = NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  numThreadPoolThreads_out = RPG_NET_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("h:i:lp:rs:tvx:"),
                             1, // skip command name
                             1, // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS, // ordering
                             0); // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
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
        converter >> pingInterval_out;

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

void
init_signals(const bool& allowUserRuntimeConnect_in,
             std::vector<int>& signals_inout)
{
  RPG_TRACE(ACE_TEXT("::init_signals"));

  // init return value(s)
  signals_inout.clear();

  // init list of handled signals...
  // *PORTABILITY*: on Windows SIGHUP and SIGQUIT are not defined,
  // so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  // *NOTE*: don't handle SIGHUP !!!! --> program will hang !
  //signals_inout.push_back(SIGHUP);
#endif
  signals_inout.push_back(SIGINT);
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  signals_inout.push_back(SIGQUIT);
#endif
//   signals_inout.push_back(SIGILL);
//   signals_inout.push_back(SIGTRAP);
//   signals_inout.push_back(SIGBUS);
//   signals_inout.push_back(SIGFPE);
//   signals_inout.push_back(SIGKILL); // cannot catch this one...
  if (allowUserRuntimeConnect_in)
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
    signals_inout.push_back(SIGUSR1);
#else
    signals_inout.push_back(SIGBREAK);
#endif
//   signals_inout.push_back(SIGSEGV);
  if (allowUserRuntimeConnect_in)
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
    signals_inout.push_back(SIGUSR2);
#else
    signals_inout.push_back(SIGABRT);
#endif
//   signals_inout.push_back(SIGPIPE);
//   signals_inout.push_back(SIGALRM);
  signals_inout.push_back(SIGTERM);
//   signals_inout.push_back(SIGSTKFLT);
//   signals_inout.push_back(SIGCHLD);
//   signals_inout.push_back(SIGCONT);
//   signals_inout.push_back(SIGSTOP); // cannot catch this one...
//   signals_inout.push_back(SIGTSTP);
//   signals_inout.push_back(SIGTTIN);
//   signals_inout.push_back(SIGTTOU);
//   signals_inout.push_back(SIGURG);
//   signals_inout.push_back(SIGXCPU);
//   signals_inout.push_back(SIGXFSZ);
//   signals_inout.push_back(SIGVTALRM);
//   signals_inout.push_back(SIGPROF);
//   signals_inout.push_back(SIGWINCH);
//   signals_inout.push_back(SIGIO);
//   signals_inout.push_back(SIGPWR);
//   signals_inout.push_back(SIGSYS);
//   signals_inout.push_back(SIGRTMIN);
//   signals_inout.push_back(SIGRTMIN+1);
// ...
//   signals_inout.push_back(SIGRTMAX-1);
//   signals_inout.push_back(SIGRTMAX);
}

void
init_signalHandling(const std::vector<int>& signals_in,
                    Net_Client_SignalHandler& eventHandler_in,
                    ACE_Sig_Handlers& signalDispatcher_in,
                    std::vector<ACE_Sig_Action>& previousActions_out,
                    std::vector<int>& sigKeys_out,
                    const bool& useReactor_in)
{
  RPG_TRACE(ACE_TEXT("::init_signalHandling"));

  // init return value(s)
  previousActions_out.clear();
  sigKeys_out.clear();

  // step1: register signal handlers for the list of signals

  // specify (default) action...
  // *IMPORTANT NOTE* don't actually need to keep this around after registration
  ACE_Sig_Action signal_action(static_cast<ACE_SignalHandler>(SIG_DFL), // default action
                               ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
                                                                        // --> block them all (bar KILL/STOP; see manual)
                               (SA_RESTART | SA_SIGINFO));              // default flags

  // register different signals...
  int sig_key = -1;
  ACE_Event_Handler* previous_handler = NULL;
  for (std::vector<int>::const_iterator iterator = signals_in.begin();
       iterator != signals_in.end();
       iterator++)
  {
    sig_key = -1;
    previous_handler = NULL;
    ACE_Sig_Action previous_action;
    sig_key = signalDispatcher_in.register_handler(*iterator,         // signal
                                                   &eventHandler_in,  // new handler
                                                   &signal_action,    // new action
                                                   &previous_handler, // previous handler
                                                   &previous_action); // previous action
    if (sig_key == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Sig_Handlers::register_handler(\"%S\"): \"%m\", continuing\n"),
                 *iterator));

    previousActions_out.push_back(previous_action);
    sigKeys_out.push_back(sig_key);

    // debug info
    if (sig_key != -1)
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("registered handler for \"%S\" (key: %d)...\n"),
                 *iterator,
                 sig_key));
  } // end FOR

  // actually, there is only a single handler for ALL signals in the set...
  // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("handling %d signal(s)...\n"),
//              signals_inout.size()));

  // step2: ignore SIGPIPE: need this to continue gracefully after a client
  // suddenly disconnects (i.e. application/system crash, etc...)
  // --> specify ignore action
  // *IMPORTANT NOTE*: don't actually need to keep this around after registration
  // *NOTE*: do NOT restart system calls in this case (see manual)
  ACE_Sig_Action ignore_action(static_cast<ACE_SignalHandler>(SIG_IGN), // ignore action
                               ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
                                                                        // --> block them all (bar KILL/STOP; see manual)
                               SA_SIGINFO);                             // flags
//                               (SA_RESTART | SA_SIGINFO));              // flags
  ACE_Sig_Action previous_action;
  if (ignore_action.register_action(SIGPIPE, &previous_action) == -1)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_Sig_Action::register_action(SIGPIPE): \"%m\", continuing\n")));
  previousActions_out.push_back(previous_action);
  sigKeys_out.push_back(-1);

  // step3: block SIGRTMIN IFF on Linux AND using the ACE_POSIX_SIG_Proactor (the default)
  // *IMPORTANT NOTE*: proactor implementation collects the signals in dispatching threads
  if (RPG_Common_Tools::isLinux() && !useReactor_in)
  {
    sigset_t signal_set;
    if (ACE_OS::sigemptyset(&signal_set) == - 1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

      return;
    } // end IF
    for (int i = ACE_SIGRTMIN;
         i <= ACE_SIGRTMAX;
         i++)
      if (ACE_OS::sigaddset(&signal_set, i) == -1)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to ACE_OS::sigaddset(): \"%m\", aborting\n")));

        return;
      } // end IF
    if (ACE_OS::thr_sigsetmask(SIG_BLOCK, &signal_set, NULL) == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));

      return;
    } // end IF
  } // end IF
}

void
fini_signalHandling(const std::vector<int>& signals_in,
                    ACE_Sig_Handlers& signalDispatcher_in,
                    const std::vector<ACE_Sig_Action>& actions_in,
                    const std::vector<int> sigKeys_in,
                    const bool& useReactor_in)
{
  RPG_TRACE(ACE_TEXT("::fini_signalHandling"));

  // restore previous signal handlers
  // register different signals...
  std::vector<ACE_Sig_Action>::const_iterator action_iterator = actions_in.begin();
  std::vector<int>::const_iterator key_iterator = sigKeys_in.begin();
  int success = -1;
  for (std::vector<int>::const_iterator iterator = signals_in.begin();
       iterator != signals_in.end();
       iterator++, action_iterator++, key_iterator++)
  {
    success = signalDispatcher_in.remove_handler(*iterator,                                      // signal
                                                 &const_cast<ACE_Sig_Action&>(*action_iterator), // new (== previous) disposition
                                                 NULL,                                           // previous disposition, don't care
                                                 *key_iterator);                                 // sigkey
    if (success == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Sig_Handlers::remove_handler(\"%S\"): \"%m\", continuing\n"),
                 *iterator));
    else
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("restored handler for \"%S\" (key: %d)...\n"),
                 *iterator,
                 *key_iterator));
  } // end FOR

  // restore previous SIGPIPE handler
 action_iterator++; key_iterator++;
 success = signalDispatcher_in.remove_handler(SIGPIPE,                                         // signal
                                              &const_cast<ACE_Sig_Action&>(*action_iterator),  // new (== previous) disposition
                                              NULL,                                            // previous disposition, don't care
                                              *key_iterator);                                  // sigkey
 if (success == -1)
   ACE_DEBUG((LM_ERROR,
              ACE_TEXT("failed to ACE_Sig_Handlers::remove_handler(\"%S\"): \"%m\", continuing\n"),
              SIGPIPE));
 else
   ACE_DEBUG((LM_DEBUG,
              ACE_TEXT("restored handler for \"%S\"...\n"),
              SIGPIPE));
}

void
do_work(const std::string& serverHostname_in,
        const unsigned int& connectionInterval_in,
        const unsigned short& serverPortNumber_in,
        const bool& useReactor_in,
        const unsigned int& pingInterval_in,
        const unsigned int& numThreadPoolThreads_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init client connector
  RPG_Net_Client_IConnector* connector = NULL;
  if (useReactor_in)
    connector = new(std::nothrow) RPG_Net_Client_Connector();
  else
    connector = new(std::nothrow) RPG_Net_Client_AsynchConnector();
  if (!connector)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return;
  } // end IF

  // step2: signal handling
  // event handler for signals
  Net_Client_SignalHandler signal_handler(serverHostname_in,   // target hostname
                                          serverPortNumber_in, // target port
                                          connector);          // connector
  ACE_Sig_Handlers signal_dispatcher;
  // *WARNING*: 'signals' appears to be a keyword in some contexts...
  std::vector<int> signalss;
  init_signals((connectionInterval_in == 0),  // allow SIGUSR1/SIGBREAK IF regular connections are off
               signalss);
  std::vector<ACE_Sig_Action> previous_actions;
  std::vector<int> sig_keys;
  init_signalHandling(signalss,
                      signal_handler,
                      signal_dispatcher,
                      previous_actions,
                      sig_keys,
                      useReactor_in);

  // step3a: init stream configuration object
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_StreamMessageAllocator messageAllocator(RPG_NET_MAX_MESSAGES,
                                                  &heapAllocator);
  RPG_Net_ConfigPOD config;
  ACE_OS::memset(&config, 0, sizeof(RPG_Net_ConfigPOD));
  config.pingInterval = pingInterval_in;
	config.pingAutoAnswer = true;
  config.printPongMessages = true;
  config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &messageAllocator;
  config.defaultBufferSize = RPG_NET_STREAM_BUFFER_SIZE;
  config.useThreadPerConnection = false;
  config.module = NULL; // just use the default stream...
  // *WARNING*: set at runtime, by the appropriate connection handler
  config.sessionID = 0; // (== socket handle !)
  config.statisticsReportingInterval = 0; // == off

  // step3b: init connection manager
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max());
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->set(config); // will be passed to all handlers

  // step4a: init timer...
  Net_Client_TimeoutHandler timeout_handler(serverHostname_in,
                                            serverPortNumber_in,
                                            connector);
  long timer_id = -1;
  if (connectionInterval_in)
  {
    // schedule server query interval timer
    ACE_Time_Value interval(connectionInterval_in, 0);
    timer_id = RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&timeout_handler,
                                                                       NULL,
                                                                       ACE_OS::gettimeofday () + interval,
                                                                       interval);
    if (timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule timer: \"%m\", aborting\n")));

      // clean up
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      fini_signalHandling(signalss,
                          signal_dispatcher,
                          previous_actions,
                          sig_keys,
                          useReactor_in);

      return;
    } // end IF
  } // end IF
  else
  {
    // step4b: ...or connect to the server immediately
    ACE_INET_Addr peer_address(serverPortNumber_in,
                               serverHostname_in.c_str());
		connector->connect(peer_address);
  } // end ELSE

  // event loop:
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step4a: init worker(s)
  int group_id = -1;
  if (!RPG_Net_Common_Tools::initEventDispatch(useReactor_in,
                                               numThreadPoolThreads_in,
                                               group_id))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init event dispatch, aborting\n")));

    // clean up
    if (connectionInterval_in)
    {
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(timer_id, NULL) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   timer_id));
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    } // end IF
    fini_signalHandling(signalss,
                        signal_dispatcher,
                        previous_actions,
                        sig_keys,
                        useReactor_in);

    return;
  } // end IF

  // *NOTE*: from this point on, we need to clean up any remote connections !

  // *NOTE*: when using a thread pool, handle things differently...
  if (numThreadPoolThreads_in > 1)
		RPG_Net_Common_Tools::finiEventDispatch(false,
																						false,
																						group_id);
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

  // step4: clean up
  if (connectionInterval_in)
  {
    if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(timer_id, NULL) <= 0)
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                 timer_id));
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
  } // end IF
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();
  fini_signalHandling(signalss,
                      signal_dispatcher,
                      previous_actions,
                      sig_keys,
                      useReactor_in);

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
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
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
ACE_TMAIN(int argc,
          ACE_TCHAR* argv[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  // step1: init
//  // *PORTABILITY*: on Windows, we need to init ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (ACE::init() == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));
//
//    return EXIT_FAILURE;
//  } // end IF
//#endif

  // step1a set defaults
  std::string serverHostname        = NET_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned int connectionInterval   = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  bool logToFile                    = false;
  unsigned short serverPortNumber   = RPG_NET_SERVER_DEF_LISTENING_PORT;
  bool useReactor                   = RPG_NET_USES_REACTOR;
  unsigned int pingInterval         = NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  bool traceInformation             = false;
  bool printVersionAndExit          = false;
  unsigned int numThreadPoolThreads = RPG_NET_CLIENT_DEF_NUM_TP_THREADS;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          serverHostname,
                          connectionInterval,
                          logToFile,
                          serverPortNumber,
                          useReactor,
                          pingInterval,
                          traceInformation,
                          printVersionAndExit,
                          numThreadPoolThreads)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  if (serverPortNumber <= 1023)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("using (privileged) port #: %d...\n"),
               serverPortNumber));

//     // make 'em learn...
//     print_usage(std::string(ACE::basename(argv[0])));
    //
//     return EXIT_FAILURE;
  } // end IF

  // step1d: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_TRACE |  // <-- DISABLE trace messages !
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

  // step1e: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv[0])));

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work(serverHostname,
          connectionInterval,
          serverPortNumber,
          useReactor,
          pingInterval,
          numThreadPoolThreads);
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

//  // *PORTABILITY*: on Windows, we must fini ACE...
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (ACE::fini() == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
//
//    return EXIT_FAILURE;
//  } // end IF
//#endif

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
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str()));
#endif

  return EXIT_SUCCESS;
} // end main
