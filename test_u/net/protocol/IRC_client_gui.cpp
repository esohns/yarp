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

#include "IRC_client_defines.h"
#include "IRC_client_gui_defines.h"
#include "IRC_client_gui_common.h"
#include "IRC_client_gui_callbacks.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_messageallocator.h"

#include "rpg_net_connection_manager.h"

#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_stream_cachedallocatorheap.h"

#include <gtk/gtk.h>

#include <ace/ACE.h>
#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Proactor.h>
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
#include <map>

void
is_entry_sensitive(GtkCellLayout*   layout_in,
                   GtkCellRenderer* renderer_in,
                   GtkTreeModel*    model_in,
                   GtkTreeIter*     iter_in,
                   gpointer         data_in)
{
//   RPG_TRACE(ACE_TEXT("::is_entry_sensitive"));

  gboolean sensitive = !gtk_tree_model_iter_has_child(model_in, iter_in);
  // set corresponding property
  g_object_set(renderer_in,
               ACE_TEXT_ALWAYS_CHAR("sensitive"), sensitive,
               NULL);
}

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                     true);
#endif // #ifdef BASEDIR

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("protocol");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_DEF_INI_FILE);
  std::cout << ACE_TEXT("-c [FILE] : config file") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-d        : debug") << ACE_TEXT(" [") << IRC_CLIENT_DEF_TRACE_ENABLED << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-l        : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-r [VALUE]: reporting interval (seconds: 0 --> OFF)") << ACE_TEXT(" [") << IRC_CLIENT_DEF_STATSINTERVAL << ACE_TEXT("]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("protocol");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_GUI_DEF_SERVERS_FILE);
  std::cout << ACE_TEXT("-s [FILE] : server config file") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("protocol");
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  std::cout << ACE_TEXT("-u [DIR]  : UI file directory") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]: #thread pool threads ([") << IRC_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& configFile_out,
                  bool& debug_out,
                  bool& logToFile_out,
                  unsigned int& reportingInterval_out,
                  std::string& serverConfigFile_out,
                  bool& traceInformation_out,
                  std::string& UIFileDirectory_out,
                  bool& printVersionAndExit_out,
                  unsigned int& numThreadPoolThreads_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                     true);
