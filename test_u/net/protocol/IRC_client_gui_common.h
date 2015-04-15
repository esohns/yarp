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

#ifndef IRC_CLIENT_GUI_COMMON_H
#define IRC_CLIENT_GUI_COMMON_H

#include <map>
#include <string>
#include <vector>

#include "common_ui_common.h"

#include "rpg_net_protocol_common.h"
//#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_IRCmessage.h"

// forward declaration(s)
class ACE_Thread_Mutex;
struct RPG_Net_Protocol_Configuration;
class RPG_Net_Protocol_IIRCControl;
class IRC_Client_GUI_Connection;

typedef std::map<std::string, IRC_Client_GUI_Connection*> connections_t;
typedef connections_t::iterator connections_iterator_t;

struct main_cb_data_t
{
  RPG_Net_Protocol_Configuration* configuration;
  connections_t                   connections;
  Common_UI_GTKState              GTKState;
  RPG_Net_Protocol_PhoneBook      phoneBook;
  std::string                     UIFileDirectory;
};

struct connection_cb_data_t
{
  // *TODO*: couldn't it be done without this one ?
  bool                          away;
  IRC_Client_GUI_Connection*    connection;
  connections_t*                connections;
  RPG_Net_Protocol_IIRCControl* controller;
  Common_UI_GTKState*           GTKState;
  std::string                   nickname;
  RPG_Net_Protocol_UserModes_t  userModes;
};

struct handler_cb_data_t
{
  RPG_Net_Protocol_ChannelModes_t channelModes;
  IRC_Client_GUI_Connection*      connection;
  RPG_Net_Protocol_IIRCControl*   controller;
  std::string                     id;
  Common_UI_GTKState*             GTKState;
  string_list_t                   parameters;
};

#endif
