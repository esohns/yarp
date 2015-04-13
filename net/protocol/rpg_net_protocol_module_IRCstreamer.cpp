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

#include "rpg_net_protocol_module_IRCstreamer.h"

#include "rpg_common_macros.h"

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_tools.h"

RPG_Net_Protocol_Module_IRCStreamer::RPG_Net_Protocol_Module_IRCStreamer ()
//  : inherited()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Module_IRCStreamer::RPG_Net_Protocol_Module_IRCStreamer"));

}

RPG_Net_Protocol_Module_IRCStreamer::~RPG_Net_Protocol_Module_IRCStreamer ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Module_IRCStreamer::~RPG_Net_Protocol_Module_IRCStreamer"));

}

void
RPG_Net_Protocol_Module_IRCStreamer::handleDataMessage (RPG_Net_Protocol_Message*& message_inout,
                                                        bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Module_IRCStreamer::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  // *NOTE*: as this is an "upstream" module, the "wording" is wrong
  // --> the logic remains the same, though
  passMessageDownstream_out = true;

  // sanity check(s)
  ACE_ASSERT (message_inout->length () == 0);
  ACE_ASSERT (message_inout->space () >= RPG_NET_PROTOCOL_IRC_FRAME_MAXSIZE);

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
  if (!message_inout->getData ()->prefix.origin.empty ())
  {
    // prefix the prefix
    *message_inout->wr_ptr () = ':';
    message_inout->wr_ptr (1);

    if (message_inout->copy (message_inout->getData ()->prefix.origin.c_str (),
                             message_inout->getData ()->prefix.origin.size ()) == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                  message_inout->getData ()->prefix.origin.c_str (),
                  message_inout->getData ()->prefix.origin.size ()));

      // clean up
      passMessageDownstream_out = false;
      message_inout->release ();
      message_inout = NULL;

      return;
    } // end IF

    // append user
    if (!message_inout->getData ()->prefix.user.empty ())
    {
      // sanity check
      if (message_inout->space () < 1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                    message_inout->getID (),
                    ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                    message_inout->length ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release();
        message_inout = NULL;

        return;
      } // end IF
      // user prefix
      *message_inout->wr_ptr () = '!';
      message_inout->wr_ptr (1);

      if (message_inout->copy (message_inout->getData ()->prefix.user.c_str (),
                               message_inout->getData ()->prefix.user.size ()) == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                    ACE_TEXT (message_inout->getData ()->prefix.user.c_str ()),
                    message_inout->getData ()->prefix.user.size ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF
    } // end IF

    // append host
    if (!message_inout->getData ()->prefix.host.empty ())
    {
      // sanity check
      if (message_inout->space () < 1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                    message_inout->getID (),
                    ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                    message_inout->length ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF
      // host prefix
      *message_inout->wr_ptr () = '@';
      message_inout->wr_ptr (1);

      if (message_inout->copy (message_inout->getData ()->prefix.host.c_str (),
                               message_inout->getData ()->prefix.host.size ()) == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                    ACE_TEXT (message_inout->getData ()->prefix.host.c_str ()),
                    message_inout->getData ()->prefix.host.size ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF
    } // end IF

    // sanity check
    if (message_inout->space () < 1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                  message_inout->getID (),
                  ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                  message_inout->length ()));

        // clean up
      passMessageDownstream_out = false;
      message_inout->release ();
      message_inout = NULL;

      return;
    } // end IF
    // add a <SPACE>
    *message_inout->wr_ptr () = ' ';
    message_inout->wr_ptr (1);
  } // end IF

  // command
  switch (message_inout->getData ()->command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      // sanity check
      if (message_inout->space () < 4)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                    message_inout->getID (),
                    ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                    message_inout->length ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF
      // convert number into the equivalent 3-letter string
      if (ACE_OS::snprintf (message_inout->wr_ptr (),      // target
                            4,                            // max length
                            ACE_TEXT_ALWAYS_CHAR ("%.3u"), // format string
                            message_inout->getData ()->command.numeric) != 3)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ::snprintf: \"%m\", aborting\n")));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF

      //... and adjust the write pointer accordingly
      message_inout->wr_ptr (3);

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      if (message_inout->copy (message_inout->getData ()->command.string->c_str (),
                               message_inout->getData ()->command.string->size ()) == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                    ACE_TEXT (message_inout->getData ()->command.string->c_str ()),
                    message_inout->getData ()->command.string->size ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%u]: invalid command type (was: %u), aborting\n"),
                  message_inout->getID (),
                  message_inout->getData ()->command.discriminator));

      // clean up
      passMessageDownstream_out = false;
      message_inout->release ();
      message_inout = NULL;

      return;
    }
  } // end SWITCH

  // parameter(s)
  if (!message_inout->getData ()->params.empty ())
  {
    // sanity check
    if (message_inout->space () < 1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                  message_inout->getID (),
                  ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                  message_inout->length ()));

      // clean up
      passMessageDownstream_out = false;
      message_inout->release ();
      message_inout = NULL;

      return;
    } // end IF
    // add a separator
    *message_inout->wr_ptr () = ' ';
    message_inout->wr_ptr (1);
  } // end IF
  unsigned long forward_i = 0;
  unsigned long reverse_i = message_inout->getData ()->params.size ();
  char param_separator = ' ';
  list_items_ranges_iterator_t range_iterator = message_inout->getData ()->list_param_ranges.begin ();
  for (RPG_Net_Protocol_ParametersIterator_t iterator = message_inout->getData ()->params.begin ();
       iterator != message_inout->getData ()->params.end ();
       iterator++, forward_i++, reverse_i--)
  {
    // (re-)set to default
    param_separator = ' ';

    // advance range iterator ?
    if ((range_iterator != message_inout->getData ()->list_param_ranges.end ()) &&
        (forward_i > (*range_iterator).second))
      range_iterator++;

    // param part of a list ?
    if ((range_iterator != message_inout->getData ()->list_param_ranges.end ()) &&
        (forward_i >= (*range_iterator).first) &&
        (forward_i <= (*range_iterator).second))
      param_separator = ',';

    // special handling for last parameter (may contain <SPACE> characters)
    // --> if necessary, prefix the trailing parameter
    // *NOTE*: the "final" parameter may be a list-item, but in this case
    // the whitespace is excluded from the allowed set of characters...
    if ((reverse_i == 1) &&
// *PORTABILITY*
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
        ((*iterator).find(' ') != std::string::npos))
#else
        ((*iterator).find(' ') != -1))
