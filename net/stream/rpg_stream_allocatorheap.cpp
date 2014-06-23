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

#include "rpg_stream_allocatorheap.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Stream_AllocatorHeap::RPG_Stream_AllocatorHeap()
 : //inherited(),
   myPoolSize(0)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::RPG_Stream_AllocatorHeap"));

}

RPG_Stream_AllocatorHeap::~RPG_Stream_AllocatorHeap()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::~RPG_Stream_AllocatorHeap"));

}

void*
RPG_Stream_AllocatorHeap::malloc(size_t bytes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::malloc"));

  // update allocation counter
  myPoolSize += bytes_in;

  // just delegate...
  return inherited::malloc(bytes_in);
}

void*
RPG_Stream_AllocatorHeap::calloc(size_t bytes_in,
                             char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::calloc"));

  // update allocation counter
  myPoolSize += bytes_in;

  // just delegate...
  return inherited::calloc(bytes_in,
                           initialValue_in);
}

void*
RPG_Stream_AllocatorHeap::calloc(size_t numElements_in,
                             size_t sizePerElement_in,
                             char initialValue_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::calloc"));

  // update allocation counter
  myPoolSize += (numElements_in * sizePerElement_in);

  // just delegate...
  return inherited::calloc(numElements_in,
                           sizePerElement_in,
                           initialValue_in);
}

void
RPG_Stream_AllocatorHeap::free(void* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::free"));

  // *TODO*: how can we update our counter ???
//   // update allocation counter
//   myPoolSize -= bytes_in;

  // just delegate...
  return inherited::free(handle_in);
}

size_t
RPG_Stream_AllocatorHeap::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::cache_depth"));

  return cache_size();
}

size_t
RPG_Stream_AllocatorHeap::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::cache_size"));

  // *TODO*: how can we update our counter (see free()) ???
  return myPoolSize.value();
}

void
RPG_Stream_AllocatorHeap::dump(void) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::dump"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("allocated heap space: %u\n"),
             myPoolSize.value()));
}

int
RPG_Stream_AllocatorHeap::remove(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::remove"));

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::bind(const char* name_in,
                           void* pointer_in,
                           int duplicates_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::bind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);
  ACE_UNUSED_ARG(duplicates_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::trybind(const char* name_in,
                              void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::trybind"));

    // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::find(const char* name_in,
                           void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::find"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::find(const char* name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::find"));

  // ignore these
  ACE_UNUSED_ARG(name_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::unbind(const char* name_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::unbind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::unbind(const char* name_in,
                             void*& pointer_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::unbind"));

  // ignore these
  ACE_UNUSED_ARG(name_in);
  ACE_UNUSED_ARG(pointer_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::sync(ssize_t length_in,
                           int flags_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::sync"));

  // ignore these
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(flags_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::sync(void* address_in,
                           size_t length_in,
                           int flags_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::sync"));

  // ignore these
  ACE_UNUSED_ARG(address_in);
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(flags_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::protect(ssize_t length_in,
                              int protection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::protect"));

  // ignore these
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(protection_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}

int
RPG_Stream_AllocatorHeap::protect(void* address_in,
                              size_t length_in,
                              int protection_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_AllocatorHeap::protect"));

  // ignore these
  ACE_UNUSED_ARG(address_in);
  ACE_UNUSED_ARG(length_in);
  ACE_UNUSED_ARG(protection_in);

  ACE_ASSERT(false);

  ACE_NOTSUP_RETURN(-1);
}
