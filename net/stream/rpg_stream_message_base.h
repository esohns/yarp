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

#ifndef RPG_STREAM_MESSAGE_BASE_H
#define RPG_STREAM_MESSAGE_BASE_H

#include "rpg_stream_exports.h"
#include "rpg_stream_messageallocatorheap.h"

#include "rpg_common_idumpstate.h"

#include <ace/Global_Macros.h>
#include <ace/Message_Block.h>
#include <ace/Atomic_Op.h>
#include <ace/Synch.h>

#include <string>

// forward declaratation(s)
class ACE_Allocator;

class RPG_Stream_Export RPG_Stream_MessageBase
 : public ACE_Message_Block,
   public RPG_Common_IDumpState
{
  // need access to specific ctors
  friend class RPG_Stream_MessageAllocatorHeap;
//   friend class RPG_Stream_MessageAllocatorHeapBase<arg1, arg2>;

 public:
  // message types
  enum MessageType
  {
    // *NOTE*: see <ace/Message_Block.h> for details...
    MB_BEGIN_STREAM_SESSION_MAP = ACE_Message_Block::MB_USER,
    // *** STREAM CONTROL ***
    MB_STREAM_SESSION,
    // *** STREAM CONTROL - END ***
    MB_BEGIN_STREAM_DATA_MAP = 0x300,
    // *** STREAM DATA ***
    MB_STREAM_DATA, // protocol data
    MB_STREAM_OBJ,  // OO type --> can be downcast dynamically
    // *** STREAM DATA - END ***
    MB_BEGIN_PROTOCOL_MAP = 0x400
  };

  virtual ~RPG_Stream_MessageBase();

  // info
  unsigned int getID() const;
  virtual int getCommand() const; // return value: (protocol) message type

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

  // reset atomic id generator
  // *WARNING*: this NEEDS to be static, otherwise this clashes with inherited::reset() !!!
  static void resetMessageIDGenerator();

  // helper methods
  static void MessageType2String(const ACE_Message_Type&, // as returned by msg_type()
                                 std::string&);           // return value: type string

 protected:
//   // ctor(s) for MB_STREAM_DATA
//   RPG_Stream_MessageBase(const unsigned long&); // total size of message data (off-wire)
   // copy ctor, to be used by derivates
   RPG_Stream_MessageBase(const RPG_Stream_MessageBase&);

  // *NOTE*: to be used by message allocators...
  RPG_Stream_MessageBase(ACE_Data_Block*, // data block
                     ACE_Allocator*, // message allocator
                     const bool& = true); // increment running message counter ?
  RPG_Stream_MessageBase(ACE_Allocator*,       // message allocator
                     const bool& = true); // increment running message counter ?

  // used for pre-allocated messages...
  void init(ACE_Data_Block*          // data block to use
            /*const ACE_Time_Value&*/); // scheduled execution time

 private:
  typedef ACE_Message_Block inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_MessageBase& operator=(const RPG_Stream_MessageBase&));

  // overrides from ACE_Message_Block
  // *WARNING*: most probably, any children need to override this as well !
  virtual ACE_Message_Block* duplicate(void) const;

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int> myCurrentID;

  unsigned int myMessageID;
};

// convenient types
typedef RPG_Stream_MessageBase::MessageType RPG_Stream_MessageType;

#endif
