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

#include "common_timer_manager.h"

#include "common_ui_common.h"
#include "common_ui_defines.h"
#include "common_ui_tools.h"

#include "common_ui_gtk_manager.h"
#include "common_ui_gtk_manager_common.h"

#include "net_common.h"
#include "net_defines.h"

#include "rpg_dice.h"

#include "rpg_common_macros.h"

#include "rpg_net_protocol_network.h"

#include "rpg_client_defines.h"
#include "rpg_client_ui_tools.h"

#include "net_server_common.h"

gboolean
idle_initialize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_initialize_UI_cb"));

  struct Net_Server_GTK_CBData* data_p =
    static_cast<struct Net_Server_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->UIState->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->UIState->gladeXML.end ());
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // step1: initialize dialog window(s)
  GtkWidget* dialog_p =
    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog_p);
  //  GtkWidget* image_icon = gtk_image_new_from_file (path.c_str());
  //  ACE_ASSERT (image_icon);
  //  gtk_window_set_icon (GTK_WINDOW (dialog_p),
  //                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon)));
  //GdkWindow* dialog_window_p = gtk_widget_get_window (dialog_p);
  //gtk_window_set_title(,
  //                     caption.c_str ());

  GtkWidget* about_dialog_p =
    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog_p);

  // step2: initialize info view
  GtkSpinButton* spinbutton_p =
    //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  ACE_ASSERT (spinbutton_p);
  gtk_spin_button_set_range (spinbutton_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  //  gtk_entry_set_editable (GTK_ENTRY(spinbutton_p),
  //                          FALSE);

  // step3: initialize text view, setup auto-scrolling
  GtkTextView* view_p =
    //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
    //                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p =
//      gtk_text_buffer_new (NULL); // text tag table --> create new
//      gtk_text_view_set_buffer (view_p, buffer_p);
      gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);

  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter (buffer_p,
  //                                &iterator);
  //  gtk_text_buffer_create_mark (buffer_p,
  //                               ACE_TEXT_ALWAYS_CHAR (NET_UI_SCROLLMARK_NAME),
  //                               &iterator,
  //                               TRUE);
//  g_object_unref (buffer_p);

  PangoFontDescription* font_description_p =
    pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_FONTDESCRIPTION));
  if (!font_description_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (NET_UI_LOG_FONTDESCRIPTION)));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  // apply font
  GtkRcStyle* rc_style_p = gtk_rc_style_new ();
  if (!rc_style_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  rc_style_p->font_desc = font_description_p;
  GdkColor base_colour, text_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_BASE),
                   &base_colour);
  rc_style_p->base[GTK_STATE_NORMAL] = base_colour;
  gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (NET_UI_LOG_TEXT),
                   &text_colour);
  rc_style_p->text[GTK_STATE_NORMAL] = text_colour;
  rc_style_p->color_flags[GTK_STATE_NORMAL] =
      static_cast<GtkRcFlags>(GTK_RC_BASE |
                              GTK_RC_TEXT);
  gtk_widget_modify_style (GTK_WIDGET (view_p),
                           rc_style_p);
  g_object_unref (rc_style_p);

  // step4: initialize updates
  // schedule asynchronous updates of the log view
  guint event_source_id = g_timeout_add_seconds (1,
                                                 idle_update_log_display_cb,
                                                 userData_in);
  if (event_source_id > 0)
    data_p->UIState->eventSourceIds.insert (event_source_id);
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
    data_p->UIState->eventSourceIds.insert (event_source_id);
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end ELSE

  // step5: disable some functions ?
  GtkButton* button_p =
      //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p), FALSE);
  button_p =
      //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p),
                            data_p->allowUserRuntimeStatistic);

  // step6: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_connect_data does not work reliably
  //glade_xml_signal_autoconnect(userData_out.xml);
  //gtk_builder_connect_signals ((*iterator).second.second,
  //                             userData_in);

  // step6a: connect default signals
  gulong result =
      g_signal_connect (dialog_p,
                        ACE_TEXT_ALWAYS_CHAR ("destroy"),
                        G_CALLBACK (gtk_widget_destroyed),
                        &dialog_p);
  ACE_ASSERT (result);

  // step6b: connect custom signals
  // *NOTE*: glade_xml_signal_connect_data does not work reliably on Windows
  //glade_xml_signal_connect_data(userData_out.xml,
  //                              ACE_TEXT_ALWAYS_CHAR("togglebutton_listen_toggled_cb"),
  //                              G_CALLBACK(togglebutton_listen_toggled_cb),
  //                              &userData_out);
  GObject* object_p =
    //GTK_TOGGLE_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                         ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_GTK_BUTTON_LISTEN_NAME)));
    gtk_builder_get_object ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (NET_SERVER_UI_GTK_BUTTON_LISTEN_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("toggled"),
                        G_CALLBACK (togglebutton_listen_toggled_cb),
                        userData_in);
  ACE_ASSERT (result);
  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_close_all_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME)));
    gtk_builder_get_object ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_REPORT_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_report_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_ABOUT_NAME)));
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_ABOUT_NAME));
  ACE_ASSERT (object_p);
  result = g_signal_connect (object_p,
                             ACE_TEXT_ALWAYS_CHAR ("clicked"),
                             G_CALLBACK (button_about_clicked_cb),
                             userData_in);
  ACE_ASSERT (result);
  object_p =
    //GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_QUIT_NAME)));
      gtk_builder_get_object ((*iterator).second.second,
                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_QUIT_NAME));
  ACE_ASSERT (object_p);
  result =
      g_signal_connect (object_p,
                        ACE_TEXT_ALWAYS_CHAR ("clicked"),
                        G_CALLBACK (button_quit_clicked_cb),
                        userData_in);
  ACE_ASSERT (result);

  //   // step7: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen (GTK_WINDOW (dialog_p),
  //                            gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step8: draw main dialog
  gtk_widget_show_all (dialog_p);

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  // leave GTK
  gtk_main_quit ();

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_update_log_display_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_update_log_display_cb"));

  struct Net_Server_GTK_CBData* data_p =
    static_cast<struct Net_Server_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->UIState->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->UIState->gladeXML.end ());
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  GtkTextView* view_p =
      //GTK_TEXT_VIEW (glade_xml_get_widget ((*iterator).second.second,
      //                                     ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
      GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_TEXTVIEW_NAME)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p = gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);

  GtkTextIter text_iterator;
  gtk_text_buffer_get_end_iter (buffer_p,
                                &text_iterator);

  gchar* converted_text = NULL;
  { ACE_Guard<ACE_SYNCH_MUTEX> aGuard (data_p->UIState->lock);
    // sanity check
    if (data_p->UIState->logStack.empty ())
      return TRUE; // G_SOURCE_CONTINUE

    // step1: convert text
    for (Common_MessageStackConstIterator_t iterator_2 = data_p->UIState->logStack.begin ();
         iterator_2 != data_p->UIState->logStack.end ();
         iterator_2++)
    {
      converted_text = Common_UI_GTK_Tools::localeToUTF8 (*iterator_2);
      if (!converted_text)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to convert message text (was: \"%s\"), aborting\n"),
                    ACE_TEXT ((*iterator_2).c_str ())));
        return FALSE; // G_SOURCE_REMOVE
      } // end IF

      // step2: display text
      gtk_text_buffer_insert (buffer_p,
                              &text_iterator,
                              converted_text,
                              -1);

      // clean up
      g_free (converted_text);
    } // end FOR

    data_p->UIState->logStack.clear ();
  } // end lock scope

  // step3: scroll the view accordingly
