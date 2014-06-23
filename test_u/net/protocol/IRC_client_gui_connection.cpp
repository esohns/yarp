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

#include "IRC_client_gui_connection.h"

#include <sstream>

#include "rpg_common_macros.h"
#include "rpg_common_file_tools.h"

#include "rpg_net_protocol_tools.h"

#include "rpg_client_ui_tools.h"

#include "IRC_client_gui_defines.h"
#include "IRC_client_gui_messagehandler.h"
#include "IRC_client_gui_callbacks.h"

IRC_Client_GUI_Connection::IRC_Client_GUI_Connection(GtkBuilder* builder_in,
                                                     RPG_Net_Protocol_IIRCControl* controller_in,
                                                     ACE_Thread_Mutex* lock_in,
                                                     connections_t* connections_in,
                                                     const std::string& label_in,
                                                     const std::string& UIFileDirectory_in,
                                                     GtkNotebook* parent_in)
 : //myUIFileDirectory(),
   //myCBData(),
   myIsFirstUsersMsg(true),
	 //myLock(),
	 //myMessageHandlers(),
   myParent(parent_in),
   myContextID(0)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::IRC_Client_GUI_Connection"));

  // sanity check(s)
  ACE_ASSERT(builder_in);
  ACE_ASSERT(parent_in);
  ACE_ASSERT(controller_in);
  ACE_ASSERT(lock_in);
  ACE_ASSERT(connections_in);
  if (!RPG_Common_File_Tools::isDirectory(UIFileDirectory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument (was: \"%s\"): not a directory, aborting\n"),
               UIFileDirectory_in.c_str()));

    return;
  } // end IF

  myUIFileDirectory = UIFileDirectory_in;

  // init cb data
  myCBData.mainBuilder = builder_in;
  myCBData.builder = NULL;
