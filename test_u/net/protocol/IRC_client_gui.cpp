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

#include "IRC_common.h"
#include "IRC_client_gui_messagehandler.h"

#include <rpg_net_protocol_defines.h>
#include <rpg_net_protocol_common.h>
#include <rpg_net_protocol_messageallocator.h>
#include <rpg_net_protocol_module_IRChandler.h>

#include <rpg_net_defines.h>
#include <rpg_net_connection_manager.h>

#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <stream_allocatorheap.h>

#include <gtk/gtk.h>

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

#define IRC_CLIENT_CNF_CLIENT_SECTION_HEADER     ACE_TEXT("client")
#define IRC_CLIENT_CNF_CONNECTION_SECTION_HEADER ACE_TEXT("connection")
#define IRC_CLIENT_DEF_SERVER_HOSTNAME           ACE_LOCALHOST
#define IRC_CLIENT_DEF_SERVER_PORT               6667
#define IRC_CLIENT_DEF_CLIENT_USES_TP            false
#define IRC_CLIENT_DEF_NUM_TP_THREADS            5
#define IRC_CLIENT_DEF_UI_FILE                   ACE_TEXT("IRC_client.glade")
#define IRC_CLIENT_DEF_LEAVE_REASON              ACE_TEXT("quitting...")

struct cb_data
{
  RPG_Net_Protocol_IRCLoginOptions loginOptions;
  RPG_Net_Protocol_IIRCControl*    controller;
  IRC_Client_GUI_MessageHandler*   messageHandler;
};

static GtkBuilder* builder  = NULL;
static GtkWidget*  window   = NULL;
static int         grp_id   = -1;
static cb_data     userData;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
void
join_clicked_cb(GtkWidget* button_in,
                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::join_clicked_cb"));

  ACE_UNUSED_ARG(button_in);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("join_clicked_cb...\n")));

  // sanity check(s)
  ACE_ASSERT(userData_in);
  cb_data* data = ACE_static_cast(cb_data*,
                                  userData_in);
  try
  {
    data->controller->join(data->loginOptions.channel);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::join(), continuing\n")));
  }
}

void
part_clicked_cb(GtkWidget* button_in,
                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::part_clicked_cb"));

  ACE_UNUSED_ARG(button_in);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("part_clicked_cb...\n")));

  // sanity check(s)
  ACE_ASSERT(userData_in);
  cb_data* data = ACE_static_cast(cb_data*,
                                  userData_in);
  try
  {
    data->controller->part(data->loginOptions.channel);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::part(), continuing\n")));
  }
}

void
register_clicked_cb(GtkWidget* button_in,
                    gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::register_clicked_cb"));

  ACE_UNUSED_ARG(button_in);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("register_clicked_cb...\n")));

  // sanity check(s)
  ACE_ASSERT(userData_in);
  cb_data* data = ACE_static_cast(cb_data*,
                                  userData_in);
  try
  {
    data->controller->registerConnection(data->loginOptions,
                                         data->messageHandler);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::registerConnection(), continuing\n")));
  }
}

void
disconnect_clicked_cb(GtkWidget* button_in,
                      gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::disconnect_clicked_cb"));

  ACE_UNUSED_ARG(button_in);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("disconnect_clicked_cb...\n")));

  // sanity check(s)
  ACE_ASSERT(userData_in);

  cb_data* data = ACE_static_cast(cb_data*,
                                  userData_in);
  try
  {
    data->controller->quit(std::string(IRC_CLIENT_DEF_LEAVE_REASON));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::quit(), continuing\n")));
  }
}

