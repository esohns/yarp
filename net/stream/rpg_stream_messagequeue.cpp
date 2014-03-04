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

#include "rpg_stream_messagequeue.h"

#include <rpg_common_macros.h>

#include <ace/Time_Value.h>

RPG_Stream_MessageQueue::RPG_Stream_MessageQueue(const unsigned long& maxMessages_in)
 : inherited(maxMessages_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueue::RPG_Stream_MessageQueue"));

}

RPG_Stream_MessageQueue::~RPG_Stream_MessageQueue()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueue::~RPG_Stream_MessageQueue"));

}

void RPG_Stream_MessageQueue::waitForIdleState() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueue::waitForIdleState"));

  // *TODO*: find a better way to do this
  ACE_Time_Value one_second(1, 0);

  do
  {
    if (const_cast<RPG_Stream_MessageQueue*> (this)->message_count() > 0)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("waiting...\n")));

      ACE_OS::sleep(one_second);

      continue;
    } // end IF

    // OK: queue is empty (AT THE MOMENT !)
    break;
  } while (true);
}
