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
#include <list>
#include <sstream>
#include <string>

#include "ace/Configuration.h"
#include "ace/Configuration_Import_Export.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Version.h"

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_ui_gtk_manager.h"

#include "stream_allocatorheap.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_file_tools.h"

#include "rpg_net_defines.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_messageallocator.h"
#include "rpg_net_protocol_module_IRChandler.h"
#include "rpg_net_protocol_network.h"

//#include "IRC_common.h"
#include "IRC_client_defines.h"
#include "IRC_client_signalhandler.h"

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif // #ifdef DEBUG_DEBUGGER

  std::cout << ACE_TEXT ("usage: ") << programName_in << ACE_TEXT (" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT ("currently available options:") << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("protocol");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_CNF_DEF_INI_FILE);
  std::cout << ACE_TEXT ("-c [FILE]   : config file") << ACE_TEXT (" [\"") << path.c_str () << ACE_TEXT ("\"]") << std::endl;
  std::cout << ACE_TEXT ("-d          : debug parser") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-l          : log to a file") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-t          : trace information") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-v          : print version information and exit") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-x [VALUE]  : #thread pool threads ([") << IRC_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT ("]") << std::endl;
} // end print_usage

bool
do_processArguments (const int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     std::string& configFile_out,
                     bool& debugParser_out,
                     bool& logToFile_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out,
                     unsigned int& numThreadPoolThreads_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif // #ifdef DEBUG_DEBUGGER

  // init configuration
  configFile_out = configuration_path;
  configFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  configFile_out += ACE_TEXT_ALWAYS_CHAR ("protocol");
  configFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  configFile_out += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_CNF_DEF_INI_FILE);

  debugParser_out          = false;
  logToFile_out            = false;
  traceInformation_out     = false;
  printVersionAndExit_out  = false;
  numThreadPoolThreads_out = IRC_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT("c:dltvx:"),
                              1, // skip command name
                              1, // report parsing errors
                              ACE_Get_Opt::PERMUTE_ARGS, // ordering
                              0); // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        configFile_out = argumentParser.opt_arg ();

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
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << argumentParser.opt_arg ();
        converter >> numThreadPoolThreads_out;

        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argumentParser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    argumentParser.long_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_initializeSignals (bool allowUserRuntimeConnect_in,
                      ACE_Sig_Set& signals_inout)
{
  RPG_TRACE(ACE_TEXT("::do_initializeSignals"));

  int result = -1;

  // init return value(s)
  result = signals_inout.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", aborting\n")));
    return;
  } // end IF

  // init list of handled signals...
  // *PORTABILITY*: on Windows SIGHUP and SIGQUIT are not defined,
  // so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *NOTE*: don't handle SIGHUP !!!! --> program will hang !
  //signals_inout.push_back(SIGHUP);
#endif
  signals_inout.sig_add (SIGINT);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signals_inout.sig_add (SIGQUIT);
#endif
//   signals_inout.sig_add(SIGILL);
//   signals_inout.sig_add(SIGTRAP);
//   signals_inout.sig_add(SIGBUS);
//   signals_inout.sig_add(SIGFPE);
//   signals_inout.sig_add(SIGKILL); // cannot catch this one...
  if (allowUserRuntimeConnect_in)
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    signals_inout.sig_add (SIGUSR1);
#else
    signals_inout.sig_add (SIGBREAK);
#endif
//   signals_inout.sig_add(SIGSEGV);
  if (allowUserRuntimeConnect_in)
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    signals_inout.sig_add (SIGUSR2);
#else
    signals_inout.sig_add (SIGABRT);
#endif
//   signals_inout.sig_add(SIGPIPE);
//   signals_inout.sig_add(SIGALRM);
  signals_inout.sig_add (SIGTERM);
//   signals_inout.sig_add(SIGSTKFLT);
//   signals_inout.sig_add(SIGCHLD);
//   signals_inout.sig_add(SIGCONT);
//   signals_inout.sig_add(SIGSTOP); // cannot catch this one...
//   signals_inout.sig_add(SIGTSTP);
//   signals_inout.sig_add(SIGTTIN);
//   signals_inout.sig_add(SIGTTOU);
//   signals_inout.sig_add(SIGURG);
//   signals_inout.sig_add(SIGXCPU);
//   signals_inout.sig_add(SIGXFSZ);
//   signals_inout.sig_add(SIGVTALRM);
//   signals_inout.sig_add(SIGPROF);
//   signals_inout.sig_add(SIGWINCH);
//   signals_inout.sig_add(SIGIO);
//   signals_inout.sig_add(SIGPWR);
//   signals_inout.sig_add(SIGSYS);
//   signals_inout.sig_add(SIGRTMIN);
//   signals_inout.sig_add(SIGRTMIN+1);
// ...
//   signals_inout.sig_add(SIGRTMAX-1);
//   signals_inout.sig_add(SIGRTMAX);
}

