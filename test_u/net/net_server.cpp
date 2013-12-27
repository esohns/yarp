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
#include <fstream>
#include <sstream>
#include <vector>

#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Proactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/High_Res_Timer.h>

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_tools.h"

#include "rpg_stream_allocatorheap.h"

#include "rpg_net_defines.h"
#include "rpg_net_common.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_stream_messageallocator.h"

#include "rpg_net_server_defines.h"
#include "rpg_net_server_listener.h"
#include "rpg_net_server_asynchlistener.h"
#include "rpg_net_server_common_tools.h"

#include "net_server_signalhandler.h"

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
	std::cout << ACE_TEXT("-c [VALUE]  : #connections ([") << RPG_NET_SERVER_MAX_NUM_OPEN_CONNECTIONS << ACE_TEXT("])") << std::endl;
  std::cout << ACE_TEXT("-i [VALUE]  : client ping interval ([") << RPG_NET_SERVER_DEF_CLIENT_PING_INTERVAL << ACE_TEXT("] second(s) {0 --> OFF})") << std::endl;
  std::cout << ACE_TEXT("-k [VALUE]  : socket keep-alive timeout ([") << RPG_NET_SOCK_KEEPALIVE << ACE_TEXT("] second(s))") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-n [STRING] : network interface [\"") << ACE_TEXT_ALWAYS_CHAR(RPG_NET_DEF_CNF_NETWORK_INTERFACE) << ACE_TEXT("\"]") << std::endl;
	// *TODO*: this doesn't really make sense (yet)
	std::cout << ACE_TEXT("-o          : use loopback [") << false << ACE_TEXT("]") << std::endl;
	std::cout << ACE_TEXT("-p [VALUE]  : listening port [") << RPG_NET_SERVER_DEF_LISTENING_PORT << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-r          : use reactor [") << RPG_NET_USES_REACTOR << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s [VALUE]  : statistics reporting interval ([") << RPG_NET_SERVER_DEF_STATISTICS_REPORTING_INTERVAL << ACE_TEXT("] second(s) {0 --> OFF})") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]  : #thread pool threads [") << RPG_NET_SERVER_DEF_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
									unsigned int& maxNumConnections_out,
                  unsigned int& clientPingInterval_out,
                  unsigned int& keepAliveTimeout_out,
                  bool& logToFile_out,
                  std::string& networkInterface_out,
									bool& useLoopback_out,
                  unsigned short& listeningPortNumber_out,
                  bool& useReactor_out,
                  unsigned int& statisticsReportingInterval_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  unsigned int& numThreadPoolThreads_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
	maxNumConnections_out = RPG_NET_SERVER_MAX_NUM_OPEN_CONNECTIONS;
  clientPingInterval_out = RPG_NET_SERVER_DEF_CLIENT_PING_INTERVAL;
  keepAliveTimeout_out = RPG_NET_SOCK_KEEPALIVE;
  logToFile_out = false;
  networkInterface_out = ACE_TEXT_ALWAYS_CHAR(RPG_NET_DEF_CNF_NETWORK_INTERFACE);
	useLoopback_out = false;
  listeningPortNumber_out = RPG_NET_SERVER_DEF_LISTENING_PORT;
	useReactor_out = RPG_NET_USES_REACTOR;
  statisticsReportingInterval_out = RPG_NET_SERVER_DEF_STATISTICS_REPORTING_INTERVAL;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  numThreadPoolThreads_out = RPG_NET_SERVER_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:i:k:ln:op:rs:tvx:"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> maxNumConnections_out;

        break;
      }
      case 'i':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> clientPingInterval_out;

        break;
      }
      case 'k':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> keepAliveTimeout_out;

        break;
      }
      case 'l':
      {
        logToFile_out = true;

        break;
      }
      case 'n':
      {
        networkInterface_out = argumentParser.opt_arg();

        break;
      }
      case 'o':
      {
        useLoopback_out = true;

        break;
      }
      case 'p':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
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
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
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
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> numThreadPoolThreads_out;

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
                   ACE_TEXT("unrecognized option \"%c\", continuing\n"),
                   option));

        break;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

