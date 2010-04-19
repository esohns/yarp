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

#ifndef RPG_NET_PROTOCOL_MESSAGE_H
#define RPG_NET_PROTOCOL_MESSAGE_H

#include "rpg_net_protocol_common.h"

#include <stream_data_message_base.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class ACE_Allocator;
class ACE_Message_Block;
class ACE_Data_Block;
// *NOTE*: this avoids a circular dependency...
class RPG_Net_SessionMessage;
// class RPG_Net_StreamMessageAllocator;
template <typename MessageType,
          typename SessionMessageType> class Stream_MessageAllocatorHeapBase;

class RPG_Net_Protocol_Message
 : public Stream_DataMessageBase<RPG_Net_Protocol_IRCMessage>
{
  // enable access to specific private ctors...
//   friend class RPG_Net_StreamMessageAllocator;
  friend class Stream_MessageAllocatorHeapBase<RPG_Net_Protocol_Message,
                                               RPG_Net_SessionMessage>;

 public:
//   RPG_Net_Protocol_Message();
  virtual ~RPG_Net_Protocol_Message();

  // used for pre-allocated messages...
  virtual void init(// Stream_DataMessageBase members
                    RPG_Net_Protocol_IRCMessage*&, // data handle
                    // ACE_Message_Block members
                    ACE_Data_Block*);              // data block to use

  // implement RPG_Net_IDumpState
  virtual void dump_state() const;

  // "normalize" the data in this message (fragment) by:
  // - aligning the rd_ptr with base() --> ACE_Message_Block::crunch()/::memmove()
  // - COPYING all bits from any continuation(s) into our buffer (until
  //   capacity() has been reached)
  // - adjusting the write pointer accordingly
  // - releasing obsoleted continuations
  // --> *NOTE*: IF this is done CONSISTENTLY, AND:
  // - our buffer has capacity for a FULL message (i.e. maximum allowed size)
  // - our peer keeps to the standard and doesn't send oversized messages (!)
  // --> THEN this measure ensures that EVERY single buffer contains a CONTIGUOUS
  //     and COMPLETE message...
  // *WARNING*: if we share buffers, this may well clobber data referenced by
  // preceding messages THAT MAY STILL BE IN USE DOWNSTREAM
  // --> safe only IFF stream processing is single-threaded !
  void crunch();

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate(void) const;

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  RPG_Net_Protocol_Message(const RPG_Net_Protocol_Message&);

 private:
  typedef Stream_DataMessageBase<RPG_Net_Protocol_IRCMessage> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Message());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Message& operator=(const RPG_Net_Protocol_Message&));

  // *NOTE*: this is used by allocators during init...
  RPG_Net_Protocol_Message(ACE_Data_Block*, // data block to use
                           ACE_Allocator*); // message allocator
//   RPG_Net_Protocol_Message(ACE_Allocator*); // message allocator

  // *NOTE*: pre-allocated messages start un-initialized...
  bool myIsInitialized;
};

#endif