void
do_work (RPG_Net_Protocol_Configuration& configuration_in,
         const std::string& serverHostname_in,
         unsigned short serverPortNumber_in,
         unsigned int numDispatchThreads_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: initialize event dispatch
  if (!Common_Tools::initializeEventDispatch (RPG_NET_USES_REACTOR,
                                              numDispatchThreads_in,
                                              configuration_in.streamConfiguration.serializeOutput))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to init event dispatch, returning\n")));
    return;
  } // end IF

  // step2: initialize client connector
  Net_Client_IConnector_t* connector_p = NULL;
  if (RPG_NET_USES_REACTOR)
    ACE_NEW_NORETURN (connector_p,
                      RPG_Net_Protocol_Connector_t (&configuration_in,
                                                    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance (),
                                                    0));
  else
    ACE_NEW_NORETURN (connector_p,
                      RPG_Net_Protocol_AsynchConnector_t (&configuration_in,
                                                          RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance (),
                                                          0));
  if (!connector_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return;
  } // end IF

  // step3: initialize signal handling
  IRC_Client_SignalHandler signal_handler (serverHostname_in,   // target hostname
                                           serverPortNumber_in, // target port
                                           connector_p);        // connector
  ACE_Sig_Handlers signal_handlers;
  // *WARNING*: 'signals' appears to be a keyword in some contexts...
  ACE_Sig_Set signal_set (0);
  do_initializeSignals (true,  // allow SIGUSR1
                        signal_set);
  Common_SignalActions_t previous_signal_actions;
  if (!Common_Tools::initializeSignals (signal_set,
                                        &signal_handler,
                                        previous_signal_actions))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handling, returning\n")));

    // clean up
    delete connector_p;

    return;
  } // end IF

  // step4a: initialize connection manager
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
  RPG_Net_Protocol_SessionData session_data;
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->set (configuration_in,
                                                                  &session_data);

  //// step5a: start GTK event loop
  //COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
  //if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to start GTK event dispatch, aborting\n")));

  //  // clean up
  //  delete connector_p;
  //  Common_Tools::finalizeSignals (signal_set,
  //                                 RPG_NET_USES_REACTOR,
  //                                 previous_signal_actions);

  //  return;
  //} // end IF

  // step5b: initialize worker(s)
  int group_id = -1;
  if (!Common_Tools::startEventDispatch (RPG_NET_USES_REACTOR,
                                         numDispatchThreads_in,
                                         group_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, aborting\n")));

    // clean up
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    delete connector_p;
    Common_Tools::finalizeSignals (signal_set,
                                   RPG_NET_USES_REACTOR,
                                   previous_signal_actions);

    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  // step6: (try to) connect to the server
  ACE_INET_Addr peer_address (serverPortNumber_in,
                              serverHostname_in.c_str ());
  bool result = connector_p->connect (peer_address);
  if (!RPG_NET_USES_REACTOR)
  {
    ACE_Time_Value delay (1, 0);
    ACE_OS::sleep (delay);
    if (RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections () != 1)
      result = false;
  } // end IF
  if (!result)
  {
    // debug info
    ACE_TCHAR buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    int result = peer_address.addr_to_string (buffer,
                                              sizeof (buffer));
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to connect(\"%s\"): \"%m\", returning\n"),
                buffer));

    // clean up
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    delete connector_p;
    Common_Tools::finalizeSignals (signal_set,
                                   RPG_NET_USES_REACTOR,
                                   previous_signal_actions);

    return;
  } // end IF

  // *NOTE*: from this point on, we need to clean up any remote connections !

  // step7: dispatch events
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
    if (RPG_NET_USES_REACTOR)
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

  // step8: clean up
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->abortConnections ();
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->waitConnections ();
  delete connector_p;
  Common_Tools::finalizeSignals (signal_set,
                                 RPG_NET_USES_REACTOR,
                                 previous_signal_actions);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_parseConfigurationFile (const std::string& configFilename_in,
                           RPG_Net_Protocol_IRCLoginOptions& loginOptions_out,
                           std::string& serverHostname_out,
                           unsigned short& serverPortNumber_out)
{
  RPG_TRACE (ACE_TEXT ("::do_parseConfigurationFile"));

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
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  // step1: init
  // *PORTABILITY*: on Windows, init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // step1 init/validate configuration

  // step1a: process commandline arguments
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
#endif // #ifdef DEBUG_DEBUGGER

  std::string configuration_file = configuration_path;
  configuration_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  configuration_file += ACE_TEXT_ALWAYS_CHAR ("protocol");
  configuration_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  configuration_file += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_CNF_DEF_INI_FILE);

  bool debug_parser                    = false;
  bool log_to_file                     = false;
  bool trace_information               = false;
  bool print_version_and_exit          = false;
  unsigned int num_thread_pool_threads = IRC_CLIENT_DEF_NUM_TP_THREADS;
  if (!do_processArguments (argc_in,
                            argv_in,
                            configuration_file,
                            debug_parser,
                            log_to_file,
                            trace_information,
                            print_version_and_exit,
                            num_thread_pool_threads))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!Common_File_Tools::isReadable (configuration_file))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // initialize logging and/or tracing
  std::string log_file;
  if (log_to_file)
    log_file = RPG_Common_File_Tools::getLogFilename (ACE::basename (argv_in[0]));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                        log_file,                   // logfile
                                        false,                      // log to syslog ?
                                        false,                      // trace messages ?
                                        trace_information,          // debug messages ?
                                        NULL))                      // logger
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

  // handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // initialize configuration object
  Stream_AllocatorHeap heap_allocator;
  RPG_Net_Protocol_MessageAllocator message_allocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                       &heap_allocator);
  RPG_Net_Protocol_Module_IRCHandler_Module IRC_handler_module (std::string ("IRCHandler"),
                                                                NULL);
  RPG_Net_Protocol_Module_IRCHandler* IRC_handler_impl = NULL;
  IRC_handler_impl =
    dynamic_cast<RPG_Net_Protocol_Module_IRCHandler*>(IRC_handler_module.writer ());
  if (!IRC_handler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<RPG_Net_Protocol_Module_IRCHandler> failed, aborting\n")));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (!IRC_handler_impl->initialize (&message_allocator,
                                     RPG_NET_PROTOCOL_BUFFER_SIZE,
                                     true,                         // auto-answer "ping" as a client ?...
                                     true))                        // clients print ('.') dots for received "pings"...
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (IRC_handler_module.name ())));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  RPG_Net_Protocol_Configuration configuration;
  // ************ socket configuration data ************
  configuration.socketConfiguration.bufferSize = NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;

  // ************ stream configuration data ****************
  configuration.streamConfiguration.messageAllocator = &message_allocator;
  configuration.streamConfiguration.bufferSize = RPG_NET_PROTOCOL_BUFFER_SIZE;
  configuration.streamConfiguration.module = &IRC_handler_module;
  configuration.streamConfiguration.statisticReportingInterval = 0; // == off

  // ************ protocol configuration data **************
  configuration.protocolConfiguration.streamConfiguration.crunchMessageBuffers =
    RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES;
  configuration.protocolConfiguration.streamConfiguration.debugScanner =
   debug_parser;
  configuration.protocolConfiguration.streamConfiguration.debugParser =
   debug_parser;
  configuration.protocolConfiguration.clientPingInterval = 0; // servers only
  configuration.protocolConfiguration.loginOptions.nick =
   IRC_CLIENT_DEF_IRC_NICK;
  std::string hostname;
  Net_Common_Tools::retrieveLocalHostname (hostname);
  if (IRC_CLIENT_CNF_IRC_USERMSG_TRADITIONAL)
  {
    configuration.protocolConfiguration.loginOptions.user.hostname.discriminator =
      RPG_Net_Protocol_IRCLoginOptions::User::Hostname::STRING;
    configuration.protocolConfiguration.loginOptions.user.hostname.string = &hostname;
  } // end IF
  else
  {
    configuration.protocolConfiguration.loginOptions.user.hostname.discriminator =
      RPG_Net_Protocol_IRCLoginOptions::User::Hostname::BITMASK;
    // *NOTE*: hybrid-7.2.3 seems to have a bug: 4 --> +i
    configuration.protocolConfiguration.loginOptions.user.hostname.mode =
      IRC_CLIENT_DEF_IRC_USERMODE;
  } // end ELSE
  configuration.protocolConfiguration.loginOptions.user.servername =
    ACE_TEXT_ALWAYS_CHAR (RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME);
  configuration.protocolConfiguration.loginOptions.channel =
    ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_DEF_IRC_CHANNEL);
  // populate user/realname
  Common_Tools::getCurrentUserName (configuration.protocolConfiguration.loginOptions.user.username,
                                    configuration.protocolConfiguration.loginOptions.user.realname);

  // step1db: parse config file (if any)
  std::string serverHostname      = ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_DEF_SERVER_HOSTNAME);
  unsigned short serverPortNumber = IRC_CLIENT_DEF_SERVER_PORT;
  if (!configuration_file.empty ())
    do_parseConfigurationFile (configuration_file,
                               configuration.protocolConfiguration.loginOptions,
                               serverHostname,
                               serverPortNumber);

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work (configuration,
           serverHostname,
           serverPortNumber,
           num_thread_pool_threads);
  // clean up
  IRC_handler_module.close ();
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  Common_Tools::period2String (working_time,
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

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage(elapsed_rusage);
  ACE_Time_Value user_time(elapsed_rusage.ru_utime);
  ACE_Time_Value system_time(elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  Common_Tools::period2String (user_time,
                               user_time_string);
  Common_Tools::period2String (system_time,
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

  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;
} // end main
