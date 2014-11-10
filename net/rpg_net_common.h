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

#ifndef RPG_NET_COMMON_H
#define RPG_NET_COMMON_H

#include "rpg_net_inotify.h"
#include "rpg_net_message.h"

#include "rpg_common.h"

#include "ace/Task.h"
#include "ace/Module.h"
#include "ace/Synch_Traits.h"

#include <list>

typedef ACE_Task<ACE_MT_SYNCH,
                 RPG_Common_TimePolicy_t> TASK_TYPE;
typedef ACE_Module<ACE_MT_SYNCH,
                   RPG_Common_TimePolicy_t> MODULE_TYPE;

enum RPG_Net_ClientServerRole_t
{
  ROLE_INVALID = -1,
  ROLE_CLIENT = 0,
  ROLE_SERVER,
  //
  ROLE_MAX
};

enum RPG_Net_TransportLayer_t
{
  TRANSPORTLAYER_INVALID = -1,
  TRANSPORTLAYER_IP_BROADCAST = 0,
  TRANSPORTLAYER_IP_MULTICAST,
  TRANSPORTLAYER_TCP,
  TRANSPORTLAYER_UDP,
  //
  TRANSPORTLAYER_MAX
};

typedef RPG_Net_INotify<RPG_Net_Message> RPG_Net_INotify_t;
typedef std::list<RPG_Net_INotify_t*> RPG_Net_NotifySubscribers_t;
typedef RPG_Net_NotifySubscribers_t::iterator RPG_Net_NotifySubscribersIterator_t;

#endif
