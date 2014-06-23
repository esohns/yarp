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

#include "rpg_common_macros.h"

template <typename TaskSynchType,
          typename TimePolicyType>
RPG_Stream_MessageQueueBase<TaskSynchType,
                            TimePolicyType>::RPG_Stream_MessageQueueBase(const unsigned int& maxMessages_in)
 : inherited(maxMessages_in, // high water mark
             maxMessages_in, // low water mark
             NULL)           // notification strategy
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::RPG_Stream_MessageQueueBase"));

}

template <typename TaskSynchType,
          typename TimePolicyType>
RPG_Stream_MessageQueueBase<TaskSynchType,
                            TimePolicyType>::~RPG_Stream_MessageQueueBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::~RPG_Stream_MessageQueueBase"));

}

template <typename TaskSynchType,
          typename TimePolicyType>
bool
RPG_Stream_MessageQueueBase<TaskSynchType,
                            TimePolicyType>::is_full_i(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::is_full_i"));

  return (inherited::cur_count_ >= inherited::high_water_mark_);
}

template <typename TaskSynchType,
          typename TimePolicyType>
void
RPG_Stream_MessageQueueBase<TaskSynchType,
                            TimePolicyType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageQueueBase::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("# currently queued objects: %d\n"),
             const_cast<own_type*>(this)->message_count()));
}