//   myCBData.nick.clear(); // cannot set this now...
  myCBData.userModes.reset();
  myCBData.controller = controller_in;
  myCBData.connectionsLock = lock_in;
  myCBData.connections = connections_in;
  myCBData.connection = this;
  myCBData.away = false;

  // create new GtkBuilder
  myCBData.builder = gtk_builder_new();
  ACE_ASSERT(myCBData.builder);

  // init builder (load widget tree)
  std::string filename = myUIFileDirectory;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += IRC_CLIENT_GUI_DEF_UI_SERVER_PAGE_FILE;
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

  // generate context ID
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(*myCBData.connectionsLock);

    // retrieve status bar
    GtkStatusbar* main_statusbar = GTK_STATUSBAR(gtk_builder_get_object(myCBData.mainBuilder,
                                                                        ACE_TEXT_ALWAYS_CHAR("main_statusbar")));
    ACE_ASSERT(main_statusbar);

    myContextID = gtk_statusbar_get_context_id(main_statusbar,
                                               IRC_CLIENT_GUI_DEF_CONTEXT_DESCRIPTION);
  } // end lock scope

  // add new server page to notebook
  // retrieve (dummy) parent window
  GtkWindow* parent = GTK_WINDOW(gtk_builder_get_object(myCBData.builder,
                                                        ACE_TEXT_ALWAYS_CHAR("server_tab_label_template")));
  ACE_ASSERT(parent);
  // retrieve server tab label
  GtkHBox* server_tab_label_hbox = GTK_HBOX(gtk_builder_get_object(myCBData.builder,
                                                                   ACE_TEXT_ALWAYS_CHAR("server_tab_label_hbox")));
  ACE_ASSERT(server_tab_label_hbox);
  g_object_ref(server_tab_label_hbox);
  gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET(server_tab_label_hbox));
  // set tab label
  GtkLabel* server_tab_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                ACE_TEXT_ALWAYS_CHAR("server_tab_label")));
  ACE_ASSERT(server_tab_label);
  // *TODO*: convert to UTF8 ?
  gtk_label_set_text(server_tab_label,
                     label_in.c_str());
  // retrieve (dummy) parent window
  parent = GTK_WINDOW(gtk_builder_get_object(myCBData.builder,
                                             ACE_TEXT_ALWAYS_CHAR("server_tab_template")));
  ACE_ASSERT(parent);
  // retrieve server tab
  GtkVBox* server_tab_vbox = GTK_VBOX(gtk_builder_get_object(myCBData.builder,
                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_vbox")));
  ACE_ASSERT(server_tab_vbox);
  g_object_ref(server_tab_vbox);
  gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET(server_tab_vbox));
  gint page_num = gtk_notebook_append_page(myParent,
                                           GTK_WIDGET(server_tab_vbox),
                                           GTK_WIDGET(server_tab_label_hbox));
  if (page_num == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_notebook_append_page(%@), aborting\n"),
               myParent));

    // clean up
    g_object_unref(server_tab_label_hbox);
    g_object_unref(server_tab_vbox);

    return;
  } // end IF
  // allow reordering
  gtk_notebook_set_tab_reorderable(myParent,
                                   GTK_WIDGET(server_tab_vbox),
                                   TRUE);
  // activate new page
  gtk_notebook_set_current_page(myParent,
                                page_num);

  // clean up
  g_object_unref(server_tab_label_hbox);
  g_object_unref(server_tab_vbox);

  // retrieve server tab channels store
  GtkListStore* server_tab_channels_store = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                                                  ACE_TEXT_ALWAYS_CHAR("server_tab_channels_store")));
  ACE_ASSERT(server_tab_channels_store);
  // make it sort the channels by #members...
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(server_tab_channels_store),
                                       1, GTK_SORT_DESCENDING);

  // connect signal(s)
  GtkButton* button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                        ACE_TEXT_ALWAYS_CHAR("server_tab_label_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(disconnect_clicked_cb),
                   &myCBData);
  GtkEntry* entry = GTK_ENTRY(gtk_builder_get_object(myCBData.builder,
                                                     ACE_TEXT_ALWAYS_CHAR("server_tab_users_nick_entry")));
  ACE_ASSERT(entry);
  g_signal_connect(entry,
                   ACE_TEXT_ALWAYS_CHAR("focus-in-event"),
                   G_CALLBACK(nick_entry_kb_focused_cb),
                   &myCBData);
  button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                             ACE_TEXT_ALWAYS_CHAR("server_tab_users_nick_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(change_clicked_cb),
                   &myCBData);
  GtkComboBox* combobox = GTK_COMBO_BOX(gtk_builder_get_object(myCBData.builder,
                                                               ACE_TEXT_ALWAYS_CHAR("server_tab_users_combobox")));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(usersbox_changed_cb),
                   &myCBData);
  button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                             ACE_TEXT_ALWAYS_CHAR("server_tab_users_refresh_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(refresh_users_clicked_cb),
                   &myCBData);
  entry = GTK_ENTRY(gtk_builder_get_object(myCBData.builder,
                                           ACE_TEXT_ALWAYS_CHAR("server_tab_channel_entry")));
  ACE_ASSERT(entry);
  g_signal_connect(entry,
                   ACE_TEXT_ALWAYS_CHAR("focus-in-event"),
                   G_CALLBACK(channel_entry_kb_focused_cb),
                   &myCBData);
  button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                             ACE_TEXT_ALWAYS_CHAR("server_tab_join_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(join_clicked_cb),
                   &myCBData);
  combobox = GTK_COMBO_BOX(gtk_builder_get_object(myCBData.builder,
                                                  ACE_TEXT_ALWAYS_CHAR("server_tab_channels_combobox")));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(channelbox_changed_cb),
                   &myCBData);
  button = GTK_BUTTON(gtk_builder_get_object(myCBData.builder,
                                             ACE_TEXT_ALWAYS_CHAR("server_tab_channels_refresh_button")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(refresh_channels_clicked_cb),
                   &myCBData);
  // toggle buttons
  GtkToggleButton* toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                                            ACE_TEXT_ALWAYS_CHAR("mode_operator_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_localoperator_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_restricted_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_away_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_wallops_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_notices_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  toggle_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                           ACE_TEXT_ALWAYS_CHAR("mode_invisible_togglebutton")));
  ACE_ASSERT(toggle_button);
  g_signal_connect(toggle_button,
                   ACE_TEXT_ALWAYS_CHAR("toggled"),
                   G_CALLBACK(user_mode_toggled_cb),
                   &myCBData);
  GtkAction* action_away = GTK_ACTION(gtk_builder_get_object(myCBData.builder,
                                                             ACE_TEXT_ALWAYS_CHAR("action_away")));
  ACE_ASSERT(action_away);
  g_signal_connect(action_away,
                   ACE_TEXT_ALWAYS_CHAR("activate"),
                   G_CALLBACK(action_away_cb),
                   &myCBData);
  // retrieve channel tabs
  GtkNotebook* server_tab_channel_tabs = GTK_NOTEBOOK(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_channel_tabs")));
  ACE_ASSERT(server_tab_channel_tabs);
  g_signal_connect(server_tab_channel_tabs,
                   ACE_TEXT_ALWAYS_CHAR("switch-page"),
                   G_CALLBACK(switch_channel_cb),
                   &myCBData);

  // retrieve server log tab child
  GtkScrolledWindow* server_tab_log_scrolledwindow = GTK_SCROLLED_WINDOW(gtk_builder_get_object(myCBData.builder,
                                                                                                ACE_TEXT_ALWAYS_CHAR("server_tab_log_scrolledwindow")));
  ACE_ASSERT(server_tab_log_scrolledwindow);
  // disallow reordering the server log tab
  gtk_notebook_set_tab_reorderable(server_tab_channel_tabs,
                                   GTK_WIDGET(server_tab_log_scrolledwindow),
                                   FALSE);

  // create default IRC_Client_GUI_MessageHandler (== server log)
  // retrieve server log textview
  GtkTextView* server_tab_log_textview = GTK_TEXT_VIEW(gtk_builder_get_object(myCBData.builder,
                                                                              ACE_TEXT_ALWAYS_CHAR("server_tab_log_textview")));
  ACE_ASSERT(server_tab_log_textview);
  IRC_Client_GUI_MessageHandler* message_handler = NULL;
	ACE_NEW_NORETURN(message_handler,
		               IRC_Client_GUI_MessageHandler(server_tab_log_textview));
  if (!message_handler)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate IRC_Client_GUI_MessageHandler, aborting\n")));

    return;
  } // end IF

  // *NOTE*: the empty channel name string denotes the log handler !
  // *NOTE*: no updates yet --> no need for locking
  // *NOTE*: in theory, there is a race condition as the user may start
  // interacting with the new UI elements by now - as GTK will draw the new elements
  // only after we return, this is not really a problem...
  myMessageHandlers.insert(std::make_pair(std::string(), message_handler));

  // subscribe to updates from the controller
  try
  {
    myCBData.controller->subscribe(this);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::subscribe(%@), continuing\n"),
               this));
  }
}

