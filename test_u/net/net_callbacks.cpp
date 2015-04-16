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

#include "glade/glade.h"

#include "common_timer_manager.h"

#include "common_ui_common.h"
#include "common_ui_defines.h"

#include "net_client_timeouthandler.h"
#include "net_common.h"
#include "net_connection_manager_common.h"
#include "net_defines.h"

#include "rpg_dice.h"

#include "rpg_common_macros.h"

#include "rpg_client_defines.h"
#include "rpg_client_ui_tools.h"

#include "rpg_net_server_ilistener.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_initialize_client_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_initialize_client_UI_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // step1: init dialog window(s)
  GtkWidget* dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog);
  //  GtkWidget* image_icon = gtk_image_new_from_file(path.c_str());
  //  ACE_ASSERT(image_icon);
  //  gtk_window_set_icon(GTK_WINDOW(dialog),
  //                      gtk_image_get_pixbuf(GTK_IMAGE(image_icon)));
  //GdkWindow* dialog_window = gtk_widget_get_window(dialog);
  //gtk_window_set_title(,
  //                     caption.c_str());

  GtkWidget* about_dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog);

  // step2: init info view
  GtkSpinButton* spinbutton =
    GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  ACE_ASSERT (spinbutton);
  gtk_spin_button_set_range (spinbutton,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  //  gtk_entry_set_editable(GTK_ENTRY(spinbutton),
  //                         FALSE);

  // step4: init text view, setup auto-scrolling
  GtkTextBuffer* buffer =
   gtk_text_buffer_new (NULL); // text tag table --> create new
  ACE_ASSERT (buffer);
  GtkTextView* view =
    GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view);
  gtk_text_view_set_buffer (view, buffer);
  //PangoFontDescription* font_description =
  //  pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_FONTDESCRIPTION));
  //if (!font_description)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
  //              ACE_TEXT (NET_UI_LOG_FONTDESCRIPTION)));
  //  return FALSE; // G_SOURCE_REMOVE
  //} // end IF
  //// apply font
  //GtkRcStyle* rc_style = gtk_rc_style_new ();
  //if (!rc_style)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
  //  return FALSE; // G_SOURCE_REMOVE
  //} // end IF
  //rc_style->font_desc = font_description;
  //GdkColor base_colour, text_colour;
  //gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_BASE),
  //                 &base_colour);
  //rc_style->base[GTK_STATE_NORMAL] = base_colour;
  //gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_TEXT),
  //                 &text_colour);
  //rc_style->text[GTK_STATE_NORMAL] = text_colour;
  //rc_style->color_flags[GTK_STATE_NORMAL] =
  //  static_cast<GtkRcFlags> (GTK_RC_BASE |
  //                           GTK_RC_TEXT);
  //gtk_widget_modify_style (GTK_WIDGET (view),
  //                         rc_style);
  //gtk_rc_style_unref (rc_style);

  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter(buffer,
  //                               &iterator);
  //  gtk_text_buffer_create_mark(buffer,
  //                              ACE_TEXT_ALWAYS_CHAR(NET_UI_SCROLLMARK_NAME),
  //                              &iterator,
  //                              TRUE);
  g_object_unref (buffer);

  // schedule asynchronous updates of the log view
  guint event_source_id = g_timeout_add_seconds (1,
                                                 idle_update_log_display_cb,
                                                 userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE
  // schedule asynchronous updates of the info view
  event_source_id = g_timeout_add (NET_UI_GTKEVENT_RESOLUTION,
                                   idle_update_info_display_cb,
                                   userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE

  // step5: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_autoconnect doesn't work reliably
  //glade_xml_signal_autoconnect(userData_out.xml);

  // step5a: connect default signals
  g_signal_connect (dialog,
                    ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (gtk_widget_destroyed),
                    NULL);

  // step5b: connect custom signals
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("button_connect_clicked_cb"),
                                 G_CALLBACK (button_connect_clicked_cb),
                                 userData_in);
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("button_close_clicked_cb"),
                                 G_CALLBACK (button_close_clicked_cb),
                                 userData_in);
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("button_close_all_clicked_cb"),
                                 G_CALLBACK (button_close_all_clicked_cb),
                                 userData_in);
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("button_ping_clicked_cb"),
                                 G_CALLBACK (button_ping_clicked_cb),
                                 userData_in);
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("button_about_clicked_cb"),
                                 G_CALLBACK (button_about_clicked_cb),
                                 userData_in);
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("button_quit_clicked_cb"),
                                 G_CALLBACK (button_quit_clicked_cb),
                                 userData_in);
  glade_xml_signal_connect_data ((*iterator).second.second,
                                 ACE_TEXT_ALWAYS_CHAR ("togglebutton_stress_toggled_cb"),
                                 G_CALLBACK (togglebutton_stress_toggled_cb),
                                 userData_in);

  //   // step6: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen(GTK_WINDOW(dialog),
  //                           gtk_widget_get_screen(const_cast<GtkWidget*> (//parentWidget_in)));

  // step7: draw main dialog
  gtk_widget_show_all (dialog);

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_initialize_server_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_initialize_server_UI_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // step2: init dialog window(s)
  GtkWidget* dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog);
  //  GtkWidget* image_icon = gtk_image_new_from_file(path.c_str());
  //  ACE_ASSERT(image_icon);
  //  gtk_window_set_icon(GTK_WINDOW(dialog),
  //                      gtk_image_get_pixbuf(GTK_IMAGE(image_icon)));
  //GdkWindow* dialog_window = gtk_widget_get_window(dialog);
  //gtk_window_set_title(,
  //                     caption.c_str());

  GtkWidget* about_dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog);

  // step3: init info view
  GtkSpinButton* spinbutton =
    GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  ACE_ASSERT (spinbutton);
  gtk_spin_button_set_range (spinbutton,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  //  gtk_entry_set_editable(GTK_ENTRY(spinbutton),
  //                         FALSE);

  // step4: init text view, setup auto-scrolling
  GtkTextBuffer* buffer = gtk_text_buffer_new (NULL); // text tag table --> create new
  ACE_ASSERT (buffer);
  GtkTextView* view =
    GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view);
  gtk_text_view_set_buffer (view,
                            buffer);
  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter(buffer,
  //                               &iterator);
  //  gtk_text_buffer_create_mark(buffer,
  //                              ACE_TEXT_ALWAYS_CHAR(NET_UI_SCROLLMARK_NAME),
  //                              &iterator,
  //                              TRUE);
  g_object_unref (buffer);
  //PangoFontDescription* font_description =
  //  pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_FONTDESCRIPTION));
  //if (!font_description)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
  //              ACE_TEXT (NET_UI_LOG_FONTDESCRIPTION)));
  //  return FALSE; // G_SOURCE_REMOVE
  //} // end IF
  //// apply font
  //GtkRcStyle* rc_style = gtk_rc_style_new ();
  //if (!rc_style)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
  //  return FALSE; // G_SOURCE_REMOVE
  //} // end IF
  //rc_style->font_desc = font_description;
  //GdkColor base_colour, text_colour;
  //gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_BASE),
  //                 &base_colour);
  //rc_style->base[GTK_STATE_NORMAL] = base_colour;
  //gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_TEXT),
  //                 &text_colour);
  //rc_style->text[GTK_STATE_NORMAL] = text_colour;
  //rc_style->color_flags[GTK_STATE_NORMAL] =
  //  static_cast<GtkRcFlags>(GTK_RC_BASE |
  //  GTK_RC_TEXT);
  //gtk_widget_modify_style (GTK_WIDGET (view),
  //                         rc_style);
  //gtk_rc_style_unref (rc_style);

  // schedule asynchronous updates of the log view
  guint event_source_id = g_timeout_add_seconds (1,
                                                 idle_update_log_display_cb,
                                                 userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE
  // schedule asynchronous updates of the info view
  event_source_id = g_timeout_add (NET_UI_GTKEVENT_RESOLUTION,
                                   idle_update_info_display_cb,
                                   userData_in);
  if (event_source_id > 0)
    data_p->GTKState.eventSourceIds.push_back (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE

  // step5: disable some functions ?
  GtkButton* button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button),
                            data_p->allowUserRuntimeStatistic);

  // step6: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_connect_data does not work reliably
  //glade_xml_signal_autoconnect(userData_out.xml);

  // step6a: connect default signals
  bool success = true;
  success = (success &&
             (g_signal_connect (dialog,
             ACE_TEXT_ALWAYS_CHAR ("destroy"),
             G_CALLBACK (gtk_widget_destroyed),
             &dialog) > 0));

  // step6b: connect custom signals
  // *NOTE*: glade_xml_signal_connect_data does not work reliably on Windows
  //glade_xml_signal_connect_data(userData_out.xml,
  //                              ACE_TEXT_ALWAYS_CHAR("togglebutton_listen_toggled_cb"),
  //                              G_CALLBACK(togglebutton_listen_toggled_cb),
  //                              &userData_out);
  GtkToggleButton* togglebutton =
    GTK_TOGGLE_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_GTK_BUTTON_LISTEN_NAME)));
  ACE_ASSERT (togglebutton);
  success = (success &&
             (g_signal_connect (togglebutton,
                                ACE_TEXT_ALWAYS_CHAR ("toggled"),
                                G_CALLBACK (togglebutton_listen_toggled_cb),
                                userData_in) > 0));
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  ACE_ASSERT (button);
  success = (success &&
             (g_signal_connect (button,
                                ACE_TEXT_ALWAYS_CHAR ("clicked"),
                                G_CALLBACK (button_close_all_clicked_cb),
                                userData_in) > 0));
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
  ACE_ASSERT (button);
  success = (success &&
             (g_signal_connect (button,
                                ACE_TEXT_ALWAYS_CHAR ("clicked"),
                                G_CALLBACK (button_report_clicked_cb),
                                userData_in) > 0));
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_ABOUT_NAME)));
  ACE_ASSERT (button);
  success = (success &&
             (g_signal_connect (button,
                                ACE_TEXT_ALWAYS_CHAR ("clicked"),
                                G_CALLBACK (button_about_clicked_cb),
                                userData_in) > 0));
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
  ACE_ASSERT (button);
  success = (success &&
             (g_signal_connect (button,
                                ACE_TEXT_ALWAYS_CHAR ("clicked"),
                                G_CALLBACK (button_quit_clicked_cb),
                                userData_in) > 0));
  if (!success)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_signal_connect(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF

  //   // step7: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen(GTK_WINDOW(dialog),
  //                           gtk_widget_get_screen(const_cast<GtkWidget*>(//parentWidget_in)));

  // step8: draw main dialog
  gtk_widget_show_all (dialog);

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean idle_finalize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_update_log_display_cb (gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::idle_update_log_display_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  GtkTextView* view =
    GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  if (!view)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  GtkTextBuffer* buffer = gtk_text_view_get_buffer (view);
  if (!buffer)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_text_view_get_buffer(%@): \"%m\", aborting\n"),
                view));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF

  GtkTextIter iterator_2;
  gtk_text_buffer_get_end_iter (buffer,
                                &iterator_2);

  gchar* converted_text = NULL;
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (data_p->GTKState.lock);

    // sanity check
    if (data_p->logStack.empty ())
      return TRUE; // G_SOURCE_CONTINUE

    // step1: convert text
    for (RPG_Client_MessageStackConstIterator_t iterator_3 = data_p->logStack.begin ();
         iterator_3 != data_p->logStack.end ();
         iterator_3++)
    {
      converted_text = RPG_Client_UI_Tools::Locale2UTF8(*iterator_3);
      if (!converted_text)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to convert message text (was: \"%s\"), aborting\n"),
                   ACE_TEXT((*iterator_3).c_str())));
        return FALSE; // G_SOURCE_REMOVE
      } // end IF

      // step2: display text
      gtk_text_buffer_insert(buffer,
                             &iterator_2,
                             converted_text,
                             -1);

      // clean up
      g_free(converted_text);
      converted_text = NULL;
    } // end FOR

    data_p->logStack.clear ();
  } // end lock scope

  // step3: scroll the view accordingly
  // move the iterator to the beginning of line, so it doesn't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset(&iterator_2, 0);

  // ...and place the mark at iter. The mark will stay there after insertion
  // because it has "right" gravity
  GtkTextMark* mark =
      gtk_text_buffer_get_mark(buffer,
                               ACE_TEXT_ALWAYS_CHAR(NET_UI_GTK_SCROLLMARK_NAME));
