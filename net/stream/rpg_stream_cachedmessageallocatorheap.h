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

#ifndef RPG_STREAM_CACHEDMESSAGEALLOCATORHEAP_H
#define RPG_STREAM_CACHEDMESSAGEALLOCATORHEAP_H

#include "rpg_stream_iallocator.h"
#include "rpg_stream_cacheddatablockallocatorheap.h"

#include <ace/Malloc_T.h>
#include <ace/Message_Block.h>
#include <ace/Synch_Traits.h>

class RPG_Stream_CachedMessageAllocatorHeap
 : public ACE_Cached_Allocator<ACE_Message_Block, ACE_SYNCH_MUTEX>,
   public RPG_Stream_IAllocator
{
 public:
  RPG_Stream_CachedMessageAllocatorHeap(const unsigned long&, // total number of chunks
                                        ACE_Allocator*);      // (heap) memory allocator...
  virtual ~RPG_Stream_CachedMessageAllocatorHeap();

  // overload these to do what we want
  // *NOTE*: returns a pointer to ACE_Message_Block...
  virtual void* malloc(size_t); // bytes

  // *NOTE*: returns a pointer to RPG_Stream_MessageBase...
  virtual void* calloc(size_t,       // bytes
                       char = '\0'); // initial value

  // *NOTE*: frees an ACE_Message_Block...
  virtual void free(void*); // element handle

  // *NOTE*: these return the # of online ACE_Message_Blocks...
  virtual size_t cache_depth() const;
  virtual size_t cache_size() const;

 private:
  typedef ACE_Cached_Allocator<ACE_Message_Block, ACE_SYNCH_MUTEX> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_CachedMessageAllocatorHeap(const RPG_Stream_CachedMessageAllocatorHeap&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_CachedMessageAllocatorHeap& operator=(const RPG_Stream_CachedMessageAllocatorHeap&));

  // data block allocator
  RPG_Stream_CachedDataBlockAllocatorHeap myDataBlockAllocator;
};

#endif
