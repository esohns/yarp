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

#include "rpg_client_logger.h"

#include <ace/Global_Macros.h>
#include <ace/Log_Msg.h>
#include <ace/Synch.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"

#include "rpg_client_common.h"

RPG_Client_Logger::RPG_Client_Logger(RPG_Client_MessageStack_t* stack_in,
                                     ACE_Recursive_Thread_Mutex* lock_in)
 : inherited(),
   myMessageStack(stack_in),
   myLock(lock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Logger::RPG_Client_Logger"));

}

RPG_Client_Logger::~RPG_Client_Logger()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Logger::~RPG_Client_Logger"));

}

int
RPG_Client_Logger::open(const ACE_TCHAR* loggerKey_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Logger::start"));

  ACE_UNUSED_ARG(loggerKey_in);

  return 0;
}

int
RPG_Client_Logger::reset()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Logger::reset"));

  return 0;
}

int
RPG_Client_Logger::close()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Logger::close"));

  return 0;
}

ssize_t
RPG_Client_Logger::log(ACE_Log_Record& record_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Logger::log"));

  // sanity check(s)
  ACE_ASSERT(myLock);
  ACE_ASSERT(myMessageStack);

  std::ostringstream string_stream;
  if (record_in.print(ACE_TEXT(RPG_Common_Tools::getHostName().c_str()),
                      (RPG_COMMON_LOG_VERBOSE ? ACE_Log_Msg::VERBOSE
                                              : ACE_Log_Msg::VERBOSE_LITE),
                      string_stream) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Log_Record::print(): \"%m\", aborting\n")));

    return -1;
  } // end IF

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(*myLock);

  myMessageStack->push_back(string_stream.str());

  return 0;
}
