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

#ifndef RPG_NET_PROTOCOL_SESSIONMESSAGE_H
#define RPG_NET_PROTOCOL_SESSIONMESSAGE_H

#include "ace/Global_Macros.h"

#include "stream_common.h"
#include "stream_session_message_base.h"

#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_stream_common.h"

// forward declarations
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;

class RPG_Protocol_Export RPG_Net_Protocol_SessionMessage
 : public Stream_SessionMessageBase_T<Stream_State_t,
                                      RPG_Net_Protocol_StreamSessionData_t>
{
//  // enable access to private ctor(s)...
//  friend class Net_StreamMessageAllocator;
//  friend class Stream_MessageAllocatorHeapBase<Net_Message, Net_SessionMessage>;

 public:
  // *NOTE*: assume lifetime responsibility for the second argument !
  RPG_Net_Protocol_SessionMessage (Stream_SessionMessageType_t,             // session message type
                                   Stream_State_t*,                         // stream state handle
                                   RPG_Net_Protocol_StreamSessionData_t*&); // session data handle
    // *NOTE*: to be used by message allocators...
  RPG_Net_Protocol_SessionMessage (ACE_Allocator*); // message allocator
  RPG_Net_Protocol_SessionMessage (ACE_Data_Block*, // data block
                                   ACE_Allocator*); // message allocator
  virtual ~RPG_Net_Protocol_SessionMessage ();

  // override from ACE_Message_Block
  // *WARNING*: any children need to override this as well
  virtual ACE_Message_Block* duplicate (void) const;

 private:
  typedef Stream_SessionMessageBase_T<Stream_State_t,
                                      RPG_Net_Protocol_StreamSessionData_t> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_SessionMessage ());
  // copy ctor (to be used by duplicate())
  RPG_Net_Protocol_SessionMessage (const RPG_Net_Protocol_SessionMessage&);
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_SessionMessage& operator= (const RPG_Net_Protocol_SessionMessage&));
};

#endif
