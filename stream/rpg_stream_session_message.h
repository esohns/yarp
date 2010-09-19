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

#ifndef RPG_STREAM_SESSION_MESSAGE_H
#define RPG_STREAM_SESSION_MESSAGE_H

#include "rpg_stream_idumpstate.h"
#include "rpg_stream_messageallocatorheap.h"
#include "rpg_stream_message_base.h"

#include <ace/Global_Macros.h>
#include <ace/Message_Block.h>

#include <string>

// forward declaratation(s)
class RPG_Stream_SessionConfig;
class ACE_Allocator;

class RPG_Stream_SessionMessage
 : public ACE_Message_Block,
   public RPG_Stream_IDumpState
{
  // need access to specific ctors
  friend class RPG_Stream_MessageAllocatorHeap;
//   friend class RPG_Stream_MessageAllocatorHeapBase<arg1, arg2>;

 public:
  enum SessionMessageType
  {
    // *NOTE*: see <stream_message_base.h> for details...
    MB_BEGIN_STREAM_SESSION_MAP = RPG_Stream_MessageBase::MB_STREAM_SESSION,
    // *** STREAM CONTROL ***
    MB_STREAM_SESSION_BEGIN,
    MB_STREAM_SESSION_STEP,
    MB_STREAM_SESSION_END,
    MB_STREAM_SESSION_STATISTICS
    // *** STREAM CONTROL - END ***
  };

  // *NOTE*: assume lifetime responsibility for the second argument !
  RPG_Stream_SessionMessage(const unsigned long&,      // session ID
                        const SessionMessageType&, // session message type
                        RPG_Stream_SessionConfig*&);   // config handle
  virtual ~RPG_Stream_SessionMessage();

  // initialization-after-construction
  // *NOTE*: assume lifetime responsibility for the second argument !
  void init(const unsigned long&,      // session ID
            const SessionMessageType&, // session message type
            RPG_Stream_SessionConfig*&);   // config handle

  // info
  const unsigned long getID() const;
  const SessionMessageType getType() const;
  // *TODO*: clean this up !
  const RPG_Stream_SessionConfig* const getConfig() const;

  // implement RPG_Stream_IDumpState
  virtual void dump_state() const;

  // overloaded from ACE_Message_Block
  virtual ACE_Message_Block* duplicate(void) const;

  // debug tools
  static void SessionMessageType2String(const SessionMessageType, // message type
                                        std::string&);            // corresp. string

 private:
  typedef ACE_Message_Block inherited;

  // copy ctor to be used by duplicate()
  RPG_Stream_SessionMessage(const RPG_Stream_SessionMessage&);

  // *NOTE*: these may be used by message allocators...
  // *WARNING*: these ctors are NOT threadsafe...
  RPG_Stream_SessionMessage(ACE_Allocator*); // message allocator
  RPG_Stream_SessionMessage(ACE_Data_Block*, // data block
                        ACE_Allocator*); // message allocator

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionMessage());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionMessage& operator=(const RPG_Stream_SessionMessage&));

  unsigned long         myID;
  SessionMessageType    myMessageType;
  RPG_Stream_SessionConfig* myConfig;
  bool                  myIsInitialized;
};

// convenient types
typedef RPG_Stream_SessionMessage::SessionMessageType RPG_Stream_SessionMessageType;

#endif
