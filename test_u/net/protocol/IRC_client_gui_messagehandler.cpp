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

#include "IRC_client_gui_messagehandler.h"

#include "IRC_client_gui_defines.h"
#include "IRC_client_gui_connection.h"
#include "IRC_client_gui_callbacks.h"

#include "IRC_client_tools.h"

#include <rpg_net_protocol_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_file_tools.h>

// update callback
static
gboolean
update_display_cb(gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::update_display_cb"));

  // sanity check(s)
  ACE_ASSERT(userData_in);
  IRC_Client_GUI_MessageHandler* messageHandler = ACE_static_cast(IRC_Client_GUI_MessageHandler*,
                                                                  userData_in);
  ACE_ASSERT(messageHandler);

  // *WARNING*: callbacks scheduled via g_idle_add need to be protected by
  // GDK_THREADS_ENTER/GDK_THREADS_LEAVE !
  GDK_THREADS_ENTER();

  messageHandler->update();

  GDK_THREADS_LEAVE();

  // remove us from the gtk_main loop idle routine...
  return FALSE;
}

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler(GtkTextView* view_in)
 : myView(view_in),
   myIsFirstNameListMsg(true),
   myParent(NULL)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT(myView);

  // init cb data
  myCBData.builder = NULL;
  myCBData.id.clear();
  myCBData.controller = NULL;

  // setup auto-scrolling
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(myView),
                               &iter);
  gtk_text_buffer_create_mark(gtk_text_view_get_buffer(myView),
                              ACE_TEXT_ALWAYS_CHAR("scroll"),
                              &iter,
                              TRUE);
}

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler(IRC_Client_GUI_Connection* connection_in,
                                                             RPG_Net_Protocol_IIRCControl* controller_in,
                                                             const std::string& id_in,
                                                             const std::string& UIFileDirectory_in,
                                                             GtkNotebook* notebook_in)
 : myView(NULL),
   myIsFirstNameListMsg(true),
   myParent(notebook_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT(connection_in);
  ACE_ASSERT(controller_in);
  ACE_ASSERT(!id_in.empty());
  if (!RPG_Common_File_Tools::isDirectory(UIFileDirectory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument (was: \"%s\"): not a directory, aborting\n"),
               UIFileDirectory_in.c_str()));

    return;
  } // end IF
  ACE_ASSERT(notebook_in);

  // init cb data
  myCBData.connection = connection_in;
  myCBData.builder = NULL;
  myCBData.id = id_in;
  myCBData.controller = controller_in;
  myCBData.channelModes = 0;

  // create new GtkBuilder
  myCBData.builder = gtk_builder_new();
  ACE_ASSERT(myCBData.builder);

  // init builder (load widget tree)
  std::string filename = UIFileDirectory_in;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += IRC_CLIENT_GUI_DEF_UI_CHANNEL_TAB_FILE;
  if (!RPG_Common_File_Tools::isReadable(filename))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid UI file (was: \"%s\"): not readable, aborting\n"),
               filename.c_str()));

    return;
  } // end IF
  GError* error = NULL;
  gtk_builder_add_from_file(myCBData.builder,
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

  // add new channel page to notebook (== server log)
  // retrieve (dummy) parent window
  GtkWindow* parent = GTK_WINDOW(gtk_builder_get_object(myCBData.builder,
                                                        ACE_TEXT_ALWAYS_CHAR("channel_tab_label_template")));
  ACE_ASSERT(parent);
  // retrieve channel tab label
  GtkHBox* channel_tab_label_hbox = GTK_HBOX(gtk_builder_get_object(myCBData.builder,
                                                                    ACE_TEXT_ALWAYS_CHAR("channel_tab_label_hbox")));
  ACE_ASSERT(channel_tab_label_hbox);
  g_object_ref(channel_tab_label_hbox);
  gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET(channel_tab_label_hbox));
  // set tab label
  GtkLabel* channel_tab_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                 ACE_TEXT_ALWAYS_CHAR("channel_tab_label")));
  ACE_ASSERT(channel_tab_label);
  std::string page_tab_label_string;
  if (!RPG_Net_Protocol_Tools::isValidIRCChannelName(myCBData.id))
  {
    // --> private conversation window, modify label accordingly
    page_tab_label_string = ACE_TEXT_ALWAYS_CHAR("[");
    page_tab_label_string += myCBData.id;
    page_tab_label_string += ACE_TEXT_ALWAYS_CHAR("]");

    // hide channel mode tab frame
    GtkFrame* channel_tab_mode_frame = GTK_FRAME(gtk_builder_get_object(myCBData.builder,
                                                                        ACE_TEXT_ALWAYS_CHAR("channel_tab_mode_frame")));
    ACE_ASSERT(channel_tab_mode_frame);
    gtk_widget_hide(GTK_WIDGET(channel_tab_mode_frame));
    // hide channel tab treeview
    GtkTreeView* channel_tab_treeview = GTK_TREE_VIEW(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("channel_tab_treeview")));
    ACE_ASSERT(channel_tab_treeview);
    gtk_widget_hide(GTK_WIDGET(channel_tab_treeview));

    // erase "topic" label
    GtkLabel* channel_tab_topic_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                         ACE_TEXT_ALWAYS_CHAR("channel_tab_topic_label")));
    ACE_ASSERT(channel_tab_topic_label);
    gtk_label_set_text(channel_tab_topic_label,
                       NULL);
  } // end IF
  else
    page_tab_label_string = myCBData.id;
  gtk_label_set_text(channel_tab_label,
                     page_tab_label_string.c_str());
  // retrieve (dummy) parent window
  parent = GTK_WINDOW(gtk_builder_get_object(myCBData.builder,
                                             ACE_TEXT_ALWAYS_CHAR("channel_tab_template")));
  ACE_ASSERT(parent);
  // retrieve channel tab
  GtkVBox* channel_tab_vbox = GTK_VBOX(gtk_builder_get_object(myCBData.builder,
                                                              ACE_TEXT_ALWAYS_CHAR("channel_tab_vbox")));
  ACE_ASSERT(channel_tab_vbox);
  g_object_ref(channel_tab_vbox);
  gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET(channel_tab_vbox));
  gint page_num = gtk_notebook_append_page(myParent,
                                           GTK_WIDGET(channel_tab_vbox),
                                           GTK_WIDGET(channel_tab_label_hbox));
  if (page_num == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_notebook_append_page(%@), aborting\n"),
               myParent));

    // clean up
    g_object_unref(channel_tab_label_hbox);
    g_object_unref(channel_tab_vbox);

    return;
  } // end IF
  // allow reordering
  gtk_notebook_set_tab_reorderable(myParent,
                                   GTK_WIDGET(channel_tab_vbox),
                                   TRUE);
  // activate new page
  gtk_notebook_set_current_page(myParent,
                                page_num);

  // clean up
  g_object_unref(channel_tab_label_hbox);
  g_object_unref(channel_tab_vbox);

  // setup auto-scrolling in textview
  myView = GTK_TEXT_VIEW(gtk_builder_get_object(myCBData.builder,
                                                ACE_TEXT_ALWAYS_CHAR("channel_tab_textview")));
  ACE_ASSERT(myView);
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(myView),
                               &iter);
  gtk_text_buffer_create_mark(gtk_text_view_get_buffer(myView),
                              ACE_TEXT_ALWAYS_CHAR("scroll"),
                              &iter,
                              TRUE);

  // enable multi-selection in treeview
  GtkTreeView* channel_tab_treeview = GTK_TREE_VIEW(gtk_builder_get_object(myCBData.builder,
                                                                           ACE_TEXT_ALWAYS_CHAR("channel_tab_treeview")));
  ACE_ASSERT(channel_tab_treeview);
  GtkTreeSelection* selection = gtk_tree_view_get_selection(channel_tab_treeview);
  ACE_ASSERT(selection);
  gtk_tree_selection_set_mode(selection,
                              GTK_SELECTION_MULTIPLE);

  // connect signal(s)
  GtkButton* channel_tab_label_button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                                          ACE_TEXT_ALWAYS_CHAR("channel_tab_label_button")));
  ACE_ASSERT(channel_tab_label_button);
  g_signal_connect(channel_tab_label_button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(part_clicked_cb),
                   &myCBData);
  // toggle buttons
  GtkToggleButton* toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                     ACE_TEXT_ALWAYS_CHAR("mode_key_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                    ACE_TEXT_ALWAYS_CHAR("mode_voice_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                    ACE_TEXT_ALWAYS_CHAR("mode_ban_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_userlimit_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_moderated_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_blockforeign_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_restricttopic_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_inviteonly_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_secret_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_private_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_operator_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(channel_mode_toggled_cb),
                   &myCBData);
  // topic label
  GtkEventBox* event_box = GTK_EVENT_BOX(gtk_builder_get_object(myCBData.builder,
                                                                ACE_TEXT_ALWAYS_CHAR("channel_tab_topic_label_eventbox")));
  ACE_ASSERT(event_box);
  g_signal_connect(event_box,
                   ACE_TEXT_ALWAYS_CHAR("button-press-event"),
                   G_CALLBACK(topic_clicked_cb),
                   &myCBData);
  // context menu in treeview
  g_signal_connect(channel_tab_treeview,
                   ACE_TEXT_ALWAYS_CHAR("button-press-event"),
                   G_CALLBACK(members_clicked_cb),
                   &myCBData);
}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

  // remove queued events
  while (g_idle_remove_by_data(this));

  // *NOTE*: the server log handler MUST NOT do this...
  if (myParent)
  {
//   // change active page ?
//   if (gtk_notebook_get_current_page(myParent) == myPageNum)
//     gtk_notebook_prev_page(myParent);

    GtkVBox* channel_tab_vbox = GTK_VBOX(gtk_builder_get_object(myCBData.builder,
                                                                ACE_TEXT_ALWAYS_CHAR("channel_tab_vbox")));
    ACE_ASSERT(channel_tab_vbox);
    if (gtk_notebook_page_num(myParent,
                              GTK_WIDGET(channel_tab_vbox)) > 1)
      gtk_notebook_prev_page(myParent);
    else
      gtk_notebook_next_page(myParent);

    // remove channel page from channel tabs notebook
    gtk_notebook_remove_page(myParent,
                             gtk_notebook_page_num(myParent,
                                                   GTK_WIDGET(channel_tab_vbox)));

    // clean up
    g_object_unref(myCBData.builder);
  } // end IF
}

