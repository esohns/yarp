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

#include "rpg_net_message.h"

#include "rpg_net_remote_comm.h"

#include "rpg_common_macros.h"

#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>

// *NOTE*: this is implicitly invoked by duplicate() as well...
template <typename ProtocolCommandType>
RPG_Net_MessageBase<ProtocolCommandType>::RPG_Net_MessageBase(const RPG_Net_MessageBase& message_in)
 : inherited(message_in),
   myIsInitialized(message_in.myIsInitialized)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_MessageBase::RPG_Net_MessageBase"));

}

template <typename ProtocolCommandType>
RPG_Net_MessageBase<ProtocolCommandType>::RPG_Net_MessageBase(ACE_Data_Block* dataBlock_in,
                                                              ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,         // use (don't own !) this data block
             messageAllocator_in), // use this when destruction is imminent...
   myIsInitialized(true)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_MessageBase::RPG_Net_MessageBase"));

}

template <typename ProtocolCommandType>
RPG_Net_MessageBase<ProtocolCommandType>::~RPG_Net_MessageBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_MessageBase::~RPG_Net_MessageBase"));

  // *NOTE*: will be called just BEFORE this is passed back to the allocator

  // clean up
  myIsInitialized = false;
}

template <typename ProtocolCommandType>
void
RPG_Net_MessageBase<ProtocolCommandType>::init(ACE_Data_Block* dataBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_MessageBase::init"));

  // sanity check: shouldn't be initialized...
  ACE_ASSERT(!myIsInitialized);

  // init base class...
  inherited::init(dataBlock_in);

  myIsInitialized = true;
}

template <typename ProtocolCommandType>
void
RPG_Net_MessageBase<ProtocolCommandType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_MessageBase::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("***** Message (ID: %u) *****\n"),
             getID()));
}

template <typename ProtocolCommandType>
bool
RPG_Net_MessageBase<ProtocolCommandType>::crunchForHeader(const unsigned int& headerSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_MessageBase::crunchForHeader"));

  // sanity check(s)
  ACE_ASSERT(size() >= headerSize_in); // enough space ?
  if (total_length() < headerSize_in)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not enough data (needed: %u, had: %u), aborting\n"),
               headerSize_in,
               total_length()));

    return false;
  } // end IF
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