//  gtk_text_buffer_move_mark(buffer,
//                            mark,
//                            &iterator);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen(view,
                                     mark);

  return TRUE; // G_SOURCE_CONTINUE
}

G_MODULE_EXPORT gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  GtkSpinButton* spinbutton =
    GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  if (!spinbutton)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF

  { // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard (data_p->GTKState.lock);
    for (Net_GTK_EventsIterator_t iterator = data_p->eventStack.begin ();
         iterator != data_p->eventStack.end ();
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

    data_p->eventStack.clear ();
  } // end lock scope

  // enable buttons ?
  gint current_value = gtk_spin_button_get_value_as_int(spinbutton);
  GtkWidget* widget = NULL;
  if (!data_p->listenerHandle) // <-- client
  {
    widget =
      GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
    if (!widget)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
    else
      gtk_widget_set_sensitive(widget,
                               (current_value > 0));
  } // end IF
  widget =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT(NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  else
    gtk_widget_set_sensitive(widget, (current_value > 0));
  if (!data_p->listenerHandle) // <-- client
  {
    widget =
      GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
    if (!widget)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT(NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
    else
      gtk_widget_set_sensitive(widget, (current_value > 0));
  } // end IF

  return TRUE; // G_SOURCE_CONTINUE
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
button_connect_clicked_cb(GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::button_connect_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
  int signal = 0;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signal = SIGUSR1;
#else
  signal = SIGBREAK;
#endif
  if (ACE_OS::raise (signal) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                signal));

  return FALSE;
} // button_connect_clicked_cb

G_MODULE_EXPORT gint
button_close_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_close_clicked_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
  int signal = 0;
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  signal = SIGUSR2;
#else
  signal = SIGTERM;
#endif
  unsigned int num_connections =
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections ();
  if (num_connections > 0)
    if (ACE_OS::raise (signal) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                  signal));

  return FALSE;
} // button_close_clicked_cb

