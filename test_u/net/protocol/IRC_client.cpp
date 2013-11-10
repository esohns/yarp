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

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "IRC_common.h"
#include "IRC_client_defines.h"
#include "IRC_client_signalhandler.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_messageallocator.h"
#include "rpg_net_protocol_module_IRChandler.h"

#include "rpg_net_defines.h"
#include "rpg_net_connection_manager.h"

#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_stream_allocatorheap.h"

#include <ace/ACE.h>
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
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>

#include <string>
#include <iostream>
#include <sstream>
#include <list>

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                              true);
#endif // #ifdef BASEDIR

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) || defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("protocol");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_DEF_INI_FILE);
  std::cout << ACE_TEXT("-c [FILE]   : config file") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-d          : debug parser") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]  : #thread pool threads ([") << IRC_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& configFile_out,
                  bool& debugParser_out,
                  bool& logToFile_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  unsigned int& numThreadPoolThreads_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                              true);
#endif // #ifdef BASEDIR

  // init configuration
  configFile_out = config_path;
  configFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) || defined(DEBUG_RELEASE)
  configFile_out += ACE_TEXT_ALWAYS_CHAR("protocol");
  configFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  configFile_out += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_DEF_INI_FILE);

  debugParser_out          = false;
  logToFile_out            = false;
  traceInformation_out     = false;
  printVersionAndExit_out  = false;
  numThreadPoolThreads_out = IRC_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:dltvx:"),
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
      case 'c':
      {
        configFile_out = argumentParser.opt_arg();

        break;
      }
      case 'd':
      {
        debugParser_out = true;

        break;
      }
      case 'l':
      {
        logToFile_out = true;

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

bool
init_threadPool(const bool& useReactor_in,
                const unsigned int& numThreadPoolThreads_in)
{
  RPG_TRACE(ACE_TEXT("::init_threadPool"));

  if (useReactor_in && (numThreadPoolThreads_in > 1))
    {
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
    } // end IF
  else
    {
      ACE_Proactor* proactor = NULL;
      ACE_NEW_RETURN(proactor,
                     ACE_Proactor(NULL, false, NULL),
                     false);
      // make this the "default" proactor...
      ACE_Proactor::instance(proactor, 1); // delete in dtor
    } // end ELSE

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
//   signals_inout.push_back(SIGBUS);
//   signals_inout.push_back(SIGFPE);
//   signals_inout.push_back(SIGKILL); // cannot catch this one...
  if (allowUserRuntimeConnect_in)
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    signals_inout.push_back(SIGUSR1);
#else
    signals_inout.push_back(SIGBREAK);
#endif
//   signals_inout.push_back(SIGSEGV);
  if (allowUserRuntimeConnect_in)
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
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

bool
init_signalHandling(const std::vector<int>& signals_inout,
                    IRC_Client_SignalHandler& eventHandler_in,
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
               ACE_TEXT("registered handler for \"%S\" (key: %d)...\n"),
               *iter,
               sigkey));
  } // end FOR

  // actually, there is only a single handler for ALL signals in the set...
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
                              SA_SIGINFO);                // flags
//                               (SA_RESTART | SA_SIGINFO)); // flags
  ACE_Sig_Action originalAction;
  ignoreAction.register_action(SIGPIPE, &originalAction);

  return true;
}

