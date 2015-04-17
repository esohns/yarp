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

#include "IRC_client_gui_messagehandler.h"

#include "common_file_tools.h"

#include "rpg_common_macros.h"

#include "rpg_client_ui_tools.h"

#include "rpg_net_protocol_iIRCControl.h"
#include "rpg_net_protocol_tools.h"

#include "IRC_client_gui_callbacks.h"
#include "IRC_client_gui_connection.h"
#include "IRC_client_gui_defines.h"

// update callback
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
update_display_cb(gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::update_display_cb"));

  // sanity check(s)
  ACE_ASSERT(userData_in);
  IRC_Client_GUI_MessageHandler* message_handler = static_cast<IRC_Client_GUI_MessageHandler*>(userData_in);
  ACE_ASSERT(message_handler);

  // *WARNING*: callbacks scheduled via g_idle_add need to be protected by
  // gdk_threads_enter/gdk_threads_leave !
  gdk_threads_enter();

  message_handler->update();

  gdk_threads_leave();

  // --> reschedule
  return TRUE;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler (Common_UI_GTKState* GTKState_in,
                                                              GtkTextView* view_in)
 : CBData_ ()
 , displayQueue_ ()
 , lock_ ()
 , eventSourceID_ (0)
 , isFirstMemberListMsg_ (true)
 , parent_ (NULL)
 , view_ (view_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT (GTKState_in);
  ACE_ASSERT (view_in);

  // initialize cb data
  CBData_.id.clear ();
  CBData_.GTKState = GTKState_in;
  CBData_.controller = NULL;

  // setup auto-scrolling
  GtkTextIter iterator;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (view_),
                                &iterator);
  gtk_text_buffer_create_mark (gtk_text_view_get_buffer (view_),
                               ACE_TEXT_ALWAYS_CHAR ("scroll"),
                               &iterator,
                               TRUE);
}

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler (Common_UI_GTKState* GTKState_in,
                                                              IRC_Client_GUI_Connection* connection_in,
                                                              RPG_Net_Protocol_IIRCControl* controller_in,
                                                              const std::string& id_in,
                                                              const std::string& UIFileDirectory_in,
                                                              GtkNotebook* parent_in)
 : CBData_ ()
 , displayQueue_ ()
 , lock_ ()
 , eventSourceID_ (0)
 , isFirstMemberListMsg_ (true)
 , parent_ (parent_in)
 , view_ (NULL)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT (GTKState_in);
  ACE_ASSERT(connection_in);
  ACE_ASSERT(controller_in);
  ACE_ASSERT (!id_in.empty ());
  if (!Common_File_Tools::isDirectory (UIFileDirectory_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was: \"%s\"): not a directory, returning\n"),
                ACE_TEXT (UIFileDirectory_in.c_str ())));
    return;
  } // end IF
  ACE_ASSERT (parent_in);

  // initialize cb data
  CBData_.GTKState = GTKState_in;
  CBData_.connection = connection_in;
  CBData_.id = id_in;
  CBData_.controller = controller_in;
  CBData_.channelModes = 0;

  // create new GtkBuilder
  GtkBuilder* builder_p = gtk_builder_new ();
  if (!builder_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));
    return;
  } // end IF
  std::string ui_definition_filename = UIFileDirectory_in;
  ui_definition_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_definition_filename += IRC_CLIENT_GUI_DEF_UI_CHANNEL_TAB_FILE;
  if (!Common_File_Tools::isReadable (ui_definition_filename))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid UI file (was: \"%s\"): not readable, returning\n"),
                ACE_TEXT (ui_definition_filename.c_str ())));

    // clean up
    g_object_unref (G_OBJECT (builder_p));

    return;
  } // end IF

  // load widget tree
  GError* error = NULL;
  gtk_builder_add_from_file (builder_p,
                             ui_definition_filename.c_str (),
                             &error);
  if (error)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_add_from_file(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (ui_definition_filename.c_str ()),
                ACE_TEXT (error->message)));

    // clean up
    g_error_free (error);
    g_object_unref (G_OBJECT (builder_p));

    return;
  } // end IF

  // setup auto-scrolling in textview
  view_ =
      GTK_TEXT_VIEW (gtk_builder_get_object (builder_p,
                                             ACE_TEXT_ALWAYS_CHAR ("channel_tab_textview")));
  ACE_ASSERT (view_);
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (view_),
                                &iter);
  gtk_text_buffer_create_mark (gtk_text_view_get_buffer (view_),
                               ACE_TEXT_ALWAYS_CHAR ("scroll"),
                               &iter,
                               TRUE);

  // enable multi-selection in treeview
  GtkTreeView* tree_view_p =
    GTK_TREE_VIEW (gtk_builder_get_object (builder_p,
                                           ACE_TEXT_ALWAYS_CHAR ("channel_tab_treeview")));
  ACE_ASSERT (tree_view_p);
  GtkTreeSelection* tree_selection_p =
    gtk_tree_view_get_selection (tree_view_p);
  ACE_ASSERT (tree_selection_p);
  gtk_tree_selection_set_mode (tree_selection_p,
                               GTK_SELECTION_MULTIPLE);

  // add the invite_channel_members_menu to the "Invite" menu item
  GtkMenu* menu_p =
    GTK_MENU (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR ("invite_channel_members_menu")));
  ACE_ASSERT (menu_p);
  GtkMenuItem* menu_item_p =
    GTK_MENU_ITEM (gtk_builder_get_object (builder_p,
                                           ACE_TEXT_ALWAYS_CHAR ("menuitem_invite")));
  ACE_ASSERT (menu_item_p);
  gtk_menu_item_set_submenu (menu_item_p, GTK_WIDGET (menu_p));

  // connect signal(s)
  GtkButton* button_p =
    GTK_BUTTON (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("channel_tab_label_button")));
  ACE_ASSERT (button_p);
  g_signal_connect (button_p,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (part_clicked_cb),
                    &CBData_);
  // toggle buttons
  GtkToggleButton* toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_key_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_voice_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_ban_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_userlimit_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_moderated_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_blockforeign_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_restricttopic_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_inviteonly_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_secret_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_private_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder_p,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_operator_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  g_signal_connect (toggle_button_p,
                    ACE_TEXT_ALWAYS_CHAR ("toggled"),
                    G_CALLBACK (channel_mode_toggled_cb),
                    &CBData_);
  // topic label
  GtkEventBox* event_box_p =
    GTK_EVENT_BOX (gtk_builder_get_object (builder_p,
                                           ACE_TEXT_ALWAYS_CHAR ("channel_tab_topic_label_eventbox")));
  ACE_ASSERT (event_box_p);
  g_signal_connect (event_box_p,
                    ACE_TEXT_ALWAYS_CHAR ("button-press-event"),
                    G_CALLBACK (topic_clicked_cb),
                    &CBData_);
  // context menu in treeview
  g_signal_connect (tree_view_p,
                    ACE_TEXT_ALWAYS_CHAR ("button-press-event"),
                    G_CALLBACK (members_clicked_cb),
                    &CBData_);
  // actions in treeview
  GtkAction* action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("action_msg")));
  ACE_ASSERT (action_p);
  g_signal_connect (action_p,
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (action_msg_cb),
                    &CBData_);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("action_invite")));
  ACE_ASSERT (action_p);
  g_signal_connect (action_p,
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (action_invite_cb),
                    &CBData_);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("action_info")));
  ACE_ASSERT (action_p);
  g_signal_connect (action_p,
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (action_info_cb),
                    &CBData_);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("action_kick")));
  ACE_ASSERT (action_p);
  g_signal_connect (action_p,
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (action_kick_cb),
                    &CBData_);
  action_p =
    GTK_ACTION (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("action_ban")));
  ACE_ASSERT (action_p);
  g_signal_connect (action_p,
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (action_ban_cb),
                    &CBData_);

  // add new channel page to notebook (== server log)
  // retrieve (dummy) parent window
  GtkWindow* window_p =
    GTK_WINDOW (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("channel_tab_label_template")));
  ACE_ASSERT (window_p);
  // retrieve channel tab label
  GtkHBox* hbox_p =
    GTK_HBOX (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR ("channel_tab_label_hbox")));
  ACE_ASSERT (hbox_p);
  g_object_ref (hbox_p);
  gtk_container_remove (GTK_CONTAINER (window_p),
                        GTK_WIDGET (hbox_p));
  // set tab label
  GtkLabel* label_p =
    GTK_LABEL (gtk_builder_get_object (builder_p,
                                       ACE_TEXT_ALWAYS_CHAR ("channel_tab_label")));
  ACE_ASSERT (label_p);
  std::string page_tab_label_string;
  if (!RPG_Net_Protocol_Tools::isValidIRCChannelName (CBData_.id))
  {
    // --> private conversation window, modify label accordingly
    page_tab_label_string = ACE_TEXT_ALWAYS_CHAR ("[");
    page_tab_label_string += CBData_.id;
    page_tab_label_string += ACE_TEXT_ALWAYS_CHAR("]");

    // hide channel mode tab frame
    GtkFrame* frame_p =
      GTK_FRAME (gtk_builder_get_object (builder_p,
                                         ACE_TEXT_ALWAYS_CHAR ("channel_tab_mode_frame")));
    ACE_ASSERT (frame_p);
    gtk_widget_hide (GTK_WIDGET (frame_p));
    // hide channel tab treeview
    tree_view_p =
      GTK_TREE_VIEW (gtk_builder_get_object (builder_p,
                                             ACE_TEXT_ALWAYS_CHAR ("channel_tab_treeview")));
    ACE_ASSERT (tree_view_p);
    gtk_widget_hide (GTK_WIDGET (tree_view_p));

    // erase "topic" label
    label_p =
      GTK_LABEL (gtk_builder_get_object (builder_p,
                                         ACE_TEXT_ALWAYS_CHAR ("channel_tab_topic_label")));
    ACE_ASSERT (label_p);
    gtk_label_set_text (label_p, NULL);
  } // end IF
  else
    page_tab_label_string = CBData_.id;
  gtk_label_set_text (label_p,
                      page_tab_label_string.c_str ());
  // retrieve (dummy) parent window
  window_p =
    GTK_WINDOW (gtk_builder_get_object (builder_p,
                                        ACE_TEXT_ALWAYS_CHAR ("channel_tab_template")));
  ACE_ASSERT (window_p);
  // retrieve channel tab
  GtkVBox* vbox_p =
    GTK_VBOX (gtk_builder_get_object (builder_p,
                                      ACE_TEXT_ALWAYS_CHAR ("channel_tab_vbox")));
  ACE_ASSERT (vbox_p);
  g_object_ref (vbox_p);
  gtk_container_remove (GTK_CONTAINER (window_p),
                        GTK_WIDGET (vbox_p));
  gint page_num = gtk_notebook_append_page (parent_,
                                            GTK_WIDGET (vbox_p),
                                            GTK_WIDGET (hbox_p));
  if (page_num == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_notebook_append_page(%@), returning\n"),
                parent_));

    // clean up
    g_object_unref (hbox_p);
    g_object_unref (vbox_p);
    g_object_unref (G_OBJECT (builder_p));

    return;
  } // end IF
  g_object_unref (hbox_p);

  // allow reordering
  gtk_notebook_set_tab_reorderable (parent_,
                                    GTK_WIDGET (vbox_p),
                                    TRUE);
  g_object_unref (vbox_p);

  // activate new page (iff it's a channel tab !)
  if (RPG_Net_Protocol_Tools::isValidIRCChannelName (CBData_.id))
    gtk_notebook_set_current_page (parent_,
                                   page_num);

  builderLabel_ = connection_in->getLabel ();
  builderLabel_ += ACE_TEXT_ALWAYS_CHAR ("::");
  builderLabel_ += page_tab_label_string;
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

    CBData_.GTKState->builders[builderLabel_] =
        std::make_pair (ui_definition_filename, builder_p);
  } // end lock scope
}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler ()
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  // remove queued events
  while (g_idle_remove_by_data (this));
  if (eventSourceID_)
  {
    Common_UI_GTKEventSourceIdsIterator_t iterator =
        CBData_.GTKState->eventSourceIds.begin();
    while (iterator != CBData_.GTKState->eventSourceIds.end())
    {
      if (*iterator == eventSourceID_)
      {
        iterator = CBData_.GTKState->eventSourceIds.erase(iterator);
        continue;
      } // end IF

      iterator++;
    }
    if (iterator != CBData_.GTKState->eventSourceIds.end ())
      CBData_.GTKState->eventSourceIds.erase (iterator);
  } // end IF

  // *NOTE*: the server log handler MUST NOT do this...
  if (parent_)
  {
    // remove server page from parent notebook
    Common_UI_GTKBuildersIterator_t iterator =
        CBData_.GTKState->builders.find (builderLabel_);
    // sanity check(s)
    if (iterator == CBData_.GTKState->builders.end ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("handler (was: \"%s\") builder not found, returning\n"),
                  ACE_TEXT (builderLabel_.c_str ())));
      return;
    } // end IF

    GtkVBox* vbox_p =
      GTK_VBOX (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR ("channel_tab_vbox")));
    ACE_ASSERT (vbox_p);
    guint page_num = gtk_notebook_page_num (parent_,
                                            GTK_WIDGET (vbox_p));

    // flip away from "this" page ?
    if (gtk_notebook_get_current_page (parent_) == static_cast<gint> (page_num))
      gtk_notebook_prev_page (parent_);

    // remove channel page from channel tabs notebook
    gtk_notebook_remove_page (parent_,
                              page_num);

    g_object_unref (G_OBJECT ((*iterator).second.second));
    CBData_.GTKState->builders.erase (iterator);
  } // end IF
}