bool
init_fileLogging(std::ofstream& stream_in)
{
  RPG_TRACE(ACE_TEXT("::init_fileLogging"));

  // retrieve fully-qualified (FQ) filename
  std::string logfilename;
  if (!RPG_Net_Server_Common_Tools::getNextLogFilename(std::string(RPG_COMMON_DEF_LOG_DIRECTORY),
                                                       logfilename))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Common_Tools::getNextLogFilename(), aborting\n")));

    return false;
  } // end IF

  // create/open the file
  stream_in.open(logfilename.c_str(),
                 (ios::out | ios::trunc));
  if (!stream_in.is_open())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to std::ofstream::open() file \"%s\": \"%m\", aborting\n"),
               logfilename.c_str()));

    return false;
  } // end IF

  // init [VERBOSE] logging to a logfile AND stderr...
  //ACE_LOG_MSG->set_flags(ACE_Log_Msg::VERBOSE_LITE);
  ACE_LOG_MSG->msg_ostream(&stream_in, 0);
  ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("logging to file: \"%s\"\n"),
             logfilename.c_str()));

  return true;
}

bool
init_coreDumping()
{
  RPG_TRACE(ACE_TEXT("::init_coreDumping"));

  // step1: retrieve current values
  // *PORTABILITY*: this is entirely un-portable so we do an ugly hack...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  rlimit core_limit;
  if (ACE_OS::getrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("corefile limits (before) [soft: \"%u\", hard: \"%u\"]...\n"),
//              core_limit.rlim_cur,
//              core_limit.rlim_max));

  // set soft/hard limits to unlimited...
  core_limit.rlim_cur = RLIM_INFINITY;
  core_limit.rlim_max = RLIM_INFINITY;
  if (ACE_OS::setrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setrlimit(RLIMIT_CORE): \"%m\", aborting\n")));

    return false;
  } // end IF

  // verify...
  if (ACE_OS::getrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%m\", aborting\n")));

    return false;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("unset corefile limits [soft: %u, hard: %u]...\n"),
             core_limit.rlim_cur,
             core_limit.rlim_max));
#else
  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("corefile limits have not been implemented on this platform, continuing\n")));
#endif

  return true;
}

void
init_signals(const bool& allowUserRuntimeStats_in,
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_inout.sig_add(SIGINT);         // 2       /* interrupt */
	signals_inout.sig_add(SIGILL);         // 4       /* illegal instruction - invalid function image */
	signals_inout.sig_add(SIGFPE);         // 8       /* floating point exception */
	signals_inout.sig_add(SIGSEGV);        // 11      /* segment violation */
	signals_inout.sig_add(SIGTERM);        // 15      /* Software termination signal from kill */
  signals_inout.sig_add(SIGBREAK);       // 21      /* Ctrl-Break sequence */
	signals_inout.sig_add(SIGABRT);        // 22      /* abnormal termination triggered by abort call */
	signals_inout.sig_add(SIGABRT_COMPAT); // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
	if (signals_inout.fill_set() == -1)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to ACE_Sig_Set::fill_set(): \"%m\", aborting\n")));

		return;
	} // end IF
#endif