G_MODULE_EXPORT gint
button_close_all_clicked_cb (GtkWidget* widget_in,
                             gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_close_all_clicked_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  NET_CONNECTIONMANAGER_SINGLETON::instance ()->abortConnections ();

  return FALSE;
} // button_close_all_clicked_cb

G_MODULE_EXPORT gint
button_ping_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_ping_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

  // sanity check
  unsigned int num_connections =
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->numConnections ();
  if (num_connections == 0)
    return FALSE;

  // grab a (random) connection handler
  // *WARNING*: there's a race condition here...
  RPG_Dice_RollResult_t result;
  RPG_Dice::generateRandomNumbers (num_connections,
                                   1,
                                   result);
  const Net_InetConnection_Manager_t::CONNECTION_T* connection_handler =
    NET_CONNECTIONMANAGER_SINGLETON::instance ()->operator[](result.front () - 1);
  if (!connection_handler)
  {
    //				ACE_DEBUG((LM_ERROR,
    //									 ACE_TEXT("failed to retrieve connection handler, continuing")));

    return FALSE;
  } // end IF

  try
  {
    const_cast<Net_InetConnection_Manager_t::CONNECTION_T*> (connection_handler)->ping ();
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in RPG_Net_IConnection::ping(), aborting\n")));

    // clean up
    const_cast<Net_InetConnection_Manager_t::CONNECTION_T*> (connection_handler)->decrease ();

    return FALSE;
  }

  // clean up
  const_cast<Net_InetConnection_Manager_t::CONNECTION_T*> (connection_handler)->decrease ();

  return FALSE;
} // button_ping_clicked_cb

