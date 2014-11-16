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

#include "rpg_net_sockethandler_base.h"

#include "rpg_common_macros.h"

#include "ace/Log_Msg.h"

RPG_Net_SocketHandlerBase::RPG_Net_SocketHandlerBase()
 : allocator_ (NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::RPG_Net_SocketHandlerBase"));

}

RPG_Net_SocketHandlerBase::~RPG_Net_SocketHandlerBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::~RPG_Net_SocketHandlerBase"));

}

void
RPG_Net_SocketHandlerBase::init (ACE_Allocator* allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SocketHandlerBase::init"));

  allocator_ = allocator_in;
}
