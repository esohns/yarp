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

#include "rpg_net_protocol_IRCparser_driver.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_IRCparser.h"
#include "rpg_net_protocol_IRCscanner.h"

#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>

// #include <iostream>
#include <sstream>

RPG_Net_Protocol_IRCParserDriver::RPG_Net_Protocol_IRCParserDriver(const bool& traceScanning_in,
                                                                   const bool& traceParsing_in)
 : myTraceScanning(traceScanning_in),
   myScannerContext(NULL),
   myCurrentNumMessages(0),
   myCurrentFragment(NULL),
   myFragmentIsResized(false),
   myCurrentBufferState(NULL),
   myTraceParsing(traceParsing_in),
   myParser(*this,             // driver
            myScannerContext), // scanner context
   myCurrentMessage(NULL),
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::RPG_Net_Protocol_IRCParserDriver"));

  // init scanner context
  if (IRCScannerlex_init_extra(&myCurrentNumMessages, // extra data
                               &myScannerContext))    // scanner context handle
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to IRCScannerlex_init_extra(): \"%m\", continuing\n")));

  // init parser
  myParser.set_debug_level(myTraceParsing);
}

RPG_Net_Protocol_IRCParserDriver::~RPG_Net_Protocol_IRCParserDriver ()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::~RPG_Net_Protocol_IRCParserDriver"));

  // fini scanner context
  if (myScannerContext)
    IRCScannerlex_destroy(myScannerContext);
}

void
RPG_Net_Protocol_IRCParserDriver::init(RPG_Net_Protocol_IRCMessage& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::parse"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);

  // set parse target
  myCurrentMessage = &message_in;

  // OK
  myIsInitialized = true;
}

const bool
RPG_Net_Protocol_IRCParserDriver::parse(ACE_Message_Block* data_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::parse"));

  // sanity check(s)
  ACE_ASSERT(myIsInitialized);
  ACE_ASSERT(data_in);

  // start with the first fragment...
  myCurrentFragment = data_in;

  // *NOTE*: we parse ALL available message fragments
  // *TODO*: yyrestart(), yy_create_buffer/yy_switch_to_buffer, YY_INPUT...
  int result = -1;
  do
  { // init scan buffer
    if (!scan_begin())
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to parse IRC message fragment, aborting\n")));

      // clean up
      myCurrentFragment = NULL;

      break;
    } // end IF

    // parse our data
    result = myParser.parse();
    if (result)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to parse IRC message fragment, continuing\n")));

    // fini buffer/scanner
    scan_end();
  } while (myCurrentFragment);

  // reset state
  myIsInitialized = false;

  return (result == 0);
}

// const RPG_Net_Protocol_IRCMessage
// RPG_Net_Protocol_IRCParserDriver::getIRCMessage() const
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::getIRCMessage"));
//
//   return myCurrentMessage;
// }

void
RPG_Net_Protocol_IRCParserDriver::error(const yy::location& location_in,
                                        const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::error"));

  // debug info
  std::ostringstream converter;
  converter << location_in;
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse IRC message (location: %s): \"%s\"...\n"),
             converter.str().c_str(),
             message_in.c_str()));

//   std::clog << location_in << ": " << message_in << std::endl;
}

void
RPG_Net_Protocol_IRCParserDriver::error(const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::error"));

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse IRC message: \"%s\"...\n"),
             message_in.c_str()));

//   std::clog << message_in << std::endl;
}

