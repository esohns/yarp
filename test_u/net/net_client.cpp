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

#include "rpg_net_server_defines.h"

#include "net_defines.h"
#include "net_common.h"
#include "net_client_timeouthandler.h"
#include "net_client_signalhandler.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint
button_connect_clicked_cb(GtkWidget* widget_in,
                          gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_connect_clicked_cb"));

  ACE_UNUSED_ARG(widget_in);
  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);

  if (ACE_OS::raise(SIGUSR1) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
               SIGUSR1));

  // enable buttons
  GtkWidget* widget =
      GTK_WIDGET(glade_xml_get_widget(data->xml,
                                      ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEBUTTON_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEBUTTON_NAME)));
  else
    gtk_widget_set_sensitive(widget, TRUE);
  widget =
      GTK_WIDGET(glade_xml_get_widget(data->xml,
                                      ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEALLBUTTON_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEALLBUTTON_NAME)));
  else
    gtk_widget_set_sensitive(widget, TRUE);
  widget = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
  else
    gtk_widget_set_sensitive(widget, TRUE);

  return FALSE;
} // button_connect_clicked_cb

G_MODULE_EXPORT gint
button_close_clicked_cb(GtkWidget* widget_in,
                        gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_close_clicked_cb"));

  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);

  unsigned int num_connections =
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->numConnections();
  if (num_connections > 0)
    if (ACE_OS::raise(SIGUSR2) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                 SIGUSR2));

  // disable buttons ?
  if (num_connections <= 1)
  {
    gtk_widget_set_sensitive(widget_in, FALSE);
    GtkWidget* widget =
        GTK_WIDGET(glade_xml_get_widget(data->xml,
                                        ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEALLBUTTON_NAME)));
    if (!widget)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEALLBUTTON_NAME)));
    else
      gtk_widget_set_sensitive(widget, FALSE);
    widget = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                             ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
    if (!widget)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
    else
      gtk_widget_set_sensitive(widget, FALSE);
  } // end IF

  return FALSE;
} // button_close_clicked_cb

G_MODULE_EXPORT gint
button_close_all_clicked_cb(GtkWidget* widget_in,
                            gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_close_all_clicked_cb"));

  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);

  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->abortConnections();

  // disable buttons
  GtkWidget* widget =
      GTK_WIDGET(glade_xml_get_widget(data->xml,
                                      ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEBUTTON_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEBUTTON_NAME)));
  else
    gtk_widget_set_sensitive(widget, FALSE);
  gtk_widget_set_sensitive(widget_in, FALSE);
  widget = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
  else
    gtk_widget_set_sensitive(widget, FALSE);

  return FALSE;
} // button_close_all_clicked_cb

G_MODULE_EXPORT gint
button_ping_clicked_cb(GtkWidget* widget_in,
                       gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_ping_clicked_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(act_in);

	// sanity check
	unsigned int num_connections =
		RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->numConnections();
	if (num_connections == 0)
		return FALSE;

	// grab a (random) connection handler
	// *WARNING*: there's a race condition here...
	RPG_Dice_RollResult_t result;
	RPG_Dice::generateRandomNumbers(num_connections,
																	1,
																	result);
	const RPG_Net_Connection_Manager_t::CONNECTION_TYPE* connection_handler =
		RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->operator [](result.front() - 1);
	if (!connection_handler)
	{
//				ACE_DEBUG((LM_ERROR,
//									 ACE_TEXT("failed to retrieve connection handler, continuing")));

		return FALSE;
	} // end IF

	try
	{
		const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->ping();
	}
	catch (...)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("caught exception in RPG_Net_IConnection::ping(), aborting\n")));

		// clean up
		const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->decrease();

		return FALSE;
	}

	// clean up
	const_cast<RPG_Net_Connection_Manager_t::CONNECTION_TYPE*>(connection_handler)->decrease();

	return FALSE;
} // button_close_clicked_cb

G_MODULE_EXPORT gint
button_about_clicked_cb(GtkWidget* widget_in,
                        gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_about_clicked_cb"));

  ACE_UNUSED_ARG(widget_in);
  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);

  // retrieve about dialog handle
  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                                            ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_ABOUTDIALOG_NAME)));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_ABOUTDIALOG_NAME)));

    return TRUE; // propagate
  } // end IF

  // run dialog
  gint result = gtk_dialog_run(GTK_DIALOG(about_dialog));
  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    default:
      break;
  } // end SWITCH

  // clean up
  gtk_widget_hide(about_dialog);

  return FALSE;
} // button_about_clicked_cb

