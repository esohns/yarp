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

#ifndef RPG_NET_CONNECTION_COMMON_H
#define RPG_NET_CONNECTION_COMMON_H

#include "rpg_net_stream_common.h"
#include "rpg_net_stream.h"
#include "rpg_net_sockethandler.h"
#include "rpg_net_asynchstreamhandler_t.h"

typedef RPG_Net_SocketHandler<RPG_Net_ConfigPOD,
                              RPG_Net_RuntimeStatistic,
                              RPG_Net_Stream> RPG_Net_SocketHandler_t;

typedef RPG_Net_SocketHandler_t RPG_Net_TCPSocketHandler_t;
typedef RPG_Net_AsynchStreamHandler<RPG_Net_ConfigPOD,
                                    RPG_Net_RuntimeStatistic,
                                    RPG_Net_Stream> RPG_Net_AsynchTCPSocketHandler_t;

#endif
