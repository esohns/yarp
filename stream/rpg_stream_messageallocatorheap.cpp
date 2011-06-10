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

#include "rpg_stream_messageallocatorheap.h"

#include "rpg_stream_allocatorheap.h"
#include "rpg_stream_message_base.h"
#include "rpg_stream_session_message.h"

#include <rpg_common_macros.h>

RPG_Stream_MessageAllocatorHeap::RPG_Stream_MessageAllocatorHeap(const unsigned long& maxNumMessages_in,
                                                         RPG_Stream_AllocatorHeap* allocator_in)
 : //inherited(),
   myFreeMessageCounter(maxNumMessages_in,
                        NULL,
                        NULL,
                        maxNumMessages_in),
   myPoolSize(0),
   myDataBlockAllocator(allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::RPG_Stream_MessageAllocatorHeap"));

}

RPG_Stream_MessageAllocatorHeap::~RPG_Stream_MessageAllocatorHeap()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::~RPG_Stream_MessageAllocatorHeap"));

}

void*
RPG_Stream_MessageAllocatorHeap::malloc(size_t bytes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::malloc"));

  // step0: wait for an empty slot...
  // *NOTE*: we don't really ever want to actually block here...
  // *WARNING*: the fact that we do this outside of the lock scope
  // leads to a temporal inconsistency between the state of the counter
  // and our pool ! In order to keep lock scope as small as possible,
  // and maximizing parallelism, we'll live with this for now.
  myFreeMessageCounter.acquire();
  myPoolSize++;

  // step1: get free data block
  ACE_Data_Block* data_block = NULL;
  try
  {
    ACE_ALLOCATOR_NORETURN(data_block,
                           static_cast<ACE_Data_Block*> (myDataBlockAllocator.malloc(bytes_in)));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_ALLOCATOR_NORETURN(ACE_Data_Block(%u)), aborting\n"),
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

  // *NOTE*: must clean up data block beyond this point !

  // step2: get free message...
  ACE_Message_Block* message = NULL;
  try
  {
    // allocate memory and perform a placement new by invoking a ctor
    // on the allocated space
    if (bytes_in)
      ACE_NEW_MALLOC_NORETURN(message,
                              static_cast<RPG_Stream_MessageBase*> (inherited::malloc(sizeof(RPG_Stream_MessageBase))),
                              RPG_Stream_MessageBase(data_block, // use the data block we just allocated
                                                 this));     // remember us upon destruction...
    else
      ACE_NEW_MALLOC_NORETURN(message,
                              static_cast<RPG_Stream_SessionMessage*> (inherited::malloc(sizeof(RPG_Stream_SessionMessage))),
                              RPG_Stream_SessionMessage(data_block, // use the data block we just allocated
                                                    this));     // remember us upon destruction...
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_NEW_MALLOC_NORETURN(RPG_Stream_[Session]MessageBase(%u)), aborting\n"),
               bytes_in));

    // clean up
    data_block->release();

    // *TODO*: what else can we do ?
    return NULL;
  }
  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("unable to allocate RPG_Stream_[Session]MessageBase(%u), aborting\n"),
               bytes_in));

    // clean up
    data_block->release();

    // *TODO*: what else can we do ?
    return NULL;
  } // end IF

  // *NOTE*: now we do this directly, via the ctor !
//   // step3: attach data block to message...
//   message->data_block(data_block);

  // ... and return the result
  // *NOTE*: the caller knows what to expect (either RPG_Stream_MessageBase || RPG_Stream_SessionMessage)
  return message;
}

void*
RPG_Stream_MessageAllocatorHeap::calloc(size_t bytes_in,
                                    char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::calloc"));

  // ignore this
  ACE_UNUSED_ARG(initialValue_in);

  // just delegate this...
  return malloc(bytes_in);
}

void
RPG_Stream_MessageAllocatorHeap::free(void* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::free"));

  // just delegate to base class...
  inherited::free(handle_in);

  // OK: one slot just emptied...
  myPoolSize--;
  myFreeMessageCounter.release();
}

size_t
RPG_Stream_MessageAllocatorHeap::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::cache_depth"));

  return myPoolSize.value();
}

size_t
RPG_Stream_MessageAllocatorHeap::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::cache_size"));

  return myDataBlockAllocator.cache_size();
}

void
RPG_Stream_MessageAllocatorHeap::dump(void) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::dump"));

  return myDataBlockAllocator.dump();
}

void*
RPG_Stream_MessageAllocatorHeap::calloc(size_t numElements_in,
                                    size_t sizePerElement_in,
                                    char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::calloc"));

  // ignore these
  ACE_UNUSED_ARG(numElements_in);
  ACE_UNUSED_ARG(sizePerElement_in);
  ACE_UNUSED_ARG(initialValue_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(NULL);
}

int
RPG_Stream_MessageAllocatorHeap::remove(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::remove"));

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::bind(const char* name_in,
                                  void* pointer_in,
                                  int duplicates_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::bind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);
  ACE_UNUSED_ARG(duplicates_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::trybind(const char* name_in,
                                     void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::trybind"));

    // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::find(const char* name_in,
                                  void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::find"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::find(const char* name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::find"));

  // ignore these
  ACE_UNUSED_ARG(name_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::unbind(const char* name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::unbind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::unbind(const char* name_in,
                                    void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::unbind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::sync(ssize_t length_in,
                                  int flags_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::sync"));

  // ignore these
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(flags_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::sync(void* address_in,
                                  size_t length_in,
                                  int flags_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::sync"));

  // ignore these
  ACE_UNUSED_ARG(address_in);
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(flags_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::protect(ssize_t length_in,
                                     int protection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::protect"));

  // ignore these
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(protection_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_MessageAllocatorHeap::protect(void* address_in,
                                     size_t length_in,
                                     int protection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageAllocatorHeap::protect"));

  // ignore these
  ACE_UNUSED_ARG(address_in);
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(protection_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}
