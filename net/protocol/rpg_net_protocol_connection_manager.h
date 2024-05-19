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

#ifndef RPG_NET_PROTOCOL_CONNECTION_MANAGER_H
#define RPG_NET_PROTOCOL_CONNECTION_MANAGER_H

#include <vector>

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_inotify.h"

#include "rpg_net_protocol_network.h"

class RPG_Net_Protocol_Connection_Manager
 : public RPG_Net_Protocol_Connection_ManagerBase_t
{
  typedef RPG_Net_Protocol_Connection_ManagerBase_t inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Net_Protocol_Connection_Manager,
                             ACE_SYNCH_MUTEX>;

 public:
  // manage subscriptions
  inline void add (Common_IDispatch* dispatch_in) { subscribers_.push_back (dispatch_in); }
  void remove (Common_IDispatch*); // subscriber handle

  virtual bool register_ (ICONNECTION_T*); // connection handle

 private:
  inline virtual ~RPG_Net_Protocol_Connection_Manager () {}
  RPG_Net_Protocol_Connection_Manager ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Connection_Manager (const RPG_Net_Protocol_Connection_Manager&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Connection_Manager& operator= (const RPG_Net_Protocol_Connection_Manager&))

  std::vector<Common_IDispatch*> subscribers_;
};

typedef ACE_Singleton<RPG_Net_Protocol_Connection_Manager,
                      ACE_SYNCH_MUTEX> RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON;
//RPG_NET_PROTOCOL_SINGLETON_DECLARE(ACE_Singleton,
//                             RPG_Net_Protocol_Connection_Manager,
//                             ACE_Thread_Mutex);
#endif
