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

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <rpg_config.h>
#endif

#include <rpg_net_defines.h>
#include <rpg_net_listener.h>
#include <rpg_net_connection_manager.h>
#include <rpg_net_signalhandler.h>
#include <rpg_net_common_tools.h>
#include <rpg_net_stream_messageallocator.h>

#include <rpg_common_tools.h>

#include <rpg_stream_allocatorheap.h>

#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/High_Res_Timer.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-i [VALUE]  : client ping interval ([") << RPG_NET_DEF_PING_INTERVAL << ACE_TEXT("] second(s))") << std::endl;
  std::cout << ACE_TEXT("-k [VALUE]  : socket keep-alive timeout ([") << RPG_NET_DEF_KEEPALIVE << ACE_TEXT("] second(s))") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-n [STRING] : network interface [\"") << RPG_NET_DEF_CNF_NETWORK_INTERFACE << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-p [VALUE]  : listening port ([") << RPG_NET_DEF_LISTENING_PORT << ACE_TEXT("])") << std::endl;
  std::cout << ACE_TEXT("-s [VALUE]  : statistics reporting interval") << ACE_TEXT(" [") << RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL << ACE_TEXT("] second(s) {0 --> OFF})") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x<[VALUE]> : use thread pool <#threads>")  << ACE_TEXT(" [") << RPG_NET_DEF_SERVER_USES_TP << ACE_TEXT(" : ") << RPG_NET_DEF_SERVER_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  unsigned long& clientPingInterval_out,
                  unsigned long& keepAliveTimeout_out,
                  bool& logToFile_out,
                  std::string& networkInterface_out,
                  unsigned short& listeningPortNumber_out,
                  unsigned long& statisticsReportingInterval_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  bool& useThreadPool_out,
                  unsigned long& numThreadPoolThreads_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  clientPingInterval_out = RPG_NET_DEF_PING_INTERVAL;
  keepAliveTimeout_out = RPG_NET_DEF_KEEPALIVE;
  logToFile_out = false;
  networkInterface_out = RPG_NET_DEF_CNF_NETWORK_INTERFACE;
  listeningPortNumber_out = RPG_NET_DEF_LISTENING_PORT;
  statisticsReportingInterval_out = RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  useThreadPool_out = RPG_NET_DEF_SERVER_USES_TP;
  numThreadPoolThreads_out = RPG_NET_DEF_SERVER_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("i:k:ln:p:s:tvx::"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
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
      case 'p':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> listeningPortNumber_out;

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
        useThreadPool_out = true;
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

const bool
init_fileLogging(std::ofstream& stream_in)
{
  RPG_TRACE(ACE_TEXT("::init_fileLogging"));

  // retrieve fully-qualified (FQ) filename
  std::string logfilename;
  if (!RPG_Net_Common_Tools::getNextLogFilename(true, // we're a server
                                                std::string(RPG_NET_DEF_LOG_DIRECTORY),
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
               ACE_TEXT("failed to std::ofstream::open() file \"%s\": \"%s\", aborting\n"),
               logfilename.c_str(),
               ACE_OS::strerror(ACE_OS::last_error())));

    return false;
  } // end IF

  // init [VERBOSE] logging to a logfile AND stderr...
  //ACE_LOG_MSG->set_flags(ACE_Log_Msg::VERBOSE_LITE);
  ACE_LOG_MSG->msg_ostream(&stream_in, 0);
  ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("logging to file: \"%s\"\n"),
             logfilename.c_str()));

  return true;
}

const bool
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
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return false;
  } // end IF

//   // debug info
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
               ACE_TEXT("failed to ACE_OS::setrlimit(RLIMIT_CORE): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return false;
  } // end IF

  // verify...
  if (ACE_OS::getrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return false;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("unset corefile limits [soft: %u, hard: %u]...\n"),
             core_limit.rlim_cur,
             core_limit.rlim_max));
#else
   // *TODO*
  ACE_DEBUG((LM_WARNING,
             ACE_TEXT("corefile limits on Windows platforms has not been implemented (yet), continuing\n")));
#endif

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