void
IRC_Client_GUI_MessageHandler::queueForDisplay(const std::string& text_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SignalHandler::queueForDisplay"));

  {
    // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myDisplayQueue.push_back(text_in);
  } // end lock scope

  // trigger asnych update
  g_idle_add(update_display_cb, this);
}

void
IRC_Client_GUI_MessageHandler::update()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SignalHandler::update"));

  // always insert new text at the END of the buffer...
  ACE_ASSERT(myView);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("printing: \"%s\"\n"),
//              myDisplayQueue.front().c_str()));

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(myView),
                               &iter);

  {  // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // sanity check
    if (myDisplayQueue.empty())
      return; // nothing to do...

    // step1: convert text
    gchar* converted_text = IRC_Client_Tools::Locale2UTF8(myDisplayQueue.front());
    if (!converted_text)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert message text, aborting\n")));

      return;
    } // end IF

    // step2: display text
    gtk_text_buffer_insert(gtk_text_view_get_buffer(myView), &iter,
                           converted_text,
                           -1);
//   gtk_text_buffer_insert_at_cursor(myTargetBuffer,
//                                    message_text.c_str(),
//                                    message_text.size());

    // clean up
    g_free(converted_text);

    // step3: pop stack
    myDisplayQueue.pop_front();
  } // end lock scope

