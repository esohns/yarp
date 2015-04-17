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
#include <map>

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

#include "gtk/gtk.h"

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_ui_defines.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager.h"

#include "stream_cachedallocatorheap.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_file_tools.h"
//#include "rpg_common_tools.h"

#include "rpg_net_defines.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_messageallocator.h"
#include "rpg_net_protocol_network.h"

#include "IRC_client_defines.h"
#include "IRC_client_gui_callbacks.h"
#include "IRC_client_gui_common.h"
#include "IRC_client_gui_defines.h"

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

  std::cout << ACE_TEXT ("usage: ") << programName_in << ACE_TEXT (" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT ("currently available options:") << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("protocol");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #ifdef DEBUG_DEBUGGER
  path += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_CNF_DEF_INI_FILE);
  std::cout << ACE_TEXT ("-c [FILE] : config file") << ACE_TEXT (" [\"") << path.c_str () << ACE_TEXT ("\"]") << std::endl;
  std::cout << ACE_TEXT ("-d        : debug") << ACE_TEXT (" [") << IRC_CLIENT_DEF_TRACE_ENABLED << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-l        : log to a file") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-r [VALUE]: reporting interval (seconds: 0 --> OFF)") << ACE_TEXT (" [") << IRC_CLIENT_DEF_STATSINTERVAL << ACE_TEXT ("]") << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("protocol");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #ifdef DEBUG_DEBUGGER
  path += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_DEF_SERVERS_FILE);
  std::cout << ACE_TEXT ("-s [FILE] : server config file") << ACE_TEXT (" [\"") << path.c_str () << ACE_TEXT ("\"]") << std::endl;
  std::cout << ACE_TEXT ("-t        : trace information") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("protocol");
#endif // #ifdef DEBUG_DEBUGGER
  std::cout << ACE_TEXT ("-u [DIR]  : UI file directory") << ACE_TEXT (" [\"") << path.c_str () << ACE_TEXT ("\"]") << std::endl;
  std::cout << ACE_TEXT ("-v        : print version information and exit") << ACE_TEXT (" [") << false << ACE_TEXT ("]") << std::endl;
  std::cout << ACE_TEXT ("-x [VALUE]: #thread pool threads ([") << IRC_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT ("]") << std::endl;
} // end print_usage

bool
do_processArguments (const int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     std::string& configurationFile_out,
                     bool& debug_out,
                     bool& logToFile_out,
                     unsigned int& reportingInterval_out,
                     std::string& phonebookFile_out,
                     bool& traceInformation_out,
                     std::string& UIFileDirectory_out,
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

  // init results
  configurationFile_out = configuration_path;
  configurationFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  configurationFile_out += ACE_TEXT_ALWAYS_CHAR ("protocol");
  configurationFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #ifdef DEBUG_DEBUGGER
  configurationFile_out += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_CNF_DEF_INI_FILE);

  debug_out             = IRC_CLIENT_DEF_TRACE_ENABLED;
  logToFile_out         = false;
  reportingInterval_out = IRC_CLIENT_DEF_STATSINTERVAL;

  phonebookFile_out = configuration_path;
  phonebookFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  phonebookFile_out += ACE_TEXT_ALWAYS_CHAR ("protocol");
  phonebookFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #ifdef DEBUG_DEBUGGER
  phonebookFile_out +=
   ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_DEF_SERVERS_FILE);

  traceInformation_out     = false;

  UIFileDirectory_out = configuration_path;
#if defined (DEBUG_DEBUGGER)
  UIFileDirectory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFileDirectory_out += ACE_TEXT_ALWAYS_CHAR("protocol");
