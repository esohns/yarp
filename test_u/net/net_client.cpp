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
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/Connector.h>
#include <ace/SOCK_Connector.h>
#include <ace/High_Res_Timer.h>

#include <gtk/gtk.h>
#include <glade/glade.h>

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice.h"

#include "rpg_common_macros.h"
#include "rpg_common.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_timer_manager.h"

#include "rpg_stream_allocatorheap.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_connection_manager.h"
#include "rpg_net_stream_messageallocator.h"

#include "rpg_net_client_defines.h"
#include "rpg_net_client_connector.h"
#include "rpg_net_client_asynchconnector.h"

#include "rpg_client_defines.h"
#include "rpg_client_GTK_manager.h"
#include "rpg_client_logger.h"
#include "rpg_client_ui_tools.h"

#include "rpg_net_server_defines.h"

#include "net_defines.h"
#include "net_common.h"
#include "net_callbacks.h"

#include "net_client_timeouthandler.h"
#include "net_client_signalhandler.h"

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-a           : alternating mode [") << false << "]" << std::endl;
  std::cout << ACE_TEXT("-c [VALUE]   : max #connections [") << NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS << "] {0 --> unlimited}" << std::endl;
  std::cout << ACE_TEXT("-h [STRING]  : server hostname [\"") << NET_CLIENT_DEF_SERVER_HOSTNAME << "\"]" << std::endl;
  std::cout << ACE_TEXT("-i [VALUE]   : connection interval (second(s)) [") << NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL << ACE_TEXT("] {0 --> OFF}") << std::endl;
  std::cout << ACE_TEXT("-l           : log to a file [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-p [VALUE]   : server port [") << RPG_NET_SERVER_DEF_LISTENING_PORT << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-r           : use reactor [") << RPG_NET_USES_REACTOR << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s           : server ping interval (millisecond(s)) [") << NET_CLIENT_DEF_SERVER_PING_INTERVAL << ACE_TEXT("] {0 --> OFF}") << std::endl;
  std::cout << ACE_TEXT("-t           : trace information [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-u [[STRING]]: UI file [\"") << NET_CLIENT_DEF_UI_FILE << "\"] {\"\" --> no GUI}" << std::endl;
  std::cout << ACE_TEXT("-v           : print version information and exit [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE]   : #dispatch threads [") << RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-y           : run stress-test [") << false << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
									bool& alternatingMode_out,
                  unsigned int& maxNumConnections_out,
                  std::string& serverHostname_out,
                  unsigned int& connectionInterval_out,
                  bool& logToFile_out,
                  unsigned short& serverPortNumber_out,
                  bool& useReactor_out,
                  unsigned int& serverPingInterval_out,
                  bool& traceInformation_out,
                  std::string& UIFile_out,
                  bool& printVersionAndExit_out,
									unsigned int& numDispatchThreads_out,
                  bool& runStressTest_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
	alternatingMode_out = false;
  maxNumConnections_out = NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  serverHostname_out = NET_CLIENT_DEF_SERVER_HOSTNAME;
  connectionInterval_out = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  logToFile_out = false;
  serverPortNumber_out = RPG_NET_SERVER_DEF_LISTENING_PORT;
  useReactor_out = RPG_NET_USES_REACTOR;
  serverPingInterval_out = NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  traceInformation_out = false;
  UIFile_out = NET_CLIENT_DEF_UI_FILE;
  printVersionAndExit_out = false;
	numDispatchThreads_out = RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
  runStressTest_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("ac:h:i:lp:rs:tu::vx:y"),
                             1,                          // skip command name
                             1,                          // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                             0);                         // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'a':
      {
        alternatingMode_out = true;

        break;
      }
      case 'c':
      {
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> maxNumConnections_out;

        break;
      }
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
        converter >> serverPingInterval_out;

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg();
        if (opt_arg)
          UIFile_out = ACE_TEXT_ALWAYS_CHAR(opt_arg);
        else
          UIFile_out.clear();

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
        converter >> numDispatchThreads_out;

        break;
      }
      case 'y':
      {
        runStressTest_out = true;

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
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  signals_inout.sig_add(SIGINT);         // 2       /* interrupt */
  signals_inout.sig_add(SIGILL);         // 4       /* illegal instruction - invalid function image */
  signals_inout.sig_add(SIGFPE);         // 8       /* floating point exception */
//  signals_inout.sig_add(SIGSEGV);        // 11      /* segment violation */
  signals_inout.sig_add(SIGTERM);        // 15      /* Software termination signal from kill */
  if (allowUserRuntimeConnect_in)
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
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_inout.sig_del(SIGKILL);         // 9       /* Kill signal */
  signals_inout.sig_del(SIGSTOP);         // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeConnect_in)
    signals_inout.sig_del(SIGUSR1);         // 10      /* User-defined signal 1 */
  // *NOTE* core dump on SIGSEGV
  signals_inout.sig_del(SIGSEGV);         // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_inout.sig_del(SIGPIPE);         // 12      /* Broken pipe: write to pipe with no readers */
#endif
}