//   // get the new "end"...
//   gtk_text_buffer_get_end_iter(myTargetBuffer,
//                                &iter);
  // move the iterator to the beginning of line, so we don't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset(&iter, 0);

  // ...and place the mark at iter. The mark will stay there after we
  // insert some text at the end because it has right gravity
  GtkTextMark* mark = NULL;
  mark = gtk_text_buffer_get_mark(gtk_text_view_get_buffer(myView),
                                  ACE_TEXT_ALWAYS_CHAR("scroll"));
  gtk_text_buffer_move_mark(gtk_text_view_get_buffer(myView),
                            mark,
                            &iter);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen(myView,
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
  gdk_window_invalidate_rect(GTK_WIDGET(myView)->window,
                             NULL,
                             TRUE);
//   gdk_region_destroy(region);
//   gtk_widget_queue_draw(GTK_WIDGET(myView));
  gdk_window_process_updates(GTK_WIDGET(myView)->window, TRUE);
//   gdk_window_process_all_updates();
}

GtkWidget*
IRC_Client_GUI_MessageHandler::getTopLevelPageChild()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::getTopLevelPageChild"));

  // *WARNING*: the server log handler doesn't have a builder...
  if (!myParent)
  {
    // sanity check(s)
    ACE_ASSERT(myView);

    return gtk_widget_get_ancestor(GTK_WIDGET(myView),
                                   GTK_TYPE_WIDGET);
  } // end IF

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // retrieve button handle
  return GTK_WIDGET(gtk_builder_get_object(myCBData.builder,
                                           ACE_TEXT_ALWAYS_CHAR("channel_tab_vbox")));
}