IRC_Client_GUI_Connection::~IRC_Client_GUI_Connection()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::~IRC_Client_GUI_Connection"));

  // unsubscribe to updates from the controller
  try
  {
    myCBData.controller->unsubscribe(this);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_Protocol_IIRCControl::unsubscribe(%@), continuing\n"),
               this));
  }

  gdk_threads_enter();

  std::string server_tab_label_text;
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    // clean up message handlers
    for (message_handlers_iterator_t iterator = myMessageHandlers.begin();
         iterator != myMessageHandlers.end();
         iterator++)
      delete (*iterator).second;
  } // end lock scope

  GtkLabel* server_tab_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                ACE_TEXT_ALWAYS_CHAR("server_tab_label")));
  ACE_ASSERT(server_tab_label);
  // *TODO*: convert to locale ?
  server_tab_label_text = gtk_label_get_text(server_tab_label);

  // remove server page from parent notebook
  GtkVBox* server_tab_vbox = GTK_VBOX(gtk_builder_get_object(myCBData.builder,
                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_vbox")));
  ACE_ASSERT(server_tab_vbox);
  if (gtk_notebook_get_n_pages(myParent) > 1)
  {
    if (gtk_notebook_page_num(myParent,
                              GTK_WIDGET(server_tab_vbox)) > 0)
      gtk_notebook_prev_page(myParent);
    else
      gtk_notebook_next_page(myParent);
  } // end IF
  gtk_notebook_remove_page(myParent,
                           gtk_notebook_page_num(myParent,
                                                 GTK_WIDGET(server_tab_vbox)));

  // clean up
  g_object_unref(myCBData.builder);

  gdk_threads_leave();

  // remove ourselves from the connection list
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(*myCBData.connectionsLock);

    connections_iterator_t iterator = myCBData.connections->find(server_tab_label_text);
    if (iterator != myCBData.connections->end())
      myCBData.connections->erase(iterator);
  } // end lock scope
}

void
IRC_Client_GUI_Connection::start()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::start"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("connected...\n")));
}

