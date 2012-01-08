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

#include "rpg_net_protocol_IRCscanner.h"

#include <rpg_common_macros.h>

RPG_Net_Protocol_IRCScanner::RPG_Net_Protocol_IRCScanner()
 : inherited(NULL,
             NULL),
   myToken(NULL),
   myLocation(NULL),
   myDriver(NULL),
   myMessageCount(NULL),
   myMemory(NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCScanner::RPG_Net_Protocol_IRCScanner"));

}

RPG_Net_Protocol_IRCScanner::~RPG_Net_Protocol_IRCScanner ()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCScanner::~RPG_Net_Protocol_IRCScanner"));

}

void
RPG_Net_Protocol_IRCScanner::set(yy::RPG_Net_Protocol_IRCParser::semantic_type* token_in,
                                 yy::RPG_Net_Protocol_IRCParser::location_type* location_in,
								 RPG_Net_Protocol_IRCParserDriver* driver_in,
	                             unsigned long* messageCount_in,
	                             std::string* memory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCScanner::set"));

  // sanity check(s)
  ACE_ASSERT(token_in);
  ACE_ASSERT(location_in);

  myToken = token_in;
  myLocation = location_in;
  myDriver = driver_in;
  myMessageCount = messageCount_in;
  myMemory = memory_in;

  myIsInitialized = true;
}
