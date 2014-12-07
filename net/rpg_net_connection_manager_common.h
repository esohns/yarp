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

#ifndef RPG_NET_CONNECTION_MANAGER_COMMON_H
#define RPG_NET_CONNECTION_MANAGER_COMMON_H

#include "rpg_net_stream_common.h"
#include "rpg_net_connection_manager.h"

#include "ace/Singleton.h"
#include "ace/Synch.h"

typedef RPG_Net_Connection_Manager<RPG_Net_ConfigPOD,
                                   RPG_Net_RuntimeStatistic> RPG_Net_Connection_Manager_t;
typedef ACE_Singleton<RPG_Net_Connection_Manager_t,
                      ACE_Recursive_Thread_Mutex> RPG_NET_CONNECTIONMANAGER_SINGLETON;
RPG_NET_SINGLETON_DECLARE(ACE_Singleton,
                          RPG_Net_Connection_Manager_t,
                          ACE_Recursive_Thread_Mutex)

#endif
