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

#include "stream_message.h"

#include "stream_packet_headers.h"
#include "stream_protocol_layer.h"

#include "stream_asterixheader.h"
#include "stream_tcpheader.h"
#include "stream_udpheader.h"
#include "stream_ipheader.h"
#include "stream_fddiframeheader.h"
#include "stream_ethernetframeheader.h"
#include "stream_packet_headers.h"

// *IMPORTANT NOTE*: this is implicitly invoked by duplicate() as well...
Stream_Message::Stream_Message(const Stream_Message& message_in)
 : inherited(message_in),
   myHeaders(message_in.myHeaders),
   myIsInitialized(true)
{
  ACE_TRACE(ACE_TEXT("Stream_Message::Stream_Message"));

}

// *IMPORTANT NOTE*: to be used by allocators ONLY !
Stream_Message::Stream_Message(ACE_Data_Block* dataBlock_in,
                               ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,         // use (don't own !) this data block
             messageAllocator_in), // use this when destruction is imminent...
   myIsInitialized(false) // not initialized --> call init() !
{
  ACE_TRACE(ACE_TEXT("Stream_Message::Stream_Message"));

}

// *IMPORTANT NOTE*: to be used by allocators ONLY !
Stream_Message::Stream_Message(ACE_Allocator* messageAllocator_in,
                               const bool& incrementMessageCounter_in)
 : inherited(messageAllocator_in,
             incrementMessageCounter_in),
   myIsInitialized(false) // not initialized --> call init() !
{
  ACE_TRACE(ACE_TEXT("Stream_Message::Stream_Message"));

}

Stream_Message::~Stream_Message()
{
  ACE_TRACE(ACE_TEXT("Stream_Message::~Stream_Message"));

  // *IMPORTANT NOTE*: this will be called just BEFORE we're passed back
  // to the allocator !!!

  // clean up
  myHeaders.clear();
  myIsInitialized = false;
}

void
Stream_Message::init()
{
  ACE_TRACE(ACE_TEXT("Stream_Message::init"));

  // sanity check: shouldn't be initialized...
  ACE_ASSERT(!myIsInitialized);

  // init base class...
//   inherited::init(scheduledPlaybackTime_in);

  // OK: we're initialized !
  myIsInitialized = true;
}

void
Stream_Message::init(ACE_Data_Block* dataBlock_in)
{
  ACE_TRACE(ACE_TEXT("Stream_Message::init"));

  // sanity check: shouldn't be initialized...
  ACE_ASSERT(!myIsInitialized);

  // init base class...
  inherited::init(dataBlock_in);

  // OK: we're initialized !
  myIsInitialized = true;
}

void
Stream_Message::addHeader(const Stream_MessageHeader_Type& headerType_in)
{
  ACE_TRACE(ACE_TEXT("Stream_Message::addHeader"));

  // adjust top-level protocol accordingly
  // getToplevelProtocol() throws a warning if myHeaders is empty --> avoid this
  if (!myHeaders.empty())
  {
    // sanity check: do NOT support "unknown" protocol stacks
    // *IMPORTANT NOTE*: this works because protocol abstraction increases along the corresponding
    // enum...
    // *IMPORTANT NOTE*: this is NOT an error for "adjacent" ASTERIX headers...
    if ((headerType_in <= getToplevelProtocol()) &&
        (headerType_in != Stream_Protocol_Layer::ASTERIX))
    {
      // debug info
      std::string type_string_in;
      std::string type_string_top;
      Stream_Protocol_Layer::ProtocolLayer2String(headerType_in,
                                                          type_string_in);
      Stream_Protocol_Layer::ProtocolLayer2String(getToplevelProtocol(),
                                                          type_string_top);

      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("message (ID: %u): failed to add (\"%s\") header to existing stack (top-level protocol: \"%s\") --> check implementation !, returning\n"),
                 getID(),
                 type_string_in.c_str(),
                 type_string_top.c_str()));

      // *TODO*: should we do anything else (like e.g. returning "false" here ?)
      return;
    } // end IF
  } // end IF

  // sanity check: offset within bounds ?
  ACE_ASSERT(ACE_static_cast(size_t,
                             (rd_ptr() - base())) <= capacity());

  // remember current offset...
  myHeaders.insert(HEADERCONTAINERPAIR_TYPE(headerType_in,
                                            inherited::rd_ptr_));

  // ... and adjust the "data" offset accordingly
  adjustDataOffset(headerType_in);
}