bool
IRC_Client_GUI_MessageHandler::isServerLog () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_SignalHandler::isServerLog"));

  return (parent_ == NULL);
}

void
IRC_Client_GUI_MessageHandler::queueForDisplay (const std::string& text_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_SignalHandler::queueForDisplay"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  {
    // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    displayQueue_.push_back (text_in);
  } // end lock scope

  // schedule asynch update(s) ?
  if (eventSourceID_ == 0)
  {
    eventSourceID_ = g_idle_add (update_display_cb, this);

    // synch access
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

      CBData_.GTKState->eventSourceIds.push_back (eventSourceID_);
    } // end lock scope
  } // end IF
}

void
IRC_Client_GUI_MessageHandler::update ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_SignalHandler::update"));

  // always insert new text at the END of the buffer...
  ACE_ASSERT (view_);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("printing: \"%s\"\n"),
//              ACE_TEXT(displayQueue_.front().c_str())));

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer (view_),
                                &iter);

  {  // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

    // sanity check
    if (displayQueue_.empty ())
      return; // nothing to do...

    // step1: convert text
    gchar* converted_text =
      RPG_Client_UI_Tools::Locale2UTF8 (displayQueue_.front ());
    if (!converted_text)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert message text (was: \"%s\"), returning\n"),
                  ACE_TEXT (displayQueue_.front ().c_str ())));
      return;
    } // end IF

    // step2: display text
    gtk_text_buffer_insert (gtk_text_view_get_buffer (view_), &iter,
                            converted_text,
                            -1);
