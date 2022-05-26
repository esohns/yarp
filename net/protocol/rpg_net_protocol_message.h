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

#include "ace/Global_Macros.h"

#include "stream_cachedmessageallocatorheap_base.h"
#include "stream_control_message.h"
#include "stream_data_message_base.h"
#include "stream_messageallocatorheap_base.h"

#include "rpg_net_protocol_common.h"

// forward declaration(s)
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
class RPG_Net_Protocol_SessionMessage;

class RPG_Net_Protocol_Message
 : public Stream_MessageBase_T<struct Stream_AllocatorConfiguration,
                               enum Stream_MessageType,
                               Stream_CommandType_t>
{
  typedef Stream_MessageBase_T<struct Stream_AllocatorConfiguration,
                               enum Stream_MessageType,
                               Stream_CommandType_t> inherited;

  // grant access to specific private ctors
  friend class Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                                 struct Stream_AllocatorConfiguration,
                                                 Stream_ControlMessage_t,
                                                 RPG_Net_Protocol_Message,
                                                 RPG_Net_Protocol_SessionMessage>;
  friend class Stream_CachedMessageAllocatorHeapBase_T<Stream_ControlMessage_t,
                                                       RPG_Net_Protocol_Message,
                                                       RPG_Net_Protocol_SessionMessage>;

 public:
  RPG_Net_Protocol_Message (unsigned int); // size
  inline virtual ~RPG_Net_Protocol_Message () {}

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy of ourselves that references the same packet
  // *NOTE*: this uses our allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

  inline virtual Stream_CommandType_t command () const { return ACE_Message_Block::MB_DATA; }
  static std::string CommandTypeToString (Stream_CommandType_t);

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  RPG_Net_Protocol_Message (const RPG_Net_Protocol_Message&);

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Message ())
  // *NOTE*: to be used by message allocators
  RPG_Net_Protocol_Message (Stream_SessionId_t,
                            ACE_Data_Block*, // data block
                            ACE_Allocator*,  // message allocator
                            bool = true);    // increment running message counter ?
  RPG_Net_Protocol_Message (Stream_SessionId_t,
                            ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Message& operator= (const RPG_Net_Protocol_Message&))
};

#endif
