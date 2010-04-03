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

// *IMPORTANT NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rpg_net_defines.h>
#include <rpg_net_listener.h>
#include <rpg_net_connection_manager.h>
#include <rpg_net_signalhandler.h>
#include <rpg_net_common_tools.h>
// #include <rpg_net_stream.h>

#include <rpg_common_tools.h>

#include <stream_allocatorheap.h>
#include <stream_messageallocatorheap.h>

#include <ace/OS.h>
#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Reactor.h>
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
  ACE_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-i [VALUE] : client ping interval ([") << RPG_NET_DEF_PING_INTERVAL << ACE_TEXT("] seconds)") << std::endl;
  std::cout << ACE_TEXT("-k [VALUE] : keep-alive timeout ([") << RPG_NET_DEF_KEEPALIVE << ACE_TEXT("] seconds)") << std::endl;
  std::cout << ACE_TEXT("-l         : log to a file") << std::endl;
  std::cout << ACE_TEXT("-n [STRING]: network interface [\"") << RPG_NET_DEF_CNF_NETWORK_INTERFACE << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-p [VALUE] : listening port ([") << RPG_NET_DEF_LISTENING_PORT << ACE_TEXT("])") << std::endl;
  std::cout << ACE_TEXT("-s [VALUE] : statistics reporting interval ([") << RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL << ACE_TEXT("] seconds)") << std::endl;
  std::cout << ACE_TEXT("-t         : trace information") << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit") << std::endl;
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
                  bool& printVersionAndExit_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  clientPingInterval_out = RPG_NET_DEF_PING_INTERVAL;
  keepAliveTimeout_out = RPG_NET_DEF_KEEPALIVE;
  logToFile_out = false;
  networkInterface_out = RPG_NET_DEF_CNF_NETWORK_INTERFACE;
  listeningPortNumber_out = RPG_NET_DEF_LISTENING_PORT;
  statisticsReportingInterval_out = RPG_NET_DEF_STATISTICS_REPORTING_INTERVAL;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("i:k:ln:p:s:tv"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'i':
      {
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> clientPingInterval_out;

        break;
      }
      case 'k':
      {
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
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> listeningPortNumber_out;

        break;
      }
      case 's':
      {
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
  ACE_TRACE(ACE_TEXT("::init_fileLogging"));

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
               ACE_OS::strerror(errno)));

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
  ACE_TRACE(ACE_TEXT("::init_coreDumping"));

  // step1: retrieve current values
  // *PORTABILITY*: this is entirely un-portable so we do an ugly hack...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  rlimit core_limit;
  if (ACE_OS::getrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

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
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // verify...
  if (ACE_OS::getrlimit(RLIMIT_CORE,
                        &core_limit) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getrlimit(RLIMIT_CORE): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("corefile limits [soft: %u, hard: %u]...\n"),
             core_limit.rlim_cur,
             core_limit.rlim_max));
#else
   // *TODO*
  ACE_DEBUG((LM_WARNING,
             ACE_TEXT("corefile limits on Windows platforms has not been implemented (yet), continuing\n")));
#endif

  return true;
}

void
init_signals(std::vector<int>& signals_in)
{
  ACE_TRACE(ACE_TEXT("::init_signals"));

  // init return value(s)
  signals_in.clear();

  // init list of handled signals...
  // *PORTABILITY*: on Windows SIGHUP and SIGQUIT are not defined,
  // so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *IMPORTANT NOTE*: don't handle SIGHUP !!!! --> program will hang !
  //signals_in.push_back(SIGHUP);
#endif
  signals_in.push_back(SIGINT);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signals_in.push_back(SIGQUIT);
#endif
//   signals_in.push_back(SIGILL);
//   signals_in.push_back(SIGTRAP);
  signals_in.push_back(SIGABRT);
//   signals_in.push_back(SIGBUS);
//   signals_in.push_back(SIGFPE);
// //   signals_in.push_back(SIGKILL);
//   signals_in.push_back(SIGUSR1);
//   signals_in.push_back(SIGSEGV);
//   signals_in.push_back(SIGUSR2);
//   signals_in.push_back(SIGPIPE);
//   signals_in.push_back(SIGALRM);
  signals_in.push_back(SIGTERM);
//   signals_in.push_back(SIGSTKFLT);
//   signals_in.push_back(SIGCHLD);
//   signals_in.push_back(SIGCONT);
// //   signals_in.push_back(SIGSTOP);
//   signals_in.push_back(SIGTSTP);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//   signals_in.push_back(SIGTTIN);
#else
  signals_in.push_back(SIGBREAK);
#endif
//   signals_in.push_back(SIGTTOU);
//   signals_in.push_back(SIGURG);
//   signals_in.push_back(SIGXCPU);
//   signals_in.push_back(SIGXFSZ);
//   signals_in.push_back(SIGVTALRM);
//   signals_in.push_back(SIGPROF);
//   signals_in.push_back(SIGWINCH);
//   signals_in.push_back(SIGIO);
//   signals_in.push_back(SIGPWR);
//   signals_in.push_back(SIGSYS);
//   signals_in.push_back(SIGRTMIN);
//   signals_in.push_back(SIGRTMIN+1);
// ...
//   signals_in.push_back(SIGRTMAX-1);
//   signals_in.push_back(SIGRTMAX);
}