//   gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer (view_),
//                                    message_text.c_str(),
//                                    message_text.size());

    // clean up
    g_free (converted_text);

    // step3: pop stack
    displayQueue_.pop_front ();
  } // end lock scope

//   // get the new "end"...
//   gtk_text_buffer_get_end_iter(myTargetBuffer,
//                                &iter);
  // move the iterator to the beginning of line, so we don't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset (&iter, 0);

  // ...and place the mark at iter. The mark will stay there after we
  // insert some text at the end because it has right gravity
  GtkTextMark* mark = NULL;
  mark = gtk_text_buffer_get_mark (gtk_text_view_get_buffer (view_),
                                   ACE_TEXT_ALWAYS_CHAR ("scroll"));
  gtk_text_buffer_move_mark (gtk_text_view_get_buffer (view_),
                             mark,
                             &iter);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen (view_,
                                      mark);

  // redraw view area...
//   // sanity check(s)
//   ACE_ASSERT(myBuilder);
// //   GtkScrolledWindow* scrolledwindow = NULL;
//   GtkWindow* dialog = NULL;
//   dialog = GTK_WINDOW(gtk_builder_get_object(myBuilder,
//                                        ACE_TEXT_ALWAYS_CHAR("dialog")));
//   ACE_ASSERT(dialog);
//   GdkRegion* region = NULL;
//   region = gdk_drawable_get_clip_region(GTK_WIDGET(dialog)->window);
//   ACE_ASSERT(region);
//   gdk_window_invalidate_region(GTK_WIDGET(dialog)->window,
//                                region,
//                                TRUE);
  gdk_window_invalidate_rect (GTK_WIDGET (view_)->window,
                              NULL,
                              TRUE);