void
init_signals(const bool& allowUserRuntimeStats_in,
             std::vector<int>& signals_inout)
{
  RPG_TRACE(ACE_TEXT("::init_signals"));

  // init return value(s)
  signals_inout.clear();

  // init list of handled signals...
  // *PORTABILITY*: on Windows SIGHUP and SIGQUIT are not defined,
  // so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *NOTE*: don't handle SIGHUP !!!! --> program will hang !
  //signals_inout.push_back(SIGHUP);
#endif
  signals_inout.push_back(SIGINT);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signals_inout.push_back(SIGQUIT);
#endif
//   signals_inout.push_back(SIGILL);
//   signals_inout.push_back(SIGTRAP);
  signals_inout.push_back(SIGABRT);
//   signals_inout.push_back(SIGBUS);
//   signals_inout.push_back(SIGFPE);
//   signals_inout.push_back(SIGKILL); // cannot catch this one...
  if (allowUserRuntimeStats_in)
    signals_inout.push_back(SIGUSR1);
//   signals_inout.push_back(SIGSEGV);
//   signals_inout.push_back(SIGUSR2);
//   signals_inout.push_back(SIGPIPE);
//   signals_inout.push_back(SIGALRM);
  signals_inout.push_back(SIGTERM);
//   signals_inout.push_back(SIGSTKFLT);
//   signals_inout.push_back(SIGCHLD);
//   signals_inout.push_back(SIGCONT);
//   signals_inout.push_back(SIGSTOP); // cannot catch this one...
//   signals_inout.push_back(SIGTSTP);
//   signals_inout.push_back(SIGTTIN);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_inout.push_back(SIGBREAK);
#endif
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

const bool
init_signalHandling(const std::vector<int>& signals_inout,
                    RPG_Net_SignalHandler& eventHandler_in,
                    ACE_Sig_Handlers& signalHandlers_in)
{
  RPG_TRACE(ACE_TEXT("::init_signalHandling"));

  // step1: register signal handlers for the list of signals we want to catch

  // specify (default) action...
  // --> we don't actually need to keep this around after registration
  ACE_Sig_Action signalAction((ACE_SignalHandler)SIG_DFL, // default action (will be overridden below)...
                              ACE_Sig_Set(1),             // mask of signals to be blocked when we're servicing
                                                          // --> block them all ! (except KILL off course...)
//                               (SA_RESTART | SA_SIGINFO)); // flags
                              SA_SIGINFO);               // flags

  // register different signals...
  int sigkey = -1;
  for (std::vector<int>::const_iterator iter = signals_inout.begin();
       iter != signals_inout.end();
       iter++)
  {
    sigkey = signalHandlers_in.register_handler(*iter,            // signal
                                                &eventHandler_in, // new handler
                                                &signalAction,    // new action
                                                NULL,             // old handler
                                                NULL);            // old action
    if (sigkey == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Sig_Handlers::register_handler(\"%S\": %d): \"%m\", aborting\n"),
                 *iter,
                 *iter));

      return false;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("registered handler for \"%S\": %d (key: %d)...\n"),
               *iter,
               *iter,
               sigkey));
  } // end FOR

  // actually, there is only a single handler for ALL signals in the set...
  // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("handling %d signal(s)...\n"),
//              signals_inout.size()));

  // step2: ignore SIGPIPE; need this to enable sending to exit gracefully
  // after an asynchronous client disconnect (i.e. crash/...)
  // specify ignore action...
  // --> we don't actually need to keep this around after registration
  ACE_Sig_Action ignoreAction((ACE_SignalHandler)SIG_IGN, // ignore action...
                              ACE_Sig_Set(1),             // mask of signals to be blocked when we're servicing
                                                          // --> block them all ! (except KILL off course...)
                              SA_SIGINFO); // flags
//                               (SA_RESTART | SA_SIGINFO)); // flags
  ACE_Sig_Action originalAction;
  ignoreAction.register_action(SIGPIPE, &originalAction);

  return true;
}