G_MODULE_EXPORT gint
togglebutton_stress_toggled_cb (GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::togglebutton_stress_toggled_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->timeoutHandler);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // schedule action interval timer ?
  if (data_p->timerId == -1)
  {
    ACE_Event_Handler* eh = data_p->timeoutHandler;
    ACE_Time_Value interval((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000),
                            ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000));
    data_p->timerId =
      COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (eh,                               // event handler
                                                            NULL,                             // ACT
                                                            COMMON_TIME_POLICY () + interval, // first wakeup time
                                                            interval);                        // interval
    if (data_p->timerId == -1)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to schedule action timer: \"%m\", aborting\n")));
      return FALSE;
    } // end IF
  } // end IF
  else
  {
    const void* act = NULL;
    if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (data_p->timerId,
                                                            &act) <= 0)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                  data_p->timerId));

    // clean up
    data_p->timerId = -1;
  } // end ELSE

  // toggle buttons
  GtkWidget* widget =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR(NET_UI_GTK_BUTTONBOX_ACTIONS_NAME)));
  if (!widget)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_UI_GTK_BUTTONBOX_ACTIONS_NAME)));
  else
    gtk_widget_set_sensitive(widget,
                             !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget_in)));

  return FALSE;
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
togglebutton_listen_toggled_cb (GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::togglebutton_listen_toggled_cb"));

  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->listenerHandle);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget_in)))
  {
    try
    {
      data_p->listenerHandle->start ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Net_Server_IListener::start(): \"%m\", continuing\n")));
    } // end catch
  } // end IF
  else
  {
    try
    {
      data_p->listenerHandle->stop ();
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Net_Server_IListener::stop(): \"%m\", continuing\n")));
    } // end catch
  } // end IF

  return FALSE;
} // togglebutton_listen_toggled_cb