// const std::string
// IRC_Client_GUI_MessageHandler::getChannel() const
// {
//   RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::getChannel"));
//
//   // sanity check: 'this' might be a private message handler !...
//   ACE_ASSERT(RPG_Net_Protocol_Tools::isValidIRCChannelName(myCBData.id));
//
//   return myCBData.id;
// }

void
IRC_Client_GUI_MessageHandler::setTopic(const std::string& topic_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::setTopic"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // retrieve label handle
  GtkLabel* channel_tab_topic_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                       ACE_TEXT_ALWAYS_CHAR("channel_tab_topic_label")));
  ACE_ASSERT(channel_tab_topic_label);
  gtk_label_set_text(channel_tab_topic_label,
                     topic_in.c_str());
}

void
IRC_Client_GUI_MessageHandler::setModes(const std::string& modes_in,
                                        const std::string& parameter_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::setModes"));

  ACE_UNUSED_ARG(parameter_in);

  RPG_Net_Protocol_Tools::merge(modes_in,
                                myCBData.channelModes);

  updateModeButtons();

  // enable channel modes ?
  // retrieve channel tab mode hbox handle
  GtkHBox* channel_tab_mode_hbox = GTK_HBOX(gtk_builder_get_object(myCBData.builder,
                                                                   ACE_TEXT_ALWAYS_CHAR("channel_tab_mode_hbox")));
  ACE_ASSERT(channel_tab_mode_hbox);
  gtk_widget_set_sensitive(GTK_WIDGET(channel_tab_mode_hbox),
                           myCBData.channelModes.test(CHANNELMODE_OPERATOR));
}

void
IRC_Client_GUI_MessageHandler::clear()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::clear"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // retrieve channel liststore handle
  GtkListStore* channel_liststore = NULL;
  channel_liststore = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                            ACE_TEXT_ALWAYS_CHAR("channel_liststore")));
  ACE_ASSERT(channel_liststore);

  // clear liststore
  gtk_list_store_clear(channel_liststore);
}

void
IRC_Client_GUI_MessageHandler::add(const std::string& nick_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::add"));

  // retrieve channel liststore handle
  GtkListStore* channel_liststore = NULL;
  channel_liststore = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                            ACE_TEXT_ALWAYS_CHAR("channel_liststore")));
  ACE_ASSERT(channel_liststore);

  // step1: convert text
  GtkTreeIter iter;
  gchar* converted_nick_string = IRC_Client_Tools::Locale2UTF8(nick_in);
  if (!converted_nick_string)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert nickname: \"%s\", aborting\n")));

    return;
  } // end IF

  // step2: append new (text) entry
  gtk_list_store_append(channel_liststore, &iter);
  gtk_list_store_set(channel_liststore, &iter,
                     0, converted_nick_string, // column 0
                     -1);

  // clean up
  g_free(converted_nick_string);
}

