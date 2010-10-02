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

#include "rpg_stream_task_synch.h"

#include <rpg_common_macros.h>

#include <ace/Message_Block.h>
#include <ace/Time_Value.h>

RPG_Stream_TaskSynch::RPG_Stream_TaskSynch()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::RPG_Stream_TaskSynch"));

}

RPG_Stream_TaskSynch::~RPG_Stream_TaskSynch()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::~RPG_Stream_TaskSynch"));

}

int
RPG_Stream_TaskSynch::put(ACE_Message_Block* mb_in,
                      ACE_Time_Value* tv_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::put"));

  ACE_UNUSED_ARG(tv_in);

  // NOTE: ignore this return value (it's only used in asynchronous mode...)
  bool stopProcessing = false;

  // "borrow" the calling thread to do our work...
  inherited::handleMessage(mb_in,
                           stopProcessing);

  return 0;
}

int
RPG_Stream_TaskSynch::open(void* args_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::open"));

  ACE_UNUSED_ARG(args_in);

//   // debug info
//   if (module())
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("module \"%s\" has no worker thread...\n"),
//                ACE_TEXT_ALWAYS_CHAR(name())));
//   } // end IF
//   else
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("no worker thread\n")));
//   } // end ELSE

  // nothing to do...
  return 0;
}

int
RPG_Stream_TaskSynch::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::close"));

  ACE_UNUSED_ARG(arg_in);

  // nothing to do...
  return 0;
}

int
RPG_Stream_TaskSynch::module_closed(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::module_closed"));

  // *NOTE*: this method is invoked by an external thread
  // either from the ACE_Module dtor or during explicit ACE_Module::close()

  // nothing to do...
  return 0;
}

void
RPG_Stream_TaskSynch::waitForIdleState() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_TaskSynch::waitForIdleState"));

  // just a stub, there's nothing to do...
}
