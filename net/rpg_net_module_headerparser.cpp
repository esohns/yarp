//%%%FILE%%%////////////////////////////////////////////////////////////////////
// File Name: rps_flb_common_module_headerparser.cpp
//
// History:
//   Date   |Name | Description of modification
// ---------|-----|-------------------------------------------------------------
// 20/02/06 | soh | Creation.
//%%%FILE%%%////////////////////////////////////////////////////////////////////

#include "rps_flb_common_module_headerparser.h"

#include "rps_flb_common_packet_headers.h"
#include "rps_flb_common_network_tools.h"
#include "rps_flb_common_protocol_layer.h"
#include "rps_flb_common_ethernetframeheader.h"
#include "rps_flb_common_tools.h"

#include <ace/OS.h>
#include <ace/Time_Value.h>
#include <ace/Message_Block.h>

#include <sstream>

RPS_FLB_Common_Module_HeaderParser::RPS_FLB_Common_Module_HeaderParser()
 : inherited(),
   myIsInitialized(false),
   myIsLinkLayerFDDI(false)
{
  ACE_TRACE(ACE_TEXT("RPS_FLB_Common_Module_HeaderParser::RPS_FLB_Common_Module_HeaderParser"));

}

RPS_FLB_Common_Module_HeaderParser::~RPS_FLB_Common_Module_HeaderParser()
{
  ACE_TRACE(ACE_TEXT("RPS_FLB_Common_Module_HeaderParser::~RPS_FLB_Common_Module_HeaderParser"));

}

const bool RPS_FLB_Common_Module_HeaderParser::init(const bool& isLinkLayerFDDI_in)
{
  ACE_TRACE(ACE_TEXT("RPS_FLB_Common_Module_HeaderParser::init"));

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

  myIsLinkLayerFDDI = isLinkLayerFDDI_in;
  myIsInitialized = true;

  return myIsInitialized;
}

void RPS_FLB_Common_Module_HeaderParser::handleDataMessage(RPS_FLB_Common_NetworkMessage*& message_inout,
                                                           bool& passMessageDownstream_out)
{
  ACE_TRACE(ACE_TEXT("RPS_FLB_Common_Module_HeaderParser::handleDataMessage"));

  // init return value(s)
  passMessageDownstream_out = true;

  // sanity check(s)
  ACE_ASSERT(message_inout);

  if (myIsLinkLayerFDDI)
  {
    // sanity check
    if (message_inout->length() < FDDI_K_SNAP_HLEN)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("message (ID: %u) too small (%u bytes) for fddi_llc_snap header, returning\n"),
                 message_inout->getID(),
                 message_inout->length()));

      // remember these messages...
      myFailedMessages.insert(message_inout->getID());

      // clean up
      passMessageDownstream_out = false;
      message_inout->release();

      return;
    } // end IF

    // add header info to message...
    // *TODO*: we probably shouldn't simply assume this is an FDDI LLC SNAP frame !!!!
    message_inout->addHeader(RPS_FLB_Common_Protocol_Layer::FDDI_LLC_SNAP);

    // sanity check: fddi frame contains IP (version 4) ?
    if (ACE_NTOHS(ACE_reinterpret_cast(fddihdr*,
                                       message_inout->base())->hdr.llc_snap.ethertype) != ETH_P_IP)
    {
      //// debug info
      //if (myUnknownPacketTypes2MessageID.find(ether_header.getPacketTypeID()) == myUnknownPacketTypes2MessageID.end())
      //{
      //  // (try to) create a string from the header field...
      //  std::string packetTypeID_string;
      //  RPS_FLB_Common_EthernetFrameHeader::EthernetPacketTypeID2String(ether_header.getPacketTypeID(),
      //                                                                  packetTypeID_string);

      //  ACE_DEBUG((LM_DEBUG,
      //             "ethernet frame (ID: %u) contains a currently unsupported packet (type: \"%s\"), aborting\n",
      //             message_in.getID(),
      //             packetTypeID_string.c_str()));
      //} // end IF

      // add message to map
      //myUnknownPacketTypes2MessageID[ether_header.getPacketTypeID()] = message_in.getID();
      myUnknownPacketTypes2MessageID.insert(PACKETTYPE2MESSAGEIDPAIR_TYPE(ACE_reinterpret_cast(fddihdr*,
                                                                                               message_inout->base())->hdr.llc_snap.ethertype,
                                                                          message_inout->getID()));

      return;
    } // end IF
  } // end IF
  else
  {
    // sanity check
    if (message_inout->length() < ETH_HLEN)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("message (ID: %u) too small (%u bytes) for ethernet header, returning\n"),
                 message_inout->getID(),
                 message_inout->length()));

      // remember these messages...
      myFailedMessages.insert(message_inout->getID());

      // clean up
      passMessageDownstream_out = false;
      message_inout->release();

      return;
    } // end IF

    // (try to) parse Ethernet header

    // add header info to message...
    // *TODO*: we probably shouldn't simply assume this is an Ethernet frame !!!!
    message_inout->addHeader(RPS_FLB_Common_Protocol_Layer::ETHERNET);

    // sanity check: ethernet frame is of type Ethernet II ?
    // *IMPORTANT NOTE*: for 802.3-type frames, this returns the "length"
    // *IMPORTANT NOTE*: we assume that the message is an ethernet frame (no offset !)
    if (ACE_NTOHS(ACE_reinterpret_cast(ether_header*,
                                       message_inout->base())->ether_type) <= ETH_DATA_LEN)
    {
      // remember these messages...
      myIEEE802_3Frames.insert(message_inout->getID());

    //     ACE_DEBUG((LM_DEBUG,
    //                "message (ID: %u) contains a currently unsupported IEEE 802.3 frame (\"len\" field: %u), aborting\n",
    //                message_in.getID(),
    //                ACE_NTOHS(ether_header.getPacketTypeID())));

      return;
    } // end IF

    // sanity check: ethernet frame contains IP (version 4) ?
    if (ACE_NTOHS(ACE_reinterpret_cast(ether_header*,
                                       message_inout->base())->ether_type) != ETH_P_IP)
    {
      //// debug info
      //if (myUnknownPacketTypes2MessageID.find(ether_header.getPacketTypeID()) == myUnknownPacketTypes2MessageID.end())
      //{
      //  // (try to) create a string from the header field...
      //  std::string packetTypeID_string;
      //  RPS_FLB_Common_EthernetFrameHeader::EthernetPacketTypeID2String(ether_header.getPacketTypeID(),
      //                                                                  packetTypeID_string);

      //  ACE_DEBUG((LM_DEBUG,
      //             "ethernet frame (ID: %u) contains a currently unsupported packet (type: \"%s\"), aborting\n",
      //             message_in.getID(),
      //             packetTypeID_string.c_str()));
      //} // end IF

      // add message to map
      //myUnknownPacketTypes2MessageID[ether_header.getPacketTypeID()] = message_in.getID();
      myUnknownPacketTypes2MessageID.insert(PACKETTYPE2MESSAGEIDPAIR_TYPE(ACE_reinterpret_cast(ether_header*,
                                                                                               message_inout->base())->ether_type,
                                                                          message_inout->getID()));

      return;
    } // end IF
  } // end ELSE

  // *IMPORTANT NOTE*: a standard IP header consists of AT LEAST 5 32-bit fields...
  // sanity check
  // *WARNING*: addHeader shifts the rd_ptr of the message, so length() has already been adjusted !
  if (message_inout->length() < (5 * 4))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("message (ID: %u) too small (%u bytes) for IPv4 header, returning\n"),
               message_inout->getID(),
               message_inout->length()));

    // remember these messages...
    myFailedMessages.insert(message_inout->getID());

    // clean up
    passMessageDownstream_out = false;
    message_inout->release();

    return;
  } // end IF

  // add corresponding header to message...
  // *TODO*: what if it's IPv6 ???
  message_inout->addHeader(RPS_FLB_Common_Protocol_Layer::IPv4);

  // sanity check: IP version 4 contains UDP/TCP ?
  // sanity check
  if (ACE_reinterpret_cast(iphdr*,
                           (message_inout->base() + ETH_HLEN))->version != 4)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("message (ID: %u) contains IP header with unsupported version: \"%u\", returning\n"),
               message_inout->getID(),
               ACE_reinterpret_cast(iphdr*,
                                    (message_inout->base() + ETH_HLEN))->version));

    return;
  } // end IF

  // sanity check: this does not work, as this IP packet MAY be fragmented...
