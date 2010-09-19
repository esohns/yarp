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

#ifndef RPG_STREAM_MESSAGEQUEUE_H
#define RPG_STREAM_MESSAGEQUEUE_H

#include "rpg_stream_messagequeue_base.h"

#include <ace/Global_Macros.h>

class RPG_Stream_MessageQueue
 : public RPG_Stream_MessageQueueBase
{
 public:
  RPG_Stream_MessageQueue(const unsigned long&); // max number of queued buffers
  virtual ~RPG_Stream_MessageQueue();

  // implement RPG_Stream_IMessageQueueSynch
  virtual void waitForIdleState() const;

 private:
  typedef RPG_Stream_MessageQueueBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageQueue());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageQueue(const RPG_Stream_MessageQueue&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageQueue& operator=(const RPG_Stream_MessageQueue&));
};

#endif