//   gdk_region_destroy(region);
//   gtk_widget_queue_draw(GTK_WIDGET(view_));
  gdk_window_process_updates (GTK_WIDGET (view_)->window, TRUE);
//   gdk_window_process_all_updates();
}

GtkWidget*
IRC_Client_GUI_MessageHandler::getTopLevelPageChild ()
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::getTopLevelPageChild"));

  GtkWidget* widget_p = NULL;

  // *WARNING*: the server log handler doesn't have a builder...
  if (!parent_)
  {
    // sanity check(s)
    ACE_ASSERT (view_);

    widget_p =  gtk_widget_get_ancestor (GTK_WIDGET (view_),
                                         GTK_TYPE_WIDGET);
  } // end IF
  else
  {
    // sanity check(s)
    ACE_ASSERT (CBData_.GTKState);

    ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

    Common_UI_GTKBuildersIterator_t iterator =
        CBData_.GTKState->builders.find (builderLabel_);
    // sanity check(s)
    ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

    widget_p =
        GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR ("channel_tab_vbox")));
  } // end ELSE
  ACE_ASSERT (widget_p);

  return widget_p;
}

// const std::string
// IRC_Client_GUI_MessageHandler::getChannel() const
// {
//   RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::getChannel"));
//
//   // sanity check: 'this' might be a private message handler !...
//   ACE_ASSERT(RPG_Net_Protocol_Tools::isValidIRCChannelName(CBData_.id));
//
//   return CBData_.id;
// }