bool
init_ui(const std::string& UIFile_in,
        Net_GTK_CBData_t& userData_out)
{
  RPG_TRACE(ACE_TEXT("::init_ui"));

  // init return value(s)
  userData_out.xml = NULL;

  // sanity check(s)
  if (UIFile_in.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid interface definition file, aborting\n")));

    return false;
  }

  // step1: load widget tree
  GDK_THREADS_ENTER();
  userData_out.xml = glade_xml_new(UIFile_in.c_str(), // definition file
                                   NULL,              // root widget --> construct all
                                   NULL);             // domain
  if (!userData_out.xml)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_new(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(UIFile_in.c_str())));

    // clean up
    GDK_THREADS_LEAVE();

    return false;
  } // end IF

  // step2: init dialog window(s)
  GtkWidget* dialog = GTK_WIDGET(glade_xml_get_widget(userData_out.xml,
                                                      ACE_TEXT_ALWAYS_CHAR(NET_UI_DIALOG_NAME)));
  ACE_ASSERT(dialog);
//  GtkWidget* image_icon = gtk_image_new_from_file(path.c_str());
//  ACE_ASSERT(image_icon);
//  gtk_window_set_icon(GTK_WINDOW(dialog),
//                      gtk_image_get_pixbuf(GTK_IMAGE(image_icon)));
  //GdkWindow* dialog_window = gtk_widget_get_window(dialog);
  //gtk_window_set_title(,
  //                     caption.c_str());

  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(userData_out.xml,
                                                            ACE_TEXT_ALWAYS_CHAR(NET_UI_ABOUTDIALOG_NAME)));
  ACE_ASSERT(about_dialog);

  // step3: init text view, setup auto-scrolling
  GtkTextBuffer* buffer = gtk_text_buffer_new(NULL); // text tag table --> create new
  ACE_ASSERT(buffer);
  GtkTextView* view = GTK_TEXT_VIEW(glade_xml_get_widget(userData_out.xml,
                                                         ACE_TEXT_ALWAYS_CHAR(NET_UI_TEXTVIEW_NAME)));
  ACE_ASSERT(view);
  gtk_text_view_set_buffer(view, buffer);
  PangoFontDescription* font_description =
      pango_font_description_from_string(ACE_TEXT_ALWAYS_CHAR(NET_UI_LOG_FONTDESCRIPTION));
  if (!font_description)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(NET_UI_LOG_FONTDESCRIPTION)));

    // clean up
    GDK_THREADS_LEAVE();

    return false;
  } // end IF
  // apply font
  GtkRcStyle* rc_style = gtk_rc_style_new();
  if (!rc_style)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_rc_style_new(): \"%m\", aborting\n")));

    // clean up
    GDK_THREADS_LEAVE();

    return false;
  } // end IF
  rc_style->font_desc = font_description;
  GdkColor base_colour, text_colour;
  gdk_color_parse(ACE_TEXT_ALWAYS_CHAR(NET_UI_LOG_BASE),
                  &base_colour);
  rc_style->base[GTK_STATE_NORMAL] = base_colour;
  gdk_color_parse(ACE_TEXT_ALWAYS_CHAR(NET_UI_LOG_TEXT),
                  &text_colour);
  rc_style->text[GTK_STATE_NORMAL] = text_colour;
  rc_style->color_flags[GTK_STATE_NORMAL] = static_cast<GtkRcFlags>(GTK_RC_BASE |
                                                                    GTK_RC_TEXT);
  gtk_widget_modify_style(GTK_WIDGET(view),
                          rc_style);
  gtk_rc_style_unref(rc_style);

