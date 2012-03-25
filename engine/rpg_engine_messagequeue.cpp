/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"

#include "rpg_engine_messagequeue.h"

#include <rpg_common_macros.h>

RPG_Engine_MessageQueue::RPG_Engine_MessageQueue(const unsigned long& maxMessages_in)
: inherited(maxMessages_in, // high water mark
            maxMessages_in, // low water mark
            NULL)           // notification strategy
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_MessageQueue::RPG_Engine_MessageQueue"));

}

RPG_Engine_MessageQueue::~RPG_Engine_MessageQueue()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_MessageQueue::~RPG_Engine_MessageQueue"));

}

bool
RPG_Engine_MessageQueue::is_full_i(void)
{
  ACE_TRACE("RPG_Engine_MessageQueue::is_full_i");

  return (cur_count_ >= high_water_mark_);
}