//  // init list of handled signals...
//  // *PORTABILITY*: on Windows SIGHUP and SIGQUIT are not defined,
//  // so we handle SIGBREAK (21) and SIGABRT (22) instead...
//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//  // *NOTE*: don't handle SIGHUP !!!! --> program will hang !
//  //signals_inout.sig_add(SIGHUP);
//#endif
//  signals_inout.sig_add(SIGINT);
//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//  signals_inout.sig_add(SIGQUIT);
//#endif
////   signals_inout.sig_add(SIGILL);
////   signals_inout.sig_add(SIGTRAP);
//  signals_inout.sig_add(SIGABRT);
////   signals_inout.sig_add(SIGBUS);
////   signals_inout.sig_add(SIGFPE);
////   signals_inout.sig_add(SIGKILL); // cannot catch this one...
//  if (allowUserRuntimeStats_in)
//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//    signals_inout.sig_add(SIGUSR1);
//#else
//    signals_inout.sig_add(SIGBREAK);
//#endif
////   signals_inout.sig_add(SIGSEGV);
////   signals_inout.sig_add(SIGUSR2);
////   signals_inout.sig_add(SIGPIPE);
////   signals_inout.sig_add(SIGALRM);
//  signals_inout.sig_add(SIGTERM);
////   signals_inout.sig_add(SIGSTKFLT);
////   signals_inout.sig_add(SIGCHLD);
////   signals_inout.sig_add(SIGCONT);
////   signals_inout.sig_add(SIGSTOP); // cannot catch this one...
////   signals_inout.sig_add(SIGTSTP);
////   signals_inout.sig_add(SIGTTIN);
////   signals_inout.sig_add(SIGTTOU);
////   signals_inout.sig_add(SIGURG);
////   signals_inout.sig_add(SIGXCPU);
////   signals_inout.sig_add(SIGXFSZ);
////   signals_inout.sig_add(SIGVTALRM);
////   signals_inout.sig_add(SIGPROF);
////   signals_inout.sig_add(SIGWINCH);
////   signals_inout.sig_add(SIGIO);
////   signals_inout.sig_add(SIGPWR);
////   signals_inout.sig_add(SIGSYS);
////   signals_inout.sig_add(SIGRTMIN);
////   signals_inout.sig_add(SIGRTMIN+1);
//// ...
////   signals_inout.sig_add(SIGRTMAX-1);
////   signals_inout.sig_add(SIGRTMAX);
}

void
init_signalHandling(ACE_Sig_Set& signals_in,
                    Net_Server_SignalHandler& eventHandler_in,
                    const bool& useReactor_in)
{
  RPG_TRACE(ACE_TEXT("::init_signalHandling"));

	// *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a multithreaded
  //                   application, the disposition of a particular signal is the same for
  //                   all threads." (see man 7 signal)

  // step1: ignore SIGPIPE: need this to continue gracefully after a client
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

  // step2: block SIGRTMIN IFF on Linux AND using the ACE_POSIX_SIG_Proactor (the default)
  // *IMPORTANT NOTE*: proactor implementation dispatches the signals in worker thread(s)
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
		{
      if (ACE_OS::sigaddset(&signal_set, i) == -1)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to ACE_OS::sigaddset(): \"%m\", aborting\n")));

        return;
      } // end IF
			if (signals_in.sig_del(i) == -1)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to ACE_Sig_Set::sig_del(%S): \"%m\", aborting\n"),
									 i));

        return;
      } // end IF
		} // end IF
    if (ACE_OS::thr_sigsetmask(SIG_BLOCK, &signal_set, NULL) == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));

      return;
    } // end IF
  } // end IF

  // *IMPORTANT NOTE*: don't actually need to keep this around after registration
  ACE_Sig_Action new_action(static_cast<ACE_SignalHandler>(SIG_DFL), // default action
                            ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
                                                                     // --> block them all (bar KILL/STOP; see manual)
                            (SA_RESTART | SA_SIGINFO));              // flags
	if (ACE_Reactor::instance()->register_handler(signals_in,
		                                            &eventHandler_in,
																								&new_action) == -1)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to ACE_Reactor::register_handler(): \"%m\", aborting\n")));

		return;
	} // end IF
}

void
fini_signalHandling(ACE_Sig_Set& signals_in,
                    const bool& useReactor_in)
{
  RPG_TRACE(ACE_TEXT("::fini_signalHandling"));

  // step1: reset SIGPIPE handling to default behaviour
  // *IMPORTANT NOTE*: don't actually need to keep this around after registration
  // *NOTE*: do NOT restart system calls in this case (see manual)
  ACE_Sig_Action default_action(static_cast<ACE_SignalHandler>(SIG_DFL), // default action
                                ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
                                                                         // --> block them all (bar KILL/STOP; see manual)
                                SA_SIGINFO);                             // flags
//                               (SA_RESTART | SA_SIGINFO));              // flags
  ACE_Sig_Action previous_action;
  if (default_action.register_action(SIGPIPE, &previous_action) == -1)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_Sig_Action::register_action(SIGPIPE): \"%m\", continuing\n")));

  // step2: unblock SIGRTMIN IFF on Linux AND using the ACE_POSIX_SIG_Proactor (the default)
  // *IMPORTANT NOTE*: proactor implementation dispatches the signals in worker thread(s)
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
    if (ACE_OS::thr_sigsetmask(SIG_UNBLOCK, &signal_set, NULL) == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to ACE_OS::thr_sigsetmask(): \"%m\", aborting\n")));

      return;
    } // end IF
  } // end IF

  // restore previous signal handlers
	if (ACE_Reactor::instance()->remove_handler(signals_in) == -1)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to ACE_Reactor::remove_handler(): \"%m\", aborting\n")));

		return;
	} // end IF
}