static
ACE_THR_FUNC_RETURN
tp_worker_func(void* args_in)
{
  RPG_TRACE(ACE_TEXT("::tp_worker_func"));

  bool use_reactor = *reinterpret_cast<bool*>(args_in);

  // *NOTE*: asynchronous writing to a closed socket triggers the
  // SIGPIPE signal (default action: abort).
  // --> as this doesn't use select(), guard against this (ignore the signal)
  ACE_Sig_Action no_sigpipe(static_cast<ACE_SignalHandler>(SIG_IGN));
  ACE_Sig_Action original_action;
  no_sigpipe.register_action(SIGPIPE, &original_action);

  int success = 0;
  // handle any events...
  if (use_reactor)
  {
    // assume ownership over the reactor...
    ACE_Reactor::instance()->owner(ACE_OS::thr_self(),
                                   NULL);
    success = ACE_Reactor::instance()->run_reactor_event_loop(0);
  }
  else
    success = ACE_Proactor::instance()->proactor_run_event_loop(0);
  if (success == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("(%t) failed to handle events: \"%m\", aborting\n")));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  // clean up
  no_sigpipe.restore_action(SIGPIPE, original_action);

  // *PORTABILITY*
  // *TODO*
  return (success == 0 ? NULL : NULL);
}

void
do_work(const RPG_Net_Protocol_ConfigPOD& config_in,
        const std::string& serverHostname_in,
        const unsigned short& serverPortNumber_in,
        const unsigned int& numThreadPoolThreads_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0: (if necessary) init the thread pool
  if (!init_threadPool(IRC_CLIENT_DEF_CLIENT_USES_REACTOR, numThreadPoolThreads_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init thread pool, aborting\n")));

    return;
  } // end IF

  // step0b: init client connector
  IRC_Client_Connector connector(ACE_Reactor::instance(), // reactor
                                 ACE_NONBLOCK);           // flags: non-blocking I/O
//                                  0);                      // flags (*TODO*: ACE_NONBLOCK ?);

  // step1: signal handling
  // event handler for signals
  IRC_Client_SignalHandler signalEventHandler(serverHostname_in,   // target hostname
                                              serverPortNumber_in, // target port
                                              &connector);         // connector
  ACE_Sig_Handlers signalHandlers;
  // *WARNING*: 'signals' appears to be a keyword in some contexts...
  std::vector<int> signalss;
  init_signals(true,  // allow SIGUSR1
               signalss);
  if (!init_signalHandling(signalss,
                           signalEventHandler,
                           signalHandlers))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init_signalHandling(), aborting\n")));

    return;
  } // end IF

  // step2a: init connection manager
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max());
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->set(config_in); // will be passed to all handlers

  // step2b: (try to) connect to the server
  IRC_Client_SocketHandler* handler = NULL;
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
    // debug info
    ACE_TCHAR buf[BUFSIZ];
    ACE_OS::memset(buf,
                   0,
                   (BUFSIZ * sizeof(ACE_TCHAR)));
    if (remote_address.addr_to_string(buf,
                                      (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Connector::connect(%s): \"%m\", aborting\n"),
               buf));

    return;
  } // end IF

  // *NOTE*: from this point on, we need to clean up any remote connections !

  // event loop:
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (and perform orderly shutdown)
  // - signal timer expiration to perform server queries

//   // *NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);

  // step3: dispatch events...
  // *NOTE*: if we use a thread pool, we need to do this differently...
  if (numThreadPoolThreads_in > 1)
  {
    // start a (group of) worker thread(s)...
    bool thread_argument = IRC_CLIENT_DEF_CLIENT_USES_REACTOR;
    int grp_id = -1;
    grp_id = ACE_Thread_Manager::instance()->spawn_n(numThreadPoolThreads_in,     // # threads
                                                     ::tp_worker_func,            // function
                                                     &thread_argument,            // argument
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
                 ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
                 numThreadPoolThreads_in));

      // clean up