void
IRC_Client_GUI_MessageHandler::setTopic (const std::string& topic_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::setTopic"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  // retrieve label handle
  GtkLabel* label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR ("channel_tab_topic_label")));
  ACE_ASSERT (label_p);
  gtk_label_set_text (label_p,
                      topic_in.c_str ());
}

void
IRC_Client_GUI_MessageHandler::setModes (const std::string& modes_in,
                                         const std::string& parameter_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::setModes"));

  ACE_UNUSED_ARG (parameter_in);

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  RPG_Net_Protocol_Tools::merge (modes_in,
                                 CBData_.channelModes);

  updateModeButtons ();

  // enable channel modes ?
  // retrieve channel tab mode hbox handle
  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  GtkHBox* hbox_p =
    GTK_HBOX (gtk_builder_get_object ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR ("channel_tab_mode_hbox")));
  ACE_ASSERT (hbox_p);
  gtk_widget_set_sensitive (GTK_WIDGET (hbox_p),
                            CBData_.channelModes.test (CHANNELMODE_OPERATOR));
}

void
IRC_Client_GUI_MessageHandler::clearMembers ()
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::clearMembers"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  // retrieve channel liststore handle
  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR ("channel_liststore")));
  ACE_ASSERT (list_store_p);

  // clear liststore
  gtk_list_store_clear (list_store_p);
}

void
IRC_Client_GUI_MessageHandler::updateNick (const std::string& oldNick_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::updateNick"));

  // sanity check(s)
  ACE_ASSERT (CBData_.connection);

  std::string new_nick = CBData_.connection->getNickname ();
  if (CBData_.channelModes.test (CHANNELMODE_OPERATOR))
    new_nick.insert (new_nick.begin (), '@');

  remove (oldNick_in);
  add (new_nick);
}

void
IRC_Client_GUI_MessageHandler::add (const std::string& nick_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::add"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  // retrieve channel liststore handle
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR ("channel_liststore")));
  ACE_ASSERT (list_store_p);

  // step1: convert text
  GtkTreeIter iter;
  gchar* converted_nick_string = RPG_Client_UI_Tools::Locale2UTF8 (nick_in);
  if (!converted_nick_string)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert nickname: \"%s\", returning\n")));
    return;
  } // end IF

  // step2: append new (text) entry
  gtk_list_store_append (list_store_p, &iter);
  gtk_list_store_set (list_store_p, &iter,
                      0, converted_nick_string, // column 0
                      -1);

  // clean up
  g_free (converted_nick_string);
}

void
IRC_Client_GUI_MessageHandler::remove (const std::string& nick_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::remove"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  // retrieve channel liststore handle
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR ("channel_liststore")));
  ACE_ASSERT (list_store_p);

  // step1: convert text
  gchar* converted_nick_string = RPG_Client_UI_Tools::Locale2UTF8 (nick_in);
  if (!converted_nick_string)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert nickname: \"%s\", returning\n")));
    return;
  } // end IF

  // step2: find matching entry
  GtkTreeIter current_iter;