#endif // #ifdef DEBUG_DEBUGGER

  printVersionAndExit_out  = false;
  numThreadPoolThreads_out = IRC_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("c:dlr:s:tu:vx:"),
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
        configurationFile_out = argumentParser.opt_arg ();

        break;
      }
      case 'd':
      {
        debug_out = true;

        break;
      }
      case 'l':
      {
        logToFile_out = true;

        break;
      }
      case 'r':
      {
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter.clear ();
        converter << argumentParser.opt_arg ();
        converter >> reportingInterval_out;

        break;
      }
      case 's':
      {
        phonebookFile_out = argumentParser.opt_arg ();

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UIFileDirectory_out = argumentParser.opt_arg ();

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
                    ACE_TEXT (argumentParser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.long_option ())));
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
do_work (unsigned int numDispatchThreads_in,
         main_cb_data_t& userData_in,
         const std::string& UIDefinitionFile_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step1: initialize event dispatch
  bool serialize_output = false;
  if (!Common_Tools::initializeEventDispatch (IRC_CLIENT_DEF_CLIENT_USES_REACTOR,
                                              numDispatchThreads_in,
                                              serialize_output))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to init event dispatch, returning\n")));
    return;
  } // end IF

  // step0b: init connection manager
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
  RPG_Net_Protocol_SessionData session_data;
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->set (*userData_in.configuration,
                                                                  &session_data);

  // *WARNING*: from this point on, we need to clean up any remote connections !
  // *NOTE* handlers register with the connection manager and will self-destruct
  // on disconnects !

  // step1: start GTK event loop
  userData_in.GTKState.initializationHook = idle_initialize_UI_cb;
  userData_in.GTKState.finalizationHook = idle_finalize_UI_cb;
  userData_in.GTKState.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (UIDefinitionFile_in, static_cast<GtkBuilder*> (NULL));
  userData_in.GTKState.userData = &userData_in;

  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
  if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start GTK event dispatch, aborting\n")));
    return;
  } // end IF

  // step2: initialize worker(s)
  int group_id = -1;
  if (!Common_Tools::startEventDispatch (IRC_CLIENT_DEF_CLIENT_USES_REACTOR,
                                         numDispatchThreads_in,
                                         group_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, aborting\n")));

    // clean up
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();

    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

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
    if (IRC_CLIENT_DEF_CLIENT_USES_REACTOR)
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

  // wait for connection processing to complete
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->abortConnections ();
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->waitConnections ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
do_parsePhonebookFile (const std::string& serverConfigFile_in,
                       RPG_Net_Protocol_PhoneBook& phoneBook_out)
{
  RPG_TRACE (ACE_TEXT ("::do_parsePhonebookFile"));

  // init return value(s)
  phoneBook_out.timestamp.update(ACE_Time_Value::zero);
  phoneBook_out.networks.clear();
  phoneBook_out.servers.clear();

  ACE_Configuration_Heap config_heap;
  if (config_heap.open())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Configuration_Heap::open() failed, aborting\n")));

    return;
  } // end IF

  ACE_Ini_ImpExp import(config_heap);
  if (import.import_config(serverConfigFile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Ini_ImpExp::import_config(\"%s\") failed, aborting\n"),
               serverConfigFile_in.c_str()));

    return;
  } // end IF

  // step1: find/open "timestamp" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_TIMESTAMP_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(\"%s\"), aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_TIMESTAMP_SECTION_HEADER)));

    return;
  } // end IF

  // import value...
  int val_index = 0;
  ACE_TString val_name, val_value;
  ACE_Configuration::VALUETYPE val_type;
  std::stringstream converter;
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
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(\"%s\"), aborting\n"),
                 val_name.c_str()));

      return;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    // *TODO*: move these strings...
    if (val_name == ACE_TEXT("date"))
    {
      std::string timestamp = val_value.c_str();
      // parse timestamp
      std::string::size_type current_fwd_slash = 0;
      std::string::size_type last_fwd_slash = 0;
      current_fwd_slash = timestamp.find('/', 0);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
      if (current_fwd_slash == -1)
#else
      if (current_fwd_slash == std::string::npos)
#endif
      {
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("\"%s\": failed to parse timestamp (was: \"%s\"), aborting\n"),
                   serverConfigFile_in.c_str(),
                   val_value.c_str()));

        return;
      } // end IF
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter.clear();
      converter << timestamp.substr(0,
                                    current_fwd_slash);
      long day = 0;
      converter >> day;
      phoneBook_out.timestamp.day(day);
      last_fwd_slash = current_fwd_slash;
      current_fwd_slash = timestamp.find('/', current_fwd_slash + 1);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
      if (current_fwd_slash == -1)
#else
      if (current_fwd_slash == std::string::npos)
#endif
      {
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("\"%s\": failed to parse timestamp (was: \"%s\"), aborting\n"),
                   serverConfigFile_in.c_str(),
                   val_value.c_str()));

        return;
      } // end IF
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter.clear();
      converter << timestamp.substr(last_fwd_slash + 1,
                                    last_fwd_slash - current_fwd_slash - 1);
      long month = 0;
      converter >> month;
      phoneBook_out.timestamp.month(month);
      last_fwd_slash = current_fwd_slash;
      current_fwd_slash = timestamp.find('/', current_fwd_slash + 1);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
      if (current_fwd_slash != -1)
#else
      if (current_fwd_slash != std::string::npos)