void
IRC_Client_GUI_Connection::notify(const RPG_Net_Protocol_IRCMessage& message_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::notify"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  switch (message_in.command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      switch (message_in.command.numeric)
      {
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:          //   1
        {
          // *NOTE*: this is the first message in any connection !

          // remember nickname
          myCBData.nickname = message_in.params.front();

          gdk_threads_enter();

          // --> display (starting) nickname
          // set server tab nickname label
          GtkLabel* server_tab_nick_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_nick_label")));
          ACE_ASSERT(server_tab_nick_label);
          // --> see Pango Text Attribute Markup Language...
          std::string nickname_string = ACE_TEXT_ALWAYS_CHAR("nickname: <b>");
          nickname_string += message_in.params.front();
          nickname_string += ACE_TEXT_ALWAYS_CHAR("</b>");
          gtk_label_set_markup(server_tab_nick_label,
                               nickname_string.c_str());

          // retrieve button handle
          GtkHBox* hbox = GTK_HBOX(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("server_tab_nick_chan_hbox")));
          ACE_ASSERT(hbox);
          gtk_widget_set_sensitive(GTK_WIDGET(hbox), TRUE);
          hbox = GTK_HBOX(gtk_builder_get_object(myCBData.builder,
                                                 ACE_TEXT_ALWAYS_CHAR("server_tab_user_modes_hbox")));
          ACE_ASSERT(hbox);
          gtk_widget_set_sensitive(GTK_WIDGET(hbox), TRUE);

          gdk_threads_leave();

          // *WARNING*: falls through !
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURHOST:         //   2
        case RPG_Net_Protocol_IRC_Codes::RPL_CREATED:          //   3
        case RPG_Net_Protocol_IRC_Codes::RPL_MYINFO:           //   4
        case RPG_Net_Protocol_IRC_Codes::RPL_PROTOCTL:         //   5
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURID:           //  42
        case RPG_Net_Protocol_IRC_Codes::RPL_STATSDLINE:       // 250
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCLIENT:      // 251
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSEROP:          // 252
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERUNKNOWN:     // 253
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCHANNELS:    // 254
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERME:          // 255
        case RPG_Net_Protocol_IRC_Codes::RPL_TRYAGAIN:         // 263
        case RPG_Net_Protocol_IRC_Codes::RPL_LOCALUSERS:       // 265
        case RPG_Net_Protocol_IRC_Codes::RPL_GLOBALUSERS:      // 266
        case RPG_Net_Protocol_IRC_Codes::RPL_INVITING:         // 341
        {
          gdk_threads_enter();

          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_USERHOST:         // 302
        {
          // bisect (WS-separated) userhost records from the final parameter

//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("bisecting records: \"%s\"...\n"),
//                      message_in.params.back().c_str()));

          std::string::size_type current_position = 0;
          std::string::size_type last_position = 0;
          std::string record;
          string_list_t list;
          do
          {
            current_position = message_in.params.back().find(' ', last_position);

            record = message_in.params.back().substr(last_position,
                                                     (((current_position == std::string::npos) ? message_in.params.back().size()
                                                                                               : current_position) - last_position));

            // check whether the record is empty
            if (!record.empty())
              list.push_back(record);

            // advance
            last_position = current_position + 1;
          } while (current_position != std::string::npos);

          std::string message_string;
          gdk_threads_enter();

          for (string_list_const_iterator_t iterator = list.begin();
               iterator != list.end();
               iterator++)
          {
            current_position = (*iterator).find('=', 0);
            if (current_position == std::string::npos)
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid USERHOST record: \"%s\", continuing\n"),
                                  (*iterator).c_str()));

              continue;
            } // end IF

            message_string = (*iterator).substr(0, current_position);
            message_string += ACE_TEXT_ALWAYS_CHAR(" is \"");

            // advance
            last_position = current_position;

            current_position = (*iterator).find('@', current_position);
            if (current_position == std::string::npos)
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid USERHOST record: \"%s\", continuing\n"),
                         (*iterator).c_str()));

              continue;
            } // end IF
            message_string += (*iterator).substr(last_position + 1, (current_position - last_position - 1));
            message_string += ACE_TEXT_ALWAYS_CHAR("\" on \"");
            message_string += (*iterator).substr(current_position + 1);
            message_string += ACE_TEXT_ALWAYS_CHAR("\"\n");

            log(message_string);
          } // end FOR

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_UNAWAY:           // 305
        {
          gdk_threads_enter();

          // *WARNING*: needs the lock protection, otherwise
          // there is a race...
          myCBData.away = false;

          // retrieve server tab tools togglebutton
          GtkToggleButton* server_tab_tools_togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                                                                    ACE_TEXT_ALWAYS_CHAR("server_tab_tools_togglebutton")));
          ACE_ASSERT(server_tab_tools_togglebutton);
          gtk_toggle_button_set_active(server_tab_tools_togglebutton, FALSE);

          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_NOWAWAY:          // 306
        {
          gdk_threads_enter();

          // *WARNING*: needs the lock protection, otherwise
          // there is a race...
          myCBData.away = true;

          // retrieve server tab tools togglebutton
          GtkToggleButton* server_tab_tools_togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                                                                    ACE_TEXT_ALWAYS_CHAR("server_tab_tools_togglebutton")));
          ACE_ASSERT(server_tab_tools_togglebutton);
          gtk_toggle_button_set_active(server_tab_tools_togglebutton, TRUE);

          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWHO:         // 315
        {
          gdk_threads_enter();

          // retrieve server tab users store
          GtkComboBox* server_tab_users_combobox = GTK_COMBO_BOX(gtk_builder_get_object(myCBData.builder,
              ACE_TEXT_ALWAYS_CHAR("server_tab_users_combobox")));
          ACE_ASSERT(server_tab_users_combobox);

          myIsFirstUsersMsg = true;

          gtk_widget_set_sensitive(GTK_WIDGET(server_tab_users_combobox), TRUE);

          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_LISTSTART:        // 321
        {
          gdk_threads_enter();

          // retrieve server tab channels store
          GtkListStore* server_tab_channels_store = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                                                          ACE_TEXT_ALWAYS_CHAR("server_tab_channels_store")));
          ACE_ASSERT(server_tab_channels_store);

          // clear the store
          gtk_list_store_clear(server_tab_channels_store);

          gdk_threads_leave();

          // *WARNING*: falls through !
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_LISTEND:          // 323
        {
          gdk_threads_enter();

          // retrieve server tab channels store
          GtkComboBox* server_tab_channels_combobox = GTK_COMBO_BOX(gtk_builder_get_object(myCBData.builder,
                                                                                           ACE_TEXT_ALWAYS_CHAR("server_tab_channels_combobox")));
          ACE_ASSERT(server_tab_channels_combobox);
          gtk_widget_set_sensitive(GTK_WIDGET(server_tab_channels_combobox), TRUE);

          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_LIST:             // 322
        {
          gdk_threads_enter();

          // retrieve server tab channels store
          GtkListStore* server_tab_channels_store = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                                                          ACE_TEXT_ALWAYS_CHAR("server_tab_channels_store")));
          ACE_ASSERT(server_tab_channels_store);

          // convert <# visible>
          RPG_Net_Protocol_ParametersIterator_t param_iterator = message_in.params.begin();
          ACE_ASSERT(message_in.params.size() >= 3);
          std::advance(param_iterator, 2);
          std::stringstream converter;
          guint num_members = 0;
          converter << *param_iterator;
          converter >> num_members;
          param_iterator--;

          // convert text
          gchar* converted_text = RPG_Client_UI_Tools::Locale2UTF8(*param_iterator);
          if (!converted_text)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to convert message text (was: \"%s\"), aborting\n"),
                       (*param_iterator).c_str()));

            gdk_threads_leave();

            break;
          } // end IF

          GtkTreeIter list_iterator;
          gtk_list_store_append(server_tab_channels_store,
                                &list_iterator);
          gtk_list_store_set(server_tab_channels_store, &list_iterator,
                             0, converted_text,                   // channel name
                             1, num_members,                      // # visible members
                             2, message_in.params.back().c_str(), // topic
                             -1);

          // clean up
          g_free(converted_text);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_NOTOPIC:          // 331
        case RPG_Net_Protocol_IRC_Codes::RPL_TOPIC:            // 332
        {
          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(message_in.params.front());
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         message_in.params.front().c_str()));

              break;
            } // end IF

            gdk_threads_enter();

            (*handler_iterator).second->setTopic(((message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::RPL_NOTOPIC) ? IRC_CLIENT_GUI_DEF_TOPIC_LABEL_TEXT
                                                                                                                          : message_in.params.back()));

            gdk_threads_leave();
          } // end lock scope

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_WHOREPLY:         // 352
        {
          // bisect user information from parameter strings
          RPG_Net_Protocol_ParametersIterator_t iterator = message_in.params.begin();
          ACE_ASSERT(message_in.params.size() >= 8);
          std::advance(iterator, 5); // nick position
          std::string nick = *iterator;
          iterator++;
          bool away = ((*iterator).find(ACE_TEXT_ALWAYS_CHAR("G"), 0) == 0);
          bool is_IRCoperator = ((*iterator).find(ACE_TEXT_ALWAYS_CHAR("*"), 1) == 1);
          bool is_operator = ((*iterator).find(ACE_TEXT_ALWAYS_CHAR("@"), 2) != std::string::npos);
          bool is_voiced = ((*iterator).find(ACE_TEXT_ALWAYS_CHAR("+"), 2) != std::string::npos);
          unsigned int hop_count = 0;
          std::string real_name;
          std::stringstream converter;
          std::string::size_type ws_position = 0;
          ws_position = message_in.params.back().find(' ', 0);
          converter << message_in.params.back().substr(0, ws_position);
          converter >> hop_count;
          real_name = message_in.params.back().substr(ws_position + 1);

          gdk_threads_enter();

          // retrieve server tab users store
          GtkListStore* server_tab_users_store = GTK_LIST_STORE(gtk_builder_get_object(myCBData.builder,
                                                                                       ACE_TEXT_ALWAYS_CHAR("server_tab_users_store")));
          ACE_ASSERT(server_tab_users_store);

          if (myIsFirstUsersMsg)
          {
            gtk_list_store_clear(server_tab_users_store);

            myIsFirstUsersMsg = false;
          } // end IF

          // ignore own record
          if (nick == myCBData.nickname)
          {
            gdk_threads_leave();

            break;
          } // end IF

          // step1: convert text
          gchar* converted_nick_string = RPG_Client_UI_Tools::Locale2UTF8(nick);
          if (!converted_nick_string)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to convert nickname: \"%s\", aborting\n"),
                       nick.c_str()));

            gdk_threads_leave();

            break;
          } // end IF
          gchar* converted_name_string = RPG_Client_UI_Tools::Locale2UTF8(real_name);
          if (!converted_name_string)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to convert name: \"%s\", aborting\n"),
                       real_name.c_str()));

            // clean up
            g_free(converted_nick_string);

            gdk_threads_leave();

            break;
          } // end IF

          // step2: append new (text) entry
          GtkTreeIter iter;
          gtk_list_store_append(server_tab_users_store, &iter);
          gtk_list_store_set(server_tab_users_store, &iter,
                             0, converted_nick_string, // column 0
                             1, away,
                             2, is_IRCoperator,
                             3, is_operator,
                             4, is_voiced,
                             5, hop_count,
                             6, converted_name_string,
                             -1);

          // clean up
          g_free(converted_nick_string);
          g_free(converted_name_string);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_NAMREPLY:         // 353
        {
          // bisect (WS-separated) nicknames from the final parameter string

//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("bisecting nicknames: \"%s\"...\n"),
//                      message_in.params.back().c_str()));

          std::string::size_type current_position = 0;
          std::string::size_type last_position = 0;
          std::string nick;
          string_list_t list;
          bool is_operator = false;
          do
          {
            current_position = message_in.params.back().find(' ', last_position);

            nick = message_in.params.back().substr(last_position,
                                                   (((current_position == std::string::npos) ? message_in.params.back().size()
                                                                                             : current_position) - last_position));

            // check whether we're channel operator
            if (nick.find(myCBData.nickname) != std::string::npos)
              is_operator = ((nick[0] == '@') &&
                             (nick.size() == (myCBData.nickname.size() + 1)));

            list.push_back(nick);

            // advance
            last_position = current_position + 1;
          } while (current_position != std::string::npos);

          // retrieve channel name
          RPG_Net_Protocol_ParametersIterator_t param_iterator = message_in.params.begin();
          ACE_ASSERT(message_in.params.size() >= 3);
          std::advance(param_iterator, 2);

          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(*param_iterator);
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         (*param_iterator).c_str()));

              break;
            } // end IF

            gdk_threads_enter();

            (*handler_iterator).second->members(list);

            if (is_operator)
            {
              // *NOTE*: ops always have a voice...
              std::string op_mode = ACE_TEXT_ALWAYS_CHAR("+ov");
              (*handler_iterator).second->setModes(op_mode,
                                                   std::string()); // none
            } // end IF

            gdk_threads_leave();
          } // end lock scope

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFNAMES:       // 366
        {
          // retrieve channel name
          RPG_Net_Protocol_ParametersIterator_t param_iterator = message_in.params.begin();
          param_iterator++;

          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(*param_iterator);
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         (*param_iterator).c_str()));

              break;
            } // end IF

            gdk_threads_enter();

            (*handler_iterator).second->endMembers();

            gdk_threads_leave();
          } // end lock scope

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFBANLIST:     // 368
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTD:             // 372
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTDSTART:        // 375
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFMOTD:        // 376
        case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHNICK:       // 401
        case RPG_Net_Protocol_IRC_Codes::ERR_NICKNAMEINUSE:    // 433
        case RPG_Net_Protocol_IRC_Codes::ERR_YOUREBANNEDCREEP: // 465
        case RPG_Net_Protocol_IRC_Codes::ERR_BADCHANNAME:      // 479
        case RPG_Net_Protocol_IRC_Codes::ERR_CHANOPRIVSNEEDED: // 482
        case RPG_Net_Protocol_IRC_Codes::ERR_UMODEUNKNOWNFLAG: // 501
        {
          gdk_threads_enter();

          log(message_in);

          if ((message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHNICK) ||
              (message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::ERR_NICKNAMEINUSE) ||
              (message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::ERR_YOUREBANNEDCREEP) ||
              (message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::ERR_BADCHANNAME) ||
              (message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::ERR_CHANOPRIVSNEEDED) ||
              (message_in.command.numeric == RPG_Net_Protocol_IRC_Codes::ERR_UMODEUNKNOWNFLAG))
            error(message_in); // show in statusbar as well...

          gdk_threads_leave();

          break;
        }
        default:
        {
//           ACE_DEBUG((LM_WARNING,
//                      ACE_TEXT("received unhandled (numeric) command/reply: \"%s\" (%u), continuing\n"),
//                      RPG_Net_Protocol_Tools::IRCCode2String(message_in.command.numeric).c_str(),
//                      message_in.command.numeric));

          message_in.dump_state();

          break;
        }
      } // end SWITCH

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      RPG_Net_Protocol_IRCMessage::CommandType command = RPG_Net_Protocol_Tools::IRCCommandString2Type(*message_in.command.string);
      switch (command)
      {
        case RPG_Net_Protocol_IRCMessage::NICK:
        {
          // remember changed nickname...
          std::string old_nick = myCBData.nickname;
          myCBData.nickname = message_in.params.front();

          gdk_threads_enter();

          // --> display (changed) nickname
          // step1: set server tab nickname label
          GtkLabel* server_tab_nick_label = GTK_LABEL(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_nick_label")));
          ACE_ASSERT(server_tab_nick_label);
          // --> see Pango Text Attribute Markup Language...
          std::string nickname_string = ACE_TEXT_ALWAYS_CHAR("nickname: <b>");
          nickname_string += myCBData.nickname;
          nickname_string += ACE_TEXT_ALWAYS_CHAR("</b>");
          gtk_label_set_markup(server_tab_nick_label,
                               nickname_string.c_str());

          // step2: update channel tab nickname label(s)
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            for (message_handlers_iterator_t iterator = myMessageHandlers.begin();
                 iterator != myMessageHandlers.end();
                 iterator++)
            {
              if ((*iterator).second->isServerLog())
                continue;

              (*iterator).second->updateNick(old_nick);
            } // end FOR
          } // end lock scope

          gdk_threads_leave();

          // *WARNING*: falls through !
        }
        case RPG_Net_Protocol_IRCMessage::USER:
        case RPG_Net_Protocol_IRCMessage::QUIT:
        {
          gdk_threads_enter();

          log(message_in);

          if ((message_in.prefix.origin == myCBData.nickname) &&
              (command == RPG_Net_Protocol_IRCMessage::QUIT))
            error(message_in); // --> show on statusbar as well...

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::JOIN:
        {
          // there are two possibilities:
          // - reply from a successful join request ?
          // - stranger entering the channel

          // reply from a successful join request ?
          if (message_in.prefix.origin == myCBData.nickname)
          {
            gdk_threads_enter();

            createMessageHandler(message_in.params.front());

            gdk_threads_leave();

            break;
          } // end IF

          // someone joined a common channel...

          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(message_in.params.back());
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         message_in.params.back().c_str()));

              break;
            } // end IF

            gdk_threads_enter();

            (*handler_iterator).second->add(message_in.prefix.origin);

            gdk_threads_leave();
          } // end lock scope

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PART:
        {
          // there are two possibilities:
          // - reply from a successful part request ?
          // - someone left a common channel

          gdk_threads_enter();

          // reply from a successful part request ?
          if (message_in.prefix.origin == myCBData.nickname)
          {
            terminateMessageHandler(message_in.params.back());

            gdk_threads_leave();

            break;
          } // end IF

          // someone left a common channel...

          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(message_in.params.back());
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         message_in.params.back().c_str()));

              gdk_threads_leave();

              break;
            } // end IF

            (*handler_iterator).second->remove(message_in.prefix.origin);

            gdk_threads_leave();
          } // end lock scope

          break;
        }
        case RPG_Net_Protocol_IRCMessage::MODE:
        {
          // there are two possibilities:
          // - user mode message
          // - channel mode message

          // retrieve mode string
          RPG_Net_Protocol_ParametersIterator_t param_iterator = message_in.params.begin();
          param_iterator++;

          gdk_threads_enter();

          if (message_in.params.front() == myCBData.nickname)
          {
            // --> user mode
            // *WARNING*: needs the lock protection, otherwise there is a race...
            RPG_Net_Protocol_Tools::merge(message_in.params.back(),
                                          myCBData.userModes);

            updateModeButtons();
          } // end IF
          else
          {
            // --> channel mode

            // retrieve message handler
            // synch access
            {
              ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

              message_handlers_iterator_t handler_iterator = myMessageHandlers.find(message_in.params.front());
              if (handler_iterator == myMessageHandlers.end())
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                           message_in.params.front().c_str()));

                gdk_threads_leave();

                break;
              } // end IF

              // *WARNING*: needs the lock protection, otherwise there is a race...
              (*handler_iterator).second->setModes(*param_iterator,
                                                   ((*param_iterator == message_in.params.back()) ? std::string()
                                                                                                  : message_in.params.back()));
            } // end lock scope
          } // end ELSE

          // log this event
          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::TOPIC:
        {
          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(message_in.params.front());
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         message_in.params.front().c_str()));

              break;
            } // end IF

            gdk_threads_enter();

            (*handler_iterator).second->setTopic(message_in.params.back());
          } // end lock scope

          // log this event
          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::KICK:
        {
          // retrieve nickname string
          RPG_Net_Protocol_ParametersIterator_t param_iterator = message_in.params.begin();
          param_iterator++;

          // retrieve message handler
          // synch access
          {
            ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

            message_handlers_iterator_t handler_iterator = myMessageHandlers.find(message_in.params.front());
            if (handler_iterator == myMessageHandlers.end())
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                         message_in.params.front().c_str()));

              break;
            } // end IF

            gdk_threads_enter();

            (*handler_iterator).second->remove(*param_iterator);
          } // end lock scope

          // log this event
          log(message_in);

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PRIVMSG:
        {
          // *TODO*: parse (list of) receiver(s)

          std::string message_text;
          if (!message_in.prefix.origin.empty())
          {
            message_text += ACE_TEXT("<");
            message_text += message_in.prefix.origin;
            message_text += ACE_TEXT("> ");
          } // end IF
          message_text += message_in.params.back();
          message_text += ACE_TEXT_ALWAYS_CHAR("\n");

          gdk_threads_enter();

          // private message ?
          std::string target_id;
          if (myCBData.nickname == message_in.params.front())
          {
            // --> send to private conversation handler

            // part of an existing conversation ?

            // retrieve message handler
            bool exists = false;
            // synch access
            {
              ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

              exists = (myMessageHandlers.find(message_in.prefix.origin) != myMessageHandlers.end());
            } // end lock scope

            if (!exists)
              createMessageHandler(message_in.prefix.origin);
          } // end IF

          // channel/nick message ?
          if (forward(((myCBData.nickname == message_in.params.front()) ? message_in.prefix.origin
                                                                        : message_in.params.front()),
                      message_text))
          {
            gdk_threads_leave();

            break;
          } // end IF

          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid receiver (was: \"%s\"), aborting\n"),
                     message_in.params.front().c_str()));

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::NOTICE:
#if defined(ACE_WIN32) || defined(ACE_WIN64)
        case RPG_Net_Protocol_IRCMessage::__QUIRK__ERROR:
