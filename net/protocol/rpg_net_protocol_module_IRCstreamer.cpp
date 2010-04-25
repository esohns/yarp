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

#include "rpg_net_protocol_module_IRCstreamer.h"

#include "rpg_net_protocol_message.h"

RPG_Net_Protocol_Module_IRCStreamer::RPG_Net_Protocol_Module_IRCStreamer()
//  : inherited()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCStreamer::RPG_Net_Protocol_Module_IRCStreamer"));

}

RPG_Net_Protocol_Module_IRCStreamer::~RPG_Net_Protocol_Module_IRCStreamer()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCStreamer::~RPG_Net_Protocol_Module_IRCStreamer"));

}

void
RPG_Net_Protocol_Module_IRCStreamer::handleDataMessage(Stream_MessageBase*& message_inout,
                                                       bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Module_IRCStreamer::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  // *NOTE*: as this is an "upstream" module, the "wording" is wrong
  // --> the logic remains the same, though
  passMessageDownstream_out = true;

  // sanity check(s)
  ACE_ASSERT(message_inout);

  // retrieve IRC message structure
  RPG_Net_Protocol_Message* message = ACE_dynamic_cast(RPG_Net_Protocol_Message*,
                                                       message_inout);
  // sanity check(s)
  ACE_ASSERT(message);

  // serialize our structured data
  // --> create the appropriate bytestream corresponding to its elements

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

  // prefix
  if (!message->getData()->prefix.origin.empty())
  {
    // prefix the prefix
    *message->wr_ptr() = ':';
    message->wr_ptr(1);

    if (message->copy(message->getData()->prefix.origin.c_str(),
                      message->getData()->prefix.origin.size()) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                 message->getData()->prefix.origin.c_str(),
                 message->getData()->prefix.origin.size()));

      // clean up
      passMessageDownstream_out = false;
      message_inout->release();
      message_inout = NULL;

      return;
    } // end IF

    // append user
    if (!message->getData()->prefix.user.empty())
    {
      // user prefix
      *message->wr_ptr() = '!';
      message->wr_ptr(1);

      if (message->copy(message->getData()->prefix.user.c_str(),
                        message->getData()->prefix.user.size()) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                   message->getData()->prefix.user.c_str(),
                   message->getData()->prefix.user.size()));

      // clean up
        passMessageDownstream_out = false;
        message_inout->release();
        message_inout = NULL;

        return;
      } // end IF
    } // end IF

    // append host
    if (!message->getData()->prefix.user.empty())
    {
      // user prefix
      *message->wr_ptr() = '@';
      message->wr_ptr(1);

      if (message->copy(message->getData()->prefix.host.c_str(),
                        message->getData()->prefix.host.size()) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                   message->getData()->prefix.host.c_str(),
                   message->getData()->prefix.host.size()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release();
        message_inout = NULL;

        return;
      } // end IF
    } // end IF
  } // end IF

  // command
  switch (message->getData()->command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      // convert number into the equivalent 3-letter string
      if (::snprintf(message->wr_ptr(),            // target
                     4,                            // max length
                     ACE_TEXT_ALWAYS_CHAR("%.3u"), // format string
                     message->getData()->command.numeric) != 3)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ::snprintf: \"%m\", aborting\n")));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release();
        message_inout = NULL;

        return;
      } // end IF

      //... and adjust the write pointer accordingly
      message->wr_ptr(3);

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      if (message->copy(message->getData()->command.string->c_str(),
                        message->getData()->command.string->size()) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                   message->getData()->command.string->c_str(),
                   message->getData()->command.string->size()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release();
        message_inout = NULL;

        return;
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%u]: invalid command type (was: %u), aborting\n"),
                 message_inout->getID(),
                 message->getData()->command.discriminator));

      // clean up
      passMessageDownstream_out = false;
      message_inout->release();
      message_inout = NULL;

      return;
    }
  } // end SWITCH

  // parameter(s)
  if (!message->getData()->params.empty())
  {
    int i = message->getData()->params.size();
    for (std::list<std::string>::const_iterator iterator = message->getData()->params.begin();
         iterator != message->getData()->params.end();
         iterator++, i--)
    {
      // add a <SPACE>
      *message->wr_ptr() = ' ';
      message->wr_ptr(1);

      // special handling for last parameter
      if (i == 1)
      {
        // prefix the trailing parameter
        *message->wr_ptr() = ':';
        message->wr_ptr(1);
      } // end IF

      if (message->copy((*iterator).c_str(),
                        (*iterator).size()) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                   (*iterator).c_str(),
                   (*iterator).size()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release();
        message_inout = NULL;

        return;
      } // end IF
    } // end FOR
  } // end IF

  // append a <CRLF>
  *message->wr_ptr() = '\r';
  *(message->wr_ptr() + 1) = '\n';
  message->wr_ptr(2);
}