//  GtkTextIter iterator;
//  gtk_text_buffer_get_end_iter(buffer,
//                               &iterator);
//  gtk_text_buffer_create_mark(buffer,
//                              ACE_TEXT_ALWAYS_CHAR(NET_UI_SCROLLMARK_NAME),
//                              &iterator,
//                              TRUE);
  g_object_unref(buffer);

  // schedule asynchronous updates of the log view area
  guint event_source_id = g_idle_add(update_display_cb,
                                     &userData_out);
  ACE_UNUSED_ARG(event_source_id);

  // step4a: connect default signals
  g_signal_connect(dialog,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(gtk_widget_destroyed),
                   NULL);

   // step4b: connect custom signals
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("button_connect_clicked_cb"),
                                G_CALLBACK(button_connect_clicked_cb),
                                &userData_out);
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("button_close_clicked_cb"),
                                G_CALLBACK(button_close_clicked_cb),
                                &userData_out);
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("button_close_all_clicked_cb"),
                                G_CALLBACK(button_close_all_clicked_cb),
                                &userData_out);
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("button_ping_clicked_cb"),
                                G_CALLBACK(button_ping_clicked_cb),
                                &userData_out);
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("button_about_clicked_cb"),
                                G_CALLBACK(button_about_clicked_cb),
                                &userData_out);
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("button_quit_clicked_cb"),
                                G_CALLBACK(button_quit_clicked_cb),
                                &userData_out);
  glade_xml_signal_connect_data(userData_out.xml,
                                ACE_TEXT_ALWAYS_CHAR("togglebutton_stress_toggled_cb"),
                                G_CALLBACK(togglebutton_stress_toggled_cb),
                                &userData_out);

//  // step5: auto-connect signals/slots
//  glade_xml_signal_autoconnect(userData_out.xml);

//   // step6: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(const_cast<GtkWidget*> (//parentWidget_in)));

  // step6: draw main dialog
  gtk_widget_show_all(dialog);

  // clean up
  GDK_THREADS_LEAVE();

  return true;
}

