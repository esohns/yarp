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

#ifndef RPG_NET_PROTOCOL_STREAM_COMMON_H
#define RPG_NET_PROTOCOL_STREAM_COMMON_H

#include "common_inotify.h"

#include "stream_common.h"
#include "stream_session_data_base.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_IRCmessage.h"

typedef Stream_SessionDataBase_T<RPG_Net_Protocol_SessionData> RPG_Net_Protocol_StreamSessionData_t;

typedef Common_INotify_T<Stream_ModuleConfiguration_t,
                         RPG_Net_Protocol_IRCMessage> RPG_Net_Protocol_INotify_t;

#endif