const bool
Stream_Message::getHeaderOffset(const Stream_MessageHeader_Type& headerType_in,
                                unsigned long& offset_out) const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::getHeaderOffset"));

  // init return value(s)
  offset_out = 0;

  // find the first header whose layer is NOT BELOW headerType_in...
  HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.lower_bound(headerType_in);
  if (iter == myHeaders.end())
  {
    // this header type simply doesn't exist in this message...

    // debug info
    //std::string type_string;
    //Stream_Protocol_Layer::ProtocolLayer2String(headerType_in,
    //                                                    type_string);
    //ACE_DEBUG((LM_WARNING,
    //           "message (ID: %u) does not contain a header of type \"%s\", aborting\n",
    //           getID(),
    //           type_string.c_str()));

    return false;
  } // end IF
  else if (iter->first != headerType_in)
  {
    // this header type doesn't exist in this message (but we found a higher-level
    // header instead !)...

    // debug info
    std::string type_string_input;
    std::string type_string_found;
    Stream_Protocol_Layer::ProtocolLayer2String(headerType_in,
                                                        type_string_input);
    Stream_Protocol_Layer::ProtocolLayer2String(iter->first,
                                                        type_string_found);
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("message (ID: %u) doesn't contain a header of type \"%s\" (found \"%s\" instead), aborting\n"),
               getID(),
               type_string_input.c_str(),
               type_string_found.c_str()));

    return false;
  } // end ELSE

  // OK: if we've come this far, we have found AT LEAST ONE header of the correct type...

  // return the (first) instance...
  offset_out = iter->second;

  return true;
}

const bool
Stream_Message::hasProtocol(const Stream_Protocol_Type& protocolType_in) const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::hasProtocol"));

  // find the first header whose layer is NOT BELOW headerType_in...
  HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.lower_bound(protocolType_in);
  if ((iter == myHeaders.end()) ||
      (iter->first != protocolType_in))
  {
    // header type doesn't exist in this message...

    // debug info
    //std::string type_string;
    //Stream_Protocol_Layer::ProtocolLayer2String(protocolType_in,
    //                                                    type_string);
    //ACE_DEBUG((LM_WARNING,
    //           "message (ID: %u) does not contain a header of type \"%s\", aborting\n",
    //           getID(),
    //           type_string.c_str()));

    return false;
  } // end IF

  return true;
}

const Stream_Protocol_Type
Stream_Message::getToplevelProtocol() const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::getToplevelProtocol"));

  // sanity check: no headers ?
  if (myHeaders.empty())
  {
//     ACE_DEBUG((LM_WARNING,
//                ACE_TEXT("message (ID: %u): doesn't contain any headers (yet) --> check implementation !, returning\n"),
//                getID()));

    // *TODO*: clean this up !
    static Stream_Protocol_Type dummy = Stream_Protocol_Layer::INVALID_PROTOCOL;
    return dummy;
  } // end IF

  // return last element (ist MAX in a map !)
  // *WARNING*: this may break IF you change the type of the container !
  return myHeaders.rbegin()->first;
}

const bool
Stream_Message::hasTransportLayerHeader() const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::hasTransportLayerHeader"));

  // *WARNING*: might have to change this in the future !
  return (getToplevelProtocol() >= Stream_Protocol_Layer::UDP);
}

