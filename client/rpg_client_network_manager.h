/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef RPG_CLIENT_NETWORK_MANAGER_H
#define RPG_CLIENT_NETWORK_MANAGER_H

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_inotify.h"

#include "rpg_net_protocol_network.h"

#include "rpg_client_common.h"

class RPG_Client_Network_Manager
 : public Common_IDispatch
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Client_Network_Manager,
                             ACE_SYNCH_MUTEX>;

 public:
  void action (const struct RPG_Engine_Action&); // action
  void action (const struct RPG_Client_Action&); // action

  // manage connections
  void add (RPG_Net_Protocol_IConnection_t*); // connection handle
  void remove (Net_ConnectionId_t); // connection id

 private:
  virtual ~RPG_Client_Network_Manager ();
  RPG_Client_Network_Manager ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Network_Manager (const RPG_Client_Network_Manager&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Network_Manager& operator= (const RPG_Client_Network_Manager&))

  virtual void dispatch (void*); // user data

  RPG_Net_Protocol_IConnection_t* connection_;
};

typedef ACE_Singleton<RPG_Client_Network_Manager,
                      ACE_SYNCH_MUTEX> RPG_CLIENT_NETWORK_MANAGER_SINGLETON;
//RPG_CLIENT_SINGLETON_DECLARE(ACE_Singleton,
//                             RPG_Client_Network_Manager,
//                             ACE_Thread_Mutex);
#endif
