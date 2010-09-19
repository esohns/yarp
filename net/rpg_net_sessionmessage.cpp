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

#include "rpg_net_sessionmessage.h"

#include <ace/Malloc_Base.h>

RPG_Net_SessionMessage::RPG_Net_SessionMessage(const unsigned long& sessionID_in,
                                               const RPG_Stream_SessionMessageType& messageType_in,
                                               RPG_Net_StreamConfig*& config_inout)
 : inherited(sessionID_in,
             messageType_in,
             config_inout)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SessionMessage::RPG_Net_SessionMessage"));

}

RPG_Net_SessionMessage::RPG_Net_SessionMessage(const RPG_Net_SessionMessage& message_in)
 : inherited(message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SessionMessage::RPG_Net_SessionMessage"));

}

RPG_Net_SessionMessage::RPG_Net_SessionMessage(ACE_Allocator* messageAllocator_in)
 : inherited(messageAllocator_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SessionMessage::RPG_Net_SessionMessage"));

}

RPG_Net_SessionMessage::RPG_Net_SessionMessage(ACE_Data_Block* dataBlock_in,
                                               ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,
             messageAllocator_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SessionMessage::RPG_Net_SessionMessage"));

}

RPG_Net_SessionMessage::~RPG_Net_SessionMessage()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SessionMessage::~RPG_Net_SessionMessage"));

}

ACE_Message_Block*
RPG_Net_SessionMessage::duplicate(void) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SessionMessage::duplicate"));

  RPG_Net_SessionMessage* nb = NULL;

  // *NOTE*: create a new RPG_Net_SessionMessage that contains unique copies of
  // the message block fields, but a reference counted duplicate of
  // the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (message_block_allocator_ == NULL)
  {
    // uses the copy ctor
    ACE_NEW_RETURN(nb,
                   RPG_Net_SessionMessage(*this),
                   NULL);
  } // end IF

  // *WARNING*:we tell the allocator to return a RPG_Net_SessionMessage
  // by passing a 0 as argument to malloc()...
  ACE_NEW_MALLOC_RETURN(nb,
                        ACE_static_cast(RPG_Net_SessionMessage*,
                                        message_block_allocator_->malloc(0)),
                        RPG_Net_SessionMessage(*this),
                        NULL);

  // increment the reference counts of all the continuation messages
  if (cont_)
  {
    nb->cont_ = cont_->duplicate();

    // If things go wrong, release all of our resources and return
    if (nb->cont_ == 0)
    {
      nb->release();
      nb = NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return nb;
}