#endif
      {
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("\"%s\": failed to parse timestamp (was: \"%s\"), aborting\n"),
                   serverConfigFile_in.c_str(),
                   val_value.c_str()));

        return;
      } // end IF
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter.clear();
      converter << timestamp.substr(last_fwd_slash + 1,
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
                                    -1);
#else
                                    std::string::npos);
#endif
      long year = 0;
      converter >> year;
      phoneBook_out.timestamp.year(year);
    }

    ++val_index;
  } // end WHILE

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("timestamp (d/m/y, h:m:s.u): %d/%d/%d, %d:%d:%d.%d\n"),
//              phoneBook_out.timestamp.day(),
//              phoneBook_out.timestamp.month(),
//              phoneBook_out.timestamp.year(),
//              phoneBook_out.timestamp.hour(),
//              phoneBook_out.timestamp.minute(),
//              phoneBook_out.timestamp.second(),
//              phoneBook_out.timestamp.microsec()));

  // step2: find/open "networks" section...
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_NETWORKS_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(\"%s\"), aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_NETWORKS_SECTION_HEADER)));

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
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(\"%s\"), aborting\n"),
                 val_name.c_str()));

      return;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    phoneBook_out.networks.insert(std::string(val_value.c_str()));

    ++val_index;
  } // end WHILE

  // step3: find/open "servers" section...
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_SERVERS_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(\"%s\"), aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_SERVERS_SECTION_HEADER)));

    return;
  } // end IF

  // import values...
  val_index = 0;
  RPG_Net_Protocol_ConnectionEntry entry;
  std::string entry_name;
  RPG_Net_Protocol_PortRange_t port_range;
  bool no_range = false;
  while (config_heap.enumerate_values(section_key,
                                      val_index,
                                      val_name,
                                      val_type) == 0)
  {
    entry.listeningPorts.clear();

    if (config_heap.get_string_value(section_key,
                                     val_name.c_str(),
                                     val_value))
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(\"%s\"), aborting\n"),
                 val_name.c_str()));

      return;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    std::string server_line_string = val_value.c_str();

    // parse connection name
    std::string::size_type current_position = 0;
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
    std::string::size_type last_position = -1;
#else
    std::string::size_type last_position = std::string::npos;
#endif
    current_position = server_line_string.find(ACE_TEXT_ALWAYS_CHAR("SERVER:"), 0);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
    if (current_position == -1)
#else
    if (current_position == std::string::npos)
#endif
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("\"%s\": failed to parse server (was: \"%s\"), aborting\n"),
                 serverConfigFile_in.c_str(),
                 val_value.c_str()));

      return;
    } // end IF
    // *TODO*: needs further parsing...
    entry_name = server_line_string.substr(0,
                                           current_position);
    last_position = current_position + 6;

    // parse hostname
    current_position = server_line_string.find(':', last_position + 1);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
    if (current_position == -1)
#else
    if (current_position == std::string::npos)
#endif
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("\"%s\": failed to parse server (was: \"%s\"), aborting\n"),
                 serverConfigFile_in.c_str(),
                 val_value.c_str()));

      return;
    } // end IF
    entry.hostName = server_line_string.substr(last_position + 1,
                                               current_position - last_position - 1);
    last_position = current_position;

    // parse (list of) port ranges
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
    std::string::size_type next_comma = -1;
#else
    std::string::size_type next_comma = std::string::npos;
#endif
    std::string::size_type group = server_line_string.find(ACE_TEXT_ALWAYS_CHAR("GROUP:"), current_position + 1);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
    if (group == -1)
#else
    if (group == std::string::npos)
#endif
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("\"%s\": failed to parse server (was: \"%s\"), aborting\n"),
                 serverConfigFile_in.c_str(),
                 val_value.c_str()));

      return;
    } // end IF
    do
    {
      no_range = false;

      next_comma = server_line_string.find(',', current_position + 1);
      if (next_comma > group)
        next_comma = group;

      // port range ?
      current_position = server_line_string.find('-', current_position + 1);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
      if ((current_position == -1) ||
#else
      if ((current_position == std::string::npos) ||
#endif
          (current_position > next_comma))
        no_range = true;
      else
      {
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter.clear();
        converter << server_line_string.substr(last_position + 1,
                                               current_position - last_position - 1);
        converter >> port_range.first;

        last_position = current_position;
      } // end ELSE
      current_position = next_comma;
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter.clear();
      converter << server_line_string.substr(last_position + 1,
                                             current_position - last_position - 1);
      converter >> port_range.second;
      if (no_range)
        port_range.first = port_range.second;
      entry.listeningPorts.push_back(port_range);

      // skip to next port(range)
      if (next_comma == group)
      {
        // this means we've reached the end of the list...
        // --> skip over "GROUP:"
        last_position = next_comma + 5;

        // proceed
        break;
      } // end IF

      last_position = current_position;
    } while (true);

    // parse "group" (== network)
    entry.network = server_line_string.substr(last_position + 1);

    phoneBook_out.networks.insert(entry.network);
    phoneBook_out.servers.insert(std::make_pair(entry_name, entry));

    ++val_index;
  } // end WHILE

