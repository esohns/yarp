/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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

#include "rpg_net_protocol_IRCbisect.h"

#include <rpg_common_macros.h>

RPG_Net_Protocol_IRCBisect::RPG_Net_Protocol_IRCBisect()
 : inherited(NULL,
             NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCBisect::RPG_Net_Protocol_IRCBisect"));

}

RPG_Net_Protocol_IRCBisect::~RPG_Net_Protocol_IRCBisect ()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCBisect::~RPG_Net_Protocol_IRCBisect"));

}

void
RPG_Net_Protocol_IRCBisect::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCScanner::init"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);

  // OK
  myIsInitialized = true;
}
