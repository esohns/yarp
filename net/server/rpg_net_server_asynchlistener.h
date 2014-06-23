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

#ifndef RPG_NET_SERVER_ASYNCHLISTENER_H
#define RPG_NET_SERVER_ASYNCHLISTENER_H

#include "rpg_net_server_exports.h"
#include "rpg_net_server_ilistener.h"

#include "rpg_net_stream_common.h"

#include "rpg_common_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Asynch_Acceptor.h>
#include <ace/Singleton.h>

class RPG_Net_Server_Export RPG_Net_Server_AsynchListener
 : public ACE_Asynch_Acceptor<RPG_Net_AsynchStreamHandler_t>,
   public RPG_Net_Server_IListener
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<RPG_Net_Server_AsynchListener,
                             ACE_Recursive_Thread_Mutex>;

 public:
  // override default creation strategy
  virtual RPG_Net_AsynchStreamHandler_t* make_handler(void);
  // override default accept strategy
  virtual int validate_connection(const ACE_Asynch_Accept::Result&, // result
                                  const ACE_INET_Addr&,             // remote address
                                  const ACE_INET_Addr&);            // local address
  virtual int should_reissue_accept(void);

  // configuration / initialization
  // implement RPG_Net_Server_IListener
  virtual void init(const unsigned short&, // port number
                    const bool& = false);  // use loopback interface ?
  const bool isInitialized() const;

  // implement RPG_Common_IControl
  // *WARNING*: this API is NOT re-entrant !
  virtual void start();
  virtual void stop(const bool& = true); // locked access ?
  virtual bool isRunning() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef ACE_Asynch_Acceptor<RPG_Net_AsynchStreamHandler_t> inherited;

  RPG_Net_Server_AsynchListener();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_AsynchListener(const RPG_Net_Server_AsynchListener&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_AsynchListener& operator=(const RPG_Net_Server_AsynchListener&));
  virtual ~RPG_Net_Server_AsynchListener();

//  // override default accept strategy
//  // *NOTE*: ACE doesn't properly handle cancellation (dangling bound port on listen socket) -->
//  // fix this here... --> *TODO*: send patch to ACE people
//  virtual void handle_accept(const ACE_Asynch_Accept::Result&); // result

  bool           myIsInitialized;
  bool           myIsListening;
  unsigned short myListeningPort;
  bool           myUseLoopback;
};

typedef ACE_Singleton<RPG_Net_Server_AsynchListener,
                      ACE_Recursive_Thread_Mutex> RPG_NET_SERVER_ASYNCHLISTENER_SINGLETON;
RPG_NET_SERVER_SINGLETON_DECLARE(ACE_Singleton, RPG_Net_Server_AsynchListener, ACE_Recursive_Thread_Mutex);

#endif