G_MODULE_EXPORT gint
button_quit_clicked_cb(GtkWidget* widget_in,
                       gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_quit_clicked_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(act_in);
//  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
//  ACE_ASSERT(data);

//  ACE_Guard<ACE_Thread_Mutex> aGuard(data->lock);

//  data->gtk_main_quit_invoked = true;

  if (ACE_OS::raise(SIGINT) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
               SIGINT));

  return FALSE;
} // button_quit_clicked_cb

//   gboolean gtk_quit_handler_cb(gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-a         : alternating mode [") << false << "]" << std::endl;
  std::cout << ACE_TEXT("-c [VALUE] : max #connections [") << NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS << "] {0 --> unlimited}" << std::endl;
  std::cout << ACE_TEXT("-h [STRING]: server hostname [\"") << NET_CLIENT_DEF_SERVER_HOSTNAME << "\"]" << std::endl;
  std::cout << ACE_TEXT("-i [VALUE] : connection interval (second(s)) [") << NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL << ACE_TEXT("] {0 --> OFF}") << std::endl;
  std::cout << ACE_TEXT("-l         : log to a file [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-p [VALUE] : server port [") << RPG_NET_SERVER_DEF_LISTENING_PORT << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-r         : use reactor [") << RPG_NET_USES_REACTOR << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s         : server ping interval (millisecond(s)) [") << NET_CLIENT_DEF_SERVER_PING_INTERVAL << ACE_TEXT("] {0 --> OFF}") << std::endl;
  std::cout << ACE_TEXT("-t         : trace information [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-u [STRING]: UI file [\"") << NET_CLIENT_DEF_UI_FILE << "\"] {"" --> no GUI}" << std::endl;
  std::cout << ACE_TEXT("-v         : print version information and exit [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x [VALUE] : #dispatch threads [") << RPG_NET_CLIENT_DEF_NUM_DISPATCH_THREADS << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-y         : run stress-test [") << false << ACE_TEXT("]") << std::endl;
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
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_inout.sig_del(SIGKILL);         // 9       /* Kill signal */
  signals_inout.sig_del(SIGSTOP);         // 19      /* Stop process */
	// ---------------------------------------------------------------------------
  // *NOTE* don't care about SIGPIPE
  signals_inout.sig_del(SIGPIPE);         // 12      /* Broken pipe: write to pipe with no readers */
#endif
}

void
init_signalHandling(ACE_Sig_Set& signals_in,
                    Net_Client_SignalHandler& eventHandler_in,
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
    if (ACE_OS::thr_sigsetmask(SIG_BLOCK,
                               &signal_set,
                               NULL) == -1)
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
    if (ACE_OS::thr_sigsetmask(SIG_UNBLOCK,
                               &signal_set,
                               NULL) == -1)
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
                                                      ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_DIALOG_NAME)));
  ACE_ASSERT(dialog);
//  GtkWidget* image_icon = gtk_image_new_from_file(path.c_str());
//  ACE_ASSERT(image_icon);
//  gtk_window_set_icon(GTK_WINDOW(dialog),
//                      gtk_image_get_pixbuf(GTK_IMAGE(image_icon)));
  //GdkWindow* dialog_window = gtk_widget_get_window(dialog);
  //gtk_window_set_title(,
  //                     caption.c_str());

  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(userData_out.xml,
                                                            ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_ABOUTDIALOG_NAME)));
  ACE_ASSERT(about_dialog);

  // step3a: connect default signals
  g_signal_connect(dialog,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(gtk_widget_destroyed),
                   NULL);

   // step3b: connect custom signals
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

  GtkTextBuffer* buffer = gtk_text_buffer_new(NULL); // text tag table --> create new
  ACE_ASSERT(buffer);
  GtkTextView* view = GTK_TEXT_VIEW(glade_xml_get_widget(userData_out.xml,
                                                         ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_TEXTVIEW_NAME)));
  ACE_ASSERT(view);
  gtk_text_view_set_buffer(view, buffer);
  g_object_unref(buffer);

//  // step5: auto-connect signals/slots
//  glade_xml_signal_autoconnect(userData_out.xml);