#endif
    {
      // sanity check(s)
      ACE_ASSERT (param_separator == ' '); // cannot be (part of) a list-item
      if (message_inout->space () < 1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                    message_inout->getID (),
                    ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                    message_inout->length ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF
      *message_inout->wr_ptr () = ':';
      message_inout->wr_ptr (1);
    } // end IF

    // append param string
    if (message_inout->copy ((*iterator).c_str (),
                             (*iterator).size ()) == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Message_Block::copy(\"%s\", %u): \"%m\", aborting\n"),
                  ACE_TEXT ((*iterator).c_str ()),
                  (*iterator).size ()));

      // clean up
      passMessageDownstream_out = false;
      message_inout->release ();
      message_inout = NULL;

      return;
    } // end IF

    // append separator <SPACE> OR <COMMA>
    if (reverse_i > 1)
    {
      // sanity check
      if (message_inout->space () < 1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                    message_inout->getID (),
                    ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                    message_inout->length ()));

        // clean up
        passMessageDownstream_out = false;
        message_inout->release ();
        message_inout = NULL;

        return;
      } // end IF
      *message_inout->wr_ptr () = param_separator;
      message_inout->wr_ptr (1);
    } // end IF
  } // end FOR

  // sanity check
  if (message_inout->space () < RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("[%u]: out of buffer space (\"%s\", %u), aborting\n"),
                message_inout->getID (),
                ACE_TEXT (std::string (message_inout->rd_ptr (), message_inout->length ()).c_str ()),
                message_inout->length ()));

    // clean up
    passMessageDownstream_out = false;
    message_inout->release ();
    message_inout = NULL;

    return;
  } // end IF
  // append a <CRLF>
  *message_inout->wr_ptr () = '\r';
  *(message_inout->wr_ptr () + 1) = '\n';
  message_inout->wr_ptr (RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE);

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("[%u]: streamed [%u byte(s)]...\n"),
//              message_inout->getID(),
//              message_inout->length()));
}
