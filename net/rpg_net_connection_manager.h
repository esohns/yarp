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

#include "rpg_net_iconnectionmanager.h"
#include "rpg_net_sockethandler_base.h"

#include "rpg_common_istatistic.h"
#include "rpg_common_idumpstate.h"

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Condition_T.h>
#include <ace/Containers_T.h>

// forward declarations
template <typename StatisticsContainerType> class RPG_Net_IConnection;

template <typename ConfigType,
          typename StatisticsContainerType>
class RPG_Net_Connection_Manager
 : public RPG_Net_IConnectionManager<ConfigType,
                                     StatisticsContainerType>,
   public RPG_Common_IStatistic<StatisticsContainerType>,
   public RPG_Common_IDumpState
{
  // singleton needs access to the ctor/dtors
  friend class ACE_Singleton<RPG_Net_Connection_Manager<ConfigType,
                                                        StatisticsContainerType>,
                             ACE_Recursive_Thread_Mutex>;

  // needs access to (de-)register itself with the singleton
  friend class RPG_Net_SocketHandlerBase<ConfigType,
                                         StatisticsContainerType>;

 public:
  typedef RPG_Net_IConnection<StatisticsContainerType> CONNECTION_TYPE;

  // configuration / initialization
  void init(const unsigned int&); // maximum number of concurrent connections
  // *NOTE*: argument is passed in init() to EVERY new connection during registration
  void set(const ConfigType&); // (user) data

  // implement RPG_Common_IControl
  virtual void start();
  virtual void stop();
  virtual bool isRunning() const;

  // *NOTE*: users of this method should be aware of potential race
  //         conditions with this method and (de-)registerConnection.
  // Scenario: well-behaved shutdown going on WHILE client closes a connection.
  //          (Note that with the current design (only one single-threaded
  //          reactor) this cannot happen).
  void abortConnections();
  void waitConnections() const;
  unsigned int numConnections() const;

  // *WARNING*: to be used for testing ONLY !
  const CONNECTION_TYPE* operator[](unsigned int) const;
  void abortOldestConnection();
  void abortNewestConnection();

  // implement RPG_Common_IStatistic
  virtual void report() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef ACE_DLList<CONNECTION_TYPE> CONNECTIONLIST_TYPE;
  typedef ACE_DLList_Iterator<CONNECTION_TYPE> CONNECTIONLIST_ITERATOR_TYPE;
  typedef ACE_DLList_Reverse_Iterator<CONNECTION_TYPE> CONNECTIONLIST_REVERSEITERATOR_TYPE;

  // implement RPG_Net_IConnectionManager
  virtual void getConfig(ConfigType&); // return value: configuration
  virtual bool registerConnection(CONNECTION_TYPE*); // connection
  virtual void deregisterConnection(const CONNECTION_TYPE*); // connection

  // implement RPG_Common_IStatistic
  // *WARNING*: this assumes we're holding our lock !
  virtual bool collect(StatisticsContainerType&) const; // return value: statistic data

  // safety measures
  RPG_Net_Connection_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Connection_Manager(const RPG_Net_Connection_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Connection_Manager& operator=(const RPG_Net_Connection_Manager&));
  virtual ~RPG_Net_Connection_Manager();

  // *NOTE*: MUST be recursive, otherwise we deadlock in abortConnections() !!!
  // *TODO*: get rid of this lock --> find a better solution
  mutable ACE_Recursive_Thread_Mutex                myLock;
  // implement blocking wait...
  // *TODO*: fix ACE to use ACE_Thread_Condition ?
  mutable ACE_Condition<ACE_Recursive_Thread_Mutex> myCondition;

  unsigned int                                      myMaxNumConnections;
  CONNECTIONLIST_TYPE                               myConnections;
  ConfigType                                        myUserData; // handler data
  bool                                              myIsInitialized;
  bool                                              myIsActive;
};

// include template implementation
#include "rpg_net_connection_manager.i"

#endif
