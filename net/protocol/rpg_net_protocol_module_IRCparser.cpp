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

#include "rpg_net_module_IRCparser.h"

#include <rpg_net_message.h>

#include <stream_iallocator.h>

RPG_Net_Protocol_Module_IRCParser::RPG_Net_Protocol_Module_IRCParser()
 : //inherited(),
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
    myCurrentBufferIsResized = true;

    // *WARNING*: beyond this point, make sure we resize the buffer back
    // to its original length...
  } // end IF
//   for (int i = 0;
//        i < RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
//        i++)
//     *(myCurrentBuffer->wr_ptr() + i) = YY_END_OF_BUFFER_CHAR;
  *(data_in->wr_ptr()) = '\0';
  *(data_in->wr_ptr() + 1) = '\0';

  // OK: parse this message
  if (!myParserDriver.parse(message_inout))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Protocol_IRCParserDriver::parse(ID: %u): \"%m\", aborting\n"),
               message_inout->getID()));

    // what else can we do ?
    return;
  } // end IF

  // clean up
  if (myCurrentBufferIsResized)
  {
    if (message_inout->size(data_in->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::size(%u), continuing\n"),
                 (message_inout->size() - RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE)));
    myCurrentBufferIsResized = false;
  } // end IF
}