#else
        case RPG_Net_Protocol_IRCMessage::ERROR:
#endif
        case RPG_Net_Protocol_IRCMessage::AWAY:
        {
          gdk_threads_enter();

          log(message_in);

#if defined(ACE_WIN32) || defined(ACE_WIN64)
          if (command == RPG_Net_Protocol_IRCMessage::__QUIRK__ERROR)
#else
          if (command == RPG_Net_Protocol_IRCMessage::ERROR)
#endif
            error(message_in); // --> show on statusbar as well...

          gdk_threads_leave();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::PING:
        {

          break;
        }
        default:
        {
//           ACE_DEBUG((LM_WARNING,
//                      ACE_TEXT("received unhandled command (was: \"%s\"), continuing\n"),
//                      message_in.command.string->c_str()));

          message_in.dump_state();

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("unknown/invalid command type (was: %u), aborting\n"),
                 message_in.command.discriminator));

      break;
    }
  } // end SWITCH
}

void
IRC_Client_GUI_Connection::end()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::end"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("connection lost...\n")));

  // clean up
  delete this;
}

RPG_Net_Protocol_IIRCControl*
IRC_Client_GUI_Connection::getController()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::getController"));

  // sanity check(s)
  ACE_ASSERT(myCBData.controller);

  return myCBData.controller;
}

