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

#include "rpg_net_protocol_listener.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

RPG_Net_Protocol_TCPListener::RPG_Net_Protocol_TCPListener ()
 : inherited ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_TCPListener::RPG_Net_Protocol_TCPListener"));

}

void
RPG_Net_Protocol_TCPListener::remove (Common_IDispatch* dispatch_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_TCPListener::remove"));

  std::vector<Common_IDispatch*>::iterator iterator =
    std::find (subscribers_.begin (), subscribers_.end (), dispatch_in);
  if (iterator != subscribers_.end ())
    subscribers_.erase (iterator);
}

int
RPG_Net_Protocol_TCPListener::activate_svc_handler (RPG_Net_Protocol_TCPConnection_t* connection_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_TCPListener::activate_svc_handler"));

  int result = inherited::activate_svc_handler (connection_in);

  // notify subscriber(s)
  RPG_Net_Protocol_IConnection_t* iconnection_p = connection_in;
  for (std::vector<Common_IDispatch*>::iterator iterator = subscribers_.begin ();
       iterator != subscribers_.end ();
       ++iterator)
    (*iterator)->dispatch (iconnection_p);

  return result;
}

//////////////////////////////////////////

RPG_Net_Protocol_AsynchTCPListener::RPG_Net_Protocol_AsynchTCPListener ()
 : inherited ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_AsynchTCPListener::RPG_Net_Protocol_AsynchTCPListener"));

}

void
RPG_Net_Protocol_AsynchTCPListener::remove (Common_IDispatch* dispatch_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_AsynchTCPListener::remove"));

  std::vector<Common_IDispatch*>::iterator iterator =
    std::find (subscribers_.begin (), subscribers_.end (), dispatch_in);
  if (iterator != subscribers_.end ())
    subscribers_.erase (iterator);
}

void
RPG_Net_Protocol_AsynchTCPListener::handle_accept (const ACE_Asynch_Accept::Result& result)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_AsynchTCPListener::handle_accept"));

  int error = 0;
  ACE_HANDLE listen_handle = ACE_INVALID_HANDLE;
  RPG_Net_Protocol_AsynchTCPConnection_t* handler_p = NULL;
  ACE_INET_Addr remote_address, local_address;

  if (unlikely (!result.success () ||
                (result.accept_handle () == ACE_INVALID_HANDLE)))
    error = 1;

  listen_handle = this->handle ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // In order to use accept handle with other Window Sockets 1.1
  // functions, we call the setsockopt function with the
  // SO_UPDATE_ACCEPT_CONTEXT option. This option initializes the
  // socket so that other Windows Sockets routines to access the
  // socket correctly.
  if (unlikely (!error &&
                ACE_OS::setsockopt (result.accept_handle (),
                                    SOL_SOCKET,
                                    SO_UPDATE_ACCEPT_CONTEXT,
                                    (char*)&listen_handle,
                                    sizeof (listen_handle)) == -1))
    error = 1;
#endif // ACE_WIN32 || ACE_WIN64

  // retrieve addresses ?
  if (likely (!error &&
              (this->validate_new_connection () || this->pass_addresses ())))
    this->parse_address (result,
                         remote_address,
                         local_address);

  // validate remote address
  if (unlikely (!error &&
                this->validate_new_connection () &&
                (this->validate_connection (result,
                                            remote_address, local_address) == -1)))
    error = 1;

  if (likely (!error))
  {
    handler_p = this->make_handler ();
    if (unlikely (!handler_p))
      error = 1;
  } // end IF

  if (likely (!error))
  {
    if (unlikely (!handler_p->proactor ()))
      handler_p->proactor (this->proactor ());

    if (likely (this->pass_addresses ()))
      handler_p->addresses (remote_address,
                            local_address);

    // *EDIT*: set role
    //    Net_ILinkLayer_T<Net_TCPSocketConfiguration_t>* ilinklayer_p = ;
    handler_p->set (NET_ROLE_SERVER);

           // Pass the ACT
    if (unlikely (result.act ()))
      handler_p->act (result.act ());

    handler_p->handle (result.accept_handle ());

    handler_p->open (result.accept_handle (),
                     result.message_block ());
  } // end IF

  if (unlikely (error &&
                result.accept_handle () != ACE_INVALID_HANDLE))
    ACE_OS::closesocket (result.accept_handle ());
  result.message_block ().release ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (likely (this->should_reissue_accept ()               &&
              (listen_handle != ACE_INVALID_HANDLE)        &&
              (result.error () != ERROR_OPERATION_ABORTED)))
#else
  if (likely (this->should_reissue_accept ()        &&
              (listen_handle != ACE_INVALID_HANDLE) &&
              (result.error () != ECANCELED)))
#endif // ACE_WIN32
    this->accept (this->bytes_to_read (), result.act ());

  // notify subscriber(s)
  RPG_Net_Protocol_IConnection_t* iconnection_p = handler_p;
  for (std::vector<Common_IDispatch*>::iterator iterator = subscribers_.begin ();
       iterator != subscribers_.end ();
       ++iterator)
    (*iterator)->dispatch (iconnection_p);
}