const bool
init_statisticsReporting(const unsigned long& reportingInterval_in,
                         RPG_Net_StatisticHandler<RPG_Net_RuntimeStatistic>& handler_in)
{
  RPG_TRACE(ACE_TEXT("::init_statisticsReporting"));

  ACE_Time_Value interval(reportingInterval_in, 0);
  long timerID = -1;
  timerID = ACE_Reactor::instance()->schedule_timer(&handler_in,
                                                    NULL,
                                                    interval,
                                                    interval);
  if (timerID == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::schedule_timer(): \"%m\", aborting\n")));

    return false;
  } // end IF

  return true;
}

void
fini_statisticsReporting(RPG_Net_StatisticHandler<RPG_Net_RuntimeStatistic>& handler_in)
{
  RPG_TRACE(ACE_TEXT("::fini_statisticsReporting"));

  if (ACE_Reactor::instance()->cancel_timer(&handler_in, // handler
                                            1) != 1)     // don't call handle_close()
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%m\", returning\n")));
  } // end IF
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
                 ACE_TEXT("(%t) error handling events: \"%p\"\n")));
  } // end WHILE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  // clean up
  no_sigpipe.restore_action(SIGPIPE, original_action);

  return 0;
}

void
do_work(const unsigned long& clientPingInterval_in,
        const std::string& networkInterface_in,
        const unsigned short& listeningPortNumber_in,
        const unsigned long& statisticsReportingInterval_in,
        const bool& useThreadPool_in,
        const unsigned long& numThreadPoolThreads_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // - start listening for connections on a TCP port
  // *NOTE*: need to cancel this for a well-behaved shutdown !

  // step0: (if necessary) init the TP_Reactor
  if (useThreadPool_in)
  {
    if (!init_threadPool())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_threadPool(), aborting\n")));

      return;
    } // end IF
  } // end IF

  // step1a: signal handling
  // event handler for signals
  RPG_Net_SignalHandler signalEventHandler(RPG_NET_LISTENER_SINGLETON::instance(),
                                           RPG_NET_CONNECTIONMANAGER_SINGLETON::instance());
  ACE_Sig_Handlers      signalHandlers;
  // *WARNING*: 'signals' appears to be a keyword in some contexts...
  std::vector<int>      signalss;
  init_signals((statisticsReportingInterval_in == 0), // allow SIGUSR1 IF regular reporting is off
               signalss);
  if (!init_signalHandling(signalss,
                           signalEventHandler,
                           signalHandlers))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init_signalHandling(), aborting\n")));

    return;
  } // end IF

  // step1b: init regular (global) stats reporting
  // event handler for timer
  RPG_Net_StatisticHandler<RPG_Net_RuntimeStatistic> statsHandler(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance(),
                                                                  RPG_Net_StatisticHandler<RPG_Net_RuntimeStatistic>::ACTION_REPORT);
  if (statisticsReportingInterval_in)
  {
    if (!init_statisticsReporting(statisticsReportingInterval_in,
                                  statsHandler))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_statisticsReporting(%u), aborting\n"),
                 statisticsReportingInterval_in));

      return;
    } // end IF
  } // end IF

  // step2a: init stream configuration object
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_StreamMessageAllocator messageAllocator(RPG_NET_DEF_MAX_MESSAGES,
                                                  &heapAllocator);
  RPG_Net_ConfigPOD config;
  ACE_OS::memset(&config,
                  0,
                  sizeof(RPG_Net_ConfigPOD));
  config.clientPingInterval = clientPingInterval_in;
  config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &messageAllocator;
  config.defaultBufferSize = RPG_NET_DEF_NETWORK_BUFFER_SIZE;
  config.module = NULL; // just use the default stream...
  // *WARNING*: set at runtime, by the appropriate connection handler
  config.sessionID = 0; // (== socket handle !)
  config.statisticsReportingInterval = 0; // don't do it per stream (see below)...
  // step2b: init connection manager
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->init(RPG_NET_DEF_MAX_NUM_OPEN_CONNECTIONS);
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->set(config); // will be passed to all handlers

  // step2c: init/start listening
  RPG_NET_LISTENER_SINGLETON::instance()->init(listeningPortNumber_in);
  RPG_NET_LISTENER_SINGLETON::instance()->start();
  if (!RPG_NET_LISTENER_SINGLETON::instance()->isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start listener (port: %u), aborting\n"),
               listeningPortNumber_in));

    return;
  } // end IF

  // *NOTE*: from this point on, we need to potentially also clean up
  //         any remote connections !

  // step2d: handle events (signals, incoming connections/data, timers, ...)
  // event loop:
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (and perform orderly shutdown)
  // - signal connection attempts to acceptor
  // - signal timer expiration to perform maintenance/local statistics reporting

