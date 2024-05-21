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
RPG_Client_Network_Manager::action (const struct RPG_Engine_Action& action_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::action"));

  // sanity check(s)
  ACE_ASSERT (connection_);

  // send command to client
  RPG_Net_Protocol_Connection_Manager* connection_manager_p =
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  RPG_Net_Protocol_ConnectionConfiguration* configuration_p = NULL;
  struct Net_UserData* user_data_p = NULL;
  connection_manager_p->get (configuration_p,
                             user_data_p);
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->allocatorConfiguration);
  ACE_ASSERT (configuration_p->messageAllocator);

  RPG_Net_Protocol_Message* message_p =
    static_cast<RPG_Net_Protocol_Message*> (configuration_p->messageAllocator->malloc (configuration_p->allocatorConfiguration->defaultBufferSize));
  ACE_ASSERT (message_p);

  struct RPG_Net_Protocol_Command command_s;
  command_s.command =
    static_cast<enum RPG_Net_Protocol_Engine_Command> (action_in.command);
  command_s.path = action_in.path;
  command_s.position = action_in.position;
  command_s.entity_id = action_in.target;

  message_p->initialize (command_s,
                         1, // *TODO*
                         NULL);
  ACE_Message_Block* message_block_p = message_p;
  connection_->send (message_block_p);
}

void
RPG_Client_Network_Manager::action (const struct RPG_Client_Action& action_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Network_Manager::action"));

  // sanity check(s)
  if (!connection_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no active connection; cannot notify peer (action was: %d), returning\n"),
                action_in.command));
    return; // cannot comply
  } // end IF

  // send command to peer
  RPG_Net_Protocol_Connection_Manager* connection_manager_p =
    RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  RPG_Net_Protocol_ConnectionConfiguration* configuration_p = NULL;
  struct Net_UserData* user_data_p = NULL;
  connection_manager_p->get (configuration_p,
                             user_data_p);
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->allocatorConfiguration);
  ACE_ASSERT (configuration_p->messageAllocator);

  RPG_Net_Protocol_Message* message_p =
    static_cast<RPG_Net_Protocol_Message*> (configuration_p->messageAllocator->malloc (configuration_p->allocatorConfiguration->defaultBufferSize));
  ACE_ASSERT (message_p);

  struct RPG_Net_Protocol_Command command_s;
  command_s.clientCommand =
    static_cast<enum RPG_Net_Protocol_Client_Command> (action_in.command);
  command_s.path = action_in.path;
  command_s.position = action_in.position;
  command_s.entity_id = action_in.entity_id;

  command_s.previous = action_in.previous;
  command_s.cursor = action_in.cursor;
  command_s.sound = action_in.sound;
  command_s.message = action_in.message;
  command_s.source = action_in.source;
  command_s.positions = action_in.positions;
  command_s.radius = action_in.radius;

  message_p->initialize (command_s,
                         1, // *TODO*
                         NULL);
  ACE_Message_Block* message_block_p = message_p;
  connection_->send (message_block_p);
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
