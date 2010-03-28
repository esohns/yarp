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

#ifndef STREAM_MESSAGE_H
#define STREAM_MESSAGE_H

#include "stream_message_base.h"

#include <ace/Time_Value.h>
#include <ace/Message_Block.h>

#include <map>

// forward declaration(s)
class ACE_Allocator;
class Stream_MessageHeaderBase;

// *IMPORTANT NOTE*: avoid circular dependency...
class Stream_Cached_NetworkMessage_Allocator;
class Stream_Dynamic_Stream_Message_Allocator;
class Stream_Cached_FixedSize_Allocator;

class Stream_Message
 : public Stream_MessageBase
{
  // we need to enable access to a specific private ctor...
  friend class Stream_Cached_NetworkMessage_Allocator;
  friend class Stream_Dynamic_Stream_Message_Allocator;
  friend class Stream_Cached_FixedSize_Allocator;

  // enable access to the pcap_pkthdr...
  friend class Stream_Module_LibPcap_FileWriter;

 public:
  Stream_Message();
  virtual ~Stream_Message();

  // used for pre-allocated messages...
  void init(// Stream_MessageBase members
            ACE_Data_Block*); // data block to use

  // header/data info
  // *IMPORTANT NOTE*: this adjusts the rd_ptr of the message to point at the beginning of the corresponding
  // message "body" (which MAY be the next header in the protocol stack...)
  void addHeader(const Stream_MessageHeader_Type&); // header type

  const bool getHeaderOffset(const Stream_MessageHeader_Type&, // header type
                             unsigned long&) const;            // return value: offset
  const bool getTransportLayerTypeAndOffset(Stream_MessageHeader_Type&, // return value: transport layer type
                                            unsigned long&) const;      // return value: transport layer offset
  const bool hasProtocol(const Stream_Protocol_Type&) const; // protocol type
  const Stream_Protocol_Type getToplevelProtocol() const; // return value: top-level (supported) protocol

  // info
  const bool hasTransportLayerHeader() const;

  // implement Stream_IDumpState
  virtual void dump_state() const;

  // overloaded from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *IMPORTANT NOTE*: this uses our allocator (if any) to create a new message: as a side effect, this MAY
  // therefore increment the running message counter --> we signal the allocator we do NOT WANT THIS !
  // *TODO*: clean this up !
  virtual ACE_Message_Block* duplicate(void) const;

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> will result in MB_OBJ-type (!) message using an incremented refcount on the same datablock ("shallow copy") !
  Stream_Message(const Stream_Message&);

 private:
  typedef Stream_MessageBase inherited;

  // list of headers
  typedef std::map<Stream_MessageHeader_Type,
                   unsigned long> HEADERCONTAINER_TYPE;
  typedef std::pair<Stream_MessageHeader_Type,
                    unsigned long> HEADERCONTAINERPAIR_TYPE;
  typedef HEADERCONTAINER_TYPE::const_iterator HEADERCONTAINER_CONSTITERATOR_TYPE;
  typedef HEADERCONTAINER_TYPE::iterator HEADERCONTAINER_ITERATOR_TYPE;

  // safety measures
//   ACE_UNIMPLEMENTED_FUNC(Stream_Message());
  ACE_UNIMPLEMENTED_FUNC(Stream_Message& operator=(const Stream_Message&));

  // *IMPORTANT NOTE*: this is used by allocators during init...
  Stream_Message(ACE_Data_Block*, // data block to use
                 ACE_Allocator*); // message allocator
  Stream_Message(ACE_Allocator*,      // message allocator
                 const bool& = true); // increment running message counter ?

  // helper methods
  void adjustDataOffset(const Stream_MessageHeader_Type&); // header type

  HEADERCONTAINER_TYPE myHeaders;

  // *IMPORTANT NOTE*: pre-allocated messages may not have been initialized...
  bool                 myIsInitialized;
};

#endif
