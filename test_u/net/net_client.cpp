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
#include <config.h>
#endif

#include "net_client_timeouthandler.h"

#include <rpg_net_defines.h>
#include <rpg_net_common_tools.h>
#include <rpg_net_client_sockethandler.h>

#include <rpg_common_tools.h>

#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Reactor.h>
#include <ace/High_Res_Timer.h>

#include <string>
#include <iostream>
#include <sstream>
#include <list>

#define NET_CLIENT_DEF_SERVER_HOSTNAME       ACE_LOCALHOST
#define NET_CLIENT_DEF_SERVER_QUERY_INTERVAL 1

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-h [STRING]: server hostname [\"") << NET_CLIENT_DEF_SERVER_HOSTNAME << "\"]" << std::endl;
  std::cout << ACE_TEXT("-i [VALUE] : connection interval ([") << NET_CLIENT_DEF_SERVER_QUERY_INTERVAL << ACE_TEXT("] seconds)") << std::endl;
  std::cout << ACE_TEXT("-l         : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s [VALUE] : server port [") << RPG_NET_DEF_LISTENING_PORT << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t         : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x         : stress-test server") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& serverHostname_out,
                  unsigned long& connectionInterval_out,
                  bool& logToFile_out,
                  unsigned short& serverPortNumber_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  bool& stressTestServer_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  serverHostname_out = NET_CLIENT_DEF_SERVER_HOSTNAME;
  connectionInterval_out = NET_CLIENT_DEF_SERVER_QUERY_INTERVAL;
  logToFile_out = false;
  serverPortNumber_out = RPG_NET_DEF_LISTENING_PORT;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  stressTestServer_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("h:i:ls:tvx"),
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
      case 's':
      {
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> serverPortNumber_out;

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
        stressTestServer_out = true;

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
do_work(const std::string& serverHostname_in,
        const unsigned long& connectionInterval_in,
        const unsigned short& serverPortNumber_in,
        const bool& stressTestServer_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step1: init (timer)
  RPG_Net_Client_Connector connector(ACE_Reactor::instance(), // reactor
                                     0);                      // flags (*TODO*: ACE_NONBLOCK ?);
  std::list<RPG_Net_Client_SocketHandler*> connectionHandlers;
  connectionHandlers.clear();
  long timerID = -1;
  Net_Client_TimeoutHandler timeoutHandler(serverHostname_in,
                                           serverPortNumber_in,
                                           &connector,
                                           &connectionHandlers);
  if (stressTestServer_in)
  {
    // schedule server query interval timer
    ACE_Time_Value interval(connectionInterval_in, 0);
    timerID = ACE_Reactor::instance()->schedule_timer(&timeoutHandler, // event handler
                                                      NULL,            // args passed to handler
                                                      interval,        // first delay
                                                      interval);       // recurrence interval
    if (timerID == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::schedule_timer(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

      return;
    } // end IF
  } // end IF
  else
  {
    // step1: connect to server...
    RPG_Net_Client_SocketHandler* handler = NULL;
    ACE_INET_Addr remote_address(serverPortNumber_in, // remote SAP
                                 serverHostname_in.c_str());
    if (connector.connect(handler,                     // service handler
                          remote_address/*,              // remote SAP
                          ACE_Synch_Options::defaults, // synch options
                          ACE_INET_Addr::sap_any,      // local SAP
                          0,                           // try to re-use address (SO_REUSEADDR)
                          O_RDWR,                      // flags
                          0*/) == -1)                  // perms
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Connector::connect(%s:%u): \"%s\", aborting\n"),
                 ACE_TEXT_CHAR_TO_TCHAR(remote_address.get_host_name()),
                 remote_address.get_port_number(),
                 ACE_OS::strerror(errno)));

      return;
    } // end IF
    // sanity check
    ACE_ASSERT(handler);

    // step2: add to connections
    connectionHandlers.push_front(handler);
  } // end ELSE

  // event loop:
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (and perform orderly shutdown)
  // - signal timer expiration to perform server queries

//   // *IMPORTANT NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);
  if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    // clean up
    if (ACE_Reactor::instance()->cancel_timer(timerID,  // timer ID
                                              NULL,     // pointer to args passed to handler
                                              1) != 1)  // don't invoke handle_close() on handler
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%s\", continuing\n"),
                 ACE_OS::strerror(errno)));
    } // end IF

    return;
  } // end IF

  // clean up
  if (ACE_Reactor::instance()->cancel_timer(timerID,  // timer ID
                                            NULL,     // pointer to args passed to handler
                                            1) != 1)  // don't invoke handle_close() on handler
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%s\", continuing\n"),
               ACE_OS::strerror(errno)));
  } // end IF
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("closing %u connection(s)...\n"),
             connectionHandlers.size()));
  for (std::list<RPG_Net_Client_SocketHandler*>::const_iterator iterator = connectionHandlers.begin();
       iterator != connectionHandlers.end();
       iterator++)
  {
    // close connection
    (*iterator)->close(0);
  } // end FOR

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
  std::string serverHostname       = NET_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned long connectionInterval = NET_CLIENT_DEF_SERVER_QUERY_INTERVAL;
  bool logToFile                   = false;
  unsigned short serverPortNumber  = RPG_NET_DEF_LISTENING_PORT;
  bool traceInformation            = false;
  bool printVersionAndExit         = false;
  bool stressTestServer            = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          serverHostname,
                          connectionInterval,
                          logToFile,
                          serverPortNumber,
                          traceInformation,
                          printVersionAndExit,
                          stressTestServer)))
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
          stressTestServer);
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
