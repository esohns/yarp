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

#ifndef RPG_NET_SESSIONMESSAGE_H
#define RPG_NET_SESSIONMESSAGE_H

#include "rpg_net_stream_config.h"
// #include "rpg_net_message.h"

#include <rpg_stream_session_message_base.h>

#include <ace/Global_Macros.h>

// forward declarations
class ACE_Message_Block;
class ACE_Allocator;
// *NOTE*: this avoids a circular dependency...
class RPG_Net_Message;
// class RPG_Net_StreamMessageAllocator;
// template <typename MessageType, typename SessionMessageType> class Stream_MessageAllocatorHeapBase;

class RPG_Net_SessionMessage
 : public RPG_Stream_SessionMessageBase<RPG_Net_StreamConfig>
{
//   // enable access to private ctor(s)...
//   friend class RPG_Net_StreamMessageAllocator;
//   friend class Stream_MessageAllocatorHeapBase<RPG_Net_Message, RPG_Net_SessionMessage>;
 public:
  // *NOTE*: assume lifetime responsibility for the second argument !
  RPG_Net_SessionMessage(const unsigned long&,                 // session ID
                         const RPG_Stream_SessionMessageType&, // session message type
                         RPG_Net_StreamConfig*&);              // config handle
    // *NOTE*: to be used by message allocators...
  RPG_Net_SessionMessage(ACE_Allocator*); // message allocator
  RPG_Net_SessionMessage(ACE_Data_Block*, // data block
                         ACE_Allocator*); // message allocator
  virtual ~RPG_Net_SessionMessage();

  // override from ACE_Message_Block
  // *WARNING*: any children need to override this as well
  virtual ACE_Message_Block* duplicate(void) const;

 private:
  typedef RPG_Stream_SessionMessageBase<RPG_Net_StreamConfig> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SessionMessage());
  // copy ctor (to be used by duplicate())
  RPG_Net_SessionMessage(const RPG_Net_SessionMessage&);
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SessionMessage& operator=(const RPG_Net_SessionMessage&));
};

#endif