//   // *WARNING*: DON'T restart system calls (after e.g. EINTR) for the reactor
//   ACE_Reactor::instance()->restart(1);

  // *NOTE*: if we use a thread pool, we need to do this differently...
  if (useThreadPool_in)
  {
    // start a (group of) worker thread(s)...
    int grp_id = -1;
    grp_id = ACE_Thread_Manager::instance()->spawn_n(numThreadPoolThreads_in,     // # threads
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
    if (grp_id == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%p\", aborting\n"),
                 numThreadPoolThreads_in));

      // clean up
      // stop listener, clean up pending connections
      RPG_NET_LISTENER_SINGLETON::instance()->stop();
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

      return;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("started group (ID: %u) of %u worker(s)...\n"),
               grp_id,
               numThreadPoolThreads_in));

    // ... and wait for this group to join
    ACE_Thread_Manager::instance()->wait_grp(grp_id);
  } // end IF
  else
  {
    // *NOTE*: asynchronous writing to a closed socket triggers the
    // SIGPIPE signal (default action: abort).
    // --> as this doesn't use select(), guard against this (ignore the signal)
    ACE_Sig_Action no_sigpipe(static_cast<ACE_SignalHandler> (SIG_IGN));
    ACE_Sig_Action original_action;
    no_sigpipe.register_action(SIGPIPE, &original_action);

    if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%p\", aborting\n")));

      // clean up
      // stop listener, clean up pending connections
      RPG_NET_LISTENER_SINGLETON::instance()->stop();
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();
      no_sigpipe.restore_action(SIGPIPE, original_action);

      return;
    } // end IF

    // clean up
    no_sigpipe.restore_action(SIGPIPE, original_action);
  } // end ELSE

  // clean up
  if (statisticsReportingInterval_in)
  {
    fini_statisticsReporting(statsHandler);
  } // end IF
  // *NOTE*: listener should have been stopped by now
  // --> clean up active connections
//   RPG_NET_LISTENER_SINGLETON::instance()->stop();
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in
      << ACE_TEXT(" : ")
      << RPG_VERSION
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
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1a set defaults
  unsigned long clientPingInterval          = RPG_NET_DEF_PING_INTERVAL;
  unsigned long keepAliveTimeout            = RPG_NET_DEF_KEEPALIVE;
  bool logToFile                            = false;
  std::string networkInterface              = RPG_NET_DEF_CNF_NETWORK_INTERFACE;
  unsigned short listeningPortNumber        = RPG_NET_DEF_LISTENING_PORT;
  unsigned long statisticsReportingInterval = RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL;
  bool traceInformation                     = false;
  bool printVersionAndExit                  = false;
  bool useThreadPool                        = RPG_NET_DEF_SERVER_USES_TP;
  unsigned long numThreadPoolThreads        = RPG_NET_DEF_SERVER_NUM_TP_THREADS;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          clientPingInterval,
                          keepAliveTimeout,
                          logToFile,
                          networkInterface,
                          listeningPortNumber,
                          statisticsReportingInterval,
                          traceInformation,
                          printVersionAndExit,
                          useThreadPool,
                          numThreadPoolThreads)))
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
  else if (useThreadPool &&
           (numThreadPoolThreads == 0))
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
               ACE_TEXT("failed to ACE_Log_Msg::open(): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

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
  do_work(clientPingInterval,
          networkInterface,
          listeningPortNumber,
          statisticsReportingInterval,
          useThreadPool,
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

  // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return EXIT_FAILURE;
  } // end IF
#endif

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

  return EXIT_SUCCESS;
} // end main
