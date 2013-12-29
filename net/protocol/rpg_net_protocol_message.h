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

#include "rpg_net_protocol_IRCmessage.h"

#include "rpg_stream_data_message_base.h"

#include <ace/Global_Macros.h>

// forward declaration(s)
class ACE_Allocator;
class ACE_Message_Block;
class ACE_Data_Block;

class RPG_Net_Protocol_Message
 : public RPG_Stream_DataMessageBase<RPG_Net_Protocol_IRCMessage>
{
 public:
  // *NOTE*: to be used by allocators...
  RPG_Net_Protocol_Message(ACE_Data_Block*, // data block to use
                           ACE_Allocator*); // message allocator
  //   RPG_Net_Protocol_Message(ACE_Allocator*); // message allocator
  virtual ~RPG_Net_Protocol_Message();

  virtual int getCommand() const; // return value: (protocol) message type

  // implement RPG_Net_IDumpState
  virtual void dump_state() const;

  // "normalize" the data in this message (fragment) by:
  // 1. aligning the rd_ptr with base() --> ACE_Message_Block::crunch()/::memmove()
  // *WARNING*: if we share buffers, this may well clobber data referenced by
  // preceding messages THAT MAY STILL BE IN USE DOWNSTREAM
  // --> safe only IFF stream processing is single-threaded !
  // --> still, we make a "best-effort", simply to reduce fragmentation...
  // 2. COPYING all bits from any continuation(s) into our buffer (until
  //    capacity() has been reached)
  // 3. adjusting the write pointer accordingly
  // 4. releasing obsoleted continuations
  // --> *NOTE*: IF this is done CONSISTENTLY, AND:
  // - our buffer has capacity for a FULL message (i.e. maximum allowed size)
  // - our peer keeps to the standard and doesn't send oversized messages (!)
  // --> THEN this measure ensures that EVERY single buffer contains a CONTIGUOUS
  //     and COMPLETE message...
  void crunch();

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate(void) const;

  static std::string commandType2String(const RPG_Net_Protocol_CommandType_t&);

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  RPG_Net_Protocol_Message(const RPG_Net_Protocol_Message&);

 private:
  typedef RPG_Stream_DataMessageBase<RPG_Net_Protocol_IRCMessage> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Message());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Message& operator=(const RPG_Net_Protocol_Message&));
};

#endif
