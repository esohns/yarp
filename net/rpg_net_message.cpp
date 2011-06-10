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

#include "rpg_net_message.h"

#include "rpg_net_remote_comm.h"

#include <rpg_common_macros.h>

#include <ace/Message_Block.h>

// *NOTE*: this is implicitly invoked by duplicate() as well...
RPG_Net_Message::RPG_Net_Message(const RPG_Net_Message& message_in)
 : inherited(message_in),
   myIsInitialized(message_in.myIsInitialized)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::RPG_Net_Message"));

}

RPG_Net_Message::RPG_Net_Message(ACE_Data_Block* dataBlock_in,
                                 ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,         // use (don't own !) this data block
             messageAllocator_in), // use this when destruction is imminent...
   myIsInitialized(true)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::RPG_Net_Message"));

}

// RPG_Net_Message::RPG_Net_Message(ACE_Allocator* messageAllocator_in)
//  : inherited(messageAllocator_in,
//              true), // usually, we want to increment the running message counter...
//    myIsInitialized(false) // not initialized --> call init() !
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_Message::RPG_Net_Message"));
//
// }

RPG_Net_Message::~RPG_Net_Message()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::~RPG_Net_Message"));

  // *NOTE*: this will be called just BEFORE we're passed back
  // to the allocator !!!

  // clean up
  myIsInitialized = false;
}

void
RPG_Net_Message::init(ACE_Data_Block* dataBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::init"));

  // sanity check: shouldn't be initialized...
  ACE_ASSERT(!myIsInitialized);

  // init base class...
  inherited::init(dataBlock_in);

  // OK: we're initialized !
  myIsInitialized = true;
}

const int
RPG_Net_Message::getCommand() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::getCommand"));

  // sanity check(s)
  ACE_ASSERT(length() >= sizeof(RPG_Net_MessageHeader));

  RPG_Net_MessageHeader* message_header = reinterpret_cast<RPG_Net_MessageHeader*> (rd_ptr());

  return message_header->messageType;
}

void
RPG_Net_Message::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("***** Message (ID: %u) *****\n"),
             getID()));

