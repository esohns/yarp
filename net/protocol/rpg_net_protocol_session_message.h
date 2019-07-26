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

#ifndef RPG_NET_PROTOCOL_SESSION_MESSAGE_H
#define RPG_NET_PROTOCOL_SESSION_MESSAGE_H

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"

#include "stream_common.h"
#include "stream_cachedmessageallocatorheap_base.h"
#include "stream_configuration.h"
#include "stream_session_message_base.h"
#include "stream_messageallocatorheap_base.h"

#include "rpg_net_protocol_stream_common.h"

// forward declaratation(s)
class ACE_Allocator;
class RPG_Net_Protocol_Message;

class RPG_Net_Protocol_SessionMessage
 : public Stream_SessionMessageBase_T<struct Stream_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      RPG_Net_Protocol_SessionData_t,
                                      struct Stream_UserData>
{
  typedef Stream_SessionMessageBase_T<struct Stream_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      RPG_Net_Protocol_SessionData_t,
                                      struct Stream_UserData> inherited;

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
  // *NOTE*: assumes responsibility for the second argument !
  // *TODO*: (using gcc) cannot pass reference to pointer for some reason
  RPG_Net_Protocol_SessionMessage (Stream_SessionId_t,
                                   enum Stream_SessionMessageType,
                                   RPG_Net_Protocol_SessionData_t*&, // session data container handle
                                   struct Stream_UserData*);
  inline virtual ~RPG_Net_Protocol_SessionMessage () {}

  // overloaded from ACE_Message_Block
  virtual ACE_Message_Block* duplicate (void) const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_SessionMessage ())
  // copy ctor to be used by duplicate()
  RPG_Net_Protocol_SessionMessage (const RPG_Net_Protocol_SessionMessage&);
  // *NOTE*: these may be used by message allocator(s)
  // *WARNING*: these ctors are NOT threadsafe
  RPG_Net_Protocol_SessionMessage (Stream_SessionId_t,
                                   ACE_Allocator*); // message allocator
  RPG_Net_Protocol_SessionMessage (Stream_SessionId_t,
                                   ACE_Data_Block*, // data block to use
                                   ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_SessionMessage& operator= (const RPG_Net_Protocol_SessionMessage&))
};

#endif
