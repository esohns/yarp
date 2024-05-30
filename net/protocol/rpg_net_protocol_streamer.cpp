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

#include "rpg_net_protocol_streamer.h"

#include <sstream>
#include <string>

#include "ace/ace_wchar.h"
#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

const char rpg_net_protocol_streamer_module_name_string[] =
  ACE_TEXT_ALWAYS_CHAR ("ProtocolStreamer");

#if defined (ACE_WIN32) || defined (ACE_WIN64)
RPG_Net_Protocol_Streamer::RPG_Net_Protocol_Streamer (ISTREAM_T* stream_in)
#else
RPG_Net_Protocol_Streamer::RPG_Net_Protocol_Streamer (typename inherited::ISTREAM_T* stream_in)
#endif // ACE_WIN32 || ACE_WIN64
 : inherited (stream_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Streamer::RPG_Net_Protocol_Streamer"));

}

void
RPG_Net_Protocol_Streamer::handleDataMessage (RPG_Net_Protocol_Message*& message_inout,
                                              bool& passMessageDownstream_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Streamer::handleDataMessage"));

  // don't care (implies yes per default, if part of a stream)
  // *NOTE*: as this is an "upstream" module, the "wording" is wrong (the logic
  //         still aplies, though)
  passMessageDownstream_out = true;

  const typename RPG_Net_Protocol_Message::DATA_T& data_r =
    message_inout->getR ();

  int result = -1;
  std::string text_string;
  std::ostringstream converter;
  ACE_UINT32 length_i = sizeof (ACE_UINT32);

  text_string = ' ';

  converter << data_r.command.type;
  text_string += converter.str ();
  text_string += ' ';

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (data_r.command.xml.find (ACE_TEXT_ALWAYS_CHAR ("\r\n"), 0) == std::string::npos);
#elif defined (ACE_LINUX)
  ACE_ASSERT (data_r.command.xml.find (ACE_TEXT_ALWAYS_CHAR ("\n"), 0) == std::string::npos);
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
  text_string += data_r.command.xml;
  text_string += ACE_TEXT_ALWAYS_CHAR ("\r\n");

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.command;
  text_string += converter.str ();
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.position.first;
  converter << ',';
  converter << data_r.command.position.second;
  text_string += converter.str ();
  text_string += ' ';

  for (RPG_Map_PathConstIterator_t iterator = data_r.command.path.begin ();
       iterator != data_r.command.path.end ();
       ++iterator)
  {
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << (*iterator).first.first;
    converter << ',';
    converter << (*iterator).first.second;
    converter << ',';
    converter << (*iterator).second;
    text_string += converter.str ();
    text_string += ' ';
  } // end FOR
  text_string += ';';
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.entity_id;
  text_string += converter.str ();
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.previous.first;
  converter << ',';
  converter << data_r.command.previous.second;
  text_string += converter.str ();
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.window;
  text_string += converter.str ();
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.cursor;
  text_string += converter.str ();
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.sound;
  text_string += converter.str ();
  text_string += ' ';

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (data_r.command.message.find (ACE_TEXT_ALWAYS_CHAR ("\r\n"), 0) == std::string::npos);
#elif defined (ACE_LINUX)
  ACE_ASSERT (data_r.command.message.find (ACE_TEXT_ALWAYS_CHAR ("\n"), 0) == std::string::npos);
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX
  text_string += data_r.command.message;
  text_string += ACE_TEXT_ALWAYS_CHAR ("\r\n");

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << data_r.command.source.first;
  converter << ',';
  converter << data_r.command.source.second;
  text_string += converter.str ();
  text_string += ' ';

  for (RPG_Map_PositionsConstIterator_t iterator = data_r.command.positions.begin ();
       iterator != data_r.command.positions.end ();
       ++iterator)
  {
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << (*iterator).first;
    converter << ',';
    converter << (*iterator).second;
    text_string += converter.str ();
    text_string += ' ';
  } // end FOR
  text_string += ';';
  text_string += ' ';

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (data_r.command.radius);
  text_string += converter.str ();
  text_string += ' ';

  length_i += static_cast<ACE_UINT32> (text_string.size ());
  if (ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN)
    length_i = ACE_SWAP_LONG (length_i);
  result = message_inout->copy (reinterpret_cast<char*> (&length_i),
                                sizeof (ACE_UINT32));
  if (result == -1)
    goto error;
  result = message_inout->copy (const_cast<char*> (text_string.c_str ()),
                                text_string.size ());
  if (result == -1)
    goto error;

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("[%u]: streamed [%u byte(s)]...\n"),
  //            message_inout->getID (),
  //            message_inout->length ()));

  return;

error:
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("%s: failed to ACE_Message_Block::copy(): \"%m\", returning\n"),
              inherited::mod_->name ()));
}