//   if (message_in->size() != (ETH_HLEN + ACE_NTOHS(ip_header.getTotalPacketLength())))
//   {
//     ACE_DEBUG((LM_ERROR,
//                "message (ID: %u) size (%u bytes excl. ethernet header) does not match the size given in the IPv4 header (%u bytes), aborting\n",
//                message_in->getID(),
//                message_in->size() - ETH_HLEN,
//                ACE_NTOHS(ip_header.getTotalPacketLength())));
//
//     // nothing more to do...
//     return false;
//   }

  switch (ACE_reinterpret_cast(iphdr*,
                               (message_inout->base() + ETH_HLEN))->protocol)
  {
    case IPPROTO_TCP:
    {
      // add corresponding header to message...
      message_inout->addHeader(RPS_FLB_Common_Protocol_Layer::TCP);

      break;
    }
    case IPPROTO_UDP:
    {
      // add corresponding header to message...
      message_inout->addHeader(RPS_FLB_Common_Protocol_Layer::UDP);

      break;
    }
    default:
    {
      // (try to) create a string from the header field...
//       std::string IPProtocol_string;
//       RPS_FLB_Common_IPHeader::IPProtocol2String(ip_header.getProtocolIdentifier(),
//                                                  IPProtocol_string);
//
//       ACE_DEBUG((LM_DEBUG,
//                  "IP packet (ID: %u) contains a currently unsupported protocol: \"%s\", continuing\n",
//                  message_in->getID(),
//                  IPProtocol_string.c_str()));
    }
  } // end SWITCH
}

void RPS_FLB_Common_Module_HeaderParser::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPS_FLB_Common_Module_HeaderParser::dump_state"));

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
      RPS_FLB_Common_EthernetFrameHeader::EthernetProtocolTypeID2String(*iter,
                                                                        packetTypeID_string);

      converter << packetTypeID_string;
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
    for (RPS_FLB_Common_MessageIDListConstIterator_Type iter = myIEEE802_3Frames.begin();
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
    for (RPS_FLB_Common_MessageIDListConstIterator_Type iter = myFailedMessages.begin();
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
