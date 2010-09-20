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

#include <rpg_net_protocol_common.h>
#include <rpg_net_protocol_iIRCControl.h>

#include <gtk/gtk.h>

#include <ace/Synch.h>

#include <vector>
#include <string>

// forward declaration(s)
class RPG_Stream_IAllocator;
class IRC_Client_GUI_Connection;

typedef std::map<std::string, IRC_Client_GUI_Connection*> connections_t;
typedef connections_t::iterator connections_iterator_t;

struct main_cb_data_t
{
  RPG_Stream_IAllocator*           allocator;
  bool                             debugParser;
  bool                             debugScanner;
  unsigned long                    statisticsReportingInterval; // seconds [0 --> OFF]
  std::string                      UIFileDirectory;
  // *WARNING*: mainBuilder needs exclusive access under the "connectionsLock"
  GtkBuilder*                      builder;
  RPG_Net_Protocol_PhoneBook       phoneBook;
  RPG_Net_Protocol_IRCLoginOptions loginOptions;
  ACE_Thread_Mutex                 connectionsLock;
  connections_t                    connections;
};

struct connection_cb_data_t
{
  // *WARNING*: mainBuilder needs exclusive access under the "connectionsLock"
  GtkBuilder*                   mainBuilder;
  GtkBuilder*                   builder;
  std::string                   nickname;
  RPG_Net_Protocol_UserModes_t  userModes;
  RPG_Net_Protocol_IIRCControl* controller;
  ACE_Thread_Mutex*             connectionsLock;
  connections_t*                connections;
};

struct handler_cb_data_t
{
  IRC_Client_GUI_Connection*      connection;
  GtkBuilder*                     builder;
  std::string                     id;
  RPG_Net_Protocol_ChannelModes_t channelModes;
  RPG_Net_Protocol_IIRCControl*   controller;
};

#endif