IRC_Client_GUI_MessageHandler*
IRC_Client_GUI_Connection::getHandler(const std::string& id_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::getHandler"));

  // existing conversation ? --> retrieve message handler

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    message_handlers_iterator_t iterator = myMessageHandlers.find(id_in);
    if (iterator != myMessageHandlers.end())
      return (*iterator).second;
  } // end lock scope

  return NULL;
}

std::string
IRC_Client_GUI_Connection::getNickname() const
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::getNickname"));

  // sanity check(s)
  ACE_ASSERT(!myCBData.nickname.empty());

  return myCBData.nickname;
}

std::string
IRC_Client_GUI_Connection::getActiveID()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::getActiveID"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // init result
  std::string result;

  // retrieve server tab channel tabs handle
  GtkNotebook* server_tab_channel_tabs = GTK_NOTEBOOK(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_channel_tabs")));
  ACE_ASSERT(server_tab_channel_tabs);
  gint page_num = gtk_notebook_get_current_page(server_tab_channel_tabs);
  if (page_num == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_notebook_get_current_page(%@): no pages, aborting\n"),
               server_tab_channel_tabs));

    return result;
  } // end IF

  // sanity check
  // server log ? --> no active handler --> return empty string
  if (page_num == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no active handler, aborting\n")));

    return result;
  } // end IF

  GtkWidget* channel_tab = gtk_notebook_get_nth_page(server_tab_channel_tabs,
                                                     page_num);
  ACE_ASSERT(channel_tab);

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    for (message_handlers_iterator_t iterator = myMessageHandlers.begin();
         iterator != myMessageHandlers.end();
         iterator++)
    {
      if ((*iterator).second->getTopLevelPageChild() == channel_tab)
      {
        result = (*iterator).first;

        break;
      } // end IF
    } // end FOR
  } // end lock scope

  // sanity check
  if (result.empty())
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to lookup active handler, aborting\n")));

  return result;
}

