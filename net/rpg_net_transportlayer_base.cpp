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
#include "stdafx.h"

#include "rpg_net_transportlayer_base.h"

#include "rpg_net_defines.h"

#include "rpg_common_macros.h"

RPG_Net_TransportLayer_Base::RPG_Net_TransportLayer_Base(RPG_Net_ClientServerRole_t role_in,
                                                         RPG_Net_TransportLayer_t transportLayer_in)
 : myClientServerRole(role_in)
 , myTransportLayer(transportLayer_in)
 , myPort(RPG_NET_DEFAULT_PORT)
 , myUseLoopback(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_Base::RPG_Net_TransportLayer_Base"));

}

RPG_Net_TransportLayer_Base::~RPG_Net_TransportLayer_Base()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_Base::~RPG_Net_TransportLayer_Base"));

}

void
RPG_Net_TransportLayer_Base::init (RPG_Net_ClientServerRole_t role_in,
                                   unsigned short port_in,
                                   bool useLoopback_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_TransportLayer_Base::init"));

  myClientServerRole = role_in;
  myPort = port_in;
  myUseLoopback = useLoopback_in;
}

void
RPG_Net_TransportLayer_Base::info (ACE_HANDLE& handle_out,
                                   ACE_INET_Addr& localSAP_out,
                                   ACE_INET_Addr& remoteSAP_out) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_TransportLayer_Base::info"));

  // init return value(s)
  handle_out = ACE_INVALID_HANDLE;
  localSAP_out = ACE_sap_any_cast (ACE_INET_Addr&);
  remoteSAP_out = ACE_sap_any_cast (ACE_INET_Addr&);

  ACE_ASSERT (false);
}
