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

#include "rpg_net_protocol_module_IRCparser.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_message.h"

#include <stream_iallocator.h>

RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser()
 : //inherited(),
   myAllocator(NULL),
   myParserDriver(false,  // trace scanning ?
                  false), // trace parsing ?
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser"));

}

RPG_Net_Protocol_Module_IRCParser::~RPG_Net_Protocol_Module_IRCParser()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::~RPG_Net_Protocol_Module_IRCParser"));

}

const bool
RPG_Net_Protocol_Module_IRCParser::init(Stream_IAllocator* allocator_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    myAllocator = NULL;
    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCParser::handleDataMessage(Stream_MessageBase*& message_inout,
                                                     bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG(passMessageDownstream_out);

// according to RFC1459:
//  <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
//  <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
//  <command>  ::= <letter> { <letter> } | <number> <number> <number>
//  <SPACE>    ::= ' ' { ' ' }
//  <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
//  <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
//                 or NUL or CR or LF, the first of which may not be ':'>
//  <trailing> ::= <Any, possibly *empty*, sequence of octets not including
//                   NUL or CR or LF>

  // allocate the target data container and attach it to our current message
  RPG_Net_Protocol_Message* message = ACE_dynamic_cast(RPG_Net_Protocol_Message*,
                                                       message_inout);
  ACE_ASSERT(message);
  RPG_Net_Protocol_IRCMessage* container = NULL;
  ACE_NEW_NORETURN(container,
                   RPG_Net_Protocol_IRCMessage());
  if (!container)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate RPG_Net_Protocol_IRCMessage: \"%m\", aborting\n")));

    // what else can we do ?
    return;
  } // end IF
  message->init(container);

  // *NOTE*: message has assumed control over "container"...

  // OK: parse this message

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("parsing message(ID: %u, %u byte(s))\n\"%s\"\n"),
//              message->getID(),
//              message->length(),
//              message->rd_ptr()));

  myParserDriver.init(ACE_const_cast(RPG_Net_Protocol_IRCMessage&, *message->getData()));
  if (!myParserDriver.parse(message))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Protocol_IRCParserDriver::parse(ID: %u), aborting\n"),
               message->getID()));

    // what else can we do ?
    return;
  } // end IF

  // debug info
  try
  {
    message->getData()->dump_state();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Common_IDumpState::dump_state(), continuing\n")));
  }
}