void
do_work(const unsigned int& maxNumConnections_in,
        const unsigned int& pingInterval_in,
        const std::string& networkInterface_in,
				const bool& useLoopback_in,
        const unsigned short& listeningPortNumber_in,
        const bool& useReactor_in,
        const unsigned int& statisticsReportingInterval_in,
        const unsigned int& numThreadPoolThreads_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init regular (global) stats reporting
  long timer_id = -1;
  RPG_Net_StatisticHandler_Reactor_t statistics_handler(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance(),
                                                        RPG_Net_StatisticHandler_Reactor_t::ACTION_REPORT);
  if (statisticsReportingInterval_in)
  {
    ACE_Time_Value interval(statisticsReportingInterval_in, 0);
    timer_id = RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&statistics_handler,
                                                                       NULL,
                                                                       ACE_OS::gettimeofday () + interval,
                                                                       interval);
    if (timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule timer: \"%m\", aborting\n")));

      // clean up
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();

      return;
    } // end IF
  } // end IF

  // step2: signal handling
	RPG_Net_Server_IListener* listener_handle = NULL;
	if (useReactor_in)
		listener_handle = RPG_NET_SERVER_LISTENER_SINGLETON::instance();
	else
		listener_handle = RPG_NET_SERVER_ASYNCHLISTENER_SINGLETON::instance();
  // event handler for signals
  Net_Server_SignalHandler signal_handler(timer_id,
		                                      listener_handle,
                                          RPG_NET_CONNECTIONMANAGER_SINGLETON::instance());
	ACE_Sig_Set signal_set(0);
	init_signals((statisticsReportingInterval_in == 0), // allow SIGUSR1/SIGBREAK IF regular reporting is off
               signal_set);
  init_signalHandling(signal_set,
                      signal_handler,
                      useReactor_in);

  // step3a: init stream configuration object
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_StreamMessageAllocator messageAllocator(RPG_NET_MAX_MESSAGES,
                                                  &heapAllocator);
  RPG_Net_ConfigPOD config;
  ACE_OS::memset(&config, 0, sizeof(RPG_Net_ConfigPOD));
  config.pingInterval = pingInterval_in;
	config.pingAutoAnswer = true;
  config.printPingMessages = false;
  config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &messageAllocator;
  config.defaultBufferSize = RPG_NET_STREAM_BUFFER_SIZE;
  config.useThreadPerConnection = false;
  config.module = NULL; // just use the default stream...
  // *WARNING*: set at runtime, by the appropriate connection handler
  config.sessionID = 0; // (== socket handle !)
  config.statisticsReportingInterval = 0; // don't do it per stream (see below)...
  // step3b: init connection manager
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->init(maxNumConnections_in);
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->set(config); // will be passed to all handlers

  // step4: handle events (signals, incoming connections/data, timers, ...)
  // event loop:
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (and perform orderly shutdown)
  // - signal connection attempts to acceptor
  // - signal timer expiration to perform maintenance/local statistics reporting

  // step4a: init worker(s)
  int group_id = -1;
  if (!RPG_Net_Common_Tools::initEventDispatch(useReactor_in,
                                               numThreadPoolThreads_in,
                                               group_id))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init event dispatch, aborting\n")));

    // clean up
    if (statisticsReportingInterval_in)
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(timer_id, NULL) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   timer_id));
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    fini_signalHandling(signal_set,
                        useReactor_in);

    return;
  } // end IF

  // step4b: start listening
  listener_handle->init(listeningPortNumber_in,
		                    useLoopback_in);
  listener_handle->start();
  if (!listener_handle->isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start listener (port: %u), aborting\n"),
               listeningPortNumber_in));

    // clean up
    RPG_Net_Common_Tools::finiEventDispatch(useReactor_in,
                                            !useReactor_in,
                                            group_id);
    if (statisticsReportingInterval_in)
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(timer_id, NULL) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   timer_id));
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    fini_signalHandling(signal_set,
                        useReactor_in);

    return;
  } // end IF

  // *NOTE*: from this point on, clean up any remote connections !

  // *NOTE*: when using a thread pool, handle things differently...
  if (numThreadPoolThreads_in)
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

  // clean up
	// *NOTE*: listener has stopped, interval timer has been cancelled,
	// and connections have been aborted...
  fini_signalHandling(signal_set,
                      useReactor_in);
  RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();

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
	unsigned int maxNumConnections           = RPG_NET_SERVER_MAX_NUM_OPEN_CONNECTIONS;
  unsigned int pingInterval                = RPG_NET_SERVER_DEF_CLIENT_PING_INTERVAL;
  unsigned int keepAliveTimeout            = RPG_NET_SOCK_KEEPALIVE;
  bool logToFile                           = false;
  std::string networkInterface             = ACE_TEXT_ALWAYS_CHAR(RPG_NET_DEF_CNF_NETWORK_INTERFACE);
	bool useLoopback                         = false;
  unsigned short listeningPortNumber       = RPG_NET_SERVER_DEF_LISTENING_PORT;
  bool useReactor                          = RPG_NET_USES_REACTOR;
  unsigned int statisticsReportingInterval = RPG_NET_SERVER_DEF_STATISTICS_REPORTING_INTERVAL;
  bool traceInformation                    = false;
  bool printVersionAndExit                 = false;
  unsigned int numThreadPoolThreads        = RPG_NET_SERVER_DEF_NUM_TP_THREADS;

  // step1b: parse/process/validate configuration
  if (!process_arguments(argc,
                         argv,
												 maxNumConnections,
                         pingInterval,
                         keepAliveTimeout,
                         logToFile,
                         networkInterface,
											 	 useLoopback,
                         listeningPortNumber,
                         useReactor,
                         statisticsReportingInterval,
                         traceInformation,
                         printVersionAndExit,
                         numThreadPoolThreads))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  if (listeningPortNumber <= 1023)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("using (privileged) port #: %d...\n"),
               listeningPortNumber));

