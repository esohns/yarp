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
#include "IRC_client_gui_messagehandler.h"

#include <rpg_net_protocol_iIRCControl.h>
#include <rpg_net_protocol_inotify.h>

#include <gtk/gtk.h>

#include <ace/Global_Macros.h>

#include <string>
#include <map>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class IRC_Client_GUI_Connection_Handler
 : public RPG_Net_Protocol_INotify
{
 public:
  IRC_Client_GUI_Connection_Handler(RPG_Net_Protocol_IIRCControl*, // controller handle
                                    const std::string&,            // label
                                    const std::string&,            // glade file
                                    GtkNotebook*);                 // parent widget
  virtual~IRC_Client_GUI_Connection_Handler();

  // implement RPG_Net_Protocol_INotify
  virtual void start();
  virtual void notify(const RPG_Net_Protocol_IRCMessage&); // message data
  virtual void end();

  RPG_Net_Protocol_IIRCControl* getController();
  const std::string getActiveChannel();
  IRC_Client_GUI_MessageHandler* getActiveHandler();

 private:
  typedef RPG_Net_Protocol_INotify inherited;

  typedef std::map<std::string, IRC_Client_GUI_MessageHandler*> message_handlers_t;
  typedef message_handlers_t::iterator message_handlers_iterator_t;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection_Handler());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection_Handler(const IRC_Client_GUI_Connection_Handler&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection_Handler& operator=(const IRC_Client_GUI_Connection_Handler&));

  // helper methods
  IRC_Client_GUI_MessageHandler* getHandler(const std::string&); // channel name

  connection_cb_data_t myCBData;
  message_handlers_t   myMessageHandlers;
};

#endif
