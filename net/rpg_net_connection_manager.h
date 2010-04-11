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

#ifndef RPG_NET_CONNECTION_MANAGER_H
#define RPG_NET_CONNECTION_MANAGER_H

#include "rpg_net_common.h"
#include "rpg_net_sockethandler_base.h"

#include <rpg_common_istatistic.h>
#include <rpg_common_idumpstate.h>

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Condition_T.h>

#include <list>

// forward declarations
class RPG_Net_IConnection;

class RPG_Net_Connection_Manager
 : public RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>,
   public RPG_Common_IDumpState
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<RPG_Net_Connection_Manager,
                             ACE_Recursive_Thread_Mutex>;

  // needs access to (de-)register itself with the singleton
  friend class RPG_Net_SocketHandlerBase;

 public:
  // configuration / initialization
  void init(const unsigned long&,      // maximum number of concurrent connections
            const RPG_Net_ConfigPOD&); // user data

  // *NOTE*: users of this method should be aware of potential race
  //         conditions with this method and (de-)registerConnection.
  // Scenario: well-behaved shutdown going on WHILE client closes a connection.
  //          (Note that with the current design (only one single-threaded
  //          reactor) this cannot happen).
  void abortConnections();
  void waitConnections() const;
  const unsigned long numConnections() const;

  // *WARNING*: to be used for testing ONLY !
  void abortOldestConnection();

  // implement RPG_Common_IStatistic
  virtual void report() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef std::list<RPG_Net_IConnection*> CONNECTIONLIST_TYPE;
  typedef CONNECTIONLIST_TYPE::const_iterator CONNECTIONLIST_CONSTITERATOR_TYPE;
  typedef CONNECTIONLIST_TYPE::iterator CONNECTIONLIST_ITERATOR_TYPE;

  // *NOTE*: these are used by RPG_Net_SocketHandler_Base
  const bool registerConnection(RPG_Net_IConnection*);
  void deregisterConnection(const unsigned long&); // connection ID

  // implement RPG_Common_IStatistic
  // *WARNING*: this assumes we're holding our lock !
  virtual const bool collect(RPG_Net_RuntimeStatistic&) const; // return value: statistic data

  // safety measures
  RPG_Net_Connection_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Connection_Manager(const RPG_Net_Connection_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Connection_Manager& operator=(const RPG_Net_Connection_Manager&));
  virtual ~RPG_Net_Connection_Manager();

  // *NOTE*: MUST be recursive, otherwise we deadlock in abortConnections() !!!
  // *TODO*: get rid of this lock --> find a better solution
  mutable ACE_Recursive_Thread_Mutex                myLock;
  // implement blocking wait...
  mutable ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;

  unsigned long                                     myMaxNumConnections;
  CONNECTIONLIST_TYPE                               myConnections;

  // handler data
  RPG_Net_ConfigPOD                                 myUserData;

  bool                                              myIsInitialized;
};

typedef ACE_Singleton<RPG_Net_Connection_Manager,
                      ACE_Recursive_Thread_Mutex> RPG_NET_CONNECTIONMANAGER_SINGLETON;

#endif