void
Stream_Message::dump_state() const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("***** Message (ID: %u) *****\n"),
             getID()));

  // step1: dump individual header types & offsets
  std::string protocol_layer;
  for (HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.begin();
       iter != myHeaders.end();
       iter++)
  {
    Stream_Protocol_Layer::ProtocolLayer2String(iter->first,
                                                        protocol_layer);

    ACE_DEBUG((LM_INFO,
               ACE_TEXT("--> header type: \"%s\" @ offset: %u\n"),
               protocol_layer.c_str(),
               iter->second));
  } // end FOR

  // step2: dump individual headers, top-down (collect sizes)
  unsigned long sum_header_size = 0;
  for (HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.begin();
       iter != myHeaders.end();
       iter++)
  {
    switch (iter->first)
    {
      case Stream_Protocol_Layer::ASTERIX:
      {
        Stream_ASTERIXHeader header(*this,
                                            iter->second);

        // remember size
        sum_header_size += header.length();

        header.dump_state();

        break;
      }
      case Stream_Protocol_Layer::ASTERIX_offset: // "resilience" bytes...
      {
        // remember size
        sum_header_size += FLB_RPS_ASTERIX_RESILIENCE_BYTES;

        // don't have a header class for this...
        ACE_DEBUG((LM_INFO,
                   ACE_TEXT(" *** ASTERIX_offset (%u bytes) ***\n"),
                   FLB_RPS_ASTERIX_RESILIENCE_BYTES));

        break;
      }
      case Stream_Protocol_Layer::TCP:
      {
        Stream_TCPHeader header(*this,
                                        iter->second);

        // remember size
        sum_header_size += header.length();

        header.dump_state();

        break;
      }
      case Stream_Protocol_Layer::UDP:
      {
        Stream_UDPHeader header(*this,
                                        iter->second);

        // remember size
        sum_header_size += header.length();

        header.dump_state();

        break;
      }
      case Stream_Protocol_Layer::IPv4:
      {
        Stream_IPHeader header(*this,
                                       iter->second);

        // remember size
        sum_header_size += header.length();

        header.dump_state();

        break;
      }
      case Stream_Protocol_Layer::FDDI_LLC_SNAP:
      {
        Stream_FDDIFrameHeader header(*this,
                                              iter->second);

        // remember size
        sum_header_size += header.length();

        header.dump_state();

        break;
      }
      case Stream_Protocol_Layer::ETHERNET:
      {
        Stream_EthernetFrameHeader header(*this,
                                                  iter->second);

        // remember size
        sum_header_size += header.length();

        header.dump_state();

        break;
      }
      default:
      {
        // debug info
        Stream_Protocol_Layer::ProtocolLayer2String(iter->first,
                                                            protocol_layer);

        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("message (ID: %u) contains unknown protocol header type \"%s\" at offset: %u --> check implementation, continuing\n"),
                   getID(),
                   protocol_layer.c_str(),
                   iter->second));

        break;
      }
    } // end SWITCH
  } // end FOR

  // step3: dump total size
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("--> total message (ID: %u) size: %u byte(s) (%u header byte(s))\n"),
             getID(),
             (length() + sum_header_size),
             sum_header_size));
}