#endif // #ifdef BASEDIR

  // init results
  configFile_out = config_path;
  configFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  configFile_out += ACE_TEXT_ALWAYS_CHAR("protocol");
  configFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  configFile_out += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_DEF_INI_FILE);

  debug_out             = IRC_CLIENT_DEF_TRACE_ENABLED;
  logToFile_out         = false;
  reportingInterval_out = IRC_CLIENT_DEF_STATSINTERVAL;

  serverConfigFile_out = config_path;
  serverConfigFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  serverConfigFile_out += ACE_TEXT_ALWAYS_CHAR("protocol");
  serverConfigFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  serverConfigFile_out += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_GUI_DEF_SERVERS_FILE);

  traceInformation_out     = false;

  UIFileDirectory_out = config_path;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  UIFileDirectory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFileDirectory_out += ACE_TEXT_ALWAYS_CHAR("protocol");
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)

  printVersionAndExit_out  = false;
  numThreadPoolThreads_out = IRC_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:dlr:s:tu:vx:"),
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
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter.clear();
        converter << argumentParser.opt_arg();
        converter >> reportingInterval_out;

        break;
      }
      case 's':
      {
        serverConfigFile_out = argumentParser.opt_arg();

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UIFileDirectory_out = argumentParser.opt_arg();

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
do_main_window(const std::string& UIFileDirectory_in,
               const main_cb_data_t& userData_in,
               const GtkWidget* parentWidget_in)
{
  RPG_TRACE(ACE_TEXT("::do_main_window"));

  ACE_ASSERT(userData_in.builder);

  // step0: assemble FQ filename (Glade-UI XML)
  std::string filename = UIFileDirectory_in;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += IRC_CLIENT_GUI_DEF_UI_MAIN_FILE;
  if (!RPG_Common_File_Tools::isReadable(filename))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid UI file (was \"%s\"): not readable, aborting\n"),
               filename.c_str()));

    return;
  } // end IF

  // step1: load widget tree
  GError* error = NULL;
  gtk_builder_add_from_file(userData_in.builder,
                            filename.c_str(),
                            &error);
  if (error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
               filename.c_str(),
               error->message));

    // clean up
    g_error_free(error);

    return;
  } // end IF

  // step2: populate phonebook liststore
  GtkTreeStore* main_servers_treestore = NULL;
  main_servers_treestore = GTK_TREE_STORE(gtk_builder_get_object(userData_in.builder,
                                          ACE_TEXT_ALWAYS_CHAR("main_servers_treestore")));
  ACE_ASSERT(main_servers_treestore);
  GtkComboBox* main_servers_combobox = NULL;
  main_servers_combobox = GTK_COMBO_BOX(gtk_builder_get_object(userData_in.builder,
                                        ACE_TEXT_ALWAYS_CHAR("main_servers_combobox")));
  ACE_ASSERT(main_servers_combobox);
  // *NOTE*: the combobox will display (selectable) column headers --> don't want that
  GList* renderers = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(main_servers_combobox));
  GtkCellRenderer* renderer = GTK_CELL_RENDERER(g_list_first(renderers)->data);
  ACE_ASSERT(renderer);
  g_list_free(renderers);
  gtk_cell_layout_set_cell_data_func(GTK_CELL_LAYOUT(main_servers_combobox), renderer,
                                     is_entry_sensitive,
                                     NULL, NULL);
  std::map<std::string, GtkTreeIter> network_map;
  std::map<std::string, GtkTreeIter>::iterator network_map_iterator;
  GtkTreeIter tree_iterator, current_row;
  for (RPG_Net_Protocol_ServersIterator_t iterator = userData_in.phoneBook.servers.begin();
       iterator != userData_in.phoneBook.servers.end();
       iterator++)
  {
    // known network ?
    network_map_iterator = network_map.find((*iterator).second.network);
    if (network_map_iterator == network_map.end())
    {
      // new toplevel row
      gtk_tree_store_append(main_servers_treestore,
                            &tree_iterator,
                            NULL);
      std::string network_label = ((*iterator).second.network.empty() ? ACE_TEXT_ALWAYS_CHAR("<none>")
                                                                      : (*iterator).second.network);
      gtk_tree_store_set(main_servers_treestore, &tree_iterator,
                         0, network_label.c_str(),
                         -1);

      network_map.insert(std::make_pair((*iterator).second.network, tree_iterator));

      network_map_iterator = network_map.find((*iterator).second.network);
      ACE_ASSERT(network_map_iterator != network_map.end());
    } // end IF

    // append new (text) entry
    gtk_tree_store_append(main_servers_treestore,
                          &current_row,
                          &(*network_map_iterator).second);
    gtk_tree_store_set(main_servers_treestore, &current_row,
                       0, (*iterator).first.c_str(), // column 0
                       -1);

    // set active item
    if ((*iterator).first == IRC_CLIENT_DEF_SERVER_HOSTNAME)
      gtk_combo_box_set_active_iter(main_servers_combobox,
                                    &current_row);
  } // end FOR
  if (!userData_in.phoneBook.servers.empty())
  {
    // sort entries (toplevel: ascending)
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(main_servers_treestore),
                                         0, GTK_SORT_ASCENDING);

    gtk_widget_set_sensitive(GTK_WIDGET(main_servers_combobox), TRUE);
  } // end IF

  // step3: connect signals/slots
