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

#ifndef RPG_NET_CLIENT_CONNECTOR_H
#define RPG_NET_CLIENT_CONNECTOR_H

#include "rpg_net_client_exports.h"
#include "rpg_net_client_iconnector.h"

#include "rpg_net_tcpconnection.h"

#include "ace/Global_Macros.h"
#include "ace/Connector.h"
#include "ace/SOCK_Connector.h"

class RPG_Net_Client_Export RPG_Net_Client_Connector
 : public ACE_Connector<RPG_Net_TCPConnection,
                        ACE_SOCK_CONNECTOR>,
   public RPG_Net_Client_IConnector
{
 public:
  RPG_Net_Client_Connector();
  virtual ~RPG_Net_Client_Connector();

  // override default creation strategy
  virtual int make_svc_handler (RPG_Net_TCPConnection*&);

  // implement RPG_Net_Client_IConnector
  virtual void abort();
  virtual void connect(const ACE_INET_Addr&);

 private:
  typedef ACE_Connector<RPG_Net_TCPConnection,
                        ACE_SOCK_CONNECTOR> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_Connector(const RPG_Net_Client_Connector&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_Connector& operator=(const RPG_Net_Client_Connector&));
};

#endif