const bool
init_signalHandling(const std::vector<int>& signals_in,
                    RPG_Net_SignalHandler& eventHandler_in,
                    ACE_Sig_Handlers& signalHandlers_in)
{
  ACE_TRACE(ACE_TEXT("::init_signalHandling"));

  // specify (default) action...
  // Note: we don't actually need to keep this around after registration...
  ACE_Sig_Action signalAction((ACE_SignalHandler)SIG_DFL, // default action (will be overridden below)...
                               ACE_Sig_Set(1),            // mask of signals to be blocked when we're servicing
                                                          // --> block them all ! (except KILL off course...)
//                              (SA_RESTART | SA_SIGINFO)); // flags
                               SA_SIGINFO);               // flags

  // handle different signals...
  int sigkey = -1;
  for (std::vector<int>::const_iterator iter = signals_in.begin();
       iter != signals_in.end();
       iter++)
  {
    sigkey = signalHandlers_in.register_handler(*iter,
                                                &eventHandler_in,
                                                &signalAction,
                                                NULL,
                                                NULL);
    if (sigkey == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Sig_Handlers::register_handler(\"%S\": %d): \"%s\", aborting\n"),
                 *iter,
                 *iter,
                 ACE_OS::strerror(errno)));

      return false;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("registered handler for \"%S\": %d (key: %d)...\n"),
               *iter,
               *iter,
               sigkey));
  } // end FOR

  // Note: in fact, we registered a SINGLE handler instance for ALL the signals we're interested in...
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("registered %d signal handler(s)...\n"),
             signals_in.size()));

  return true;
}

void
do_work(const unsigned long& clientPingInterval_in,
        const std::string& networkInterface_in,
        const unsigned short& listeningPortNumber_in,
        const unsigned long& statisticsReportingInterval_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // - start listening for connections on a TCP port
  // *IMPORTANT NOTE*: need to cancel this for a well-behaved shutdown !

  // step0: signal handling
  // event handler for signals
  RPG_Net_SignalHandler signalEventHandler(NULL);
  ACE_Sig_Handlers      signalHandlers;
  std::vector<int>      signals;
  init_signals(signals);
  if (!init_signalHandling(signals,
                           signalEventHandler,
                           signalHandlers))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init_signalHandling(), aborting\n")));

    return;
  } // end IF

  // step1: init configuration object
  Stream_AllocatorHeap heapAllocator;
  Stream_MessageAllocatorHeap messageAllocator(RPG_NET_DEF_MAX_MESSAGES,
                                               &heapAllocator);
  RPG_Net_ConfigPOD config;
  ACE_OS::memset(&config,
                 0,
                 sizeof(RPG_Net_ConfigPOD));
  config.socketBufferSize = RPG_NET_DEF_PCAP_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &heapAllocator;
  config.statisticsReportingInterval = statisticsReportingInterval_in;
//   RPG_Net_Stream stream;
//   if (!stream.init(config))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to init processing stream, aborting\n")));
//
//     return;
//   } // end IF

  // step2: init/start listening
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

  // event loop:
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (and perform orderly shutdown)
  // - signal connection attempts to acceptor
  // - signal timer expiration to perform maintenance/local statistics reporting

//   // *IMPORTANT NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);
  if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    // clean up
    // stop listener, clean up pending connections
    RPG_NET_LISTENER_SINGLETON::instance()->stop();
    RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_printVersion(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;

  // create version string...
  // *IMPORTANT NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
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
  ACE_TRACE(ACE_TEXT("::main"));

  // step1: init
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

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
                          printVersionAndExit)))
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
               ACE_OS::strerror(errno)));

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
  // *IMPORTANT NOTE*: this setting will be inherited by any child processes (daemon mode)
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
          statisticsReportingInterval);
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
               ACE_OS::strerror(errno)));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
