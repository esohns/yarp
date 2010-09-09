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

#include <rpg_net_protocol_tools.h>

#include <gtk/gtk.h>

#include <string>

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

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler(GtkBuilder* builder_in)
 : myGtkInitialized(false),
   myBuilder(builder_in),
   myTargetView(NULL),
   myTargetBuffer(NULL),
   myIsFirstNameListMsg(true)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT(myBuilder);

  GDK_THREADS_ENTER();

  // step0: retrieve text view
  myTargetView = GTK_TEXT_VIEW(gtk_builder_get_object(myBuilder,
                                                      ACE_TEXT_ALWAYS_CHAR("textview")));
  ACE_ASSERT(myTargetView);
  // step1: retrieve target buffer
  myTargetBuffer = gtk_text_view_get_buffer(myTargetView);
  if (!myTargetBuffer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_text_view_get_buffer(): \"%m\", aborting\n")));

    return;
  } // end IF

  // step2: setup auto-scrolling
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(myTargetBuffer,
                               &iter);
  gtk_text_buffer_create_mark(myTargetBuffer,
                              ACE_TEXT_ALWAYS_CHAR("scroll"),
                              &iter,
                              TRUE);

  GDK_THREADS_LEAVE();
}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

}

void
IRC_Client_GUI_MessageHandler::start()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::start"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connected...\n")));
}