//   for (RPG_Net_Protocol_NetworksIterator_t iterator = phoneBook_out.networks.begin();
//        iterator != phoneBook_out.networks.end();
//        iterator++)
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("network: \"%s\"\n"),
//                (*iterator).c_str()));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("parsed %u phonebook (timestamp: %u/%u/%u) entries (%u network(s))...\n"),
              phoneBook_out.servers.size (),
              phoneBook_out.timestamp.month (),
              phoneBook_out.timestamp.day (),
              phoneBook_out.timestamp.year (),
              phoneBook_out.networks.size ()));
}

void
do_parseConfigurationFile (const std::string& configFilename_in,
                           RPG_Net_Protocol_IRCLoginOptions& loginOptions_out,
                           RPG_Net_Protocol_PhoneBook& phoneBook_out)
{
  RPG_TRACE (ACE_TEXT ("::do_parseConfigurationFile"));

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
                               ACE_TEXT(IRC_CLIENT_CNF_LOGIN_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               ACE_TEXT(IRC_CLIENT_CNF_LOGIN_SECTION_HEADER)));

    return;
  } // end IF

  // import values...
  int val_index = 0;
  ACE_TString val_name, val_string_value;
  ACE_Configuration::VALUETYPE val_type;
  while (config_heap.enumerate_values(section_key,
                                      val_index,
                                      val_name,
                                      val_type) == 0)
  {
    if (val_type == ACE_Configuration::STRING)
      if (config_heap.get_string_value(section_key,
                                       val_name.c_str(),
                                       val_string_value))
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
      loginOptions_out.password = val_string_value.c_str();
    }
    else if (val_name == ACE_TEXT("nick"))
    {
      loginOptions_out.nick = val_string_value.c_str();
    }
    else if (val_name == ACE_TEXT("user"))
    {
      loginOptions_out.user.username = val_string_value.c_str();
    }
    else if (val_name == ACE_TEXT("realname"))
    {
      loginOptions_out.user.realname = val_string_value.c_str();
    }
    else if (val_name == ACE_TEXT("channel"))
    {
      loginOptions_out.channel = val_string_value.c_str();
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
  RPG_Net_Protocol_ConnectionEntry entry;
//   u_int port = 0;
  std::stringstream converter;
  while (config_heap.enumerate_values(section_key,
                                      val_index,
                                      val_name,
                                      val_type) == 0)
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    ACE_ASSERT(val_type == ACE_Configuration::STRING);
    if (config_heap.get_string_value(section_key,
                                     val_name.c_str(),
                                     val_string_value))
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
                 val_name.c_str()));

      return;
    } // end IF

    // *TODO*: move these strings...
    if (val_name == ACE_TEXT("server"))
    {
      entry.hostName = val_string_value.c_str();

      if (!entry.listeningPorts.empty())
        phoneBook_out.servers.insert(std::make_pair(entry.hostName, entry));
    }
    else if (val_name == ACE_TEXT("port"))
    {
//       ACE_ASSERT(val_type == ACE_Configuration::INTEGER);
//       if (config_heap.get_integer_value(section_key,
//                                         val_name.c_str(),
//                                         port))
//       {
//         ACE_ERROR((LM_ERROR,
//                    ACE_TEXT("failed to ACE_Configuration_Heap::get_integer_value(%s), returning\n"),
//                    val_name.c_str()));
//
//         return;
//       } // end IF
      RPG_Net_Protocol_PortRange_t port_range;
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter.clear();
      converter << val_string_value.c_str();
      converter >> port_range.first;
//       port_range.first = static_cast<unsigned short> (port);
      port_range.second = port_range.first;
      entry.listeningPorts.push_back(port_range);

      if (!entry.hostName.empty())
        phoneBook_out.servers.insert(std::make_pair(entry.hostName, entry));
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 val_name.c_str()));
    } // end ELSE

    ++val_index;
  } // end WHILE

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("imported \"%s\"...\n"),
//              configFilename_in.c_str()));
}

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
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

  // step1: initialize libraries
  // *PORTABILITY*: on Windows, init ACE...
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
  process_profile.start ();

  // step2 initialize/validate configuration

  // step2a: process commandline arguments
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
#endif // #ifdef DEBUG_DEBUGGER
  configuration_file += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_CNF_DEF_INI_FILE);

  bool debug                      = IRC_CLIENT_DEF_TRACE_ENABLED;
  bool log_to_file                = false;
  unsigned int reporting_interval = IRC_CLIENT_DEF_STATSINTERVAL;

  std::string phonebook_file = configuration_path;
  phonebook_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  phonebook_file += ACE_TEXT_ALWAYS_CHAR ("protocol");
  phonebook_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #ifdef DEBUG_DEBUGGER
  phonebook_file += ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_DEF_SERVERS_FILE);

  bool trace_information       = false;

  std::string UIFile_directory = configuration_path;
  UIFile_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  UIFile_directory += ACE_TEXT_ALWAYS_CHAR ("protocol");