void
send_clicked_cb(GtkWidget* button_in,
                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::send_clicked_cb"));

  ACE_UNUSED_ARG(button_in);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("send_clicked_cb...\n")));

  // step1: retrieve available data
  // sanity check(s)
  ACE_ASSERT(builder);
  // retrieve buffer handle
  GtkEntryBuffer* buffer = NULL;
  GtkEntry* entry = NULL;
  entry = GTK_ENTRY(gtk_builder_get_object(builder,
                                           ACE_TEXT_ALWAYS_CHAR("entry")));
  ACE_ASSERT(entry);
  buffer = gtk_entry_get_buffer(entry);
  ACE_ASSERT(buffer);

  // sanity check
  if (gtk_entry_buffer_get_length(buffer) == 0)
    return; // nothing to do...

  // retrieve textbuffer data
  std::string message_string;
  message_string.append(gtk_entry_buffer_get_text(buffer),
                        gtk_entry_buffer_get_length(buffer));

  // step2: pass data to controller
  // sanity check(s)
  ACE_ASSERT(userData_in);

  cb_data* data = ACE_static_cast(cb_data*,
                                  userData_in);
  ACE_ASSERT(data->controller);
  try
  {
    data->controller->send(data->loginOptions.channel,
                           message_string);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::send(), continuing\n")));
  }

  // echo data locally...
  message_string.insert(0, ACE_TEXT("<me> "));
  message_string += ACE_TEXT_ALWAYS_CHAR("\n");
  ACE_ASSERT(data->messageHandler);
  data->messageHandler->queueForDisplay(message_string);

  // clear buffer
  gtk_entry_buffer_delete_text(buffer,
                               0,
                               gtk_entry_buffer_get_length(buffer));
}

gint
quit_activated_cb(GtkWidget* widget_in,
                  GdkEvent* event_in,
                  gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::quit_activated_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  // stop reactor
  if (ACE_Reactor::instance()->end_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
  } // end IF

  // ... and wait for the reactor worker(s) to join
  ACE_Thread_Manager::instance()->wait_grp(grp_id);

  // no more data will arrive from here on...

  // wait for connection processing to complete
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

  // finished processing data, no more data will be advertised
  // --> we can safely close the window...
  // sanity check(s)
//   ACE_ASSERT(window);
//   gtk_widget_destroy(window);

  // ...and leave GTK
  gtk_main_quit();

  // destroy the toplevel widget
  return TRUE;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-c [FILE]   : config file") << std::endl;
  std::cout << ACE_TEXT("-d          : debug parser") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-u [FILE]   : UI file") << ACE_TEXT(" [") << IRC_CLIENT_DEF_UI_FILE << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x<[VALUE]> : use thread pool <#threads>") << ACE_TEXT(" [") << IRC_CLIENT_DEF_CLIENT_USES_TP  << ACE_TEXT(" : ") << IRC_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& configFile_out,
                  bool& debugParser_out,
                  bool& logToFile_out,
                  bool& traceInformation_out,
                  std::string& UIfile_out,
                  bool& printVersionAndExit_out,
                  bool& useThreadPool_out,
                  unsigned long& numThreadPoolThreads_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  configFile_out           = ACE_TEXT(""); // cannot assume this !
  debugParser_out          = false;
  logToFile_out            = false;
  traceInformation_out     = false;
  UIfile_out               = IRC_CLIENT_DEF_UI_FILE;
  printVersionAndExit_out  = false;
  useThreadPool_out        = IRC_CLIENT_DEF_CLIENT_USES_TP;
  numThreadPoolThreads_out = IRC_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:dltu:vx::"),
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
      case 'u':
      {
        UIfile_out = argumentParser.opt_arg();

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

const bool
init_threadPool()
{
  ACE_TRACE(ACE_TEXT("::init_threadPool"));

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

static
ACE_THR_FUNC_RETURN
tp_worker_func(void* args_in)
{
  ACE_TRACE(ACE_TEXT("::tp_worker_func"));

  ACE_UNUSED_ARG(args_in);

  while (!ACE_Reactor::event_loop_done())
  {
    // block and wait for an event...
    if (ACE_Reactor::instance()->handle_events(NULL) == -1)
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("(%t) error handling events: \"%m\"\n")));
  } // end WHILE

  ACE_ERROR((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

static
ACE_THR_FUNC_RETURN
reactor_worker_func(void* args_in)
{
  ACE_TRACE(ACE_TEXT("::reactor_worker_func"));

  ACE_UNUSED_ARG(args_in);

  // assume ownership over the reactor...
  ACE_Reactor::instance()->owner(ACE_OS::thr_self(),
                                 NULL);

  if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", aborting\n")));

      // clean up
//       if (stressTestServer_in)
//       {
//         if (ACE_Reactor::instance()->cancel_timer(timerID,  // timer ID
//                                                   NULL,     // pointer to args passed to handler
//                                                   1) != 1)  // don't invoke handle_close() on handler
//         {
//           ACE_DEBUG((LM_ERROR,
//                     ACE_TEXT("failed to ACE_Reactor::cancel_timer(): \"%p\", continuing\n")));
//         } // end IF
//       } // end IF
  } // end IF

  // wait for any connection(s)
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

  ACE_ERROR((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

void
do_builder(const std::string& UIfile_in,
           const cb_data& userData_in,
           const GtkWidget* parentWidget_in,
           GtkTextView*& textView_out)
{
  ACE_TRACE(ACE_TEXT("::do_builder"));

  // init return value(s)
  textView_out = NULL;

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(UIfile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("file \"%s\" doesn't exist, aborting\n"),
               UIfile_in.c_str()));

    return;
  } // end IF
  if (builder)
  {
    // clean up
    g_object_unref(G_OBJECT(builder));
    builder = NULL;
  } // end IF
  if (window)
  {
    // clean up
    gtk_widget_destroy(window);
    window = NULL;
  } // end IF

  // step1: load widget tree
  builder = gtk_builder_new();
  ACE_ASSERT(builder);
  if (!builder)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_builder_new(): \"%m\", aborting\n")));

    return;
  } // end IF
  GError* error = NULL;
  gtk_builder_add_from_file(builder,
                            UIfile_in.c_str(),
                            &error);
  if (error)
  {
    ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
                UIfile_in.c_str(),
                error->message));

    // clean up
    g_error_free(error);
    g_object_unref(G_OBJECT(builder));
    builder = NULL;

    return;
  } // end IF

  // step2: retrieve textview handle
  textView_out = GTK_TEXT_VIEW(gtk_builder_get_object(builder,
                                                      ACE_TEXT_ALWAYS_CHAR("textview")));
  ACE_ASSERT(textView_out);
  if (!textView_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_builder_get_object(\"textview\"): \"%m\", aborting\n")));

    g_object_unref(G_OBJECT(builder));
    builder = NULL;

    return;
  } // end IF

  // step3: connect signals/slots