//  // move the iterator to the beginning of line, so it doesn't scroll
//  // in horizontal direction
//  gtk_text_iter_set_line_offset (&text_iterator, 0);

//  // ...and place the mark at iter. The mark will stay there after insertion
//  // because it has "right" gravity
//  GtkTextMark* text_mark_p =
//      gtk_text_buffer_get_mark (buffer_p,
//                                ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SCROLLMARK_NAME));
////  gtk_text_buffer_move_mark (buffer_p,
////                             text_mark_p,
////                             &text_iterator);

//  // scroll the mark onscreen
//  gtk_text_view_scroll_mark_onscreen (view_p,
//                                      text_mark_p);
  GtkAdjustment* adjustment_p =
      GTK_ADJUSTMENT (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_ADJUSTMENT_NAME)));
  ACE_ASSERT (adjustment_p);
  gtk_adjustment_set_value (adjustment_p,
                            gtk_adjustment_get_upper (adjustment_p));

  return TRUE; // G_SOURCE_CONTINUE
}

gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  struct Net_Server_GTK_CBData* data_p = static_cast<struct Net_Server_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->UIState->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->UIState->gladeXML.end ());
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  GtkSpinButton* spinbutton_p =
    //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
    //                                       ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME)));
  ACE_ASSERT (spinbutton_p);

  enum Common_UI_EventType* event_p = NULL;
  { // synch access
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (data_p->UIState->lock);
    for (Common_UI_Events_t::ITERATOR iterator_2 (data_p->UIState->eventStack);
         iterator_2.next (event_p);
         iterator_2.advance ())
    { ACE_ASSERT (event_p);
      switch (*event_p)
      {
        case COMMON_UI_EVENT_CONNECT:
        {
          // update info label
          gtk_spin_button_spin (spinbutton_p,
                                GTK_SPIN_STEP_FORWARD,
                                1.0);
          break;
        }
        case COMMON_UI_EVENT_DISCONNECT:
        {
          // update info label
          gtk_spin_button_spin (spinbutton_p,
                                GTK_SPIN_STEP_BACKWARD,
                                1.0);
          break;
        }
        case COMMON_UI_EVENT_STATISTIC:
        {
          // *TODO*
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                      *event_p));
          break;
        }
      } // end SWITCH
    } // end FOR

    // clean up
    enum Common_UI_EventType event_e = COMMON_UI_EVENT_INVALID;
    int result = -1;
    while (!data_p->UIState->eventStack.is_empty ())
    {
      result = data_p->UIState->eventStack.pop (event_e);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
          ACE_TEXT ("failed to ACE_Unbounded_Stack::pop(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope

  // enable buttons ?
  gint current_value = gtk_spin_button_get_value_as_int (spinbutton_p);
  GtkWidget* widget_p = NULL;
  if (!data_p->listenerHandle) // <-- client
  {
    widget_p =
      //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
    if (!widget_p)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME)));
    else
      gtk_widget_set_sensitive (widget_p, (current_value > 0));
  } // end IF
  widget_p =
    //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  if (!widget_p)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (NET_UI_GTK_BUTTON_CLOSEALL_NAME)));
  else
    gtk_widget_set_sensitive (widget_p, (current_value > 0));
  if (!data_p->listenerHandle) // <-- client
  {
    widget_p =
      //GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
      //                                  ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
    if (!widget_p)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (NET_CLIENT_UI_GTK_BUTTON_PING_NAME)));
    else
      gtk_widget_set_sensitive (widget_p, (current_value > 0));
  } // end IF

  return TRUE; // G_SOURCE_CONTINUE
}

