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

#include "rpg_net_client_asynchconnector.h"

#include "rpg_net_common.h"

#include "rpg_common_macros.h"

RPG_Net_Client_AsynchConnector::RPG_Net_Client_AsynchConnector()
 : inherited()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::RPG_Net_Client_AsynchConnector"));

  // init base class
  if (inherited::open(false,                    // pass addresses
                      ACE_Proactor::instance(), // default proactor
                      false) == -1)             // validate new connections
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Connector::open(): \"%m\", continuing\n")));
}

RPG_Net_Client_AsynchConnector::~RPG_Net_Client_AsynchConnector()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::~RPG_Net_Client_AsynchConnector"));

}

RPG_Net_StreamHandler_t*
RPG_Net_Client_AsynchConnector::make_handler(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Client_AsynchConnector::make_handler"));

  // init return value(s)
  RPG_Net_StreamHandler_t* handler_out = NULL;

  // default behavior
  ACE_NEW_NORETURN(handler_out,
                   RPG_Net_StreamHandler_t(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()));

  return handler_out;
}