void
IRC_Client_GUI_MessageHandler::notify(const RPG_Net_Protocol_IRCMessage& message_in)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::notify"));

  // sanity check(s)
  ACE_ASSERT(myBuilder);
  if (!myGtkInitialized)
  {
//     gdk_threads_init();
    myGtkInitialized = true;
  } // end IF

  GDK_THREADS_ENTER();

  switch (message_in.command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      switch (message_in.command.numeric)
      {
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:
        {
          // retrieve button handle
          GtkButton* button = NULL;
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("join")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("register")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("disconnect")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_NOTOPIC:
        case RPG_Net_Protocol_IRC_Codes::RPL_TOPIC:
        {
          // re-retrieve channel name
          RPG_Net_Protocol_ParametersIterator_t iterator = message_in.params.begin();
          iterator++;
          std::string channel_label = *iterator;

          if (message_in.command.numeric != RPG_Net_Protocol_IRC_Codes::RPL_NOTOPIC)
          {
            channel_label += ACE_TEXT_ALWAYS_CHAR(": ");
            channel_label += message_in.params.back();
          } // end IF

          // retrieve label handle
          GtkLabel* label = NULL;
          label = GTK_LABEL(gtk_builder_get_object(myBuilder,
                                                   ACE_TEXT_ALWAYS_CHAR("channel_label")));
          ACE_ASSERT(label);
          gtk_label_set_text(label,
                             channel_label.c_str());

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_NAMREPLY:
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFNAMES:
        {
          if (message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::RPL_ENDOFNAMES)
          {
            // retrieve treeview handle
            GtkTreeView* treeview = NULL;
            treeview = GTK_TREE_VIEW(gtk_builder_get_object(myBuilder,
                                                            ACE_TEXT_ALWAYS_CHAR("treeview")));
            ACE_ASSERT(treeview);
            gtk_widget_set_sensitive(GTK_WIDGET(treeview), TRUE);

            // clean up
            myIsFirstNameListMsg = true;

            // done
            break;
          } // end IF

          // retrieve liststore handle
          GtkListStore* liststore = NULL;
          liststore = GTK_LIST_STORE(gtk_builder_get_object(myBuilder,
                                                            ACE_TEXT_ALWAYS_CHAR("liststore")));
          ACE_ASSERT(liststore);

          if (myIsFirstNameListMsg)
          {
            // clear liststore
            gtk_list_store_clear(liststore);

            myIsFirstNameListMsg = false;
          } // end IF

          // bisect (WS-separated) nicknames from the final parameter string
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("bisecting nicknames: \"%s\"...\n"),
//                      message_in.params.back().c_str()));

          std::string::size_type current_position = 0;
          std::string::size_type last_position = 0;
          std::string nick;
          GtkTreeIter iter;
          gchar* converted_text = NULL;
          GError* conversion_error = NULL;
          do
          {
            current_position = message_in.params.back().find(' ', last_position);

            nick = message_in.params.back().substr(last_position,
                                                   (((current_position == std::string::npos) ? message_in.params.back().size()
                                                                                             : current_position) - last_position));

            // step1: convert text
            converted_text = NULL;
            conversion_error = NULL;
            converted_text = g_locale_to_utf8(nick.c_str(), // text
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
              gtk_list_store_append(liststore, &iter);
              gtk_list_store_set(liststore, &iter,
                                 0, converted_text, // column 0
                                 -1);

              // clean up
              g_free(converted_text);
            } // end ELSE

            // step3: advance to next nickname
            last_position = current_position + 1;
          } while (current_position != std::string::npos);

          break;
        }
        default:
        {
//           ACE_DEBUG((LM_WARNING,
//                      ACE_TEXT("received (numeric) command/reply: \"%s\" (%u), continuing\n"),
//                      RPG_Net_Protocol_Tools::IRCCode2String(message_in.command.numeric).c_str(),
//                      message_in.command.numeric));

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      switch (RPG_Net_Protocol_Tools::IRCCommandString2Type(*message_in.command.string))
      {
        case RPG_Net_Protocol_IRCMessage::JOIN:
        {
          // sanity check(s)
          ACE_ASSERT(myBuilder);

          // clear text buffer
          GtkTextIter start, end;
          gtk_text_buffer_get_bounds(myTargetBuffer,
                                     &start,
                                     &end);
          gtk_text_buffer_delete(myTargetBuffer,
                                 &start,
                                 &end);

          // retrieve entry handle
          GtkEntry* entry = NULL;
          entry = GTK_ENTRY(gtk_builder_get_object(myBuilder,
                                                   ACE_TEXT_ALWAYS_CHAR("entry")));
          ACE_ASSERT(entry);
          gtk_widget_set_sensitive(GTK_WIDGET(entry), TRUE);
          // retrieve button handle
          GtkButton* button = NULL;
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("join")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("part")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("send")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
          // retrieve label handle
          GtkLabel* label = NULL;
          label = GTK_LABEL(gtk_builder_get_object(myBuilder,
                                                   ACE_TEXT_ALWAYS_CHAR("channel_label")));
          ACE_ASSERT(label);
          gtk_label_set_text(label,
                             message_in.params.front().c_str());

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PART:
        {
          // sanity check(s)
          ACE_ASSERT(myBuilder);

          // retrieve entry handle
          GtkEntry* entry = NULL;
          entry = GTK_ENTRY(gtk_builder_get_object(myBuilder,
                                                   ACE_TEXT_ALWAYS_CHAR("entry")));
          ACE_ASSERT(entry);
          gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);
          // retrieve button handle
          GtkButton* button = NULL;
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("join")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("part")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("send")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
          // retrieve label handle
          GtkLabel* label = NULL;
          label = GTK_LABEL(gtk_builder_get_object(myBuilder,
                            ACE_TEXT_ALWAYS_CHAR("channel_label")));
          ACE_ASSERT(label);
          gtk_label_set_text(label,
                             IRC_CLIENT_GUI_DEF_CHANNEL_LABEL_TEXT);
          // retrieve liststore handle
          GtkListStore* liststore = NULL;
          liststore = GTK_LIST_STORE(gtk_builder_get_object(myBuilder,
                                     ACE_TEXT_ALWAYS_CHAR("liststore")));
          ACE_ASSERT(liststore);
          // clear liststore
          gtk_list_store_clear(liststore);
          // retrieve treeview handle
          GtkTreeView* treeview = NULL;
          treeview = GTK_TREE_VIEW(gtk_builder_get_object(myBuilder,
                                   ACE_TEXT_ALWAYS_CHAR("treeview")));
          ACE_ASSERT(treeview);
          gtk_widget_set_sensitive(GTK_WIDGET(treeview), FALSE);

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PRIVMSG:
        case RPG_Net_Protocol_IRCMessage::NOTICE:
        case RPG_Net_Protocol_IRCMessage::ERROR:
        {
          std::string message_text;
          if (!message_in.prefix.origin.empty())
          {
            message_text += ACE_TEXT("<");
            message_text += message_in.prefix.origin;
            message_text += ACE_TEXT("> ");
          } // end IF
          message_text += message_in.params.back();
          message_text += ACE_TEXT_ALWAYS_CHAR("\n");
          queueForDisplay(message_text);

          break;
        }
        case RPG_Net_Protocol_IRCMessage::MODE:
        case RPG_Net_Protocol_IRCMessage::PING:
        {

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid/unknown command (was: \"%s\"), aborting\n"),
                     message_in.command.string->c_str()));

          message_in.dump_state();

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command type (was: %u), aborting\n"),
                 message_in.command.discriminator));

      break;
    }
  } // end SWITCH

  GDK_THREADS_LEAVE();
}

void
IRC_Client_GUI_MessageHandler::end()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::end"));

  // sanity check(s)
  ACE_ASSERT(myBuilder);

  GDK_THREADS_ENTER();

  // retrieve label handle
  GtkLabel* label = NULL;
  label = GTK_LABEL(gtk_builder_get_object(myBuilder,
                                           ACE_TEXT_ALWAYS_CHAR("channel_label")));
  ACE_ASSERT(label);
  gtk_label_set_text(label,
                     IRC_CLIENT_GUI_DEF_CHANNEL_LABEL_TEXT);
  // retrieve liststore handle
  GtkListStore* liststore = NULL;
  liststore = GTK_LIST_STORE(gtk_builder_get_object(myBuilder,
                                                    ACE_TEXT_ALWAYS_CHAR("liststore")));
  ACE_ASSERT(liststore);
  // clear liststore
  gtk_list_store_clear(liststore);
  // retrieve treeview handle
  GtkTreeView* treeview = NULL;
  treeview = GTK_TREE_VIEW(gtk_builder_get_object(myBuilder,
                                                  ACE_TEXT_ALWAYS_CHAR("treeview")));
  ACE_ASSERT(treeview);
  gtk_widget_set_sensitive(GTK_WIDGET(treeview), FALSE);
  // retrieve entry handle
  GtkEntry* entry = NULL;
  entry = GTK_ENTRY(gtk_builder_get_object(myBuilder,
                                           ACE_TEXT_ALWAYS_CHAR("entry")));
  ACE_ASSERT(entry);
  gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);
  // retrieve button handle
  GtkButton* button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                        ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  // retrieve button handle
  button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                             ACE_TEXT_ALWAYS_CHAR("part")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  // retrieve button handle
  button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                             ACE_TEXT_ALWAYS_CHAR("send")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
    // retrieve button handle
  button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                             ACE_TEXT_ALWAYS_CHAR("register")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  // retrieve button handle
  button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                             ACE_TEXT_ALWAYS_CHAR("disconnect")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  GDK_THREADS_LEAVE();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection lost...\n")));
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
  ACE_ASSERT(myTargetBuffer);

  GDK_THREADS_ENTER();

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(myTargetBuffer,
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
      gtk_text_buffer_insert(myTargetBuffer,
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
  mark = gtk_text_buffer_get_mark(myTargetBuffer,
                                  ACE_TEXT_ALWAYS_CHAR("scroll"));
  gtk_text_buffer_move_mark(myTargetBuffer,
                            mark,
                            &iter);

  // scroll the mark onscreen
  ACE_ASSERT(myTargetView);
  gtk_text_view_scroll_mark_onscreen(myTargetView,
                                     mark);

  // redraw our window...
  // sanity check(s)
  ACE_ASSERT(myBuilder);
//   GtkScrolledWindow* scrolledwindow = NULL;
  GtkWindow* dialog = NULL;
  dialog = GTK_WINDOW(gtk_builder_get_object(myBuilder,
                                       ACE_TEXT_ALWAYS_CHAR("dialog")));
  ACE_ASSERT(dialog);
//   GdkRegion* region = NULL;
//   region = gdk_drawable_get_clip_region(GTK_WIDGET(dialog)->window);
//   ACE_ASSERT(region);
//   gdk_window_invalidate_region(GTK_WIDGET(dialog)->window,
//                                region,
//                                TRUE);
  gdk_window_invalidate_rect(GTK_WIDGET(dialog)->window,
                             NULL,
                             TRUE);
//   gdk_region_destroy(region);
  gdk_window_process_updates(GTK_WIDGET(dialog)->window, TRUE);
//   gdk_window_process_all_updates();
  gtk_widget_queue_draw(GTK_WIDGET(dialog));

  GDK_THREADS_LEAVE();
}
