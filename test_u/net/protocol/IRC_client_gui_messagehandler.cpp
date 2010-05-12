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

#include <rpg_net_protocol_tools.h>

#include <gtk/gtk.h>

#include <string>

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler(GtkBuilder* builder_in,
                                                             ACE_Thread_Mutex& lock_in)
 : myBuilder(builder_in),
   myLock(lock_in),
   myTargetView(NULL),
   myTargetBuffer(NULL)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

  // sanity check(s)
  ACE_ASSERT(myBuilder);

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
}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

}

void
IRC_Client_GUI_MessageHandler::notify(const RPG_Net_Protocol_IRCMessage& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::notify"));

  // sanity check(s)
  ACE_ASSERT(myTargetBuffer);

  switch (message_in.command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      switch (message_in.command.numeric)
      {
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:
        {
          // sanity check(s)
          ACE_ASSERT(myBuilder);
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
          // retrieve button handle
          GtkButton* button = NULL;
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("send")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("join")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
          button = GTK_BUTTON(gtk_builder_get_object(myBuilder,
                                                     ACE_TEXT_ALWAYS_CHAR("part")));
          ACE_ASSERT(button);
          gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PART:
        {
          // sanity check(s)
          ACE_ASSERT(myBuilder);
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
          insertText(message_text);

          break;
        }
        case RPG_Net_Protocol_IRCMessage::MODE:
        case RPG_Net_Protocol_IRCMessage::PING:
        {

          break;
        }
        default:
        {
          // debug info
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
}

void
IRC_Client_GUI_MessageHandler::insertText(const std::string& text_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::insertText"));

  // synch access to buffer
  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // always insert new text at the END of the buffer...
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(myTargetBuffer,
                               &iter);

  // *NOTE*: iter should be updated...
  gtk_text_buffer_insert(myTargetBuffer,
                         &iter,
                         text_in.c_str(),
                         -1);

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
  gtk_text_view_scroll_mark_onscreen(myTargetView,
                                     mark);

//   gtk_text_buffer_insert_at_cursor(myTargetBuffer,
//                                    message_text.c_str(),
//                                    message_text.size());
}