//       if (stressTestServer_in)
//       {
//         if (ACE_Reactor::instance()->cancel_timer(timerID,  // timer ID
//             NULL,     // pointer to args passed to handler
//             1) != 1)  // don't invoke handle_close() on handler
//         {
//           ACE_DEBUG((LM_ERROR,
//                      ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%p\", continuing\n")));
//         } // end IF
//       } // end IF
      RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
      RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

      return;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("spawned %u event handlers (group ID: %u)...\n"),
               numThreadPoolThreads_in,
               grp_id));

    // ... and wait for this group to join
    ACE_Thread_Manager::instance()->wait_grp(grp_id);
  } // end IF
  else
  {
    if (IRC_CLIENT_DEF_CLIENT_USES_REACTOR)
    {
/*    // *WARNING*: DON'T restart system calls (after e.g. EINTR) for the reactor
      ACE_Reactor::instance()->restart(1);
*/
      while (!ACE_Reactor::instance()->reactor_event_loop_done())
      {
        if (ACE_Reactor::instance()->run_reactor_event_loop(0) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to handle events: \"%m\", aborting\n")));

	  break;
	} // end IF
      } // end WHILE
    } // end IF
    else
    {
      while (!ACE_Proactor::instance()->proactor_event_loop_done())
      {
        if (ACE_Proactor::instance()->proactor_run_event_loop(0) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to handle events: \"%m\", aborting\n")));

          // clean up
          // *TODO*: stop connector
          //			    connector.cancel();

          break;
        } // end IF
      } // end WHILE
    } // end ELSE
  } // end ELSE

  // step4: clean up
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseConfigFile(const std::string& configFilename_in,
                   RPG_Net_Protocol_IRCLoginOptions& loginOptions_out,
                   std::string& serverHostname_out,
                   unsigned short& serverPortNumber_out)
{
  RPG_TRACE(ACE_TEXT("::do_parseConfigFile"));

  // init return value(s)
  serverHostname_out   = ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_DEF_SERVER_HOSTNAME);
  serverPortNumber_out = IRC_CLIENT_DEF_SERVER_PORT;

  ACE_Configuration_Heap config_heap;
  if (config_heap.open())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Configuration_Heap::open failed, returning\n")));

    return;
  } // end IF

  ACE_Ini_ImpExp import(config_heap);
  if (import.import_config(configFilename_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Ini_ImpExp::import_config(\"%s\") failed, returning\n"),
               configFilename_in.c_str()));

    return;
  } // end IF

  // find/open "login" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_LOGIN_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_LOGIN_SECTION_HEADER)));

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
    if (val_name == ACE_TEXT("password"))
    {
      loginOptions_out.password = val_value.c_str();
    }
    else if (val_name == ACE_TEXT("nick"))
    {
      loginOptions_out.nick = val_value.c_str();
    }
    else if (val_name == ACE_TEXT("user"))
    {
      loginOptions_out.user.username = val_value.c_str();
    }
    else if (val_name == ACE_TEXT("realname"))
    {
      loginOptions_out.user.realname = val_value.c_str();
    }
    else if (val_name == ACE_TEXT("channel"))
    {
      loginOptions_out.channel = val_value.c_str();
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 val_name.c_str()));
    } // end ELSE

    ++val_index;
  } // end WHILE

  // find/open "connection" section...
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT(IRC_CLIENT_CNF_CONNECTION_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               ACE_TEXT(IRC_CLIENT_CNF_CONNECTION_SECTION_HEADER)));

    return;
  } // end IF

  // import values...
  val_index = 0;
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
    if (val_name == ACE_TEXT("server"))
    {
      serverHostname_out = val_value.c_str();
    }
    else if (val_name == ACE_TEXT("port"))
    {
      serverPortNumber_out = ::atoi(val_value.c_str());
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 val_name.c_str()));
    } // end ELSE

    ++val_index;
  } // end WHILE

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("imported \"%s\"...\n"),
//              configFilename_in.c_str()));
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
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1 init/validate configuration

  // step1a: process commandline arguments
  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                              true);
