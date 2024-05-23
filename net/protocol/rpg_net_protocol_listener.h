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

#ifndef RPG_NET_PROTOCOL_LISTENER_H
#define RPG_NET_PROTOCOL_LISTENER_H

#include <vector>

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_inotify.h"

#include "rpg_net_protocol_network.h"

class RPG_Net_Protocol_TCPListener
 : public RPG_Net_Protocol_TCPListenerBase_t
{
  typedef RPG_Net_Protocol_TCPListenerBase_t inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Net_Protocol_TCPListener,
                             ACE_SYNCH_MUTEX>;

 public:
  // convenient types
  typedef ACE_Singleton<RPG_Net_Protocol_TCPListener,
                        ACE_SYNCH_MUTEX> SINGLETON_T;

  // manage subscriptions
  inline void add (Common_IDispatch* dispatch_in) { subscribers_.push_back (dispatch_in); }
  void remove (Common_IDispatch*); // subscriber handle

 protected:
  // override default activation strategy
  virtual int activate_svc_handler (RPG_Net_Protocol_TCPConnection_t*);

 private:
  inline virtual ~RPG_Net_Protocol_TCPListener () {}
  RPG_Net_Protocol_TCPListener ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_TCPListener (const RPG_Net_Protocol_TCPListener&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_TCPListener& operator= (const RPG_Net_Protocol_TCPListener&))

  std::vector<Common_IDispatch*> subscribers_;
};

//////////////////////////////////////////

class RPG_Net_Protocol_AsynchTCPListener
 : public RPG_Net_Protocol_AsynchTCPListenerBase_t
{
  typedef RPG_Net_Protocol_AsynchTCPListenerBase_t inherited;

  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Net_Protocol_AsynchTCPListener,
                             ACE_SYNCH_MUTEX>;

 public:
  // convenient types
  typedef ACE_Singleton<RPG_Net_Protocol_AsynchTCPListener,
                        ACE_SYNCH_MUTEX> SINGLETON_T;

  // manage subscriptions
  inline void add (Common_IDispatch* dispatch_in) { subscribers_.push_back (dispatch_in); }
  void remove (Common_IDispatch*); // subscriber handle

 protected:
  // override default accept strategy
  virtual void handle_accept (const ACE_Asynch_Accept::Result&); // result

 private:
  inline virtual ~RPG_Net_Protocol_AsynchTCPListener () {}
  RPG_Net_Protocol_AsynchTCPListener ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_AsynchTCPListener (const RPG_Net_Protocol_AsynchTCPListener&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_AsynchTCPListener& operator= (const RPG_Net_Protocol_AsynchTCPListener&))

  std::vector<Common_IDispatch*> subscribers_;
};

//////////////////////////////////////////

typedef ACE_Singleton<RPG_Net_Protocol_TCPListener,
                      ACE_SYNCH_MUTEX> RPG_NET_PROTOCOL_TCP_LISTENER_SINGLETON;
typedef ACE_Singleton<RPG_Net_Protocol_AsynchTCPListener,
                      ACE_SYNCH_MUTEX> RPG_NET_PROTOCOL_ASYNCH_TCP_LISTENER_SINGLETON;





#endif
