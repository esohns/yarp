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

#include "net_callbacks.h"

#include <glade/glade.h>

#include "rpg_dice.h"

#include "rpg_common_macros.h"
#include "rpg_common_timer_manager.h"

#include "rpg_net_common.h"

#include "rpg_client_ui_tools.h"

#include "rpg_net_server_ilistener.h"

#include "net_defines.h"
#include "net_common.h"
#include "net_client_timeouthandler.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_update_log_display_cb(gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::idle_update_log_display_cb"));

  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);

  GtkTextView* view =
      GTK_TEXT_VIEW(glade_xml_get_widget(data->xml,
                                         ACE_TEXT_ALWAYS_CHAR(NET_UI_TEXTVIEW_NAME)));
  if (!view)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_UI_TEXTVIEW_NAME)));

    // clean up
//    gdk_threads_leave();

    return FALSE;
  } // end IF
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(view);
  if (!buffer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_text_view_get_buffer(%@): \"%m\", aborting\n"),
               view));

    // clean up
//    gdk_threads_leave();

    return FALSE;
  } // end IF

  GtkTextIter iterator;
  gtk_text_buffer_get_end_iter(buffer,
                               &iterator);

  gchar* converted_text = NULL;
  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(data->lock);

    // sanity check
    if (data->log_stack.empty())
    {
      // clean up
//      gdk_threads_leave();

      return TRUE; // nothing to do...
    } // end IF

    // step1: convert text
    converted_text = RPG_Client_UI_Tools::Locale2UTF8(data->log_stack.front());
    if (!converted_text)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert message text (was: \"%s\"), aborting\n"),
                 data->log_stack.front().c_str()));

      // clean up
//      gdk_threads_leave();

      return FALSE;
    } // end IF

    data->log_stack.pop_front();
  } // end lock scope

  // step2: display text
  gtk_text_buffer_insert(buffer,
                         &iterator,
                         converted_text,
                         -1);

  // clean up
  g_free(converted_text);
  converted_text = NULL;

  // step3: scroll the view accordingly
  // move the iterator to the beginning of line, so it doesn't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset(&iterator, 0);

  // ...and place the mark at iter. The mark will stay there after insertion
  // because it has "right" gravity
  GtkTextMark* mark =
      gtk_text_buffer_get_mark(buffer,
                               ACE_TEXT_ALWAYS_CHAR(NET_UI_SCROLLMARK_NAME));
//  gtk_text_buffer_move_mark(buffer,
//                            mark,
//                            &iterator);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen(view,
                                     mark);

  return TRUE;
}

G_MODULE_EXPORT gboolean
idle_update_info_display_cb(gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::idle_update_info_display_cb"));

  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);

  GtkSpinButton* spinbutton =
      GTK_SPIN_BUTTON(glade_xml_get_widget(data->xml,
                                          ACE_TEXT_ALWAYS_CHAR(NET_UI_NUMCONNECTIONS_NAME)));
  if (!spinbutton)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_UI_NUMCONNECTIONS_NAME)));

    return FALSE;
  } // end IF

  { // synch access
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(data->lock);
    for (Net_GTK_EventsIterator_t iterator = data->event_stack.begin();
         iterator != data->event_stack.end();
         iterator++)
    {
      switch (*iterator)
      {
        case NET_GTKEVENT_CONNECT:
        {
          // update info label
          gtk_spin_button_spin(spinbutton,
                               GTK_SPIN_STEP_FORWARD,
                               1.0);

          break;
        }
        case NET_GTKEVENT_DISCONNECT:
        {
          // update info label
          gtk_spin_button_spin(spinbutton,
                               GTK_SPIN_STEP_BACKWARD,
                               1.0);

          break;
        }
        case NET_GTKEVENT_STATISTICS:
        {
          // *TODO*
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid/unknown event type (was: %d), continuing\n"),
                     *iterator));

          break;
        }
      } // end SWITCH
    } // end FOR

    data->event_stack.clear();
  } // end lock scope

  // enable buttons ?
  gint current_value = gtk_spin_button_get_value_as_int(spinbutton);
  GtkWidget* widget = NULL;
  if (data->listener_handle == NULL) // <-- client
  {
    widget =
        GTK_WIDGET(glade_xml_get_widget(data->xml,
                                        ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEBUTTON_NAME)));
    if (!widget)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEBUTTON_NAME)));
    else
      gtk_widget_set_sensitive(widget,
                               (current_value > 0));
  } // end IF
  widget =
      GTK_WIDGET(glade_xml_get_widget(data->xml,
                                      ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEALLBUTTON_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_CLOSEALLBUTTON_NAME)));
  else
    gtk_widget_set_sensitive(widget, (current_value > 0));
  if (data->listener_handle == NULL) // <-- client
  {
    widget = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                             ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
    if (!widget)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT_ALWAYS_CHAR(NET_CLIENT_UI_PINGBUTTON_NAME)));
    else
      gtk_widget_set_sensitive(widget, (current_value > 0));
  } // end IF

  return TRUE;
}

