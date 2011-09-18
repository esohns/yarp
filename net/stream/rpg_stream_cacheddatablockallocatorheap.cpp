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

#include "rpg_stream_cacheddatablockallocatorheap.h"

#include <rpg_common_macros.h>

// init statics
RPG_Stream_CachedDataBlockAllocatorHeap::DATABLOCK_LOCK_TYPE RPG_Stream_CachedDataBlockAllocatorHeap::myReferenceCountLock;

RPG_Stream_CachedDataBlockAllocatorHeap::RPG_Stream_CachedDataBlockAllocatorHeap(const unsigned long& chunks_in,
                                                                                 ACE_Allocator* allocator_in)
 : inherited(chunks_in),
   myHeapAllocator(allocator_in),
   myPoolSize(chunks_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::RPG_Stream_CachedDataBlockAllocatorHeap"));

  // *NOTE*: NULL --> use heap (== default allocator !)
  if (!myHeapAllocator)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("using default (== heap) message buffer allocation strategy...\n")));
  } // end IF
}

RPG_Stream_CachedDataBlockAllocatorHeap::~RPG_Stream_CachedDataBlockAllocatorHeap()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::~RPG_Stream_CachedDataBlockAllocatorHeap"));

}

void*
RPG_Stream_CachedDataBlockAllocatorHeap::malloc(size_t bytes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::malloc"));

  ACE_Data_Block* data_block = NULL;
  try
  {
    // - delegate allocation to our base class and
    // - perform a placement new by invoking a ctor on the allocated space
    // --> perform necessary initialization...
    ACE_NEW_MALLOC_RETURN(data_block,
                          static_cast<ACE_Data_Block*>(inherited::malloc(sizeof(ACE_Data_Block))),
                          ACE_Data_Block(bytes_in,                                 // size of data chunk
                                         ACE_Message_Block::MB_DATA,               // message type
                                         NULL,                                     // data --> use allocator !
                                         myHeapAllocator,                          // allocator
                                         //NULL,                                   // no allocator --> allocate this off the heap !
                                         &RPG_Stream_CachedDataBlockAllocatorHeap::myReferenceCountLock, // reference count lock
                                         0,                                        // flags: release our (heap) memory when we die
                                         this),                                    // remember us upon destruction...
                          NULL);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_NEW_MALLOC_RETURN(ACE_Data_Block(%u)), aborting\n"),
               bytes_in));

    // *TODO*: what else can we do ?
    return NULL;
  }

  return data_block;
}

void*
RPG_Stream_CachedDataBlockAllocatorHeap::calloc(size_t bytes_in,
                                                char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::calloc"));

  // ignore this
  ACE_UNUSED_ARG(initialValue_in);

  // just delegate this (for now)...
  return malloc(bytes_in);
}

void
RPG_Stream_CachedDataBlockAllocatorHeap::free(void* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::free"));

  inherited::free(handle_in);
}

size_t
RPG_Stream_CachedDataBlockAllocatorHeap::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::cache_depth"));

  return const_cast<RPG_Stream_CachedDataBlockAllocatorHeap*>(this)->pool_depth();
}

size_t
RPG_Stream_CachedDataBlockAllocatorHeap::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedDataBlockAllocatorHeap::cache_size"));

  return myPoolSize;
}