//   gtk_builder_connect_signals(builder,
//                               &ACE_const_cast(cb_data&, userData_in));
  GtkButton* button = NULL;
  button = GTK_BUTTON(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(join_clicked_cb),
                   &ACE_const_cast(cb_data&, userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("part")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(part_clicked_cb),
                   &ACE_const_cast(cb_data&, userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("send")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(send_clicked_cb),
                   &ACE_const_cast(cb_data&, userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("register")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(register_clicked_cb),
                   &ACE_const_cast(cb_data&, userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("disconnect")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(disconnect_clicked_cb),
                   &ACE_const_cast(cb_data&, userData_in));
  button = GTK_BUTTON(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("quit")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(quit_activated_cb),
                   NULL);

  // step4: retrieve toplevel handle
  window = GTK_WIDGET(gtk_builder_get_object(builder,
                                             ACE_TEXT_ALWAYS_CHAR("dialog")));
  ACE_ASSERT(window);
  if (!window)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_builder_get_object(\"dialog\"): \"%m\", aborting\n")));

    g_object_unref(G_OBJECT(builder));
    builder = NULL;
    textView_out = NULL;

    return;
  } // end IF
  // connect default signals
  g_signal_connect(window,
                   ACE_TEXT_ALWAYS_CHAR("delete-event"),
                   G_CALLBACK(quit_activated_cb),
                   &window);
//   g_signal_connect(window,
//                    ACE_TEXT_ALWAYS_CHAR("destroy-event"),
//                    G_CALLBACK(quit_activated_cb),
//                    &window);
  g_signal_connect(window,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(gtk_widget_destroyed),
                   &window);

  // use correct screen
  if (parentWidget_in)
    gtk_window_set_screen(GTK_WINDOW(window),
                          gtk_widget_get_screen(ACE_const_cast(GtkWidget*,
                                                               parentWidget_in)));

  // step5: draw it
  gtk_widget_show_all(window);
}

