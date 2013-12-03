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

#include "rpg_net_client_connector.h"

#include "rpg_net_common.h"

#include "rpg_common_macros.h"

RPG_Net_Client_Connector::RPG_Net_Client_Connector()
 : inherited(ACE_Reactor::instance(), // default reactor
             ACE_NONBLOCK)            // flags: non-blocking I/O
             //0)                       // flags
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_Connector::RPG_Net_Client_Connector"));

}

RPG_Net_Client_Connector::~RPG_Net_Client_Connector()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_Connector::~RPG_Net_Client_Connector"));

}

int
RPG_Net_Client_Connector::make_svc_handler(RPG_Net_StreamHandler_t*& handler_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_Connector::make_svc_handler"));

  // init return value(s)
  handler_inout = NULL;

  // default behavior
  ACE_NEW_NORETURN(handler_inout,
                   RPG_Net_StreamHandler_t());

  return ((handler_inout == NULL) ? -1 : 0);
}
