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

// update callback
static
gboolean
update_display_cb(gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::update_display_cb"));

  // sanity check(s)
  ACE_ASSERT(userData_in);
  IRC_Client_GUI_MessageHandler* messageHandler = ACE_static_cast(IRC_Client_GUI_MessageHandler*,
                                                                  userData_in);
  ACE_ASSERT(messageHandler);

  messageHandler->update();

  // remove us from the gtk_main loop idle routine...
  return FALSE;
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
void
part_clicked_cb(GtkWidget* button_in,
                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::part_clicked_cb"));

  ACE_UNUSED_ARG(button_in);

  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("part_clicked_cb...\n")));

  // sanity check(s)
  channel_cb_data_t* data = ACE_static_cast(channel_cb_data_t*,
                                            userData_in);
  ACE_ASSERT(data);

  try
  {
    data->controller->part(data->channel);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::part(), continuing\n")));
  }
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler(RPG_Net_Protocol_IIRCControl* controller_in,
                                                             const std::string& label_in,
                                                             const std::string& UIfile_in,
                                                             GtkNotebook* notebook_in,
                                                             const bool& isDefaultHandler_in)
 : myView(NULL),
   myIsFirstNameListMsg(true)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT(controller_in);
  ACE_ASSERT(notebook_in);

  // init cb data
  myCBData.builder = NULL;
  myCBData.channel.clear();
  myCBData.controller = controller_in;

  // create new GtkBuilder
  myCBData.builder = gtk_builder_new();
  ACE_ASSERT(myCBData.builder);

  // init builder (load widget tree)
  GError* error = NULL;
  gtk_builder_add_from_file(myCBData.builder,
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

    return;
  } // end IF

  // add new channel page to notebook (== server log)
  // retrieve channel tab label
  GtkHBox* channel_tab_label_hbox = NULL;
  if (isDefaultHandler_in)
  {
    channel_tab_label_hbox = GTK_HBOX(gtk_builder_get_object(myCBData.builder,
                                                             ACE_TEXT_ALWAYS_CHAR("channel_tab_label_hbox")));
    ACE_ASSERT(channel_tab_label_hbox);
  } // end IF
  // set tab label
  if (!label_in.empty())
  {
    GtkLabel* channel_tab_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                  ACE_TEXT_ALWAYS_CHAR("channel_tab_label")));
    ACE_ASSERT(channel_tab_label);
    gtk_label_set_text(channel_tab_label,
                      label_in.c_str());
  } // end IF
  // retrieve channel tab
  GtkFrame* channel_tab_frame = GTK_FRAME(gtk_builder_get_object(myCBData.builder,
                                                                 ACE_TEXT_ALWAYS_CHAR("channel_tab_frame")));
  ACE_ASSERT(channel_tab_frame);
  gint page_num = gtk_notebook_append_page(notebook_in,
                                           GTK_WIDGET(channel_tab_frame),
                                           GTK_WIDGET(channel_tab_label_hbox));
  if (page_num == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_notebook_append_page(), aborting\n")));

    return;
  } // end IF

  if (isDefaultHandler_in)
  {
    // connect signal(s)
    GtkButton* channel_tab_label_button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                                            ACE_TEXT_ALWAYS_CHAR("channel_tab_label_button")));
    ACE_ASSERT(channel_tab_label_button);
    g_signal_connect(channel_tab_label_button,
                     ACE_TEXT_ALWAYS_CHAR("clicked"),
                     G_CALLBACK(part_clicked_cb),
                     &myCBData);
  } // end IF

  // retrieve text view
  myView = GTK_TEXT_VIEW(gtk_builder_get_object(myCBData.builder,
                                                ACE_TEXT_ALWAYS_CHAR("channel_tab_textview")));
  ACE_ASSERT(myView);

  // setup auto-scrolling
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(myView),
                               &iter);
  gtk_text_buffer_create_mark(gtk_text_view_get_buffer(myView),
                              ACE_TEXT_ALWAYS_CHAR("scroll"),
                              &iter,
                              TRUE);
}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

  // clean up
  if (myCBData.builder)
    g_object_unref(myCBData.builder);
}

