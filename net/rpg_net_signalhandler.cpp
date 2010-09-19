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

#include "rpg_net_signalhandler.h"

#include "rpg_net_common_tools.h"

#include <rpg_common_macros.h>
#include <rpg_common_icontrol.h>

#include <ace/Reactor.h>
#include <ace/Log_Msg.h>

#include <sstream>

RPG_Net_SignalHandler::RPG_Net_SignalHandler(RPG_Common_IControl* control_in,
                                             RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>* report_in)
 : inherited(ACE_Reactor::instance(),         // corresp. reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myControl(control_in),
   myReport(report_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SignalHandler::RPG_Net_SignalHandler"));

//   // sanity check
//   ACE_ASSERT(myControl);
}

RPG_Net_SignalHandler::~RPG_Net_SignalHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SignalHandler::~RPG_Net_SignalHandler"));

}

int
RPG_Net_SignalHandler::handle_signal(int signal_in,
                                     siginfo_t* info_in,
                                     ucontext_t* context_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_SignalHandler::handle_signal"));

  // debug info
  if (info_in == NULL)
  {
    // *PORTABILITY*: tracing in a signal handler context is not portable
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%D: received [%S], but no siginfo_t was available, continuing\n"),
               signal_in));
  } // end IF
  else
  {
    // collect some context information...
    std::string information;
    RPG_Net_Common_Tools::retrieveSignalInfo(signal_in,
                                             *info_in,
                                             context_in,
                                             information);

    // *PORTABILITY*: tracing in a signal handler context is not portable
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%D: received [%S]: %s\n"),
               signal_in,
               information.c_str()));
  } // end ELSE

  bool stop_reactor = false;
  bool report = false;
  switch (signal_in)
  {
    case SIGINT:
    case SIGTERM:
    // *PORTABILITY*: this isn't portable: on Windows SIGQUIT and SIGHUP are not defined,
    // so we handle SIGBREAK (21) and SIGABRT (22) instead...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGHUP:
    case SIGQUIT:
#else
    case SIGBREAK:
    case SIGABRT:
#endif
    {
//       // *PORTABILITY*: tracing in a signal handler context is not portable
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("shutting down...\n")));

      // shutdown...
      stop_reactor = true;

      break;
    }
    case SIGUSR1:
    {
      // dump statistics
      report = true;

      break;
    }
    default:
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown signal: \"%S\", continuing\n"),
                 signal_in));

      break;
    }
  } // end SWITCH

  // report ?
  if (report)
  {
    // step1: invoke our reporter (if any)
    if (myReport)
    {
      try
      {
        myReport->report();
      }
      catch (...)
      {
        // *PORTABILITY*: tracing in a signal handler context is not portable
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Common_IStatistic::report(), continuing\n")));
      }
    } // end IF
  } // end IF

  // ...shutdown ?
  if (stop_reactor)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets (listeners), maintenance timers...
    // - break out of any (blocking) calls
    // --> (try to) terminate in a well-behaved manner

    // step1: stop reactor
    if (reactor()->end_event_loop() == -1)
    {
      // *PORTABILITY*: tracing in a signal handler context is not portable
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
    } // end IF

    // step2: invoke our controller (if any)
    if (myControl)
    {
      try
      {
        myControl->stop();
      }
      catch (...)
      {
        // *PORTABILITY*: tracing in a signal handler context is not portable
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Common_IControl::stop(), continuing\n")));
      }
    } // end IF

    // de-register ourselves from the reactor...
    return -1;
  } // end IF

  return 0;
}
