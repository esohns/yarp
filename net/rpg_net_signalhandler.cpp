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

#include <rpg_common_icontrol.h>

#include <ace/OS.h>
#include <ace/Reactor.h>
#include <ace/Log_Msg.h>

#include <sstream>

RPG_Net_SignalHandler::RPG_Net_SignalHandler(RPG_Common_IControl* control_in)
 : inherited(ACE_Reactor::instance(),         // corresp. reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myControl(control_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::RPG_Net_SignalHandler"));

//   // sanity check
//   ACE_ASSERT(myControl);
}

RPG_Net_SignalHandler::~RPG_Net_SignalHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::~RPG_Net_SignalHandler"));

}

int
RPG_Net_SignalHandler::handle_signal(int signal_in,
                                     siginfo_t* info_in,
                                     ucontext_t* context_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::handle_signal"));

  ACE_UNUSED_ARG(context_in);

  bool stop_reactor = false;

  // sanity check
  if (info_in == NULL)
  {
    // *NOTE*: we want to keep a record of this incident...
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("%D: received [%S], but no siginfo_t was available, continuing\n"),
               signal_in));
  } // end IF
  else
  {
    // collect some context information...
    std::string information;
    retrieveSignalInfo(signal_in,
                       *info_in,
//                       (context_in ? *context_in : NULL),
                       information);

    // *NOTE*: we want to keep a record of this incident...
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("%D: received [%S]: %s\n"),
               signal_in,
               information.c_str()));
  } // end ELSE

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
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("shutting down...\n")));

      // shutdown...
      stop_reactor = true;

      break;
    }
    default:
    {
      // *NOTE*: this means that the set of signals we registered for
      // does not correspond to this implementation --> check main.cpp !
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown signal: \"%S\", continuing\n"),
                 signal_in));

      break;
    }
  } // end SWITCH

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
      // --> application will probably hang ! :-(
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%s\", continuing\n"),
                 ACE_OS::strerror(ACE_OS::last_error())));
    } // end IF

    // step2: signal our controller (if any)
    if (myControl)
    {
      try
      {
        myControl->stop();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Common_IControl::stop(), continuing\n")));
      }
    } // end IF

    // de-register ourselves from the reactor...
    return -1;
  } // end IF

  return 0;
}

void
RPG_Net_SignalHandler::retrieveSignalInfo(const int& signal_in,
                                          const siginfo_t& info_in,
//                                           const ucontext_t& context_in,
                                          std::string& information_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::retrieveSignalInfo"));

  // init return value
  information_out.resize(0);

  std::ostringstream information;
  information.clear();

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // step0: retrieve some basic information
  information << ACE_TEXT("PID/UID: ");
  information << info_in.si_pid;
  information << ACE_TEXT("/");
  information << info_in.si_uid;

  // (try to) get user name
  char pw_buf[BUFSIZ];
  passwd pw_struct;
  passwd* pw_ptr = NULL;
  // *PORTABILITY*: this isn't completely portable... (man getpwuid_r)
  if (::getpwuid_r(info_in.si_uid,
                   &pw_struct,
                   pw_buf,
                   sizeof(pw_buf),
                   &pw_ptr))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::getpwuid_r(%d) : \"%s\", continuing\n"),
               info_in.si_uid,
               ACE_OS::strerror(ACE_OS::last_error())));
  } // end IF
  else
  {
    information << ACE_TEXT("[\"");
    information << pw_struct.pw_name;
    information << ACE_TEXT("\"]");
  } // end ELSE

  information << ACE_TEXT(", errno: ");
  information << info_in.si_errno;
  information << ACE_TEXT("[\"");
  information << ACE_OS::strerror(info_in.si_errno);
  information << ACE_TEXT("\"], code: ");

  // step1: retrieve signal code...
  switch (info_in.si_code)
  {
    case SI_TIMER:
    {
      information << ACE_TEXT("SI_TIMER");

      break;
    }
    case SI_USER:
    {
      information << ACE_TEXT("SI_USER");

      break;
    }
    case SI_KERNEL:
    {
      information << ACE_TEXT("SI_KERNEL");

      break;
    }
    case SI_QUEUE:
    {
      information << ACE_TEXT("SI_QUEUE");

      break;
    }
    case SI_MESGQ:
    {
      information << ACE_TEXT("SI_MESGQ");

      break;
    }
    case SI_ASYNCIO:
    {
      information << ACE_TEXT("SI_ASYNCIO");

      break;
    }
    case SI_SIGIO:
    {
      information << ACE_TEXT("SI_SIGIO");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid/unknown signal (code: %d), continuing\n"),
                 info_in.si_code));

      information << ACE_TEXT("SI_XXX[");
      information << info_in.si_code;
      information << ACE_TEXT("]");

      break;
    }
  } // end SWITCH

  // step2: retrieve more (signal-dependant) information
  switch (signal_in)
  {
    case SIGFPE:
    {
      switch (info_in.si_code)
      {
        case FPE_INTDIV:
        case FPE_FLTDIV:
        {
          information << ACE_TEXT(", divide by zero: ");
          information << info_in.si_addr;

          break;
        }
        case FPE_INTOVF:
        case FPE_FLTOVF:
        {
          information << ACE_TEXT(", numeric overflow: ");
          information << info_in.si_addr;

          break;
        }
        default:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                     info_in.si_code));

          break;
        }
      } // end SWITCH

      break;
    }
    case SIGSEGV:
    {
      // *TODO*: more data ?
      information << ACE_TEXT(", segmentation fault: ");
      information << info_in.si_addr;

      break;
    }
    case SIGCHLD:
    {
      information << ACE_TEXT(", child process has exited (consumed: ");
      information << info_in.si_utime;
      information << ACE_TEXT("/");
      information << info_in.si_stime;
      information << ACE_TEXT(" time, exit status: ");
      information << info_in.si_status;

      break;
    }
    default:
    {
      // *TODO*: add more ...
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("no additional information for signal: \"%S\"...\n"),
                 signal_in));

      break;
    }
  } // end SWITCH
#else
  // *IMPORTANT NOTE*: under Windows (TM), we only have the handle(s)...
  ACE_UNUSED_ARG(signal_in);

  information << ACE_TEXT(", signalled handle: ");
  information << info_in.si_handle_;
  information << ACE_TEXT(", array of signalled handle(s): ");
  information << info_in.si_handles_;
#endif

  // OK: set return value
  information_out = information.str();
}
