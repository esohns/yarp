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
#include "rpg_net_protocol_IRCmessage.h"
#include "rpg_net_protocol_IRCparser.h"
#include "rpg_net_protocol_IRCscanner.h"
#include "rpg_net_protocol_message.h"

#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>

#include <sstream>

RPG_Net_Protocol_IRCParserDriver::RPG_Net_Protocol_IRCParserDriver(const bool& traceScanning_in,
                                                                   const bool& traceParsing_in)
 : myTraceScanning(traceScanning_in),
   myScannerContext(NULL),
   myCurrentNumMessages(0),
   myCurrentFragment(NULL),
   myFragmentIsResized(false),
   myCurrentBufferState(NULL),
   myParser(*this,                // driver
            myCurrentNumMessages, // counter
            myScannerContext),    // scanner context
   myCurrentMessage(NULL),
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::RPG_Net_Protocol_IRCParserDriver"));

  // init scanner context
  if (IRCScannerlex_init_extra(this,               // extra data
                               &myScannerContext)) // scanner context handle
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to IRCScannerlex_init_extra(): \"%m\", continuing\n")));

  // init parser
  myParser.set_debug_level(traceParsing_in); // binary (see bison manual)
}

RPG_Net_Protocol_IRCParserDriver::~RPG_Net_Protocol_IRCParserDriver ()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::~RPG_Net_Protocol_IRCParserDriver"));

  // fini scanner context
  if (myScannerContext)
    IRCScannerlex_destroy(myScannerContext);
}

void
RPG_Net_Protocol_IRCParserDriver::init(RPG_Net_Protocol_IRCMessage& message_in,
                                       const bool& debugParser_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::parse"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);

  // set parse target
  myCurrentMessage = &message_in;

  if (debugParser_in)
  {
    // remember this setting
    myTraceScanning = debugParser_in;
    // init parser
    myParser.set_debug_level(debugParser_in); // binary (see bison manual)
  } // end IF

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
//   do
//   { // init scan buffer
    if (!scan_begin())
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to parse IRC message fragment, aborting\n")));

      // clean up
      myCurrentFragment = NULL;

//       break;
      return false;
    } // end IF

    // parse our data
    result = myParser.parse();
    if (result)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to parse IRC message fragment, continuing\n")));

    // fini buffer/scanner
    scan_end();

    // debug info
    if (myParser.debug_level())
    {
      try
      {
        myCurrentMessage->dump_state();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Common_IDumpState::dump_state(), continuing\n")));
      }
    } // end IF
//   } while (myCurrentFragment);

  // reset state
  myIsInitialized = false;

  return (result == 0);
}

const bool
RPG_Net_Protocol_IRCParserDriver::switchBuffer()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::switchBuffer"));

  // sanity check
  if (myCurrentFragment->cont() == NULL)
    return false; // <-- nothing to do

  // switch to the next fragment
  myCurrentFragment = myCurrentFragment->cont();

  // clean state
  ACE_ASSERT(myCurrentBufferState);
  IRCScanner_delete_buffer(myCurrentBufferState,
                           myScannerContext);
  myCurrentBufferState = NULL;

  // init next buffer
  myCurrentBufferState = IRCScanner_scan_bytes(myCurrentFragment->rd_ptr(),
                                               myCurrentFragment->length(),
                                               myScannerContext);
  if (myCurrentBufferState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::IRCScanner_scan_bytes(%@, %d), aborting\n"),
               myCurrentFragment->rd_ptr(),
               myCurrentFragment->length()));

    // what else can we do ?
    return false;
  } // end IF

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("switched to next buffer...\n")));

  return true;
}

const bool
RPG_Net_Protocol_IRCParserDriver::moreData()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::moreData"));

  return (myCurrentFragment->cont() != NULL);
}

const bool
RPG_Net_Protocol_IRCParserDriver::getTraceScanning() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::getTraceScanning"));

  return myTraceScanning;
}

void
RPG_Net_Protocol_IRCParserDriver::error(const yy::location& location_in,
                                        const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::error"));

  // debug info
  std::ostringstream converter;
  converter << location_in;
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse \"%s\" (@%s): \"%s\"\n"),
             std::string(myCurrentFragment->rd_ptr(), myCurrentFragment->length()).c_str(),
             converter.str().c_str(),
             message_in.c_str()));

  // dump message
  RPG_Net_Protocol_Message* message = NULL;
  message = ACE_dynamic_cast(RPG_Net_Protocol_Message*,
                             myCurrentFragment);
  ACE_ASSERT(message);
  try
  {
    message->dump_state();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Common_IDumpState::dump_state(), continuing\n")));
  }

//   std::clog << location_in << ": " << message_in << std::endl;
}

void
RPG_Net_Protocol_IRCParserDriver::error(const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParserDriver::error"));

  // debug info
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("failed to parse \"%s\": \"%s\"...\n"),
             std::string(myCurrentFragment->rd_ptr(), myCurrentFragment->length()).c_str(),
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

  // *WARNING*: cannot use yy_scan_buffer(), as flex modifies the data... :-(
//   // *NOTE*: in order to accomodate flex, the buffer needs two trailing
//   // '\0' characters...
//   // --> make sure it has this capacity
//   if (myCurrentFragment->space() < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)
//   {
//   // *sigh*: (try to) resize it then...
//     if (myCurrentFragment->size(myCurrentFragment->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
//     {
//       ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("failed to ACE_Message_Block::size(%u), aborting\n"),
//                   (myCurrentFragment->size() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
//
//     // what else can we do ?
//       return false;
//     } // end IF
//     myFragmentIsResized = true;
//
//     // *WARNING*: beyond this point, make sure we resize the buffer back
//     // to its original length...
//   } // end IF
// //   for (int i = 0;
// //        i < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
// //        i++)
// //     *(myCurrentBuffer->wr_ptr() + i) = YY_END_OF_BUFFER_CHAR;
//   *(myCurrentFragment->wr_ptr()) = '\0';
//   *(myCurrentFragment->wr_ptr() + 1) = '\0';
//   // create/init a new buffer state
//   myCurrentBufferState = IRCScanner_scan_buffer(myCurrentFragment->rd_ptr(),
//                                                 (myCurrentFragment->length() + RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE),
//                                                 myScannerContext);
  myCurrentBufferState = IRCScanner_scan_bytes(myCurrentFragment->rd_ptr(),
                                               myCurrentFragment->length(),
                                               myScannerContext);
  if (myCurrentBufferState == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::IRCScanner_scan_bytes(%@, %d), aborting\n"),
               myCurrentFragment->rd_ptr(),
               myCurrentFragment->length()));

//     // clean up
//     if (myFragmentIsResized)
//     {
//       if (myCurrentFragment->size(myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
//                    (myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
//       myFragmentIsResized = false;
//     } // end IF

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

//   // clean buffer
//   if (myFragmentIsResized)
//   {
//     if (myCurrentFragment->size(myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
//                  (myCurrentFragment->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
//     myFragmentIsResized = false;
//   } // end IF

  // clean state
  IRCScanner_delete_buffer(myCurrentBufferState,
                           myScannerContext);
  myCurrentBufferState = NULL;

//   // switch to the next fragment (if any)
//   myCurrentFragment = myCurrentFragment->cont();
}
