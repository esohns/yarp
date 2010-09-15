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
#ifndef IRC_CLIENT_GUI_CONNECTION_HANDLER_H
#define IRC_CLIENT_GUI_CONNECTION_HANDLER_H

#include "IRC_client_gui_common.h"

#include <rpg_net_protocol_common.h>
#include <rpg_net_protocol_iIRCControl.h>
#include <rpg_net_protocol_inotify.h>

#include <gtk/gtk.h>

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

#include <string>
#include <map>

// forward declaration(s)
class IRC_Client_GUI_MessageHandler;

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class IRC_Client_GUI_Connection
 : public RPG_Net_Protocol_INotify
{
 public:
  // *WARNING*: make sure the ctor/dtor calls are made either:
  // - by the main thread (servicing the gtk_main event loop)
  // - protected by GDK_THREADS_ENTER/GDK_THREADS_LEAVE
  IRC_Client_GUI_Connection(GtkBuilder*,                   // main builder handle
                            RPG_Net_Protocol_IIRCControl*, // controller handle
                            ACE_Thread_Mutex*,             // connections lock handle
                            connections_t*,                // connections handle
//                             const std::string&,            // (starting) nickname
                            const std::string&,            // (server tab) label
                            const std::string&,            // UI (glade) file directory
                            GtkNotebook*);                 // parent widget
  virtual~IRC_Client_GUI_Connection();

  // implement RPG_Net_Protocol_INotify
  virtual void start();
  virtual void notify(const RPG_Net_Protocol_IRCMessage&); // message data
  virtual void end();

  RPG_Net_Protocol_IIRCControl* getController();

  // *WARNING*: callers need protection
  // - by the main thread (servicing the gtk_main event loop)
  // - protected by GDK_THREADS_ENTER/GDK_THREADS_LEAVE
  const std::string getActiveChannel();
  IRC_Client_GUI_MessageHandler* getActiveHandler();

 private:
  typedef RPG_Net_Protocol_INotify inherited;

  typedef std::map<std::string, IRC_Client_GUI_MessageHandler*> message_handlers_t;
  typedef message_handlers_t::iterator message_handlers_iterator_t;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection(const IRC_Client_GUI_Connection&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection& operator=(const IRC_Client_GUI_Connection&));

//   // helper methods
//   IRC_Client_GUI_MessageHandler* getHandler(const std::string&); // channel name
  void log(const RPG_Net_Protocol_IRCMessage&);
  void error(const RPG_Net_Protocol_IRCMessage&);
  const std::string nick();

  std::string                  myUIFileDirectory;
  connection_cb_data_t         myCBData;

  ACE_Thread_Mutex             myLock;
  message_handlers_t           myMessageHandlers;

  RPG_Net_Protocol_UserModes_t myUserModes;

  GtkNotebook*                 myParent;
  guint                        myContextID;
};

#endif
