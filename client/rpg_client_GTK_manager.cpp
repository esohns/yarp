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

#include "rpg_client_GTK_manager.h"

#include "rpg_common_macros.h"
#include "rpg_common_timer_manager.h"

#include "rpg_client_defines.h"

#include <ace/Log_Msg.h>

RPG_Client_GTK_Manager::RPG_Client_GTK_Manager()
 : inherited(true)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::RPG_Client_GTK_Manager"));

}

RPG_Client_GTK_Manager::~RPG_Client_GTK_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::~RPG_Client_GTK_Manager"));

}

void
RPG_Client_GTK_Manager::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::start"));

  inherited::open(NULL);
}

void
RPG_Client_GTK_Manager::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::stop"));

  inherited::close(0);
}

bool
RPG_Client_GTK_Manager::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::Running"));

  return (inherited::thr_count() > 0);
}

int
RPG_Client_GTK_Manager::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::svc"));

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("(%t) worker starting...\n")));

  int result = -1;
  ACE_Message_Block* ace_mb = NULL;
  ACE_Time_Value no_wait = RPG_COMMON_TIME_POLICY();
  ACE_Time_Value yield_interval((RPG_CLIENT_SDL_GTKEVENT_RESOLUTION / 1000),
                                ((RPG_CLIENT_SDL_GTKEVENT_RESOLUTION % 1000) * 1000));
  while (true)
  {
    // finished ?
    result = inherited::getq(ace_mb,
                             &no_wait);
    if (result == 0)
    {
      if (!ace_mb)
        break;

      if (ace_mb->msg_type() == ACE_Message_Block::MB_STOP)
      {
        // clean up
        ace_mb->release();

        return 0; // done
      } // end IF

      // clean up
      ace_mb->release();
      ace_mb = NULL;
    } // end IF

    // run GTK event loop,  dispatch any pending events
    GDK_THREADS_ENTER();
    if (gtk_events_pending())
    {
      while (gtk_events_pending())
        if (gtk_main_iteration_do(FALSE)) // NEVER block !
        {
          //					// gtk_main_quit() has been invoked --> finished event processing

          //          // *NOTE*: as gtk_main() is never invoked, gtk_main_iteration_do ALWAYS
          //          // returns true... provide a workaround by using the gtk_quit_add hook
          //          // --> check if that has been called...
          //          // synch access
          //          {
          //            ACE_Guard<ACE_Thread_Mutex> aGuard(user_data->lock);

          //            if (user_data->gtk_main_quit_invoked)
          //              if (ACE_OS::raise(SIGINT) == -1) // --> shutdown
          //                ACE_DEBUG((LM_ERROR,
          //                           ACE_TEXT("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
          //                           SIGINT));

          break; // ignore any remaining GTK events
          //          } // end lock scope
        } // end IF
      GDK_THREADS_LEAVE();
    } // end IF
    else
    {
      GDK_THREADS_LEAVE();
      ACE_OS::sleep(yield_interval); // nothing to do... wait a little while
    } // end ELSE
  } // end WHILE

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n")));

  return -1;
}