//     // make 'em learn...
//     print_usage(std::string(ACE::basename(argv[0])));
//
//     return EXIT_FAILURE;
  } // end IF
  else if (numThreadPoolThreads == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("need at least 1 worker thread in the pool...\n")));

    return EXIT_FAILURE;
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

  // step1f: start logging !
  // *NOTE*: the default mode is to log everything to STDERR[/SYSLOG]...
  // *TODO*: ACE::basename(argv[0]) doesn't seem to work here :-(
  if (ACE_LOG_MSG->open(ACE::basename(argv[0]),
//                         ACE_TEXT("net_server"),
                        (ACE_Log_Msg::STDERR/* | ACE_Log_Msg::SYSLOG*/),
                        NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Log_Msg::open(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF

  std::ofstream logstream;
  if (logToFile)
  {
    if (!init_fileLogging(logstream))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_fileLogging(), aborting\n")));

      return EXIT_FAILURE;
    } // end IF

    // don't write to stderr anymore...
    ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR);
  } // end IF

  // step1g: we WILL (try to) coredump !
  // *NOTE*: this setting will be inherited by any child processes (daemon mode)
  if (!init_coreDumping())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init_coreDumping(), aborting\n")));

    return EXIT_FAILURE;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work(maxNumConnections,
		      pingInterval,
          networkInterface,
					useLoopback,
          listeningPortNumber,
          useReactor,
          statisticsReportingInterval,
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
//               ACE_TEXT("failed to ACE::fini(): \"%s\", aborting\n"),
//               ACE_OS::strerror(ACE_OS::last_error())));
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
