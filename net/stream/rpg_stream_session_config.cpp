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
#include "stdafx.h"

#include "rpg_stream_session_config.h"

#include <ace/Guard_T.h>
#include <ace/Synch.h>

#include "rpg_common_macros.h"

#include "rpg_stream_tools.h"

RPG_Stream_SessionConfig::RPG_Stream_SessionConfig(const void* data_in,
																									 const ACE_Time_Value& startOfSession_in,
																									 const bool& userAbort_in)
 : inherited(1,     // initial count
             true), // delete on zero ?
   myUserData(data_in),
   myStartOfSession(startOfSession_in),
   myUserAbort(userAbort_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionConfig::RPG_Stream_SessionConfig"));

}

RPG_Stream_SessionConfig::~RPG_Stream_SessionConfig()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionConfig::~RPG_Stream_SessionConfig"));

}

const void*
RPG_Stream_SessionConfig::getUserData() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionConfig::getUserData"));

  return myUserData;
}

ACE_Time_Value
RPG_Stream_SessionConfig::getStartOfSession() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionConfig::getStartOfSession"));

  return myStartOfSession;
}

bool
RPG_Stream_SessionConfig::getUserAbort() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionConfig::getUserAbort"));

  return myUserAbort;
}

void
RPG_Stream_SessionConfig::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionConfig::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("user data: %@, start of session: %s%s\n"),
             myUserData,
             RPG_Stream_Tools::timestamp2LocalString(myStartOfSession).c_str(),
             (myUserAbort ? ACE_TEXT(" [user abort !]")
                          : ACE_TEXT(""))));
}
