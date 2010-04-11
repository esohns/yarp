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

#ifndef RPG_NET_LISTENER_H
#define RPG_NET_LISTENER_H

#include "rpg_net_sockethandler.h"
// #include "rpg_net_stream.h"
// #include "rpg_net_stream_socket_base.h"

#include <rpg_common_icontrol.h>
#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

class RPG_Net_Listener
 : public ACE_Acceptor<RPG_Net_SocketHandler,
//                         RPG_Net_StreamSocketBase<RPG_Net_Stream>,
                       ACE_SOCK_ACCEPTOR>,
    public RPG_Common_IControl
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<RPG_Net_Listener,
                             ACE_Recursive_Thread_Mutex>;

 public:
  // configuration / initialization
  void init(const unsigned short&); // port number
  const bool isInitialized() const;

  // override some methods from ACE_Acceptor
  // *NOTE*: "in the event that an accept fails, this method will be called and
  // the return value will be returned from handle_input()."
  virtual int handle_accept_error(void);

  // implement RPG_Common_IControl
  // *WARNING*: this API is NOT re-entrant !
  virtual void start();
  virtual void stop();
  virtual const bool isRunning();

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef ACE_Acceptor<RPG_Net_SocketHandler,
//                        RPG_Net_StreamSocketBase<RPG_Net_Stream>,
                       ACE_SOCK_ACCEPTOR> inherited;

  // safety measures
  RPG_Net_Listener();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Listener(const RPG_Net_Listener&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Listener& operator=(const RPG_Net_Listener&));
  virtual ~RPG_Net_Listener();

  bool              myIsInitialized;
  bool              myIsListening;
  bool              myIsOpen;
  unsigned short    myListeningPort;
};

typedef ACE_Singleton<RPG_Net_Listener,
                      ACE_Recursive_Thread_Mutex> RPG_NET_LISTENER_SINGLETON;

#endif