IRC_Client_GUI_MessageHandler*
IRC_Client_GUI_Connection::getActiveHandler()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::getActiveHandler"));

  // sanity check(s)
  ACE_ASSERT(myCBData.builder);

  // retrieve server tab channel tabs handle
  GtkNotebook* server_tab_channel_tabs = GTK_NOTEBOOK(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_channel_tabs")));
  ACE_ASSERT(server_tab_channel_tabs);
  gint page_num = gtk_notebook_get_current_page(server_tab_channel_tabs);
  if (page_num == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_notebook_get_current_page(%@): no pages, aborting\n"),
               server_tab_channel_tabs));

    return NULL;
  } // end IF
  ACE_ASSERT(page_num >= 0);
  GtkWidget* channel_tab = gtk_notebook_get_nth_page(server_tab_channel_tabs,
                                                     page_num);
  ACE_ASSERT(channel_tab);

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    for (message_handlers_iterator_t iterator = myMessageHandlers.begin();
         iterator != myMessageHandlers.end();
         iterator++)
    {
      if ((*iterator).second->getTopLevelPageChild() == channel_tab)
        return (*iterator).second;
    } // end FOR
  } // end lock scope

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to lookup active handler, aborting\n")));

  return NULL;
}

bool
IRC_Client_GUI_Connection::forward(const std::string& channel_in,
                                   const std::string& messageText_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::forward"));

  // --> pass to channel log

  // retrieve message handler
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    message_handlers_iterator_t handler_iterator = myMessageHandlers.find(channel_in);
    if (handler_iterator == myMessageHandlers.end())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("no handler for channel (was: \"%s\"), aborting\n"),
                 channel_in.c_str()));

      return false;
    } // end IF

    (*handler_iterator).second->queueForDisplay(messageText_in);
  } // end lock scope

  return true;
}

void
IRC_Client_GUI_Connection::log(const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::log"));

  // --> pass to server log

  // retrieve message handler
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    message_handlers_iterator_t handler_iterator = myMessageHandlers.find(std::string());
    ACE_ASSERT(handler_iterator != myMessageHandlers.end());
    (*handler_iterator).second->queueForDisplay(message_in);
  } // end lock scope
}

void
IRC_Client_GUI_Connection::log(const RPG_Net_Protocol_IRCMessage& message_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::log"));

  // --> pass to server log

  // retrieve message handler
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    message_handlers_iterator_t handler_iterator = myMessageHandlers.find(std::string());
    ACE_ASSERT(handler_iterator != myMessageHandlers.end());
    (*handler_iterator).second->queueForDisplay(RPG_Net_Protocol_Tools::IRCMessage2String(message_in));
  } // end lock scope
}

void
IRC_Client_GUI_Connection::error(const RPG_Net_Protocol_IRCMessage& message_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::error"));

  // error --> print on statusbar

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(*myCBData.connectionsLock);

    GtkStatusbar* main_statusbar = GTK_STATUSBAR(gtk_builder_get_object(myCBData.mainBuilder,
                                                                        ACE_TEXT_ALWAYS_CHAR("main_statusbar")));
    ACE_ASSERT(main_statusbar);

    gtk_statusbar_push(main_statusbar,
                       myContextID,
                       RPG_Net_Protocol_Tools::dump(message_in).c_str());
  } // end lock scope
}

