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
#include "stdafx.h"

#include "rpg_stream_cachedmessageallocatorheap.h"

#include <rpg_common_macros.h>

RPG_Stream_CachedMessageAllocatorHeap::RPG_Stream_CachedMessageAllocatorHeap(const unsigned long& chunks_in,
                                                                             ACE_Allocator* allocator_in)
 : inherited(chunks_in),
   myDataBlockAllocator(chunks_in,
                        allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::RPG_Stream_CachedMessageAllocatorHeap"));

}

RPG_Stream_CachedMessageAllocatorHeap::~RPG_Stream_CachedMessageAllocatorHeap()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::~RPG_Stream_CachedMessageAllocatorHeap"));

}

void*
RPG_Stream_CachedMessageAllocatorHeap::malloc(size_t bytes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::malloc"));

  // step1: get free data block
  ACE_Data_Block* data_block = NULL;
  try
  {
    ACE_ALLOCATOR_RETURN(data_block,
                         static_cast<ACE_Data_Block*>(myDataBlockAllocator.malloc(bytes_in)),
                         NULL);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_ALLOCATOR_RETURN(%u), aborting\n"),
               bytes_in));

    // *TODO*: what else can we do ?
    return NULL;
  }

  // *NOTE*: must clean up data block beyond this point !

  // step2: get free message...
  ACE_Message_Block* message = NULL;
  try
  {
    // allocate memory and perform a placement new by invoking a ctor
    // on the allocated space
    ACE_NEW_MALLOC_NORETURN(message,
                            static_cast<ACE_Message_Block*>(inherited::malloc(sizeof(ACE_Message_Block))),
                            ACE_Message_Block(data_block, // reference the data block we just allocated
                                              0,          // flags: release our data block when we die
                                              this));     // remember us upon destruction...
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_NEW_MALLOC_NORETURN(ACE_Message_Block(%u)), aborting\n"),
               bytes_in));

    // clean up
    data_block->release();

    // *TODO*: what else can we do ?
    return NULL;
  }
  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("unable to allocate ACE_Message_Block(%u), aborting\n"),
               bytes_in));

    // clean up
    data_block->release();

    // *TODO*: what else can we do ?
    return NULL;
  } // end IF

  return message;
}

void*
RPG_Stream_CachedMessageAllocatorHeap::calloc(size_t bytes_in,
                                              char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::calloc"));

  // ignore this
  ACE_UNUSED_ARG(initialValue_in);

  // just delegate this...
  return malloc(bytes_in);
}

void
RPG_Stream_CachedMessageAllocatorHeap::free(void* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::free"));

  // just delegate to base class...
  inherited::free(handle_in);
}

size_t
RPG_Stream_CachedMessageAllocatorHeap::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::cache_depth"));

  return const_cast<RPG_Stream_CachedMessageAllocatorHeap*>(this)->pool_depth();
}

size_t
RPG_Stream_CachedMessageAllocatorHeap::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeap::cache_size"));

  return myDataBlockAllocator.cache_size();
}