//   // step1: dump individual header types & offsets
//   std::string protocol_layer;
//   for (HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.begin();
//        iter != myHeaders.end();
//        iter++)
//   {
//     RPG_Net_Protocol_Layer::ProtocolLayer2String(iter->first,
//                                                         protocol_layer);
//
//     ACE_DEBUG((LM_INFO,
//                ACE_TEXT("--> header type: \"%s\" @ offset: %u\n"),
//                protocol_layer.c_str(),
//                iter->second));
//   } // end FOR
//
//   // step2: dump individual headers, top-down (collect sizes)
//   unsigned long sum_header_size = 0;
//   for (HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.begin();
//        iter != myHeaders.end();
//        iter++)
//   {
//     switch (iter->first)
//     {
// //       case RPG_Net_Protocol_Layer::ASTERIX:
// //       {
// //         Stream_ASTERIXHeader header(*this,
// //                                     iter->second);
// //
// //         // remember size
// //         sum_header_size += header.length();
// //
// //         header.dump_state();
// //
// //         break;
// //       }
// //       case RPG_Net_Protocol_Layer::ASTERIX_offset: // "resilience" bytes...
// //       {
// //         // remember size
// //         sum_header_size += FLB_RPS_ASTERIX_RESILIENCE_BYTES;
// //
// //         // don't have a header class for this...
// //         ACE_DEBUG((LM_INFO,
// //                    ACE_TEXT(" *** ASTERIX_offset (%u bytes) ***\n"),
// //                    FLB_RPS_ASTERIX_RESILIENCE_BYTES));
// //
// //         break;
// //       }
//       case RPG_Net_Protocol_Layer::TCP:
//       {
//         struct tcphdr* header = reinterpret_cast<struct tcphdr*> (//                                                      (rd_ptr() + iter->second));
//
//         // remember size
//         // *NOTE*: TCP header field "Data Offset" gives the size of the
//         // TCP header in 32 bit words...
//         sum_header_size += (header->doff * 4);
//
//         // debug info
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("*** TCP (%u bytes) ***\nSource#: %u --> %u\nDestination#: %u --> %u\nSequence#: %u --> %u (swapped)\nAcknowledgement#: %u --> %u (swapped)\nReserved: %u\nData Offset: %u --> %u bytes\nFlags: %u\nWindow: %u --> %u (swapped)\nChecksum: %u --> %u (swapped)\nUrgent Pointer: %u --> %u (swapped)\n"),
//                    (header->doff * 4),
//                    header->source,
//                    ACE_NTOHS(header->source), // byte swapping
//                    header->dest,
//                    ACE_NTOHS(header->dest), // byte swapping
//                    header->seq,
//                    ACE_NTOHL(header->seq), // byte swapping
//                    header->ack_seq,
//                    ACE_NTOHL(header->ack_seq), // byte swapping
//                    header->res1,
//                    header->doff,
//                    (header->doff * 4), // convert to bytes (value is in 32 bit words)
//                    0, // *TODO*
//                    header->window,
//                    ACE_NTOHS(header->window), // byte swapping
//                    header->check,
//                    ACE_NTOHS(header->check), // byte swapping
//                    header->urg_ptr,
//                    ACE_NTOHS(header->urg_ptr))); // byte swapping
//
//         break;
//       }
//       case RPG_Net_Protocol_Layer::UDP:
//       {
//         struct udphdr* header = reinterpret_cast<struct udphdr*> (//                                                      (rd_ptr() + iter->second));
//
//         // remember size
//         // *NOTE*: UDP headers are 8 bytes long...
//         sum_header_size += sizeof(struct udphdr);
//
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("*** UDP (%u bytes) ***\nSource#: %u --> %u\nDestination#: %u --> %u\nLength: %u --> %u bytes\nChecksum: %u --> %u (swapped)\n"),
//                    sizeof(struct udphdr),
//                    header->source,
//                    ACE_NTOHS(header->source), // byte swapping
//                    header->dest,
//                    ACE_NTOHS(header->dest), // byte swapping
//                    header->len,
//                    ACE_NTOHS(header->len), // byte swapping
//                    header->check,
//                    ACE_NTOHS(header->check))); // byte swapping
//
//         break;
//       }
//       case RPG_Net_Protocol_Layer::IPv4:
//       {
//         struct iphdr* header = reinterpret_cast<struct iphdr*> (//                                                     (rd_ptr() + iter->second));
//
//         // remember size
//         // *NOTE*: IPv4 header field "Header Length" gives the size of the
//         // IP header in 32 bit words...
//         sum_header_size += (header->ihl * 4);
//
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("*** IP (%u bytes) ***\nVersion: %u\nHeader Length: %u --> %u bytes\nType-of-Service Flags: %u\nTotal Packet Length: %u --> %u bytes\nFragment Identifier: %u --> %u (swapped)\nFragmentation Flags: %u (3 LSBits)\nFragmentation Offset: %u\nTime-to-Live: %u\nProtocol Identifier: %u --> \"%s\"\nHeader Checksum: %u --> %u (swapped)\nSource#: %u --> \"%s\"\nDestination#: %u --> \"%s\"\nOptions: %u byte(s)\n"),
//                    (header->ihl * 4),
//                    header->version,
//                    header->ihl,
//                    (header->ihl * 4), // <-- Header Length is in in multiples of 32 bits
//                    header->tos,
//                    header->tot_len,
//                    ACE_NTOHS(header->tot_len), // byte swapping
//                    header->id,
//                    ACE_NTOHS(header->id), // byte swapping
//                    (ACE_NTOHS(header->frag_off) >> 13), // consider the head three bits...
//                    (ACE_NTOHS(header->frag_off) & IP_OFFMASK),
//                    header->ttl,
//                    header->protocol,
//                    RPG_Net_Common_Tools::IPProtocol2String(header->protocol).c_str(),
//                    header->check,
//                    ACE_NTOHS(header->check), // byte swapping
//                    header->saddr,
//                    RPG_Net_Common_Tools::IPAddress2String(header->saddr, 0).c_str(), // no port
//                    header->daddr,
//                    RPG_Net_Common_Tools::IPAddress2String(header->daddr, 0).c_str(), // no port
//                    (header->ihl - 5)));
//
//         break;
//       }
//       case RPG_Net_Protocol_Layer::FDDI_LLC_SNAP:
//       {
//         struct fddihdr* header = reinterpret_cast<struct fddihdr*> (//                                                       (rd_ptr() + iter->second));
//
//         // remember size
//         // *NOTE*: for the time being, we only support LLC SNAP...
//         sum_header_size += FDDI_K_SNAP_HLEN;
//
//         // *TODO*: add Organization Code
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("*** FDDI_LLC_SNAP (%u bytes) ***\nFrame Control: %u\nDestination MAC#: \"%s\"\nSource MAC#: \"%s\"\nDSAP: %u\nSSAP: %u\nControl Field: %u\nPacket Type ID: %u --> %u (swapped) --> \"%s\"\n"),
//                    FDDI_K_SNAP_HLEN,
//                    header->fc,
//                    RPG_Net_Common_Tools::MACAddress2String((rd_ptr() + iter->second) + 1).c_str(),
//                    RPG_Net_Common_Tools::MACAddress2String((rd_ptr() + iter->second) + 1 + FDDI_K_ALEN).c_str(),
//                    header->hdr.llc_snap.dsap,
//                    header->hdr.llc_snap.ssap,
//                    header->hdr.llc_snap.ctrl,
//                    header->hdr.llc_snap.ethertype,
//                    ACE_NTOHS(header->hdr.llc_snap.ethertype), // byte swapping
//                    RPG_Net_Common_Tools::EthernetProtocolTypeID2String(header->hdr.llc_snap.ethertype).c_str()));
//
//         break;
//       }
//       case RPG_Net_Protocol_Layer::ETHERNET:
//       {
//         struct ether_header* header = reinterpret_cast<struct ether_header*> (//                                                            (rd_ptr() + iter->second));
//
//         // remember size
//         // *NOTE*: Ethernet headers are 14 bytes long...
//         sum_header_size += ETH_HLEN;
//
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("*** ETHERNET (%u bytes) ***\nDestination MAC#: \"%s\"\nSource MAC#: \"%s\"\nProtocol Type/Length: %u --> %u (swapped) --> \"%s\"\n"),
//                    ETH_HLEN,
//                    RPG_Net_Common_Tools::MACAddress2String((rd_ptr() + iter->second)).c_str(),
//                    RPG_Net_Common_Tools::MACAddress2String((rd_ptr() + iter->second) + ETH_ALEN).c_str(),
//                    header->ether_type,
//                    ACE_NTOHS(header->ether_type), // byte swapping
//                    RPG_Net_Common_Tools::EthernetProtocolTypeID2String(header->ether_type).c_str()));
//
//         break;
//       }
//       default:
//       {
//         // debug info
//         RPG_Net_Protocol_Layer::ProtocolLayer2String(iter->first,
//                                                      protocol_layer);
//
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("message (ID: %u) contains unknown protocol header type \"%s\" at offset: %u --> check implementation, continuing\n"),
//                    getID(),
//                    protocol_layer.c_str(),
//                    iter->second));
//
//         break;
//       }
//     } // end SWITCH
//   } // end FOR
//
//   // step3: dump total size
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("--> total message (ID: %u) size: %u byte(s) (%u header byte(s))\n"),
//              getID(),
//              (length() + sum_header_size),
//              sum_header_size));
}

