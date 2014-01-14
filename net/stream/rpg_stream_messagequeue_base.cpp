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

#include "rpg_stream_messagequeue_base.h"

#include <rpg_common_macros.h>

RPG_Stream_MessageQueueBase::RPG_Stream_MessageQueueBase(const unsigned long& maxMessages_in)
 : inherited(maxMessages_in, // high water mark
             maxMessages_in, // low water mark
             NULL)           // notification strategy
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::RPG_Stream_MessageQueueBase"));

}

RPG_Stream_MessageQueueBase::~RPG_Stream_MessageQueueBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::~RPG_Stream_MessageQueueBase"));

}

bool
RPG_Stream_MessageQueueBase::is_full_i(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::is_full_i"));

  return (cur_count_ >= high_water_mark_);
}

void
RPG_Stream_MessageQueueBase::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("# currently queued objects: %d\n"),
             const_cast<RPG_Stream_MessageQueueBase*> (this)->message_count()));
}
