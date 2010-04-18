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

#include "rpg_net_module_IRCparser.h"

#include <rpg_net_message.h>

#include <stream_iallocator.h>

RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser()
 : //inherited(),
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser"));

}

RPG_Net_Protocol_Module_IRCParser::~RPG_Net_Protocol_Module_IRCParser()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::~RPG_Net_Protocol_Module_IRCParser"));

}

const bool
RPG_Net_Protocol_Module_IRCParser::init(Stream_IAllocator* allocator_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    myAllocator = NULL;
    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCParser::handleDataMessage(Stream_MessageBase*& message_inout,
                                                     bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::handleDataMessage"));

  // init return value(s), default behavior is to pass all messages along...
  // --> we don't want that !
  passMessageDownstream_out = false;

  // sanity check(s)
  ACE_ASSERT(message_inout);
  ACE_ASSERT(myIsInitialized);

}
