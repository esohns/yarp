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

#include "rpg_net_protocol_sockethandler.h"

#include "rpg_common_macros.h"

#include "rpg_stream_imodule.h"

#include "rpg_net_connection_manager.h"

#include "rpg_net_protocol_common.h"

RPG_Net_Protocol_SocketHandler::RPG_Net_Protocol_SocketHandler()
 : inherited(RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON::instance())
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::RPG_Net_Protocol_SocketHandler"));

}

RPG_Net_Protocol_SocketHandler::~RPG_Net_Protocol_SocketHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::~RPG_Net_Protocol_SocketHandler"));

	myStream.waitForCompletion();
}

void
RPG_Net_Protocol_SocketHandler::ping()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::ping"));

  ACE_ASSERT(false);
}

int
RPG_Net_Protocol_SocketHandler::open(void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::open"));

  // *NOTE*: fini() on the stream invokes close() which will reset any module's
  // writer/reader tasks --> in order to allow module reuse, reset this here !
  ACE_ASSERT(myUserData.module);
  IMODULE_TYPE* imodule_handle = dynamic_cast<IMODULE_TYPE*>(myUserData.module);
  if (!imodule_handle)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to dynamic_cast<RPG_Stream_IModule>, aborting\n")));

    return -1;
  } // end IF
  try
  {
    imodule_handle->reset();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Stream_IModule::reset(), aborting\n")));

    return -1;
  }

  // init/start stream, register reading data with reactor...
  // --> done by the base class
  if (inherited::open(arg_in))
  {
    // MOST PROBABLE REASON: too many open connections...

    if (ACE_OS::last_error() != EBUSY)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to inherited::open(): \"%m\", aborting\n")));

    // reactor will invoke close() --> handle_close()
    return -1;
  } // end IF

//   // OK: start a worker
//   if (activate((THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
//                1,                           // # threads
//                0,                           // force spawning
//                ACE_DEFAULT_THREAD_PRIORITY, // priority
//                -1,                          // group id
//                NULL,                        // corresp. task --> use 'this'
//                NULL,                        // thread handle(s)
//                NULL,                        // thread stack(s)
//                NULL,                        // thread stack size(s)
//                NULL))                       // thread id(s)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to activate(): \"%m\", aborting\n")));
//
//     // reactor will invoke close() --> handle_close()
//     return -1;
//   } // end IF

  return 0;
}

int
RPG_Net_Protocol_SocketHandler::handle_close(ACE_HANDLE handle_in,
                                             ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::handle_close"));

  // connection shuting down, signal any worker(s)
  if (inherited::myUserData.useThreadPerConnection)
    shutdown();

  // *NOTE*: this MAY "delete this"...
  return inherited::handle_close(handle_in,
                                 mask_in);
}

void
RPG_Net_Protocol_SocketHandler::shutdown()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_SocketHandler::shutdown"));

  ACE_Message_Block* stop_mb = NULL;
  ACE_NEW_NORETURN(stop_mb,
                   ACE_Message_Block(0,                                  // size
                                     ACE_Message_Block::MB_STOP,         // type
                                     NULL,                               // continuation
                                     NULL,                               // data
                                     NULL,                               // buffer allocator
                                     NULL,                               // locking strategy
                                     ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
                                     ACE_Time_Value::zero,               // execution time
                                     ACE_Time_Value::max_time,           // deadline time
                                     NULL,                               // data block allocator
                                     NULL));                             // message allocator)
  if (!stop_mb)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate ACE_Message_Block: \"%m\", aborting\n")));

    return;
  } // end IF

  try
  {
    if (myStream.head()->reader()->put(stop_mb, NULL) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to ACE_Task::put(): \"%m\", continuing\n")));

			// clean up
			stop_mb->release();
    } // end IF
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_Task::put(): \"%m\", aborting\n")));

		// clean up
    stop_mb->release();

    return;
  }
}
