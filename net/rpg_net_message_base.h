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

#ifndef RPG_NET_MESSAGE_BASE_H
#define RPG_NET_MESSAGE_BASE_H

#include "ace/Global_Macros.h"

#include "stream_message_base.h"

#include "rpg_net_remote_comm.h"

// forward declaration(s)
class ACE_Data_Block;
class ACE_Allocator;

template <typename ProtocolCommandType>
class RPG_Net_MessageBase
 : public Stream_MessageBase
{
 public:
  virtual ~RPG_Net_MessageBase();

  // used for pre-allocated messages...
  virtual void init(// Stream_MessageBase members
                    ACE_Data_Block*); // data block to use

  virtual ProtocolCommandType getCommand() const = 0; // return value: message type

  // implement RPG_Net_IDumpState
  virtual void dump_state() const;

  // *NOTE*: this "normalizes" data in this message (fragment) in the sense that
  // enough contiguous data is available to "comfortably" interpret a message
  // header of given size by simply using its struct declaration. To do this,
  // some data may need to be COPIED.
  // --> if necessary, the missing data is taken from the continuation(s),
  // adjusting read/write pointers as necessary
  bool crunchForHeader(const unsigned int&); // header size

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  RPG_Net_MessageBase(const RPG_Net_MessageBase&);
  // *NOTE*: to be used by allocators...
  RPG_Net_MessageBase(ACE_Data_Block*, // data block to use
                      ACE_Allocator*); // message allocator

 private:
  typedef Stream_MessageBase inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_MessageBase());
//   RPG_Net_MessageBase(ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_MessageBase& operator=(const RPG_Net_MessageBase&));

  bool myIsInitialized;
};

#include "rpg_net_message_base.inl"

#endif