//   gtk_builder_connect_signals(builder,
//                               &const_cast<main_cb_data&> (userData_in));
  GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(userData_in.builder,
                                                     ACE_TEXT_ALWAYS_CHAR("main_send_entry")));
  ACE_ASSERT(entry);
  g_signal_connect(entry,
                   ACE_TEXT_ALWAYS_CHAR("focus-in-event"),
                   G_CALLBACK(send_entry_kb_focused_cb),
                   &const_cast<main_cb_data_t&>(userData_in));
  GtkButton* button = GTK_BUTTON(gtk_builder_get_object(userData_in.builder,
                                                        ACE_TEXT_ALWAYS_CHAR("main_send_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(send_clicked_cb),
                   &const_cast<main_cb_data_t&>(userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(userData_in.builder,
                                             ACE_TEXT_ALWAYS_CHAR("main_connect_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(connect_clicked_cb),
                   &const_cast<main_cb_data_t&>(userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(userData_in.builder,
                                             ACE_TEXT_ALWAYS_CHAR("main_quit_buton")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(quit_activated_cb),
                   NULL);

  // step4: retrieve toplevel handle
  GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(userData_in.builder,
                                                        ACE_TEXT_ALWAYS_CHAR("main_dialog")));
  ACE_ASSERT(window);
  if (!window)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_builder_get_object(\"main_dialog\"): \"%m\", aborting\n")));

    return;
  } // end IF
  // connect default signals
  g_signal_connect(window,
                   ACE_TEXT_ALWAYS_CHAR("delete-event"),
                   G_CALLBACK(quit_activated_cb),
                   window);
//   g_signal_connect(window,
//                    ACE_TEXT_ALWAYS_CHAR("destroy-event"),
//                    G_CALLBACK(quit_activated_cb),
//                    window);
  g_signal_connect(window,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(gtk_widget_destroyed),
                   window);

  // use correct screen
  if (parentWidget_in)
    gtk_window_set_screen(window,
                          gtk_widget_get_screen(const_cast<GtkWidget*>(parentWidget_in)));

  // step5: draw it
  gtk_widget_show_all(GTK_WIDGET(window));
}

void
do_work(const unsigned int& numThreadPoolThreads_in,
        const std::string& UIFileDirectory_in,
        main_cb_data_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0: (if necessary) init the thread pool
  if (!init_threadPool(IRC_CLIENT_DEF_CLIENT_USES_REACTOR, numThreadPoolThreads_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init thread pool, aborting\n")));

    return;
  } // end IF


  // step0b: init connection manager
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max());

  // *WARNING*: from this point on, we need to clean up any remote connections !
  // *NOTE* handlers register with the connection manager and will self-destruct
  // on disconnects !

  // step1: setup UI
  do_main_window(UIFileDirectory_in, // glade file directory
                 userData_in,        // cb data
                 NULL);              // there's no parent widget

  // event loops:
  // - perform socket I/O --> ACE_Reactor
  // - UI events --> GTK main loop

  // step2: dispatch events...
  // *NOTE*: if we use a thread pool, we need to do this differently...
  bool thread_argument = IRC_CLIENT_DEF_CLIENT_USES_REACTOR;
  int grp_id = -1;

  // start a (group of) worker thread(s)...
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

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("spawned %u event handlers (group ID: %u)...\n"),
             numThreadPoolThreads_in,
             grp_id));

  // dispatch GTK events
  // *WARNING*: this doesn't really make any sense - still, it seems to be a
  // requirement to somehow "initialize" the mutex from the "main" thread...
  // IOW: without this, gdk_threads_enter(), when first invoked from a child thread,
  // will block indefinetly (go on, try it !)
  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished GTK event dispatching...\n")));

  // done handling UI events

  // stop reactor/proactor
  if (IRC_CLIENT_DEF_CLIENT_USES_REACTOR)
  {
    if (ACE_Reactor::instance()->end_event_loop() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to terminate event handling: \"%m\", continuing\n")));
  }
  else
    if (ACE_Proactor::instance()->end_event_loop() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to terminate event handling: \"%m\", continuing\n")));

  // no more data will be enqueued onto the processing streams...

  // wait for the reactor worker(s) to join
  ACE_Thread_Manager::instance()->wait_grp(grp_id);

  // no more data will arrive from here on...

  // wait for connection processing to complete
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
  RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseServerConfigFile(const std::string& serverConfigFile_in,
                         RPG_Net_Protocol_PhoneBook& phoneBook_out)
{
  RPG_TRACE(ACE_TEXT("::do_parseServerConfigFile"));

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

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("parsed %u phonebook (timestamp: %u/%u/%u) entries (%u network(s))...\n"),
             phoneBook_out.servers.size(),
             phoneBook_out.timestamp.month(),
             phoneBook_out.timestamp.day(),
             phoneBook_out.timestamp.year(),
             phoneBook_out.networks.size()));
}

void
do_parseConfigFile(const std::string& configFilename_in,
                   RPG_Net_Protocol_IRCLoginOptions& loginOptions_out,
                   RPG_Net_Protocol_PhoneBook& phoneBook_out)
{
  RPG_TRACE(ACE_TEXT("::do_parseConfigFile"));

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
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

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
ACE_TMAIN(int argc,
          ACE_TCHAR* argv[])
{
//   RPG_TRACE(ACE_TEXT("::main"));

  // step1: init libraries
//  // *PORTABILITY*: on Windows, we need to init ACE...
//#if defined(ACE_WIN32) || defined(ACE_WIN64)
//  if (ACE::init() == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));
//
//    return EXIT_FAILURE;
//  } // end IF
//#endif

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  // init GTK
//  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(&argc, &argv);

  // step2 init/validate configuration

  // step2a: process commandline arguments
  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                     true);
#endif // #ifdef BASEDIR

  std::string configFile = config_path;
  configFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  configFile += ACE_TEXT_ALWAYS_CHAR("protocol");
  configFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  configFile += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_CNF_DEF_INI_FILE);

  bool debug                     = IRC_CLIENT_DEF_TRACE_ENABLED;
  bool logToFile                 = false;
  unsigned int reportingInterval = IRC_CLIENT_DEF_STATSINTERVAL;

  std::string serverConfigFile = config_path;
  serverConfigFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  serverConfigFile += ACE_TEXT_ALWAYS_CHAR("protocol");
  serverConfigFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)
  serverConfigFile += ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_GUI_DEF_SERVERS_FILE);

  bool traceInformation       = false;

  std::string UIFileDirectory = config_path;
  UIFileDirectory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  UIFileDirectory += ACE_TEXT_ALWAYS_CHAR("protocol");