void
do_work(const RPG_Net_Protocol_ConfigPOD& config_in,
        const std::string& serverHostname_in,
        const unsigned short& serverPortNumber_in,
        const bool& useThreadPool_in,
        const unsigned long& numThreadPoolThreads_in,
        const std::string& UIfile_in,
        cb_data& userData_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step0a: (if necessary) init the TP_Reactor
  if (useThreadPool_in)
  {
    if (!init_threadPool())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_threadPool(), aborting\n")));

      return;
    } // end IF
  } // end IF

  // step0b: init client connector
  IRC_Client_Connector connector(ACE_Reactor::instance(), // reactor
                                 ACE_NONBLOCK);           // flags: non-blocking I/O
//                                  0);                      // flags (*TODO*: ACE_NONBLOCK ?);

  // step1a: init connection manager
  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max(),
                                                                 config_in); // will be passed to all handlers

  // step1b: (try to) connect to the server
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

  // step2: setup UI
  GtkTextView* textView = NULL;
  do_builder(UIfile_in,   // glade file
             userData_in, // cb data
             NULL,        // there's no parent widget
             textView);   // return value: target view
  ACE_ASSERT(builder);
  IRC_Client_GUI_MessageHandler messageHandler(builder);
  userData_in.messageHandler = &messageHandler;

  // event loops:
  // - perform socket I/O --> ACE_Reactor
  // - UI events --> GTK main loop

//   // *NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);

  // step3: dispatch events...
  // *NOTE*: if we use a thread pool, we invoke a different function...
//   int grp_id = -1;
  if (useThreadPool_in)
  {
    // start a (group of) worker(s)...
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
  } // end IF
  else
  {
    // start a worker...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(1,                           // # threads
                                                     ::reactor_worker_func,       // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end ELSE
  if (grp_id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
               (useThreadPool_in ? numThreadPoolThreads_in : 1)));

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
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started group (ID: %u) of %u worker thread(s)...\n"),
             grp_id,
             (useThreadPool_in ? numThreadPoolThreads_in : 1)));

  // dispatch GTK events
  gtk_main();

//   // ... and wait for the reactor thread(s) to join
//   ACE_Thread_Manager::instance()->wait_grp(grp_id);
//
//   // step4: clean up
//   RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();
//   RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance()->waitConnections();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseConfigFile(const std::string& configFilename_in,
                   RPG_Net_Protocol_IRCLoginOptions& loginOptions_out,
                   std::string& serverHostname_out,
                   unsigned short& serverPortNumber_out)
{
  ACE_TRACE(ACE_TEXT("::do_parseConfigFile"));

  // init return value(s)
  if (loginOptions_out.user.hostname.discriminator == RPG_Net_Protocol_IRCLoginOptions::User::Hostname::STRING)
  {
    // clean up
    delete loginOptions_out.user.hostname.string;
    loginOptions_out.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::INVALID;
  } // end IF
  loginOptions_out.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::INVALID;

  loginOptions_out.password                    = RPG_NET_PROTOCOL_DEF_IRC_PASSWORD;
  loginOptions_out.nick                        = RPG_NET_PROTOCOL_DEF_IRC_NICK;
  loginOptions_out.user.username               = RPG_NET_PROTOCOL_DEF_IRC_USER;
  loginOptions_out.user.hostname.mode          = RPG_NET_PROTOCOL_DEF_IRC_MODE;
  loginOptions_out.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::BITMASK;
  loginOptions_out.user.servername             = RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME;
  loginOptions_out.user.realname               = RPG_NET_PROTOCOL_DEF_IRC_REALNAME;
  loginOptions_out.channel                     = RPG_NET_PROTOCOL_DEF_IRC_CHANNEL;

  serverHostname_out                           = IRC_CLIENT_DEF_SERVER_HOSTNAME;
  serverPortNumber_out                         = IRC_CLIENT_DEF_SERVER_PORT;

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

  // find/open "client" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               ACE_TEXT(IRC_CLIENT_CNF_CLIENT_SECTION_HEADER),
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               ACE_TEXT(IRC_CLIENT_CNF_CLIENT_SECTION_HEADER)));

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