const bool
RPG_Net_Protocol_IRCParserDriver::scan_begin()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::scan_begin"));

  // sanity check(s)
  ACE_ASSERT(myCurrentBufferState == NULL);
  ACE_ASSERT(myCurrentFragment);

  // *NOTE*: in order to accomodate flex, the buffer needs two trailing
  // '\0' characters...
  // --> make sure it has this capacity
  if (myCurrentFragment->space() < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)
  {
  // *sigh*: (try to) resize it then...
    if (myCurrentFragment->size(myCurrentFragment->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
    {
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to ACE_Message_Block::size(%u), aborting\n"),
                  (myCurrentFragment->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));

    // what else can we do ?
      return false;
    } // end IF
    myFragmentIsResized = true;

    // *WARNING*: beyond this point, make sure we resize the buffer back
    // to its original length...
  } // end IF
//   for (int i = 0;
//        i < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
//        i++)
//     *(myCurrentBuffer->wr_ptr() + i) = YY_END_OF_BUFFER_CHAR;
  *(myCurrentFragment->wr_ptr()) = '\0';
  *(myCurrentFragment->wr_ptr() + 1) = '\0';

//  yy_flex_debug = myTraceScanning;

  // create/init a new buffer state
  myCurrentBufferState = IRCScanner_scan_buffer(myCurrentFragment->rd_ptr(),
                                                (myCurrentFragment->length() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE),
                                                myScannerContext);
  if (myCurrentBufferState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::IRCScanner_scan_buffer(%@, %d), aborting\n"),
               myCurrentFragment->rd_ptr(),
               (myCurrentFragment->length() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));

    // clean up
    if (myFragmentIsResized)
    {
      if (myCurrentFragment->size(myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
                   (myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
      myFragmentIsResized = false;
    } // end IF

    // what else can we do ?
    return false;
  } // end IF

  return true;
}

void
RPG_Net_Protocol_IRCParserDriver::scan_end()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::scan_end"));

  // sanity check(s)
  ACE_ASSERT(myCurrentBufferState);

  // clean buffer
  if (myFragmentIsResized)
  {
    if (myCurrentFragment->size(myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
                 (myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
    myFragmentIsResized = false;
  } // end IF

  // clean state
  IRCScanner_delete_buffer(myCurrentBufferState,
                           myScannerContext);
  myCurrentBufferState = NULL;

  // switch fragment (perhaps we have MORE data in any continuation(s))
  myCurrentFragment = myCurrentFragment->cont();
}

// void
// RPG_Net_Protocol_IRCParserDriver::reset()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::reset"));
//
//   if (myCurrentMessage.prefix.origin)
//     delete myCurrentMessage.prefix.origin;
//   myCurrentMessage.prefix.origin = NULL;
//   if (myCurrentMessage.prefix.user)
//     delete myCurrentMessage.prefix.user;
//   myCurrentMessage.prefix.user = NULL;
//   if (myCurrentMessage.prefix.host)
//     delete myCurrentMessage.prefix.host;
//   myCurrentMessage.prefix.host = NULL;
//   switch (myCurrentMessage.command.discriminator)
//   {
//     case RPG_Net_Protocol_IRCMessage::Command::STRING:
//     {
//       if (myCurrentMessage.command.string)
//         delete myCurrentMessage.command.string;
//       myCurrentMessage.command.string = NULL;
//       myCurrentMessage.command.discriminator = RPG_Net_Protocol_IRCMessage::Command::INVALID;
//
//       break;
//     }
//     case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
//     {
//       myCurrentMessage.command.numeric = RPG_Net_Protocol_IRC_Codes::RPG_NET_PROTOCOL_IRC_CODES_INVALID;
//       myCurrentMessage.command.discriminator = RPG_Net_Protocol_IRCMessage::Command::INVALID;
//
//       break;
//     }
//     default:
//     {
//       myCurrentMessage.command.string = NULL;
//       myCurrentMessage.command.numeric = RPG_Net_Protocol_IRC_Codes::RPG_NET_PROTOCOL_IRC_CODES_INVALID;
//       myCurrentMessage.command.discriminator = RPG_Net_Protocol_IRCMessage::Command::INVALID;
//
//       break;
//     }
//   } // end SWITCH
//   if (myCurrentMessage.params)
//   {
//     myCurrentMessage.params->clear();
//     delete myCurrentMessage.params;
//     myCurrentMessage.params = NULL;
//   } // end IF
// //   ACE_OS::memset(&myCurrentMessage,
// //                  0,
// //                  sizeof(RPG_Net_Protocol_IRCMessage));
// }