guint
IRC_Client_GUI_Connection::exists(const std::string& id_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::exists"));

  // retrieve server tab channel tabs handle
  GtkNotebook* server_tab_channel_tabs = GTK_NOTEBOOK(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_channel_tabs")));
  ACE_ASSERT(server_tab_channel_tabs);

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    message_handlers_iterator_t iterator = myMessageHandlers.find(id_in);

    // sanity check
    if ((myMessageHandlers.empty()) ||
        (iterator == myMessageHandlers.end()))
      return -1;

    // *NOTE*: page_num 0 is the server log: ALWAYS !
    GtkWidget* channel_tab = NULL;
    gint num_pages = gtk_notebook_get_n_pages(server_tab_channel_tabs);
    for (gint page_num = 0;
         page_num < num_pages;
         page_num++)
    {
      // *NOTE*: channel id's are unique, but notebook page numbers
      // change as channels are joined/parted and private conversations "come and
      // go", (for lack of a better metaphor)...
      channel_tab = gtk_notebook_get_nth_page(server_tab_channel_tabs,
                                              page_num);
      ACE_ASSERT(channel_tab);

      if ((*iterator).second->getTopLevelPageChild() == channel_tab)
        return page_num;
    } // end FOR
  } // end lock scope

  return -1;
}

void
IRC_Client_GUI_Connection::channels(string_list_t& channels_out)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::channels"));

  // init return value
  channels_out.clear();

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    for (message_handlers_iterator_t iterator = myMessageHandlers.begin();
         iterator != myMessageHandlers.end();
         iterator++)
      if (RPG_Net_Protocol_Tools::isValidIRCChannelName((*iterator).first))
        channels_out.push_back((*iterator).first);
  } // end lock scope
}

void
IRC_Client_GUI_Connection::createMessageHandler(const std::string& id_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::createMessageHandler"));

  // retrieve channel tabs
  GtkNotebook* server_tab_channel_tabs = GTK_NOTEBOOK(gtk_builder_get_object(myCBData.builder,
                                                                             ACE_TEXT_ALWAYS_CHAR("server_tab_channel_tabs")));
  ACE_ASSERT(server_tab_channel_tabs);

  // create new IRC_Client_GUI_MessageHandler
  IRC_Client_GUI_MessageHandler* message_handler = NULL;
	ACE_NEW_NORETURN(message_handler,
		               IRC_Client_GUI_MessageHandler(this,
                                                 myCBData.controller,
                                                 id_in,
                                                 myUIFileDirectory,
                                                 server_tab_channel_tabs));
  if (!message_handler)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate IRC_Client_GUI_MessageHandler, aborting\n")));

    return;
  } // end IF

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    myMessageHandlers.insert(std::make_pair(id_in, message_handler));

    // check whether this is the first channel of the first connection
    // --> enable corresponding widgets in the main UI
    // synch access
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard2(*myCBData.connectionsLock);

      if ((myCBData.connections->size() == 1) &&
          (myMessageHandlers.size() == 2)) // server log + first channel
      {
        GtkHBox* main_send_hbox = GTK_HBOX(gtk_builder_get_object(myCBData.mainBuilder,
                                                                  ACE_TEXT_ALWAYS_CHAR("main_send_hbox")));
        ACE_ASSERT(main_send_hbox);
        gtk_widget_set_sensitive(GTK_WIDGET(main_send_hbox),
                                 TRUE);
      } // end IF
    } // end lock scope
  } // end lock scope
}

void
IRC_Client_GUI_Connection::terminateMessageHandler(const std::string& id_in)
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::terminateMessageHandler"));

  // retrieve message handler
  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    message_handlers_iterator_t iterator = myMessageHandlers.find(id_in);
    if (iterator == myMessageHandlers.end())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("no handler for id (was: \"%s\"), aborting\n"),
                 id_in.c_str()));

      return;
    } // end IF

    // retrieve channel tabs
    GtkNotebook* server_tab_channel_tabs = GTK_NOTEBOOK(gtk_builder_get_object(myCBData.builder,
                                                                               ACE_TEXT_ALWAYS_CHAR("server_tab_channel_tabs")));
    ACE_ASSERT(server_tab_channel_tabs);

    // clean up
    delete (*iterator).second;
    myMessageHandlers.erase(iterator);

    // check whether this was the last handler of the last connection
    // --> disable corresponding widgets in the main UI
    // synch access
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard2(*myCBData.connectionsLock);

      if ((myCBData.connections->size() == 1) &&
          (myMessageHandlers.size() == 1)) // server log
      {
        GtkHBox* main_send_hbox = GTK_HBOX(gtk_builder_get_object(myCBData.mainBuilder,
                                                                  ACE_TEXT_ALWAYS_CHAR("main_send_hbox")));
        ACE_ASSERT(main_send_hbox);
        gtk_widget_set_sensitive(GTK_WIDGET(main_send_hbox),
                                 FALSE);
      } // end IF
    } // end lock scope
  } // end lock scope
}

void
IRC_Client_GUI_Connection::updateModeButtons()
{
  RPG_TRACE(ACE_TEXT("IRC_Client_GUI_Connection::updateModeButtons"));

  // display (changed) user modes
  GtkToggleButton* togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                                           ACE_TEXT_ALWAYS_CHAR("mode_operator_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_OPERATOR]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_wallops_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_RECVWALLOPS]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_notices_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_RECVNOTICES]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_invisible_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_INVISIBLE]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_localoperator_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_LOCALOPERATOR]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_restricted_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_RESTRICTEDCONN]);
  togglebutton = GTK_TOGGLE_BUTTON(gtk_builder_get_object(myCBData.builder,
                                                          ACE_TEXT_ALWAYS_CHAR("mode_away_togglebutton")));
  ACE_ASSERT(togglebutton);
  gtk_toggle_button_set_active(togglebutton,
                               myCBData.userModes[USERMODE_AWAY]);
}
