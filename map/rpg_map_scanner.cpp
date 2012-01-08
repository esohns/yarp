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

#include "rpg_map_scanner.h"

#include <rpg_common_macros.h>

RPG_Map_Scanner::RPG_Map_Scanner()
 : inherited(NULL,
             NULL),
   myToken(NULL),
   myLocation(NULL),
   myDriver(NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Scanner::RPG_Map_Scanner"));

}

RPG_Map_Scanner::~RPG_Map_Scanner ()
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Scanner::~RPG_Map_Scanner"));

}

void
RPG_Map_Scanner::set(yy::RPG_Map_Parser::semantic_type* token_in,
                     yy::RPG_Map_Parser::location_type* location_in,
					 RPG_Map_ParserDriver* driver_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Map_Scanner::set"));

  // sanity check(s)
  ACE_ASSERT(token_in);
  ACE_ASSERT(location_in);

  myToken = token_in;
  myLocation = location_in;
  myDriver = driver_in;

  myIsInitialized = (driver_in != NULL);
}
