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

#include "rpg_client_network_manager.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

#include "rpg_net_protocol_connection_manager.h"

RPG_Client_Network_Manager::RPG_Client_Network_Manager ()
 : connection_ (NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::RPG_Client_Network_Manager"));

  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->add (this);
}

RPG_Client_Network_Manager::~RPG_Client_Network_Manager ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::~RPG_Client_Network_Manager"));

  // clean up
  if (connection_)
  {
    connection_->abort ();
    connection_->decrease ();
  } // end IF

  RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ()->remove (this);
}

void
RPG_Client_Network_Manager::action (const struct RPG_Client_Action& action_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::action"));

  // sanity check(s)
  ACE_ASSERT (connection_);

  ACE_ASSERT (false); // *TODO*
}

void
RPG_Client_Network_Manager::add (RPG_Net_Protocol_IConnection_t* connection_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::add"));

  // sanity check(s)
  ACE_ASSERT (!connection_);

  connection_ = connection_in;
}

void
RPG_Client_Network_Manager::remove (Net_ConnectionId_t id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::remove"));

  // sanity check(s)
  ACE_ASSERT (connection_ && connection_->id () == id_in);

  connection_->abort ();
  connection_->decrease (); connection_ = NULL;
}

void
RPG_Client_Network_Manager::dispatch (void* connection_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::dispatch"));

  RPG_Net_Protocol_IConnection_t* iconnection_p =
    reinterpret_cast<RPG_Net_Protocol_IConnection_t*> (connection_in);
  ACE_ASSERT (iconnection_p);

  add (iconnection_p);
}
