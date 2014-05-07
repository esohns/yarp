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

#include "rpg_net_asynchsockethandler.h"

#include "rpg_common_macros.h"

#include "rpg_net_connection_manager.h"

RPG_Net_AsynchSocketHandler::RPG_Net_AsynchSocketHandler()
 : inherited(RPG_NET_CONNECTIONMANAGER_SINGLETON::instance())
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler::RPG_Net_AsynchSocketHandler"));

}

RPG_Net_AsynchSocketHandler::~RPG_Net_AsynchSocketHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler::~RPG_Net_AsynchSocketHandler"));

}

void
RPG_Net_AsynchSocketHandler::ping()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler::ping"));

  inherited::myStream.ping();
}

void
RPG_Net_AsynchSocketHandler::open(ACE_HANDLE handle_in,
                                  ACE_Message_Block& messageBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler::open"));

  // step1: prepare data processing stream
  inherited::myUserData.sessionID = inherited::getID(); // (== socket handle)

  // step2: init/start stream, register reading data with reactor...
  inherited::open(handle_in,
                  messageBlock_in);
}