// -----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
gint
button_close_all_clicked_cb (GtkWidget* widget_in,
                             gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_close_all_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);
  //struct Net_Server_GTK_CBData* data_p = static_cast<struct Net_Server_GTK_CBData*> (userData_in);

  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->UIState->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  //ACE_ASSERT (iterator != data_p->UIState->gladeXML.end ());

  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->abort ();

  return FALSE;
} // button_close_all_clicked_cb

gint
togglebutton_listen_toggled_cb (GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::togglebutton_listen_toggled_cb"));

  struct Net_Server_GTK_CBData* data_p = static_cast<struct Net_Server_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->listenerHandle);

  ////Common_UI_GladeXMLsIterator_t iterator =
  ////  data_p->UIState->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  //Common_UI_GTK_BuildersIterator_t iterator =
  //  data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  //// sanity check(s)
  ////ACE_ASSERT (iterator != data_p->UIState->gladeXML.end ());
  //ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget_in)))
  {
    try {
      data_p->listenerHandle->start (NULL);
    } catch (...) {
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

gint
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

gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct Net_Server_GTK_CBData* data_p = static_cast<struct Net_Server_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  //Common_UI_GladeXMLsIterator_t iterator =
  //  data_p->UIState->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  //ACE_ASSERT (iterator != data_p->UIState->gladeXML.end ());
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve about dialog handle
  GtkDialog* about_dialog =
    //GTK_DIALOG (glade_xml_get_widget ((*iterator).second.second,
    //                                  ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
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

gint
button_quit_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::button_quit_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);
  //struct Net_Server_GTK_CBData* data_p = static_cast<struct Net_Server_GTK_CBData*> (userData_in);
  //// sanity check(s)
  //ACE_ASSERT (data_p);

  //// step1: remove event sources
  //{
  //  ACE_Guard<ACE_Thread_Mutex> aGuard (data_p->UIState->lock);

  //  for (Common_UI_GTKEventSourceIdsIterator_t iterator = data_p->UIState->eventSourceIds.begin ();
  //       iterator != data_p->UIState->eventSourceIds.end ();
  //       iterator++)
  //    if (!g_source_remove (*iterator))
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
  //                  *iterator));
  //  data_p->UIState->eventSourceIds.clear ();
  //} // end lock scope

  // step2: initiate shutdown sequence
  if (ACE_OS::raise (SIGINT) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));

  // step3: stop GTK event processing
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();

  return FALSE;
} // button_quit_clicked_cb
#ifdef __cplusplus
}
#endif /* __cplusplus */