#endif // #ifdef DEBUG_DEBUGGER

  bool print_version_and_exit          = false;
  unsigned int num_thread_pool_threads = IRC_CLIENT_DEF_NUM_TP_THREADS;
  if (!do_processArguments (argc_in,
                            argv_in,
                            configuration_file,
                            debug,
                            log_to_file,
                            reporting_interval,
                            phonebook_file,
                            trace_information,
                            UIFile_directory,
                            print_version_and_exit,
                            num_thread_pool_threads))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
      return EXIT_FAILURE;
    } // end IF
#endif

    return EXIT_FAILURE;
  } // end IF

  // assemble FQ filename (Glade-UI XML)
  std::string ui_definition_filename = UIFile_directory;
  ui_definition_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_filename += IRC_CLIENT_GUI_DEF_UI_MAIN_FILE;

  // step2b: validate argument(s)
  if (!Common_File_Tools::isReadable (configuration_file)    ||
      !Common_File_Tools::isReadable (phonebook_file)        ||
      !Common_File_Tools::isReadable (ui_definition_filename))
  {
    // make 'em learn...
    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
      return EXIT_FAILURE;
    } // end IF
#endif

    return EXIT_FAILURE;
  } // end IF

  // step3: initialize logging and/or tracing
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

  // step4: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
      return EXIT_FAILURE;
    } // end IF
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step5: initialize configuration objects

  // initialize protocol configuration
  Stream_CachedAllocatorHeap heap_allocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                             RPG_NET_PROTOCOL_BUFFER_SIZE);
  RPG_Net_Protocol_MessageAllocator message_allocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                       &heap_allocator);

  RPG_Net_Protocol_Configuration configuration;
  ACE_OS::memset (&configuration, 0, sizeof (configuration));

  configuration.streamConfiguration.messageAllocator = &message_allocator;
  configuration.streamConfiguration.statisticReportingInterval =
   reporting_interval;
  configuration.protocolConfiguration.streamConfiguration.debugScanner =
    RPG_NET_PROTOCOL_DEF_TRACE_SCANNING;
  configuration.protocolConfiguration.streamConfiguration.debugParser = debug;

  main_cb_data_t user_data;
  user_data.configuration = &configuration;
  user_data.UIFileDirectory = UIFile_directory;
//   userData.phoneBook;
//   userData.loginOptions.password = ;
  user_data.configuration->protocolConfiguration.loginOptions.nick =
    ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_DEF_IRC_NICK);
//   userData.loginOptions.user.username = ;
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

  // parse configuration file(s) (if any)
  if (!phonebook_file.empty ())
    do_parsePhonebookFile (phonebook_file,
                           user_data.phoneBook);
  if (!configuration_file.empty ())
    do_parseConfigurationFile (configuration_file,
                               configuration.protocolConfiguration.loginOptions,
                               user_data.phoneBook);

  // step6: initialize GTK UI
  Common_UI_GtkBuilderDefinition ui_definition (argc_in,
                                                argv_in);
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                            argv_in,
                                                            &user_data.GTKState,
                                                            &ui_definition);

  // step7: do work
  ACE_High_Res_Timer timer;
  timer.start ();
  do_work (num_thread_pool_threads,
           user_data,
           ui_definition_filename);

  // debug info
  timer.stop ();
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  Common_Tools::period2String (working_time,
                               working_time_string);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // debug info
  process_profile.stop ();
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
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
      return EXIT_FAILURE;
    } // end IF
#endif

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
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
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#endif

  // step8: finalize libraries
  // *PORTABILITY*: on Windows, fini ACE...
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
