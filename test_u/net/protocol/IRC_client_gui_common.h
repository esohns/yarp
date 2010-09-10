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

#include <rpg_net_protocol_iIRCControl.h>

#include <gtk/gtk.h>

#include <vector>
#include <string>

struct connection_cb_data_t
{
  GtkBuilder*                    builder;
  RPG_Net_Protocol_IIRCControl*  controller;
};

struct channel_cb_data_t
{
  GtkBuilder*                   builder;
  std::string                   channel;
  RPG_Net_Protocol_IIRCControl* controller;
};

typedef std::vector<std::string> string_list_t;
typedef string_list_t::const_iterator string_list_iterator_t;

#endif