// -----------------------------------------------------------------------------

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

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
  int signal = 0;
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  signal = SIGUSR1;
#else
  signal = SIGBREAK;
#endif
  if (ACE_OS::raise(signal) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
               signal));

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

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
  int signal = 0;
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  signal = SIGUSR2;
#else
  signal = SIGTERM;
#endif
  unsigned int num_connections =
      RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->numConnections();
  if (num_connections > 0)
    if (ACE_OS::raise(signal) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                 signal));

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
} // button_ping_clicked_cb

G_MODULE_EXPORT gint
togglebutton_stress_toggled_cb(GtkWidget* widget_in,
															 gpointer act_in)
{
	RPG_TRACE(ACE_TEXT("::togglebutton_stress_toggled_cb"));

  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(data->xml);
  ACE_ASSERT(data->timeout_handler);

  // schedule action interval timer ?
  if (data->timer_id == -1)
  {
    ACE_Time_Value interval((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000),
                            ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000));
    data->timer_id =
        RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(data->timeout_handler,               // event handler handle
                                                                NULL,                                // ACT
                                                                RPG_COMMON_TIME_POLICY() + interval, // first wakeup time
                                                                interval);                           // interval
    if (data->timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule action timer: \"%m\", aborting\n")));

      return FALSE;
    } // end IF
  } // end IF
  else
  {
    const void* act = NULL;
    if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(data->timer_id,
                                                              &act) <= 0)
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                 data->timer_id));

    // clean up
    data->timer_id = -1;
  } // end ELSE

  // toggle buttons
  GtkWidget* widget =
      GTK_WIDGET(glade_xml_get_widget(data->xml,
                                      ACE_TEXT_ALWAYS_CHAR(NET_UI_ACTIONS_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_UI_ACTIONS_NAME)));
  else
    gtk_widget_set_sensitive(widget,
                             !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget_in)));

  return FALSE;
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
togglebutton_listen_toggled_cb(GtkWidget* widget_in,
                               gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::togglebutton_listen_toggled_cb"));

  Net_GTK_CBData_t* data = static_cast<Net_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);
  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(data->xml);
  ACE_ASSERT(data->listener_handle);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget_in)))
  {
    try
    {
      data->listener_handle->start();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_Server_IListener::start(): \"%m\", continuing\n")));
    } // end catch
  } // end IF
  else
  {
    try
    {
      data->listener_handle->stop();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_Server_IListener::stop(): \"%m\", continuing\n")));
    } // end catch
  } // end IF

  return FALSE;
} // togglebutton_listen_toggled_cb

G_MODULE_EXPORT gint
button_report_clicked_cb(GtkWidget* widget_in,
                         gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::button_report_clicked_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(act_in);

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) instead...
  int signal = 0;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signal = SIGUSR1;
#else
  signal = SIGBREAK;
#endif
  if (ACE_OS::raise(signal) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
               signal));

  return FALSE;
}

// -----------------------------------------------------------------------------

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
                                                            ACE_TEXT_ALWAYS_CHAR(NET_UI_ABOUTDIALOG_NAME)));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_UI_ABOUTDIALOG_NAME)));

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

#ifdef __cplusplus
}
#endif /* __cplusplus */
