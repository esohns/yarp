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

#include "rpg_net_protocol_tools.h"

#include "rpg_common_macros.h"

#include <ace/Message_Block.h>
#include <ace/Malloc_Base.h>

#include <sstream>

RPG_Net_Protocol_Message::RPG_Net_Protocol_Message(const RPG_Net_Protocol_Message& message_in)
 : inherited(message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));

}

RPG_Net_Protocol_Message::RPG_Net_Protocol_Message(ACE_Data_Block* dataBlock_in,
                                                   ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,        // use (don't own !) this data block
             messageAllocator_in) // use this when destruction is imminent...
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));

}

// RPG_Net_Protocol_Message::RPG_Net_Protocol_Message(ACE_Allocator* messageAllocator_in)
//  : inherited(messageAllocator_in,
//              true), // usually, we want to increment the running message counter...
//    myIsInitialized(false) // not initialized --> call init() !
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::RPG_Net_Protocol_Message"));
//
// }

RPG_Net_Protocol_Message::~RPG_Net_Protocol_Message()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::~RPG_Net_Protocol_Message"));

  // *NOTE*: will be called just BEFORE we're passed back to the allocator
}

int
RPG_Net_Protocol_Message::getCommand() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::getCommand"));

  // sanity check(s)
  ACE_ASSERT(getData());

  switch (getData()->command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
      return RPG_Net_Protocol_Tools::IRCCommandString2Type(*getData()->command.string);
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
      return getData()->command.numeric;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command type (was: %d), aborting\n"),
                 getData()->command.discriminator));

      break;
    }
  } // end SWITCH

  return RPG_Net_Protocol_IRCMessage::RPG_NET_PROTOCOL_COMMANDTYPE_INVALID;
}

void
RPG_Net_Protocol_Message::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::dump_state"));

  std::ostringstream converter;

  // count continuations
  unsigned int count = 1;
  std::string info;
  for (const ACE_Message_Block* source = this;
       source != NULL;
       source = source->cont(), count++)
  {
    converter.str(ACE_TEXT(""));
    converter << count;
    info += converter.str();
    info += ACE_TEXT("# [");
    converter.str(ACE_TEXT(""));
    converter << source->length();
    info += converter.str();
    info += ACE_TEXT(" byte(s)]: \"");
    info.append(source->rd_ptr(), source->length());
    info += ACE_TEXT("\"\n");
  } // end FOR
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("***** Message (ID: %u, %u byte(s)) *****\n%s"),
             getID(),
             total_length(),
             info.c_str()));
  // delegate to base
  inherited::dump_state();
}

void
RPG_Net_Protocol_Message::crunch()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::crunch"));

  // sanity check
  // *WARNING*: this is NOT enough, it's a race.
  // Anyway, there may be trailing messages and/or pieces referencing the same
  // buffer...
  // --> in fact, that should be the norm
//   ACE_ASSERT(reference_count() == 1);
  // ... assuming stream processing is indeed single-threaded, then the
  // reference count at this stage SHOULD be 2: us, and the next,
  // trailing "message head". (Of course, it COULD be just "us"...)
  if (reference_count() <= 2)
  {  // step1: align rd_ptr() with base()
    if (inherited::crunch())
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to ACE_Message_Block::crunch(): \"%m\", aborting\n")));

      return;
    } // end IF
  } // end IF

  // step2: copy the data
  ACE_Message_Block* source = NULL;
  size_t amount = 0;
  for (source = cont();
       source != NULL;
       source = source->cont())
  {
    amount = (space() < source->length() ? space()
                                         : source->length());
    if (copy(source->rd_ptr(), amount))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::copy(): \"%m\", aborting\n")));

      return;
    } // end IF

    // adjust read pointer accordingly
    source->rd_ptr(amount);
  } // end FOR

  // step3: release any thus obsoleted continuations
  source = cont();
  ACE_Message_Block* prev = this;
  ACE_Message_Block* obsolete = NULL;
  do
  {
    // finished ?
    if (source == NULL)
      break;

    if (source->length() == 0)
    {
      obsolete = source;
      source = source->cont();
      prev->cont(source);
      obsolete->release();
    } // end IF
  } while (true);
}

ACE_Message_Block*
RPG_Net_Protocol_Message::duplicate(void) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::duplicate"));

  RPG_Net_Protocol_Message* new_message = NULL;

  // create a new RPG_Net_Protocol_Message that contains unique copies of
  // the message block fields, but a (reference counted) shallow duplicate of
  // the ACE_Data_Block.

  // if there is no allocator, use the standard new and delete calls.
  if (message_block_allocator_ == NULL)
  {
    ACE_NEW_RETURN(new_message,
                   RPG_Net_Protocol_Message(*this), // invoke copy ctor
                   NULL);
  } // end IF
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to malloc SHOULDN'T really matter, as this will be
    // a "shallow" copy which just references our data block...
    // *IMPORTANT NOTE*: cached allocators require the object size as argument to
    // malloc() (instead of its internal "capacity()" !)
    // *TODO*: (depending on the allocator) we senselessly allocate a datablock
    // anyway, only to immediately release it again...
    ACE_NEW_MALLOC_RETURN(new_message,
//                          static_cast<RPG_Net_Protocol_Message*>(message_block_allocator_->malloc(capacity())),
                          static_cast<RPG_Net_Protocol_Message*>(message_block_allocator_->malloc(sizeof(RPG_Net_Protocol_Message))),
                          RPG_Net_Protocol_Message(*this),
                          NULL);
  } // end ELSE

  // increment the reference counts of all the continuation messages
  if (cont_)
  {
    new_message->cont_ = cont_->duplicate();

    // If things go wrong, release all of our resources and return
    if (new_message->cont_ == NULL)
    {
      new_message->release();
      new_message = NULL;
    } // end IF
  } // end IF

  // *NOTE*: the "clone" always starts un-initialized --> use inherited::init()

  return new_message;
}

