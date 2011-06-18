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

#ifndef RPG_STREAM_DATA_MESSAGE_BASE_H
#define RPG_STREAM_DATA_MESSAGE_BASE_H

#include "rpg_stream_message_base.h"

#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Message_Block.h>

// forward declarations
class ACE_Allocator;

template <typename DataType>
class RPG_Stream_DataMessageBase
 : public RPG_Stream_MessageBase,
   public RPG_Common_IDumpState
{
 public:
  virtual ~RPG_Stream_DataMessageBase();

  // initialization-after-construction
  // *NOTE*: assume lifetime responsibility for the first argument !
  void init(DataType*&,              // data handle
            ACE_Data_Block* = NULL); // buffer

  // *TODO*: clean this up !
  const DataType* const getData() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 protected:
  // *NOTE*: assume lifetime responsibility for the argument !
  // *WARNING*: this ctor doesn't allocate a buffer off the heap...
  RPG_Stream_DataMessageBase(DataType*&); // data handle
  // copy ctor, to be used by derived::duplicate()
  // *WARNING*: while the clone inherits a "shallow copy" of the referenced
  // data block, it will NOT inherit the attached data --> use init()...
  RPG_Stream_DataMessageBase(const RPG_Stream_DataMessageBase<DataType>&);

  // *NOTE*: to be used by message allocators...
  // *WARNING*: these ctors are NOT threadsafe...
  RPG_Stream_DataMessageBase(ACE_Allocator*); // message allocator
  RPG_Stream_DataMessageBase(ACE_Data_Block*, // data block
                         ACE_Allocator*); // message allocator

 private:
  typedef RPG_Stream_MessageBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_DataMessageBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_DataMessageBase<DataType>& operator=(const RPG_Stream_DataMessageBase<DataType>&));

  // overloaded from ACE_Message_Block
  // *WARNING*: any children need to override this too !
  virtual ACE_Message_Block* duplicate(void) const;

  DataType* myData;
  bool      myIsInitialized;
};

// include template implementation
#include "rpg_stream_data_message_base.i"

#endif
