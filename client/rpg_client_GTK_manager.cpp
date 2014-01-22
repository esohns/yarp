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

  if (close(0) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_GTK_Manager::close(0): \"%m\", continuing\n")));

  if (inherited::wait() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", continuing\n")));
}

bool
RPG_Client_GTK_Manager::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::isRunning"));

  return (inherited::thr_count() > 0);
}

int
RPG_Client_GTK_Manager::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::svc"));

  // *NOTE*: this method may be invoked
  // - by an external thread closing down the active object
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    case 0:
    { // check specifically for the second case...
      if (ACE_OS::thr_equal(ACE_Thread::self(),
                            inherited::last_thread()))
        break;

      // *WARNING*: falls through !
    }
    case 1:
    {
      if (inherited::thr_count() == 0)
        return 0; // nothing to do

      if (gtk_main_level() > 0)
        gtk_main_quit();

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument: %u, aborting\n"),
                 arg_in));

      return -1;
    }
  } // end SWITCH

  return 0;
}

int
RPG_Client_GTK_Manager::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTK_Manager::svc"));

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("(%t) worker starting...\n")));

  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  // gtk_main_quit() has been called...

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("leaving GTK event dispatch...\n")));

  return 0;
}
