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

#include "rpg_net_protocol_connection_manager.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

RPG_Net_Protocol_Connection_Manager::RPG_Net_Protocol_Connection_Manager ()
 : inherited ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Connection_Manager::RPG_Net_Protocol_Connection_Manager"));

}

void
RPG_Net_Protocol_Connection_Manager::remove (Common_IDispatch* dispatch_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Connection_Manager::remove"));

  std::vector<Common_IDispatch*>::iterator iterator =
    std::find (subscribers_.begin (), subscribers_.end (), dispatch_in);
  if (iterator != subscribers_.end ())
    subscribers_.erase (iterator);
}

bool
RPG_Net_Protocol_Connection_Manager::register_ (ICONNECTION_T* connection_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Connection_Manager::register_"));

  bool result = inherited::register_ (connection_in);

  // notify subscriber(s)
  for (std::vector<Common_IDispatch*>::iterator iterator = subscribers_.begin ();
       iterator != subscribers_.end ();
       ++iterator)
    (*iterator)->dispatch (connection_in);

  return result;
}