//     // debug info
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

//     // debug info
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
  std::string configFile             = ACE_TEXT(""); // cannot assume this !
  bool debugParser                   = false;
  bool logToFile                     = false;
  bool traceInformation              = false;
  std::string UIfile                 = IRC_CLIENT_DEF_UI_FILE;
  bool printVersionAndExit           = false;
  bool useThreadPool                 = IRC_CLIENT_DEF_CLIENT_USES_TP;
  unsigned long numThreadPoolThreads = IRC_CLIENT_DEF_NUM_TP_THREADS;
  if (!(process_arguments(argc,
                          argv,
                          configFile,
                          debugParser,
                          logToFile,
                          traceInformation,
                          UIfile,
                          printVersionAndExit,
                          useThreadPool,
                          numThreadPoolThreads)))
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
  Stream_AllocatorHeap heapAllocator;
  RPG_Net_Protocol_MessageAllocator messageAllocator(RPG_NET_DEF_MAX_MESSAGES,
                                                     &heapAllocator);
  RPG_Net_Protocol_Module_IRCHandler_Module IRChandlerModule(std::string("IRCHandler"),
                                                             NULL);
  RPG_Net_Protocol_Module_IRCHandler* IRChandler_impl = NULL;
  IRChandler_impl = ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCHandler*,
                                     IRChandlerModule.writer());
  if (!IRChandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Protocol_Module_IRCHandler) failed, aborting\n")));

    return EXIT_FAILURE;
  } // end IF
  if (!IRChandler_impl->init(&messageAllocator,
                             RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE,
                             RPG_NET_DEF_CLIENT_PING_PONG, // auto-answer "ping" as a client ?...
                             true))                        // clients print ('.') dots for received "pings"...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               IRChandlerModule.name()));

    return EXIT_FAILURE;
  } // end IF

  userData.loginOptions.password = RPG_NET_PROTOCOL_DEF_IRC_PASSWORD;
  userData.loginOptions.nick = RPG_NET_PROTOCOL_DEF_IRC_NICK;
  userData.loginOptions.user.username = RPG_NET_PROTOCOL_DEF_IRC_USER;
  userData.loginOptions.user.hostname.mode = RPG_NET_PROTOCOL_DEF_IRC_MODE;
  userData.loginOptions.user.hostname.discriminator = RPG_Net_Protocol_IRCLoginOptions::User::Hostname::BITMASK;
  userData.loginOptions.user.servername = RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME;
  userData.loginOptions.user.realname = RPG_NET_PROTOCOL_DEF_IRC_REALNAME;
  userData.loginOptions.channel = RPG_NET_PROTOCOL_DEF_IRC_CHANNEL;
  userData.controller = IRChandler_impl;
  userData.messageHandler = NULL; // MUST be set in do_work !
  RPG_Net_Protocol_ConfigPOD config;
  // step1da: populate config object with default/collected data
  // ************ connection config data ************
  config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &messageAllocator;
  config.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
  // ************ protocol config data **************
  config.clientPingInterval = 0; // servers do this...
//   config.loginOptions = userData.loginOptions;
  // ************ stream config data ****************
  config.debugParser = debugParser;
  config.module = &IRChandlerModule;
  // *WARNING*: set at runtime, by the appropriate connection handler
  config.sessionID = 0; // (== socket handle !)
  config.statisticsReportingInterval = 0; // == off

  // step1db: parse config file (if any)
  std::string serverHostname      = IRC_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned short serverPortNumber = IRC_CLIENT_DEF_SERVER_PORT;
  if (!configFile.empty())
    do_parseConfigFile(configFile,
                       userData.loginOptions,
                       serverHostname,
                       serverPortNumber);
  config.loginOptions = userData.loginOptions;

  // step1e: init GTK
  gtk_init(&argc, &argv);

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work(config,
          serverHostname,
          serverPortNumber,
          useThreadPool,
          numThreadPoolThreads,
          UIfile,
          userData);
  timer.stop();

  // clean up
  IRChandlerModule.close();

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
