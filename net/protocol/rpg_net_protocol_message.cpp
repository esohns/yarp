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

#include "rpg_net_protocol_message.h"

#include "ace/Malloc_Base.h"

#include "rpg_common_macros.h"

RPG_Net_Protocol_Message::RPG_Net_Protocol_Message (unsigned int size_in)
 : inherited (1, size_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));

}

RPG_Net_Protocol_Message::RPG_Net_Protocol_Message (const RPG_Net_Protocol_Message& message_in)
 : inherited (message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));

}

RPG_Net_Protocol_Message::RPG_Net_Protocol_Message (Stream_SessionId_t sessionId_in,
                                                    ACE_Data_Block* dataBlock_in,
                                                    ACE_Allocator* messageAllocator_in,
                                                    bool incrementMessageCounter_in)
 : inherited (sessionId_in,
              dataBlock_in,        // use (don't own (!) memory of-) this data block
              messageAllocator_in, // re-use the same allocator
              incrementMessageCounter_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));

}

RPG_Net_Protocol_Message::RPG_Net_Protocol_Message (Stream_SessionId_t sessionId_in,
                                                        ACE_Allocator* messageAllocator_in)
 : inherited (sessionId_in,
              messageAllocator_in) // message block allocator
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));

}

ACE_Message_Block*
RPG_Net_Protocol_Message::duplicate (void) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Message::duplicate"));

  RPG_Net_Protocol_Message* message_p = NULL;

  // create a new Stream_MessageBase that contains unique copies of
  // the message block fields, but a (reference counted) shallow duplicate of
  // the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (inherited::message_block_allocator_ == NULL)
    ACE_NEW_NORETURN (message_p,
                      RPG_Net_Protocol_Message (*this));
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to alloc() does not really matter, as this creates
    //         a shallow copy of the existing data block
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<RPG_Net_Protocol_Message*> (inherited::message_block_allocator_->calloc (inherited::capacity (),
                                                                                                                 '\0')),
                             RPG_Net_Protocol_Message (*this));
  } // end ELSE
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
      dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate Stream_MessageBase: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment the reference counts of any continuation messages
  if (inherited::cont_)
  {
    message_p->cont_ = inherited::cont_->duplicate ();
    if (!message_p->cont_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Stream_MessageBase::duplicate(): \"%m\", aborting\n")));

      // clean up
      message_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}

std::string
RPG_Net_Protocol_Message::CommandTypeToString (Stream_CommandType_t command_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Message::CommandTypeToString"));

  ACE_UNUSED_ARG (command_in);

  return ACE_TEXT_ALWAYS_CHAR ("MB_DATA");
}