const bool
RPG_Net_Message::crunchForHeader(const unsigned long& headerSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::crunchForHeader"));

  // sanity check(s)
  ACE_ASSERT(size() >= headerSize_in);         // enough space ?
  ACE_ASSERT(total_length() >= headerSize_in); // enough data ?

  // anything to do at all ?
  if (length() >= headerSize_in)
    return true; // nothing to do...

  ACE_Message_Block* source_block = this;
  size_t missing_data = headerSize_in - length();
  size_t amount = 0;
  while (missing_data)
  {
    // *sigh*: copy some data from the chain...
    source_block = cont();

    // skip over any "empty" continuations...
    while (source_block->length() == 0)
      source_block = source_block->cont();

    // copy some data... this adjusts our write pointer
    amount = (source_block->length() < missing_data ? source_block->length() : missing_data);
    if (copy(source_block->rd_ptr(), amount))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::copy(): \"%m\", aborting\n")));

      return false;
    } // end IF

    missing_data -= amount;

    // adjust the continuation accordingly...
    source_block->rd_ptr(amount);
  } // end WHILE

  // sanity check
  ACE_ASSERT(length() == headerSize_in);

  return true;
}

ACE_Message_Block*
RPG_Net_Message::duplicate(void) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::duplicate"));

  RPG_Net_Message* nb = NULL;

  // create a new RPG_Net_Message that contains unique copies of
  // the message block fields, but a (reference counted) shallow duplicate of
  // the ACE_Data_Block.

  // if there is no allocator, use the standard new and delete calls.
  if (message_block_allocator_ == NULL)
  {
    ACE_NEW_RETURN(nb,
                   RPG_Net_Message(*this),
                   NULL);
  } // end IF
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to malloc SHOULDN'T really matter, as this will be
    // a "shallow" copy which just references our data block...
    // *TODO*: (depending on the allocator) we senselessly allocate a datablock
    // anyway, only to immediately release it again...
    ACE_NEW_MALLOC_RETURN(nb,
                          static_cast<RPG_Net_Message*> (message_block_allocator_->malloc(capacity())),
                          RPG_Net_Message(*this),
                          NULL);
  } // end ELSE

  // increment the reference counts of all the continuation messages
  if (cont_)
  {
    nb->cont_ = cont_->duplicate();

    // If things go wrong, release all of our resources and return
    if (nb->cont_ == 0)
    {
      nb->release();
      nb = NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return nb;
}

const std::string
RPG_Net_Message::commandType2String(const RPG_Net_MessageType& messageType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Message::commandType2String"));

  std::string result = ACE_TEXT("INVALID");

  switch (messageType_in)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
      result = ACE_TEXT("RPG_NET_PING"); break;
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
      result = ACE_TEXT("RPG_NET_PONG"); break;
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid message type (was %d), aborting\n"),
                 messageType_in));

      break;
    }
  } // end SWITCH

  return result;
}

