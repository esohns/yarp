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

#include <rpg_common_macros.h>

template <typename MessageType,
          typename SessionMessageType>
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::RPG_Stream_CachedMessageAllocatorHeapBase(const unsigned long& maxNumMessages_in,
                                                                                                         ACE_Allocator* allocator_in)
 : //inherited(),
   myMessageAllocator(maxNumMessages_in),
   mySessionMessageAllocator(maxNumMessages_in),
   myDataBlockAllocator(maxNumMessages_in,
                        allocator_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::RPG_Stream_CachedMessageAllocatorHeapBase"));

}

template <typename MessageType,
          typename SessionMessageType>
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::~RPG_Stream_CachedMessageAllocatorHeapBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::~RPG_Stream_CachedMessageAllocatorHeapBase"));

}

template <typename MessageType,
          typename SessionMessageType>
void*
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::malloc(size_t bytes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::malloc"));

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
    if (bytes_in)
      ACE_NEW_MALLOC_NORETURN(message,
                              static_cast<MessageType*>(myMessageAllocator.malloc(sizeof(MessageType))),
                              MessageType(data_block,            // use the data block we just allocated
                                          &myMessageAllocator)); // remember allocator upon destruction...
    else
      ACE_NEW_MALLOC_NORETURN(message,
                              static_cast<SessionMessageType*>(mySessionMessageAllocator.malloc(sizeof(SessionMessageType))),
                              SessionMessageType(data_block,                   // use the data block we just allocated
                                                 &mySessionMessageAllocator)); // remember allocator upon destruction...
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in ACE_NEW_MALLOC_NORETURN([Session]MessageType(%u), aborting\n"),
               bytes_in));

    // clean up
    data_block->release();

    // *TODO*: what else can we do ?
    return NULL;
  }
  if (!message)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("unable to allocate [Session]MessageType(%u), aborting\n"),
               bytes_in));

    // clean up
    data_block->release();

    // *TODO*: what else can we do ?
    return NULL;
  } // end IF

  // ... and return the result
  // *NOTE*: the caller knows what to expect (either MessageType || SessionMessageType)
  return message;
}

template <typename MessageType,
          typename SessionMessageType>
void*
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::calloc(size_t bytes_in,
                                                                      char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::calloc"));

  // ignore this
  ACE_UNUSED_ARG(initialValue_in);

  // just delegate this...
  return malloc(bytes_in);
}

template <typename MessageType,
          typename SessionMessageType>
void
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::free(void* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::free"));

  // *NOTE*: messages should contact their individual allocators !!!
  ACE_ASSERT(false);
}

template <typename MessageType,
          typename SessionMessageType>
size_t
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::cache_depth"));

  return myDataBlockAllocator.cache_depth();
}

template <typename MessageType,
          typename SessionMessageType>
size_t
RPG_Stream_CachedMessageAllocatorHeapBase<MessageType,
                                          SessionMessageType>::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedMessageAllocatorHeapBase::cache_size"));

  return myDataBlockAllocator.cache_size();
}
