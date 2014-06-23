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

#ifndef RPG_STREAM_CACHEDMESSAGEALLOCATORHEAP_BASE_H
#define RPG_STREAM_CACHEDMESSAGEALLOCATORHEAP_BASE_H

#include "rpg_stream_iallocator.h"
#include "rpg_stream_cacheddatablockallocatorheap.h"

#include <ace/Malloc_T.h>
#include <ace/Message_Block.h>
#include <ace/Synch.h>

template <typename MessageType,
          typename SessionMessageType>
class RPG_Stream_CachedMessageAllocatorHeapBase
 : public RPG_Stream_IAllocator
{
 public:
  RPG_Stream_CachedMessageAllocatorHeapBase(const unsigned long&, // total number of concurrent messages
                                            ACE_Allocator*);      // (heap) memory allocator...
  virtual ~RPG_Stream_CachedMessageAllocatorHeapBase();

  // overload these to do what we want
  // *NOTE*: returns a pointer to <MessageType>...
  // *NOTE: passing a value of 0 will return a <SessionMessageType>
  // *TODO*: the way message IDs are implemented, they can be truly unique
  // only IF allocation is synchronized...
  virtual void* malloc(size_t); // bytes

  // *NOTE*: returns a pointer to <MessageType>/<SessionMessageType>
  // --> see above
  virtual void* calloc(size_t,       // bytes
                       char = '\0'); // initial value

  // *NOTE*: frees an <MessageType>/<SessionMessageType>...
  virtual void free(void*); // element handle

  // *NOTE*: these return the # of online ACE_Data_Blocks...
  virtual size_t cache_depth() const;
  virtual size_t cache_size() const;

 private:
  typedef RPG_Stream_IAllocator inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_CachedMessageAllocatorHeapBase(const RPG_Stream_CachedMessageAllocatorHeapBase<MessageType, SessionMessageType>&));
  // *NOTE*: apparently, ACE_UNIMPLEMENTED_FUNC gets confused with more than one template parameter...
//   ACE_UNIMPLEMENTED_FUNC(RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
//                                                          SessionMessageType>& operator=(const RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
//                                                                                          SessionMessageType>&));

  // message allocator
  ACE_Cached_Allocator<MessageType,
                       ACE_SYNCH_MUTEX>   myMessageAllocator;
  ACE_Cached_Allocator<SessionMessageType,
                       ACE_SYNCH_MUTEX>   mySessionMessageAllocator;
  // data block allocator
  RPG_Stream_CachedDataBlockAllocatorHeap myDataBlockAllocator;
};

// include template implementation
#include "rpg_stream_cachedmessageallocatorheap_base.inl"

#endif