ACE_Message_Block*
Stream_Message::duplicate(void) const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::duplicate"));

  Stream_Message* nb = NULL;

  // create a new <Stream_Message> that contains unique copies of
  // the message block fields, but a (reference counted) shallow duplicate of
  // the <ACE_Data_Block>.

  // if there is no allocator, use the standard new and delete calls.
  if (message_block_allocator_ == NULL)
  {
    ACE_NEW_RETURN(nb,
                   Stream_Message(*this),
                   NULL);
  } // end IF
  else // otherwise, use the existing message_block_allocator
  {
    // *IMPORTANT NOTE*: by passing the "magic" value of 0 (instead of sizeof(Stream_Message) or the like),
    // we (hopefully) signal the allocator to omit incrementing the running message counter here...
    // *TODO*: find a better way of doing this !
    ACE_NEW_MALLOC_RETURN(nb,
                          ACE_static_cast(Stream_Message*,
                                          message_block_allocator_->malloc(0)), // DON'T increment running message counter !
                          Stream_Message(*this),
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

  return nb;
}

const bool
Stream_Message::getTransportLayerTypeAndOffset(Stream_MessageHeader_Type& headerType_out,
                                               unsigned long& offset_out) const
{
  ACE_TRACE(ACE_TEXT("Stream_Message::getTransportLayerTypeAndOffset"));

  // init return value(s)
  headerType_out = Stream_Protocol_Layer::ETHERNET;
  offset_out     = 0;

  for (HEADERCONTAINER_CONSTITERATOR_TYPE iter = myHeaders.begin();
       iter != myHeaders.end();
       iter++)
  {
    if ((iter->first == Stream_Protocol_Layer::UDP) ||
        (iter->first == Stream_Protocol_Layer::TCP))
    {
      headerType_out = iter->first;
      offset_out = iter->second;

      return true;
    } // end IF
  } // end FOR

  // no/unknown transport layer
  return false;
}

void
Stream_Message::adjustDataOffset(const Stream_MessageHeader_Type& headerType_in)
{
  ACE_TRACE(ACE_TEXT("Stream_Message::adjustDataOffset"));

  unsigned long dataOffset = 0;

  // create header
  switch (headerType_in)
  {
    case Stream_Protocol_Layer::ETHERNET:
    {
      // *IMPORTANT NOTE*: Ethernet headers are 14 bytes long...
      dataOffset = ETH_HLEN;

      break;
    }
    case Stream_Protocol_Layer::FDDI_LLC_SNAP:
    {
      // *IMPORTANT NOTE*:
      // - FDDI LLC headers are 13 bytes long...
      // - FDDI SNAP headers are 8 bytes long...
      dataOffset = FDDI_K_SNAP_HLEN;

      break;
    }
    case Stream_Protocol_Layer::IPv4:
    {
      // *IMPORTANT NOTE*: IPv4 header field "Header Length" gives the size of the
      // IP header in 32 bit words...
      // *IMPORTANT NOTE*: use our current offset...
      dataOffset = (ACE_reinterpret_cast(iphdr*,
                                         rd_ptr())->ihl * 4);

      break;
    }
    case Stream_Protocol_Layer::TCP:
    {
      // *IMOPRTANT NOTE*: TCP header field "Data Offset" gives the size of the
      // TCP header in 32 bit words...
      // *IMPORTANT NOTE*: use our current offset...
      dataOffset = (ACE_reinterpret_cast(tcphdr*,
                                         rd_ptr())->doff * 4);

      break;
    }
    case Stream_Protocol_Layer::UDP:
    {
      // *IMPORTANT NOTE*: UDP headers are 8 bytes long...
      dataOffset = 8;

      break;
    }
    case Stream_Protocol_Layer::ASTERIX_offset:
    {
      // *IMPORTANT NOTE*: ASTERIX "resilience" headers are 4 bytes long...
      dataOffset = FLB_RPS_ASTERIX_RESILIENCE_BYTES;

      break;
    }
    case Stream_Protocol_Layer::ASTERIX:
    {
      // *IMPORTANT NOTE*: ASTERIX headers are 3 bytes long...
      dataOffset = FLB_RPS_ASTERIX_HEADER_SIZE;

      break;
    }
    default:
    {
      // debug info
      std::string type_string;
      Stream_Protocol_Layer::ProtocolLayer2String(headerType_in,
                                                          type_string);
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("message (ID: %u) header (type: \"%s\") is currently unsupported --> check implementation, continuing\n"),
                 getID(),
                 type_string.c_str()));

      break;
    }
  } // end SWITCH

  // advance rd_ptr() to the start of the data (or to the next header in the stack)...
  rd_ptr(dataOffset);
}