void
do_work(const Net_Client_TimeoutHandler::ActionMode_t& actionMode_in,
        const unsigned int& maxNumConnections_in,
        const std::string& serverHostname_in,
        const unsigned int& connectionInterval_in,
        const unsigned short& serverPortNumber_in,
        const bool& useReactor_in,
        const unsigned int& serverPingInterval_in,
        const std::string& UIFile_in,
        const unsigned int& numDispatchThreads_in,
        Net_GTK_CBData_t& GTKUserData_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0a: init ui ?
  if (!UIFile_in.empty() &&
      !init_ui(UIFile_in,
               GTKUserData_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init user interface, aborting\n")));

    return;
  } // end IF

  // step0b: init randomization
  try
  {
    RPG_Dice::init();
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Dice::init, returning\n")));

    return;
  }

  // step0c: init event dispatch
  if (!RPG_Net_Common_Tools::initEventDispatch(useReactor_in,
                                               numDispatchThreads_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init event dispatch, aborting\n")));

		return;
	} // end IF

  // step1: init client connector
  RPG_Net_Client_IConnector* connector = NULL;
  if (useReactor_in)
    ACE_NEW_NORETURN(connector, RPG_Net_Client_Connector());
  else
    ACE_NEW_NORETURN(connector, RPG_Net_Client_AsynchConnector());
  if (!connector)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return;
  } // end IF

  // step2a: init stream configuration object
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_StreamMessageAllocator messageAllocator(RPG_NET_MAX_MESSAGES,
                                                  &heapAllocator);
  RPG_Net_ConfigPOD config;
  ACE_OS::memset(&config, 0, sizeof(config));
  config.peerPingInterval = ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? 0
                                                                                         : serverPingInterval_in);
  config.pingAutoAnswer = true;
  config.printPingMessages = true;
  config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
  config.messageAllocator = &messageAllocator;
  config.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
  config.useThreadPerConnection = false;
  config.module = NULL; // just use the default stream...
  // *WARNING*: set at runtime, by the appropriate connection handler
  config.sessionID = 0; // (== socket handle !)
  config.statisticsReportingInterval = 0; // == off
	config.printFinalReport = false;

  // step2b: init connection manager
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max());
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->set(config); // will be passed to all handlers

  // step3: init action timer ?
  ACE_INET_Addr peer_address(serverPortNumber_in,
                             serverHostname_in.c_str(),
                             AF_INET);
	Net_Client_TimeoutHandler timeout_handler(actionMode_in,
		                                        ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? std::numeric_limits<unsigned int>::max()
                                                                                                         : maxNumConnections_in),
                                            peer_address,
                                            connector);
  GTKUserData_in.timeout_handler = &timeout_handler;
  GTKUserData_in.timer_id = -1;
  if (!UIFile_in.empty() &&
      (connectionInterval_in ||
       (actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS)))
  {
    // schedule action interval timer
    ACE_Time_Value interval(((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? (NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000)
                                                                                         : connectionInterval_in),
                            ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000)
                                                                                         : 0));
    GTKUserData_in.timer_id =
			RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&timeout_handler,                    // event handler handle
                                                              NULL,                                // ACT
                                                              RPG_COMMON_TIME_POLICY() + interval, // first wakeup time
                                                              interval);                           // interval
    if (GTKUserData_in.timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule action timer: \"%m\", aborting\n")));

      // clean up
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      delete connector;

      return;
    } // end IF
  } // end IF

  // step4: init signal handling
  Net_Client_SignalHandler signal_handler(GTKUserData_in.timer_id, // action timer id
//                                          gtk_timer_id,          // gtk timer id (if any)
                                          peer_address,            // remote SAP
                                          connector,               // connector
                                          useReactor_in);          // use reactor ?
  ACE_Sig_Set signal_set(0);
  init_signals((connectionInterval_in == 0), // allow SIGUSR1/SIGBREAK IFF
                                             // regular connections are off
               signal_set);
  RPG_Common_SignalActions_t previous_signal_actions;
  if (!RPG_Common_Tools::initSignals(signal_set,
                                     &signal_handler,
                                     useReactor_in,
                                     previous_signal_actions))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init signal handling, aborting\n")));

    // clean up
    if (GTKUserData_in.timer_id != -1)
    {
      const void* act = NULL;
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(GTKUserData_in.timer_id,
                                                                &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   GTKUserData_in.timer_id));
    } // end IF
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;

    return;
  } // end IF

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step5a: start GTK event loop ?
  if (!UIFile_in.empty())
  {
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->start();
    if (!RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->isRunning())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to start GTK event dispatch, aborting\n")));

      // clean up
      if (GTKUserData_in.timer_id != -1)
      {
        const void* act = NULL;
        if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(GTKUserData_in.timer_id,
                                                                  &act) <= 0)
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                     GTKUserData_in.timer_id));
      } // end IF
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      connector->abort();
      delete connector;
      RPG_Common_Tools::finiSignals(signal_set,
                                    useReactor_in,
                                    previous_signal_actions);

      return;
    } // end IF
  } // end IF

  // step5b: init worker(s)
  int group_id = -1;
  if (!RPG_Net_Common_Tools::startEventDispatch(useReactor_in,
                                                numDispatchThreads_in,
                                                group_id))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start event dispatch, aborting\n")));

    // clean up
    if (GTKUserData_in.timer_id != -1)
		{
			const void* act = NULL;
			if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(GTKUserData_in.timer_id,
				                                                        &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   GTKUserData_in.timer_id));
		} // end IF
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->stop();
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;
    RPG_Common_Tools::finiSignals(signal_set,
                                  useReactor_in,
                                  previous_signal_actions);

    return;
  } // end IF

  // step5c: connect immediately ?
  if (UIFile_in.empty() && (connectionInterval_in == 0))
    connector->connect(peer_address);

  // *NOTE*: from this point on, we need to clean up any remote connections !

  // step6: dispatch events
  // *NOTE*: when using a thread pool, handle things differently...
  if (numDispatchThreads_in > 1)
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

  // step7: clean up
  // *NOTE*: any action timer has been cancelled and connections have been
  // aborted by now...
  RPG_Common_Tools::finiSignals(signal_set,
                                useReactor_in,
                                previous_signal_actions);
  RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
  delete connector;

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
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
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
	Net_Client_TimeoutHandler::ActionMode_t actionMode = Net_Client_TimeoutHandler::ACTION_NORMAL;
  bool alternatingMode                               = false;
	unsigned int maxNumConnections                     = NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS;
  std::string serverHostname                         = NET_CLIENT_DEF_SERVER_HOSTNAME;
  unsigned int connectionInterval                    = NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL;
  bool logToFile                                     = false;
  unsigned short serverPortNumber                    = RPG_NET_SERVER_DEF_LISTENING_PORT;
  bool useReactor                                    = RPG_NET_USES_REACTOR;
  unsigned int serverPingInterval                    = NET_CLIENT_DEF_SERVER_PING_INTERVAL;
  bool traceInformation                              = false;
  std::string UIFile                                 = NET_CLIENT_DEF_UI_FILE;
  bool printVersionAndExit                           = false;
	unsigned int numDispatchThreads                    = RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS;
  bool runStressTest                                 = false;

  // step1b: parse/process/validate configuration
  if (!process_arguments(argc_in,
                         argv_in,
												 alternatingMode,
                         maxNumConnections,
                         serverHostname,
                         connectionInterval,
                         logToFile,
                         serverPortNumber,
                         useReactor,
                         serverPingInterval,
                         traceInformation,
                         UIFile,
                         printVersionAndExit,
												 numDispatchThreads,
                         runStressTest))
  {
    // make 'em learn...
    print_usage(ACE::basename(argv_in[0]));

    return EXIT_FAILURE;
  } // end IF

  // step1c: validate arguments
  if ((!UIFile.empty() && (alternatingMode || runStressTest))                     ||
      (runStressTest && ((serverPingInterval != 0) || (connectionInterval != 0))) ||
      (alternatingMode && runStressTest))
	{
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid arguments, aborting\n")));

		// make 'em learn...
		print_usage(ACE::basename(argv_in[0]));

		return EXIT_FAILURE;
	} // end IF
	if (!UIFile.empty() &&
			!RPG_Common_File_Tools::isReadable(UIFile))
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("invalid UI definition file (was: %s), aborting\n"),
							 ACE_TEXT(UIFile.c_str())));

    return EXIT_FAILURE;
  } // end IF
  if (numDispatchThreads == 0)
    numDispatchThreads = 1;

	if (alternatingMode)
		actionMode = Net_Client_TimeoutHandler::ACTION_ALTERNATING;
	if (runStressTest)
		actionMode = Net_Client_TimeoutHandler::ACTION_STRESS;

  Net_GTK_CBData_t gtk_cb_user_data;
  // step1d: initialize logging and/or tracing
  RPG_Client_Logger logger(&gtk_cb_user_data.log_stack,
                           &gtk_cb_user_data.lock);
  std::string log_file;
  if (logToFile)
    log_file = RPG_Common_File_Tools::getLogFilename(ACE::basename(argv_in[0]));
  if (!RPG_Common_Tools::initLogging(ACE::basename(argv_in[0]),   // program name
                                     log_file,                    // logfile
                                     false,                       // log to syslog ?
                                     false,                       // trace messages ?
                                     traceInformation,            // debug messages ?
                                     (UIFile.empty() ? NULL
                                                     : &logger))) // logger ?
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initLogging(), aborting\n")));

    return EXIT_SUCCESS;
  } // end IF

  // step1e: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(ACE::basename(argv_in[0]));

    return EXIT_SUCCESS;
  } // end IF

  // step2b: init GLIB / G(D|T)K[+] / GNOME ?
  if (!UIFile.empty())
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    g_thread_init(NULL);
#endif
    gdk_threads_init();
    if (!gtk_init_check(&argc_in,
                        &argv_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to gtk_init_check(): \"%m\", aborting\n")));

      return EXIT_FAILURE;
    } // end IF
    //   GnomeClient* gnomeSession = NULL;
    //   gnomeSession = gnome_client_new();
    //   ACE_ASSERT(gnomeSession);
    //   gnome_client_set_program(gnomeSession, ACE::basename(argv_in[0]));
    //  GnomeProgram* gnomeProgram = NULL;
    //  gnomeProgram = gnome_program_init(ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GNOME_APPLICATION_ID), // app ID
    //#ifdef HAVE_CONFIG_H
    ////                                     ACE_TEXT_ALWAYS_CHAR(VERSION),    // version
    //                                    ACE_TEXT_ALWAYS_CHAR(RPG_VERSION),   // version
    //#else
    //	                                NULL,
    //#endif
    //                                    LIBGNOMEUI_MODULE,                   // module info
    //                                    argc_in,                             // cmdline
    //                                    argv_in,                             // cmdline
    //                                    NULL);                               // property name(s)
    //  ACE_ASSERT(gnomeProgram);
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();
  // step2: do actual work
  do_work(actionMode,
		      maxNumConnections,
          serverHostname,
          connectionInterval,
          serverPortNumber,
          useReactor,
          serverPingInterval,
          UIFile,
          numDispatchThreads,
          gtk_cb_user_data);
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
  ACE_OS::memset(&elapsed_rusage, 0, sizeof(elapsed_rusage));
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