//   // step6: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(const_cast<GtkWidget*> (//parentWidget_in)));

  // step6: draw main dialog
  gtk_widget_show_all(dialog);
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
				const unsigned int& numDispatchThreads_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0a: init ui ?
  Net_GTK_CBData_t user_data;
  if (!UIFile_in.empty() &&
      !init_ui(UIFile_in,
               user_data))
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

  // step3: init timer(s)...
  ACE_INET_Addr peer_address(serverPortNumber_in,
                             serverHostname_in.c_str(),
                             AF_INET);
	Net_Client_TimeoutHandler timeout_handler(actionMode_in,
		                                        ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? std::numeric_limits<unsigned int>::max()
                                                                                                         : maxNumConnections_in),
                                            peer_address,
                                            connector);
  long action_timer_id = -1;
  if (connectionInterval_in ||
		  (actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS))
  {
    // schedule action interval timer
    ACE_Time_Value interval(((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? (NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000)
                                                                                         : connectionInterval_in),
                            ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000)
                                                                                         : 0));
    action_timer_id =
			RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&timeout_handler,                    // event handler handle
                                                              NULL,                                // ACT
                                                              RPG_COMMON_TIME_POLICY() + interval, // first wakeup time
                                                              interval);                           // interval
    if (action_timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule action timer: \"%m\", aborting\n")));

      // clean up
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      delete connector;

      return;
    } // end IF
  } // end IF
//  long gtk_timer_id = -1;
//  if (!UIFile_in.empty())
//  {
//    // schedule gtk dispatch interval timer
//    // RPG_CLIENT_SDL_GTKEVENT_RESOLUTION
//    ACE_Time_Value interval(0,                                           // second(s)
//                            (NET_CLIENT_UI_GTKEVENT_RESOLUTION * 1000)); // usecond(s)
//    gtk_timer_id =
//      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(&timeout_handler,                    // event handler handle
//                                                              &user_data,                          // ACT
//                                                              RPG_COMMON_TIME_POLICY() + interval, // first wakeup time
//                                                              interval);                           // interval
//    if (gtk_timer_id == -1)
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("failed to schedule gtk dispatch timer: \"%m\", aborting\n")));

//      // clean up
//      if (connectionInterval_in)
//      {
//        const void* act = NULL;
//        if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(action_timer_id,
//                                                                  &act) <= 0)
//          ACE_DEBUG((LM_DEBUG,
//                     ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
//                     action_timer_id));
//      } // end IF
//      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
//      delete connector;

//      return;
//    } // end IF
//  } // end IF

  // step4: init signal handling
  Net_Client_SignalHandler signal_handler(action_timer_id, // action timer id
//                                          gtk_timer_id,    // gtk timer id (if any)
                                          peer_address,    // remote SAP
                                          connector,       // connector
                                          useReactor_in);  // use reactor ?
  ACE_Sig_Set signal_set(0);
  init_signals((connectionInterval_in == 0),  // allow SIGUSR1/SIGBREAK IF regular connections are off
               signal_set);
  init_signalHandling(signal_set,
                      signal_handler,
                      useReactor_in);

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
      if (connectionInterval_in)
      {
        const void* act = NULL;
        if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(action_timer_id,
                                                                  &act) <= 0)
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                     action_timer_id));
      } // end IF
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      connector->abort();
      delete connector;
      fini_signalHandling(signal_set,
                          useReactor_in);

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
    if (connectionInterval_in)
		{
			const void* act = NULL;
			if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(action_timer_id,
				                                                        &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   action_timer_id));
		} // end IF
//		if (!UIFile_in.empty())
//		{
//			const void* act = NULL;
//			if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(gtk_timer_id,
//																																&act) <= 0)
//				ACE_DEBUG((LM_DEBUG,
//									 ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
//									 gtk_timer_id));
//		} // end IF
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->stop();
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;
    fini_signalHandling(signal_set,
                        useReactor_in);

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
  // *NOTE*: action/gtk timer(s) have been cancelled, and connections have been aborted
  fini_signalHandling(signal_set,
                      useReactor_in);
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
    print_usage(std::string(ACE::basename(argv_in[0])));

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
		print_usage(std::string(ACE::basename(argv_in[0])));

		return EXIT_FAILURE;
	} // end IF
	if (!UIFile.empty() && !RPG_Common_File_Tools::isReadable(UIFile))
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("invalid ui file (was: %s), aborting\n"),
							 ACE_TEXT(UIFile.c_str())));

    return EXIT_FAILURE;
  } // end IF
  if (serverPortNumber <= 1023)
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("using (privileged) port #: %d...\n"),
               serverPortNumber));

	if (alternatingMode)
		actionMode = Net_Client_TimeoutHandler::ACTION_ALTERNATING;
	if (runStressTest)
		actionMode = Net_Client_TimeoutHandler::ACTION_STRESS;

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
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step2b: init GLIB / G(D|T)K[+] / GNOME
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
					numDispatchThreads);
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