#endif // #ifdef BASEDIR

  std::string configFile = config_path;
  configFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) || defined(DEBUG_RELEASE)
  configFile += ACE_TEXT_ALWAYS_CHAR("protocol");
  configFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  configFile += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_DEF_INI_FILE);

  bool debugParser                  = false;
  bool logToFile                    = false;
  bool traceInformation             = false;
  bool printVersionAndExit          = false;
  unsigned int numThreadPoolThreads = IRC_CLIENT_DEF_NUM_TP_THREADS;
  if (!(process_arguments(argc,
                          argv,
                          configFile,
                          debugParser,
                          logToFile,
                          traceInformation,
                          printVersionAndExit,
                          numThreadPoolThreads)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!RPG_Common_File_Tools::isReadable(configFile))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step1c: set correct trace level
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

  // step1d: init configuration object
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_Protocol_MessageAllocator messageAllocator(RPG_NET_DEF_MAX_MESSAGES,
                                                     &heapAllocator);
  RPG_Net_Protocol_Module_IRCHandler_Module IRChandlerModule(std::string("IRCHandler"),
                                                             NULL);
  RPG_Net_Protocol_Module_IRCHandler* IRChandler_impl = NULL;
  IRChandler_impl = dynamic_cast<RPG_Net_Protocol_Module_IRCHandler*>(IRChandlerModule.writer());
  if (!IRChandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Protocol_Module_IRCHandler) failed> (aborting\n")));

    return EXIT_FAILURE;
  } // end IF
  if (!IRChandler_impl->init(&messageAllocator,
                             RPG_NET_DEF_CLIENT_PING_PONG, // auto-answer "ping" as a client ?...
                             true))                        // clients print ('.') dots for received "pings"...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               IRChandlerModule.name()));

    return EXIT_FAILURE;
  } // end IF

  RPG_Net_Protocol_ConfigPOD config;
  // step1da: populate config object with default/collected data
  // ************ connection config data ************
  config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &messageAllocator;
  config.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
  // ************ protocol config data **************
  config.clientPingInterval = 0; // servers do this...
//   config.loginOptions.password = ;
  config.loginOptions.nick = IRC_CLIENT_DEF_IRC_NICK;
//   config.loginOptions.user.username = ;
  std::string hostname = RPG_Common_Tools::getHostName();
  if (IRC_CLIENT_CNF_IRC_USERMSG_TRADITIONAL)
  {
    config.loginOptions.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::STRING;
    config.loginOptions.user.hostname.string = &hostname;
  } // end IF
  else
  {
    config.loginOptions.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::BITMASK;
    // *NOTE*: hybrid-7.2.3 seems to have a bug: 4 --> +i
    config.loginOptions.user.hostname.mode = IRC_CLIENT_DEF_IRC_USERMODE;
  } // end ELSE
  config.loginOptions.user.servername = ACE_TEXT_ALWAYS_CHAR(RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME);
//   config.loginOptions.user.realname = ;
  config.loginOptions.channel = ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_DEF_IRC_CHANNEL);
  // ************ stream config data ****************
  config.module = &IRChandlerModule;
  config.crunchMessageBuffers = false;
  config.debugScanner = debugParser;
  config.debugParser = debugParser;
  // *WARNING*: set at runtime, by the appropriate connection handler
  config.sessionID = 0; // (== socket handle !)
  config.statisticsReportingInterval = 0; // == off
  config.currentStatistics.numBytes = 0;
  config.currentStatistics.numDataMessages = 0;
  config.lastCollectionTimestamp = ACE_Time_Value::zero;

  // populate user/realname
  if (!RPG_Common_Tools::getUserName(config.loginOptions.user.username,
                                     config.loginOptions.user.realname))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::getUserName(), aborting\n")));

    return EXIT_FAILURE;
  } // end IF

  // step1db: parse config file (if any)
  std::string serverHostname      = ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_DEF_SERVER_HOSTNAME);
  unsigned short serverPortNumber = IRC_CLIENT_DEF_SERVER_PORT;
  if (!configFile.empty())
    do_parseConfigFile(configFile,
                       config.loginOptions,
                       serverHostname,
                       serverPortNumber);

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work(config,
          serverHostname,
          serverPortNumber,
          numThreadPoolThreads);
  // clean up
  IRChandlerModule.close();
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
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

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