//   GValue current_value;
  gchar* current_value_string = NULL;
  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store_p),
                                      &current_iter))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_tree_model_get_iter_first(%@), returning\n"),
                list_store_p));

    // clean up
    g_free (converted_nick_string);

    return;
  } // end IF
  bool found_row = false;
  do
  {
    current_value_string = NULL;

    // retrieve value
//     gtk_tree_model_get_value(GTK_TREE_MODEL(list_store_p),
//                              current_iter,
//                              0, &current_value);
    gtk_tree_model_get (GTK_TREE_MODEL (list_store_p),
                        &current_iter,
                        0, &current_value_string,
                        -1);
    if (g_str_equal (current_value_string,
                     converted_nick_string) ||
        (g_str_has_suffix (current_value_string,
                           converted_nick_string) &&
         ((current_value_string[0] == '@'))))
      found_row = true;

    // clean up
    g_free (current_value_string);

    if (found_row)
      break; // found value
  } while (gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store_p),
                                     &current_iter));

  // clean up
  g_free (converted_nick_string);

  if (found_row)
    gtk_list_store_remove (list_store_p,
                           &current_iter);
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to remove nick (was: \"%s\"), aborting\n"),
                ACE_TEXT (nick_in.c_str ())));
}

void
IRC_Client_GUI_MessageHandler::members (const string_list_t& list_in)
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::members"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  if (isFirstMemberListMsg_)
  {
    clearMembers ();

    isFirstMemberListMsg_ = false;
  } // end IF

  // retrieve channel liststore handle
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR ("channel_liststore")));
  ACE_ASSERT (list_store_p);

  GtkTreeIter iter;
  gchar* converted_nick_string = NULL;
  for (string_list_const_iterator_t iterator = list_in.begin ();
       iterator != list_in.end ();
       iterator++)
  {
    // step1: convert text
    converted_nick_string = RPG_Client_UI_Tools::Locale2UTF8 (*iterator);
    if (!converted_nick_string)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert nickname: \"%s\", returning\n")));
      return;
    } // end IF

    // step2: append new (text) entry
    gtk_list_store_append (list_store_p, &iter);
    gtk_list_store_set (list_store_p, &iter,
                        0, converted_nick_string, // column 0
                        -1);

    // clean up
    g_free (converted_nick_string);
  } // end FOR
}

void
IRC_Client_GUI_MessageHandler::endMembers ()
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::endMembers"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  isFirstMemberListMsg_ = true;

  // retrieve treeview handle
  GtkTreeView* tree_view_p =
      GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR ("channel_tab_treeview")));
  ACE_ASSERT (tree_view_p);
  gtk_widget_set_sensitive (GTK_WIDGET (tree_view_p), TRUE);
}

void
IRC_Client_GUI_MessageHandler::updateModeButtons ()
{
  RPG_TRACE (ACE_TEXT ("IRC_Client_GUI_MessageHandler::updateModeButtons"));

  // sanity check(s)
  ACE_ASSERT (CBData_.GTKState);

  ACE_Guard<ACE_Thread_Mutex> aGuard (CBData_.GTKState->lock);

  Common_UI_GTKBuildersIterator_t iterator =
      CBData_.GTKState->builders.find (builderLabel_);
  // sanity check(s)
  ACE_ASSERT (iterator != CBData_.GTKState->builders.end ());

  // display (changed) channel modes
  GtkToggleButton* toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_key_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_PASSWORD]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_voice_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_VOICE]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_ban_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_BAN]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_userlimit_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_USERLIMIT]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_moderated_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_MODERATED]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_blockforeign_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_BLOCKFOREIGNMSGS]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_restricttopic_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_RESTRICTEDTOPIC]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_inviteonly_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_INVITEONLY]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_secret_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_SECRET]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_private_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_PRIVATE]);
  toggle_button_p =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR ("mode_operator_toggle_button_p")));
  ACE_ASSERT (toggle_button_p);
  gtk_toggle_button_set_active (toggle_button_p,
                                CBData_.channelModes[CHANNELMODE_OPERATOR]);
}