#endif // #if (defined _DEBUG) || (defined DEBUG_RELEASE)

  bool printVersionAndExit          = false;
  unsigned int numThreadPoolThreads = IRC_CLIENT_DEF_NUM_TP_THREADS;
  if (!(process_arguments(argc,
                          argv,
                          configFile,
                          debug,
                          logToFile,
                          reportingInterval,
                          serverConfigFile,
                          traceInformation,
                          UIFileDirectory,
                          printVersionAndExit,
                          numThreadPoolThreads)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!RPG_Common_File_Tools::isReadable(configFile) ||
      !RPG_Common_File_Tools::isReadable(serverConfigFile) ||
      !RPG_Common_File_Tools::isDirectory(UIFileDirectory))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

  // step2b: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step2c: set correct verbosity
  u_long process_priority_mask = (LM_SHUTDOWN |
//                                   LM_TRACE |  // <-- DISABLE trace messages !
//                                   LM_DEBUG |  // <-- DISABLE ACE_DEBUG messages !
                                  LM_INFO |
                                  LM_NOTICE |
                                  LM_WARNING |
                                  LM_STARTUP |
                                  LM_ERROR |
                                  LM_CRITICAL |
                                  LM_ALERT |
                                  LM_EMERGENCY);
  if (traceInformation)
  {
    process_priority_mask |= LM_DEBUG;

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF
  // set process priority mask...
  ACE_LOG_MSG->priority_mask(process_priority_mask,
                             ACE_Log_Msg::PROCESS);

  // init global (!) processing stream message allocator
  RPG_Stream_CachedAllocatorHeap heapAllocator(RPG_NET_MAX_MESSAGES,
                                               RPG_NET_PROTOCOL_BUFFER_SIZE);
  RPG_Net_Protocol_MessageAllocator messageAllocator(RPG_NET_MAX_MESSAGES,
                                                     &heapAllocator);

  // step2d: init callback data
  main_cb_data_t userData;
  userData.allocator = &messageAllocator;
  userData.debugScanner = RPG_NET_PROTOCOL_DEF_TRACE_SCANNING;
  userData.debugParser = debug;
  userData.statisticsReportingInterval = reportingInterval;
  userData.UIFileDirectory = UIFileDirectory;
  userData.builder = gtk_builder_new();
  ACE_ASSERT(userData.builder);
//   userData.phoneBook;
//   userData.loginOptions.password = ;
  userData.loginOptions.nick = ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_DEF_IRC_NICK);
//   userData.loginOptions.user.username = ;
  std::string hostname = RPG_Common_Tools::getHostName();
  if (IRC_CLIENT_CNF_IRC_USERMSG_TRADITIONAL)
  {
    userData.loginOptions.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::STRING;
    userData.loginOptions.user.hostname.string = &hostname;
  } // end IF
  else
  {
    userData.loginOptions.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::BITMASK;
    // *NOTE*: hybrid-7.2.3 seems to have a bug: 4 --> +i
    userData.loginOptions.user.hostname.mode = IRC_CLIENT_DEF_IRC_USERMODE;
  } // end ELSE
  userData.loginOptions.user.servername = ACE_TEXT_ALWAYS_CHAR(RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME);
//  userData.loginOptions.user.username = ;
//  userData.loginOptions.user.realname = ;
  userData.loginOptions.channel = ACE_TEXT_ALWAYS_CHAR(IRC_CLIENT_DEF_IRC_CHANNEL);
  userData.connections.clear();

  // populate user/realname
  RPG_Common_Tools::getCurrentUserName(userData.loginOptions.user.username,
                                       userData.loginOptions.user.realname);

  // step2e: parse config file(s) (if any)
  if (!serverConfigFile.empty())
    do_parseServerConfigFile(serverConfigFile,
                             userData.phoneBook);
  if (!configFile.empty())
    do_parseConfigFile(configFile,
                       userData.loginOptions,
                       userData.phoneBook);

  // step3: do actual work
  ACE_High_Res_Timer timer;
  timer.start();
  do_work(numThreadPoolThreads,
          UIFileDirectory,
          userData);

  // debug info
  timer.stop();
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             working_time_string.c_str()));

  // clean up
  g_object_unref(userData.builder);

  // debug info
  process_profile.stop();
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

  // step4: fini libraries
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

  return EXIT_SUCCESS;
} // end main
