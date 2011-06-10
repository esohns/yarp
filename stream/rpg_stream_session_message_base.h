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

#ifndef RPG_STREAM_SESSION_MESSAGE_BASE_H
#define RPG_STREAM_SESSION_MESSAGE_BASE_H

#include "rpg_stream_idumpstate.h"
#include "rpg_stream_session_message.h"

#include <ace/Global_Macros.h>
#include <ace/Message_Block.h>

#include <string>

// forward declarations
class ACE_Allocator;

template <typename ConfigType>
class RPG_Stream_SessionMessageBase
 : public ACE_Message_Block,
   public RPG_Stream_IDumpState
{
 public:
  // *NOTE*: assume lifetime responsibility for the second argument !
  RPG_Stream_SessionMessageBase(const unsigned long&,             // session ID
                            const RPG_Stream_SessionMessageType&, // session message type
                            ConfigType*&);                    // config handle
  virtual ~RPG_Stream_SessionMessageBase();

  // initialization-after-construction
  // *NOTE*: assume lifetime responsibility for the second argument !
  void init(const unsigned long&,             // session ID
            const RPG_Stream_SessionMessageType&, // session message type
            ConfigType*&);                    // config handle

  // info
  const unsigned long getID() const;
  const RPG_Stream_SessionMessageType getType() const;
  // *TODO*: clean this up !
  const ConfigType* const getConfig() const;

  // implement RPG_Stream_IDumpState
  virtual void dump_state() const;

 protected:
  // copy ctor to be used by duplicate()
  RPG_Stream_SessionMessageBase(const RPG_Stream_SessionMessageBase<ConfigType>&);

  // *NOTE*: these may be used by message allocators...
  // *WARNING*: these ctors are NOT threadsafe...
  RPG_Stream_SessionMessageBase(ACE_Allocator*); // message allocator
  RPG_Stream_SessionMessageBase(ACE_Data_Block*, // data block
                            ACE_Allocator*); // message allocator

  ConfigType*               myConfig;

 private:
  typedef ACE_Message_Block inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionMessageBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionMessageBase<ConfigType>& operator=(const RPG_Stream_SessionMessageBase<ConfigType>&));

  // overloaded from ACE_Message_Block
  // *WARNING*: any children need to override this too !
  virtual ACE_Message_Block* duplicate(void) const;

  unsigned long             myID;
  RPG_Stream_SessionMessageType myMessageType;
  bool                      myIsInitialized;
};

// include template implementation
#include "rpg_stream_session_message_base.i"

#endif