void
IRC_Client_GUI_MessageHandler::remove(const std::string& nick_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::remove"));

  // retrieve channel liststore handle
  GtkListStore* channel_liststore = NULL;
  channel_liststore = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                            ACE_TEXT_ALWAYS_CHAR("channel_liststore")));
  ACE_ASSERT(channel_liststore);

  // step1: convert text
  gchar* converted_nick_string = IRC_Client_Tools::Locale2UTF8(nick_in);
  if (!converted_nick_string)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert nickname: \"%s\", aborting\n")));

    return;
  } // end IF

  // step2: find matching entry
  GtkTreeIter current_iter;
//   GValue current_value;
  gchar* current_value_string = NULL;
  if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(channel_liststore),
                                     &current_iter))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_tree_model_get_iter_first(%@), aborting\n"),
               channel_liststore));

    // clean up
    g_free(converted_nick_string);

    return;
  } // end IF
  bool found_row = false;
  do
  {
    current_value_string = NULL;

    // retrieve value
//     gtk_tree_model_get_value(GTK_TREE_MODEL(channel_liststore),
//                              current_iter,
//                              0, &current_value);
    gtk_tree_model_get(GTK_TREE_MODEL(channel_liststore),
                       &current_iter,
                       0, &current_value_string,
                       -1);
    if (g_strcasecmp(converted_nick_string,
                     current_value_string) == 0)
    {
      found_row = true;

      // clean up
      g_free(current_value_string);

      break; // found value
    }

    // clean up
    g_free(current_value_string);
  } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(channel_liststore),
                                    &current_iter));

  // clean up
  g_free(converted_nick_string);

  if (found_row)
    gtk_list_store_remove(channel_liststore,
                          &current_iter);
  else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to remove nick (was: \"%s\"), aborting\n"),
               nick_in.c_str()));
}

void
IRC_Client_GUI_MessageHandler::append(const string_list_t& list_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::append"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  if (myIsFirstNameListMsg)
  {
    clear();

    myIsFirstNameListMsg = false;
  } // end IF

  // retrieve channel liststore handle
  GtkListStore* channel_liststore = NULL;
  channel_liststore = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                            ACE_TEXT_ALWAYS_CHAR("channel_liststore")));
  ACE_ASSERT(channel_liststore);

  GtkTreeIter iter;
  gchar* converted_nick_string = NULL;
  for (string_list_iterator_t iterator = list_in.begin();
       iterator != list_in.end();
       iterator++)
  {
    // step1: convert text
    converted_nick_string = IRC_Client_Tools::Locale2UTF8(*iterator);
    if (!converted_nick_string)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert nickname: \"%s\", aborting\n")));

      return;
    } // end IF

    // step2: append new (text) entry
    gtk_list_store_append(channel_liststore, &iter);
    gtk_list_store_set(channel_liststore, &iter,
                       0, converted_nick_string, // column 0
                       -1);

    // clean up
    g_free(converted_nick_string);
  } // end FOR
}

void
IRC_Client_GUI_MessageHandler::end()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::end"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  myIsFirstNameListMsg = true;

  // retrieve treeview handle
  GtkTreeView* channel_tab_treeview = NULL;
  channel_tab_treeview = GTK_TREE_VIEW(gtk_builder_get_object(myCBData.builder,
                                                              ACE_TEXT_ALWAYS_CHAR("channel_tab_treeview")));
  ACE_ASSERT(channel_tab_treeview);
  gtk_widget_set_sensitive(GTK_WIDGET(channel_tab_treeview), TRUE);
}

void
IRC_Client_GUI_MessageHandler::updateModeButtons()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::updateModeButtons"));

  // display (changed) channel modes
  GtkToggleButton* togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
      ACE_TEXT_ALWAYS_CHAR("mode_key_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_PASSWORD]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_voice_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_VOICE]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_ban_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_BAN]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_userlimit_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_USERLIMIT]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_moderated_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_MODERATED]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_blockforeign_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_BLOCKFOREIGNMSGS]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_restricttopic_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_RESTRICTEDTOPIC]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_inviteonly_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_INVITEONLY]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_secret_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_SECRET]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_private_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_PRIVATE]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                   ACE_TEXT_ALWAYS_CHAR("mode_operator_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.channelModes[CHANNELMODE_OPERATOR]);
}
