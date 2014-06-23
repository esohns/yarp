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

#include "rpg_common_signalhandler.h"

#include "rpg_common_macros.h"
#include "rpg_common_isignal.h"
#include "rpg_common_tools.h"

#include <ace/Assert.h>
#include <ace/OS.h>
#include <ace/Reactor.h>
#include <ace/Proactor.h>
#include <ace/Log_Msg.h>

#include <string>

RPG_Common_SignalHandler::RPG_Common_SignalHandler(RPG_Common_ISignal* handler_in,
                                                   const bool& useReactor_in)
 : inherited(),
   inherited2(NULL,                            // default reactor
              ACE_Event_Handler::LO_PRIORITY), // priority
   myHandler(handler_in),
   myUseReactor(useReactor_in),
   mySignal(-1)
#if defined(ACE_WIN32) || defined(ACE_WIN64)
   ,mySigInfo(ACE_INVALID_HANDLE),
   myUContext(-1)
#else
   //mySigInfo(),
   //myUContext()
#endif
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SignalHandler::RPG_Common_SignalHandler"));

  // sanity check
  ACE_ASSERT(myHandler);

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  ACE_OS::memset(&mySigInfo, 0, sizeof(mySigInfo));
  ACE_OS::memset(&myUContext, 0, sizeof(myUContext));
#endif
}

RPG_Common_SignalHandler::~RPG_Common_SignalHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SignalHandler::~RPG_Common_SignalHandler"));

}

int
RPG_Common_SignalHandler::handle_signal(int signal_in,
                                        siginfo_t* info_in,
                                        ucontext_t* context_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SignalHandler::handle_signal"));

	// init return value
	int result = -1;

  // *IMPORTANT NOTE*: in signal context, most actions are forbidden, so save
  // the state and notify the reactor/proactor for callback instead (see below)

  // save state
  mySignal = signal_in;
  ACE_OS::memset(&mySigInfo, 0, sizeof(mySigInfo));
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  mySigInfo.si_handle_ = static_cast<ACE_HANDLE>(info_in);
#else
  if (info_in)
    mySigInfo = *info_in;
#endif
  if (context_in)
    myUContext = *context_in;

  // schedule an event (see below)
	if (myUseReactor)
	{
		result = ACE_Reactor::instance()->notify(this,
		                                         ACE_Event_Handler::EXCEPT_MASK,
																					   NULL);
		if (result == -1)
			ACE_DEBUG((LM_ERROR,
			           ACE_TEXT("failed to ACE_Reactor::notify: \"%m\", aborting\n")));
	} // end IF
	else
	{
		result = ACE_Proactor::instance()->schedule_timer(*this,                 // event handler
			                                                NULL,                  // act
																					            ACE_Time_Value::zero); // expire immediately
		if (result == -1)
			ACE_DEBUG((LM_ERROR,
			           ACE_TEXT("failed to ACE_Proactor::schedule_timer: \"%m\", aborting\n")));
	} // end ELSE

  return result;
}

void
RPG_Common_SignalHandler::handle_time_out(const ACE_Time_Value& time_in,
                                          const void* act_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SignalHandler::handle_time_out"));

	ACE_UNUSED_ARG(time_in);
	ACE_UNUSED_ARG(act_in);

	if (handle_exception(ACE_INVALID_HANDLE) == -1)
		ACE_DEBUG((LM_ERROR,
		           ACE_TEXT("failed to RPG_Common_SignalHandler::handle_exception: \"%m\", continuing\n")));
}

int
RPG_Common_SignalHandler::handle_exception(ACE_HANDLE handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_SignalHandler::handle_exception"));

  ACE_UNUSED_ARG(handle_in);

  // debug info
  std::string information;
  RPG_Common_Tools::retrieveSignalInfo(mySignal,
                                       mySigInfo,
                                       &myUContext,
                                       information);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%D: received [%S]: %s\n"),
             mySignal,
             information.c_str()));

	bool success = true;
	try
	{
		success = myHandler->handleSignal(mySignal);
	}
	catch (...)
	{
		ACE_DEBUG((LM_ERROR,
			         ACE_TEXT("caught exception in RPG_Common_ISignal::handleSignal: \"%m\", continuing\n")));
	}

  return (success ? 0 : -1);
}
