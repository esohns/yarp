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

#ifndef RPG_NET_CONNECTION_BASE_H
#define RPG_NET_CONNECTION_BASE_H

#include "rpg_common_referencecounter_base.h"

#include "rpg_net_iconnection.h"
#include "rpg_net_iconnectionmanager.h"

#include "ace/INET_Addr.h"

template <typename ConfigurationType,
          typename StatisticsContainerType>
class RPG_Net_ConnectionBase
 : public RPG_Common_ReferenceCounterBase,
   public RPG_Net_IConnection<StatisticsContainerType>
{
 public:
  // implement (part of) RPG_Net_ITransportLayer
  virtual bool init(const ACE_INET_Addr&, // peer address
                    unsigned short);      // port number
  virtual void fini();

 protected:
  typedef RPG_Net_IConnectionManager<ConfigurationType,
                                     StatisticsContainerType> MANAGER_T;
  RPG_Net_ConnectionBase(MANAGER_T*);
  virtual ~RPG_Net_ConnectionBase();

  MANAGER_T*        myManager;
  ConfigurationType myUserData;
  bool              myIsRegistered;

 private:
  typedef RPG_Common_ReferenceCounterBase inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_ConnectionBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_ConnectionBase(const RPG_Net_ConnectionBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_ConnectionBase& operator=(const RPG_Net_ConnectionBase&));
};

// include template implementation
#include "rpg_net_connection_base.inl"

#endif
