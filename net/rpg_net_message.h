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

#include "rpg_net_remote_comm.h"

#include "rpg_net_message_base.h"

#include <string>

// forward declaration(s)
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
class RPG_Net_SessionMessage;
// class RPG_Net_StreamMessageAllocator;
template <typename MessageType,
          typename SessionMessageType> class RPG_Stream_MessageAllocatorHeapBase;

class RPG_Net_Message
 : public RPG_Net_MessageBase<RPG_Net_MessageType>
{
  // enable access to specific private ctors...
  //   friend class RPG_Net_StreamMessageAllocator;
  friend class RPG_Stream_MessageAllocatorHeapBase<RPG_Net_Message,
                                                   RPG_Net_SessionMessage>;

 public:
  virtual ~RPG_Net_Message();

  virtual RPG_Net_MessageType getCommand() const; // return value: message type

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate(void) const;

  // implement RPG_Net_IDumpState
  virtual void dump_state() const;

  static std::string commandType2String(const RPG_Net_MessageType&);

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  RPG_Net_Message(const RPG_Net_Message&);

 private:
  typedef RPG_Net_MessageBase<RPG_Net_MessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Message());
  // *NOTE*: to be used by allocators...
  RPG_Net_Message(ACE_Data_Block*, // data block to use
                  ACE_Allocator*); // message allocator
//   RPG_Net_Message(ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Message& operator=(const RPG_Net_Message&));
};

#endif
