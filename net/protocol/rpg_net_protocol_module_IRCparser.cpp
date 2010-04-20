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
   myBufferIsResized(false),
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
    myBufferIsResized = false;
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

  // *NOTE*: in order to accomodate flex, the buffer needs two trailing
  // '\0' characters...
  // --> make sure it has this capacity
  if (message_inout->space() < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)
  {
    // *sigh*: (try to) resize it then...
    if (message_inout->size(message_inout->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::size(%u), aborting\n"),
                 (message_inout->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));

      // what else can we do ?
      return;
    } // end IF
    myBufferIsResized = true;

    // *WARNING*: beyond this point, make sure we resize the buffer back
    // to its original length...
  } // end IF
//   for (int i = 0;
//        i < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
//        i++)
//     *(myCurrentBuffer->wr_ptr() + i) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr()) = '\0';
  *(message_inout->wr_ptr() + 1) = '\0';

  // OK: parse this message
  RPG_Net_Protocol_Message* message = ACE_dynamic_cast(RPG_Net_Protocol_Message*,
                                                       message_inout);
  ACE_ASSERT(message);
  myParserDriver.init(ACE_const_cast(RPG_Net_Protocol_IRCMessage&, *message->getData()));
  if (!myParserDriver.parse(message_inout))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Protocol_IRCParserDriver::parse(ID: %u): \"%m\", aborting\n"),
               message_inout->getID()));

    // what else can we do ?
    return;
  } // end IF

  // clean up
  if (myBufferIsResized)
  {
    if (message_inout->size(message_inout->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
                 (message_inout->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
    myBufferIsResized = false;
  } // end IF
}