void
IRC_Client_GUI_MessageHandler::queueForDisplay(const std::string& text_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::queueForDisplay"));

  {
    // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myDisplayQueue.push_back(text_in);
  } // end lock scope

  GDK_THREADS_ENTER();
  // trigger asnych update
  g_idle_add(update_display_cb, this);

  GDK_THREADS_LEAVE();
}

void
IRC_Client_GUI_MessageHandler::update()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::update"));

  // always insert new text at the END of the buffer...
  ACE_ASSERT(myView);

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(gtk_text_view_get_buffer(myView),
                               &iter);

  {  // synch access
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // sanity check
    if (myDisplayQueue.empty())
      return; // nothing to do...

    // step1: convert text
    gchar* converted_text = NULL;
    GError* conversion_error = NULL;
    converted_text = g_locale_to_utf8(myDisplayQueue.front().c_str(), // text
                                      -1,   // \0-terminated
                                      NULL, // bytes read (don't care)
                                      NULL, // bytes written (don't care)
                                      &conversion_error); // return value: error
    if (conversion_error)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert message text: \"%s\", continuing\n"),
                 conversion_error->message));

      // clean up
      g_error_free(conversion_error);
    } // end IF
    else
    {
      // sanity check
      ACE_ASSERT(converted_text);

      // step2: display text
      gtk_text_buffer_insert(gtk_text_view_get_buffer(myView),
                             &iter,
                             converted_text,
                             -1);
  //   gtk_text_buffer_insert_at_cursor(myTargetBuffer,
  //                                    message_text.c_str(),
  //                                    message_text.size());

      // clean up
      g_free(converted_text);
    } // end ELSE

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
  gdk_window_process_updates(GTK_WIDGET(myView)->window, TRUE);
//   gdk_window_process_all_updates();
  gtk_widget_queue_draw(GTK_WIDGET(myView));
}

GtkWidget*
IRC_Client_GUI_MessageHandler::getTopLevel()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::getTopLevel"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // retrieve button handle
  return GTK_WIDGET(gtk_builder_get_object(myCBData.builder,
                                           ACE_TEXT_ALWAYS_CHAR("channel_tab_frame")));
}

void
IRC_Client_GUI_MessageHandler::setTopic(const std::string& topic_in)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::setTopic"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // retrieve label handle
  GtkLabel* channel_tab_topic_label = NULL;
  channel_tab_topic_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                             ACE_TEXT_ALWAYS_CHAR("channel_tab_topic_label")));
  ACE_ASSERT(channel_tab_topic_label);
  gtk_label_set_text(channel_tab_topic_label,
                     topic_in.c_str());
}

void
IRC_Client_GUI_MessageHandler::clearMembers()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::clearMembers"));

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
IRC_Client_GUI_MessageHandler::appendMembers(const string_list_t& list_in)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::appendMembers"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  if (myIsFirstNameListMsg)
  {
    clearMembers();

    myIsFirstNameListMsg = false;
  } // end IF

  // retrieve channel liststore handle
  GtkListStore* channel_liststore = NULL;
  channel_liststore = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                            ACE_TEXT_ALWAYS_CHAR("channel_liststore")));
  ACE_ASSERT(channel_liststore);

  GtkTreeIter iter;
  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  for (string_list_iterator_t iterator = list_in.begin();
       iterator != list_in.end();
       iterator++)
  {
    // step1: convert text
    converted_text = NULL;
    conversion_error = NULL;
    converted_text = g_locale_to_utf8((*iterator).c_str(), // text
                                      -1,   // \0-terminated
                                      NULL, // bytes read (don't care)
                                      NULL, // bytes written (don't care)
                                      &conversion_error); // return value: error
    if (conversion_error)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert nickname: \"%s\", continuing\n"),
                 conversion_error->message));

      // clean up
      g_error_free(conversion_error);
    } // end IF
    else
    {
      // sanity check
      ACE_ASSERT(converted_text);

      // step2: append new (text) entry
      gtk_list_store_append(channel_liststore, &iter);
      gtk_list_store_set(channel_liststore, &iter,
                         0, converted_text, // column 0
                         -1);

      // clean up
      g_free(converted_text);
    } // end ELSE
  } // end FOR
}

void
IRC_Client_GUI_MessageHandler::endMembers()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::endMembers"));

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
