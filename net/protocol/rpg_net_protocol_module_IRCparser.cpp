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

#include "rpg_net_protocol_sessionmessage.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_tools.h"

#include <rpg_stream_iallocator.h>

#include <rpg_common_macros.h>

RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser()
 : //inherited(),
   myAllocator(NULL),
   myDriver(RPG_NET_PROTOCOL_DEF_TRACE_SCANNING,           // trace scanning ?
            RPG_NET_PROTOCOL_DEF_TRACE_PARSING),           // trace parsing ?
   myDebugScanner(RPG_NET_PROTOCOL_DEF_TRACE_SCANNING),    // trace scanning ?
   myDebugParser(RPG_NET_PROTOCOL_DEF_TRACE_PARSING),      // trace parsing ?
   myCrunchMessages(RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES), // "crunch" messages ?
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser"));

}

RPG_Net_Protocol_Module_IRCParser::~RPG_Net_Protocol_Module_IRCParser()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::~RPG_Net_Protocol_Module_IRCParser"));

}

const bool
RPG_Net_Protocol_Module_IRCParser::init(RPG_Stream_IAllocator* allocator_in,
                                        const bool& crunchMessages_in,
                                        const bool& debugScanner_in,
                                        const bool& debugParser_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::init"));

  // sanity check(s)
  ACE_ASSERT(allocator_in);
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("re-initializing...\n")));

    myAllocator = NULL;
    myDebugScanner = debugScanner_in;
    myDebugParser = debugParser_in;
    myCrunchMessages = RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES;
    myIsInitialized = false;
  } // end IF

  myAllocator = allocator_in;
  myDebugScanner = debugScanner_in;
  myDebugParser = debugParser_in;
  myCrunchMessages = crunchMessages_in;

  // OK: all's well...
  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Protocol_Module_IRCParser::handleDataMessage(RPG_Net_Protocol_Message*& message_inout,
                                                     bool& passMessageDownstream_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::handleDataMessage"));

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

  // sanity check(s)
//   ACE_ASSERT(message_inout->getData() == NULL);

  // "crunch" messages for easier parsing ?
  RPG_Net_Protocol_Message* message = message_inout;
  if (myCrunchMessages)
  {
    // step1: get a new message buffer
    message = allocateMessage(RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE);
    if (message == NULL)
    {
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to allocate message(%u), aborting\n"),
                  RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE));

      return;
    } // end IF

    // step2: copy available data
    for (ACE_Message_Block* source = message_inout;
          source != NULL;
          source = source->cont())
    {
      ACE_ASSERT(source->length() <= message->space());
      if (message->copy(source->rd_ptr(),
                        source->length()))
      {
        ACE_DEBUG((LM_ERROR,
                    ACE_TEXT("failed to ACE_Message_Block::copy(): \"%m\", aborting\n")));

        // clean up
        message->release();

        // what can we do ?
        return;
      } // end IF
    } // end FOR

    // step3: append the "\0\0"-sequence, as required by flex
    ACE_ASSERT(message->space() >= RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE);
    ACE_ASSERT(RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE == 2);
    *(message->wr_ptr()) = YY_END_OF_BUFFER_CHAR;
    *(message->wr_ptr() + 1) = YY_END_OF_BUFFER_CHAR;
    // *NOTE*: DO NOT adjust the write pointer --> length() should stay as it was !
  } // end IF

  // allocate the target data container and attach it to our current message
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
  // *NOTE*: at this time, the parser does not bisect (','-separated) list-items
  // (i.e. items which are themselves lists) from the (' '-separated) list of
  // parameters. This means that any list-items are extracted as a "whole" - a
  // list-item will just be a single (long) string...
  // *TODO*: check whether any messages actually use this feature on the client side
  // and either:
  // - make the parser more intelligent
  // - bisect any list items in a post-processing step...
  message_inout->init(container);

  // *NOTE*: message has assumed control over "container"...

  // OK: parse this message

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("parsing message(ID: %u, %u byte(s))\n\"%s\"\n"),
//              message_inout->getID(),
//              message_inout->length(),
//              std::string(message_inout->rd_ptr(), message_inout->length()).c_str()));

  myDriver.init(ACE_const_cast(RPG_Net_Protocol_IRCMessage&, *message_inout->getData()),
                myDebugScanner,
                myDebugParser);
  if (!myDriver.parse(message,           // data block
                      myCrunchMessages)) // has data been crunched ?
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Protocol_IRCParserDriver::parse(ID: %u), aborting\n"),
               message_inout->getID()));

    // clean up
    if (message != message_inout)
      message->release();
    passMessageDownstream_out = false;
    message_inout->release();

    // what else can we do ?
    return;
  } // end IF

  // clean up
  if (message != message_inout)
    message->release();
}

RPG_Net_Protocol_Message*
RPG_Net_Protocol_Module_IRCParser::allocateMessage(const unsigned long& requestedSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCParser::allocateMessage"));

  // init return value(s)
  RPG_Net_Protocol_Message* message_out = NULL;

  try
  {
    message_out = ACE_static_cast(RPG_Net_Protocol_Message*,
                                  myAllocator->malloc(requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  } // end IF

  return message_out;
}
