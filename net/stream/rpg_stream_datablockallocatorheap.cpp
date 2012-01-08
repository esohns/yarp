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

#include "rpg_stream_datablockallocatorheap.h"

#include "rpg_stream_allocatorheap.h"

#include <rpg_common_macros.h>

#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>

// init statics
RPG_Stream_DataBlockAllocatorHeap::DATABLOCK_LOCK_TYPE RPG_Stream_DataBlockAllocatorHeap::myReferenceCountLock;

RPG_Stream_DataBlockAllocatorHeap::RPG_Stream_DataBlockAllocatorHeap(RPG_Stream_AllocatorHeap* allocator_in)
 : //inherited(),
   myPoolSize(0),
   myHeapAllocator(allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::RPG_Stream_DataBlockAllocatorHeap"));

  // *NOTE*: NULL --> use heap (== default allocator !)
  if (!myHeapAllocator)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("using default (== heap) message buffer allocation strategy...\n")));
  } // end IF
}

RPG_Stream_DataBlockAllocatorHeap::~RPG_Stream_DataBlockAllocatorHeap()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::~RPG_Stream_DataBlockAllocatorHeap"));

}

void*
RPG_Stream_DataBlockAllocatorHeap::malloc(size_t bytes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::malloc"));

  ACE_Data_Block* data_block = NULL;
  try
  {
    // - delegate allocation to our base class and
    // - perform a placement new by invoking a ctor on the allocated space
    // --> perform necessary initialization...
    ACE_NEW_MALLOC_NORETURN(data_block,
                            static_cast<ACE_Data_Block*> (inherited::malloc(sizeof(ACE_Data_Block))),
                            ACE_Data_Block(bytes_in,                                 // size of data chunk
                                           // *TODO*: make this configurable ?
                                           (bytes_in ? ACE_Message_Block::MB_DATA    // message type
                                                     : ACE_Message_Block::MB_PROTO),
                                           NULL,                                     // data --> use allocator !
                                           myHeapAllocator,                          // allocator
                                           //NULL,                                   // no allocator --> allocate this off the heap !
                                           &RPG_Stream_DataBlockAllocatorHeap::myReferenceCountLock, // reference count lock
                                           0,                                        // flags: release our (heap) memory when we die
                                           this));                                   // remember us upon destruction...
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_NEW_MALLOC_NORETURN(ACE_Data_Block(%u)), aborting\n"),
               bytes_in));

    // *TODO*: what else can we do ?
    return NULL;
  }
  if (!data_block)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("unable to allocate ACE_Data_Block(%u), aborting\n"),
               bytes_in));

    // *TODO*: what else can we do ?
    return NULL;
  } // end IF

  // increment running counter...
//   myPoolSize += data_block->capacity();
  myPoolSize++;

  return data_block;
}

void*
RPG_Stream_DataBlockAllocatorHeap::calloc(size_t bytes_in,
                                      char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::calloc"));

  // ignore this
  ACE_UNUSED_ARG(initialValue_in);

  // just delegate this (for now)...
  return malloc(bytes_in);
}

void
RPG_Stream_DataBlockAllocatorHeap::free(void* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::free"));

  // *NOTE*: handle_in SHOULD really be a ACE_Data_Block*...
//   ACE_Data_Block* data_block = NULL;
//   data_block = static_cast<ACE_Data_Block*> (//                                handle_in);
//   ACE_ASSERT(data_block);
//
//   // update allocation counter
//   myPoolSize -= data_block->capacity();
  myPoolSize--;

  inherited::free(handle_in);
}

size_t
RPG_Stream_DataBlockAllocatorHeap::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::cache_depth"));

  return myPoolSize.value();
}

size_t
RPG_Stream_DataBlockAllocatorHeap::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::cache_size"));

  if (myHeapAllocator)
    return myHeapAllocator->cache_size();

  // *TODO*: what should we do ?
  return -1;
}

void
RPG_Stream_DataBlockAllocatorHeap::dump(void) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::dump"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("# ACE_Data_Blocks online: %u\n"),
             myPoolSize.value()));
}

void*
RPG_Stream_DataBlockAllocatorHeap::calloc(size_t numElements_in,
                                      size_t sizePerElement_in,
                                      char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::calloc"));

  // ignore these
  ACE_UNUSED_ARG(numElements_in);
  ACE_UNUSED_ARG(sizePerElement_in);
  ACE_UNUSED_ARG(initialValue_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(NULL);
}

int
RPG_Stream_DataBlockAllocatorHeap::remove(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::remove"));

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::bind(const char* name_in,
                                    void* pointer_in,
                                    int duplicates_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::bind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);
  ACE_UNUSED_ARG(duplicates_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::trybind(const char* name_in,
                                       void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::trybind"));

    // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::find(const char* name_in,
                                    void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::find"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::find(const char* name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::find"));

  // ignore these
  ACE_UNUSED_ARG(name_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::unbind(const char* name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::unbind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::unbind(const char* name_in,
                                      void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::unbind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::sync(ssize_t length_in,
                                    int flags_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::sync"));

  // ignore these
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(flags_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::sync(void* address_in,
                                    size_t length_in,
                                    int flags_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::sync"));

  // ignore these
  ACE_UNUSED_ARG(address_in);
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(flags_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::protect(ssize_t length_in,
                                       int protection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::protect"));

  // ignore these
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(protection_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_DataBlockAllocatorHeap::protect(void* address_in,
                                       size_t length_in,
                                       int protection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataBlockAllocatorHeap::protect"));

  // ignore these
  ACE_UNUSED_ARG(address_in);
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(protection_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}
