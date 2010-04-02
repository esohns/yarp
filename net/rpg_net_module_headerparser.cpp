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

#include "rpg_net_module_headerparser.h"

// #include "rps_flb_common_packet_headers.h"
// #include "rps_flb_common_network_tools.h"
// #include "rps_flb_common_protocol_layer.h"
// #include "rps_flb_common_ethernetframeheader.h"
#include "rpg_net_common_tools.h"

#include <ace/OS.h>
#include <ace/Time_Value.h>
#include <ace/Message_Block.h>

#include <sstream>

RPG_Net_Module_HeaderParser::RPG_Net_Module_HeaderParser()
 : //inherited(),
   myIsInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::RPG_Net_Module_HeaderParser"));

}

RPG_Net_Module_HeaderParser::~RPG_Net_Module_HeaderParser()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::~RPG_Net_Module_HeaderParser"));

}

const bool
RPG_Net_Module_HeaderParser::init()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("re-initializing...\n")));

    // reset various maps...
    myUnknownPacketTypes2MessageID.clear();
    myIEEE802_3Frames.clear();
    myFailedMessages.clear();

    myIsInitialized = false;
  } // end IF

  myIsInitialized = true;

  return myIsInitialized;
}

void
RPG_Net_Module_HeaderParser::handleDataMessage(Stream_MessageBase*& message_inout,
                                               bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::handleDataMessage"));

  // init return value(s)
  passMessageDownstream_out = true;

  // sanity check(s)
  ACE_ASSERT(message_inout);

  // *TODO*
}

void
RPG_Net_Module_HeaderParser::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_HeaderParser::dump_state"));

  std::string line_string;
  std::ostringstream converter;

  ACE_DEBUG((LM_INFO,
             ACE_TEXT(" ***** MODULE: \"%s\" state *****\n"),
             ACE_TEXT_ALWAYS_CHAR(name())));

  if (myUnknownPacketTypes2MessageID.size())
  {
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("--> Unknown Packet Types <--\n")));

    // step1: dump unknown packet types (and corresponding message IDs)...
    // step1a: (pre-processing) collect all of the different packet types...
    PACKETTYPECONTAINER_TYPE unknown_packets;
    for (PACKETTYPE2MESSAGEIDMAP_CONSTITERATOR_TYPE iter = myUnknownPacketTypes2MessageID.begin();
         iter != myUnknownPacketTypes2MessageID.end();
         iter++)
    {
      // *IMPORTANT NOTE*: don't need to worry about duplicates...
      unknown_packets.insert(iter->first);
    } // end FOR

    // OK: now we can dump some meaningful results...
    std::string packetTypeID_string;
    for (PACKETTYPECONTAINER_CONSTITERATOR_TYPE iter = unknown_packets.begin();
         iter != unknown_packets.end();
         iter++)
    {
      converter.str(std::string(ACE_TEXT_ALWAYS_CHAR("\""))); // "reset" it...

      // (try to) create a string from the Protocol Type identifier...
      converter << RPG_Net_Common_Tools::EthernetProtocolTypeID2String(*iter);
      converter << ACE_TEXT_ALWAYS_CHAR("\" [");
      converter << myUnknownPacketTypes2MessageID.count(*iter);
      converter << ACE_TEXT_ALWAYS_CHAR(" message(s)]: ");

      for (PACKETTYPE2MESSAGEIDMAP_CONSTITERATOR_TYPE iter2 = myUnknownPacketTypes2MessageID.begin();
           iter2 != myUnknownPacketTypes2MessageID.end();
           iter2++)
      {
        // our type ?
        if (iter2->first == (*iter))
        {
          converter << iter2->second;
          converter << ACE_TEXT_ALWAYS_CHAR(", ");
        } // end IF
      } // end FOR

      // *IMPORTANT NOTE*: according to our implemented logic, there must be at least one entry
      line_string = converter.str();
      // --> there's a trailing comma: ", " !
      line_string.resize(line_string.size() - 2);

      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("%s\n"),
                 line_string.c_str()));
    } // end FOR
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("\\END--> Unknown Packet Types [total: %u] <--\n"),
               myUnknownPacketTypes2MessageID.size()));
  } // end IF

  // step2: dump (unsupported) IEEE 802.3 packets (if any)...
  if (myIEEE802_3Frames.size())
  {
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("--> IEEE 802.3 Packets <--\n")));

    converter.str(std::string()); // "reset" it...
    for (std::vector<unsigned long>::const_iterator iter = myIEEE802_3Frames.begin();
         iter != myIEEE802_3Frames.end();
         iter++)
    {
      converter << *iter;
      converter << ACE_TEXT_ALWAYS_CHAR(", ");
    } // end FOR

    // *IMPORTANT NOTE*: according to our implemented logic, there must be at least one entry
    line_string = converter.str();
    // --> there's a trailing comma: ", " !
    line_string.resize(line_string.size() - 2);

    ACE_DEBUG((LM_INFO,
               ACE_TEXT("%s\n"),
               line_string.c_str()));

    ACE_DEBUG((LM_INFO,
               ACE_TEXT("\\END--> IEEE 802.3 Packets [total: %u] <--\n"),
               myIEEE802_3Frames.size()));
  } // end IF

  // step3: dump failed messages...
  if (myFailedMessages.size())
  {
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("--> Failed Messages <--\n")));

    converter.str(std::string()); // "reset" it...
    for (std::vector<unsigned long>::const_iterator iter = myFailedMessages.begin();
         iter != myFailedMessages.end();
         iter++)
    {
      converter << *iter;
      converter << ACE_TEXT_ALWAYS_CHAR(", ");
    } // end FOR

    // *IMPORTANT NOTE*: according to our implemented logic, there must be at least one entry
    line_string = converter.str();
    // --> there's a trailing comma: ", " !
    line_string.resize(line_string.size() - 2);

    ACE_DEBUG((LM_INFO,
               ACE_TEXT("%s\n"),
               line_string.c_str()));

    ACE_DEBUG((LM_INFO,
               ACE_TEXT("\\END--> Failed Messages [total: %u] <--\n"),
               myFailedMessages.size()));
  } // end IF

  ACE_DEBUG((LM_INFO,
             ACE_TEXT(" ***** MODULE: \"%s\" state *****\\END\n"),
             ACE_TEXT_ALWAYS_CHAR(name())));
}