G_MODULE_EXPORT gint
button_report_clicked_cb (GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_report_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) instead...
  int signal = 0;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  signal = SIGUSR1;
#else
  signal = SIGBREAK;
#endif
  if (ACE_OS::raise (signal) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                signal));

  return FALSE;
}

// -----------------------------------------------------------------------------

G_MODULE_EXPORT gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // retrieve about dialog handle
  GtkDialog* about_dialog =
    GTK_DIALOG (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));

    return TRUE; // propagate
  } // end IF

  // run dialog
  gint result = gtk_dialog_run (about_dialog);
  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    default:
      break;
  } // end SWITCH
  gtk_widget_hide (GTK_WIDGET (about_dialog));

  return FALSE;
} // button_about_clicked_cb

G_MODULE_EXPORT gint
button_quit_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_quit_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  Net_GTK_CBData_t* data_p = static_cast<Net_GTK_CBData_t*> (userData_in);
  // sanity check(s)
  ACE_ASSERT (data_p);

  // step1: remove event sources
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (data_p->GTKState.lock);

    for (Common_UI_GTKEventSourceIdsIterator_t iterator = data_p->GTKState.eventSourceIds.begin ();
         iterator != data_p->GTKState.eventSourceIds.end ();
         iterator++)
      if (!g_source_remove (*iterator))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
                    *iterator));
    data_p->GTKState.eventSourceIds.clear ();
  } // end lock scope

  // step2: initiate shutdown sequence
  if (ACE_OS::raise (SIGINT) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));

  return FALSE;
} // button_quit_clicked_cb

#ifdef __cplusplus
}
#endif /* __cplusplus */