std::string
RPG_Net_Protocol_Message::commandType2String(const RPG_Net_Protocol_CommandType_t& commandType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Message::commandType2String"));

  std::string result = ACE_TEXT("INVALID/UNKNOWN");

  switch (commandType_in)
  {
    case RPG_Net_Protocol_IRCMessage::PASS:
      result = ACE_TEXT("PASS"); break;
    case RPG_Net_Protocol_IRCMessage::NICK:
      result = ACE_TEXT("NICK"); break;
    case RPG_Net_Protocol_IRCMessage::USER:
      result = ACE_TEXT("USER"); break;
    case RPG_Net_Protocol_IRCMessage::SERVER:
      result = ACE_TEXT("SERVER"); break;
    case RPG_Net_Protocol_IRCMessage::OPER:
      result = ACE_TEXT("OPER"); break;
    case RPG_Net_Protocol_IRCMessage::QUIT:
      result = ACE_TEXT("QUIT"); break;
    case RPG_Net_Protocol_IRCMessage::SQUIT:
      result = ACE_TEXT("SQUIT"); break;
    case RPG_Net_Protocol_IRCMessage::JOIN:
      result = ACE_TEXT("JOIN"); break;
    case RPG_Net_Protocol_IRCMessage::PART:
      result = ACE_TEXT("PART"); break;
    case RPG_Net_Protocol_IRCMessage::MODE:
      result = ACE_TEXT("MODE"); break;
    case RPG_Net_Protocol_IRCMessage::TOPIC:
      result = ACE_TEXT("TOPIC"); break;
    case RPG_Net_Protocol_IRCMessage::NAMES:
      result = ACE_TEXT("NAMES"); break;
    case RPG_Net_Protocol_IRCMessage::LIST:
      result = ACE_TEXT("LIST"); break;
    case RPG_Net_Protocol_IRCMessage::INVITE:
      result = ACE_TEXT("INVITE"); break;
    case RPG_Net_Protocol_IRCMessage::KICK:
      result = ACE_TEXT("KICK"); break;
    case RPG_Net_Protocol_IRCMessage::SVERSION:
      result = ACE_TEXT("VERSION"); break;
    case RPG_Net_Protocol_IRCMessage::STATS:
      result = ACE_TEXT("STATS"); break;
    case RPG_Net_Protocol_IRCMessage::LINKS:
      result = ACE_TEXT("LINKS"); break;
    case RPG_Net_Protocol_IRCMessage::TIME:
      result = ACE_TEXT("TIME"); break;
    case RPG_Net_Protocol_IRCMessage::CONNECT:
      result = ACE_TEXT("CONNECT"); break;
    case RPG_Net_Protocol_IRCMessage::TRACE:
      result = ACE_TEXT("TRACE"); break;
    case RPG_Net_Protocol_IRCMessage::ADMIN:
      result = ACE_TEXT("ADMIN"); break;
    case RPG_Net_Protocol_IRCMessage::INFO:
      result = ACE_TEXT("INFO"); break;
    case RPG_Net_Protocol_IRCMessage::PRIVMSG:
      result = ACE_TEXT("PRIVMSG"); break;
    case RPG_Net_Protocol_IRCMessage::NOTICE:
      result = ACE_TEXT("NOTICE"); break;
    case RPG_Net_Protocol_IRCMessage::WHO:
      result = ACE_TEXT("WHO"); break;
    case RPG_Net_Protocol_IRCMessage::WHOIS:
      result = ACE_TEXT("WHOIS"); break;
    case RPG_Net_Protocol_IRCMessage::WHOWAS:
      result = ACE_TEXT("WHOWAS"); break;
    case RPG_Net_Protocol_IRCMessage::KILL:
      result = ACE_TEXT("KILL"); break;
    case RPG_Net_Protocol_IRCMessage::PING:
      result = ACE_TEXT("PING"); break;
    case RPG_Net_Protocol_IRCMessage::PONG:
      result = ACE_TEXT("PONG"); break;
#if defined ACE_WIN32 || defined ACE_WIN64
#pragma message("applying quirk code for this compiler...")
    case RPG_Net_Protocol_IRCMessage::__QUIRK__ERROR:
#else
    case RPG_Net_Protocol_IRCMessage::ERROR:
#endif
      result = ACE_TEXT("ERROR"); break;
    case RPG_Net_Protocol_IRCMessage::AWAY:
      result = ACE_TEXT("AWAY"); break;
    case RPG_Net_Protocol_IRCMessage::REHASH:
      result = ACE_TEXT("REHASH"); break;
    case RPG_Net_Protocol_IRCMessage::RESTART:
      result = ACE_TEXT("RESTART"); break;
    case RPG_Net_Protocol_IRCMessage::SUMMON:
      result = ACE_TEXT("SUMMON"); break;
    case RPG_Net_Protocol_IRCMessage::USERS:
      result = ACE_TEXT("USERS"); break;
    case RPG_Net_Protocol_IRCMessage::WALLOPS:
      result = ACE_TEXT("WALLOPS"); break;
    case RPG_Net_Protocol_IRCMessage::USERHOST:
      result = ACE_TEXT("USERHOST"); break;
    case RPG_Net_Protocol_IRCMessage::ISON:
      result = ACE_TEXT("ISON"); break;
    default:
    {
      // try numeric conversion
      result = RPG_Net_Protocol_Tools::IRCCode2String(static_cast<RPG_Net_Protocol_IRCNumeric_t>(commandType_in));

      break;
    }
  } // end SWITCH

  return result;
}