// void
// RPG_Net_Message::adjustDataOffset(const RPG_Net_MessageHeader_t& headerType_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_Message::adjustDataOffset"));
//
//   unsigned long dataOffset = 0;
//
//   // create header
//   switch (headerType_in)
//   {
//     case RPG_Net_Protocol_Layer::ETHERNET:
//     {
//       // *NOTE*: Ethernet headers are 14 bytes long...
//       dataOffset = ETH_HLEN;
//
//       break;
//     }
//     case RPG_Net_Protocol_Layer::FDDI_LLC_SNAP:
//     {
//       // *NOTE*:
//       // - FDDI LLC headers are 13 bytes long...
//       // - FDDI SNAP headers are 8 bytes long...
//       dataOffset = FDDI_K_SNAP_HLEN;
//
//       break;
//     }
//     case RPG_Net_Protocol_Layer::IPv4:
//     {
//       // *NOTE*: IPv4 header field "Header Length" gives the size of the
//       // IP header in 32 bit words...
//       // *NOTE*: use our current offset...
//       dataOffset = (reinterpret_cast<iphdr*> (//                                          rd_ptr())->ihl * 4);
//
//       break;
//     }
//     case RPG_Net_Protocol_Layer::TCP:
//     {
//       // *NOTE*: TCP header field "Data Offset" gives the size of the
//       // TCP header in 32 bit words...
//       // *NOTE*: use our current offset...
//       dataOffset = (reinterpret_cast<tcphdr*> (//                                          rd_ptr())->doff * 4);
//
//       break;
//     }
//     case RPG_Net_Protocol_Layer::UDP:
//     {
//       // *NOTE*: UDP headers are 8 bytes long...
//       dataOffset = 8;
//
//       break;
//     }
// //     case RPG_Net_Protocol_Layer::ASTERIX_offset:
// //     {
// //       // *NOTE*: ASTERIX "resilience" headers are 4 bytes long...
// //       dataOffset = FLB_RPS_ASTERIX_RESILIENCE_BYTES;
// //
// //       break;
// //     }
// //     case RPG_Net_Protocol_Layer::ASTERIX:
// //     {
// //       // *NOTE*: ASTERIX headers are 3 bytes long...
// //       dataOffset = FLB_RPS_ASTERIX_HEADER_SIZE;
// //
// //       break;
// //     }
//     default:
//     {
//       // debug info
//       std::string type_string;
//       RPG_Net_Protocol_Layer::ProtocolLayer2String(headerType_in,
//                                                           type_string);
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("message (ID: %u) header (type: \"%s\") is currently unsupported, continuing\n"),
//                  getID(),
//                  type_string.c_str()));
//
//       break;
//     }
//   } // end SWITCH
//
//   // advance rd_ptr() to the start of the data (or to the next header in the stack)...
//   rd_ptr(dataOffset);
// }
