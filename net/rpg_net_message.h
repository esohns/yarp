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

#ifndef RPG_NET_MESSAGE_H
#define RPG_NET_MESSAGE_H

#include <stream_message_base.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class ACE_Allocator;
class ACE_Message_Block;
class ACE_Data_Block;
class RPG_Net_SessionMessage;
// class RPG_Net_StreamMessageAllocator;
template <typename MessageType,
          typename SessionMessageType> class Stream_MessageAllocatorHeapBase;

class RPG_Net_Message
 : public Stream_MessageBase
{
  // enable access to specific private ctors...
//   friend class RPG_Net_StreamMessageAllocator;
  friend class Stream_MessageAllocatorHeapBase<RPG_Net_Message,
                                               RPG_Net_SessionMessage>;
 public:
  virtual ~RPG_Net_Message();

  // used for pre-allocated messages...
  virtual void init(// Stream_MessageBase members
                    ACE_Data_Block*); // data block to use

  virtual const int getCommand() const; // return value: (protocol) message type

  // implement RPG_Net_IDumpState
  virtual void dump_state() const;

  // *NOTE*: this "normalizes" data in this message (fragment) in the sense that
  // enough contiguous data is available to "comfortably" interpret a message
  // header of given size by simply using its struct declaration. To do this,
  // some data may need to be COPIED.
  // --> if necessary, the missing data is taken from the continuation(s),
  // adjusting read/write pointers as necessary
  const bool crunchForHeader(const unsigned long&); // header size

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate(void) const;

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  RPG_Net_Message(const RPG_Net_Message&);

 private:
  typedef Stream_MessageBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Message());
  // *NOTE*: to be used by allocators...
  RPG_Net_Message(ACE_Data_Block*, // data block to use
                  ACE_Allocator*); // message allocator
//   RPG_Net_Message(ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Message& operator=(const RPG_Net_Message&));

  // *NOTE*: pre-allocated messages may not have been initialized...
  bool myIsInitialized;
};

#endif
